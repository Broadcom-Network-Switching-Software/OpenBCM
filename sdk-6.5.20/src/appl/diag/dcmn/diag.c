/* 
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        diag.c
 * Purpose:     Device diagnostics commands.
 */

#include <shared/bsl.h>
#include <shared/swstate/access/sw_state_access.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/dbx/dbx_file.h>

#include <soc/drv.h>
#include <soc/wb_engine.h>
#include <bcm/debug.h>
#include <bcm/error.h>
#include <bcm/fabric.h>
#include <appl/diag/diag.h>
#include <appl/diag/diag_pp.h>
#include <appl/diag/diag_oam.h>
#include <appl/diag/diag_buffers.h>
#include <appl/diag/diag_mmu.h>
#include <appl/diag/diag_field.h>
#include <appl/diag/diag_alloc.h>
#include <appl/diag/dcmn/fabric.h>
#include <sal/appl/sal.h>
#include <appl/diag/dcmn/diag.h>
#include <appl/diag/system.h>
#ifdef INCLUDE_IPSEC
#include <appl/diag/diag_ipsec.h>
#endif

#ifdef BCM_DFE_SUPPORT
#include <appl/diag/dfe/fabric.h>

#include <bcm_int/dfe/fabric.h>

#include <soc/dfe/cmn/dfe_drv.h>
#include <soc/dfe/cmn/dfe_fabric.h>
#include <soc/dfe/cmn/dfe_diag.h>
#include <soc/dfe/cmn/mbcm.h>
#endif

#include <appl/diag/dcmn/diag_signals_dump.h>

#ifdef BCM_PETRA_SUPPORT
#include <soc/dpp/TMC/tmc_api_general.h>
#include <bcm_int/petra_dispatch.h>
#include <soc/dpp/dpp_defs.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <soc/dpp/ARAD/arad_egr_queuing.h>
#include <soc/dpp/QAX/qax_egr_queuing.h>
#include <soc/dpp/QAX/qax_multicast_imp.h>
#include <soc/dpp/ARAD/arad_api_diagnostics.h>
#include <soc/dpp/ARAD/arad_debug.h>
#include <soc/dpp/ARAD/arad_fabric.h>
#include <soc/dpp/ARAD/arad_nif.h>
#include <soc/dpp/ARAD/arad_sw_db.h>
#include <soc/dpp/ARAD/arad_ports.h>
#include <soc/dpp/ARAD/arad_egr_prge_mgmt.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_diag.h>
#include <soc/dpp/port_sw_db.h>
#include <soc/dpp/multicast_imp.h>
#include <soc/dpp/trunk_sw_db.h>
#include <bcm/types.h>


#include <shared/swstate/access/sw_state_access.h>
#include <soc/dcmn/dcmn_intr_corr_act_func.h>

#ifndef __KERNEL__
#define UINT_MAX 0xffffffff
#endif
#define  ARAD_INGR_QUEUE_TABLE_BYTE_RESOL 16
extern cmd_result_t
   dpp_diag_egq_print(int unit, int last_cell_print, int map_print, int port_print);
extern cmd_result_t
   dpp_diag_e2e_ps_graphic(int unit, int port_num);
extern void
    phy_measure_ch0_calc(soc_reg_above_64_val_t reg_val, int *ch0_out_int, int *ch0_out_remainder);
#endif
#include <soc/defs.h>
#include <soc/field.h>

#ifdef BCM_PETRA_SUPPORT
#include <soc/dpp/cosq.h>
#include <bcm_int/dpp/cosq.h>
#include <bcm_int/dpp/utils.h>
#include <bcm_int/dpp/port.h>
#include <bcm_int/dpp/error.h>
#include <soc/dpp/mbcm.h>
#include <shared/util.h>

#include <soc/dpp/dpp_config_defs.h>
#include <soc/dpp/dpp_config_imp_defs.h>
#include <sal/compiler.h>
#endif
#ifdef DUNE_UI
extern void dune_ui_module_free_bits();
#endif

#ifdef BCM_88750_SUPPORT
extern diag_counter_data_t fe1600_counters_data[];
extern diag_counter_data_t fe1600_packet_flow_counters_data[];
#endif

#ifdef BCM_88950_SUPPORT
extern diag_counter_data_t fe3200_counters_data[];
#endif

#ifdef BCM_PETRA_SUPPORT
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_isem_access.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_flp_init.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_occupation_mgmt.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_diag.h>
#endif

#ifndef BSL_LOG_MODULE
#define BSL_LOG_MODULE BSL_LS_APPLDNX_DIAGDNX
#endif

#ifdef BCM_JERICHO_SUPPORT
#include <soc/dpp/JER/jer_nif.h>
#include <soc/dpp/QAX/qax_nif.h>
#include <soc/portmod/portmod_internal.h>
#include <soc/portmod/portmod_common.h>
#include <bcm/port.h>
extern diag_counter_data_t jericho_counters_data[];
extern diag_counter_data_t jericho_plus_counters_data[];
extern diag_counter_data_t jericho_counters_for_graphic_display[];
extern diag_counter_data_t jericho_rqp_counters_for_graphic_display[];
extern diag_counter_data_t qax_counters_data[];
extern diag_counter_data_t qax_counters_for_graphic_display[];
extern diag_counter_data_t qax_rqp_counters_for_graphic_display[];
extern diag_counter_data_t qux_counters_data[];
extern diag_counter_data_t qux_counters_for_graphic_display[];
extern diag_counter_data_t qux_rqp_counters_for_graphic_display[];

extern cmd_result_t jer_phy_measure_nif_pll(int unit, bcm_port_t port, int *ch0_out_int, int *ch0_out_remainder);
soc_reg_t jer_register_per_lane_num[] = {
    CLPORT_XGXS0_LN0_STATUS0_REGr,
    CLPORT_XGXS0_LN1_STATUS0_REGr,
    CLPORT_XGXS0_LN2_STATUS0_REGr,
    CLPORT_XGXS0_LN3_STATUS0_REGr,

    XLPORT_XGXS0_LN0_STATUS0_REGr,
    XLPORT_XGXS0_LN1_STATUS0_REGr,
    XLPORT_XGXS0_LN2_STATUS0_REGr,
    XLPORT_XGXS0_LN3_STATUS0_REGr
};
#define NOF_LANES_IN_PM      4
#define MAX_NOF_PMS_IN_ILKN  6

#endif

/* Defines for diag counters graphical */
#define LINE_WIDTH 176 /* must divide by 8 */
#define MAX_NUM_LEN 64

#include <appl/diag/dcmn/rate_calc.h>
#ifdef BCM_PETRA_SUPPORT
extern diag_counter_data_t arad_counters_data[];
extern diag_counter_data_t ardon_counters_data[];
extern diag_counter_data_t arad_packet_flow_counters_data[];
extern diag_counter_data_t ardon_packet_flow_counters_data[];
extern diag_counter_data_t arad_diag_fabric_gci_backoff[];
extern diag_counter_data_t arad_diag_fabric_gci[];
/*import mesure function from phy cmds shell*/
extern int _phy_arad_nif_measure(int unit, bcm_port_t port, uint32 *type_of_bit_clk, int *one_clk_time_measured_int, int *one_clk_time_measured_remainder, int *serdes_freq_int, int *serdes_freq_remainder, uint32 *lane);
static char  core_str[20];
char *get_core_str(int unit, int core) {

#ifdef BCM_JERICHO_SUPPORT
     if (SOC_IS_JERICHO(unit)) {
         sal_sprintf(core_str, "Core %u:", core);
     }
     else 
#endif /*BCM_JERICHO_SUPPORT*/
     {
         return (char*)"";
     }
     return core_str;
}


soc_reg_t register_per_lane_num[] = { /*0*/ PORT_XGXS_0_LN_0_STATUS_0_REGr,
   /*1*/ PORT_XGXS_0_LN_1_STATUS_0_REGr,
   /*2*/ PORT_XGXS_0_LN_2_STATUS_0_REGr,
   /*3*/ PORT_XGXS_0_LN_3_STATUS_0_REGr,
   /*4*/ PORT_XGXS_1_LN_0_STATUS_0_REGr,
   /*5*/ PORT_XGXS_1_LN_1_STATUS_0_REGr,
   /*6*/ PORT_XGXS_1_LN_2_STATUS_0_REGr,
   /*7*/ PORT_XGXS_1_LN_3_STATUS_0_REGr,
   /*8*/ PORT_XGXS_2_LN_0_STATUS_0_REGr,
   /*9*/ PORT_XGXS_2_LN_1_STATUS_0_REGr,
   /*10*/ PORT_XGXS_2_LN_2_STATUS_0_REGr,
   /*11*/ PORT_XGXS_2_LN_3_STATUS_0_REGr,

   /*12*/ PORT_XGXS_0_LN_0_STATUS_0_REGr,
   /*13*/ PORT_XGXS_0_LN_1_STATUS_0_REGr,
   /*14*/ PORT_XGXS_0_LN_2_STATUS_0_REGr,
   /*15*/ PORT_XGXS_0_LN_3_STATUS_0_REGr,

   /*16*/ PORT_XGXS_0_LN_0_STATUS_0_REGr,
   /*17*/ PORT_XGXS_0_LN_1_STATUS_0_REGr,
   /*18*/ PORT_XGXS_0_LN_2_STATUS_0_REGr,
   /*19*/ PORT_XGXS_0_LN_3_STATUS_0_REGr,
   /*20*/ PORT_XGXS_1_LN_0_STATUS_0_REGr,
   /*21*/ PORT_XGXS_1_LN_1_STATUS_0_REGr,
   /*22*/ PORT_XGXS_1_LN_2_STATUS_0_REGr,
   /*23*/ PORT_XGXS_1_LN_3_STATUS_0_REGr,
   /*24*/ PORT_XGXS_2_LN_0_STATUS_0_REGr,
   /*25*/ PORT_XGXS_2_LN_1_STATUS_0_REGr,
   /*26*/ PORT_XGXS_2_LN_2_STATUS_0_REGr,
   /*27*/ PORT_XGXS_2_LN_3_STATUS_0_REGr,

   /*28*/ PORT_XGXS_0_LN_0_STATUS_0_REGr,
   /*29*/ PORT_XGXS_0_LN_1_STATUS_0_REGr,
   /*30*/ PORT_XGXS_0_LN_2_STATUS_0_REGr,
   /*31*/ PORT_XGXS_0_LN_3_STATUS_0_REGr };
#endif

STATIC diag_counter_data_t* diag_counters_get(int unit) {
#ifdef BCM_88750_SUPPORT
   if (SOC_IS_FE1600(unit)) {
      return fe1600_counters_data;
   }
#endif
#ifdef BCM_88950_SUPPORT
   if (SOC_IS_FE3200(unit)) {
       return fe3200_counters_data;
   }
#endif
#ifdef BCM_QUX_SUPPORT
   if (SOC_IS_QUX(unit)) {
       return qux_counters_data;
   }
#endif

#ifdef BCM_88470_A0
   if (SOC_IS_QAX(unit)) {
       return qax_counters_data;
   }
#endif

#ifdef BCM_JERICHO_SUPPORT
   if ((!SOC_IS_JERICHO_PLUS(unit))&&SOC_IS_JERICHO(unit)) {
       return jericho_counters_data;
   } else if (SOC_IS_JERICHO_PLUS(unit)) {
      return jericho_plus_counters_data;
   }
#endif
#ifdef BCM_PETRA_SUPPORT
   if (SOC_IS_ARDON(unit)) {
      return ardon_counters_data;
   } else if (SOC_IS_ARAD(unit)) {
      return arad_counters_data;
   }
#endif
   return NULL;
}

#ifdef BCM_PETRA_SUPPORT
STATIC cmd_result_t
dpp_diag_cosq_flow_and_up_print(int unit,
                                       int core,
                                       uint32 is_flow,
                                       uint32 dest_id,
                                       uint32 print_status,
                                       uint32 short_format);
STATIC cmd_result_t
dpp_diag_cosq_print_modport_to_sysport_to_voq_mapping(int unit, int by_sys, int by_mod, int by_voq);
#endif


STATIC uint32 get_field(uint32 in, int first, int last) {
   /*DESCRIPTION# Declaration    : uint32 get_field(uint32 in, int first, int last);
     DESCRIPTION# Input          : in - input value; first - LSB bit of in; last - MSB field of in;
     DESCRIPTION# NOTE           : The field MUST be no longer than 32 bit.
     DESCRIPTION# Output         : NONE.
     DESCRIPTION# Return         : field value (uint32).*/
   if (last != 31) in &= (0x7FFFFFFF >> (30 - last));
   in = in >> first;
   in &= ~(0xFFFFFFFF << (last - first + 1));
   return (in);
}

STATIC uint32 common_read_reg_long(int unit_, char *reg_name, uint32 *val, int nof_words) {
   soc_reg_above_64_val_t r_;
   int i;
   if (diag_reg_get(unit_, reg_name, r_) != BCM_E_NONE) return (0);
   for (i = nof_words - 1; i >= 0; i--) val[i] = r_[i];
   return (1);
}


STATIC int diag_discard_interrupt_to_string(int unit, 
                                            soc_reg_t reg, 
                                            uint64 reg_val, 
                                            int *field_indx, 
                                            int line_len, 
                                            char *str_buf){

    soc_reg_info_t      *reginfo;
    soc_field_info_t    *fld;
    int                 space_left = line_len;
    uint32              field_val;
    uint8               num_of_rejects = 0, add_prefix = 0;
    uint8               reject_or_discard  = SOC_IS_QUX(unit)?(reg == EGQ_ERPP_DISCARD_INT_REGr):(reg == EGQ_ERPP_DISCARD_INTERRUPT_REGISTERr);
    char                *space_prefix = SOC_IS_ARADPLUS_AND_BELOW(unit) ? "     " : "   ";
    char                *print_prefix = (reject_or_discard == 1) ? "Discards" : "Rejects";
    char                *field_suffix = (reject_or_discard == 1) ? "_INT"   : "_RJCT";
    

    reginfo = &SOC_REG_INFO(unit, reg);

    /* 
     *  The "Discards:"/"Rejects" prefix is only necessary at the first line.
     */
    if (*field_indx == 0) {
        add_prefix = 1;
        sal_sprintf(str_buf, "%s%s:", space_prefix, print_prefix);
        space_left -= sal_strlen(str_buf);
    }

    for (; *field_indx < reginfo->nFields; (*field_indx)++) {
      fld = &reginfo->fields[*field_indx];
      field_val = soc_reg64_field32_get(unit, reg, reg_val, fld->field);
      if (0 != field_val) {
            char *field_name = SOC_FIELD_NAME(unit, fld->field);
            int field_print_len = sal_strlen(field_name) - sal_strlen(field_suffix);

            if(!sal_strcasestr(field_name, field_suffix)){
                /* On Jericho and QAX, some of the fields don't include the suffix*/
                field_print_len = sal_strlen(field_name);
            }
         
            if (!SOC_IS_JERICHO(unit) && sal_strcasecmp(field_name + field_print_len, field_suffix) != 0) { /* On Arad devices we need to filter some of the reasons*/
                continue;
            } else {
                char field_print_name[40] = "";
                num_of_rejects++;
                sal_strncpy(field_print_name, field_name, field_print_len);
                if (space_left > field_print_len) {
                    /* There's still room in the line to add this field. */
                    sal_sprintf(str_buf, "%s %s", str_buf, field_print_name);
                    space_left = space_left - field_print_len - 1;
                } else if (space_left < field_print_len) {
                    /* No more room in the line. Return string to be printed. */
                    break;
                }
            }
        }
    }

    /* If there was nothing to write, override the prefix.*/
    if (add_prefix && !num_of_rejects) {
        str_buf[0] = 0;
    }

    return num_of_rejects;
}

STATIC int diag_print_iqm_rejects_and_erpp_discards(int unit, uint8 core_disp) {
    int      erpp_current_field[MAX_NUM_OF_CORES], iqm_current_field[MAX_NUM_OF_CORES], cgm_current_field = 0;
    uint32   erpp_reg_val[MAX_NUM_OF_CORES];
    uint64   iqm_reg_val[MAX_NUM_OF_CORES], erpp_reg_val64[MAX_NUM_OF_CORES], cgm_reg_val;
    char     iqm_str_buf[MAX_NUM_OF_CORES][LINE_WIDTH / 2], erpp_str_buf[MAX_NUM_OF_CORES][LINE_WIDTH / 2], cgm_str_buf[LINE_WIDTH / 2];
    int      core = 0;
    uint8    num_of_rejects;
    int      line_len;

    /* Reset the array that represents our current iteration. */
    sal_memset(erpp_current_field, 0, sizeof(erpp_current_field));
    sal_memset(iqm_current_field,  0, sizeof(iqm_current_field));

    /* Line len depends on device, and on core display. */
    line_len = (SOC_IS_JERICHO(unit) && core_disp)   ? ((LINE_WIDTH / 4) - 1)
                           : (SOC_IS_JERICHO(unit))  ? ((LINE_WIDTH / 2) - 1)
                           : 57 /* Hard coded value, legacy for Arad. */ ;

    /* First, get the registers value from all the cores. */
#ifdef BCM_PETRA_SUPPORT
    SOC_DPP_CORES_ITER(SOC_CORE_ALL, core)
#endif
    {
        if (SOC_IS_QAX(unit)){
            if (BCM_E_NONE != READ_CGM_REJECT_STATUS_BITMAPr(unit, SOC_CORE_ALL, &cgm_reg_val)) {
               return CMD_FAIL;
            }
        }
        else{
            if (BCM_E_NONE != READ_IQM_REJECT_STATUS_BMPr(unit, core, &iqm_reg_val[core])) {
               return CMD_FAIL;
            }
            if (!core_disp) {
                /* If we don't want to display counters per core, merge all the bitmaps into the first core. */
               COMPILER_64_OR(iqm_reg_val[0], iqm_reg_val[core]);
            }
        }

        if (SOC_IS_QUX(unit)) {
            if (BCM_E_NONE != READ_EGQ_ERPP_DISCARD_INT_REGr(unit, &erpp_reg_val[core])) {
               return CMD_FAIL;
            }
        } else {
            if (BCM_E_NONE != READ_EGQ_ERPP_DISCARD_INTERRUPT_REGISTERr(unit, core, &erpp_reg_val[core])) {
               return CMD_FAIL;
            }
        }

        COMPILER_64_SET(erpp_reg_val64[core], 0, erpp_reg_val[core]);
        if (!core_disp) {
            /* If we don't want to display counters per core, merge all the bitmaps into the first core. */
            COMPILER_64_OR(erpp_reg_val64[0], erpp_reg_val64[core]);
        }

        /* reset EGQ_ERPP_DISCARD_INTERRUPT_REGISTER by writing 1s*/
        if (SOC_IS_QUX(unit)) {
            if (BCM_E_NONE != WRITE_EGQ_ERPP_DISCARD_INT_REGr(unit, erpp_reg_val[core])) {
               return CMD_FAIL;
            }
        } else {
            if (BCM_E_NONE != WRITE_EGQ_ERPP_DISCARD_INTERRUPT_REGISTERr(unit, core, erpp_reg_val[core])) {
               return CMD_FAIL;
            }
        }

    }

    do {
        /* Reset strings and counter. */
        num_of_rejects = 0;
#ifdef BCM_PETRA_SUPPORT
        SOC_DPP_CORES_ITER((core_disp) ? SOC_CORE_ALL : SOC_CORE_DEFAULT, core)
#endif
        {

            iqm_str_buf[core][0] = 0;
            cgm_str_buf[0] = 0;

            if (SOC_IS_QAX(unit)){
                num_of_rejects += diag_discard_interrupt_to_string(unit,
                                                              CGM_REJECT_STATUS_BITMAPr,
                                                              cgm_reg_val,
                                                              &cgm_current_field,
                                                              line_len,
                                                              cgm_str_buf);
            } else {
                num_of_rejects += diag_discard_interrupt_to_string(unit, 
                                                              IQM_REJECT_STATUS_BMPr, 
                                                              iqm_reg_val[core], 
                                                              &iqm_current_field[core], 
                                                              line_len, 
                                                              iqm_str_buf[core]);
            }
            erpp_str_buf[core][0] = 0;
            num_of_rejects += diag_discard_interrupt_to_string(unit, 
                                                              SOC_IS_QUX(unit)?EGQ_ERPP_DISCARD_INT_REGr:EGQ_ERPP_DISCARD_INTERRUPT_REGISTERr,
                                                              erpp_reg_val64[core], 
                                                              &erpp_current_field[core], 
                                                              line_len, 
                                                              erpp_str_buf[core]);
        }

        if (num_of_rejects != 0) {
            if (core_disp) {
                /* We assume Jericho size here for the spacing. */
                cli_out("|%-41s  |%-41s  |%-41s  |%-41s  |\n", iqm_str_buf[0], iqm_str_buf[1], erpp_str_buf[0], erpp_str_buf[1]);
            } else {
                if(SOC_IS_QAX(unit)){
                    cli_out("|%-85s  |%-85s  |\n", cgm_str_buf, erpp_str_buf[0] );
                }
                else if (SOC_IS_JERICHO(unit)) {
                    cli_out("|%-85s  |%-85s  |\n", iqm_str_buf[0], erpp_str_buf[0]);
                } else if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
                    cli_out("|%-59s  |%-59s  |\n", iqm_str_buf[0], erpp_str_buf[0]);
                }
            }
        }

    } while (num_of_rejects); 

    return CMD_OK;
}




STATIC int diag_print_pqp_discard_reasons(int unit){
    uint32  counter_data[2];
    uint32  reasons=0;
    uint32  mask=1;
    uint32  i;
    
    uint32  space_left=55;
    char    reject_names[56];
    char    curr_reason[55];
    uint32  first=1;


    
    
    common_read_reg_long(unit,"CGM_PQP_DISCARD_REASONS",counter_data,2);
    reasons=counter_data[0];/*bitmap of reasons*/
    
    if (reasons)
    {
        
        sal_sprintf(reject_names,"Discard Reasons: ");
        space_left-=sal_strlen("Discard Reasons: ");

        for (i=0;i<=15;i++) /* go over the 16 optional reasons*/
        {

            if (reasons&mask)
            {
                switch (i)
                {
                case 0:
                    sal_strncpy(curr_reason, "Total PDs violated", sizeof(curr_reason));
                    break;
                case 1:
                    sal_strncpy(curr_reason, "Total PDs UC pool size violated", sizeof(curr_reason));
                    break;
                case 2:
                    sal_strncpy(curr_reason, "Per port UC PDs", sizeof(curr_reason));
                    break;
                case 3:
                    sal_strncpy(curr_reason, "Per Q UC PDs", sizeof(curr_reason));
                    break;
                case 4:
                    sal_strncpy(curr_reason, "Per port UC DBs", sizeof(curr_reason));
                    break;
                case 5:
                    sal_strncpy(curr_reason, "Per Q UC DBs", sizeof(curr_reason));
                    break;
                case 8:
                    sal_strncpy(curr_reason, "Total PDs MC pool size", sizeof(curr_reason));
                    break;
                case 9:
                    sal_strncpy(curr_reason, "Per interface PDs", sizeof(curr_reason));
                    break;
                case 10:
                    sal_strncpy(curr_reason, "MC SP", sizeof(curr_reason));
                    break;
                case 11:
                    sal_strncpy(curr_reason, "Per MC-TC", sizeof(curr_reason));
                    break;
                case 12:
                    sal_strncpy(curr_reason, "MC PDs per port", sizeof(curr_reason));
                    break;
                case 13:
                    sal_strncpy(curr_reason, "MC PDs per Q", sizeof(curr_reason));
                    break;
                case 14:
                    sal_strncpy(curr_reason, "MC per port size(bytes)", sizeof(curr_reason));
                    break;
                case 15:
                    sal_strncpy(curr_reason, "MC per Q size(bytes)", sizeof(curr_reason));
                    break;
                }

                /*printing phase*/
                if (space_left>=sal_strlen(curr_reason)+1) /* enough space to put current reason */
                {
                    if(first){
                         sal_sprintf(reject_names,"%s%s",reject_names,curr_reason); /*dont put ","*/
                         first=0;
                    } else {
                        sal_sprintf(reject_names,"%s,%s",reject_names,curr_reason);
                    }
                    space_left-=sal_strlen(curr_reason)+1;
                    
                }
                else{
                    sal_sprintf(reject_names,"%s%s",reject_names,",");
                    cli_out("|                                                             |   %-57s |\n",reject_names);
                    sal_sprintf(reject_names,curr_reason);
                    space_left=55-sal_strlen(curr_reason);
                    
                }
                
            } /* end of "if current bit is 1"*/
            mask<<=1;
        }
        cli_out("|                                                             |   %-57s |\n",reject_names);
        
    }
    return CMD_OK;


}


STATIC int diag_print_rqp_discard_reasons(int unit)
{
    uint32  counter_data[2];
    uint32  reasons=0;
    uint32  mask=1;
    uint32  i;
    
    uint32  space_left=55;
    char    reject_names[56];
    char    curr_reason[55];
    uint32  first=1;

    common_read_reg_long(unit,"CGM_RQP_DISCARD_REASONS",counter_data,2);
    reasons=counter_data[0]; /* bitmap of reasons*/
    
    if (reasons)
    {
        
        sal_sprintf(reject_names,"Discard Reasons: ");
        space_left-=sal_strlen("Discard Reasons: ");

        for (i=0;i<=9;i++) /* go over the 10 optional reasons*/
        {

            if (reasons&mask)
            {
                switch (i)
                {

                case 0:
                    sal_strncpy(curr_reason, "Total DBs violated", sizeof(curr_reason));
                    break;
                case 1:
                    sal_strncpy(curr_reason, "Total UC DBs pool size violated", sizeof(curr_reason));
                    break;
                case 2:
                    sal_strncpy(curr_reason, "MC HP packet discarded in EHP because MC FIFO is full", sizeof(curr_reason));
                    break;
                case 3:
                    sal_strncpy(curr_reason, "MC LP packet discarded in EHP because MC FIFO is full", sizeof(curr_reason));
                    break;
                case 4:
                    sal_strncpy(curr_reason, "Total MC DBs pool size violated", sizeof(curr_reason));
                    break;
                case 5:
                    sal_strncpy(curr_reason, "Packet-DP can't take from shared DBs resources", sizeof(curr_reason));
                    break;
                case 6:
                    sal_strncpy(curr_reason, "SP DBs threshold violated", sizeof(curr_reason));
                    break;
                case 7:
                    sal_strncpy(curr_reason, "Discrete-Partioning method:MC-TC DBs violated", sizeof(curr_reason));
                    break;
                case 8:
                    sal_strncpy(curr_reason, "Strict-priority method:MC-TC mapped to SP0 is violated", sizeof(curr_reason));
                    break;
                case 9:
                    sal_strncpy(curr_reason, "Strict-priority method:MC-TC mapped to SP1 is violated", sizeof(curr_reason));
                    break;
               
                }

                /*printing phase*/
                if (space_left>=sal_strlen(curr_reason)+1) /* enough space to put current reason */
                {
                    if(first){
                         sal_sprintf(reject_names,"%s%s",reject_names,curr_reason); /*dont put ","*/
                         first=0;
                    } else {
                        sal_sprintf(reject_names,"%s,%s",reject_names,curr_reason);
                    }
                    space_left-=sal_strlen(curr_reason)+1;
                    
                }
                else{
                    sal_sprintf(reject_names,"%s%s",reject_names,",");
                    cli_out("|                                                             |   %-57s |\n",reject_names);
                    sal_sprintf(reject_names,curr_reason);
                    space_left=55-sal_strlen(curr_reason);
                    
                }
                
            } /* end of "if current bit is 1"*/
            mask<<=1;
        }
        cli_out("|                                                             |   %-57s |\n",reject_names);
        
    }
    return CMD_OK;


}

#ifdef BCM_PETRA_SUPPORT
STATIC void
dpp_diag_seq_done_buffer_get(uint32 seq_done, char *seq_done_buffer) {
    if (seq_done == -1)
    {
        sal_sprintf(seq_done_buffer, "%s%5s", "N/A", "");
    }
    else
    {
        sal_sprintf(seq_done_buffer, "0x%-6x", seq_done);
    }
}

#endif /* BCM_PETRA_SUPPORT */
#ifdef BCM_JERICHO_SUPPORT

/* print_counter_val_to_string
 * 
 * generate a string with counter value, according to the given attributes. 
 * resulting string is given as res_buff 
 */
STATIC void
print_counter_val_to_string(int unit, uint64 counter_val, uint64 counter_ovf, uint32 print_none_zero,
                            uint32 hex_base, char* res_buff) {
    char ovf[] = "(ovf)";
    char empty[] = "";
    char *is_ovf;
    char buffer[32];
    int should_print = 1;
    int  comma = soc_property_get(unit, spn_DIAG_COMMA, ',');

    is_ovf = COMPILER_64_IS_ZERO(counter_ovf) ? empty: ovf;
    if ((print_none_zero == 1) && (COMPILER_64_IS_ZERO(counter_val))) {
        res_buff[0] = should_print = 0;
    } else if (hex_base == 1){
        _shr_format_uint64_hexa_string(counter_val, buffer);
    } else {
        format_uint64_decimal(buffer, counter_val, comma);
    }
    if (should_print){
        sal_snprintf(res_buff, 32, "%s %s", buffer, is_ovf);
    }
}

STATIC void
compute_sum_fda_drop_count(int unit, uint64* counters_data, uint32 print_none_zero,
                           uint32 hex_base, char* res_buff){
    uint64 cnt_sum = COMPILER_64_INIT(0,0);
    uint64 cnt_val1, cnt_val2;
    uint32 ovf = 0;
    uint64 ovf_64;

    cnt_val1 = soc_reg64_field_get(unit, FDA_EGQ_FABFIF_0_PRIO_0_DROP_CNTr, counters_data[FDA_EGQ_N_FABFIF_0_PRIO_0_DROP_CNT0], 
                                   EGQ_N_FABFIF_0_PRIO_0_DROP_CNTf);
    cnt_val2 = soc_reg64_field_get(unit, FDA_EGQ_FABFIF_0_PRIO_0_DROP_CNTr, counters_data[FDA_EGQ_N_FABFIF_0_PRIO_0_DROP_CNT1], 
                                   EGQ_N_FABFIF_0_PRIO_0_DROP_CNTf);
    COMPILER_64_ADD_64(cnt_sum, cnt_val1);
    COMPILER_64_ADD_64(cnt_sum, cnt_val2);
    ovf |= soc_reg64_field32_get(unit, FDA_EGQ_FABFIF_0_PRIO_0_DROP_CNTr, counters_data[FDA_EGQ_N_FABFIF_0_PRIO_0_DROP_CNT0], 
                                 EGQ_N_FABFIF_0_PRIO_0_DROP_CNT_OVERFLOWf);
    ovf |= soc_reg64_field32_get(unit, FDA_EGQ_FABFIF_0_PRIO_0_DROP_CNTr, counters_data[FDA_EGQ_N_FABFIF_0_PRIO_0_DROP_CNT1], 
                                 EGQ_N_FABFIF_0_PRIO_0_DROP_CNT_OVERFLOWf);

    cnt_val1 = soc_reg64_field_get(unit, FDA_EGQ_FABFIF_0_PRIO_1_DROP_CNTr, counters_data[FDA_EGQ_N_FABFIF_0_PRIO_1_DROP_CNT0], 
                                   EGQ_N_FABFIF_0_PRIO_1_DROP_CNTf);
    cnt_val2 = soc_reg64_field_get(unit, FDA_EGQ_FABFIF_0_PRIO_1_DROP_CNTr, counters_data[FDA_EGQ_N_FABFIF_0_PRIO_1_DROP_CNT1], 
                                   EGQ_N_FABFIF_0_PRIO_1_DROP_CNTf);
    COMPILER_64_ADD_64(cnt_sum, cnt_val1);
    COMPILER_64_ADD_64(cnt_sum, cnt_val2);
    ovf |= soc_reg64_field32_get(unit, FDA_EGQ_FABFIF_0_PRIO_1_DROP_CNTr, counters_data[FDA_EGQ_N_FABFIF_0_PRIO_1_DROP_CNT0], 
                                 EGQ_N_FABFIF_0_PRIO_1_DROP_CNT_OVERFLOWf);
    ovf |= soc_reg64_field32_get(unit, FDA_EGQ_FABFIF_0_PRIO_1_DROP_CNTr, counters_data[FDA_EGQ_N_FABFIF_0_PRIO_1_DROP_CNT1], 
                                 EGQ_N_FABFIF_0_PRIO_1_DROP_CNT_OVERFLOWf);

    cnt_val1 = soc_reg64_field_get(unit, FDA_EGQ_FABFIF_0_PRIO_2_DROP_CNTr, counters_data[FDA_EGQ_N_FABFIF_0_PRIO_2_DROP_CNT0], 
                                  EGQ_N_FABFIF_0_PRIO_2_DROP_CNTf);
    cnt_val2 = soc_reg64_field_get(unit, FDA_EGQ_FABFIF_0_PRIO_2_DROP_CNTr, counters_data[FDA_EGQ_N_FABFIF_0_PRIO_2_DROP_CNT1], 
                                  EGQ_N_FABFIF_0_PRIO_2_DROP_CNTf);
    COMPILER_64_ADD_64(cnt_sum, cnt_val1);
    COMPILER_64_ADD_64(cnt_sum, cnt_val2);    
    ovf |= soc_reg64_field32_get(unit, FDA_EGQ_FABFIF_0_PRIO_2_DROP_CNTr, counters_data[FDA_EGQ_N_FABFIF_0_PRIO_2_DROP_CNT0], 
                                 EGQ_N_FABFIF_0_PRIO_2_DROP_CNT_OVERFLOWf);
    ovf |= soc_reg64_field32_get(unit, FDA_EGQ_FABFIF_0_PRIO_2_DROP_CNTr, counters_data[FDA_EGQ_N_FABFIF_0_PRIO_2_DROP_CNT1], 
                                 EGQ_N_FABFIF_0_PRIO_2_DROP_CNT_OVERFLOWf);

    cnt_val1 = soc_reg64_field_get(unit, FDA_EGQ_FABFIF_1_PRIO_0_DROP_CNTr, counters_data[FDA_EGQ_N_FABFIF_1_PRIO_0_DROP_CNT0], 
                                   EGQ_N_FABFIF_1_PRIO_0_DROP_CNTf);
    cnt_val2 = soc_reg64_field_get(unit, FDA_EGQ_FABFIF_1_PRIO_0_DROP_CNTr, counters_data[FDA_EGQ_N_FABFIF_1_PRIO_0_DROP_CNT1], 
                                   EGQ_N_FABFIF_1_PRIO_0_DROP_CNTf);
    COMPILER_64_ADD_64(cnt_sum, cnt_val1);
    COMPILER_64_ADD_64(cnt_sum, cnt_val2);        
    ovf |= soc_reg64_field32_get(unit, FDA_EGQ_FABFIF_1_PRIO_0_DROP_CNTr, counters_data[FDA_EGQ_N_FABFIF_1_PRIO_0_DROP_CNT0], 
                                 EGQ_N_FABFIF_1_PRIO_0_DROP_CNT_OVERFLOWf);
    ovf |= soc_reg64_field32_get(unit, FDA_EGQ_FABFIF_1_PRIO_0_DROP_CNTr, counters_data[FDA_EGQ_N_FABFIF_1_PRIO_0_DROP_CNT1], 
                                 EGQ_N_FABFIF_1_PRIO_0_DROP_CNT_OVERFLOWf);

    cnt_val1 = soc_reg64_field_get(unit, FDA_EGQ_FABFIF_1_PRIO_1_DROP_CNTr, counters_data[FDA_EGQ_N_FABFIF_1_PRIO_1_DROP_CNT0], 
                                   EGQ_N_FABFIF_1_PRIO_1_DROP_CNTf);
    cnt_val2 = soc_reg64_field_get(unit, FDA_EGQ_FABFIF_1_PRIO_1_DROP_CNTr, counters_data[FDA_EGQ_N_FABFIF_1_PRIO_1_DROP_CNT1], 
                                   EGQ_N_FABFIF_1_PRIO_1_DROP_CNTf);
    COMPILER_64_ADD_64(cnt_sum, cnt_val1);
    COMPILER_64_ADD_64(cnt_sum, cnt_val2);      
    ovf |= soc_reg64_field32_get(unit, FDA_EGQ_FABFIF_1_PRIO_1_DROP_CNTr, counters_data[FDA_EGQ_N_FABFIF_1_PRIO_1_DROP_CNT0], 
                                 EGQ_N_FABFIF_1_PRIO_1_DROP_CNT_OVERFLOWf);
    ovf |= soc_reg64_field32_get(unit, FDA_EGQ_FABFIF_1_PRIO_1_DROP_CNTr, counters_data[FDA_EGQ_N_FABFIF_1_PRIO_1_DROP_CNT1], 
                                 EGQ_N_FABFIF_1_PRIO_1_DROP_CNT_OVERFLOWf);

    cnt_val1 = soc_reg64_field_get(unit, FDA_EGQ_FABFIF_1_PRIO_2_DROP_CNTr, counters_data[FDA_EGQ_N_FABFIF_1_PRIO_2_DROP_CNT0], 
                                  EGQ_N_FABFIF_1_PRIO_2_DROP_CNTf);
    cnt_val2 = soc_reg64_field_get(unit, FDA_EGQ_FABFIF_1_PRIO_2_DROP_CNTr, counters_data[FDA_EGQ_N_FABFIF_1_PRIO_2_DROP_CNT1], 
                                  EGQ_N_FABFIF_1_PRIO_2_DROP_CNTf);
    COMPILER_64_ADD_64(cnt_sum, cnt_val1);
    COMPILER_64_ADD_64(cnt_sum, cnt_val2);      
    ovf |= soc_reg64_field32_get(unit, FDA_EGQ_FABFIF_1_PRIO_2_DROP_CNTr, counters_data[FDA_EGQ_N_FABFIF_1_PRIO_2_DROP_CNT0], 
                                 EGQ_N_FABFIF_1_PRIO_2_DROP_CNT_OVERFLOWf);
    ovf |= soc_reg64_field32_get(unit, FDA_EGQ_FABFIF_1_PRIO_2_DROP_CNTr, counters_data[FDA_EGQ_N_FABFIF_1_PRIO_2_DROP_CNT1], 
                                 EGQ_N_FABFIF_1_PRIO_2_DROP_CNT_OVERFLOWf);

    cnt_val1 = soc_reg64_field_get(unit, FDA_EGQ_FABFIF_2_PRIO_0_DROP_CNTr, counters_data[FDA_EGQ_N_FABFIF_2_PRIO_0_DROP_CNT0], 
                                   EGQ_N_FABFIF_2_PRIO_0_DROP_CNTf);
    cnt_val2 = soc_reg64_field_get(unit, FDA_EGQ_FABFIF_2_PRIO_0_DROP_CNTr, counters_data[FDA_EGQ_N_FABFIF_2_PRIO_0_DROP_CNT1], 
                                   EGQ_N_FABFIF_2_PRIO_0_DROP_CNTf);
    COMPILER_64_ADD_64(cnt_sum, cnt_val1);
    COMPILER_64_ADD_64(cnt_sum, cnt_val2);     
    ovf |= soc_reg64_field32_get(unit, FDA_EGQ_FABFIF_2_PRIO_0_DROP_CNTr, counters_data[FDA_EGQ_N_FABFIF_2_PRIO_0_DROP_CNT0], 
                                 EGQ_N_FABFIF_2_PRIO_0_DROP_CNT_OVERFLOWf);
    ovf |= soc_reg64_field32_get(unit, FDA_EGQ_FABFIF_2_PRIO_0_DROP_CNTr, counters_data[FDA_EGQ_N_FABFIF_2_PRIO_0_DROP_CNT1], 
                                 EGQ_N_FABFIF_2_PRIO_0_DROP_CNT_OVERFLOWf);

    cnt_val1 = soc_reg64_field_get(unit, FDA_EGQ_FABFIF_2_PRIO_1_DROP_CNTr, counters_data[FDA_EGQ_N_FABFIF_2_PRIO_1_DROP_CNT0], 
                                   EGQ_N_FABFIF_2_PRIO_1_DROP_CNTf);
    cnt_val2 = soc_reg64_field_get(unit, FDA_EGQ_FABFIF_2_PRIO_1_DROP_CNTr, counters_data[FDA_EGQ_N_FABFIF_2_PRIO_1_DROP_CNT1], 
                                   EGQ_N_FABFIF_2_PRIO_1_DROP_CNTf);
    COMPILER_64_ADD_64(cnt_sum, cnt_val1);
    COMPILER_64_ADD_64(cnt_sum, cnt_val2);      
    ovf |= soc_reg64_field32_get(unit, FDA_EGQ_FABFIF_2_PRIO_1_DROP_CNTr, counters_data[FDA_EGQ_N_FABFIF_2_PRIO_1_DROP_CNT0], 
                                 EGQ_N_FABFIF_2_PRIO_1_DROP_CNT_OVERFLOWf);
    ovf |= soc_reg64_field32_get(unit, FDA_EGQ_FABFIF_2_PRIO_1_DROP_CNTr, counters_data[FDA_EGQ_N_FABFIF_2_PRIO_1_DROP_CNT1], 
                                 EGQ_N_FABFIF_2_PRIO_1_DROP_CNT_OVERFLOWf);

    cnt_val1 = soc_reg64_field_get(unit, FDA_EGQ_FABFIF_2_PRIO_2_DROP_CNTr, counters_data[FDA_EGQ_N_FABFIF_2_PRIO_2_DROP_CNT0], 
                                   EGQ_N_FABFIF_2_PRIO_2_DROP_CNTf);
    cnt_val2 = soc_reg64_field_get(unit, FDA_EGQ_FABFIF_2_PRIO_2_DROP_CNTr, counters_data[FDA_EGQ_N_FABFIF_2_PRIO_2_DROP_CNT1], 
                                   EGQ_N_FABFIF_2_PRIO_2_DROP_CNTf);
    COMPILER_64_ADD_64(cnt_sum, cnt_val1);
    COMPILER_64_ADD_64(cnt_sum, cnt_val2);     
    ovf |= soc_reg64_field32_get(unit, FDA_EGQ_FABFIF_2_PRIO_2_DROP_CNTr, counters_data[FDA_EGQ_N_FABFIF_2_PRIO_2_DROP_CNT0], 
                                 EGQ_N_FABFIF_2_PRIO_2_DROP_CNT_OVERFLOWf);
    ovf |= soc_reg64_field32_get(unit, FDA_EGQ_FABFIF_2_PRIO_2_DROP_CNTr, counters_data[FDA_EGQ_N_FABFIF_2_PRIO_2_DROP_CNT1], 
                                 EGQ_N_FABFIF_2_PRIO_2_DROP_CNT_OVERFLOWf);

    COMPILER_64_SET(ovf_64, 0, ovf);
    print_counter_val_to_string(unit, cnt_sum, ovf_64, print_none_zero, hex_base, res_buff);
}


STATIC void
compute_sum_fda_meshmc_drop_count(int unit, uint64* counters_data, uint32 print_none_zero, 
                                  uint32 hex_base, char* res_buff){                                
    uint64 cnt_sum = COMPILER_64_INIT(0,0);
    uint64 cnt_val1, cnt_val2;
    uint32 ovf = 0;
    uint64 ovf_64;
   
    cnt_val1 = soc_reg64_field_get(unit, FDA_EGQ_MESHMC_PRIO_0_DROP_CNTr, counters_data[FDA_EGQ_N_MESHMC_PRIO_0_DROP_CNT0], 
                                   EGQ_N_MESHMC_PRIO_0_DROP_CNTf);
    cnt_val2 = soc_reg64_field_get(unit, FDA_EGQ_MESHMC_PRIO_0_DROP_CNTr, counters_data[FDA_EGQ_N_MESHMC_PRIO_0_DROP_CNT1], 
                                   EGQ_N_MESHMC_PRIO_0_DROP_CNTf);
    COMPILER_64_ADD_64(cnt_sum, cnt_val1);
    COMPILER_64_ADD_64(cnt_sum, cnt_val2);     
    ovf |= soc_reg64_field32_get(unit, FDA_EGQ_MESHMC_PRIO_0_DROP_CNTr, counters_data[FDA_EGQ_N_MESHMC_PRIO_0_DROP_CNT0], 
                                 EGQ_N_MESHMC_PRIO_0_DROP_CNT_OVERFLOWf);
    ovf |= soc_reg64_field32_get(unit, FDA_EGQ_MESHMC_PRIO_0_DROP_CNTr, counters_data[FDA_EGQ_N_MESHMC_PRIO_0_DROP_CNT1], 
                                 EGQ_N_MESHMC_PRIO_0_DROP_CNT_OVERFLOWf);

    cnt_val1 = soc_reg64_field_get(unit, FDA_EGQ_MESHMC_PRIO_1_DROP_CNTr, counters_data[FDA_EGQ_N_MESHMC_PRIO_1_DROP_CNT0], 
                                  EGQ_N_MESHMC_PRIO_1_DROP_CNTf);;
    cnt_val2 = soc_reg64_field_get(unit, FDA_EGQ_MESHMC_PRIO_1_DROP_CNTr, counters_data[FDA_EGQ_N_MESHMC_PRIO_1_DROP_CNT1], 
                                  EGQ_N_MESHMC_PRIO_1_DROP_CNTf);;
    COMPILER_64_ADD_64(cnt_sum, cnt_val1);
    COMPILER_64_ADD_64(cnt_sum, cnt_val2);     
    ovf |= soc_reg64_field32_get(unit, FDA_EGQ_MESHMC_PRIO_1_DROP_CNTr, counters_data[FDA_EGQ_N_MESHMC_PRIO_1_DROP_CNT0], 
                                 EGQ_N_MESHMC_PRIO_1_DROP_CNT_OVERFLOWf);
    ovf |= soc_reg64_field32_get(unit, FDA_EGQ_MESHMC_PRIO_1_DROP_CNTr, counters_data[FDA_EGQ_N_MESHMC_PRIO_1_DROP_CNT1], 
                                 EGQ_N_MESHMC_PRIO_1_DROP_CNT_OVERFLOWf);

    cnt_val1 = soc_reg64_field_get(unit, FDA_EGQ_MESHMC_PRIO_2_DROP_CNTr, counters_data[FDA_EGQ_N_MESHMC_PRIO_2_DROP_CNT0], 
                                  EGQ_N_MESHMC_PRIO_2_DROP_CNTf);
    cnt_val2 = soc_reg64_field_get(unit, FDA_EGQ_MESHMC_PRIO_2_DROP_CNTr, counters_data[FDA_EGQ_N_MESHMC_PRIO_2_DROP_CNT1], 
                                  EGQ_N_MESHMC_PRIO_2_DROP_CNTf);
    COMPILER_64_ADD_64(cnt_sum, cnt_val1);
    COMPILER_64_ADD_64(cnt_sum, cnt_val2);
    ovf |= soc_reg64_field32_get(unit, FDA_EGQ_MESHMC_PRIO_2_DROP_CNTr, counters_data[FDA_EGQ_N_MESHMC_PRIO_2_DROP_CNT0], 
                                 EGQ_N_MESHMC_PRIO_2_DROP_CNT_OVERFLOWf);
    ovf |= soc_reg64_field32_get(unit, FDA_EGQ_MESHMC_PRIO_2_DROP_CNTr, counters_data[FDA_EGQ_N_MESHMC_PRIO_2_DROP_CNT1], 
                                 EGQ_N_MESHMC_PRIO_2_DROP_CNT_OVERFLOWf);

    COMPILER_64_SET(ovf_64, 0, ovf);
    print_counter_val_to_string(unit, cnt_sum, ovf_64, print_none_zero, hex_base, res_buff);
}


/* compute_counter_val_into_string
 * 
 * get the values of the given field/s from the register value, 
 * and generate a string from the result. 
 *  
 * returned string is in res_buff 
 */
STATIC void
compute_counter_val_into_string(int unit, uint64 reg_val, soc_reg_t reg, soc_field_t field, 
                                soc_field_t ovf_field, uint32 print_none_zero, uint32 hex_base, char* res_buff) {
    uint64 counter_val;
    uint32 counter_ovf = 0;
    uint64 counter_ovf_64;
    
    counter_val = soc_reg64_field_get(unit, reg, reg_val, field); 

    if (ovf_field != INVALIDf) {
        counter_ovf = soc_reg64_field32_get(unit, reg, reg_val, ovf_field); 
    }
    COMPILER_64_SET(counter_ovf_64, 0, counter_ovf);
    print_counter_val_to_string(unit, counter_val, counter_ovf_64, print_none_zero, hex_base, res_buff);
}

STATIC void
compute_counter_val_into_string_2(int unit, uint64 reg_val, soc_reg_t reg, soc_field_t field,
                                int bitnum, uint32 print_none_zero, uint32 hex_base, char* res_buff) {
    uint64 counter_val;
    uint64 counter_ovf;

    counter_val = soc_reg64_field_get(unit, reg, reg_val, field);
    COMPILER_64_SET(counter_ovf, 0, 1);
    COMPILER_64_SHL(counter_ovf, bitnum);
    COMPILER_64_AND(counter_ovf, counter_val);
    print_counter_val_to_string(unit, counter_val, counter_ovf, print_none_zero, hex_base, res_buff);
}

/*if value is higher than 64 bit print as hexa, otherwise hexa/decimal*/
STATIC void
compute_counter_val_into_string_2_field_above_64(int unit, soc_reg_above_64_val_t reg_val, soc_reg_t reg, soc_field_t field,
                                int bitnum, uint32 print_none_zero, uint32 hex_base, char* res_buff) {
    soc_reg_above_64_val_t counter_val;
    uint32 counter_ovf[2], quarter_of_counter_val, hi, lo;
    uint64 lsb_counter_val, counter_ovf_dummy;
    int i;

    soc_reg_above_64_field_get(unit, reg, reg_val, field, counter_val);
    SOC_REG_ABOVE_64_RANGE_COPY(counter_ovf, 0, counter_val, 64, 64);

    if(counter_ovf[0] || counter_ovf[1]){
        sal_sprintf(res_buff, "0x");
        /*skip leading zeros*/
        if(counter_ovf[1]) {
            SOC_REG_ABOVE_64_RANGE_COPY(&quarter_of_counter_val, 0, counter_val, 96, 32);
            sal_sprintf(res_buff + sal_strlen(res_buff), "%x", quarter_of_counter_val);
            i = 2;
        } else {
            SOC_REG_ABOVE_64_RANGE_COPY(&quarter_of_counter_val, 0, counter_val, 64, 32);
            sal_sprintf(res_buff + sal_strlen(res_buff), "%x", quarter_of_counter_val);
            i = 1;
        }
        for(; i>=0; --i) {
            SOC_REG_ABOVE_64_RANGE_COPY(&quarter_of_counter_val, 0, counter_val, i*32, 32);
            sal_sprintf(res_buff + sal_strlen(res_buff), "%08x", quarter_of_counter_val);
        }
    } else {
        SOC_REG_ABOVE_64_RANGE_COPY(&lo, 0, counter_val, 0, 32);
        SOC_REG_ABOVE_64_RANGE_COPY(&hi, 0, counter_val, 32, 32);
        COMPILER_64_SET(lsb_counter_val, hi, lo);
        COMPILER_64_ZERO(counter_ovf_dummy);
        print_counter_val_to_string(unit, lsb_counter_val, counter_ovf_dummy, print_none_zero, hex_base, res_buff);
    }
}

STATIC void
compute_sum_counters_into_string(int unit, uint64* counter_vals, jericho_graphic_counters_e* counter_names, int nof_counters, 
                                diag_counter_data_t* counter_data, uint32 print_none_zero, uint32 hex_base, char* res_buff) {
    uint64 counter_val;
    uint64 counter_sum = COMPILER_64_INIT(0,0);
    uint32 counter_ovf = 0;
    jericho_graphic_counters_e index;
    int i;
    uint64 counter_ovf_64;
  
    for (i = 0; i < nof_counters; ++i) {
        index = counter_names[i];
        counter_val = soc_reg64_field_get(unit, counter_data[index].reg, counter_vals[index], counter_data[index].cnt_field); 
        COMPILER_64_ADD_64(counter_sum, counter_val);
        if (counter_data[index].overflow_field != INVALIDf) {
            counter_ovf |= soc_reg64_field32_get(unit, counter_data[index].reg, counter_vals[index], counter_data[index].overflow_field); 
        }
    }
    COMPILER_64_SET(counter_ovf_64, 0, counter_ovf);
    print_counter_val_to_string(unit, counter_sum, counter_ovf_64, print_none_zero, hex_base, res_buff);
}

#if defined(BCM_88470_A0) || defined(BCM_QUX_SUPPORT)
STATIC void
qax_compute_sum_counters_into_string(int unit, uint64* counter_vals, qax_graphic_counters_e* counter_names, int nof_counters, 
                                diag_counter_data_t* counter_data, uint32 print_none_zero, uint32 hex_base, char* res_buff) {
    uint64 counter_val;
    uint64 counter_sum = COMPILER_64_INIT(0,0);
    uint32 counter_ovf = 0;
    qax_graphic_counters_e index;
    int i;
    uint64 counter_ovf_64;
  
    for (i = 0; i < nof_counters; ++i) {
        index = counter_names[i];
        counter_val = soc_reg64_field_get(unit, counter_data[index].reg, counter_vals[index], counter_data[index].cnt_field); 
        COMPILER_64_ADD_64(counter_sum, counter_val);
        if (counter_data[index].overflow_field != INVALIDf) {
            counter_ovf |= soc_reg64_field32_get(unit, counter_data[index].reg, counter_vals[index], counter_data[index].overflow_field); 
        }
    }
    COMPILER_64_SET(counter_ovf_64, 0, counter_ovf);
    print_counter_val_to_string(unit, counter_sum, counter_ovf_64, print_none_zero, hex_base, res_buff);
}
#endif /* (BCM_88470_A0 || BCM_QUX_SUPPORT) */

STATIC void
compute_sum_rqp_counters(int unit, soc_reg_above_64_val_t rqp_reg_vals, jericho_rqp_graphic_counters_e* counter_names, int nof_counters,
                                diag_counter_data_t* counter_data, uint32 print_none_zero, uint32 hex_base, uint64* counter_sum) {
    uint64 counter_val;
    jericho_rqp_graphic_counters_e index;
    int i;
    COMPILER_64_SET(*counter_sum, 0, 0);

    for (i = 0; i < nof_counters; ++i) {
        index = counter_names[i];
        counter_val = soc_reg_above_64_field64_get(unit, counter_data[index].reg, rqp_reg_vals, counter_data[index].cnt_field);
        COMPILER_64_ADD_64(*counter_sum, counter_val);
    }
}


STATIC void
compute_sum_rqp_counters32(int unit, soc_reg_above_64_val_t rqp_reg_vals, jericho_rqp_graphic_counters_e* counter_names, int nof_counters,
                                diag_counter_data_t* counter_data, uint32 print_none_zero, uint32 hex_base, uint64* counter_sum) {
    uint32 counter_val;
    jericho_rqp_graphic_counters_e index;
    int i;
    COMPILER_64_SET(*counter_sum, 0, 0);

    for (i = 0; i < nof_counters; ++i) {
        index = counter_names[i];
        counter_val = soc_reg_above_64_field32_get(unit, counter_data[index].reg, rqp_reg_vals, counter_data[index].cnt_field);
        COMPILER_64_ADD_32(*counter_sum, counter_val);
    }
}

#if defined(BCM_88470_A0) || defined(BCM_QUX_SUPPORT)
STATIC void
qax_compute_sum_rqp_counters32(int unit, soc_reg_above_64_val_t rqp_reg_vals, qax_rqp_graphic_counters_e* counter_names, int nof_counters,
                                diag_counter_data_t* counter_data, uint32 print_none_zero, uint32 hex_base, uint64* counter_sum) {
    uint32 counter_val;
    jericho_rqp_graphic_counters_e index;
    int i;
    COMPILER_64_SET(*counter_sum, 0, 0);

    for (i = 0; i < nof_counters; ++i) {
        index = counter_names[i];
        counter_val = soc_reg_above_64_field32_get(unit, counter_data[index].reg, rqp_reg_vals, counter_data[index].cnt_field);
        COMPILER_64_ADD_32(*counter_sum, counter_val);
    }
}

STATIC void
qax_compute_sum_rqp_counters64(int unit, soc_reg_above_64_val_t rqp_reg_vals, qax_rqp_graphic_counters_e* counter_names, int nof_counters,
                                diag_counter_data_t* counter_data, uint32 print_none_zero, uint32 hex_base, uint64* counter_sum) {
    uint64 counter_val;
    jericho_rqp_graphic_counters_e index;
    int i;
    COMPILER_64_SET(counter_val, 0, 0);
    COMPILER_64_SET(*counter_sum, 0, 0);

    for (i = 0; i < nof_counters; ++i) {
        index = counter_names[i];
        counter_val = soc_reg_above_64_field64_get(unit, counter_data[index].reg, rqp_reg_vals, counter_data[index].cnt_field);
        COMPILER_64_ADD_64(*counter_sum, counter_val);
    }
}
#endif /* (BCM_88470_A0 || BCM_QUX_SUPPORT) */

STATIC void
print_counter_val_if_neccesary(int unit, char* str_to_print, char* counter_name_str, char* counter_val_str) {

    if (counter_name_str && sal_strcmp("", counter_name_str) && counter_val_str && *counter_val_str){
        cli_out(str_to_print, counter_name_str, '=', counter_val_str);
    } else {
        cli_out(str_to_print, "", ' ', "");
    }
}

STATIC void
print_g_new_line(void) {
   cli_out("|\n");
}

STATIC void
print_g_pattern_new_line(void) {
   cli_out("+\n");
}

STATIC void
print_g_title(char *title) {
    char temp_str[LINE_WIDTH + 1];
    int len = sal_strlen(title);
    sal_sprintf(temp_str, "|%%-%ds%%-%ds", LINE_WIDTH/2 - 1 - len/2, LINE_WIDTH/2 + len/2);
    cli_out(temp_str, "", title);
    print_g_new_line();
}

STATIC void
print_g_section_title(char* title) {
    char temp_str[LINE_WIDTH + 1];
    int len = sal_strlen(title);
    sal_sprintf(temp_str, "|%%-%ds%%-%ds", LINE_WIDTH/4 - 1 - len/2, LINE_WIDTH/4 + len/2);
    cli_out(temp_str, "", title);
}

STATIC void
print_g_title_2(char *title1, char *title2) {
    print_g_section_title(title1);
    print_g_section_title(title2);
    print_g_new_line();
}

STATIC void
print_g_pattern_section(void) {
    int i;
    cli_out("+");
    for (i = 1; i < LINE_WIDTH/2; i++) {
        if (0 == i % (LINE_WIDTH/4))
            cli_out("+");
        else
            cli_out("-");
    }
}

STATIC void
print_g_pattern_line(void) {
    print_g_pattern_section();
    print_g_pattern_section();
    print_g_pattern_new_line();
}

STATIC void
print_g_counter_format(char *counter, char *value, char *str_format, int spacing, int spacing_offset) {
    char temp_str[LINE_WIDTH + 1];
    sal_sprintf(temp_str, str_format, spacing/2 + 1, spacing/2 - 7 - spacing_offset);
    print_counter_val_if_neccesary(0, temp_str, counter, value);
}


STATIC void
print_g_section_format(char *counter1, char *counter2, char (*values)[MAX_NUM_LEN], char *str_format, int spacing_offset) {
    int spacing = (NULL == counter2)? LINE_WIDTH/2 : LINE_WIDTH/4;
    print_g_counter_format(counter1, values[0], str_format, spacing, spacing_offset);
    if (NULL != counter2) {
        print_g_counter_format(counter2, values[1], str_format, spacing, spacing_offset);
    }
}

STATIC void
print_g_section(char *counter1, char *counter2, char (*values)[MAX_NUM_LEN]) {
    print_g_section_format(counter1, counter2, values, "|  %%-%ds %%c %%-%ds", 0);
}

STATIC void
print_g_line(char *counter1, char *counter2, char *counter3, char *counter4, char (*values)[MAX_NUM_LEN]) {
    int offset = (NULL != counter1) + (NULL != counter2 && sal_strcmp("", counter2));
    print_g_section(counter1, counter2, values);
    print_g_section(counter3, counter4, values + offset);
    print_g_new_line();
}

STATIC void
print_g_section_arrow_1(char *counter1, char *counter2, char (*values)[MAX_NUM_LEN]) {
    print_g_section_format(counter1, counter2, values, "|  %%-%ds %%c %%-%ds-", 1);
}

STATIC void
print_g_section_arrow_2(char *counter1, char *counter2, char (*values)[MAX_NUM_LEN]) {
    int spacing = LINE_WIDTH/4;
    print_g_counter_format(counter1, values[0], "-> %%-%ds %%c %%-%ds", spacing, 0);
    print_g_counter_format(counter2, values[1], "|  %%-%ds %%c %%-%ds", spacing, 0);
}

STATIC void
print_g_empty_section(void) {
    print_g_section_title("");
}

/* 
 * jericho_counters_graphic_print 
 *  
 * Read Jericho counters and print graphicly
 * core_disp - display counters per core or only total counters on dual-core blocks
 */
int jericho_counters_graphic_print(int unit, uint32 print_none_zero, uint32 hex_base, uint32 core_disp) {

    diag_counter_data_t* jer_counters = jericho_counters_for_graphic_display;
    diag_counter_data_t* jer_rqp_counters = jericho_rqp_counters_for_graphic_display;
    uint64 *counters_data = NULL;
    uint64 *dual_counters_data = NULL;
    soc_reg_above_64_val_t rqp_reg;
    soc_reg_above_64_val_t rqp_dual_reg;
    soc_reg_above_64_val_t nbih_rx_total_byte_counter_reg, nbih_tx_total_byte_counter_reg;
    uint64 sum_counters;
    uint64 dual_sum_counters;
    char cntbuf[4][MAX_NUM_LEN];
    jericho_graphic_counters_e counters_sum_array[4];
    jericho_rqp_graphic_counters_e rqp_counters_sum_array[6];
    uint64 overflow;

    char *core_div = (!core_disp) ? "" : "CORE 0   ------------+------------   CORE 1";
    /* used for alignment in case core display is requested */
    char *counter_alignment = (core_disp)? "" : NULL;
    
    jericho_graphic_counters_e i;

    counters_data = sal_alloc(128 * sizeof(uint64), "jericho_counters_graphic_print.counters_data");
    if(counters_data == NULL) {
        cli_out("Memory allocation failure\n");
        return CMD_FAIL;
    }
    sal_memset(counters_data, 0x0, 128 * sizeof(uint64));

    dual_counters_data = sal_alloc(128 * sizeof(uint64), "jericho_counters_graphic_print.dual_counters_data");
    if(dual_counters_data == NULL) {
        sal_free(counters_data);
        cli_out("Memory allocation failure\n");
        return CMD_FAIL;
    }
    sal_memset(dual_counters_data, 0x0, 128 * sizeof(uint64));
    
    COMPILER_64_SET(overflow, 0, 0);
    /*read values of all counters registers into array*/
    for (i = 0; jer_counters[i].reg != INVALIDr ; ++i) {
           /*leave holes in counters_data array, for counters of unrelevant device*/
           if((SOC_IS_JERICHO_PLUS(unit) && (jer_counters[i].flags == DIAG_COUNTERS_F_JERICHO_ONLY)) || (!SOC_IS_JERICHO_PLUS_ONLY(unit) && (jer_counters[i].flags == DIAG_COUNTERS_F_JERICHOPLUS_ONLY))) {
               continue;
           }
           if (SOC_E_NONE != soc_reg_get(unit, jer_counters[i].reg, 0, jer_counters[i].reg_index, &counters_data[i])) {
               LOG_ERROR(BSL_LS_APPL_SHELL,
                         (BSL_META_U(unit,
                                     "Failed to read counter: %s\n"), jer_counters[i].reg_name)); 
           }
           if ((jer_counters[i].block == SOC_BLK_IQM) ||
               (jer_counters[i].block == SOC_BLK_EGQ) ||
               (jer_counters[i].block == SOC_BLK_NBIL) ||
               (jer_counters[i].block == SOC_BLK_EPNI)) {
               if (SOC_E_NONE != soc_reg_get(unit, jer_counters[i].reg, 1, jer_counters[i].reg_index, &dual_counters_data[i])) {
                   LOG_ERROR(BSL_LS_APPL_SHELL,
                             (BSL_META_U(unit,
                                         "Failed to read counter: %s\n"), jer_counters[i].reg_name));
               }
           }
           if (!COMPILER_64_IS_ZERO(dual_counters_data[i]) && !core_disp) {
               COMPILER_64_ADD_64(counters_data[i], dual_counters_data[i]);
           }
    }

    /*read RQP register*/
    if (SOC_E_NONE != soc_reg_above_64_get(unit, EGQ_PRP_DEBUG_COUNTERSr, 0, 0, rqp_reg)) {
       LOG_ERROR(BSL_LS_APPL_SHELL,
                 (BSL_META_U(unit,
                             "Failed to read counter EGQ_PRP_DEBUG_COUNTERS\n")));
    }
    if (SOC_E_NONE != soc_reg_above_64_get(unit, EGQ_PRP_DEBUG_COUNTERSr, 1, 1, rqp_dual_reg)) {
       LOG_ERROR(BSL_LS_APPL_SHELL,
                 (BSL_META_U(unit,
                             "Failed to read counter EGQ_PRP_DEBUG_COUNTERS\n")));
    }

    /*read NBIH_RX_TOTAL_BYTE_COUNTER and NBIH_TX_TOTAL_BYTE_COUNTER in Jericho+. Jericho's registers were read in above loop into counters_data*/
    if (SOC_IS_JERICHO_PLUS_ONLY(unit)) {
        if (SOC_E_NONE != soc_reg_above_64_get(unit, NBIH_RX_TOTAL_BYTE_COUNTERr, 0, 0, nbih_rx_total_byte_counter_reg)) {
           LOG_ERROR(BSL_LS_APPL_SHELL,
                     (BSL_META_U(unit,
                                 "Failed to read counter NBIH_RX_TOTAL_BYTE_COUNTER\n")));
        }
        if (SOC_E_NONE != soc_reg_above_64_get(unit, NBIH_TX_TOTAL_BYTE_COUNTERr, 0, 0, nbih_tx_total_byte_counter_reg)) {
          LOG_ERROR(BSL_LS_APPL_SHELL,
                    (BSL_META_U(unit,
                                "Failed to read counter NBIH_TX_TOTAL_BYTE_COUNTER\n")));
       }
   }

    /*print counters*/
    cli_out("\n");
    cli_out("                                            |                                                                      /|\\\n");
    cli_out("                                            |           J E R I C H O   N E T W O R K   I N T E R F A C E           |\n");
    cli_out("                                           \\|/                                                                      |\n");
    print_g_pattern_line();
    print_g_title("NBI");

    if(SOC_IS_JERICHO_PLUS_ONLY(unit)) {
        /*if value is higher than 64 bit print as hexa, otherwise hexa/decimal according to input parameter*/
        compute_counter_val_into_string_2_field_above_64(unit, nbih_rx_total_byte_counter_reg, NBIH_RX_TOTAL_BYTE_COUNTERr,
                                                        RX_TOTAL_BYTE_COUNTERf, 127, print_none_zero, hex_base, cntbuf[0]);
        compute_counter_val_into_string_2_field_above_64(unit, nbih_tx_total_byte_counter_reg, NBIH_TX_TOTAL_BYTE_COUNTERr,
                                                        TX_TOTAL_BYTE_COUNTERf, 127, print_none_zero, hex_base, cntbuf[1]);
    } else {
        compute_counter_val_into_string_2(unit, counters_data[NBIH_RX_TOTAL_BYTE_COUNTER], NBIH_RX_TOTAL_BYTE_COUNTERr,
                                        RX_TOTAL_BYTE_COUNTERf, 61, print_none_zero, hex_base, cntbuf[0]);
        compute_counter_val_into_string_2(unit, counters_data[NBIH_TX_TOTAL_BYTE_COUNTER], NBIH_TX_TOTAL_BYTE_COUNTERr,
                                        TX_TOTAL_BYTE_COUNTERf, 61, print_none_zero, hex_base, cntbuf[1]);
   }

    print_g_line("RX_TOTAL_BYTE_COUNTER", NULL, "TX_TOTAL_BYTE_COUNTER", NULL, cntbuf);

    compute_counter_val_into_string_2(unit, counters_data[NBIH_RX_TOTAL_PKT_COUNTER], NBIH_RX_TOTAL_PKT_COUNTERr, 
                                    RX_TOTAL_PKT_COUNTERf, 31, print_none_zero, hex_base, cntbuf[0]);
    compute_counter_val_into_string_2(unit, counters_data[NBIH_TX_TOTAL_PKT_COUNTER], NBIH_TX_TOTAL_PKT_COUNTERr, 
                                    TX_TOTAL_PKT_COUNTERf, 31, print_none_zero, hex_base, cntbuf[1]);

    print_g_line("RX_TOTAL_PKT_COUNTER", NULL, "TX_TOTAL_PKT_COUNTER", NULL, cntbuf);

    COMPILER_64_ZERO(sum_counters);
    COMPILER_64_ADD_64(sum_counters ,counters_data[NBIH_RX_NUM_TOTAL_DROPPED_EOPS]);
    COMPILER_64_ADD_64(sum_counters ,counters_data[NBIL_RX_NUM_TOTAL_DROPPED_EOPS]);
    COMPILER_64_ADD_64(sum_counters ,dual_counters_data[NBIL_RX_NUM_TOTAL_DROPPED_EOPS]);
    compute_counter_val_into_string_2(unit, sum_counters, NBIH_RX_NUM_TOTAL_DROPPED_EOPSr, 
                                    RX_NUM_TOTAL_DROPPED_EOPSf, 31, print_none_zero, hex_base, cntbuf[0]);

    print_g_line("RX_TOTAL_DROPPED_EOPS", NULL, NULL, NULL, cntbuf);

    print_g_pattern_line();
    print_g_title_2("IRE", "EPNI");

    compute_counter_val_into_string(unit, counters_data[IRE_CPU_PACKET_COUNTER], IRE_CPU_PACKET_COUNTERr, 
                                    CPU_PACKET_COUNTERf, CPU_PACKET_COUNTER_OVFf, print_none_zero, hex_base, cntbuf[0]);

    print_g_line("CPU_PACKET_COUNTER", counter_alignment, NULL, NULL, cntbuf);

    counters_sum_array[0] = IRE_NIF_N_PACKET_COUNTER0;
    counters_sum_array[1] = IRE_NIF_N_PACKET_COUNTER1;
    compute_sum_counters_into_string(unit, counters_data, counters_sum_array, 2, jer_counters, print_none_zero, hex_base, cntbuf[0]);
    
    compute_counter_val_into_string_2(unit, counters_data[EPNI_EPE_BYTES_COUNTER], EPNI_EPE_BYTES_COUNTERr,
                                    EPE_BYTES_COUNTERf, 46, print_none_zero, hex_base, cntbuf[1]);

    if (!core_disp) {
        print_g_line("NIF_PACKET_COUNTER", NULL, "EPE_BYTES_COUNTER", NULL, cntbuf);

    } else {
        compute_counter_val_into_string_2(unit, dual_counters_data[EPNI_EPE_BYTES_COUNTER], EPNI_EPE_BYTES_COUNTERr,
                                        EPE_BYTES_COUNTERf, 46, print_none_zero, hex_base, cntbuf[2]);
        counters_sum_array[0] = IRE_NIF_N_PACKET_COUNTER0;
        compute_sum_counters_into_string(unit, counters_data, counters_sum_array, 1, jer_counters, print_none_zero, hex_base, cntbuf[0]);
        sal_strcpy(cntbuf[3], cntbuf[2]);
        sal_strcpy(cntbuf[2], cntbuf[1]);
        counters_sum_array[0] = IRE_NIF_N_PACKET_COUNTER1;
        compute_sum_counters_into_string(unit, counters_data, counters_sum_array, 1, jer_counters, print_none_zero, hex_base, cntbuf[1]);
        print_g_line("NIF_PACKET_COUNTER_0", "NIF_PACKET_COUNTER_1", "EPE_BYTES_COUNTER", "EPE_BYTES_COUNTER", cntbuf);
    }

    compute_counter_val_into_string(unit, counters_data[IRE_OAMP_PACKET_COUNTER], IRE_OAMP_PACKET_COUNTERr,
                                    OAMP_PACKET_COUNTERf, OAMP_PACKET_COUNTER_OVFf, print_none_zero, hex_base, cntbuf[0]); 
    compute_counter_val_into_string_2(unit, counters_data[EPNI_EPE_PACKET_COUNTER], EPNI_EPE_PACKET_COUNTERr, 
                                    EPE_PACKET_COUNTERf, 32, print_none_zero, hex_base, cntbuf[1]);
    if (!core_disp) {
        print_g_line("OAMP_PACKET_COUNTER", NULL, "EPE_PKT_COUNTER", NULL, cntbuf);
    } else {
        compute_counter_val_into_string_2(unit, dual_counters_data[EPNI_EPE_PACKET_COUNTER], EPNI_EPE_PACKET_COUNTERr,
                                        EPE_PACKET_COUNTERf, 32, print_none_zero, hex_base, cntbuf[2]);
        print_g_line("OAMP_PACKET_COUNTER", counter_alignment, "EPE_PKT_COUNTER", "EPE_PKT_COUNTER", cntbuf);
    }
    
    compute_counter_val_into_string(unit, counters_data[IRE_OLP_PACKET_COUNTER], IRE_OLP_PACKET_COUNTERr,
                                    OLP_PACKET_COUNTERf, OLP_PACKET_COUNTER_OVFf, print_none_zero, hex_base, cntbuf[0]); 
    compute_counter_val_into_string_2(unit, counters_data[EPNI_EPE_DISCARDED_PACKETS_COUNTER], EPNI_EPE_DISCARDED_PACKETS_COUNTERr,
                                    EPE_DISCARDED_PACKETS_COUNTERf, 46, print_none_zero, hex_base, cntbuf[1]);
    if (!core_disp) {
        print_g_line("OLP_PACKET_COUNTER", NULL, "EPE_DSCRD_PKT_CNT", NULL, cntbuf);

    } else {
        compute_counter_val_into_string_2(unit, dual_counters_data[EPNI_EPE_DISCARDED_PACKETS_COUNTER], EPNI_EPE_DISCARDED_PACKETS_COUNTERr,
                                        EPE_DISCARDED_PACKETS_COUNTERf, 46, print_none_zero, hex_base, cntbuf[2]);
        print_g_line("OLP_PACKET_COUNTER", counter_alignment, "EPE_DSCRD_PKT_CNT", "EPE_DSCRD_PKT_CNT", cntbuf);
    }

    if (!core_disp) {
            counters_sum_array[0] = IRE_RCY_N_PACKET_COUNTER0;
            counters_sum_array[1] = IRE_RCY_N_PACKET_COUNTER1;
            counters_sum_array[2] = IRE_RCY_N_PACKET_COUNTER2;
            counters_sum_array[3] = IRE_RCY_N_PACKET_COUNTER3;
            compute_sum_counters_into_string(unit, counters_data, counters_sum_array, 4, jer_counters, print_none_zero, hex_base, cntbuf[0]);

            print_g_line("RCY_PACKET_COUNTER", NULL, NULL, NULL, cntbuf);
    } else {
            counters_sum_array[0] = IRE_RCY_N_PACKET_COUNTER0;
            compute_sum_counters_into_string(unit, counters_data, counters_sum_array, 1, jer_counters, print_none_zero, hex_base, cntbuf[0]);
            counters_sum_array[0] = IRE_RCY_N_PACKET_COUNTER2;
            compute_sum_counters_into_string(unit, counters_data, counters_sum_array, 1, jer_counters, print_none_zero, hex_base, cntbuf[1]);
            print_g_line("RCY_PACKET_CNT_0_0", "RCY_PACKET_CNT_1_0", NULL, NULL, cntbuf);

            counters_sum_array[0] = IRE_RCY_N_PACKET_COUNTER1;
            compute_sum_counters_into_string(unit, counters_data, counters_sum_array, 1, jer_counters, print_none_zero, hex_base, cntbuf[0]);
            counters_sum_array[0] = IRE_RCY_N_PACKET_COUNTER3;
            compute_sum_counters_into_string(unit, counters_data, counters_sum_array, 1, jer_counters, print_none_zero, hex_base, cntbuf[1]);
            print_g_line("RCY_PACKET_CNT_0_1", "RCY_PACKET_CNT_1_1", NULL, NULL, cntbuf);


    }

    compute_counter_val_into_string(unit, counters_data[IRE_FDT_INTERFACE_COUNTER], IRE_FDT_INTERFACE_COUNTERr,
                                        FDT_CREDITSf, FDT_CREDITS_OVFf, print_none_zero, hex_base, cntbuf[0]);
    print_g_line("IRE_FDT_INTRFACE_CNT", counter_alignment, NULL, NULL, cntbuf);

    print_g_pattern_line();
    print_g_title_2("IDR", "EGQ");
    print_g_title_2("", core_div);

    compute_counter_val_into_string_2(unit, counters_data[MMU_IDR_PACKET_COUNTER], MMU_IDR_PACKET_COUNTERr,
                                    IDR_PACKET_COUNTERf, 31, print_none_zero, hex_base, cntbuf[0]);
    compute_counter_val_into_string_2(unit, counters_data[EGQ_FQP_PACKET_COUNTER], EGQ_FQP_PACKET_COUNTERr,
                                    FQP_PACKET_COUNTERf, 32, print_none_zero, hex_base, cntbuf[1]);
    

    if (!core_disp) {
        print_g_line("MMU_IDR_PACKET_COUNTER", NULL, "FQP_PACKET_COUNTER", NULL, cntbuf);

    } else {
        compute_counter_val_into_string_2(unit, dual_counters_data[EGQ_FQP_PACKET_COUNTER], EGQ_FQP_PACKET_COUNTERr,
                                        FQP_PACKET_COUNTERf, 32, print_none_zero, hex_base, cntbuf[2]);
        print_g_line("MMU_IDR_PACKET_COUNTER", NULL, "FQP_PACKET_COUNTER", "FQP_PACKET_COUNTER", cntbuf);
    }

    compute_counter_val_into_string(unit, counters_data[IDR_OCB_INTERFACE_COUNTER], IDR_OCB_INTERFACE_COUNTERr,
                                    OCB_BUBBLE_REQUESTSf, OCB_BUBBLE_REQUESTS_OVFf, print_none_zero, hex_base, cntbuf[0]); 
    compute_counter_val_into_string_2(unit, counters_data[EGQ_PQP_UNICAST_PACKET_COUNTER], EGQ_PQP_UNICAST_PACKET_COUNTERr, 
                                    PQP_UNICAST_PACKET_COUNTERf, 32, print_none_zero, hex_base, cntbuf[1]);     

    if (!core_disp) {
        print_g_line("IDR_OCB_INTERFACE_COUNTER", NULL, "PQP_UNICAST_PKT_CNT", NULL, cntbuf);
    } else {
        compute_counter_val_into_string_2(unit, dual_counters_data[EGQ_PQP_UNICAST_PACKET_COUNTER], EGQ_PQP_UNICAST_PACKET_COUNTERr,
                                        PQP_UNICAST_PACKET_COUNTERf, 32, print_none_zero, hex_base, cntbuf[2]);
        print_g_line("IDR_OCB_INTERFACE_COUNTER", NULL, "PQP_UNICAST_PKT_CNT", "PQP_UNICAST_PKT_CNT", cntbuf);
    }

    compute_counter_val_into_string_2(unit, counters_data[EGQ_PQP_DISCARD_UNICAST_PACKET_COUNTER], EGQ_PQP_DISCARD_UNICAST_PACKET_COUNTERr,
                                    PQP_DISCARD_UNICAST_PACKET_COUNTERf, 32, print_none_zero, hex_base, cntbuf[0]);

    if (!core_disp) {
        print_g_line(NULL, NULL, "PQP_DSCRD_UC_PKT_CNT", NULL, cntbuf);

    } else {
        compute_counter_val_into_string_2(unit, dual_counters_data[EGQ_PQP_DISCARD_UNICAST_PACKET_COUNTER], EGQ_PQP_DISCARD_UNICAST_PACKET_COUNTERr,
                                        PQP_DISCARD_UNICAST_PACKET_COUNTERf, 32, print_none_zero, hex_base, cntbuf[1]);
        print_g_line(NULL, NULL, "PQP_DSCRD_UC_PKT_CNT", "PQP_DSCRD_UC_PKT_CNT", cntbuf);
    }

    compute_counter_val_into_string_2(unit, counters_data[EGQ_PQP_UNICAST_BYTES_COUNTER], EGQ_PQP_UNICAST_BYTES_COUNTERr,
                                    PQP_UNICAST_BYTES_COUNTERf, 46, print_none_zero, hex_base, cntbuf[0]); 

    if (!core_disp) {
        print_g_line(NULL, NULL, "PQP_UC_BYTES_CNT", NULL, cntbuf);

    } else {
        compute_counter_val_into_string_2(unit, dual_counters_data[EGQ_PQP_UNICAST_BYTES_COUNTER], EGQ_PQP_UNICAST_BYTES_COUNTERr,
                                        PQP_UNICAST_BYTES_COUNTERf, 46, print_none_zero, hex_base, cntbuf[1]);
        print_g_line(NULL, NULL, "PQP_UC_BYTES_CNT", "PQP_UC_BYTES_CNT", cntbuf);
    }

    compute_counter_val_into_string_2(unit, counters_data[EGQ_PQP_MULTICAST_PACKET_COUNTER], EGQ_PQP_MULTICAST_PACKET_COUNTERr,
                                    PQP_MULTICAST_PACKET_COUNTERf, 32, print_none_zero, hex_base, cntbuf[0]);

    print_g_pattern_section();
    if (!core_disp) {
        print_g_section("PQP_MC_PKT_CNT", NULL, cntbuf);

    } else {
        compute_counter_val_into_string_2(unit, dual_counters_data[EGQ_PQP_MULTICAST_PACKET_COUNTER], EGQ_PQP_MULTICAST_PACKET_COUNTERr,
                                        PQP_MULTICAST_PACKET_COUNTERf, 32, print_none_zero, hex_base, cntbuf[1]);
        print_g_section("PQP_MC_PKT_CNT", "PQP_MC_PKT_CNT", cntbuf);
    }
    print_g_new_line();

    compute_counter_val_into_string_2(unit, counters_data[EGQ_PQP_DISCARD_MULTICAST_PACKET_COUNTER], EGQ_PQP_DISCARD_MULTICAST_PACKET_COUNTERr,
                                    PQP_DISCARD_MULTICAST_PACKET_COUNTERf, 32, print_none_zero, hex_base, cntbuf[0]);

    print_g_section_title("IQM");
    if (!core_disp) {
        print_g_section("PQP_DSCRD_MC_PKT_CNT", NULL, cntbuf);
    } else {
        compute_counter_val_into_string_2(unit, dual_counters_data[EGQ_PQP_DISCARD_MULTICAST_PACKET_COUNTER], EGQ_PQP_DISCARD_MULTICAST_PACKET_COUNTERr,
                                        PQP_DISCARD_MULTICAST_PACKET_COUNTERf, 32, print_none_zero, hex_base, cntbuf[1]);
        print_g_section("PQP_DSCRD_MC_PKT_CNT", "PQP_DSCRD_MC_PKT_CNT", cntbuf);        
    }
    print_g_new_line();

    compute_counter_val_into_string_2(unit, counters_data[EGQ_PQP_MULTICAST_BYTES_COUNTER], EGQ_PQP_MULTICAST_BYTES_COUNTERr,
                                    PQP_MULTICAST_BYTES_COUNTERf, 46, print_none_zero, hex_base, cntbuf[0]);
    print_g_section_title(core_div);
    if (!core_disp) {
        print_g_section("PQP_MC_BYTES_CNT", NULL, cntbuf);

    } else {
        compute_counter_val_into_string_2(unit, dual_counters_data[EGQ_PQP_MULTICAST_BYTES_COUNTER], EGQ_PQP_MULTICAST_BYTES_COUNTERr,
                                        PQP_MULTICAST_BYTES_COUNTERf, 46, print_none_zero, hex_base, cntbuf[1]);
        print_g_section("PQP_MC_BYTES_CNT", "PQP_MC_BYTES_CNT", cntbuf);
    }
    print_g_new_line();

    compute_counter_val_into_string_2(unit, counters_data[IQM_ENQUEUE_PACKET_COUNTER], IQM_ENQUEUE_PACKET_COUNTERr, 
                                    ENQUEUE_PACKET_COUNTERf, 31, print_none_zero, hex_base, cntbuf[0]); 
    compute_counter_val_into_string_2(unit, counters_data[EGQ_EHP_UNICAST_PACKET_COUNTER], EGQ_EHP_UNICAST_PACKET_COUNTERr,
                                    EHP_UNICAST_PACKET_COUNTERf, 32, print_none_zero, hex_base, cntbuf[1]);
    if (!core_disp) {
        print_g_line("ENQUEUE_PKT_CNT", NULL, "EHP_UNICAST_PKT_CNT", NULL, cntbuf);
    } else {
        sal_strcpy(cntbuf[2], cntbuf[1]);
        compute_counter_val_into_string_2(unit, dual_counters_data[IQM_ENQUEUE_PACKET_COUNTER], IQM_ENQUEUE_PACKET_COUNTERr,
                                        ENQUEUE_PACKET_COUNTERf, 31, print_none_zero, hex_base, cntbuf[1]);
        compute_counter_val_into_string_2(unit, dual_counters_data[EGQ_EHP_UNICAST_PACKET_COUNTER], EGQ_EHP_UNICAST_PACKET_COUNTERr,
                                        EHP_UNICAST_PACKET_COUNTERf, 32, print_none_zero, hex_base, cntbuf[3]);
        print_g_line("ENQUEUE_PKT_CNT", "ENQUEUE_PKT_CNT", "EHP_UNICAST_PKT_CNT", "EHP_UNICAST_PKT_CNT", cntbuf);
    }

    compute_counter_val_into_string_2(unit, counters_data[IQM_DEQUEUE_PACKET_COUNTER], IQM_DEQUEUE_PACKET_COUNTERr, 
                                    DEQUEUE_PACKET_COUNTERf, 31, print_none_zero, hex_base, cntbuf[0]); 
    compute_counter_val_into_string_2(unit, counters_data[EGQ_EHP_MULTICAST_HIGH_PACKET_COUNTER], EGQ_EHP_MULTICAST_HIGH_PACKET_COUNTERr,
                                    EHP_MULTICAST_HIGH_PACKET_COUNTERf, 32, print_none_zero, hex_base, cntbuf[1]);
    if (!core_disp) {
        print_g_line("DEQUEUE_PKT_CNT", NULL, "EHP_MC_HIGH_PKT_CNT", NULL, cntbuf);

    } else {
        sal_strcpy(cntbuf[2], cntbuf[1]);
        compute_counter_val_into_string_2(unit, dual_counters_data[IQM_DEQUEUE_PACKET_COUNTER], IQM_DEQUEUE_PACKET_COUNTERr,
                                        DEQUEUE_PACKET_COUNTERf, 31, print_none_zero, hex_base, cntbuf[1]);
        compute_counter_val_into_string_2(unit, dual_counters_data[EGQ_EHP_MULTICAST_HIGH_PACKET_COUNTER], EGQ_EHP_MULTICAST_HIGH_PACKET_COUNTERr,
                                        EHP_MULTICAST_HIGH_PACKET_COUNTERf, 32, print_none_zero, hex_base, cntbuf[3]);
        print_g_line("DEQUEUE_PKT_CNT", "DEQUEUE_PKT_CNT", "EHP_MC_HI_PKT_CNT", "EHP_MC_HI_PKT_CNT", cntbuf);
    }
  
    compute_counter_val_into_string_2(unit, counters_data[IQM_QUEUE_DELETED_PACKET_COUNTER], IQM_QUEUE_DELETED_PACKET_COUNTERr,
                                    QUEUE_DELETED_PACKET_COUNTERf, 31, print_none_zero, hex_base, cntbuf[0]);
    compute_counter_val_into_string_2(unit, counters_data[EGQ_EHP_MULTICAST_LOW_PACKET_COUNTER], EGQ_EHP_MULTICAST_LOW_PACKET_COUNTERr,
                                    EHP_MULTICAST_LOW_PACKET_COUNTERf, 32, print_none_zero, hex_base, cntbuf[1]);
    if (!core_disp) {
        print_g_line("DELETED_PKT_CNT", NULL, "EHP_MC_LOW_PKT_CNT", NULL, cntbuf);

    } else {
        sal_strcpy(cntbuf[2], cntbuf[1]);
        compute_counter_val_into_string_2(unit, dual_counters_data[IQM_QUEUE_DELETED_PACKET_COUNTER], IQM_QUEUE_DELETED_PACKET_COUNTERr,
                                       QUEUE_DELETED_PACKET_COUNTERf, 31, print_none_zero, hex_base, cntbuf[1]);
        compute_counter_val_into_string_2(unit, dual_counters_data[EGQ_EHP_MULTICAST_LOW_PACKET_COUNTER], EGQ_EHP_MULTICAST_LOW_PACKET_COUNTERr,
                                        EHP_MULTICAST_LOW_PACKET_COUNTERf, 32, print_none_zero, hex_base, cntbuf[3]);
        print_g_line("DELETED_PKT_CNT", "DELETED_PKT_CNT", "EHP_MC_LOW_PKT_CNT", "EHP_MC_LOW_PKT_CNT", cntbuf);
    }
 
    compute_counter_val_into_string_2(unit, counters_data[IQM_QUEUE_ENQ_DISCARDED_PACKET_COUNTER], IQM_QUEUE_ENQ_DISCARDED_PACKET_COUNTERr,
                                    QUEUE_ENQ_DISCARDED_PACKET_COUNTERf, 31, print_none_zero, hex_base, cntbuf[0]);
    compute_counter_val_into_string_2(unit, counters_data[EGQ_EHP_DISCARD_PACKET_COUNTER], EGQ_EHP_DISCARD_PACKET_COUNTERr,
                                    EHP_DISCARD_PACKET_COUNTERf, 32, print_none_zero, hex_base, cntbuf[1]);
    if (!core_disp) {
        print_g_line("ENQ_DISCARDED_PACKET_COUNTER", NULL, "DELETED_PKT_CNT", NULL, cntbuf);
    } else {
        sal_strcpy(cntbuf[2], cntbuf[1]);
        compute_counter_val_into_string_2(unit, dual_counters_data[IQM_QUEUE_ENQ_DISCARDED_PACKET_COUNTER], IQM_QUEUE_ENQ_DISCARDED_PACKET_COUNTERr,
                                        QUEUE_ENQ_DISCARDED_PACKET_COUNTERf, 31, print_none_zero, hex_base, cntbuf[1]);
        compute_counter_val_into_string_2(unit, dual_counters_data[EGQ_EHP_DISCARD_PACKET_COUNTER], EGQ_EHP_DISCARD_PACKET_COUNTERr,
                                        EHP_DISCARD_PACKET_COUNTERf, 32, print_none_zero, hex_base, cntbuf[3]);
        print_g_line("ENQ_DSCRD_PKT_CNT", "ENQ_DSCRD_PKT_CNT", "DELETED_PKT_CNT", "DELETED_PKT_CNT", cntbuf);        
    }

    diag_print_iqm_rejects_and_erpp_discards(unit, core_disp);
    

    COMPILER_64_ZERO(sum_counters);
    COMPILER_64_ZERO(dual_sum_counters);

    rqp_counters_sum_array[0] = EGQ_PRP_PACKET_GOOD_UC_CNT0;
    rqp_counters_sum_array[1] = EGQ_PRP_PACKET_GOOD_MC_CNT0;
    rqp_counters_sum_array[2] = EGQ_PRP_PACKET_GOOD_TDM_CNT0;
    compute_sum_rqp_counters(unit, rqp_reg, rqp_counters_sum_array, 3, jer_rqp_counters, print_none_zero, hex_base, &sum_counters);

    rqp_counters_sum_array[0] = EGQ_PRP_PACKET_GOOD_UC_CNT1;
    rqp_counters_sum_array[1] = EGQ_PRP_PACKET_GOOD_MC_CNT1;
    rqp_counters_sum_array[2] = EGQ_PRP_PACKET_GOOD_TDM_CNT1;
    compute_sum_rqp_counters(unit, rqp_dual_reg, rqp_counters_sum_array, 3, jer_rqp_counters, print_none_zero, hex_base, &dual_sum_counters);

    if (!core_disp) {
        COMPILER_64_ADD_64(sum_counters, dual_sum_counters);
        print_counter_val_to_string(unit, sum_counters, overflow, print_none_zero, hex_base, cntbuf[0]);
        print_g_line(NULL, NULL, "RQP_PKT_CNT", NULL, cntbuf);
    } else {
        print_counter_val_to_string(unit, sum_counters, overflow, print_none_zero, hex_base, cntbuf[0]);
        print_counter_val_to_string(unit, dual_sum_counters, overflow, print_none_zero, hex_base, cntbuf[1]);
        print_g_line(NULL, NULL, "RQP_PKT_CNT", "RQP_PKT_CNT", cntbuf);
    }

    COMPILER_64_ZERO(sum_counters);
    COMPILER_64_ZERO(dual_sum_counters);

    rqp_counters_sum_array[0] = EGQ_PRP_PACKET_DISCARD_UC_CNT0;
    rqp_counters_sum_array[1] = EGQ_PRP_PACKET_DISCARD_MC_CNT0;
    rqp_counters_sum_array[2] = EGQ_PRP_PACKET_DISCARD_TDM_CNT0;
    rqp_counters_sum_array[3] = EGQ_PRP_SOP_DISCARD_UC_CNT0;
    rqp_counters_sum_array[4] = EGQ_PRP_SOP_DISCARD_MC_CNT0;
    rqp_counters_sum_array[5] = EGQ_PRP_SOP_DISCARD_TDM_CNT0;
    compute_sum_rqp_counters32(unit, rqp_reg, rqp_counters_sum_array, 6, jer_rqp_counters, print_none_zero, hex_base, &sum_counters);

    rqp_counters_sum_array[0] = EGQ_PRP_PACKET_DISCARD_UC_CNT1;
    rqp_counters_sum_array[1] = EGQ_PRP_PACKET_DISCARD_MC_CNT1;
    rqp_counters_sum_array[2] = EGQ_PRP_PACKET_DISCARD_TDM_CNT1;
    rqp_counters_sum_array[3] = EGQ_PRP_SOP_DISCARD_UC_CNT1;
    rqp_counters_sum_array[4] = EGQ_PRP_SOP_DISCARD_MC_CNT1;
    rqp_counters_sum_array[5] = EGQ_PRP_SOP_DISCARD_TDM_CNT1;
    compute_sum_rqp_counters32(unit, rqp_dual_reg, rqp_counters_sum_array, 6, jer_rqp_counters, print_none_zero, hex_base, &dual_sum_counters);

    if (!core_disp) {
        COMPILER_64_ADD_64(sum_counters, dual_sum_counters);
        print_counter_val_to_string(unit, sum_counters, overflow, print_none_zero, hex_base, cntbuf[0]);
        print_g_line(NULL, NULL, "RQP_DSCRD_PKT_CNT", NULL, cntbuf);
    } else {
        print_counter_val_to_string(unit, sum_counters, overflow, print_none_zero, hex_base, cntbuf[0]);
        print_counter_val_to_string(unit, dual_sum_counters, overflow, print_none_zero, hex_base, cntbuf[1]);
        print_g_line(NULL, NULL, "RQP_DSCRD_PKT_CNT", "RQP_DSCRD_PKT_CNT", cntbuf);
    }

    rqp_counters_sum_array[0] = EGQ_PRP_PACKET_DISCARD_TDM_CNT0;
    compute_sum_rqp_counters32(unit, rqp_reg, rqp_counters_sum_array, 1, jer_rqp_counters, print_none_zero, hex_base, &sum_counters);
    print_counter_val_to_string(unit, sum_counters, overflow, print_none_zero, hex_base, cntbuf[0]);

    if (!core_disp) {
        print_g_line(NULL, NULL, "PRP_PKT_DSCRD_TDM_CNT", NULL, cntbuf);
    } else {
        rqp_counters_sum_array[0] = EGQ_PRP_PACKET_DISCARD_TDM_CNT1;
        compute_sum_rqp_counters32(unit, rqp_reg, rqp_counters_sum_array, 1, jer_rqp_counters, print_none_zero, hex_base, &dual_sum_counters);
        print_counter_val_to_string(unit, dual_sum_counters, overflow, print_none_zero, hex_base, cntbuf[1]);
        print_g_line(NULL, NULL, "PRP_PKT_DSCRD_TDM_CNT", "PRP_PKT_DSCRD_TDM_CNT", cntbuf);
    }

    rqp_counters_sum_array[0] = EGQ_PRP_SOP_DISCARD_UC_CNT0;
    compute_sum_rqp_counters32(unit, rqp_reg, rqp_counters_sum_array, 1, jer_rqp_counters, print_none_zero, hex_base, &sum_counters);
    print_counter_val_to_string(unit, sum_counters, overflow, print_none_zero, hex_base, cntbuf[0]);
    

    if (!core_disp) {
        print_g_line(NULL, NULL, "PRP_SOP_DSCRD_UC_CNT", NULL, cntbuf);
    } else {
        rqp_counters_sum_array[0] = EGQ_PRP_SOP_DISCARD_UC_CNT1;
        compute_sum_rqp_counters32(unit, rqp_reg, rqp_counters_sum_array, 1, jer_rqp_counters, print_none_zero, hex_base, &dual_sum_counters);
        print_counter_val_to_string(unit, dual_sum_counters, overflow, print_none_zero, hex_base, cntbuf[1]);
        print_g_line(NULL, NULL, "PRP_SOP_DSCRD_UC_CNT", "PRP_SOP_DSCRD_UC_CNT", cntbuf);
    }

    rqp_counters_sum_array[0] = EGQ_PRP_SOP_DISCARD_MC_CNT0;
    compute_sum_rqp_counters32(unit, rqp_reg, rqp_counters_sum_array, 1, jer_rqp_counters, print_none_zero, hex_base, &sum_counters);
    print_counter_val_to_string(unit, sum_counters, overflow, print_none_zero, hex_base, cntbuf[0]);
    

    if (!core_disp) {
        print_g_line(NULL, NULL, "PRP_SOP_DSCRD_MC_CNT", NULL, cntbuf);
    } else {
        rqp_counters_sum_array[0] = EGQ_PRP_SOP_DISCARD_MC_CNT1;
        compute_sum_rqp_counters32(unit, rqp_reg, rqp_counters_sum_array, 1, jer_rqp_counters, print_none_zero, hex_base, &dual_sum_counters);
        print_counter_val_to_string(unit, dual_sum_counters, overflow, print_none_zero, hex_base, cntbuf[1]);
        print_g_line(NULL, NULL, "PRP_SOP_DSCRD_MC_CNT", "PRP_SOP_DSCRD_MC_CNT", cntbuf);
    }
    rqp_counters_sum_array[0] = EGQ_PRP_SOP_DISCARD_TDM_CNT0;
    compute_sum_rqp_counters32(unit, rqp_reg, rqp_counters_sum_array, 1, jer_rqp_counters, print_none_zero, hex_base, &sum_counters);
    print_counter_val_to_string(unit, sum_counters, overflow, print_none_zero, hex_base, cntbuf[0]);
    
    if (!core_disp) {
        print_g_line(NULL, NULL, "PRP_SOP_DSCRD_TDM_CNT", NULL, cntbuf);
    } else {
        rqp_counters_sum_array[0] = EGQ_PRP_SOP_DISCARD_TDM_CNT1;
        compute_sum_rqp_counters32(unit, rqp_reg, rqp_counters_sum_array, 1, jer_rqp_counters, print_none_zero, hex_base, &dual_sum_counters);
        print_counter_val_to_string(unit, dual_sum_counters, overflow, print_none_zero, hex_base, cntbuf[1]);
        print_g_line(NULL, NULL, "PRP_SOP_DSCRD_TDM_CNT", "PRP_SOP_DSCRD_TDM_CNT", cntbuf);
    }

    /* ERPP Regs counter */
    compute_counter_val_into_string_2(unit, counters_data[EGQ_EHP_MULTICAST_HIGH_DISCARDS_COUNTER], EGQ_EHP_MULTICAST_HIGH_DISCARDS_COUNTERr, 
                                    EHP_MULTICAST_HIGH_DISCARDS_COUNTERf, 32, print_none_zero, hex_base, cntbuf[0]); 
    if (!core_disp) {
        print_g_line(NULL, NULL, "EHP_MC_HIGH_DSCRD_CNT", NULL, cntbuf);
    }else{
        compute_counter_val_into_string_2(unit, dual_counters_data[EGQ_EHP_MULTICAST_HIGH_DISCARDS_COUNTER], EGQ_EHP_MULTICAST_HIGH_DISCARDS_COUNTERr,
                                        EHP_MULTICAST_HIGH_DISCARDS_COUNTERf, 32, print_none_zero, hex_base, cntbuf[1]);
        print_g_line(NULL, NULL, "EHP_MC_HIGH_DSCRD_CNT", "EHP_MC_HIGH_DSCRD_CNT", cntbuf);   
    }

    compute_counter_val_into_string_2(unit, counters_data[EGQ_EHP_MULTICAST_LOW_DISCARDS_COUNTER], EGQ_EHP_MULTICAST_LOW_DISCARDS_COUNTERr, 
                                    EHP_MULTICAST_LOW_DISCARDS_COUNTERf, 32, print_none_zero, hex_base, cntbuf[0]); 
    if (!core_disp) {
        print_g_line(NULL, NULL, "EHP_MC_LOW_DSCRD_CNT", NULL, cntbuf);
    }else{
        compute_counter_val_into_string_2(unit, dual_counters_data[EGQ_EHP_MULTICAST_LOW_DISCARDS_COUNTER], EGQ_EHP_MULTICAST_LOW_DISCARDS_COUNTERr,
                                        EHP_MULTICAST_LOW_DISCARDS_COUNTERf, 32, print_none_zero, hex_base, cntbuf[1]);
        print_g_line(NULL, NULL, "EHP_MC_LOW_DSCRD_CNT", "EHP_MC_LOW_DSCRD_CNT", cntbuf);   
    }

    compute_counter_val_into_string_2(unit, counters_data[EGQ_ERPP_LAG_PRUNING_DISCARDS_COUNTER], EGQ_ERPP_LAG_PRUNING_DISCARDS_COUNTERr, 
                                    ERPP_LAG_PRUNING_DISCARDS_COUNTERf, 32, print_none_zero, hex_base, cntbuf[0]); 
    if (!core_disp) {
        print_g_line(NULL, NULL, "ERPP_LAG_PRUNING_DSCRD_CNT", NULL, cntbuf);
    }else{
        compute_counter_val_into_string_2(unit, dual_counters_data[EGQ_ERPP_LAG_PRUNING_DISCARDS_COUNTER], EGQ_ERPP_LAG_PRUNING_DISCARDS_COUNTERr,
                                        ERPP_LAG_PRUNING_DISCARDS_COUNTERf, 32, print_none_zero, hex_base, cntbuf[1]);
        print_g_line(NULL, NULL, "EHP_MC_LOW_DSCRD_CNT", "EHP_MC_LOW_DSCRD_CNT", cntbuf);   
    }

    compute_counter_val_into_string_2(unit, counters_data[EGQ_ERPP_PMF_DISCARDS_COUNTER], EGQ_ERPP_PMF_DISCARDS_COUNTERr, 
                                    ERPP_PMF_DISCARDS_COUNTERf, 32, print_none_zero, hex_base, cntbuf[0]); 
    if (!core_disp) {
        print_g_line(NULL, NULL, "ERPP_PMF_DISCARDS_CNT", NULL, cntbuf);
    }
    else{
        compute_counter_val_into_string_2(unit, dual_counters_data[EGQ_ERPP_PMF_DISCARDS_COUNTER], EGQ_ERPP_PMF_DISCARDS_COUNTERr,
                                        ERPP_PMF_DISCARDS_COUNTERf, 32, print_none_zero, hex_base, cntbuf[1]);
        print_g_line(NULL, NULL, "ERPP_PMF_DSCRD_CNT", "ERPP_PMF_DSCRD_CNT", cntbuf);   
    }

    compute_counter_val_into_string_2(unit, counters_data[EGQ_ERPP_VLAN_MEMBERSHIP_DISCARDS_COUNTER], EGQ_ERPP_VLAN_MEMBERSHIP_DISCARDS_COUNTERr, 
                                    ERPP_VLAN_MEMBERSHIP_DISCARDS_COUNTERf, 32, print_none_zero, hex_base, cntbuf[0]); 
    if (!core_disp) {
        print_g_line(NULL, NULL, "ERPP_VLAN_MBR_DSCRD_CNT", NULL, cntbuf);
    }
    else{
        compute_counter_val_into_string_2(unit, dual_counters_data[EGQ_ERPP_VLAN_MEMBERSHIP_DISCARDS_COUNTER], EGQ_ERPP_VLAN_MEMBERSHIP_DISCARDS_COUNTERr,
                                        ERPP_VLAN_MEMBERSHIP_DISCARDS_COUNTERf, 32, print_none_zero, hex_base, cntbuf[1]);
        print_g_line(NULL, NULL, "ERPP_VLAN_MBR_DSCRD_CNT", "ERPP_VLAN_MBR_DSCRD_CNT", cntbuf);   
    }


    print_g_pattern_line();
    print_g_title_2("", "FDA");
 
    if(SOC_IS_JERICHO_PLUS_ONLY(unit)) {
        compute_counter_val_into_string(unit, counters_data[FDA_FAB_48_SCH_CELLS_IN_CNT_P_1], FDA_FAB_48_SCH_CELLS_IN_CNT_P_1r,
                                            FAB_48_SCH_CELLS_IN_CNT_P_1f, FAB_48_SCH_CELLS_IN_CNT_P_1_OVERFLOWf, print_none_zero, hex_base, cntbuf[0]);
    } else {
        compute_counter_val_into_string(unit, counters_data[FDA_FAB_36_SCH_CELLS_IN_CNT_P_1], FDA_FAB_36_SCH_CELLS_IN_CNT_P_1r,
                                            FAB_36_SCH_CELLS_IN_CNT_P_1f, FAB_36_SCH_CELLS_IN_CNT_P_1_OVERFLOWf, print_none_zero, hex_base, cntbuf[0]);
    }
    
    counters_sum_array[0] = FDA_EGQ_N_CELLS_OUT_CNT_FAB_PIPE_00;
    counters_sum_array[1] = FDA_EGQ_N_CELLS_OUT_CNT_FAB_PIPE_01;
    compute_sum_counters_into_string(unit, counters_data, counters_sum_array, 2, jer_counters, print_none_zero, hex_base, cntbuf[1]);

    print_g_line(NULL, NULL, "CELLS_IN_CNT_P1", "CELLS_OUT_CNT_P1", cntbuf);
    
    if(SOC_IS_JERICHO_PLUS_ONLY(unit)) {
        compute_counter_val_into_string(unit, counters_data[FDA_FAB_48_SCH_CELLS_IN_CNT_P_2], FDA_FAB_48_SCH_CELLS_IN_CNT_P_2r,
                                            FAB_48_SCH_CELLS_IN_CNT_P_2f, FAB_48_SCH_CELLS_IN_CNT_P_2_OVERFLOWf, print_none_zero, hex_base, cntbuf[0]);
    } else {
        compute_counter_val_into_string(unit, counters_data[FDA_FAB_36_SCH_CELLS_IN_CNT_P_2], FDA_FAB_36_SCH_CELLS_IN_CNT_P_2r,
                                            FAB_36_SCH_CELLS_IN_CNT_P_2f, FAB_36_SCH_CELLS_IN_CNT_P_2_OVERFLOWf, print_none_zero, hex_base, cntbuf[0]);
    }
    
    counters_sum_array[0] = FDA_EGQ_N_CELLS_OUT_CNT_FAB_PIPE_10;
    counters_sum_array[1] = FDA_EGQ_N_CELLS_OUT_CNT_FAB_PIPE_11;
    compute_sum_counters_into_string(unit, counters_data, counters_sum_array, 2, jer_counters, print_none_zero, hex_base, cntbuf[1]);


    print_g_line(NULL, NULL, "CELLS_IN_CNT_P2", "CELLS_OUT_CNT_P2", cntbuf);

    if(SOC_IS_JERICHO_PLUS_ONLY(unit)) {
        compute_counter_val_into_string(unit, counters_data[FDA_FAB_48_SCH_CELLS_IN_CNT_P_3], FDA_FAB_48_SCH_CELLS_IN_CNT_P_3r,
                                            FAB_48_SCH_CELLS_IN_CNT_P_3f, FAB_48_SCH_CELLS_IN_CNT_P_3_OVERFLOWf, print_none_zero, hex_base, cntbuf[0]);
    } else {
        compute_counter_val_into_string(unit, counters_data[FDA_FAB_36_SCH_CELLS_IN_CNT_P_3], FDA_FAB_36_SCH_CELLS_IN_CNT_P_3r,
                                            FAB_36_SCH_CELLS_IN_CNT_P_3f, FAB_36_SCH_CELLS_IN_CNT_P_3_OVERFLOWf, print_none_zero, hex_base, cntbuf[0]);
    }
    
    counters_sum_array[0] = FDA_EGQ_N_CELLS_OUT_CNT_FAB_PIPE_20;
    counters_sum_array[1] = FDA_EGQ_N_CELLS_OUT_CNT_FAB_PIPE_21;
    compute_sum_counters_into_string(unit, counters_data, counters_sum_array, 2, jer_counters, print_none_zero, hex_base, cntbuf[1]);

    print_g_pattern_section();
    print_g_section("CELLS_IN_CNT_P3", "CELLS_OUT_CNT_P3", cntbuf);
    print_g_new_line();

    counters_sum_array[0] = FDA_EGQ_N_CELLS_IN_CNT_TDM0;
    counters_sum_array[1] = FDA_EGQ_N_CELLS_IN_CNT_TDM1;
    compute_sum_counters_into_string(unit, counters_data, counters_sum_array, 2, jer_counters, print_none_zero, hex_base, cntbuf[0]);

    counters_sum_array[0] = FDA_EGQ_N_CELLS_OUT_CNT_TDM0;
    counters_sum_array[1] = FDA_EGQ_N_CELLS_OUT_CNT_TDM1;
    compute_sum_counters_into_string(unit, counters_data, counters_sum_array, 2, jer_counters, print_none_zero, hex_base, cntbuf[1]);

    print_g_section_title("IPT");
    print_g_section("CELLS_IN_TDM_CNT", "CELLS_OUT_TDM_CNT", cntbuf);
    print_g_new_line();

    counters_sum_array[0] = FDA_EGQ_N_CELLS_IN_CNT_MESHMC0;
    counters_sum_array[1] = FDA_EGQ_N_CELLS_IN_CNT_MESHMC1;
    compute_sum_counters_into_string(unit, counters_data, counters_sum_array, 2, jer_counters, print_none_zero, hex_base, cntbuf[0]);

    counters_sum_array[0] = FDA_EGQ_N_CELLS_OUT_CNT_MESHMC0;
    counters_sum_array[1] = FDA_EGQ_N_CELLS_OUT_CNT_MESHMC1;
    compute_sum_counters_into_string(unit, counters_data, counters_sum_array, 2, jer_counters, print_none_zero, hex_base, cntbuf[1]);

    print_g_section_title(core_div);
    print_g_section("CELLS_IN_MESHMC_CNT", "CELLS_OUT_MESHMC_CNT", cntbuf);
    print_g_new_line();


    counters_sum_array[0] = FDA_EGQ_N_CELLS_IN_CNT_IPT0;
    counters_sum_array[1] = FDA_EGQ_N_CELLS_IN_CNT_IPT1;
    compute_sum_counters_into_string(unit, counters_data, counters_sum_array, 2, jer_counters, print_none_zero, hex_base, cntbuf[1]);

    counters_sum_array[0] = FDA_EGQ_N_CELLS_OUT_CNT_IPT0;
    counters_sum_array[1] = FDA_EGQ_N_CELLS_OUT_CNT_IPT1;
    compute_sum_counters_into_string(unit, counters_data, counters_sum_array, 2, jer_counters, print_none_zero, hex_base, cntbuf[2]);

    if (!core_disp) {
        counters_sum_array[0] = IPT_EGQ_0_PKT_CNT;
        counters_sum_array[1] = IPT_EGQ_1_PKT_CNT;
        compute_sum_counters_into_string(unit, counters_data, counters_sum_array, 2, jer_counters, print_none_zero, hex_base, cntbuf[0]);
        print_g_section_arrow_1("EGQ_PKT_CNT", NULL, cntbuf);
    } else {

        /*compute_counter_val_into_string(unit, counters_data[IPT_EGQ_0_PKT_CNT], IPT_EGQ_0_PKT_CNTr,
                                        EGQ_0_PKT_CNTf, EGQ_0_PKT_CNT_OVFf, print_none_zero, hex_base, cntbuf[2]);
        print_counter_val_if_neccesary(unit, "|  %-20s %c %-9s ", "EGQ_PKT_CNT", cntbuf[2]);
        compute_counter_val_into_string(unit, counters_data[IPT_EGQ_1_PKT_CNT], IPT_EGQ_1_PKT_CNTr,
                                        EGQ_1_PKT_CNTf, EGQ_1_PKT_CNT_OVFf, print_none_zero, hex_base, cntbuf[2]);
        print_counter_val_if_neccesary(unit, "|  %-20s %c %-9s ", "EGQ_PKT_CNT", cntbuf[2]);*/
        print_g_empty_section();
    }
    print_g_section_arrow_2("CELLS_IN_IPT_CNT", "CELLS_OUT_IPT_CNT", cntbuf + 1);
    print_g_new_line();

    compute_sum_fda_drop_count(unit, counters_data, print_none_zero, hex_base, cntbuf[1]);

    if (!core_disp) {
        counters_sum_array[0] = IPT_ENQ_0_PKT_CNT;
        counters_sum_array[1] = IPT_ENQ_1_PKT_CNT;
        compute_sum_counters_into_string(unit, counters_data, counters_sum_array, 2, jer_counters, print_none_zero, hex_base, cntbuf[0]);
        print_g_line("ENQ_PKT_CNT", NULL, "EGQ_DROP_CNT", NULL, cntbuf);

    } else {
        sal_strcpy(cntbuf[2], cntbuf[1]);
        compute_counter_val_into_string(unit, counters_data[IPT_ENQ_0_PKT_CNT], IPT_ENQ_0_PKT_CNTr,
                                        ENQ_0_PKT_CNTf, ENQ_0_PKT_CNT_OVFf, print_none_zero, hex_base, cntbuf[0]);
        compute_counter_val_into_string(unit, counters_data[IPT_ENQ_1_PKT_CNT], IPT_ENQ_1_PKT_CNTr,
                                        ENQ_1_PKT_CNTf, ENQ_1_PKT_CNT_OVFf, print_none_zero, hex_base, cntbuf[1]);
        print_g_line("ENQ_PKT_CNT", "ENQ_PKT_CNT", "EGQ_DROP_CNT", counter_alignment, cntbuf);
    }

    compute_sum_fda_meshmc_drop_count(unit, counters_data, print_none_zero, hex_base, cntbuf[1]);

    if (!core_disp) {
        counters_sum_array[0] = IPT_FDT_0_PKT_CNT;
        counters_sum_array[1] = IPT_FDT_1_PKT_CNT;
        compute_sum_counters_into_string(unit, counters_data, counters_sum_array, 2, jer_counters, print_none_zero, hex_base, cntbuf[0]);
        print_g_line("FDT_PKT_CNT", NULL, "EGQ_MESHMC_DROP_CNT", NULL, cntbuf);

    } else {
        sal_strcpy(cntbuf[2], cntbuf[1]);
        compute_counter_val_into_string(unit, counters_data[IPT_FDT_0_PKT_CNT], IPT_FDT_0_PKT_CNTr,
                                        FDT_0_PKT_CNTf, FDT_0_PKT_CNT_OVFf, print_none_zero, hex_base, cntbuf[0]);
        compute_counter_val_into_string(unit, counters_data[IPT_FDT_1_PKT_CNT], IPT_FDT_1_PKT_CNTr,
                                        FDT_1_PKT_CNTf, FDT_1_PKT_CNT_OVFf, print_none_zero, hex_base, cntbuf[1]);
        print_g_line("FDT_PKT_CNT", "FDT_PKT_CNT", "EGQ_MESHMC_DROP_CNT", counter_alignment, cntbuf);
    }

    compute_counter_val_into_string(unit, counters_data[IPT_CRC_ERR_CNT], IPT_CRC_ERROR_COUNTERr,
                                       CRC_ERR_CNTf, CRC_ERR_CNT_OVFf, print_none_zero, hex_base, cntbuf[0]);

    counters_sum_array[0] = FDA_EGQ_N_TDM_OVF_DROP_CNT0;
    counters_sum_array[1] = FDA_EGQ_N_TDM_OVF_DROP_CNT1;
    compute_sum_counters_into_string(unit, counters_data, counters_sum_array, 2, jer_counters, print_none_zero, hex_base, cntbuf[1]);
   
    print_g_line("CRC_ERROR_CNT", counter_alignment, "EGQ_TDM_OVF_DROP_CNT", counter_alignment, cntbuf);

    compute_counter_val_into_string(unit, counters_data[IPT_CFG_EVNT_CNT], IPT_CFG_EVENT_CNTr,
                                       CFG_EVENT_CNTf, CFG_EVENT_CNT_OVFf, print_none_zero, hex_base, cntbuf[0]);
    /* Add a mark in case it's not 0 */
    if (sal_strcmp(cntbuf[0], "0 ")) {
        sal_sprintf(cntbuf[0], "%s*", cntbuf[0]);
    }
    print_g_line("CFG_EVENT_CNT", counter_alignment, NULL, NULL, cntbuf);

    compute_counter_val_into_string(unit, counters_data[IPT_CFG_BYT_CNT], IPT_CFG_BYTE_CNTr,
                                       CFG_BYTE_CNTf, CFG_BYTE_CNT_OVFf, print_none_zero, hex_base, cntbuf[0]);
    print_g_line("CFG_BYTE_CNT", counter_alignment, NULL, NULL, cntbuf);


    print_g_pattern_line();
    print_g_title_2("FDT", "FDR");

    compute_counter_val_into_string(unit, counters_data[FDT_DESC_CELL_CNT], FDT_IPT_DESC_CELL_COUNTERr, 
                                    DESC_CELL_CNTf, DESC_CELL_CNTOf, print_none_zero, hex_base, cntbuf[0]); 
    compute_counter_val_into_string(unit, counters_data[FDR_P_1_CELL_IN_CNT], FDR_P_1_CELL_IN_CNTr, 
                                    P_1_CELL_IN_CNTf, P_1_CELL_IN_CNT_Of, print_none_zero, hex_base, cntbuf[1]);

    print_g_line("IPT_DESC_CELL_COUNTER", NULL, "P1_CELL_IN_CNT", NULL, cntbuf);

    compute_counter_val_into_string(unit, counters_data[FDT_IRE_DESC_CELL_CNT], FDT_IRE_DESC_CELL_COUNTERr, 
                                    IRE_DESC_CELL_CNTf, IRE_DESC_CELL_CNTOf, print_none_zero, hex_base, cntbuf[0]); 
    compute_counter_val_into_string(unit, counters_data[FDR_P_2_CELL_IN_CNT], FDR_P_2_CELL_IN_CNTr, 
                                    P_2_CELL_IN_CNTf, P_2_CELL_IN_CNT_Of, print_none_zero, hex_base, cntbuf[1]);

    print_g_line("IRE_DESC_CELL_COUNTER", NULL, "P2_CELL_IN_CNT", NULL, cntbuf);

    compute_counter_val_into_string(unit, counters_data[FDR_P_3_CELL_IN_CNT], FDR_P_3_CELL_IN_CNTr, 
                                    P_3_CELL_IN_CNTf, P_3_CELL_IN_CNT_Of, print_none_zero, hex_base, cntbuf[0]);

    print_g_line(NULL, NULL, "P3_CELL_IN_CNT", NULL, cntbuf);

    compute_counter_val_into_string(unit, counters_data[FDT_DATA_CELL_CNT], FDT_TRANSMITTED_DATA_CELLS_COUNTERr, 
                                    DATA_CELL_CNTf, DATA_CELL_CNT_0f, print_none_zero, hex_base, cntbuf[0]); 
    compute_counter_val_into_string(unit, counters_data[FDR_CELL_IN_CNT], FDR_CELL_IN_CNT_TOTALr, 
                                    CELL_IN_CNTf, CELL_IN_CNT_Of, print_none_zero, hex_base, cntbuf[1]);

    print_g_line("TRANSMITTED_DATA_CELLS_COUNTER", NULL, "CELL_IN_CNT_TOTAL", NULL, cntbuf);

    print_g_pattern_line();
    cli_out("                                            |                                                                      /|\\\n");
    cli_out("                                            |            J E R I C H O   F A B R I C   I N T E R F A C E            |\n");
    cli_out("                                           \\|/                                                                      |\n");

    cli_out("\n\n");

    sal_free(counters_data);
    sal_free(dual_counters_data);
    return CMD_OK;
}

#endif /*BCM_JERICHO_SUPPORT*/

#ifdef BCM_88470_A0

int qax_counters_graphic_print(int unit, uint32 print_none_zero, uint32 hex_base, uint32 core_disp) {

    diag_counter_data_t* qax_counters = qax_counters_for_graphic_display;
    diag_counter_data_t* qax_rqp_counters = qax_rqp_counters_for_graphic_display;
    uint64 counters_data[128];
    uint64 dual_counters_data[128] = {COMPILER_64_INIT(0,0)}; 
    soc_reg_above_64_val_t* rqp_reg = NULL;
    /*soc_reg_above_64_val_t rqp_dual_reg;*/
    uint64 sum_counters;
    /*uint64 dual_sum_counters;*/
    char cntbuf[4][MAX_NUM_LEN];
    qax_graphic_counters_e counters_sum_array[4];
    qax_rqp_graphic_counters_e rqp_counters_sum_array[6];
    uint64 overflow;

    char *core_div = (!core_disp) ? "" : "CORE 0   ------------+------------   CORE 1";
    /* used for alignment in case core display is requested */
    char *counter_alignment = (core_disp)? "" : NULL;
    qax_graphic_counters_e i;

    COMPILER_64_SET(overflow, 0, 0);
    rqp_reg = sal_alloc(128*sizeof(soc_reg_above_64_val_t), "qax_counters_graphic_print.rqp_reg");
    if(rqp_reg == NULL) {
        cli_out("Memory allocation failure\n");
        return CMD_FAIL;
    }
    /*read values of all counters registers into array*/
    for (i = 0; i < QAX_NOF_GRAPHIC_COUNTERS; ++i)
    {
       if (i == QAX_TXQ_EGQ_SRAM_COUNTER ||
           i == QAX_TXQ_EGQ_DRAM_COUNTER ||
           i == QAX_TXQ_FDT_SRAM_COUNTER ||
           i == QAX_TXQ_FDT_DRAM_COUNTER ||
           i == QAX_MMU_IDR_PACKET_COUNTER ||
           i == QAX_SPB_ALMOST_FULL_SRAM_REJECT_COUNTER)
        {
           if (SOC_E_NONE != soc_reg_above_64_get(unit, qax_counters[i].reg, 0, 0, rqp_reg[i])) {
               LOG_ERROR(BSL_LS_APPL_SHELL,(BSL_META_U(unit,
                                            "Failed to read counter %s\n"), qax_counters[i].reg_name));
           }
       }else
       {
           if (SOC_E_NONE != soc_reg_get(unit, qax_counters[i].reg, 0, qax_counters[i].reg_index, &counters_data[i])) {
                   LOG_ERROR(BSL_LS_APPL_SHELL,(BSL_META_U(unit,
                                                 "Failed to read counter: %s\n"), qax_counters[i].reg_name));
           }
       }
       /*if ((qax_counters[i].block == SOC_BLK_IQM) || 
           (qax_counters[i].block == SOC_BLK_EGQ) ||
           (qax_counters[i].block == SOC_BLK_NBIL) ||
           (qax_counters[i].block == SOC_BLK_EPNI)) {
           if (SOC_E_NONE != soc_reg_get(unit, qax_counters[i].reg, 1, qax_counters[i].reg_index, &dual_counters_data[i])) { 
               LOG_ERROR(BSL_LS_APPL_SHELL,
                         (BSL_META_U(unit,
                                     "Failed to read counter: %s\n"), qax_counters[i].reg_name)); 
           }
       }
       if (!COMPILER_64_IS_ZERO(dual_counters_data[i]) && !core_disp) {
           COMPILER_64_ADD_64(counters_data[i], dual_counters_data[i]);
       }*/
    }

    /*read RQP register*/
    if (SOC_E_NONE != soc_reg_above_64_get(unit, EGQ_PRP_DEBUG_COUNTERSr, 0, 0, rqp_reg[0])) {
       LOG_ERROR(BSL_LS_APPL_SHELL,
                 (BSL_META_U(unit,
                             "Failed to read counter EGQ_PRP_DEBUG_COUNTERS\n")));
    }
    /*if (SOC_E_NONE != soc_reg_above_64_get(unit, EGQ_PRP_DEBUG_COUNTERSr, 1, 1, rqp_dual_reg)) {
       LOG_ERROR(BSL_LS_APPL_SHELL,
                 (BSL_META_U(unit,
                             "Failed to read counter EGQ_PRP_DEBUG_COUNTERS\n")));
    }*/

    /*print counters*/
    cli_out("\n");
    cli_out("                                            |                                                                      /|\\\n");
    cli_out("                                            |                   Q A X   N E T W O R K   I N T E R F A C E           |\n");
    cli_out("                                           \\|/                                                                      |\n");
    print_g_pattern_line();
    print_g_title("NBI");

    compute_counter_val_into_string_2(unit, counters_data[QAX_NBIH_RX_TOTAL_BYTE_COUNTER], NBIH_RX_TOTAL_BYTE_COUNTERr, 
                                    RX_TOTAL_BYTE_COUNTERf, 61, print_none_zero, hex_base, cntbuf[0]);
    compute_counter_val_into_string_2(unit, counters_data[QAX_NBIH_TX_TOTAL_BYTE_COUNTER], NBIH_TX_TOTAL_BYTE_COUNTERr, 
                                    TX_TOTAL_BYTE_COUNTERf, 61, print_none_zero, hex_base, cntbuf[1]);

    print_g_line("RX_TOTAL_BYTE_COUNTER", NULL, "TX_TOTAL_BYTE_COUNTER", NULL, cntbuf);

    compute_counter_val_into_string_2(unit, counters_data[QAX_NBIH_RX_TOTAL_PKT_COUNTER], NBIH_RX_TOTAL_PKT_COUNTERr, 
                                    RX_TOTAL_PKT_COUNTERf, 31, print_none_zero, hex_base, cntbuf[0]);
    compute_counter_val_into_string_2(unit, counters_data[QAX_NBIH_TX_TOTAL_PKT_COUNTER], NBIH_TX_TOTAL_PKT_COUNTERr, 
                                    TX_TOTAL_PKT_COUNTERf, 31, print_none_zero, hex_base, cntbuf[1]);

    print_g_line("RX_TOTAL_PKT_COUNTER", NULL, "TX_TOTAL_PKT_COUNTER", NULL, cntbuf);

    COMPILER_64_ZERO(sum_counters);
    COMPILER_64_ADD_64(sum_counters ,counters_data[QAX_NBIH_RX_NUM_TOTAL_DROPPED_EOPS]);
    COMPILER_64_ADD_64(sum_counters ,counters_data[QAX_NBIL_RX_NUM_TOTAL_DROPPED_EOPS]);
    /*COMPILER_64_ADD_64(sum_counters ,dual_counters_data[NBIL_RX_NUM_TOTAL_DROPPED_EOPS]);*/
    compute_counter_val_into_string_2(unit, sum_counters, NBIH_RX_NUM_TOTAL_DROPPED_EOPSr, 
                                    RX_NUM_TOTAL_DROPPED_EOPSf, 31, print_none_zero, hex_base, cntbuf[0]);

    print_g_line("RX_TOTAL_DROPPED_EOPS", NULL, NULL, NULL, cntbuf);

    print_g_pattern_line();
    print_g_title_2("IRE", "EPNI");

    compute_counter_val_into_string(unit, counters_data[QAX_IRE_CPU_PACKET_COUNTER], IRE_CPU_PACKET_COUNTERr, 
                                    CPU_PACKET_COUNTERf, CPU_PACKET_COUNTER_OVFf, print_none_zero, hex_base, cntbuf[0]);

    print_g_line("CPU_PACKET_COUNTER", counter_alignment, NULL, NULL, cntbuf);

    counters_sum_array[0] = QAX_IRE_NIF_PACKET_COUNTER;
    qax_compute_sum_counters_into_string(unit, counters_data, counters_sum_array, 1, qax_counters, print_none_zero, hex_base, cntbuf[0]);
    
    compute_counter_val_into_string_2(unit, counters_data[QAX_EPNI_EPE_BYTES_COUNTER], EPNI_EPE_BYTES_COUNTERr,
                                    EPE_BYTES_COUNTERf, 46, print_none_zero, hex_base, cntbuf[1]);

    print_g_line("NIF_PACKET_COUNTER", NULL, "EPE_BYTES_COUNTER", NULL, cntbuf);

    compute_counter_val_into_string(unit, counters_data[QAX_IRE_OAMP_PACKET_COUNTER], IRE_OAMP_PACKET_COUNTERr,
                                    OAMP_PACKET_COUNTERf, OAMP_PACKET_COUNTER_OVFf, print_none_zero, hex_base, cntbuf[0]); 
    compute_counter_val_into_string_2(unit, counters_data[QAX_EPNI_EPE_PACKET_COUNTER], EPNI_EPE_PACKET_COUNTERr, 
                                    EPE_PACKET_COUNTERf, 32, print_none_zero, hex_base, cntbuf[1]);

    print_g_line("OAMP_PACKET_COUNTER", NULL, "EPE_PKT_COUNTER", NULL, cntbuf);
    
    compute_counter_val_into_string(unit, counters_data[QAX_IRE_OLP_PACKET_COUNTER], IRE_OLP_PACKET_COUNTERr,
                                    OLP_PACKET_COUNTERf, OLP_PACKET_COUNTER_OVFf, print_none_zero, hex_base, cntbuf[0]); 
    compute_counter_val_into_string_2(unit, counters_data[QAX_EPNI_EPE_DISCARDED_PACKETS_COUNTER], EPNI_EPE_DISCARDED_PACKETS_COUNTERr,
                                    EPE_DISCARDED_PACKETS_COUNTERf, 46, print_none_zero, hex_base, cntbuf[1]);
    if (!core_disp) {
        print_g_line("OLP_PACKET_COUNTER", NULL, "EPE_DSCRD_PKT_CNT", NULL, cntbuf);

    } else {
        compute_counter_val_into_string_2(unit, dual_counters_data[QAX_EPNI_EPE_DISCARDED_PACKETS_COUNTER], EPNI_EPE_DISCARDED_PACKETS_COUNTERr,
                                        EPE_DISCARDED_PACKETS_COUNTERf, 46, print_none_zero, hex_base, cntbuf[2]);
        print_g_line("OLP_PACKET_COUNTER", counter_alignment, "EPE_DSCRD_PKT_CNT", "EPE_DSCRD_PKT_CNT", cntbuf);
    }

    /*counters_sum_array[0] = QAX_IRE_RCY_N_PACKET_COUNTER0;
    counters_sum_array[1] = QAX_IRE_RCY_N_PACKET_COUNTER1;
    counters_sum_array[2] = QAX_IRE_RCY_N_PACKET_COUNTER2;
    counters_sum_array[3] = QAX_IRE_RCY_N_PACKET_COUNTER3;
    qax_compute_sum_counters_into_string(unit, counters_data, counters_sum_array, 4, qax_counters, print_none_zero, hex_base, cntbuf[0]);

    print_g_line("RCY_PACKET_COUNTER", NULL, NULL, NULL, cntbuf);*/

    compute_counter_val_into_string(unit, counters_data[QAX_IRE_FDT_INTERFACE_COUNTER], IRE_FDT_INTERFACE_COUNTERr,
                                        FDT_CREDITSf, FDT_CREDITS_OVFf, print_none_zero, hex_base, cntbuf[0]);
    print_g_line("IRE_FDT_INTRFACE_CNT", counter_alignment, NULL, NULL, cntbuf);

    compute_counter_val_into_string(unit, counters_data[QAX_IRE_RCY_PACKET_COUNTER], IRE_RCY_PACKET_COUNTERr,
                                        RCY_PACKET_COUNTERf, RCY_PACKET_COUNTER_OVFf, print_none_zero, hex_base, cntbuf[0]);
    print_g_line("RCY_PACKET_COUNTER", counter_alignment, NULL, NULL, cntbuf);

    print_g_pattern_line();
    print_g_title_2("DDP", "EGQ");
    print_g_title_2("", core_div);

    /*compute_counter_val_into_string_2(unit, counters_data[QAX_MMU_IDR_PACKET_COUNTER], DDP_MMU_COUNTERSr,
                                    MMU_RD_REQ_CNTf, 31, print_none_zero, hex_base, cntbuf[0]);*/

    rqp_counters_sum_array[0] = QAX_MMU_IDR_PACKET_COUNTER;
    qax_compute_sum_rqp_counters32(unit, rqp_reg[QAX_MMU_IDR_PACKET_COUNTER], rqp_counters_sum_array, 1, qax_counters, print_none_zero, hex_base, &sum_counters);
    print_counter_val_to_string(unit, sum_counters, overflow, print_none_zero, hex_base, cntbuf[0]);
    /*compute_counter_val_into_string_2(unit, counters_data[QAX_EGQ_FQP_PACKET_COUNTER], EGQ_FQP_PACKET_COUNTERr,
                                    FQP_PACKET_COUNTERf, 32, print_none_zero, hex_base, cntbuf[1]);*/


    /*print_g_line("MMU_RD_REQ_CNT", NULL, "FQP_PACKET_COUNTER", NULL, cntbuf);*/
    print_g_line("MMU_RD_REQ_CNT", NULL, NULL, NULL, cntbuf);


    /*compute_counter_val_into_string(unit, counters_data[QAX_IDR_OCB_INTERFACE_COUNTER], IDR_OCB_INTERFACE_COUNTERr,
                                    OCB_BUBBLE_REQUESTSf, OCB_BUBBLE_REQUESTS_OVFf, print_none_zero, hex_base, cntbuf[0]);
    compute_counter_val_into_string_2(unit, counters_data[QAX_EGQ_PQP_UNICAST_PACKET_COUNTER], EGQ_PQP_UNICAST_PACKET_COUNTERr,
                                    PQP_UNICAST_PACKET_COUNTERf, 32, print_none_zero, hex_base, cntbuf[1]);

    print_g_line("IDR_OCB_INTERFACE_COUNTER", NULL, "PQP_UNICAST_PKT_CNT", NULL, cntbuf);*/
    print_g_pattern_section();
    print_g_section_title("");
    print_g_new_line();

    print_g_section_title("SPB");
    print_g_new_line();

    rqp_counters_sum_array[0] = QAX_SPB_ALMOST_FULL_SRAM_REJECT_COUNTER;
    qax_compute_sum_rqp_counters64(unit, rqp_reg[QAX_SPB_ALMOST_FULL_SRAM_REJECT_COUNTER], rqp_counters_sum_array, 1, qax_counters, print_none_zero, hex_base, &sum_counters);
    print_counter_val_to_string(unit, sum_counters, overflow, print_none_zero, hex_base, cntbuf[0]);

    compute_counter_val_into_string_2(unit, counters_data[QAX_EGQ_PQP_DISCARD_UNICAST_PACKET_COUNTER], EGQ_PQP_DISCARD_UNICAST_PACKET_COUNTERr,
                                    PQP_DISCARD_UNICAST_PACKET_COUNTERf, 32, print_none_zero, hex_base, cntbuf[1]);

    print_g_line("SPB_AF_SRAM_RJCT_PKT_CNT", NULL, "PQP_DSCRD_UC_PKT_CNT", NULL, cntbuf);

    COMPILER_64_ZERO(sum_counters);
    COMPILER_64_ADD_64(sum_counters ,counters_data[QAX_SPB_ALMOST_FULL_0_SRAM_REJECT_COUNTER]);
    COMPILER_64_ADD_64(sum_counters ,counters_data[QAX_SPB_ALMOST_FULL_1_SRAM_REJECT_COUNTER]);
    COMPILER_64_ADD_64(sum_counters ,counters_data[QAX_SPB_ALMOST_FULL_2_SRAM_REJECT_COUNTER]);
    print_counter_val_to_string(unit, sum_counters, overflow, print_none_zero, hex_base, cntbuf[0]);

    compute_counter_val_into_string_2(unit, counters_data[QAX_EGQ_PQP_UNICAST_BYTES_COUNTER], EGQ_PQP_UNICAST_BYTES_COUNTERr,
                                    PQP_UNICAST_BYTES_COUNTERf, 46, print_none_zero, hex_base, cntbuf[1]);


    print_g_line("SPB_AF_N_SRAM_RJCT_PKT_CNT", NULL, "PQP_UC_BYTES_CNT", NULL, cntbuf);

    compute_counter_val_into_string_2(unit, counters_data[QAX_EGQ_PQP_MULTICAST_PACKET_COUNTER], EGQ_PQP_MULTICAST_PACKET_COUNTERr,
                                    PQP_MULTICAST_PACKET_COUNTERf, 32, print_none_zero, hex_base, cntbuf[0]);

    print_g_pattern_section();
    print_g_section("PQP_MC_PKT_CNT", NULL, cntbuf);
    print_g_new_line();

    compute_counter_val_into_string_2(unit, counters_data[QAX_EGQ_PQP_DISCARD_MULTICAST_PACKET_COUNTER], EGQ_PQP_DISCARD_MULTICAST_PACKET_COUNTERr,
                                    PQP_DISCARD_MULTICAST_PACKET_COUNTERf, 32, print_none_zero, hex_base, cntbuf[0]);

    print_g_section_title("CGM");
    print_g_section("PQP_DSCRD_MC_PKT_CNT", NULL, cntbuf);
    print_g_new_line();

    compute_counter_val_into_string_2(unit, counters_data[QAX_EGQ_PQP_MULTICAST_BYTES_COUNTER], EGQ_PQP_MULTICAST_BYTES_COUNTERr,
                                    PQP_MULTICAST_BYTES_COUNTERf, 46, print_none_zero, hex_base, cntbuf[0]);
    print_g_section_title(core_div);
    print_g_section("PQP_MC_BYTES_CNT", NULL, cntbuf);
    print_g_new_line();

    compute_counter_val_into_string_2(unit, counters_data[QAX_CGM_VOQ_SRAM_ENQ_PACKET_COUNTER], CGM_VOQ_SRAM_ENQ_PKT_CTRr,
                                    VOQ_SRAM_ENQ_PKT_CTRf, 31, print_none_zero, hex_base, cntbuf[0]);
    compute_counter_val_into_string_2(unit, counters_data[QAX_EGQ_EHP_UNICAST_PACKET_COUNTER], EGQ_EHP_UNICAST_PACKET_COUNTERr,
                                    EHP_UNICAST_PACKET_COUNTERf, 32, print_none_zero, hex_base, cntbuf[1]);
    print_g_line("CGM_VOQ_SRAM_ENQ_PKT_CTR", NULL, "EHP_UNICAST_PKT_CNT", NULL, cntbuf);

    compute_counter_val_into_string_2(unit, counters_data[QAX_CGM_VOQ_SRAM_DEQ_PACKET_COUNTER], CGM_VOQ_SRAM_DEQ_PKT_CTRr,
                                    VOQ_SRAM_DEQ_PKT_CTRf, 31, print_none_zero, hex_base, cntbuf[0]);
    compute_counter_val_into_string_2(unit, counters_data[QAX_EGQ_EHP_MULTICAST_HIGH_PACKET_COUNTER], EGQ_EHP_MULTICAST_HIGH_PACKET_COUNTERr,
                                    EHP_MULTICAST_HIGH_PACKET_COUNTERf, 32, print_none_zero, hex_base, cntbuf[1]);
    print_g_line("CGM_VOQ_SRAM_DEQ_PKT_CTR", NULL, "EHP_MC_HIGH_PKT_CNT", NULL, cntbuf);

    compute_counter_val_into_string_2(unit, counters_data[QAX_CGM_VOQ_SRAM_DEL_PACKET_COUNTER], CGM_VOQ_SRAM_DEL_PKT_CTRr,
                                    VOQ_SRAM_DEL_PKT_CTRf, 31, print_none_zero, hex_base, cntbuf[0]);
    compute_counter_val_into_string_2(unit, counters_data[QAX_EGQ_EHP_MULTICAST_LOW_PACKET_COUNTER], EGQ_EHP_MULTICAST_LOW_PACKET_COUNTERr,
                                    EHP_MULTICAST_LOW_PACKET_COUNTERf, 32, print_none_zero, hex_base, cntbuf[1]);
    print_g_line("CGM_VOQ_SRAM_DEL_PKT_CTR", NULL, "EHP_MC_LOW_PKT_CNT", NULL, cntbuf);

    compute_counter_val_into_string_2(unit, counters_data[QAX_CGM_VOQ_SRAM_ENQ_RJCT_PACKET_COUNTER], CGM_VOQ_SRAM_ENQ_RJCT_PKT_CTRr,
                                    VOQ_SRAM_ENQ_RJCT_PKT_CTRf, 31, print_none_zero, hex_base, cntbuf[0]);

    rqp_counters_sum_array[0] = EGQ_PRP_PACKET_DISCARD_TDM_CNT0;
    qax_compute_sum_rqp_counters32(unit, rqp_reg[0], rqp_counters_sum_array, 1, qax_rqp_counters, print_none_zero, hex_base, &sum_counters);
    print_counter_val_to_string(unit, sum_counters, overflow, print_none_zero, hex_base, cntbuf[1]);

    print_g_line("CGM_VOQ_SRAM_ENQ_RJCT_PKT_CTR", NULL, "PRP_PKT_DSCRD_TDM_CNT", NULL, cntbuf);

    COMPILER_64_ZERO(sum_counters);
    /*COMPILER_64_ZERO(dual_sum_counters);*/

    /*rqp_counters_sum_array[0] = QAX_EGQ_PRP_PACKET_GOOD_UC_CNT0;
    rqp_counters_sum_array[1] = QAX_EGQ_PRP_PACKET_GOOD_MC_CNT0;
    rqp_counters_sum_array[2] = QAX_EGQ_PRP_PACKET_GOOD_TDM_CNT0;
    compute_sum_rqp_counters(unit, rqp_reg[0], rqp_counters_sum_array, 3, qax_rqp_counters, print_none_zero, hex_base, &sum_counters);

    COMPILER_64_ADD_64(sum_counters, dual_sum_counters);
    print_counter_val_to_string(unit, sum_counters, overflow, print_none_zero, hex_base, cntbuf[0]);
    print_g_line(NULL, NULL, "RQP_PKT_CNT", NULL, cntbuf);*/

    COMPILER_64_ZERO(sum_counters);
    /*COMPILER_64_ZERO(dual_sum_counters);*/

    /*rqp_counters_sum_array[0] = QAX_EGQ_PRP_PACKET_DISCARD_UC_CNT0;
    rqp_counters_sum_array[1] = QAX_EGQ_PRP_PACKET_DISCARD_MC_CNT0;
    rqp_counters_sum_array[2] = QAX_EGQ_PRP_PACKET_DISCARD_TDM_CNT0;
    rqp_counters_sum_array[3] = QAX_EGQ_PRP_SOP_DISCARD_UC_CNT0;
    rqp_counters_sum_array[4] = QAX_EGQ_PRP_SOP_DISCARD_MC_CNT0;
    rqp_counters_sum_array[5] = QAX_EGQ_PRP_SOP_DISCARD_TDM_CNT0;
    qax_compute_sum_rqp_counters32(unit, rqp_reg[0], rqp_counters_sum_array, 6, qax_rqp_counters, print_none_zero, hex_base, &sum_counters);

    COMPILER_64_ADD_64(sum_counters, dual_sum_counters);
    print_counter_val_to_string(unit, sum_counters, overflow, print_none_zero, hex_base, cntbuf[0]);
    print_g_line(NULL, NULL, "RQP_DSCRD_PKT_CNT", NULL, cntbuf);*/
    compute_counter_val_into_string_2(unit, counters_data[QAX_CGM_VOQ_DRAM_ENQ_PACKET_COUNTER], CGM_VOQ_DRAM_ENQ_BUNDLE_CTRr,
                                    VOQ_DRAM_ENQ_BUNDLE_CTRf, 31, print_none_zero, hex_base, cntbuf[0]);

    rqp_counters_sum_array[0] = EGQ_PRP_SOP_DISCARD_UC_CNT0;
    qax_compute_sum_rqp_counters32(unit, rqp_reg[0], rqp_counters_sum_array, 1, qax_rqp_counters, print_none_zero, hex_base, &sum_counters);
    print_counter_val_to_string(unit, sum_counters, overflow, print_none_zero, hex_base, cntbuf[1]);

    print_g_line("CGM_VOQ_DRAM_ENQ_BUNDLE_CTR", NULL, "PRP_SOP_DSCRD_UC_CNT", NULL, cntbuf);

    compute_counter_val_into_string_2(unit, counters_data[QAX_CGM_VOQ_DRAM_DEQ_PACKET_COUNTER], CGM_VOQ_DRAM_DEQ_PKT_CTRr,
                                    VOQ_DRAM_DEQ_PKT_CTRf, 31, print_none_zero, hex_base, cntbuf[0]);

    rqp_counters_sum_array[0] = EGQ_PRP_SOP_DISCARD_MC_CNT0;
    qax_compute_sum_rqp_counters32(unit, rqp_reg[0], rqp_counters_sum_array, 1, qax_rqp_counters, print_none_zero, hex_base, &sum_counters);
    print_counter_val_to_string(unit, sum_counters, overflow, print_none_zero, hex_base, cntbuf[1]);


    print_g_line("CGM_VOQ_DRAM_DEQ_PKT_CTR", NULL, "PRP_SOP_DSCRD_MC_CNT", NULL, cntbuf);

    compute_counter_val_into_string_2(unit, counters_data[QAX_CGM_VOQ_DRAM_DEL_PACKET_COUNTER], CGM_VOQ_DRAM_DEL_PKT_CTRr,
                                    VOQ_DRAM_DEL_PKT_CTRf, 31, print_none_zero, hex_base, cntbuf[0]);

    rqp_counters_sum_array[0] = EGQ_PRP_SOP_DISCARD_TDM_CNT0;
    qax_compute_sum_rqp_counters32(unit, rqp_reg[0], rqp_counters_sum_array, 1, qax_rqp_counters, print_none_zero, hex_base, &sum_counters);
    print_counter_val_to_string(unit, sum_counters, overflow, print_none_zero, hex_base, cntbuf[1]);


    print_g_line("CGM_VOQ_DRAM_DEL_PKT_CTR", NULL, "PRP_SOP_DSCRD_TDM_CNT", NULL, cntbuf);

    compute_counter_val_into_string_2(unit, counters_data[QAX_EGQ_EHP_DISCARD_PACKET_COUNTER], EGQ_EHP_DISCARD_PACKET_COUNTERr,
                                    EHP_DISCARD_PACKET_COUNTERf, 32, print_none_zero, hex_base, cntbuf[0]);

    print_g_line(NULL, NULL, "EHP_DISCARD_PACKET_COUNTER", NULL, cntbuf);

    diag_print_iqm_rejects_and_erpp_discards(unit, FALSE);


    print_g_pattern_line();
    print_g_title_2("", "FDA");

    /*compute_counter_val_into_string(unit, counters_data[QAX_FDA_FAB_36_SCH_CELLS_IN_CNT_P_1], FDA_FAB_36_SCH_CELLS_IN_CNT_P_1r, 
                                    FAB_36_SCH_CELLS_IN_CNT_P_1f, FAB_36_SCH_CELLS_IN_CNT_P_1_OVERFLOWf, print_none_zero, hex_base, cntbuf[0]);

    counters_sum_array[0] = QAX_FDA_EGQ_N_CELLS_OUT_CNT_FAB_PIPE_0;
    qax_compute_sum_counters_into_string(unit, counters_data, counters_sum_array, 1, qax_counters, print_none_zero, hex_base, cntbuf[1]);

    print_g_line(NULL, NULL, "CELLS_IN_CNT_P1", "CELLS_OUT_CNT_P1", cntbuf);

    compute_counter_val_into_string(unit, counters_data[QAX_FDA_FAB_36_SCH_CELLS_IN_CNT_P_2], FDA_FAB_36_SCH_CELLS_IN_CNT_P_2r, 
                                    FAB_36_SCH_CELLS_IN_CNT_P_2f, FAB_36_SCH_CELLS_IN_CNT_P_2_OVERFLOWf, print_none_zero, hex_base, cntbuf[0]);

    counters_sum_array[0] = QAX_FDA_EGQ_N_CELLS_OUT_CNT_FAB_PIPE_10;
    counters_sum_array[1] = QAX_FDA_EGQ_N_CELLS_OUT_CNT_FAB_PIPE_11;
    qax_compute_sum_counters_into_string(unit, counters_data, counters_sum_array, 2, qax_counters, print_none_zero, hex_base, cntbuf[1]);


    print_g_line(NULL, NULL, "CELLS_IN_CNT_P2", "CELLS_OUT_CNT_P2", cntbuf);

    compute_counter_val_into_string(unit, counters_data[QAX_FDA_FAB_36_SCH_CELLS_IN_CNT_P_3], FDA_FAB_36_SCH_CELLS_IN_CNT_P_3r, 
                                    FAB_36_SCH_CELLS_IN_CNT_P_3f, FAB_36_SCH_CELLS_IN_CNT_P_3_OVERFLOWf, print_none_zero, hex_base, cntbuf[0]);

    counters_sum_array[0] = QAX_FDA_EGQ_N_CELLS_OUT_CNT_FAB_PIPE_20;
    counters_sum_array[1] = QAX_FDA_EGQ_N_CELLS_OUT_CNT_FAB_PIPE_21;
    qax_compute_sum_counters_into_string(unit, counters_data, counters_sum_array, 2, qax_counters, print_none_zero, hex_base, cntbuf[1]);

    print_g_pattern_section();
    print_g_section("CELLS_IN_CNT_P3", "CELLS_OUT_CNT_P3", cntbuf);
    print_g_new_line();*/

    counters_sum_array[0] = QAX_FDA_EGQ_N_CELLS_IN_CNT_TDM;
    qax_compute_sum_counters_into_string(unit, counters_data, counters_sum_array, 1, qax_counters, print_none_zero, hex_base, cntbuf[0]);

    counters_sum_array[0] = QAX_FDA_EGQ_N_CELLS_OUT_CNT_TDM;
    qax_compute_sum_counters_into_string(unit, counters_data, counters_sum_array, 1, qax_counters, print_none_zero, hex_base, cntbuf[1]);

    print_g_section_title("TXQ");
    print_g_section("CELLS_IN_TDM_CNT", "CELLS_OUT_TDM_CNT", cntbuf);
    print_g_new_line();

    counters_sum_array[0] = QAX_FDA_EGQ_N_CELLS_IN_CNT_MESHMC;
    qax_compute_sum_counters_into_string(unit, counters_data, counters_sum_array, 1, qax_counters, print_none_zero, hex_base, cntbuf[0]);

    counters_sum_array[0] = QAX_FDA_EGQ_N_CELLS_OUT_CNT_MESHMC;
    qax_compute_sum_counters_into_string(unit, counters_data, counters_sum_array, 1, qax_counters, print_none_zero, hex_base, cntbuf[1]);

    print_g_section_title(core_div);
    print_g_section("CELLS_IN_MESHMC_CNT", "CELLS_OUT_MESHMC_CNT", cntbuf);
    print_g_new_line();


    counters_sum_array[0] = QAX_FDA_EGQ_N_CELLS_IN_CNT_IPT;
    qax_compute_sum_counters_into_string(unit, counters_data, counters_sum_array, 1, qax_counters, print_none_zero, hex_base, cntbuf[2]);

    counters_sum_array[0] = QAX_FDA_EGQ_N_CELLS_OUT_CNT_IPT;
    qax_compute_sum_counters_into_string(unit, counters_data, counters_sum_array, 1, qax_counters, print_none_zero, hex_base, cntbuf[3]);

    /*compute_counter_val_into_string_2(unit, counters_data[QAX_TXQ_EGQ_SRAM_COUNTER], TXQ_EGQ_COUNTERr,
                                    LOC_SRAM_PACKET_CNTf, 31, print_none_zero, hex_base, cntbuf[0]);*/
    rqp_counters_sum_array[0] = QAX_TXQ_EGQ_SRAM_COUNTER;
    qax_compute_sum_rqp_counters32(unit, rqp_reg[QAX_TXQ_EGQ_SRAM_COUNTER], rqp_counters_sum_array, 1, qax_counters, print_none_zero, hex_base, &sum_counters);
    print_counter_val_to_string(unit, sum_counters, overflow, print_none_zero, hex_base, cntbuf[0]);
    /*compute_counter_val_into_string_2(unit, counters_data[QAX_TXQ_EGQ_DRAM_COUNTER], TXQ_EGQ_COUNTERr,
                                    LOC_DRAM_PACKET_CNTf, 31, print_none_zero, hex_base, cntbuf[1]);*/
    rqp_counters_sum_array[0] = QAX_TXQ_EGQ_DRAM_COUNTER;
    qax_compute_sum_rqp_counters32(unit, rqp_reg[QAX_TXQ_EGQ_DRAM_COUNTER], rqp_counters_sum_array, 1, qax_counters, print_none_zero, hex_base, &sum_counters);
    print_counter_val_to_string(unit, sum_counters, overflow, print_none_zero, hex_base, cntbuf[1]);
    print_g_section_arrow_1("EGQ_SRAM_PKT_CNT", "EGQ_DRAM_PKT_CNT", cntbuf);
    print_g_section_arrow_2("CELLS_IN_IPT_CNT", "CELLS_OUT_IPT_CNT", cntbuf + 2);
    print_g_new_line();

    /*compute_sum_fda_drop_count(unit, counters_data, print_none_zero, hex_base, cntbuf[1]);*/

    /*counters_sum_array[0] = QAX_IPT_ENQ_0_PKT_CNT;
    counters_sum_array[1] = QAX_IPT_ENQ_1_PKT_CNT;
    qax_compute_sum_counters_into_string(unit, counters_data, counters_sum_array, 2, qax_counters, print_none_zero, hex_base, cntbuf[0]);
    print_g_line("ENQ_PKT_CNT", NULL, "EGQ_DROP_CNT", NULL, cntbuf);*/

    /*compute_sum_fda_meshmc_drop_count(unit, counters_data, print_none_zero, hex_base, cntbuf[1]);*/

    /*compute_counter_val_into_string_2(unit, counters_data[QAX_TXQ_FDT_SRAM_COUNTER], TXQ_FDT_COUNTERr,
                                    PRG_0_PACKET_CNTf, 31, print_none_zero, hex_base, cntbuf[0]);*/
    rqp_counters_sum_array[0] = QAX_TXQ_FDT_SRAM_COUNTER;
    qax_compute_sum_rqp_counters32(unit, rqp_reg[QAX_TXQ_FDT_SRAM_COUNTER], rqp_counters_sum_array, 1, qax_counters, print_none_zero, hex_base, &sum_counters);
    print_counter_val_to_string(unit, sum_counters, overflow, print_none_zero, hex_base, cntbuf[0]);
    /*compute_counter_val_into_string_2(unit, counters_data[QAX_TXQ_FDT_DRAM_COUNTER], TXQ_FDT_COUNTERr,
                                    PRG_1_PACKET_CNTf, 31, print_none_zero, hex_base, cntbuf[1]);*/
    rqp_counters_sum_array[0] = QAX_TXQ_FDT_DRAM_COUNTER;
    qax_compute_sum_rqp_counters32(unit, rqp_reg[QAX_TXQ_FDT_DRAM_COUNTER], rqp_counters_sum_array, 1, qax_counters, print_none_zero, hex_base, &sum_counters);
    print_counter_val_to_string(unit, sum_counters, overflow, print_none_zero, hex_base, cntbuf[1]);
    /*print_g_line("FDT_PKT_CNT", NULL, "EGQ_MESHMC_DROP_CNT", NULL, cntbuf);*/
    print_g_line("FDT_SRAM_PKT_CNT", "FDT_DRAM_PKT_CNT", NULL, NULL, cntbuf);

    /*compute_counter_val_into_string(unit, counters_data[QAX_IPT_CRC_ERR_CNT], IPT_CRC_ERROR_COUNTERr,
                                       CRC_ERR_CNTf, CRC_ERR_CNT_OVFf, print_none_zero, hex_base, cntbuf[0]);*/

    counters_sum_array[0] = QAX_FDA_EGQ_N_TDM_OVF_DROP_CNT;
    qax_compute_sum_counters_into_string(unit, counters_data, counters_sum_array, 1, qax_counters, print_none_zero, hex_base, cntbuf[0]);

    /*print_g_line("CRC_ERROR_CNT", counter_alignment, "EGQ_TDM_OVF_DROP_CNT", counter_alignment, cntbuf);*/

    print_g_line(NULL, counter_alignment, "EGQ_TDM_OVF_DROP_CNT", counter_alignment, cntbuf);
    /*compute_counter_val_into_string(unit, counters_data[QAX_IPT_CFG_EVNT_CNT], IPT_CFG_EVENT_CNTr,
                                       CFG_EVENT_CNTf, CFG_EVENT_CNT_OVFf, print_none_zero, hex_base, cntbuf[0]);*/
    /* Add a mark in case it's not 0 */
    /*if (sal_strcmp(cntbuf[0], "0 ")) {
        sal_sprintf(cntbuf[0], "%s*", cntbuf[0]);
    }
    print_g_line("CFG_EVENT_CNT", counter_alignment, NULL, NULL, cntbuf);

    compute_counter_val_into_string(unit, counters_data[QAX_IPT_CFG_BYT_CNT], IPT_CFG_BYTE_CNTr,
                                       CFG_BYTE_CNTf, CFG_BYTE_CNT_OVFf, print_none_zero, hex_base, cntbuf[0]);
    print_g_line("CFG_BYTE_CNT", counter_alignment, NULL, NULL, cntbuf);*/


    print_g_pattern_line();
    print_g_title_2("FDT", "FDR");

    compute_counter_val_into_string(unit, counters_data[QAX_FDT_DESC_CELL_CNT], FDT_IPT_DESC_CELL_COUNTERr,
                                    DESC_CELL_CNTf, DESC_CELL_CNTOf, print_none_zero, hex_base, cntbuf[0]);
    compute_counter_val_into_string(unit, counters_data[QAX_FDR_P_1_CELL_IN_CNT], FDR_P_1_CELL_IN_CNTr,
                                    P_1_CELL_IN_CNTf, P_1_CELL_IN_CNT_Of, print_none_zero, hex_base, cntbuf[1]);

    print_g_line("IPT_DESC_CELL_COUNTER", NULL, "P1_CELL_IN_CNT", NULL, cntbuf);

    compute_counter_val_into_string(unit, counters_data[QAX_FDT_IRE_DESC_CELL_CNT], FDT_IRE_DESC_CELL_COUNTERr,
                                    IRE_DESC_CELL_CNTf, IRE_DESC_CELL_CNTOf, print_none_zero, hex_base, cntbuf[0]);
    compute_counter_val_into_string(unit, counters_data[QAX_FDR_P_2_CELL_IN_CNT], FDR_P_2_CELL_IN_CNTr,
                                    P_2_CELL_IN_CNTf, P_2_CELL_IN_CNT_Of, print_none_zero, hex_base, cntbuf[1]);

    print_g_line("IRE_DESC_CELL_COUNTER", NULL, "P2_CELL_IN_CNT", NULL, cntbuf);

    compute_counter_val_into_string(unit, counters_data[QAX_FDR_P_3_CELL_IN_CNT], FDR_P_3_CELL_IN_CNTr,
                                    P_3_CELL_IN_CNTf, P_3_CELL_IN_CNT_Of, print_none_zero, hex_base, cntbuf[0]);

    print_g_line(NULL, NULL, "P3_CELL_IN_CNT", NULL, cntbuf);

    compute_counter_val_into_string(unit, counters_data[QAX_FDT_DATA_CELL_CNT], FDT_TRANSMITTED_DATA_CELLS_COUNTERr,
                                    DATA_CELL_CNTf, DATA_CELL_CNT_0f, print_none_zero, hex_base, cntbuf[0]);
    compute_counter_val_into_string(unit, counters_data[QAX_FDR_CELL_IN_CNT], FDR_CELL_IN_CNT_TOTALr,
                                    CELL_IN_CNTf, CELL_IN_CNT_Of, print_none_zero, hex_base, cntbuf[1]);

    print_g_line("TRANSMITTED_DATA_CELLS_COUNTER", NULL, "CELL_IN_CNT_TOTAL", NULL, cntbuf);

    print_g_pattern_line();
    cli_out("                                            |                                                                      /|\\\n");
    cli_out("                                            |                    Q A X   F A B R I C   I N T E R F A C E            |\n");
    cli_out("                                           \\|/                                                                      |\n");

    cli_out("\n\n");
    sal_free(rqp_reg);
    return CMD_OK;
}

#endif /*BCM_88470_A0*/

#ifdef BCM_QUX_SUPPORT
int qux_counters_graphic_print(int unit, uint32 print_none_zero, uint32 hex_base, uint32 core_disp)
{
    diag_counter_data_t* qux_counters = qux_counters_for_graphic_display;
    diag_counter_data_t* qux_rqp_counters = qux_rqp_counters_for_graphic_display;
    uint64 counters_data[128];
    soc_reg_above_64_val_t* rqp_reg = NULL;
    uint64 sum_counters;
    char cntbuf[4][MAX_NUM_LEN];
    qax_graphic_counters_e counters_sum_array[4];
    qax_rqp_graphic_counters_e rqp_counters_sum_array[6];
    uint64 overflow;

    /** core_disp always is 0*/
    /* used for alignment in case core display is requested */
    char *counter_alignment = (core_disp)? "" : NULL;
    qux_graphic_counters_e i;

    COMPILER_64_SET(overflow, 0, 0);
    rqp_reg = sal_alloc(128*sizeof(soc_reg_above_64_val_t), "qux_counters_graphic_print.rqp_reg");
    if(rqp_reg == NULL)
    {
        cli_out("Memory allocation failure\n");
        return CMD_FAIL;
    }
    /*read values of all counters registers into array*/
    for (i = 0; i < QUX_NOF_GRAPHIC_COUNTERS; ++i)
    {
        switch (i)
        {
            case QUX_TXQ_EGQ_SRAM_COUNTER:
            case QUX_TXQ_EGQ_DRAM_COUNTER:
            case QUX_SPB_ALMOST_FULL_SRAM_REJECT_COUNTER:
            case QUX_MMU_RD_REQ_COUNTER:
            case QUX_MMU_WR_REQ_COUNTER:
            case QUX_MMU_SPB_GOOD_PACKET_CNT:
            case QUX_MMU_SPB_PACKET_ERROR_CNT:
            case QUX_MMU_DRAM_GOOD_PACKET_CNT:
            case QUX_MMU_DRAM_PACKET_CRC_ERROR_CNT:
            case QUX_MMU_DRAM_PACKET_PKUP_DISCARD_CNT:
            {
                if (SOC_E_NONE != soc_reg_above_64_get(unit, qux_counters[i].reg, 0, 0, rqp_reg[i]))
                {
                   LOG_ERROR(BSL_LS_APPL_SHELL, (BSL_META_U(unit, "Failed to read counter %s\n"), qux_counters[i].reg_name));
                }
                break;
            }
			default:
            {
               if (SOC_E_NONE != soc_reg_get(unit, qux_counters[i].reg, 0, qux_counters[i].reg_index, &counters_data[i]))
               {
                   LOG_ERROR(BSL_LS_APPL_SHELL,(BSL_META_U(unit, "Failed to read counter: %s\n"), qux_counters[i].reg_name));
               }
               break;
            }
        }
    }
    /*read RQP register*/
    if (SOC_E_NONE != soc_reg_above_64_get(unit, EGQ_RQP_DEBUG_COUNTERSr, 0, 0, rqp_reg[0]))
    {
        LOG_ERROR(BSL_LS_APPL_SHELL, (BSL_META_U(unit,
                                      "Failed to read counter EGQ_PQP_DEBUG_COUNTERS\n")));
    }
    /*print counters*/
    cli_out("\n");
    cli_out("                                            |                                                                      /|\\\n");
    cli_out("                                            |                   Q U X   N E T W O R K   I N T E R F A C E           |\n");
    cli_out("                                           \\|/                                                                      |\n");
    print_g_pattern_line();
    print_g_title("NBI");

    compute_counter_val_into_string_2(unit, counters_data[QUX_NIF_RX_TOTAL_BYTE_COUNTER], NIF_RX_TOTAL_BYTE_COUNTERr,
                                      RX_TOTAL_BYTE_COUNTERf, 61, print_none_zero, hex_base, cntbuf[0]);
    compute_counter_val_into_string_2(unit, counters_data[QUX_NIF_TX_TOTAL_BYTE_COUNTER], NIF_TX_TOTAL_BYTE_COUNTERr,
                                      TX_TOTAL_BYTE_COUNTERf, 61, print_none_zero, hex_base, cntbuf[1]);

    print_g_line("RX_TOTAL_BYTE_COUNTER", NULL, "TX_TOTAL_BYTE_COUNTER", NULL, cntbuf);

    compute_counter_val_into_string_2(unit, counters_data[QUX_NIF_RX_TOTAL_PKT_COUNTER], NIF_RX_TOTAL_PKT_COUNTERr,
                                      RX_TOTAL_PKT_COUNTERf, 31, print_none_zero, hex_base, cntbuf[0]);
    compute_counter_val_into_string_2(unit, counters_data[QUX_NIF_TX_TOTAL_PKT_COUNTER], NIF_TX_TOTAL_PKT_COUNTERr,
                                      TX_TOTAL_PKT_COUNTERf, 31, print_none_zero, hex_base, cntbuf[1]);

    print_g_line("RX_TOTAL_PKT_COUNTER", NULL, "TX_TOTAL_PKT_COUNTER", NULL, cntbuf);

    compute_counter_val_into_string_2(unit, counters_data[QUX_NIF_RX_NUM_TOTAL_DROPPED_EOPS], NIF_RX_NUM_TOTAL_DROPPED_EOPSr,
                                      RX_NUM_TOTAL_DROPPED_EOPSf, 31, print_none_zero, hex_base, cntbuf[0]);

    print_g_line("RX_TOTAL_DROPPED_EOPS", NULL, NULL, NULL, cntbuf);
    /** the second */
    print_g_pattern_line();
    print_g_title_2("IRE", "EPNI");
    compute_counter_val_into_string(unit, counters_data[QUX_IRE_CPU_PACKET_COUNTER], IRE_CPU_PACKET_COUNTERr,
                                    CPU_PACKET_COUNTERf, CPU_PACKET_COUNTER_OVFf, print_none_zero, hex_base, cntbuf[0]);

    print_g_line("CPU_PACKET_COUNTER", counter_alignment, NULL, NULL, cntbuf);

    counters_sum_array[0] = QUX_IRE_NIF_PACKET_COUNTER;
    qax_compute_sum_counters_into_string(unit, counters_data, counters_sum_array, 1, qux_counters, print_none_zero, hex_base, cntbuf[0]);

    compute_counter_val_into_string_2(unit, counters_data[QUX_EPNI_EPE_BYTES_COUNTER], EPNI_EPE_BYTES_COUNTERr,
                                      EPE_BYTES_COUNTERf, 46, print_none_zero, hex_base, cntbuf[1]);

    print_g_line("NIF_PACKET_COUNTER", NULL, "EPE_BYTES_COUNTER", NULL, cntbuf);

    compute_counter_val_into_string(unit, counters_data[QUX_IRE_OAMP_PACKET_COUNTER], IRE_OAMP_PACKET_COUNTERr,
                                    OAMP_PACKET_COUNTERf, OAMP_PACKET_COUNTER_OVFf, print_none_zero, hex_base, cntbuf[0]); 
    compute_counter_val_into_string_2(unit, counters_data[QUX_EPNI_EPE_PACKET_COUNTER], EPNI_EPE_PACKET_COUNTERr,
                                      EPE_PACKET_COUNTERf, 32, print_none_zero, hex_base, cntbuf[1]);

    print_g_line("OAMP_PACKET_COUNTER", NULL, "EPE_PKT_COUNTER", NULL, cntbuf);

    compute_counter_val_into_string(unit, counters_data[QUX_IRE_OLP_PACKET_COUNTER], IRE_OLP_PACKET_COUNTERr,
                                     OLP_PACKET_COUNTERf, OLP_PACKET_COUNTER_OVFf, print_none_zero, hex_base, cntbuf[0]); 
    compute_counter_val_into_string_2(unit, counters_data[QUX_EPNI_EPE_DISCARDED_PACKETS_COUNTER], EPNI_EPE_DISCARDED_PACKETS_COUNTERr,
                                      EPE_DISCARDED_PACKETS_COUNTERf, 46, print_none_zero, hex_base, cntbuf[1]);
    print_g_line("OLP_PACKET_COUNTER", NULL, "EPE_DSCRD_PKT_CNT", NULL, cntbuf);


    compute_counter_val_into_string(unit, counters_data[QUX_IRE_RCY_PACKET_COUNTER], IRE_RCY_PACKET_COUNTERr,
                                    RCY_PACKET_COUNTERf, RCY_PACKET_COUNTER_OVFf, print_none_zero, hex_base, cntbuf[0]);
    compute_counter_val_into_string_2(unit, counters_data[QUX_EPNI_EPE_MIRRORED_PACKETS_COUNTER], EPNI_EPE_MIRRORED_PACKETS_COUNTERr,
                                      EPE_MIRRORED_PACKETS_COUNTERf, 32, print_none_zero, hex_base, cntbuf[1]);
    print_g_line("RCY_PACKET_COUNTER", counter_alignment, "EPE_MIRRORED_PKT_CNT", NULL, cntbuf);

    compute_counter_val_into_string(unit, counters_data[QUX_IRE_IPSEC_PACKET_COUNTER], IRE_IPSEC_PACKET_COUNTERr,
                                    IPSEC_PACKET_COUNTERf, IPSEC_PACKET_COUNTER_OVFf, print_none_zero, hex_base, cntbuf[0]);
    print_g_line("IPSEC_PACKET_COUNTER", counter_alignment, NULL, NULL, cntbuf);

    compute_counter_val_into_string(unit, counters_data[QUX_IRE_SAT_PACKET_COUNTER], IRE_SAT_PACKET_COUNTERr,
                                    SAT_PACKET_COUNTERf, SAT_PACKET_COUNTER_OVFf, print_none_zero, hex_base, cntbuf[0]);
    print_g_line("SAT_PACKET_COUNTER", counter_alignment, NULL, NULL, cntbuf);

    print_g_pattern_line();
    print_g_title_2("DDP", "EGQ");

    rqp_counters_sum_array[0] = QUX_MMU_RD_REQ_COUNTER;
    qax_compute_sum_rqp_counters32(unit, rqp_reg[QUX_MMU_RD_REQ_COUNTER], rqp_counters_sum_array, 1, qux_counters, print_none_zero, hex_base, &sum_counters);
    print_counter_val_to_string(unit, sum_counters, overflow, print_none_zero, hex_base, cntbuf[0]);
    compute_counter_val_into_string_2(unit, counters_data[QUX_EGQ_FQP_CELL_COUNTER], EGQ_FQP_CELL_COUNTERr,
                                      FQP_CELL_COUNTERf, 32, print_none_zero, hex_base, cntbuf[1]);
    print_g_line("MMU_RD_REQ_CNT", NULL, "FQP_CELL_CNT", NULL, cntbuf);

    rqp_counters_sum_array[0] = QUX_MMU_WR_REQ_COUNTER;
    qax_compute_sum_rqp_counters32(unit, rqp_reg[QUX_MMU_WR_REQ_COUNTER], rqp_counters_sum_array, 1, qux_counters, print_none_zero, hex_base, &sum_counters);
    print_counter_val_to_string(unit, sum_counters, overflow, print_none_zero, hex_base, cntbuf[0]);
    compute_counter_val_into_string_2(unit, counters_data[QUX_EGQ_FQP_PACKET_COUNTER], EGQ_FQP_PACKET_COUNTERr,
                                      FQP_PACKET_COUNTERf, 32, print_none_zero, hex_base, cntbuf[1]);
    print_g_line("MMU_WR_REQ_CNT", NULL, "FQP_PACKET_CNT", NULL, cntbuf);

    rqp_counters_sum_array[0] = QUX_MMU_SPB_GOOD_PACKET_CNT;
    qax_compute_sum_rqp_counters32(unit, rqp_reg[QUX_MMU_SPB_GOOD_PACKET_CNT], rqp_counters_sum_array, 1, qux_counters, print_none_zero, hex_base, &sum_counters);
    print_counter_val_to_string(unit, sum_counters, overflow, print_none_zero, hex_base, cntbuf[0]);
    print_g_line("SPB_GOOD_PACKET_CNT", NULL, NULL, NULL, cntbuf);

    rqp_counters_sum_array[0] = QUX_MMU_SPB_PACKET_ERROR_CNT;
    qax_compute_sum_rqp_counters32(unit, rqp_reg[QUX_MMU_SPB_PACKET_ERROR_CNT], rqp_counters_sum_array, 1, qux_counters, print_none_zero, hex_base, &sum_counters);
    print_counter_val_to_string(unit, sum_counters, overflow, print_none_zero, hex_base, cntbuf[0]);
    print_g_line("SPB_PACKET_ERROR_CNT", NULL, NULL, NULL, cntbuf);

    rqp_counters_sum_array[0] = QUX_MMU_DRAM_GOOD_PACKET_CNT;
    qax_compute_sum_rqp_counters32(unit, rqp_reg[QUX_MMU_DRAM_GOOD_PACKET_CNT], rqp_counters_sum_array, 1, qux_counters, print_none_zero, hex_base, &sum_counters);
    print_counter_val_to_string(unit, sum_counters, overflow, print_none_zero, hex_base, cntbuf[0]);
    compute_counter_val_into_string_2(unit, counters_data[QUX_EGQ_PQP_DISCARD_MULTICAST_PACKET_COUNTER], EGQ_PQP_DISCARD_MULTICAST_PACKET_COUNTERr,
                                      PQP_DISCARD_MULTICAST_PACKET_COUNTERf, 32, print_none_zero, hex_base, cntbuf[1]);
    print_g_line("DRAM_GOOD_PACKET_CNT", NULL, "PQP_DSCRD_MC_PKT_CNT", NULL, cntbuf);

    rqp_counters_sum_array[0] = QUX_MMU_DRAM_PACKET_CRC_ERROR_CNT;
    qax_compute_sum_rqp_counters32(unit, rqp_reg[QUX_MMU_DRAM_PACKET_CRC_ERROR_CNT], rqp_counters_sum_array, 1, qux_counters, print_none_zero, hex_base, &sum_counters);
    print_counter_val_to_string(unit, sum_counters, overflow, print_none_zero, hex_base, cntbuf[0]);
    compute_counter_val_into_string_2(unit, counters_data[QUX_EGQ_PQP_DISCARD_UNICAST_PACKET_COUNTER], EGQ_PQP_DISCARD_UNICAST_PACKET_COUNTERr,
                                      PQP_DISCARD_UNICAST_PACKET_COUNTERf, 32, print_none_zero, hex_base, cntbuf[1]);
    print_g_line("DRAM_PACKET_CRC_ERROR_CNT", NULL, "PQP_DSCRD_UC_PKT_CNT", NULL, cntbuf);

    rqp_counters_sum_array[0] = QUX_MMU_DRAM_PACKET_PKUP_DISCARD_CNT;
    qax_compute_sum_rqp_counters32(unit, rqp_reg[QUX_MMU_DRAM_PACKET_PKUP_DISCARD_CNT], rqp_counters_sum_array, 1, qux_counters, print_none_zero, hex_base, &sum_counters);
    print_counter_val_to_string(unit, sum_counters, overflow, print_none_zero, hex_base, cntbuf[0]);
    compute_counter_val_into_string_2(unit, counters_data[QUX_EGQ_PQP_MULTICAST_BYTES_COUNTER], EGQ_PQP_MULTICAST_BYTES_COUNTERr,
                                      PQP_MULTICAST_BYTES_COUNTERf, 46, print_none_zero, hex_base, cntbuf[1]);
    print_g_line("DRAM_PACKET_PKUP_DISCARD_CNT", NULL, "PQP_MC_BYTES_CNT", NULL, cntbuf);

    compute_counter_val_into_string_2(unit, counters_data[QUX_EGQ_PQP_MULTICAST_PACKET_COUNTER], EGQ_PQP_MULTICAST_PACKET_COUNTERr,
                                      PQP_MULTICAST_PACKET_COUNTERf, 32, print_none_zero, hex_base, cntbuf[0]);
    print_g_pattern_section();
    print_g_section("PQP_MC_PKT_CNT", NULL, cntbuf);
    print_g_new_line();

    compute_counter_val_into_string_2(unit, counters_data[QUX_EGQ_PQP_UNICAST_BYTES_COUNTER], EGQ_PQP_UNICAST_BYTES_COUNTERr,
                                      PQP_UNICAST_BYTES_COUNTERf, 46, print_none_zero, hex_base, cntbuf[0]);
    print_g_section_title("SPB");
    print_g_section("PQP_UC_BYTES_CNT", NULL, cntbuf);
    print_g_new_line();


    COMPILER_64_ZERO(sum_counters);
    COMPILER_64_ADD_64(sum_counters ,counters_data[QUX_SPB_DISCARDED_PACKETS_CNT_0]);
    COMPILER_64_ADD_64(sum_counters ,counters_data[QUX_SPB_DISCARDED_PACKETS_CNT_1]);
    COMPILER_64_ADD_64(sum_counters ,counters_data[QUX_SPB_DISCARDED_PACKETS_CNT_2]);
    COMPILER_64_ADD_64(sum_counters ,counters_data[QUX_SPB_DISCARDED_PACKETS_CNT_3]);
    print_counter_val_to_string(unit, sum_counters, overflow, print_none_zero, hex_base, cntbuf[0]);
    compute_counter_val_into_string_2(unit, counters_data[QUX_EGQ_PQP_UNICAST_PACKET_COUNTER], EGQ_PQP_UNICAST_PACKET_COUNTERr,
                                      PQP_UNICAST_PACKET_COUNTERf, 32, print_none_zero, hex_base, cntbuf[0]);
    print_g_line("DISCARDED_PACKETS_N_CNT", NULL, "PQP_UC_PKT_CNT", NULL, cntbuf);

    print_counter_val_to_string(unit, counters_data[QUX_SPB_FULL_SRAM_REJECT_COUNTER], overflow, print_none_zero, hex_base, cntbuf[0]);
    print_g_line("SPB_FULL_SRAM_REJECT_PCK_CNT", NULL, NULL, NULL, cntbuf);

    rqp_counters_sum_array[0] = QUX_SPB_ALMOST_FULL_SRAM_REJECT_COUNTER;
    qax_compute_sum_rqp_counters64(unit, rqp_reg[QUX_SPB_ALMOST_FULL_SRAM_REJECT_COUNTER], rqp_counters_sum_array, 1, qux_counters, print_none_zero, hex_base, &sum_counters);
    print_counter_val_to_string(unit, sum_counters, overflow, print_none_zero, hex_base, cntbuf[0]);
    compute_counter_val_into_string_2(unit, counters_data[QUX_EGQ_EHP_DISCARD_PACKET_COUNTER], EGQ_EHP_DISCARD_PACKET_COUNTERr,
                                      EHP_DISCARD_PACKET_COUNTERf, 32, print_none_zero, hex_base, cntbuf[1]);
    print_g_line("SPB_ALMOST_FULL_SRAM_RJCT_PKT_CNT", NULL, "EHP_DSCRD_PKT_CNT", NULL, cntbuf);

    COMPILER_64_ZERO(sum_counters);
    COMPILER_64_ADD_64(sum_counters ,counters_data[QUX_SPB_ALMOST_FULL_0_SRAM_REJECT_COUNTER]);
    COMPILER_64_ADD_64(sum_counters ,counters_data[QUX_SPB_ALMOST_FULL_1_SRAM_REJECT_COUNTER]);
    COMPILER_64_ADD_64(sum_counters ,counters_data[QUX_SPB_ALMOST_FULL_2_SRAM_REJECT_COUNTER]);
    print_counter_val_to_string(unit, sum_counters, overflow, print_none_zero, hex_base, cntbuf[0]);
    compute_counter_val_into_string_2(unit, counters_data[QUX_EGQ_EHP_MULTICAST_HIGH_DISCARDS_COUNTER], EGQ_EHP_MULTICAST_HIGH_DISCARDS_COUNTERr,
                                      EHP_MULTICAST_HIGH_DISCARDS_COUNTERf, 32, print_none_zero, hex_base, cntbuf[1]);
    print_g_line("SPB_ALMOST_FULL_N_SRAM_RJCT_PKT_CNT", NULL, "EHP_MC_HIGH_DSCRD_CNT", NULL, cntbuf);


    compute_counter_val_into_string_2(unit, counters_data[QUX_EGQ_EHP_MULTICAST_LOW_DISCARDS_COUNTER],  EGQ_EHP_MULTICAST_LOW_DISCARDS_COUNTERr,
                                      EHP_MULTICAST_LOW_DISCARDS_COUNTERf, 32, print_none_zero, hex_base, cntbuf[0]);
    print_g_pattern_section();
    print_g_section("EHP_MC_LOW_DSCRD_CNT", NULL, cntbuf);
    print_g_new_line();


    compute_counter_val_into_string_2(unit, counters_data[QUX_EGQ_EHP_MULTICAST_HIGH_PACKET_COUNTER], EGQ_EHP_MULTICAST_HIGH_PACKET_COUNTERr,
                                      EHP_MULTICAST_HIGH_PACKET_COUNTERf, 32, print_none_zero, hex_base, cntbuf[0]);
    print_g_section_title("CGM");
    print_g_section("EHP_MC_HIGH_PKT_CNT", NULL, cntbuf);
    print_g_new_line();

	print_counter_val_to_string(unit, counters_data[QUX_CGM_QNUM_NOT_VALID_PACKETS], overflow, print_none_zero, hex_base, cntbuf[0]);
    compute_counter_val_into_string_2(unit, counters_data[QUX_EGQ_EHP_MULTICAST_LOW_PACKET_COUNTER], EGQ_EHP_MULTICAST_LOW_PACKET_COUNTERr,
                                      EHP_MULTICAST_LOW_PACKET_COUNTERf, 32, print_none_zero, hex_base, cntbuf[1]);
    print_g_line("QNUM_NOT_VALID_PKT_CNT", NULL, "EHP_MC_LOW_PKT_CNT", NULL, cntbuf);

    compute_counter_val_into_string_2(unit, counters_data[QUX_CGM_VOQ_SRAM_ENQ_PACKET_COUNTER], CGM_VOQ_SRAM_ENQ_PKT_CTRr,
                                      VOQ_SRAM_ENQ_PKT_CTRf, 31, print_none_zero, hex_base, cntbuf[0]);
    compute_counter_val_into_string_2(unit, counters_data[QUX_EGQ_EHP_UNICAST_PACKET_COUNTER], EGQ_EHP_UNICAST_PACKET_COUNTERr,
                                      EHP_UNICAST_PACKET_COUNTERf, 32, print_none_zero, hex_base, cntbuf[1]);
    print_g_line("CGM_VOQ_SRAM_ENQ_PKT_CNT", NULL, "EHP_UNICAST_PKT_CNT", NULL, cntbuf);


    compute_counter_val_into_string_2(unit, counters_data[QUX_CGM_VOQ_SRAM_DEQ_PACKET_COUNTER], CGM_VOQ_SRAM_DEQ_PKT_CTRr,
                                      VOQ_SRAM_DEQ_PKT_CTRf, 31, print_none_zero, hex_base, cntbuf[0]);
    print_g_line("CGM_VOQ_SRAM_DEQ_PKT_CNT", NULL, NULL, NULL, cntbuf);

    COMPILER_64_ZERO(sum_counters);
    compute_counter_val_into_string_2(unit, counters_data[QUX_CGM_VOQ_SRAM_DEL_PACKET_COUNTER], CGM_VOQ_SRAM_DEL_PKT_CTRr,
                                      VOQ_SRAM_DEL_PKT_CTRf, 31, print_none_zero, hex_base, cntbuf[0]);
    rqp_counters_sum_array[0] = QUX_EGQ_PRP_PACKET_GOOD_UC_CNT;
    qax_compute_sum_rqp_counters32(unit, rqp_reg[0], rqp_counters_sum_array, 1, qux_rqp_counters, print_none_zero, hex_base, &sum_counters);
    print_counter_val_to_string(unit, sum_counters, overflow, print_none_zero, hex_base, cntbuf[1]);
    print_g_line("CGM_VOQ_SRAM_DEL_PKT_CNT", NULL, "PRP_PKT_GOOD_UC_CNT", NULL, cntbuf);


    compute_counter_val_into_string_2(unit, counters_data[QUX_CGM_VOQ_DRAM_ENQ_PACKET_COUNTER], CGM_VOQ_DRAM_ENQ_BUNDLE_CTRr,
                                     VOQ_DRAM_ENQ_BUNDLE_CTRf, 31, print_none_zero, hex_base, cntbuf[0]);
    rqp_counters_sum_array[0] = QUX_EGQ_PRP_PACKET_GOOD_MC_CNT;
    qax_compute_sum_rqp_counters32(unit, rqp_reg[0], rqp_counters_sum_array, 1, qux_rqp_counters, print_none_zero, hex_base, &sum_counters);
    print_counter_val_to_string(unit, sum_counters, overflow, print_none_zero, hex_base, cntbuf[1]);
    print_g_line("CGM_VOQ_DRAM_ENQ_BUNDLE_CTR", NULL, "PRP_PKT_GOOD_MC_CNT", NULL, cntbuf);

    compute_counter_val_into_string_2(unit, counters_data[QUX_CGM_VOQ_DRAM_DEQ_PACKET_COUNTER], CGM_VOQ_DRAM_DEQ_PKT_CTRr,
                                      VOQ_DRAM_DEQ_PKT_CTRf, 31, print_none_zero, hex_base, cntbuf[0]);
    rqp_counters_sum_array[0] = QUX_EGQ_PRP_PACKET_DISCARD_UC_CNT;
    qax_compute_sum_rqp_counters32(unit, rqp_reg[0], rqp_counters_sum_array, 1, qux_rqp_counters, print_none_zero, hex_base, &sum_counters);
    print_counter_val_to_string(unit, sum_counters, overflow, print_none_zero, hex_base, cntbuf[1]);
    print_g_line("CGM_VOQ_DRAM_DEQ_PKT_CTR", NULL, "PRP_PKT_DSCRD_UC_CNT", NULL, cntbuf);

    compute_counter_val_into_string_2(unit, counters_data[QUX_CGM_VOQ_DRAM_DEL_PACKET_COUNTER], CGM_VOQ_DRAM_DEL_PKT_CTRr,
                                      VOQ_DRAM_DEL_PKT_CTRf, 31, print_none_zero, hex_base, cntbuf[0]);
    rqp_counters_sum_array[0] = QUX_EGQ_PRP_PACKET_DISCARD_MC_CNT;
    qax_compute_sum_rqp_counters32(unit, rqp_reg[0], rqp_counters_sum_array, 1, qux_rqp_counters, print_none_zero, hex_base, &sum_counters);
    print_counter_val_to_string(unit, sum_counters, overflow, print_none_zero, hex_base, cntbuf[1]);
    print_g_line("CGM_VOQ_DRAM_DEL_PKT_CTR", NULL, "PRP_PKT_DSCRD_MC_CNT", NULL, cntbuf);

	print_counter_val_to_string(unit, counters_data[QUX_CGM_VOQ_SRAM_ENQ_RJCT_PACKET_COUNTER], overflow, print_none_zero, hex_base, cntbuf[0]);
    rqp_counters_sum_array[0] = QUX_EGQ_PRP_SOP_DISCARD_UC_CNT;
    qax_compute_sum_rqp_counters32(unit, rqp_reg[0], rqp_counters_sum_array, 1, qux_rqp_counters, print_none_zero, hex_base, &sum_counters);
    print_counter_val_to_string(unit, sum_counters, overflow, print_none_zero, hex_base, cntbuf[1]);
    print_g_line("CGM_VOQ_SRAM_ENQ_REJECT_PCK_CNT", NULL, "PRP_SOP_DSCRD_UC_CNT", NULL, cntbuf);

    rqp_counters_sum_array[0] = QUX_EGQ_PRP_SOP_DISCARD_MC_CNT;
    qax_compute_sum_rqp_counters32(unit, rqp_reg[0], rqp_counters_sum_array, 1, qux_rqp_counters, print_none_zero, hex_base, &sum_counters);
    print_counter_val_to_string(unit, sum_counters, overflow, print_none_zero, hex_base, cntbuf[1]);
    print_g_line(NULL, NULL, "PRP_SOP_DSCRD_UC_CNT", NULL, cntbuf);

    rqp_counters_sum_array[0] = QUX_EGQ_PRP_PACKET_IN_CNT;
    qax_compute_sum_rqp_counters32(unit, rqp_reg[0], rqp_counters_sum_array, 1, qux_rqp_counters, print_none_zero, hex_base, &sum_counters);
    print_counter_val_to_string(unit, sum_counters, overflow, print_none_zero, hex_base, cntbuf[1]);
    print_g_line(NULL, NULL, "PRP_PACKET_IN_CNT", NULL, cntbuf);

    rqp_counters_sum_array[0] = QUX_EGQ_PQP_CELL_IN_CNT;
    qax_compute_sum_rqp_counters32(unit, rqp_reg[0], rqp_counters_sum_array, 1, qux_rqp_counters, print_none_zero, hex_base, &sum_counters);
    print_counter_val_to_string(unit, sum_counters, overflow, print_none_zero, hex_base, cntbuf[1]);
    print_g_line(NULL, NULL, "PQP_CELL_IN_CNT", NULL, cntbuf);

    /** print reject reason */
    diag_print_iqm_rejects_and_erpp_discards(unit, FALSE);

    print_g_pattern_section();
    print_g_section_title("");
    print_g_new_line();
    print_g_section_title("TXQ");
    print_g_new_line();

    rqp_counters_sum_array[0] = QUX_TXQ_EGQ_SRAM_COUNTER;
    qax_compute_sum_rqp_counters32(unit, rqp_reg[QUX_TXQ_EGQ_SRAM_COUNTER], rqp_counters_sum_array, 1, qux_counters, print_none_zero, hex_base, &sum_counters);
    print_counter_val_to_string(unit, sum_counters, overflow, print_none_zero, hex_base, cntbuf[0]);
    print_g_line("EGQ_SRAM_PKT_CNT", NULL, NULL, NULL, cntbuf);

    rqp_counters_sum_array[0] = QUX_TXQ_EGQ_DRAM_COUNTER;
    qax_compute_sum_rqp_counters32(unit, rqp_reg[QUX_TXQ_EGQ_DRAM_COUNTER], rqp_counters_sum_array, 1, qux_counters, print_none_zero, hex_base, &sum_counters);
    print_counter_val_to_string(unit, sum_counters, overflow, print_none_zero, hex_base, cntbuf[0]);
    print_g_line("EGQ_DRAM_PKT_CNT", NULL, NULL, NULL, cntbuf);
    print_g_new_line();

    print_g_pattern_line();
    cli_out("                                            |                                                                      /|\\\n");
    cli_out("                                            |                    Q U X   F A B R I C   I N T E R F A C E            |\n");
    cli_out("                                           \\|/                                                                      |\n");
    cli_out("\n\n");
    sal_free(rqp_reg);
    return CMD_OK;
}

#endif /*BCM_88470_A0*/


/* Read Arad counters*/
int arad_packet_flow_counter_get(int unit, uint32 print_none_zero, uint32 hex_base) {
   char    counter1_str[30];
   char    counter2_str[30];
   uint32  counter_data[2];
   uint32  counter_MSB;
   uint32  counter_LSB;
   uint32  counter_OVF;
   uint32  print_reason=0;
   uint32  skip_first_print = 0, skip_second_print = 0;
   char    buf_val[32];
   int     commachr = soc_property_get(unit, spn_DIAG_COMMA, ',');
   uint64  val_64;
   int rc;




   cli_out("\n");
   cli_out("                               |                                                            /|\\\n");
   cli_out("                               |         A R A D   N E T W O R K   I N T E R F A C E         |\n");
   cli_out("                              \\|/                                                            |\n");
   cli_out("+------------------------------+------------------------------+------------------------------+------------------------------+\n");
   cli_out("|                                                            NBI                                                            |\n");

    if(!SOC_IS_ARDON(unit)) {
        common_read_reg_long(unit, "NBI_STATISTICS_RX_BURSTS_OK_CNT", counter_data, 2); 
        counter_MSB = counter_data[1];
        counter_LSB = counter_data[0];
        counter_OVF = get_field(counter_MSB, 16, 16);
        counter_MSB = get_field(counter_MSB, 0, 15);
        
        if (hex_base == 1) {
           if (counter_OVF == 0) {
              if (counter_MSB == 0) {sal_snprintf(counter1_str, sizeof(counter1_str), "0x%x", counter_LSB); } else                  {sal_snprintf(counter1_str, sizeof(counter1_str), "0x%x%x", counter_MSB, counter_LSB); }
           } else {
              if (counter_MSB == 0) {sal_snprintf(counter1_str, sizeof(counter1_str), "0x%x (ovf)", counter_LSB); } else                  {sal_snprintf(counter1_str, sizeof(counter1_str), "0x%x%x (ovf)", counter_MSB, counter_LSB); }
           }
       } else {
           COMPILER_64_SET(val_64, counter_MSB, counter_LSB);
           format_uint64_decimal(buf_val, val_64, commachr);
           if (counter_OVF == 0) {
               sal_snprintf(counter1_str, sizeof(counter1_str), "%s", buf_val);
           } else {
               sal_snprintf(counter1_str, sizeof(counter1_str), "%s (ovf)", buf_val);
           }
        }

        if (print_none_zero == 1) {
            if ((counter_LSB == 0) && (counter_MSB == 0)) {
                skip_first_print = 1;
            }
        }

        common_read_reg_long(unit, "NBI_STATISTICS_TX_BURSTS_CNT", counter_data, 2); 
        counter_MSB = counter_data[1];
        counter_LSB = counter_data[0];

        if (print_none_zero == 1) {
            if ((counter_LSB == 0) && (counter_MSB == 0)) {
                skip_second_print = 1;
            }
        }

        counter_OVF = get_field(counter_MSB, 16, 16);
        counter_MSB = get_field(counter_MSB, 0, 15);


        if (hex_base == 1) {
            if (counter_OVF == 0) {
                if (counter_MSB == 0) {sal_snprintf(counter2_str, sizeof(counter2_str), "0x%x", counter_LSB); } else                  {sal_snprintf(counter2_str, sizeof(counter2_str), "0x%x%x", counter_MSB, counter_LSB); }
            } else {
                if (counter_MSB == 0) {sal_snprintf(counter2_str, sizeof(counter2_str), "0x%x (ovf)", counter_LSB); } else                  {sal_snprintf(counter2_str, sizeof(counter2_str), "0x%x%x (ovf)", counter_MSB, counter_LSB); }
            }
        } else {
            COMPILER_64_SET(val_64, counter_MSB, counter_LSB);
            format_uint64_decimal(buf_val, val_64, commachr);
            if (counter_OVF == 0) {
                sal_snprintf(counter2_str, sizeof(counter2_str), "%s", buf_val);
            } else {
                sal_snprintf(counter2_str, sizeof(counter2_str), "%s (ovf)", buf_val);
            }
        }

        if ((skip_first_print == 1) && (skip_second_print == 1)) {
            cli_out("|   %-30s   %-24s     %-30s   %-24s |\n", "", "", "", "");
        } else if ((skip_first_print == 1) && (skip_second_print == 0)) {
            cli_out("|   %-30s   %-24s     %-30s = %-24s |\n", "", "", "STATISTICS_TX_BURSTS_CNT", counter2_str);
        } else if ((skip_first_print == 0) && (skip_second_print == 1)) {
            cli_out("|   %-30s = %-24s     %-30s   %-24s |\n", "STATISTICS_RX_BURSTS_OK_CNT", counter1_str, "", "");
        } else {
            cli_out("|   %-30s = %-24s     %-30s = %-24s |\n", "STATISTICS_RX_BURSTS_OK_CNT", counter1_str, "STATISTICS_TX_BURSTS_CNT", counter2_str);
        }
        
        skip_first_print = 0;
        skip_second_print = 0;

        common_read_reg_long(unit, "NBI_STATISTICS_RX_BURSTS_ERR_CNT", counter_data, 1);
        counter_LSB = counter_data[0];
        counter_OVF = get_field(counter_LSB, 31, 31);
        counter_LSB = get_field(counter_LSB, 0, 30);

        if (hex_base == 1) {
            if (counter_OVF == 0) {
                sal_snprintf(counter1_str, sizeof(counter1_str), "0x%x", counter_LSB);
            } else {
                sal_snprintf(counter1_str, sizeof(counter1_str), "0x%x (ovf)", counter_LSB);
            }
        } else {
            COMPILER_64_SET(val_64, 0, counter_LSB);
            format_uint64_decimal(buf_val, val_64, commachr);
            if (counter_OVF == 0) {
                sal_snprintf(counter1_str, sizeof(counter1_str), "%s", buf_val);
            } else {
                sal_snprintf(counter1_str, sizeof(counter1_str), "%s (ovf)", buf_val);
            }
        }

        if (print_none_zero == 1) {
            if (counter_LSB == 0) {
                skip_first_print = 1;
            }
        }
        if (skip_first_print == 1) {
            cli_out("|   %-30s   %-24s                                                               |\n", "", "");
        } else {
            cli_out("|   %-30s = %-24s                                                               |\n", "STATISTICS_RX_BURSTS_ERR_CNT", counter1_str);
        }
   
   } else { 

        common_read_reg_long(unit, "NBI_RX_BYTE_TOTAL_CNT", counter_data, 2); 
        counter_MSB = counter_data[1];
        counter_LSB = counter_data[0];
        counter_OVF = get_field(counter_MSB, 30, 30);
        counter_MSB = get_field(counter_MSB, 0, 29);
        
        if (hex_base == 1) {
           if (counter_OVF == 0) {
              if (counter_MSB == 0) {sal_snprintf(counter1_str, sizeof(counter1_str), "0x%x", counter_LSB); } else                  {sal_snprintf(counter1_str, sizeof(counter1_str), "0x%x%x", counter_MSB, counter_LSB); }
           } else {
              if (counter_MSB == 0) {sal_snprintf(counter1_str, sizeof(counter1_str), "0x%x (ovf)", counter_LSB); } else                  {sal_snprintf(counter1_str, sizeof(counter1_str), "0x%x%x (ovf)", counter_MSB, counter_LSB); }
           }
       } else {
           COMPILER_64_SET(val_64, counter_MSB, counter_LSB);
           format_uint64_decimal(buf_val, val_64, commachr);
           if (counter_OVF == 0) {
               sal_snprintf(counter1_str, sizeof(counter1_str), "%s", buf_val);
           } else {
               sal_snprintf(counter1_str, sizeof(counter1_str), "%s (ovf)", buf_val);
           }
        }

        if (print_none_zero == 1) {
            if ((counter_LSB == 0) && (counter_MSB == 0)) {
                skip_first_print = 1;
            }
        }

        common_read_reg_long(unit, "NBI_TX_BYTE_TOTAL_CNT", counter_data, 2); 
        counter_MSB = counter_data[1];
        counter_LSB = counter_data[0];

        if (print_none_zero == 1) {
            if ((counter_LSB == 0) && (counter_MSB == 0)) {
                skip_second_print = 1;
            }
        }

        counter_OVF = get_field(counter_MSB, 30, 30);
        counter_MSB = get_field(counter_MSB, 0, 29);


        if (hex_base == 1) {
            if (counter_OVF == 0) {
                if (counter_MSB == 0) {sal_snprintf(counter2_str, sizeof(counter2_str), "0x%x", counter_LSB); } else                  {sal_snprintf(counter2_str, sizeof(counter2_str), "0x%x%x", counter_MSB, counter_LSB); }
            } else {
                if (counter_MSB == 0) {sal_snprintf(counter2_str, sizeof(counter2_str), "0x%x (ovf)", counter_LSB); } else                  {sal_snprintf(counter2_str, sizeof(counter2_str), "0x%x%x (ovf)", counter_MSB, counter_LSB); }
            }
        } else {
            COMPILER_64_SET(val_64, counter_MSB, counter_LSB);
            format_uint64_decimal(buf_val, val_64, commachr);
            if (counter_OVF == 0) {
                sal_snprintf(counter2_str, sizeof(counter2_str), "%s", buf_val);
            } else {
                sal_snprintf(counter2_str, sizeof(counter2_str), "%s (ovf)", buf_val);
            }
        }
        
        if ((skip_first_print == 1) && (skip_second_print == 1)) {
            cli_out("|   %-30s   %-24s     %-30s   %-24s |\n", "", "", "", "");
        } else if ((skip_first_print == 1) && (skip_second_print == 0)) {
            cli_out("|   %-30s   %-24s     %-30s = %-24s |\n", "", "", "NBI_TX_BYTE_TOTAL_CNT", counter2_str);
        } else if ((skip_first_print == 0) && (skip_second_print == 1)) {
            cli_out("|   %-30s = %-24s     %-30s   %-24s |\n", "NBI_RX_BYTE_TOTAL_CNT", counter1_str, "", "");
        } else {
            cli_out("|   %-30s = %-24s     %-30s = %-24s |\n", "NBI_RX_BYTE_TOTAL_CNT", counter1_str, "NBI_TX_BYTE_TOTAL_CNT", counter2_str);
        }

        skip_first_print = 0;
        skip_second_print = 0;
    
        common_read_reg_long(unit, "NBI_RX_EOP_PKT_TOTAL_CNT", counter_data, 2); 
        counter_MSB = counter_data[1];
        counter_LSB = counter_data[0];
        counter_OVF = get_field(counter_MSB, 30, 30);
        counter_MSB = get_field(counter_MSB, 0, 29);
        
        if (hex_base == 1) {
           if (counter_OVF == 0) {
              if (counter_MSB == 0) {sal_snprintf(counter1_str, sizeof(counter1_str), "0x%x", counter_LSB); } else                  {sal_snprintf(counter1_str, sizeof(counter1_str), "0x%x%x", counter_MSB, counter_LSB); }
           } else {
              if (counter_MSB == 0) {sal_snprintf(counter1_str, sizeof(counter1_str), "0x%x (ovf)", counter_LSB); } else                  {sal_snprintf(counter1_str, sizeof(counter1_str), "0x%x%x (ovf)", counter_MSB, counter_LSB); }
           }
       } else {
           COMPILER_64_SET(val_64, counter_MSB, counter_LSB);
           format_uint64_decimal(buf_val, val_64, commachr);
           if (counter_OVF == 0) {
               sal_snprintf(counter1_str, sizeof(counter1_str), "%s", buf_val);
           } else {
               sal_snprintf(counter1_str, sizeof(counter1_str), "%s (ovf)", buf_val);
           }
        }

        if (print_none_zero == 1) {
            if ((counter_LSB == 0) && (counter_MSB == 0)) {
                skip_first_print = 1;
            }
        }

        common_read_reg_long(unit, "NBI_TX_EOP_PKT_TOTAL_CNT", counter_data, 2); 
        counter_MSB = counter_data[1];
        counter_LSB = counter_data[0];

        if (print_none_zero == 1) {
            if ((counter_LSB == 0) && (counter_MSB == 0)) {
                skip_second_print = 1;
            }
        }

        counter_OVF = get_field(counter_MSB, 30, 30);
        counter_MSB = get_field(counter_MSB, 0, 29);


        if (hex_base == 1) {
            if (counter_OVF == 0) {
                if (counter_MSB == 0) {sal_snprintf(counter2_str, sizeof(counter2_str), "0x%x", counter_LSB); } else                  {sal_snprintf(counter2_str, sizeof(counter2_str), "0x%x%x", counter_MSB, counter_LSB); }
            } else {
                if (counter_MSB == 0) {sal_snprintf(counter2_str, sizeof(counter2_str), "0x%x (ovf)", counter_LSB); } else                  {sal_snprintf(counter2_str, sizeof(counter2_str), "0x%x%x (ovf)", counter_MSB, counter_LSB); }
            }
        } else {
            COMPILER_64_SET(val_64, counter_MSB, counter_LSB);
            format_uint64_decimal(buf_val, val_64, commachr);
            if (counter_OVF == 0) {
                sal_snprintf(counter2_str, sizeof(counter2_str), "%s", buf_val);
            } else {
                sal_snprintf(counter2_str, sizeof(counter2_str), "%s (ovf)", buf_val);
            }
        }
               
        if ((skip_first_print == 1) && (skip_second_print == 1)) {
            cli_out("|   %-30s   %-24s     %-30s   %-24s |\n", "", "", "", "");
        } else if ((skip_first_print == 1) && (skip_second_print == 0)) {
            cli_out("|   %-30s   %-24s     %-30s = %-24s |\n", "", "", "NBI_TX_EOP_PKT_TOTAL_CNT", counter2_str);
        } else if ((skip_first_print == 0) && (skip_second_print == 1)) {
            cli_out("|   %-30s = %-24s     %-30s   %-24s |\n", "NBI_RX_EOP_PKT_TOTAL_CNT", counter1_str, "", "");
        } else {
            cli_out("|   %-30s = %-24s     %-30s = %-24s |\n", "NBI_RX_EOP_PKT_TOTAL_CNT", counter1_str, "NBI_TX_EOP_PKT_TOTAL_CNT", counter2_str);
        }

    
        skip_first_print = 0;
        skip_second_print = 0;
    
        common_read_reg_long(unit, "NBI_RX_SOP_PKT_TOTAL_CNT", counter_data, 2); 
        counter_MSB = counter_data[1];
        counter_LSB = counter_data[0];
        counter_OVF = get_field(counter_MSB, 30, 30);
        counter_MSB = get_field(counter_MSB, 0, 29);
        
        if (hex_base == 1) {
           if (counter_OVF == 0) {
              if (counter_MSB == 0) {sal_snprintf(counter1_str, sizeof(counter1_str), "0x%x", counter_LSB); } else                  {sal_snprintf(counter1_str, sizeof(counter1_str), "0x%x%x", counter_MSB, counter_LSB); }
           } else {
              if (counter_MSB == 0) {sal_snprintf(counter1_str, sizeof(counter1_str), "0x%x (ovf)", counter_LSB); } else                  {sal_snprintf(counter1_str, sizeof(counter1_str), "0x%x%x (ovf)", counter_MSB, counter_LSB); }
           }
       } else {
           COMPILER_64_SET(val_64, counter_MSB, counter_LSB);
           format_uint64_decimal(buf_val, val_64, commachr);
           if (counter_OVF == 0) {
               sal_snprintf(counter1_str, sizeof(counter1_str), "%s", buf_val);
           } else {
               sal_snprintf(counter1_str, sizeof(counter1_str), "%s (ovf)", buf_val);
           }
        }

        if (print_none_zero == 1) {
            if ((counter_LSB == 0) && (counter_MSB == 0)) {
                skip_first_print = 1;
            }
        }

        common_read_reg_long(unit, "NBI_TX_SOP_PKT_TOTAL_CNT", counter_data, 2); 
        counter_MSB = counter_data[1];
        counter_LSB = counter_data[0];

        if (print_none_zero == 1) {
            if ((counter_LSB == 0) && (counter_MSB == 0)) {
                skip_second_print = 1;
            }
        }

        counter_OVF = get_field(counter_MSB, 30, 30);
        counter_MSB = get_field(counter_MSB, 0, 29);


        if (hex_base == 1) {
            if (counter_OVF == 0) {
                if (counter_MSB == 0) {sal_snprintf(counter2_str, sizeof(counter2_str), "0x%x", counter_LSB); } else                  {sal_snprintf(counter2_str, sizeof(counter2_str), "0x%x%x", counter_MSB, counter_LSB); }
            } else {
                if (counter_MSB == 0) {sal_snprintf(counter2_str, sizeof(counter2_str), "0x%x (ovf)", counter_LSB); } else                  {sal_snprintf(counter2_str, sizeof(counter2_str), "0x%x%x (ovf)", counter_MSB, counter_LSB); }
            }
        } else {
            COMPILER_64_SET(val_64, counter_MSB, counter_LSB);
            format_uint64_decimal(buf_val, val_64, commachr);
            if (counter_OVF == 0) {
                sal_snprintf(counter2_str, sizeof(counter2_str), "%s", buf_val);
            } else {
                sal_snprintf(counter2_str, sizeof(counter2_str), "%s (ovf)", buf_val);
            }
        }    

        if ((skip_first_print == 1) && (skip_second_print == 1)) {
            cli_out("|   %-30s   %-24s     %-30s   %-24s |\n", "", "", "", "");
        } else if ((skip_first_print == 1) && (skip_second_print == 0)) {
            cli_out("|   %-30s   %-24s     %-30s = %-24s |\n", "", "", "NBI_TX_SOP_PKT_TOTAL_CNT", counter2_str);
        } else if ((skip_first_print == 0) && (skip_second_print == 1)) {
            cli_out("|   %-30s = %-24s     %-30s   %-24s |\n", "NBI_RX_SOP_PKT_TOTAL_CNT", counter1_str, "", "");
        } else {
            cli_out("|   %-30s = %-24s     %-30s = %-24s |\n", "NBI_RX_SOP_PKT_TOTAL_CNT", counter1_str, "NBI_TX_SOP_PKT_TOTAL_CNT", counter2_str);
        }
    
        skip_first_print = 0;
        skip_second_print = 0;
    
        common_read_reg_long(unit, "NBI_RX_ERR_PKT_TOTAL_CNT", counter_data, 2); 
        counter_MSB = counter_data[1];
        counter_LSB = counter_data[0];
        counter_OVF = get_field(counter_MSB, 30, 30);
        counter_MSB = get_field(counter_MSB, 0, 29);
        
        if (hex_base == 1) {
           if (counter_OVF == 0) {
              if (counter_MSB == 0) {sal_snprintf(counter1_str, sizeof(counter1_str), "0x%x", counter_LSB); } else                  {sal_snprintf(counter1_str, sizeof(counter1_str), "0x%x%x", counter_MSB, counter_LSB); }
           } else {
              if (counter_MSB == 0) {sal_snprintf(counter1_str, sizeof(counter1_str), "0x%x (ovf)", counter_LSB); } else                  {sal_snprintf(counter1_str, sizeof(counter1_str), "0x%x%x (ovf)", counter_MSB, counter_LSB); }
           }
       } else {
           COMPILER_64_SET(val_64, counter_MSB, counter_LSB);
           format_uint64_decimal(buf_val, val_64, commachr);
           if (counter_OVF == 0) {
               sal_snprintf(counter1_str, sizeof(counter1_str), "%s", buf_val);
           } else {
               sal_snprintf(counter1_str, sizeof(counter1_str), "%s (ovf)", buf_val);
           }
        }

        if (print_none_zero == 1) {
            if ((counter_LSB == 0) && (counter_MSB == 0)) {
                skip_first_print = 1;
            }
        }

        common_read_reg_long(unit, "NBI_TX_ERR_PKT_TOTAL_CNT", counter_data, 2); 
        counter_MSB = counter_data[1];
        counter_LSB = counter_data[0];

        if (print_none_zero == 1) {
            if ((counter_LSB == 0) && (counter_MSB == 0)) {
                skip_second_print = 1;
            }
        }

        counter_OVF = get_field(counter_MSB, 30, 30);
        counter_MSB = get_field(counter_MSB, 0, 29);


        if (hex_base == 1) {
            if (counter_OVF == 0) {
                if (counter_MSB == 0) {sal_snprintf(counter2_str, sizeof(counter2_str), "0x%x", counter_LSB); } else                  {sal_snprintf(counter2_str, sizeof(counter2_str), "0x%x%x", counter_MSB, counter_LSB); }
            } else {
                if (counter_MSB == 0) {sal_snprintf(counter2_str, sizeof(counter2_str), "0x%x (ovf)", counter_LSB); } else                  {sal_snprintf(counter2_str, sizeof(counter2_str), "0x%x%x (ovf)", counter_MSB, counter_LSB); }
            }
        } else {
            COMPILER_64_SET(val_64, counter_MSB, counter_LSB);
            format_uint64_decimal(buf_val, val_64, commachr);
            if (counter_OVF == 0) {
                sal_snprintf(counter2_str, sizeof(counter2_str), "%s", buf_val);
            } else {
                sal_snprintf(counter2_str, sizeof(counter2_str), "%s (ovf)", buf_val);
            }
        }      
    
    if ((skip_first_print == 1) && (skip_second_print == 1)) {
             cli_out("|   %-30s   %-24s     %-30s   %-24s |\n", "", "", "", "");
         } else if ((skip_first_print == 1) && (skip_second_print == 0)) {
             cli_out("|   %-30s   %-24s     %-30s = %-24s |\n", "", "", "NBI_TX_ERR_PKT_TOTAL_CNT", counter2_str);
         } else if ((skip_first_print == 0) && (skip_second_print == 1)) {
             cli_out("|   %-30s = %-24s     %-30s   %-24s |\n", "NBI_RX_ERR_PKT_TOTAL_CNT", counter1_str, "", "");
         } else {
             cli_out("|   %-30s = %-24s     %-30s = %-24s |\n", "NBI_RX_ERR_PKT_TOTAL_CNT", counter1_str, "NBI_TX_ERR_PKT_TOTAL_CNT", counter2_str);
         }
   }
   
   skip_first_print = 0;
   skip_second_print = 0;

   cli_out("|-------------------------------------------------------------+-------------------------------------------------------------|\n");
   cli_out("|                             IRE                             |                             EPNI                            |\n");

   common_read_reg_long(unit, "IRE_NIF_PACKET_COUNTER", counter_data, 2);
   counter_MSB = counter_data[1];
   counter_LSB = counter_data[0];
   counter_OVF = get_field(counter_MSB, 31, 31);
   counter_MSB = get_field(counter_MSB, 0, 30);


   if (hex_base == 1) {
      if (counter_OVF == 0) {
         if (counter_MSB == 0) {sal_snprintf(counter1_str, sizeof(counter1_str), "0x%x", counter_LSB); } else                  {sal_snprintf(counter1_str, sizeof(counter1_str), "0x%x%x", counter_MSB, counter_LSB); }
      } else {
         if (counter_MSB == 0) {sal_snprintf(counter1_str, sizeof(counter1_str), "0x%x (ovf)", counter_LSB); } else                  {sal_snprintf(counter1_str, sizeof(counter1_str), "0x%x%x (ovf)", counter_MSB, counter_LSB); }
      }
   } else {
      COMPILER_64_SET(val_64, counter_MSB, counter_LSB);
      format_uint64_decimal(buf_val, val_64, commachr);
      if (counter_OVF == 0) {
         sal_snprintf(counter1_str, sizeof(counter1_str), "%s", buf_val);
      } else {
         sal_snprintf(counter1_str, sizeof(counter1_str), "%s (ovf)", buf_val);
      }
   }

   if (print_none_zero == 1) {
      if ((counter_LSB == 0) && (counter_MSB == 0)) {
         skip_first_print = 1;
      }
   }

   common_read_reg_long(unit, "EPNI_EPE_PACKET_COUNTER", counter_data, 2);
   counter_MSB = counter_data[1];
   counter_LSB = counter_data[0];
   counter_OVF = get_field(counter_MSB, 0, 0);

   if (hex_base == 1) {
      if (counter_OVF == 0) {
         sal_snprintf(counter2_str, sizeof(counter2_str), "0x%x", counter_LSB);
      } else {
         sal_snprintf(counter2_str, sizeof(counter2_str), "0x%x (ovf)", counter_LSB);
      }
   } else {
      COMPILER_64_SET(val_64, 0, counter_LSB);
      format_uint64_decimal(buf_val, val_64, commachr);
      if (counter_OVF == 0) {
         sal_snprintf(counter2_str, sizeof(counter2_str), "%s", buf_val);
      } else {
         sal_snprintf(counter2_str, sizeof(counter2_str), "%s (ovf)", buf_val);
      }
   }

   if (print_none_zero == 1) {
      if ((counter_LSB == 0) && (counter_MSB == 0)) {
         skip_second_print = 1;
      }
   }

   if ((skip_first_print == 1) && (skip_second_print == 1)) {
      cli_out("|   %-30s   %-24s |   %-30s   %-24s |\n", "", "", "", "");
   } else if ((skip_first_print == 1) && (skip_second_print == 0)) {
      cli_out("|   %-30s   %-24s |   %-30s = %-24s |\n", "", "", "EPE_PACKET_COUNTER", counter2_str);
   } else if ((skip_first_print == 0) && (skip_second_print == 1)) {
      cli_out("|   %-30s = %-24s |   %-30s   %-24s |\n", "NIF_PACKET_COUNTER", counter1_str, "", "");
   } else {
      cli_out("|   %-30s = %-24s |   %-30s = %-24s |\n", "NIF_PACKET_COUNTER", counter1_str, "EPE_PACKET_COUNTER", counter2_str);
   }
   skip_first_print = 0;
   skip_second_print = 0;

   common_read_reg_long(unit, "IRE_CPU_PACKET_COUNTER", counter_data, 2);
   counter_MSB = counter_data[1];
   counter_LSB = counter_data[0];
   counter_OVF = get_field(counter_MSB, 31, 31);
   counter_MSB = get_field(counter_MSB, 0, 30);

   if (hex_base == 1) {
      if (counter_OVF == 0) {
         if (counter_MSB == 0) {sal_snprintf(counter1_str, sizeof(counter1_str), "0x%x", counter_LSB); } else                  {sal_snprintf(counter1_str, sizeof(counter1_str), "0x%x%x", counter_MSB, counter_LSB); }
      } else {
         if (counter_MSB == 0) {sal_snprintf(counter1_str, sizeof(counter1_str), "0x%x (ovf)", counter_LSB); } else                  {sal_snprintf(counter1_str, sizeof(counter1_str), "0x%x%x (ovf)", counter_MSB, counter_LSB); }
      }
   } else {
      COMPILER_64_SET(val_64, counter_MSB, counter_LSB);
      format_uint64_decimal(buf_val, val_64, commachr);
      if (counter_OVF == 0) {
         sal_snprintf(counter1_str, sizeof(counter1_str), "%s", buf_val);
      } else {
         sal_snprintf(counter1_str, sizeof(counter1_str), "%s (ovf)", buf_val);
      }
   }

   if (print_none_zero == 1) {
      if ((counter_LSB == 0) && (counter_MSB == 0)) {
         skip_first_print = 1;
      }
   }
   common_read_reg_long(unit, "EPNI_EPE_DISCARDED_PACKETS_COUNTER", counter_data, 2);
   counter_MSB = counter_data[1];
   counter_LSB = counter_data[0];
   counter_OVF = get_field(counter_MSB, 0, 0);

   if (hex_base == 1) {
      if (counter_OVF == 0) {
         sal_snprintf(counter2_str, sizeof(counter2_str), "0x%x", counter_LSB);
      } else {
         sal_snprintf(counter2_str, sizeof(counter2_str), "0x%x (ovf)", counter_LSB);
      }
   } else {
      COMPILER_64_SET(val_64, 0, counter_LSB);
      format_uint64_decimal(buf_val, val_64, commachr);
      if (counter_OVF == 0) {
         sal_snprintf(counter2_str, sizeof(counter2_str), "%s", buf_val);
      } else {
         sal_snprintf(counter2_str, sizeof(counter2_str), "%s (ovf)", buf_val);
      }
   }

   if (print_none_zero == 1) {
      if ((counter_LSB == 0) && (counter_MSB == 0)) {
         skip_second_print = 1;
      }
   }

   if ((skip_first_print == 1) && (skip_second_print == 1)) {
      cli_out("|   %-30s   %-24s |   %-30s   %-24s |\n", "", "", "", "");
   } else if ((skip_first_print == 1) && (skip_second_print == 0)) {
      cli_out("|   %-30s   %-24s |   %-30s = %-24s |\n", "", "", "EPE_DISCARDED_PACKETS_COUNTER", counter2_str);
   } else if ((skip_first_print == 0) && (skip_second_print == 1)) {
      cli_out("|   %-30s = %-24s |   %-30s   %-24s |\n", "CPU_PACKET_COUNTER", counter1_str, "", "");
   } else {
      cli_out("|   %-30s = %-24s |   %-30s = %-24s |\n", "CPU_PACKET_COUNTER", counter1_str, "EPE_DISCARDED_PACKETS_COUNTER", counter2_str);
   }
   skip_first_print = 0;
   skip_second_print = 0;

   common_read_reg_long(unit, "IRE_REGI_PACKET_COUNTER", counter_data, 2);
   counter_MSB = counter_data[1];
   counter_LSB = counter_data[0];
   counter_OVF = get_field(counter_MSB, 31, 31);
   counter_MSB = get_field(counter_MSB, 0, 30);

   if (hex_base == 1) {
      if (counter_OVF == 0) {
         if (counter_MSB == 0) {sal_snprintf(counter1_str, sizeof(counter1_str), "0x%x", counter_LSB); } else                  {sal_snprintf(counter1_str, sizeof(counter1_str), "0x%x%x", counter_MSB, counter_LSB); }
      } else {
         if (counter_MSB == 0) {sal_snprintf(counter1_str, sizeof(counter1_str), "0x%x (ovf)", counter_LSB); } else                  {sal_snprintf(counter1_str, sizeof(counter1_str), "0x%x%x (ovf)", counter_MSB, counter_LSB); }
      }
   } else {
      COMPILER_64_SET(val_64, counter_MSB, counter_LSB);
      format_uint64_decimal(buf_val, val_64, commachr);
      if (counter_OVF == 0) {
         sal_snprintf(counter1_str, sizeof(counter1_str), "%s", buf_val);
      } else {
         sal_snprintf(counter1_str, sizeof(counter1_str), "%s (ovf)", buf_val);
      }
   }

   if (print_none_zero == 1) {
      if ((counter_LSB == 0) && (counter_MSB == 0)) {
         skip_first_print = 1;
      }
   }

   common_read_reg_long(unit, "EPNI_EPE_BYTES_COUNTER", counter_data, 2);
   counter_LSB     = counter_data[0];
   counter_MSB     = counter_data[1];
   counter_OVF     = get_field(counter_MSB, 14, 14);
   counter_MSB     = get_field(counter_MSB, 0, 13);

   if (hex_base == 1) {
      if (counter_OVF == 0) {
         sal_snprintf(counter2_str, sizeof(counter2_str), "0x%x", counter_LSB);
      } else {
         sal_snprintf(counter2_str, sizeof(counter2_str), "0x%x (ovf)", counter_LSB);
      }
   } else {
      COMPILER_64_SET(val_64, 0, counter_LSB);
      format_uint64_decimal(buf_val, val_64, commachr);
      if (counter_OVF == 0) {
         sal_snprintf(counter2_str, sizeof(counter2_str), "%s", buf_val);
      } else {
         sal_snprintf(counter2_str, sizeof(counter2_str), "%s (ovf)", buf_val);
      }
   }

   if (print_none_zero == 1) {
      if ((counter_LSB == 0) && (counter_MSB == 0)) {
         skip_second_print = 1;
      }
   }

   if ((skip_first_print == 1) && (skip_second_print == 1)) {
      cli_out("|   %-30s   %-24s |   %-30s   %-24s |\n", "", "", "", "");
   } else if ((skip_first_print == 1) && (skip_second_print == 0)) {
      cli_out("|   %-30s   %-24s |   %-30s = %-24s |\n", "", "", "EPE_BYTES_COUNTER", counter2_str);
   } else if ((skip_first_print == 0) && (skip_second_print == 1)) {
      cli_out("|   %-30s = %-24s |   %-30s   %-24s |\n", "REGI_PACKET_COUNTER", counter1_str, "", "");
   } else {
      cli_out("|   %-30s = %-24s |   %-30s = %-24s |\n", "REGI_PACKET_COUNTER", counter1_str, "EPE_BYTES_COUNTER", counter2_str);
   }
   skip_first_print = 0;
   skip_second_print = 0;

   common_read_reg_long(unit, "IRE_RCY_PACKET_COUNTER", counter_data, 2);
   counter_MSB = counter_data[1];
   counter_LSB = counter_data[0];
   counter_OVF = get_field(counter_MSB, 31, 31);
   counter_MSB = get_field(counter_MSB, 0, 30);

   if (hex_base == 1) {
      if (counter_OVF == 0) {
         if (counter_MSB == 0) {sal_snprintf(counter1_str, sizeof(counter1_str), "0x%x", counter_LSB); } else                  {sal_snprintf(counter1_str, sizeof(counter1_str), "0x%x%x", counter_MSB, counter_LSB); }
      } else {
         if (counter_MSB == 0) {sal_snprintf(counter1_str, sizeof(counter1_str), "0x%x (ovf)", counter_LSB); } else                  {sal_snprintf(counter1_str, sizeof(counter1_str), "0x%x%x (ovf)", counter_MSB, counter_LSB); }
      }
   } else {
      COMPILER_64_SET(val_64, counter_MSB, counter_LSB);
      format_uint64_decimal(buf_val, val_64, commachr);
      if (counter_OVF == 0) {
         sal_snprintf(counter1_str, sizeof(counter1_str), "%s", buf_val);
      } else {
         sal_snprintf(counter1_str, sizeof(counter1_str), "%s (ovf)", buf_val);
      }
   }

   if (print_none_zero == 1) {
      if ((counter_LSB == 0) && (counter_MSB == 0)) {
         skip_first_print = 1;
      }
   }

   if (skip_first_print == 1) {
      cli_out("|   %-30s   %-24s +-------------------------------------------------------------|\n", "", "");
   } else {
      cli_out("|   %-30s = %-24s +-------------------------------------------------------------|\n", "RCY_PACKET_COUNTER", counter1_str);
   }
   skip_first_print = 0;


   common_read_reg_long(unit, "IRE_OAMP_PACKET_COUNTER", counter_data, 2);
   counter_MSB = counter_data[1];
   counter_LSB = counter_data[0];
   counter_OVF = get_field(counter_MSB, 31, 31);
   counter_MSB = get_field(counter_MSB, 0, 30);

   if (hex_base == 1) {
      if (counter_OVF == 0) {
         if (counter_MSB == 0) {sal_snprintf(counter1_str, sizeof(counter1_str), "0x%x", counter_LSB); } else                  {sal_snprintf(counter1_str, sizeof(counter1_str), "0x%x%x", counter_MSB, counter_LSB); }
      } else {
         if (counter_MSB == 0) {sal_snprintf(counter1_str, sizeof(counter1_str), "0x%x (ovf)", counter_LSB); } else                  {sal_snprintf(counter1_str, sizeof(counter1_str), "0x%x%x (ovf)", counter_MSB, counter_LSB); }
      }
   } else {
      COMPILER_64_SET(val_64, counter_MSB, counter_LSB);
      format_uint64_decimal(buf_val, val_64, commachr);
      if (counter_OVF == 0) {
         sal_snprintf(counter1_str, sizeof(counter1_str), "%s", buf_val);
      } else {
         sal_snprintf(counter1_str, sizeof(counter1_str), "%s (ovf)", buf_val);
      }
   }

   if (print_none_zero == 1) {
      if ((counter_LSB == 0) && (counter_MSB == 0)) {
         skip_first_print = 1;
      }
   }

   if (skip_first_print == 1) {
      cli_out("|   %-30s   %-24s |                             EGQ                             |\n", "", "");
   } else {
      cli_out("|   %-30s = %-24s |                             EGQ                             |\n", "OAMP_PACKET_COUNTER", counter1_str);
   }
   skip_first_print = 0;



   common_read_reg_long(unit, "IRE_OLP_PACKET_COUNTER", counter_data, 2);
   counter_MSB = counter_data[1];
   counter_LSB = counter_data[0];
   counter_OVF = get_field(counter_MSB, 31, 31);
   counter_MSB = get_field(counter_MSB, 0, 30);

   if (hex_base == 1) {
      if (counter_OVF == 0) {
         if (counter_MSB == 0) {sal_snprintf(counter1_str, sizeof(counter1_str), "0x%x", counter_LSB); } else                  {sal_snprintf(counter1_str, sizeof(counter1_str), "0x%x%x", counter_MSB, counter_LSB); }
      } else {
         if (counter_MSB == 0) {sal_snprintf(counter1_str, sizeof(counter1_str), "0x%x (ovf)", counter_LSB); } else                  {sal_snprintf(counter1_str, sizeof(counter1_str), "0x%x%x (ovf)", counter_MSB, counter_LSB); }
      }
   } else {
      COMPILER_64_SET(val_64, counter_MSB, counter_LSB);
      format_uint64_decimal(buf_val, val_64, commachr);
      if (counter_OVF == 0) {
         sal_snprintf(counter1_str, sizeof(counter1_str), "%s", buf_val);
      } else {
         sal_snprintf(counter1_str, sizeof(counter1_str), "%s (ovf)", buf_val);
      }
   }

   if (print_none_zero == 1) {
      if ((counter_LSB == 0) && (counter_MSB == 0)) {
         skip_first_print = 1;
      }
   }

   common_read_reg_long(unit, "EGQ_FQP_PACKET_COUNTER", counter_data, 2);
   counter_MSB = counter_data[1];
   counter_LSB = counter_data[0];
   counter_OVF = get_field(counter_MSB, 0, 0);

   if (hex_base == 1) {
      if (counter_OVF == 0) {
         sal_snprintf(counter2_str, sizeof(counter2_str), "0x%x", counter_LSB);
      } else {
         sal_snprintf(counter2_str, sizeof(counter2_str), "0x%x (ovf)", counter_LSB);
      }
   } else {
      COMPILER_64_SET(val_64, 0, counter_LSB);
      format_uint64_decimal(buf_val, val_64, commachr);
      if (counter_OVF == 0) {
         sal_snprintf(counter2_str, sizeof(counter2_str), "%s", buf_val);
      } else {
         sal_snprintf(counter2_str, sizeof(counter2_str), "%s (ovf)", buf_val);
      }
   }

   if (print_none_zero == 1) {
      if ((counter_LSB == 0) && (counter_MSB == 0)) {
         skip_second_print = 1;
      }
   }

   if ((skip_first_print == 1) && (skip_second_print == 1)) {
      cli_out("|   %-30s   %-24s |   %-30s   %-24s |\n", "", "", "", "");
   } else if ((skip_first_print == 1) && (skip_second_print == 0)) {
      cli_out("|   %-30s   %-24s |   %-30s = %-24s |\n", "", "", "FQP_PACKET_COUNTER", counter2_str);
   } else if ((skip_first_print == 0) && (skip_second_print == 1)) {
      cli_out("|   %-30s = %-24s |   %-30s   %-24s |\n", "OLP_PACKET_COUNTER", counter1_str, "", "");
   } else {
      cli_out("|   %-30s = %-24s |   %-30s = %-24s |\n", "OLP_PACKET_COUNTER", counter1_str, "FQP_PACKET_COUNTER", counter2_str);
   }
   skip_first_print = 0;
   skip_second_print = 0;

   cli_out("|-------------------------------------------------------------+                                                             |\n");

   common_read_reg_long(unit, "EGQ_PQP_UNICAST_PACKET_COUNTER", counter_data, 2);
   counter_MSB = counter_data[1];
   counter_LSB = counter_data[0];
   counter_OVF = get_field(counter_MSB, 0, 0);

   if (hex_base == 1) {
      if (counter_OVF == 0) {
         sal_snprintf(counter2_str, sizeof(counter2_str), "0x%x", counter_LSB);
      } else {
         sal_snprintf(counter2_str, sizeof(counter2_str), "0x%x (ovf)", counter_LSB);
      }
   } else {
      COMPILER_64_SET(val_64, 0, counter_LSB);
      format_uint64_decimal(buf_val, val_64, commachr);
      if (counter_OVF == 0) {
         sal_snprintf(counter2_str, sizeof(counter2_str), "%s", buf_val);
      } else {
         sal_snprintf(counter2_str, sizeof(counter2_str), "%s (ovf)", buf_val);
      }
   }

   if (print_none_zero == 1) {
      if ((counter_LSB == 0) && (counter_MSB == 0)) {
         skip_first_print = 1;
      }
   }

   if (skip_first_print == 1) {
      cli_out("|                                                             |   %-30s   %-24s |\n", "", "");
   } else {
      cli_out("|                                                             |   %-30s = %-24s |\n", "PQP_UNICAST_PACKET_COUNTER", counter2_str);
   }
   skip_first_print = 0;



   common_read_reg_long(unit, "EGQ_PQP_DISCARD_UNICAST_PACKET_COUNTER", counter_data, 2);
   counter_MSB = counter_data[1];
   counter_LSB = counter_data[0];
   counter_OVF = get_field(counter_MSB, 0, 0);

if (counter_LSB && SOC_IS_ARADPLUS(unit)) print_reason=1; /* if counter is on, we'll want to print the reasons */

   

   if (hex_base == 1) {
      if (counter_OVF == 0) {
         sal_snprintf(counter2_str, sizeof(counter2_str), "0x%x", counter_LSB);
      } else {
         sal_snprintf(counter2_str, sizeof(counter2_str), "0x%x (ovf)", counter_LSB);
      }
   } else {
      COMPILER_64_SET(val_64, 0, counter_LSB);
      format_uint64_decimal(buf_val, val_64, commachr);
      if (counter_OVF == 0) {
         sal_snprintf(counter2_str, sizeof(counter2_str), "%s", buf_val);
      } else {
         sal_snprintf(counter2_str, sizeof(counter2_str), "%s (ovf)", buf_val);
      }
   }

   if (print_none_zero == 1) {
      if ((counter_LSB == 0) && (counter_MSB == 0)) {
         skip_first_print = 1;
      }
   }

   if (skip_first_print == 1) {
      cli_out("|                                                             |   %-30s   %-24s |\n", "", "");
   } else {
      cli_out("|                                                             |   %-30s = %-24s |\n", "PQP_DISCARD_UC_PACKET_COUNTER", counter2_str);
   }
   skip_first_print = 0;

   common_read_reg_long(unit, "EGQ_PQP_UNICAST_BYTES_COUNTER", counter_data, 2);
   counter_LSB     = counter_data[0];
   counter_MSB     = counter_data[1];
   counter_OVF     = get_field(counter_MSB, 14, 14);
   counter_MSB     = get_field(counter_MSB, 0, 13);

   if (hex_base == 1) {
      if (counter_OVF == 0) {
         if (counter_MSB == 0) {sal_snprintf(counter2_str, sizeof(counter2_str), "0x%x", counter_LSB); } else                  {sal_snprintf(counter2_str, sizeof(counter2_str), "0x%x%x", counter_MSB, counter_LSB); }
      } else {
         if (counter_MSB == 0) {sal_snprintf(counter2_str, sizeof(counter2_str), "0x%x (ovf)", counter_LSB); } else                  {sal_snprintf(counter2_str, sizeof(counter2_str), "0x%x%x (ovf)", counter_MSB, counter_LSB); }
      }
   } else {
      COMPILER_64_SET(val_64, counter_MSB, counter_LSB);
      format_uint64_decimal(buf_val, val_64, commachr);
      if (counter_OVF == 0) {
         sal_snprintf(counter2_str, sizeof(counter2_str), "%s", buf_val);
      } else {
         sal_snprintf(counter2_str, sizeof(counter2_str), "%s (ovf)", buf_val);
      }
   }

   if (print_none_zero == 1) {
      if ((counter_LSB == 0) && (counter_MSB == 0)) {
         skip_first_print = 1;
      }
   }

   if (skip_first_print == 1) {
      cli_out("|                                                             |   %-30s   %-24s |\n", "", "");
   } else {
      cli_out("|                                                             |   %-30s = %-24s |\n", "PQP_UNICAST_BYTES_COUNTER", counter2_str);
   }
   skip_first_print = 0;



   common_read_reg_long(unit, "EGQ_PQP_MULTICAST_PACKET_COUNTER", counter_data, 2);
   counter_MSB = counter_data[1];
   counter_LSB = counter_data[0];
   counter_OVF = get_field(counter_MSB, 0, 0);

   if (hex_base == 1) {
      if (counter_OVF == 0) {
         sal_snprintf(counter2_str, sizeof(counter2_str), "0x%x", counter_LSB);
      } else {
         sal_snprintf(counter2_str, sizeof(counter2_str), "0x%x (ovf)", counter_LSB);
      }
   } else {
      COMPILER_64_SET(val_64, 0, counter_LSB);
      format_uint64_decimal(buf_val, val_64, commachr);
      if (counter_OVF == 0) {
         sal_snprintf(counter2_str, sizeof(counter2_str), "%s", buf_val);
      } else {
         sal_snprintf(counter2_str, sizeof(counter2_str), "%s (ovf)", buf_val);
      }
   }

   if (print_none_zero == 1) {
      if ((counter_LSB == 0) && (counter_MSB == 0)) {
         skip_first_print = 1;
      }
   }

   if (skip_first_print == 1) {
      cli_out("|                                                             |   %-30s   %-24s |\n", "", "");
   } else {
      cli_out("|                                                             |   %-30s = %-24s |\n", "PQP_MULTICAST_PACKET_COUNTER", counter2_str);
   }
   skip_first_print = 0;

   common_read_reg_long(unit, "EGQ_PQP_DISCARD_MULTICAST_PACKET_COUNTER", counter_data, 2);
   counter_MSB = counter_data[1];
   counter_LSB = counter_data[0];
   counter_OVF = get_field(counter_MSB, 0, 0);

if (counter_LSB && SOC_IS_ARADPLUS(unit)) print_reason=1; /* if counter is on, we'll want to print the reasons */


   if (hex_base == 1) {
      if (counter_OVF == 0) {
         sal_snprintf(counter2_str, sizeof(counter2_str), "0x%x", counter_LSB);
      } else {
         sal_snprintf(counter2_str, sizeof(counter2_str), "0x%x (ovf)", counter_LSB);
      }
   } else {
      COMPILER_64_SET(val_64, 0, counter_LSB);
      format_uint64_decimal(buf_val, val_64, commachr);
      if (counter_OVF == 0) {
         sal_snprintf(counter2_str, sizeof(counter2_str), "%s", buf_val);
      } else {
         sal_snprintf(counter2_str, sizeof(counter2_str), "%s (ovf)", buf_val);
      }
   }

   if (print_none_zero == 1) {
      if ((counter_LSB == 0) && (counter_MSB == 0)) {
         skip_first_print = 1;
      }
   }

   if (skip_first_print == 1) {
      cli_out("|                                                             |   %-30s   %-24s |\n", "", "");
   } else {
      cli_out("|                                                             |   %-30s = %-24s |\n", "PQP_DISCARD_MC_PACKET_COUNTER", counter2_str);
   }
   skip_first_print = 0;



   common_read_reg_long(unit, "EGQ_PQP_MULTICAST_BYTES_COUNTER", counter_data, 2);
   counter_LSB     = counter_data[0];
   counter_MSB     = counter_data[1];
   counter_OVF     = get_field(counter_MSB, 14, 14);
   counter_MSB     = get_field(counter_MSB, 0, 13);

   if (hex_base == 1) {
      if (counter_OVF == 0) {
         if (counter_MSB == 0) {sal_snprintf(counter2_str, sizeof(counter2_str), "0x%x", counter_LSB); } else                  {sal_snprintf(counter2_str, sizeof(counter2_str), "0x%x%x", counter_MSB, counter_LSB); }
      } else {
         if (counter_MSB == 0) {sal_snprintf(counter2_str, sizeof(counter2_str), "0x%x (ovf)", counter_LSB); } else                  {sal_snprintf(counter2_str, sizeof(counter2_str), "0x%x%x (ovf)", counter_MSB, counter_LSB); }
      }
   } else {
      COMPILER_64_SET(val_64, counter_MSB, counter_LSB);
      format_uint64_decimal(buf_val, val_64, commachr);
      if (counter_OVF == 0) {
         sal_snprintf(counter2_str, sizeof(counter2_str), "%s", buf_val);
      } else {
         sal_snprintf(counter2_str, sizeof(counter2_str), "%s (ovf)", buf_val);
      }
   }

   if (print_none_zero == 1) {
      if ((counter_LSB == 0) && (counter_MSB == 0)) {
         skip_first_print = 1;
      }
   }

   if (skip_first_print == 1) {
      cli_out("|                                                             |   %-30s   %-24s |\n", "", "");
   } else {
      cli_out("|                                                             |   %-30s = %-24s |\n", "PQP_MULTICAST_BYTES_COUNTER", counter2_str);
   }
   skip_first_print = 0;


   if (print_reason) diag_print_pqp_discard_reasons(unit); /* if ARAD+ and counter was on, print reasons*/
   print_reason=0;       

   

   cli_out("|-------------------------------------------------------------+                                                             |\n");

   common_read_reg_long(unit, "EGQ_EHP_UNICAST_PACKET_COUNTER", counter_data, 2);
   counter_MSB = counter_data[1];
   counter_LSB = counter_data[0];
   counter_OVF = get_field(counter_MSB, 0, 0);

   if (hex_base == 1) {
      if (counter_OVF == 0) {
         sal_snprintf(counter2_str, sizeof(counter2_str), "0x%x", counter_LSB);
      } else {
         sal_snprintf(counter2_str, sizeof(counter2_str), "0x%x (ovf)", counter_LSB);
      }
   } else {
      COMPILER_64_SET(val_64, 0, counter_LSB);
      format_uint64_decimal(buf_val, val_64, commachr);
      if (counter_OVF == 0) {
         sal_snprintf(counter2_str, sizeof(counter2_str), "%s", buf_val);
      } else {
         sal_snprintf(counter2_str, sizeof(counter2_str), "%s (ovf)", buf_val);
      }
   }

   if (print_none_zero == 1) {
      if ((counter_LSB == 0) && (counter_MSB == 0)) {
         skip_first_print = 1;
      }
   }

   if (skip_first_print == 1) {
      cli_out("|                             IQM                             |   %-30s   %-24s |\n", "", "");
   } else {
      cli_out("|                             IQM                             |   %-30s = %-24s |\n", "EHP_UNICAST_PACKET_COUNTER", counter2_str);
   }
   skip_first_print = 0;



   common_read_reg_long(unit, "IQM_ENQUEUE_PACKET_COUNTER", counter_data, 1);
   counter_LSB = counter_data[0];
   counter_OVF = get_field(counter_LSB, 31, 31);
   counter_LSB = get_field(counter_LSB, 0, 30);

   if (hex_base == 1) {
      if (counter_OVF == 0) {
         sal_snprintf(counter1_str, sizeof(counter1_str), "0x%x", counter_LSB);
      } else {
         sal_snprintf(counter1_str, sizeof(counter1_str), "0x%x (ovf)", counter_LSB);
      }
   } else {
      COMPILER_64_SET(val_64, 0, counter_LSB);
      format_uint64_decimal(buf_val, val_64, commachr);
      if (counter_OVF == 0) {
         sal_snprintf(counter1_str, sizeof(counter1_str), "%s", buf_val);
      } else {
         sal_snprintf(counter1_str, sizeof(counter1_str), "%s (ovf)", buf_val);
      }
   }

   if (print_none_zero == 1) {
      if (counter_LSB == 0) {
         skip_first_print = 1;
      }
   }

   common_read_reg_long(unit, "EGQ_EHP_MULTICAST_HIGH_PACKET_COUNTER", counter_data, 2);
   counter_MSB = counter_data[1];
   counter_LSB = counter_data[0];
   counter_OVF = get_field(counter_MSB, 0, 0);

   if (hex_base == 1) {
      if (counter_OVF == 0) {
         sal_snprintf(counter2_str, sizeof(counter2_str), "0x%x", counter_LSB);
      } else {
         sal_snprintf(counter2_str, sizeof(counter2_str), "0x%x (ovf)", counter_LSB);
      }
   } else {
      COMPILER_64_SET(val_64, counter_MSB, counter_LSB);
      format_uint64_decimal(buf_val, val_64, commachr);
      if (counter_OVF == 0) {
         sal_snprintf(counter2_str, sizeof(counter2_str), "%s", buf_val);
      } else {
         sal_snprintf(counter2_str, sizeof(counter2_str), "%s (ovf)", buf_val);
      }
   }

   if (print_none_zero == 1) {
      if ((counter_LSB == 0) && (counter_MSB == 0)) {
         skip_second_print = 1;
      }
   }

   if ((skip_first_print == 1) && (skip_second_print == 1)) {
      cli_out("|   %-30s   %-24s |   %-30s   %-24s |\n", "", "", "", "");
   } else if ((skip_first_print == 1) && (skip_second_print == 0)) {
      cli_out("|   %-30s   %-24s |   %-30s = %-24s |\n", "", "", "EHP_MC_HIGH_PACKET_COUNTER", counter2_str);
   } else if ((skip_first_print == 0) && (skip_second_print == 1)) {
      cli_out("|   %-30s = %-24s |   %-30s   %-24s |\n", "ENQUEUE_PACKET_COUNTER", counter1_str, "", "");
   } else {
      cli_out("|   %-30s = %-24s |   %-30s = %-24s |\n", "ENQUEUE_PACKET_COUNTER", counter1_str, "EHP_MC_HIGH_PACKET_COUNTER", counter2_str);
   }
   skip_first_print = 0;
   skip_second_print = 0;


   common_read_reg_long(unit, "IQM_DEQUEUE_PACKET_COUNTER", counter_data, 1);
   counter_LSB = counter_data[0];
   counter_OVF = get_field(counter_LSB, 31, 31);
   counter_LSB = get_field(counter_LSB, 0, 30);

   if (hex_base == 1) {
      if (counter_OVF == 0) {
         sal_snprintf(counter1_str, sizeof(counter1_str), "0x%x", counter_LSB);
      } else {
         sal_snprintf(counter1_str, sizeof(counter1_str), "0x%x (ovf)", counter_LSB);
      }
   } else {
      COMPILER_64_SET(val_64, 0, counter_LSB);
      format_uint64_decimal(buf_val, val_64, commachr);
      if (counter_OVF == 0) {
         sal_snprintf(counter1_str, sizeof(counter1_str), "%s", buf_val);
      } else {
         sal_snprintf(counter1_str, sizeof(counter1_str), "%s (ovf)", buf_val);
      }
   }

   if (print_none_zero == 1) {
      if (counter_LSB == 0) {
         skip_first_print = 1;
      }
   }

   common_read_reg_long(unit, "EGQ_EHP_MULTICAST_LOW_PACKET_COUNTER", counter_data, 2);
   counter_MSB = counter_data[1];
   counter_LSB = counter_data[0];
   counter_OVF = get_field(counter_MSB, 0, 0);

   if (hex_base == 1) {
      if (counter_OVF == 0) {
         sal_snprintf(counter2_str, sizeof(counter2_str), "0x%x", counter_LSB);
      } else {
         sal_snprintf(counter2_str, sizeof(counter2_str), "0x%x (ovf)", counter_LSB);
      }
   } else {
      COMPILER_64_SET(val_64, 0, counter_LSB);
      format_uint64_decimal(buf_val, val_64, commachr);
      if (counter_OVF == 0) {
         sal_snprintf(counter2_str, sizeof(counter2_str), "%s", buf_val);
      } else {
         sal_snprintf(counter2_str, sizeof(counter2_str), "%s (ovf)", buf_val);
      }
   }


   if (print_none_zero == 1) {
      if ((counter_LSB == 0) && (counter_MSB == 0)) {
         skip_second_print = 1;
      }
   }

   if ((skip_first_print == 1) && (skip_second_print == 1)) {
      cli_out("|   %-30s   %-24s |   %-30s   %-24s |\n", "", "", "", "");
   } else if ((skip_first_print == 1) && (skip_second_print == 0)) {
      cli_out("|   %-30s   %-24s |   %-30s = %-24s |\n", "", "", "EHP_MC_LOW_PACKET_COUNTER", counter2_str);
   } else if ((skip_first_print == 0) && (skip_second_print == 1)) {
      cli_out("|   %-30s = %-24s |   %-30s   %-24s |\n", "DEQUEUE_PACKET_COUNTER", counter1_str, "", "");
   } else {
      cli_out("|   %-30s = %-24s |   %-30s = %-24s |\n", "DEQUEUE_PACKET_COUNTER", counter1_str, "EHP_MC_LOW_PACKET_COUNTER", counter2_str);
   }
   skip_first_print = 0;
   skip_second_print = 0;


   common_read_reg_long(unit, "IQM_TOTAL_DISCARDED_PACKET_COUNTER", counter_data, 1);
   counter_LSB = counter_data[0];
   counter_OVF = get_field(counter_LSB, 31, 31);
   counter_LSB = get_field(counter_LSB, 0, 30);

   if (hex_base == 1) {
      if (counter_OVF == 0) {
         sal_snprintf(counter1_str, sizeof(counter1_str), "0x%x", counter_LSB);
      } else {
         sal_snprintf(counter1_str, sizeof(counter1_str), "0x%x (ovf)", counter_LSB);
      }
   } else {
      COMPILER_64_SET(val_64, 0, counter_LSB);
      format_uint64_decimal(buf_val, val_64, commachr);
      if (counter_OVF == 0) {
         sal_snprintf(counter1_str, sizeof(counter1_str), "%s", buf_val);
      } else {
         sal_snprintf(counter1_str, sizeof(counter1_str), "%s (ovf)", buf_val);
      }
   }

   if (print_none_zero == 1) {
      if (counter_LSB == 0) {
         skip_first_print = 1;
      }
   }

   common_read_reg_long(unit, "EGQ_EHP_DISCARD_PACKET_COUNTER", counter_data, 2);
   counter_MSB = counter_data[1];
   counter_LSB = counter_data[0];
   counter_OVF = get_field(counter_MSB, 0, 0);


   if (hex_base == 1) {
      if (counter_OVF == 0) {
         sal_snprintf(counter2_str, sizeof(counter2_str), "0x%x", counter_LSB);
      } else {
         sal_snprintf(counter2_str, sizeof(counter2_str), "0x%x (ovf)", counter_LSB);
      }
   } else {
      COMPILER_64_SET(val_64, 0, counter_LSB);
      format_uint64_decimal(buf_val, val_64, commachr);
      if (counter_OVF == 0) {
         sal_snprintf(counter2_str, sizeof(counter2_str), "%s", buf_val);
      } else {
         sal_snprintf(counter2_str, sizeof(counter2_str), "%s (ovf)", buf_val);
      }
   }

   if (print_none_zero == 1) {
      if ((counter_LSB == 0) && (counter_MSB == 0)) {
         skip_second_print = 1;
      }
   }

   if ((skip_first_print == 1) && (skip_second_print == 1)) {
      cli_out("|   %-30s   %-24s |   %-30s   %-24s |\n", "", "", "", "");
   } else if ((skip_first_print == 1) && (skip_second_print == 0)) {
      cli_out("|   %-30s   %-24s |   %-30s = %-24s |\n", "", "", "EHP_DISCARD_PACKET_COUNTER", counter2_str);
   } else if ((skip_first_print == 0) && (skip_second_print == 1)) {
      cli_out("|   %-30s = %-24s |   %-30s   %-24s |\n", "TOTAL_DISCARDED_PACKET_COUNTER", counter1_str, "", "");
   } else {
      cli_out("|   %-30s = %-24s |   %-30s = %-24s |\n", "TOTAL_DISCARDED_PACKET_COUNTER", counter1_str, "EHP_DISCARD_PACKET_COUNTER", counter2_str);
   }

   rc = diag_print_iqm_rejects_and_erpp_discards(unit, FALSE);
   if (rc != CMD_OK) {
       return rc;
   }

   skip_first_print = 0;
   skip_second_print = 0;

   common_read_reg_long(unit, "IQM_DELETED_PACKET_COUNTER", counter_data, 1);
   counter_LSB = counter_data[0];
   counter_OVF = get_field(counter_LSB, 31, 31);
   counter_LSB = get_field(counter_LSB, 0, 30);

   if (hex_base == 1) {
      if (counter_OVF == 0) {
         sal_snprintf(counter1_str, sizeof(counter1_str), "0x%x", counter_LSB);
      } else {
         sal_snprintf(counter1_str, sizeof(counter1_str), "0x%x (ovf)", counter_LSB);
      }
   } else {
      COMPILER_64_SET(val_64, 0, counter_LSB);
      format_uint64_decimal(buf_val, val_64, commachr);
      if (counter_OVF == 0) {
         sal_snprintf(counter1_str, sizeof(counter1_str), "%s", buf_val);
      } else {
         sal_snprintf(counter1_str, sizeof(counter1_str), "%s (ovf)", buf_val);
      }
   }

   if (print_none_zero == 1) {
      if (counter_LSB == 0) {
         skip_first_print = 1;
      }
   }
   if (skip_first_print == 1) {
      cli_out("|   %-30s   %-24s |                                                             |\n", "", "");
   } else {
      cli_out("|   %-30s = %-24s |                                                             |\n", "DELETED_PACKET_COUNTER", counter1_str);
   }
   skip_first_print = 0;

   common_read_reg_long(unit, "EGQ_RQP_PACKET_COUNTER", counter_data, 2);
   counter_MSB = counter_data[1];
   counter_LSB = counter_data[0];
   counter_OVF = get_field(counter_MSB, 0, 0);

   if (hex_base == 1) {
      if (counter_OVF == 0) {
         sal_snprintf(counter2_str, sizeof(counter2_str), "0x%x", counter_LSB);
      } else {
         sal_snprintf(counter2_str, sizeof(counter2_str), "0x%x (ovf)", counter_LSB);
      }
   } else {
      COMPILER_64_SET(val_64, 0, counter_LSB);
      format_uint64_decimal(buf_val, val_64, commachr);
      if (counter_OVF == 0) {
         sal_snprintf(counter2_str, sizeof(counter2_str), "%s", buf_val);
      } else {
         sal_snprintf(counter2_str, sizeof(counter2_str), "%s (ovf)", buf_val);
      }
   }

   if (print_none_zero == 1) {
      if ((counter_LSB == 0) && (counter_MSB == 0)) {
         skip_first_print = 1;
      }
   }

   if (skip_first_print == 1) {
      cli_out("|-------------------------------------------------------------+   %-30s   %-24s |\n", "", "");
   } else {
      cli_out("|-------------------------------------------------------------+   %-30s = %-24s |\n", "RQP_PACKET_COUNTER", counter2_str);
   }
   skip_first_print = 0;



   common_read_reg_long(unit, "EGQ_RQP_DISCARD_PACKET_COUNTER", counter_data, 2);
   counter_MSB = counter_data[1];
   counter_LSB = counter_data[0];
   counter_OVF = get_field(counter_MSB, 0, 0);

   if (counter_LSB && SOC_IS_ARADPLUS(unit)) print_reason=1; /* if counter is on, we'll want to print the reasons */

   if (hex_base == 1) {
      if (counter_OVF == 0) {
         sal_snprintf(counter2_str, sizeof(counter2_str), "0x%x", counter_LSB);
      } else {
         sal_snprintf(counter2_str, sizeof(counter2_str), "0x%x (ovf)", counter_LSB);
      }
   } else {
      COMPILER_64_SET(val_64, 0, counter_LSB);
      format_uint64_decimal(buf_val, val_64, commachr);
      if (counter_OVF == 0) {
         sal_snprintf(counter2_str, sizeof(counter2_str), "%s", buf_val);
      } else {
         sal_snprintf(counter2_str, sizeof(counter2_str), "%s (ovf)", buf_val);
      }
   }

   if (print_none_zero == 1) {
      if ((counter_LSB == 0) && (counter_MSB == 0)) {
         skip_first_print = 1;
      }
   }
   if (skip_first_print == 1) {
      cli_out("|                             IPT                             |   %-30s   %-24s |\n", "", "");
   } else {
      cli_out("|                             IPT                             |   %-30s = %-24s |\n", "RQP_DISCARD_PACKET_COUNTER", counter2_str);
   }
   skip_first_print = 0;

   if (print_reason && SOC_IS_ARADPLUS(unit)) diag_print_rqp_discard_reasons(unit); /* if ARAD+ and counter was on, print reasons*/
   print_reason=0;       

   common_read_reg_long(unit, "IPT_ENQ_PKT_CNT", counter_data, 1);
   counter_LSB = counter_data[0];
   counter_OVF = get_field(counter_LSB, 31, 31);
   counter_LSB = get_field(counter_LSB, 0, 30);


   if (hex_base == 1) {
      if (counter_OVF == 0) {
         sal_snprintf(counter1_str, sizeof(counter1_str), "0x%x", counter_LSB);
      } else {
         sal_snprintf(counter1_str, sizeof(counter1_str), "0x%x (ovf)", counter_LSB);
      }
   } else {
      COMPILER_64_SET(val_64, 0, counter_LSB);
      format_uint64_decimal(buf_val, val_64, commachr);
      if (counter_OVF == 0) {
         sal_snprintf(counter1_str, sizeof(counter1_str), "%s", buf_val);
      } else {
         sal_snprintf(counter1_str, sizeof(counter1_str), "%s (ovf)", buf_val);
      }
   }

   if (print_none_zero == 1) {
      if (counter_LSB == 0) {
         skip_first_print = 1;
      }
   }

   if (SOC_IS_ARADPLUS(unit)) {
       common_read_reg_long(unit, "EGQ_RQP_DISCARD_SOP_COUNTER", counter_data, 1);
       counter_LSB = counter_data[0];
       counter_OVF = get_field(counter_LSB, 31, 31);
       counter_LSB = get_field(counter_LSB, 0, 30);


       if (hex_base == 1) {
          if (counter_OVF == 0) {
             sal_snprintf(counter2_str, sizeof(counter2_str), "0x%x", counter_LSB);
          } else {
             sal_snprintf(counter2_str, sizeof(counter2_str), "0x%x (ovf)", counter_LSB);
          }
       } else {
          COMPILER_64_SET(val_64, 0, counter_LSB);
          format_uint64_decimal(buf_val, val_64, commachr);
          if (counter_OVF == 0) {
             sal_snprintf(counter2_str, sizeof(counter2_str), "%s", buf_val);
          } else {
             sal_snprintf(counter2_str, sizeof(counter2_str), "%s (ovf)", buf_val);
          }
       }

       if (print_none_zero == 1) {
          if (counter_LSB == 0) {
             skip_second_print = 1;
          }
       }
   } else
   {
       skip_second_print = 1;
   }


   if ((skip_first_print == 1) && (skip_second_print == 1)) {
      cli_out("|   %-30s   %-24s |   %-30s   %-24s |\n", "", "", "", "");
   } else if ((skip_first_print == 1) && (skip_second_print == 0)) {
      cli_out("|   %-30s   %-24s |   %-30s = %-24s |\n", "", "", "RQP_DISCARD_SOP_COUNTER", counter2_str);
   } else if ((skip_first_print == 0) && (skip_second_print == 1)) {
      cli_out("|   %-30s = %-24s |   %-30s   %-24s |\n", "ENQ_PKT_CNT", counter1_str, "", "");
   } else {
      cli_out("|   %-30s = %-24s |   %-30s = %-24s |\n", "ENQ_PKT_CNT", counter1_str, "RQP_DISCARD_SOP_COUNTER", counter2_str);
   }
   skip_first_print = 0;
   skip_second_print = 0;
   cli_out("|                                                             |                                                             |\n");


   common_read_reg_long(unit, "IPT_CRC_ERROR_COUNTER", counter_data, 1);
   counter_LSB = counter_data[0];
   counter_OVF = get_field(counter_LSB, 31, 31);
   counter_LSB = get_field(counter_LSB, 0, 30);

   if (hex_base == 1) {
      if (counter_OVF == 0) {
         sal_snprintf(counter1_str, sizeof(counter1_str), "0x%x", counter_LSB);
      } else {
         sal_snprintf(counter1_str, sizeof(counter1_str), "0x%x (ovf)", counter_LSB);
      }
   } else {
      COMPILER_64_SET(val_64, 0, counter_LSB);
      format_uint64_decimal(buf_val, val_64, commachr);
      if (counter_OVF == 0) {
         sal_snprintf(counter1_str, sizeof(counter1_str), "%s", buf_val);
      } else {
         sal_snprintf(counter1_str, sizeof(counter1_str), "%s (ovf)", buf_val);
      }
   }

   if (print_none_zero == 1) {
      if (counter_LSB == 0) {
         skip_first_print = 1;
      }
   }

   common_read_reg_long(unit, "EGQ_CPU_PACKET_COUNTER", counter_data, 2);
   counter_MSB = counter_data[1];
   counter_LSB = counter_data[0];
   counter_LSB = get_field(counter_LSB, 0, 31);
   counter_OVF = get_field(counter_MSB, 0, 0);

   if (hex_base == 1) {
      if (counter_OVF == 0) {
         sal_snprintf(counter2_str, sizeof(counter2_str), "0x%x", counter_LSB);
      } else {
         sal_snprintf(counter2_str, sizeof(counter2_str), "0x%x (ovf)", counter_LSB);
      }
   } else {
      COMPILER_64_SET(val_64, 0, counter_LSB);
      format_uint64_decimal(buf_val, val_64, commachr);
      if (counter_OVF == 0) {
         sal_snprintf(counter2_str, sizeof(counter2_str), "%s", buf_val);
      } else {
         sal_snprintf(counter2_str, sizeof(counter2_str), "%s (ovf)", buf_val);
      }
   }

   if (print_none_zero == 1) {
      if ((counter_LSB == 0) && (counter_MSB == 0)) {
         skip_second_print = 1;
      }
   }

   if ((skip_first_print == 1) && (skip_second_print == 1)) {
      cli_out("|   %-30s   %-24s |   %-30s   %-24s |\n", "", "", "", "");
   } else if ((skip_first_print == 1) && (skip_second_print == 0)) {
      cli_out("|   %-30s   %-24s |   %-30s = %-24s |\n", "", "", "CPU_PACKET_COUNTER", counter2_str);
   } else if ((skip_first_print == 0) && (skip_second_print == 1)) {
      cli_out("|   %-30s = %-24s |   %-30s   %-24s |\n", "CRC_ERROR_COUNTER", counter1_str, "", "");
   } else {
      cli_out("|   %-30s = %-24s |   %-30s = %-24s |\n", "CRC_ERROR_COUNTER", counter1_str, "CPU_PACKET_COUNTER", counter2_str);
   }
   skip_first_print = 0;
   skip_second_print = 0;


   cli_out("|                                                             |                                                             |\n");

   common_read_reg_long(unit, "IPT_EGQ_PKT_CNT", counter_data, 1);
   counter_LSB = counter_data[0];
   counter_OVF = get_field(counter_LSB, 31, 31);
   counter_LSB = get_field(counter_LSB, 0, 30);

   if (hex_base == 1) {
      if (counter_OVF == 0) {
         sal_snprintf(counter1_str, sizeof(counter1_str), "0x%x", counter_LSB);
      } else {
         sal_snprintf(counter1_str, sizeof(counter1_str), "0x%x (ovf)", counter_LSB);
      }
   } else {
      COMPILER_64_SET(val_64, 0, counter_LSB);
      format_uint64_decimal(buf_val, val_64, commachr);
      if (counter_OVF == 0) {
         sal_snprintf(counter1_str, sizeof(counter1_str), "%s", buf_val);
      } else {
         sal_snprintf(counter1_str, sizeof(counter1_str), "%s (ovf)", buf_val);
      }
   }

   if (print_none_zero == 1) {
      if (counter_LSB == 0) {
         skip_first_print = 1;
      }
   }

   common_read_reg_long(unit, "EGQ_IPT_PACKET_COUNTER", counter_data, 2);
   counter_MSB = counter_data[1];
   counter_LSB = counter_data[0];
   counter_LSB = get_field(counter_LSB, 0, 31);
   counter_OVF = get_field(counter_MSB, 0, 0);


   if (hex_base == 1) {
      if (counter_OVF == 0) {
         sal_snprintf(counter2_str, sizeof(counter2_str), "0x%x", counter_LSB);
      } else {
         sal_snprintf(counter2_str, sizeof(counter2_str), "0x%x (ovf)", counter_LSB);
      }
   } else {
      COMPILER_64_SET(val_64, 0, counter_LSB);
      format_uint64_decimal(buf_val, val_64, commachr);
      if (counter_OVF == 0) {
         sal_snprintf(counter2_str, sizeof(counter2_str), "%s", buf_val);
      } else {
         sal_snprintf(counter2_str, sizeof(counter2_str), "%s (ovf)", buf_val);
      }
   }

   if (print_none_zero == 1) {
      if ((counter_LSB == 0) && (counter_MSB == 0)) {
         skip_second_print = 1;
      }
   }

   if ((skip_first_print == 1) && (skip_second_print == 1)) {
      cli_out("|   %-30s   %-24s-->  %-30s   %-24s |\n", "", "", "", "");
   } else if ((skip_first_print == 1) && (skip_second_print == 0)) {
      cli_out("|   %-30s   %-24s-->  %-30s = %-24s |\n", "", "", "IPT_PACKET_COUNTER", counter2_str);
   } else if ((skip_first_print == 0) && (skip_second_print == 1)) {
      cli_out("|   %-30s = %-24s-->  %-30s   %-24s |\n", "EGQ_PKT_CNT", counter1_str, "", "");
   } else {
      cli_out("|   %-30s = %-24s-->  %-30s = %-24s |\n", "EGQ_PKT_CNT", counter1_str, "IPT_PACKET_COUNTER", counter2_str);
   }
   skip_first_print = 0;
   skip_second_print = 0;


   cli_out("|                                                             |                                                             |\n");

   common_read_reg_long(unit, "EGQ_FDR_PRIMARY_PACKET_COUNTER", counter_data, 2);
   counter_MSB = counter_data[1];
   counter_LSB = counter_data[0];
   counter_OVF = get_field(counter_MSB, 0, 0);

   if (hex_base == 1) {
      if (counter_OVF == 0) {
         sal_snprintf(counter2_str, sizeof(counter2_str), "0x%x", counter_LSB);
      } else {
         sal_snprintf(counter2_str, sizeof(counter2_str), "0x%x (ovf)", counter_LSB);
      }
   } else {
      COMPILER_64_SET(val_64, 0, counter_LSB);
      format_uint64_decimal(buf_val, val_64, commachr);
      if (counter_OVF == 0) {
         sal_snprintf(counter2_str, sizeof(counter2_str), "%s", buf_val);
      } else {
         sal_snprintf(counter2_str, sizeof(counter2_str), "%s (ovf)", buf_val);
      }
   }

   if (print_none_zero == 1) {
      if ((counter_LSB == 0) && (counter_MSB == 0)) {
         skip_first_print = 1;
      }
   }

   if (skip_first_print == 1) {
      cli_out("|                                                             |   %-30s   %-24s |\n", "", "");
   } else {
      cli_out("|                                                             |   %-30s = %-24s |\n", "FDR_PRIMARY_PACKET_COUNTER", counter2_str);
   }
   skip_first_print = 0;


   common_read_reg_long(unit, "IPT_FDT_PKT_CNT", counter_data, 1);
   counter_LSB = counter_data[0];
   counter_OVF = get_field(counter_LSB, 31, 31);
   counter_LSB = get_field(counter_LSB, 0, 30);

   if (hex_base == 1) {
      if (counter_OVF == 0) {
         sal_snprintf(counter1_str, sizeof(counter1_str), "0x%x", counter_LSB);
      } else {
         sal_snprintf(counter1_str, sizeof(counter1_str), "0x%x (ovf)", counter_LSB);
      }
   } else {
      COMPILER_64_SET(val_64, 0, counter_LSB);
      format_uint64_decimal(buf_val, val_64, commachr);
      if (counter_OVF == 0) {
         sal_snprintf(counter1_str, sizeof(counter1_str), "%s", buf_val);
      } else {
         sal_snprintf(counter1_str, sizeof(counter1_str), "%s (ovf)", buf_val);
      }
   }

   if (print_none_zero == 1) {
      if (counter_LSB == 0) {
         skip_first_print = 1;
      }
   }

   common_read_reg_long(unit, "EGQ_FDR_SECONDARY_PACKET_COUNTER", counter_data, 2);
   counter_MSB = counter_data[1];
   counter_LSB = counter_data[0];
   counter_OVF = get_field(counter_MSB, 0, 0);


   if (hex_base == 1) {
      if (counter_OVF == 0) {
         sal_snprintf(counter2_str, sizeof(counter2_str), "0x%x", counter_LSB);
      } else {
         sal_snprintf(counter2_str, sizeof(counter2_str), "0x%x (ovf)", counter_LSB);
      }
   } else {
      COMPILER_64_SET(val_64, 0, counter_LSB);
      format_uint64_decimal(buf_val, val_64, commachr);
      if (counter_OVF == 0) {
         sal_snprintf(counter2_str, sizeof(counter2_str), "%s", buf_val);
      } else {
         sal_snprintf(counter2_str, sizeof(counter2_str), "%s (ovf)", buf_val);
      }
   }

   if (print_none_zero == 1) {
      if ((counter_LSB == 0) && (counter_MSB == 0)) {
         skip_second_print = 1;
      }
   }

   if ((skip_first_print == 1) && (skip_second_print == 1)) {
      cli_out("|   %-30s   %-24s |   %-30s   %-24s |\n", "", "", "", "");
   } else if ((skip_first_print == 1) && (skip_second_print == 0)) {
      cli_out("|   %-30s   %-24s |   %-30s = %-24s |\n", "", "", "FDR_SECONDARY_PACKET_COUNTER", counter2_str);
   } else if ((skip_first_print == 0) && (skip_second_print == 1)) {
      cli_out("|   %-30s = %-24s |   %-30s   %-24s |\n", "FDT_PKT_CNT", counter1_str, "", "");
   } else {
      cli_out("|   %-30s = %-24s |   %-30s = %-24s |\n", "FDT_PKT_CNT", counter1_str, "FDR_SECONDARY_PACKET_COUNTER", counter2_str);
   }
   skip_first_print = 0;
   skip_second_print = 0;



   cli_out("|-------------------------------------------------------------+-------------------------------------------------------------|\n");
   cli_out("|                             FDT                             |                             FDR                             |\n");

   common_read_reg_long(unit, "FDT_IPT_DESC_CELL_COUNTER", counter_data, 1);
   counter_LSB = counter_data[0];
   counter_OVF = get_field(counter_LSB, 31, 31);
   counter_LSB = get_field(counter_LSB, 0, 30);

   if (hex_base == 1) {
      if (counter_OVF == 0) {
         sal_snprintf(counter1_str, sizeof(counter1_str), "0x%x", counter_LSB);
      } else {
         sal_snprintf(counter1_str, sizeof(counter1_str), "0x%x (ovf)", counter_LSB);
      }
   } else {
      COMPILER_64_SET(val_64, 0, counter_LSB);
      format_uint64_decimal(buf_val, val_64, commachr);
      if (counter_OVF == 0) {
         sal_snprintf(counter1_str, sizeof(counter1_str), "%s", buf_val);
      } else {
         sal_snprintf(counter1_str, sizeof(counter1_str), "%s (ovf)", buf_val);
      }
   }

   if (print_none_zero == 1) {
      if (counter_LSB == 0) {
         skip_first_print = 1;
      }
   }

   common_read_reg_long(unit, "FDR_CELL_OUT_CNT_PRIMARY", counter_data, 1);
   counter_LSB = counter_data[0];
   counter_OVF = get_field(counter_LSB, 31, 31);
   counter_LSB = get_field(counter_LSB, 0, 30);

   if (hex_base == 1) {
      if (counter_OVF == 0) {
         sal_snprintf(counter2_str, sizeof(counter2_str), "0x%x", counter_LSB);
      } else {
         sal_snprintf(counter2_str, sizeof(counter2_str), "0x%x (ovf)", counter_LSB);
      }
   } else {
      COMPILER_64_SET(val_64, 0, counter_LSB);
      format_uint64_decimal(buf_val, val_64, commachr);
      if (counter_OVF == 0) {
         sal_snprintf(counter2_str, sizeof(counter2_str), "%s", buf_val);
      } else {
         sal_snprintf(counter2_str, sizeof(counter2_str), "%s (ovf)", buf_val);
      }
   }

   if (print_none_zero == 1) {
      if ((counter_LSB == 0) && (counter_MSB == 0)) {
         skip_second_print = 1;
      }
   }

   if ((skip_first_print == 1) && (skip_second_print == 1)) {
      cli_out("|   %-30s   %-24s |   %-30s   %-24s |\n", "", "", "", "");
   } else if ((skip_first_print == 1) && (skip_second_print == 0)) {
      cli_out("|   %-30s   %-24s |   %-30s = %-24s |\n", "", "", "CELL_OUT_CNT_PRIMARY", counter2_str);
   } else if ((skip_first_print == 0) && (skip_second_print == 1)) {
      cli_out("|   %-30s = %-24s |   %-30s   %-24s |\n", "IPT_DESC_CELL_COUNTER", counter1_str, "", "");
   } else {
      cli_out("|   %-30s = %-24s |   %-30s = %-24s |\n", "IPT_DESC_CELL_COUNTER", counter1_str, "CELL_OUT_CNT_PRIMARY", counter2_str);
   }
   skip_first_print = 0;
   skip_second_print = 0;



   common_read_reg_long(unit, "FDT_IRE_DESC_CELL_COUNTER", counter_data, 1);
   counter_LSB = counter_data[0];
   counter_OVF = get_field(counter_LSB, 31, 31);
   counter_LSB = get_field(counter_LSB, 0, 30);

   if (hex_base == 1) {
      if (counter_OVF == 0) {
         sal_snprintf(counter1_str, sizeof(counter1_str), "0x%x", counter_LSB);
      } else {
         sal_snprintf(counter1_str, sizeof(counter1_str), "0x%x (ovf)", counter_LSB);
      }
   } else {
      COMPILER_64_SET(val_64, 0, counter_LSB);
      format_uint64_decimal(buf_val, val_64, commachr);
      if (counter_OVF == 0) {
         sal_snprintf(counter1_str, sizeof(counter1_str), "%s", buf_val);
      } else {
         sal_snprintf(counter1_str, sizeof(counter1_str), "%s (ovf)", buf_val);
      }
   }
   if (print_none_zero == 1) {
      if (counter_LSB == 0) {
         skip_first_print = 1;
      }
   }
   common_read_reg_long(unit, "FDR_CELL_OUT_CNT_SECONDARY", counter_data, 1);
   counter_LSB = counter_data[0];
   counter_OVF = get_field(counter_LSB, 31, 31);
   counter_LSB = get_field(counter_LSB, 0, 30);

   if (hex_base == 1) {
      if (counter_OVF == 0) {
         sal_snprintf(counter2_str, sizeof(counter2_str), "0x%x", counter_LSB);
      } else {
         sal_snprintf(counter2_str, sizeof(counter2_str), "0x%x (ovf)", counter_LSB);
      }
   } else {
      COMPILER_64_SET(val_64, 0, counter_LSB);
      format_uint64_decimal(buf_val, val_64, commachr);
      if (counter_OVF == 0) {
         sal_snprintf(counter2_str, sizeof(counter2_str), "%s", buf_val);
      } else {
         sal_snprintf(counter2_str, sizeof(counter2_str), "%s (ovf)", buf_val);
      }
   }

   if (print_none_zero == 1) {
      if ((counter_LSB == 0) && (counter_MSB == 0)) {
         skip_second_print = 1;
      }
   }

   if ((skip_first_print == 1) && (skip_second_print == 1)) {
      cli_out("|   %-30s   %-24s |   %-30s   %-24s |\n", "", "", "", "");
   } else if ((skip_first_print == 1) && (skip_second_print == 0)) {
      cli_out("|   %-30s   %-24s |   %-30s = %-24s |\n", "", "", "CELL_OUT_CNT_SECONDARY", counter2_str);
   } else if ((skip_first_print == 0) && (skip_second_print == 1)) {
      cli_out("|   %-30s = %-24s |   %-30s   %-24s |\n", "IRE_DESC_CELL_COUNTER", counter1_str, "", "");
   } else {
      cli_out("|   %-30s = %-24s |   %-30s = %-24s |\n", "IRE_DESC_CELL_COUNTER", counter1_str, "CELL_OUT_CNT_SECONDARY", counter2_str);
   }
   skip_first_print = 0;
   skip_second_print = 0;

   common_read_reg_long(unit, "FDR_CELL_IN_CNT_PRIMARY", counter_data, 1);
   counter_LSB = counter_data[0];
   counter_OVF = get_field(counter_LSB, 31, 31);
   counter_LSB = get_field(counter_LSB, 0, 30);

   if (hex_base == 1) {
      if (counter_OVF == 0) {
         sal_snprintf(counter2_str, sizeof(counter2_str), "0x%x", counter_LSB);
      } else {
         sal_snprintf(counter2_str, sizeof(counter2_str), "0x%x (ovf)", counter_LSB);
      }
   } else {
      COMPILER_64_SET(val_64, 0, counter_LSB);
      format_uint64_decimal(buf_val, val_64, commachr);
      if (counter_OVF == 0) {
         sal_snprintf(counter2_str, sizeof(counter2_str), "%s", buf_val);
      } else {
         sal_snprintf(counter2_str, sizeof(counter2_str), "%s (ovf)", buf_val);
      }
   }
   if (print_none_zero == 1) {
      if (counter_LSB == 0) {
         skip_first_print = 1;
      }
   }

   if (skip_first_print == 1) {
      cli_out("|                                                             |   %-30s   %-24s |\n", "", "");
   } else {
      cli_out("|                                                             |   %-30s = %-24s |\n", "CELL_IN_CNT_PRIMARY", counter2_str);
   }
   skip_first_print = 0;

   common_read_reg_long(unit, "FDR_CELL_IN_CNT_SECONDARY", counter_data, 1);
   counter_LSB = counter_data[0];
   counter_OVF = get_field(counter_LSB, 31, 31);
   counter_LSB = get_field(counter_LSB, 0, 30);

   if (hex_base == 1) {
      if (counter_OVF == 0) {
         sal_snprintf(counter2_str, sizeof(counter2_str), "0x%x", counter_LSB);
      } else {
         sal_snprintf(counter2_str, sizeof(counter2_str), "0x%x (ovf)", counter_LSB);
      }
   } else {
      COMPILER_64_SET(val_64, 0, counter_LSB);
      format_uint64_decimal(buf_val, val_64, commachr);
      if (counter_OVF == 0) {
         sal_snprintf(counter2_str, sizeof(counter2_str), "%s", buf_val);
      } else {
         sal_snprintf(counter2_str, sizeof(counter2_str), "%s (ovf)", buf_val);
      }
   }
   if (print_none_zero == 1) {
      if (counter_LSB == 0) {
         skip_first_print = 1;
      }
   }

   if (skip_first_print == 1) {
      cli_out("|                                                             |   %-30s   %-24s |\n", "", "");
   } else {
      cli_out("|                                                             |   %-30s = %-24s |\n", "CELL_IN_CNT_SECONDARY", counter2_str);
   }
   skip_first_print = 0;

   common_read_reg_long(unit, "FDT_TRANSMITTED_DATA_CELLS_COUNTER", counter_data, 1);
   counter_LSB = counter_data[0];
   counter_OVF = get_field(counter_LSB, 31, 31);
   counter_LSB = get_field(counter_LSB, 0, 30);

   if (hex_base == 1) {
      if (counter_OVF == 0) {
         sal_snprintf(counter1_str, sizeof(counter1_str), "0x%x", counter_LSB);
      } else {
         sal_snprintf(counter1_str, sizeof(counter1_str), "0x%x (ovf)", counter_LSB);
      }
   } else {
      COMPILER_64_SET(val_64, 0, counter_LSB);
      format_uint64_decimal(buf_val, val_64, commachr);
      if (counter_OVF == 0) {
         sal_snprintf(counter1_str, sizeof(counter1_str), "%s", buf_val);
      } else {
         sal_snprintf(counter1_str, sizeof(counter1_str), "%s (ovf)", buf_val);
      }
   }
   if (print_none_zero == 1) {
      if (counter_LSB == 0) {
         skip_first_print = 1;
      }
   }

   common_read_reg_long(unit, "FDR_CELL_IN_CNT_TOTAL", counter_data, 1);
   counter_LSB = counter_data[0];
   counter_OVF = get_field(counter_LSB, 31, 31);
   counter_LSB = get_field(counter_LSB, 0, 30);

   if (hex_base == 1) {
      if (counter_OVF == 0) {
         sal_snprintf(counter2_str, sizeof(counter2_str), "0x%x", counter_LSB);
      } else {
         sal_snprintf(counter2_str, sizeof(counter2_str), "0x%x (ovf)", counter_LSB);
      }
   } else {
      COMPILER_64_SET(val_64, 0, counter_LSB);
      format_uint64_decimal(buf_val, val_64, commachr);
      if (counter_OVF == 0) {
         sal_snprintf(counter2_str, sizeof(counter2_str), "%s", buf_val);
      } else {
         sal_snprintf(counter2_str, sizeof(counter2_str), "%s (ovf)", buf_val);
      }
   }

   if (print_none_zero == 1) {
      if (counter_LSB == 0) {
         skip_second_print = 1;
      }
   }

   if ((skip_first_print == 1) && (skip_second_print == 1)) {
      cli_out("|   %-30s   %-24s |   %-30s   %-24s |\n", "", "", "", "");
   } else if ((skip_first_print == 1) && (skip_second_print == 0)) {
      cli_out("|   %-30s   %-24s |   %-30s = %-24s |\n", "", "", "CELL_IN_CNT_TOTAL", counter2_str);
   } else if ((skip_first_print == 0) && (skip_second_print == 1)) {
      cli_out("|   %-30s = %-24s |   %-30s   %-24s |\n", "TRANSMITTED_DATA_CELLS_COUNTER", counter1_str, "", "");
   } else {
      cli_out("|   %-30s = %-24s |   %-30s = %-24s |\n", "TRANSMITTED_DATA_CELLS_COUNTER", counter1_str, "CELL_IN_CNT_TOTAL", counter2_str);
   }
   skip_first_print = 0;
   skip_second_print = 0;

   cli_out("+------------------------------+------------------------------+------------------------------+------------------------------+\n");
   cli_out("                               |                                                            /|\\\n");
   cli_out("                               |          A R A D   F A B R I C   I N T E R F A C E          |\n");
   cli_out("                              \\|/                                                            |\n");

   cli_out("\n\n");
   return CMD_OK;
}

#define CMD_DPP_DIAG_USAGE_SIZE 8000
char cmd_dpp_diag_usage[CMD_DPP_DIAG_USAGE_SIZE];

void cmd_dpp_diag_usage_update()
{
    memset(cmd_dpp_diag_usage, '\0', sizeof(char)*CMD_DPP_DIAG_USAGE_SIZE );
    sal_strncpy(cmd_dpp_diag_usage, "\n\tDIAGnostics commands\n\t", sal_strnlen(cmd_dpp_diag_usage, CMD_DPP_DIAG_USAGE_SIZE));
    sal_strncat(cmd_dpp_diag_usage, "Usages:\n\t", CMD_DPP_DIAG_USAGE_SIZE - (sal_strnlen(cmd_dpp_diag_usage, CMD_DPP_DIAG_USAGE_SIZE) + 1));
    sal_strncat(cmd_dpp_diag_usage, "DIAG [OPTION] <parameters> ...", CMD_DPP_DIAG_USAGE_SIZE - (sal_strnlen(cmd_dpp_diag_usage, CMD_DPP_DIAG_USAGE_SIZE) + 1));
    sal_strncat(cmd_dpp_diag_usage, "OPTION can be:\n\t", CMD_DPP_DIAG_USAGE_SIZE - (sal_strnlen(cmd_dpp_diag_usage, CMD_DPP_DIAG_USAGE_SIZE) + 1));
#ifdef BCM_PETRA_SUPPORT
    sal_strncat(cmd_dpp_diag_usage, "\npp             display packet processing diagnostics (use DIAG pp ? for further details) ", CMD_DPP_DIAG_USAGE_SIZE - (sal_strnlen(cmd_dpp_diag_usage, CMD_DPP_DIAG_USAGE_SIZE) + 1));
    sal_strncat(cmd_dpp_diag_usage, "\nfield          display field diagnostics (use DIAG field ? for further details) ", CMD_DPP_DIAG_USAGE_SIZE - (sal_strnlen(cmd_dpp_diag_usage, CMD_DPP_DIAG_USAGE_SIZE) + 1));
    sal_strncat(cmd_dpp_diag_usage, "\nrates          display rates diagnostics (use DIAG rates ? for further details) ", CMD_DPP_DIAG_USAGE_SIZE - (sal_strnlen(cmd_dpp_diag_usage, CMD_DPP_DIAG_USAGE_SIZE) + 1));
    sal_strncat(cmd_dpp_diag_usage, "\noam            display oam diagnostics (use DIAG oam ? for further details) ", CMD_DPP_DIAG_USAGE_SIZE - (sal_strnlen(cmd_dpp_diag_usage, CMD_DPP_DIAG_USAGE_SIZE) + 1));
    sal_strncat(cmd_dpp_diag_usage, "\nalloc          display allocation management diagnostics (use DIAG alloc for further details) ", CMD_DPP_DIAG_USAGE_SIZE - (sal_strnlen(cmd_dpp_diag_usage, CMD_DPP_DIAG_USAGE_SIZE) + 1));
    sal_strncat(cmd_dpp_diag_usage, "\ntemplate       display template manager diagnostics (use DIAG template for further details)", CMD_DPP_DIAG_USAGE_SIZE - (sal_strnlen(cmd_dpp_diag_usage, CMD_DPP_DIAG_USAGE_SIZE) + 1));
    sal_strncat(cmd_dpp_diag_usage, "\nlast_packet    display information on the last packet", CMD_DPP_DIAG_USAGE_SIZE - (sal_strnlen(cmd_dpp_diag_usage, CMD_DPP_DIAG_USAGE_SIZE) + 1));
    sal_strncat(cmd_dpp_diag_usage, "\nHeaderDiff     display global compensation statistics", CMD_DPP_DIAG_USAGE_SIZE - (sal_strnlen(cmd_dpp_diag_usage, CMD_DPP_DIAG_USAGE_SIZE) + 1));
#endif
    sal_strncat(cmd_dpp_diag_usage, "\nlag            display LAGs information (use DIAG lag ? for further details)", CMD_DPP_DIAG_USAGE_SIZE - (sal_strnlen(cmd_dpp_diag_usage, CMD_DPP_DIAG_USAGE_SIZE) + 1));
    sal_strncat(cmd_dpp_diag_usage, "\nnif            display all links status information ", CMD_DPP_DIAG_USAGE_SIZE - (sal_strnlen(cmd_dpp_diag_usage, CMD_DPP_DIAG_USAGE_SIZE) + 1));
    sal_strncat(cmd_dpp_diag_usage, "\ncounters       display all counters values", CMD_DPP_DIAG_USAGE_SIZE - (sal_strnlen(cmd_dpp_diag_usage, CMD_DPP_DIAG_USAGE_SIZE) + 1));
    sal_strncat(cmd_dpp_diag_usage, "\n\t\tnz - filter zero counters", CMD_DPP_DIAG_USAGE_SIZE - (sal_strnlen(cmd_dpp_diag_usage, CMD_DPP_DIAG_USAGE_SIZE) + 1));
    sal_strncat(cmd_dpp_diag_usage, "\n\t\tpacket_flow - show packet_flow", CMD_DPP_DIAG_USAGE_SIZE - (sal_strnlen(cmd_dpp_diag_usage, CMD_DPP_DIAG_USAGE_SIZE) + 1));
    sal_strncat(cmd_dpp_diag_usage, "\n\t\tgraphical - show a graphical representation", CMD_DPP_DIAG_USAGE_SIZE - (sal_strnlen(cmd_dpp_diag_usage, CMD_DPP_DIAG_USAGE_SIZE) + 1));
#ifdef BCM_JERICHO_SUPPORT
    sal_strncat(cmd_dpp_diag_usage, "\n\t\tcore_disp(cdsp) - show counters per core", CMD_DPP_DIAG_USAGE_SIZE - (sal_strnlen(cmd_dpp_diag_usage, CMD_DPP_DIAG_USAGE_SIZE) + 1));
#endif
    sal_strncat(cmd_dpp_diag_usage, "\n\t\tinterval=x - show the counters rate in an interval of x", CMD_DPP_DIAG_USAGE_SIZE - (sal_strnlen(cmd_dpp_diag_usage, CMD_DPP_DIAG_USAGE_SIZE) + 1));
    sal_strncat(cmd_dpp_diag_usage, "\n\t\t<blockName> - show counters in block <blockName> only (more then one blockName is supported)", CMD_DPP_DIAG_USAGE_SIZE - (sal_strnlen(cmd_dpp_diag_usage, CMD_DPP_DIAG_USAGE_SIZE) + 1));
    sal_strncat(cmd_dpp_diag_usage, "\ning_congestion display ingress congestion statistics", CMD_DPP_DIAG_USAGE_SIZE - (sal_strnlen(cmd_dpp_diag_usage, CMD_DPP_DIAG_USAGE_SIZE) + 1));
    sal_strncat(cmd_dpp_diag_usage, "\negr_congestion display egress congestion statistics", CMD_DPP_DIAG_USAGE_SIZE - (sal_strnlen(cmd_dpp_diag_usage, CMD_DPP_DIAG_USAGE_SIZE) + 1));
    sal_strncat(cmd_dpp_diag_usage, "\negr_calendars  display egress calendars info", CMD_DPP_DIAG_USAGE_SIZE - (sal_strnlen(cmd_dpp_diag_usage, CMD_DPP_DIAG_USAGE_SIZE) + 1));
    sal_strncat(cmd_dpp_diag_usage, "\n\t\tcurrent - print only current statistics", CMD_DPP_DIAG_USAGE_SIZE - (sal_strnlen(cmd_dpp_diag_usage, CMD_DPP_DIAG_USAGE_SIZE) + 1));
    sal_strncat(cmd_dpp_diag_usage, "\n\t\tmax - print only max statistics", CMD_DPP_DIAG_USAGE_SIZE - (sal_strnlen(cmd_dpp_diag_usage, CMD_DPP_DIAG_USAGE_SIZE) + 1));
    sal_strncat(cmd_dpp_diag_usage, "\n\t\tcontinuous - do not to disable updates to maximum values during their collection; will provide non correlated max values, but less time during which max values are not gathered", CMD_DPP_DIAG_USAGE_SIZE - (sal_strnlen(cmd_dpp_diag_usage, CMD_DPP_DIAG_USAGE_SIZE) + 1));
    sal_strncat(cmd_dpp_diag_usage, "\n\t\tall - print current and current statistics - default", CMD_DPP_DIAG_USAGE_SIZE - (sal_strnlen(cmd_dpp_diag_usage, CMD_DPP_DIAG_USAGE_SIZE) + 1));
    sal_strncat(cmd_dpp_diag_usage, "\n\t\tnz - don't print zero statistics", CMD_DPP_DIAG_USAGE_SIZE - (sal_strnlen(cmd_dpp_diag_usage, CMD_DPP_DIAG_USAGE_SIZE) + 1));
    sal_strncat(cmd_dpp_diag_usage, "\n\t\tglobal - print only global related statistics", CMD_DPP_DIAG_USAGE_SIZE - (sal_strnlen(cmd_dpp_diag_usage, CMD_DPP_DIAG_USAGE_SIZE) + 1));
    sal_strncat(cmd_dpp_diag_usage, "\n\t\tport=id - print only port id related statistics", CMD_DPP_DIAG_USAGE_SIZE - (sal_strnlen(cmd_dpp_diag_usage, CMD_DPP_DIAG_USAGE_SIZE) + 1));
    sal_strncat(cmd_dpp_diag_usage, "\n\t\tqueue=id - port queue related statistics", CMD_DPP_DIAG_USAGE_SIZE - (sal_strnlen(cmd_dpp_diag_usage, CMD_DPP_DIAG_USAGE_SIZE) + 1));
    sal_strncat(cmd_dpp_diag_usage, "\n\t\tinterface=id - interface queue related statistics", CMD_DPP_DIAG_USAGE_SIZE - (sal_strnlen(cmd_dpp_diag_usage, CMD_DPP_DIAG_USAGE_SIZE) + 1));
    sal_strncat(cmd_dpp_diag_usage, "\n\t\tingress [NetworkHeaderSize=<value>] - display packet length difference on ingerss path", CMD_DPP_DIAG_USAGE_SIZE - (sal_strnlen(cmd_dpp_diag_usage, CMD_DPP_DIAG_USAGE_SIZE) + 1));
    sal_strncat(cmd_dpp_diag_usage, "\n\t\tegress [NetworkHeaderSize=<value>] - display packet length difference on egerss path", CMD_DPP_DIAG_USAGE_SIZE - (sal_strnlen(cmd_dpp_diag_usage, CMD_DPP_DIAG_USAGE_SIZE) + 1));
    sal_strncat(cmd_dpp_diag_usage, "\nreassembly_context diaplay reassembly & port termination context", CMD_DPP_DIAG_USAGE_SIZE - (sal_strnlen(cmd_dpp_diag_usage, CMD_DPP_DIAG_USAGE_SIZE) + 1));
#ifdef BCM_JERICHO_SUPPORT
    sal_strncat(cmd_dpp_diag_usage, "\nresource_alloc  display VSQs resource allocation", CMD_DPP_DIAG_USAGE_SIZE - (sal_strnlen(cmd_dpp_diag_usage, CMD_DPP_DIAG_USAGE_SIZE) + 1));
    sal_strncat(cmd_dpp_diag_usage, "\n\t\ttype=<src/dst> - source/destination based", CMD_DPP_DIAG_USAGE_SIZE - (sal_strnlen(cmd_dpp_diag_usage, CMD_DPP_DIAG_USAGE_SIZE) + 1));
#endif
    sal_strncat(cmd_dpp_diag_usage, "\ncosq\t\tdisplay cosq statistics", CMD_DPP_DIAG_USAGE_SIZE - (sal_strnlen(cmd_dpp_diag_usage, CMD_DPP_DIAG_USAGE_SIZE) + 1));
    sal_strncat(cmd_dpp_diag_usage, "\n\t\tvoq - display all global information", CMD_DPP_DIAG_USAGE_SIZE - (sal_strnlen(cmd_dpp_diag_usage, CMD_DPP_DIAG_USAGE_SIZE) + 1));
    sal_strncat(cmd_dpp_diag_usage, "\n\t\t\tcongestion=<1/0> - display all congested VOQs", CMD_DPP_DIAG_USAGE_SIZE - (sal_strnlen(cmd_dpp_diag_usage, CMD_DPP_DIAG_USAGE_SIZE) + 1));
    sal_strncat(cmd_dpp_diag_usage, "\n\t\t\tid=<id> - print queue <id> related statistics only", CMD_DPP_DIAG_USAGE_SIZE - (sal_strnlen(cmd_dpp_diag_usage, CMD_DPP_DIAG_USAGE_SIZE) + 1));
    sal_strncat(cmd_dpp_diag_usage, "\n\t\t\tmost=<num_of_queues> - print <num_of_queues> most congested VOQs", CMD_DPP_DIAG_USAGE_SIZE - (sal_strnlen(cmd_dpp_diag_usage, CMD_DPP_DIAG_USAGE_SIZE) + 1));
    sal_strncat(cmd_dpp_diag_usage, "\n\t\t\tdetailed=<1/0> - print given VOQ's attributes", CMD_DPP_DIAG_USAGE_SIZE - (sal_strnlen(cmd_dpp_diag_usage, CMD_DPP_DIAG_USAGE_SIZE) + 1));
    sal_strncat(cmd_dpp_diag_usage, "\n\t\tprint_flow_and_up - print packet flow info, parametrs:", CMD_DPP_DIAG_USAGE_SIZE - (sal_strnlen(cmd_dpp_diag_usage, CMD_DPP_DIAG_USAGE_SIZE) + 1));
    sal_strncat(cmd_dpp_diag_usage, "\n\t\t\tis_voq_conn - 1 for voq_connector, 0 for system_port. dest_id - flow_id in case is_voq_conn is 1, system_port_id otherwise.", CMD_DPP_DIAG_USAGE_SIZE - (sal_strnlen(cmd_dpp_diag_usage, CMD_DPP_DIAG_USAGE_SIZE) + 1));
    sal_strncat(cmd_dpp_diag_usage, "\n\t\tprint_status - 1 for printing actual credit rate.", CMD_DPP_DIAG_USAGE_SIZE - (sal_strnlen(cmd_dpp_diag_usage, CMD_DPP_DIAG_USAGE_SIZE) + 1));
    sal_strncat(cmd_dpp_diag_usage, "\n\t\tnon_empty_queues - print non empty queues", CMD_DPP_DIAG_USAGE_SIZE - (sal_strnlen(cmd_dpp_diag_usage, CMD_DPP_DIAG_USAGE_SIZE) + 1));
    sal_strncat(cmd_dpp_diag_usage, "\n\t\tlocal_to_sys - translate local port to system port", CMD_DPP_DIAG_USAGE_SIZE - (sal_strnlen(cmd_dpp_diag_usage, CMD_DPP_DIAG_USAGE_SIZE) + 1));
    sal_strncat(cmd_dpp_diag_usage, "\n\t\tprint_mapping - print mapping mode", CMD_DPP_DIAG_USAGE_SIZE - (sal_strnlen(cmd_dpp_diag_usage, CMD_DPP_DIAG_USAGE_SIZE) + 1));
    sal_strncat(cmd_dpp_diag_usage, "\n\t\t\tby_sys=<1/0> - print all mapping by sysports order", CMD_DPP_DIAG_USAGE_SIZE - (sal_strnlen(cmd_dpp_diag_usage, CMD_DPP_DIAG_USAGE_SIZE) + 1));
    sal_strncat(cmd_dpp_diag_usage, "\n\t\t\tby_mod=<1/0> - print all mapping by modports order", CMD_DPP_DIAG_USAGE_SIZE - (sal_strnlen(cmd_dpp_diag_usage, CMD_DPP_DIAG_USAGE_SIZE) + 1));
    sal_strncat(cmd_dpp_diag_usage, "\n\t\t\tby_voq=<1/0> - print all mapping by VOQs order", CMD_DPP_DIAG_USAGE_SIZE - (sal_strnlen(cmd_dpp_diag_usage, CMD_DPP_DIAG_USAGE_SIZE) + 1));
    sal_strncat(cmd_dpp_diag_usage, "\n\t\tqpair - display qpair related diagnostics", CMD_DPP_DIAG_USAGE_SIZE - (sal_strnlen(cmd_dpp_diag_usage, CMD_DPP_DIAG_USAGE_SIZE) + 1));
    sal_strncat(cmd_dpp_diag_usage, "\n\t\t\tegq - display egq diagnostics", CMD_DPP_DIAG_USAGE_SIZE - (sal_strnlen(cmd_dpp_diag_usage, CMD_DPP_DIAG_USAGE_SIZE) + 1));
    sal_strncat(cmd_dpp_diag_usage, "\n\t\t\te2e - display e2e diagnostics", CMD_DPP_DIAG_USAGE_SIZE - (sal_strnlen(cmd_dpp_diag_usage, CMD_DPP_DIAG_USAGE_SIZE) + 1));
    sal_strncat(cmd_dpp_diag_usage, "\n\t\t\t\tps=<id> - display a graphical representation of the port scheduler in E2E associated with local port <id>", CMD_DPP_DIAG_USAGE_SIZE - (sal_strnlen(cmd_dpp_diag_usage, CMD_DPP_DIAG_USAGE_SIZE) + 1));
    sal_strncat(cmd_dpp_diag_usage, "\n\t\tegq_port_shaper - enable egress shaping", CMD_DPP_DIAG_USAGE_SIZE - (sal_strnlen(cmd_dpp_diag_usage, CMD_DPP_DIAG_USAGE_SIZE) + 1));
    sal_strncat(cmd_dpp_diag_usage, "\n\t\tautocredit - Configure the Scheduler AutoCredit", CMD_DPP_DIAG_USAGE_SIZE - (sal_strnlen(cmd_dpp_diag_usage, CMD_DPP_DIAG_USAGE_SIZE) + 1));
    sal_strncat(cmd_dpp_diag_usage, "\n\t\tfc - enable flow control", CMD_DPP_DIAG_USAGE_SIZE - (sal_strnlen(cmd_dpp_diag_usage, CMD_DPP_DIAG_USAGE_SIZE) + 1));
    sal_strncat(cmd_dpp_diag_usage, "\n\t\tscheduler_alloc_manager  - print allocated scheduling elements and voq connectors. parameters:", CMD_DPP_DIAG_USAGE_SIZE - (sal_strnlen(cmd_dpp_diag_usage, CMD_DPP_DIAG_USAGE_SIZE) + 1));
    sal_strncat(cmd_dpp_diag_usage, "\n\t\t1. allocated_resource - can be either connector, cl, fq, hr, connector_composite, cl_composite, fq_composite or hr_composite. example allocated_resource==cl etc...", CMD_DPP_DIAG_USAGE_SIZE - (sal_strnlen(cmd_dpp_diag_usage, CMD_DPP_DIAG_USAGE_SIZE) + 1));
    sal_strncat(cmd_dpp_diag_usage, "\n\t\t2. flow_id - requested flow id (flow_id=<flow_id>) - can be called together with flow_count(=<flow_count>). Prints the quad associated with the requested flow_id", CMD_DPP_DIAG_USAGE_SIZE - (sal_strnlen(cmd_dpp_diag_usage, CMD_DPP_DIAG_USAGE_SIZE) + 1));
#ifdef BCM_PETRA_SUPPORT
    sal_strncat(cmd_dpp_diag_usage, "\n\t\tcredit_watchdog - possibly set and print the credit watchdog global configuration, parametrs:", CMD_DPP_DIAG_USAGE_SIZE - (sal_strnlen(cmd_dpp_diag_usage, CMD_DPP_DIAG_USAGE_SIZE) + 1));
    sal_strncat(cmd_dpp_diag_usage, "\n\t\t\tenable=<0/1>, first_queue=<queue number>, last_queue=<queue_number>", CMD_DPP_DIAG_USAGE_SIZE - (sal_strnlen(cmd_dpp_diag_usage, CMD_DPP_DIAG_USAGE_SIZE) + 1));
    sal_strncat(cmd_dpp_diag_usage, "\nport_db        display software database information", CMD_DPP_DIAG_USAGE_SIZE - (sal_strnlen(cmd_dpp_diag_usage, CMD_DPP_DIAG_USAGE_SIZE) + 1));
    sal_strncat(cmd_dpp_diag_usage, "\nDBAL           display database abstraction layer diagnostics (use diag dbal for further details)", CMD_DPP_DIAG_USAGE_SIZE - (sal_strnlen(cmd_dpp_diag_usage, CMD_DPP_DIAG_USAGE_SIZE) + 1));
    sal_strncat(cmd_dpp_diag_usage, "\nretransmit     display retransmit information", CMD_DPP_DIAG_USAGE_SIZE - (sal_strnlen(cmd_dpp_diag_usage, CMD_DPP_DIAG_USAGE_SIZE) + 1));
    sal_strncat(cmd_dpp_diag_usage, "\nprge_info      display egress programmable editor information", CMD_DPP_DIAG_USAGE_SIZE - (sal_strnlen(cmd_dpp_diag_usage, CMD_DPP_DIAG_USAGE_SIZE) + 1));
    sal_strncat(cmd_dpp_diag_usage, "\nprge_last      display last program that was chosen by the egress programmable editor", CMD_DPP_DIAG_USAGE_SIZE - (sal_strnlen(cmd_dpp_diag_usage, CMD_DPP_DIAG_USAGE_SIZE) + 1));
    sal_strncat(cmd_dpp_diag_usage, "\n\t\tcore - Core ID (0/1)", CMD_DPP_DIAG_USAGE_SIZE - (sal_strnlen(cmd_dpp_diag_usage, CMD_DPP_DIAG_USAGE_SIZE) + 1));
    sal_strncat(cmd_dpp_diag_usage, "\nprge_mgmt      display egress editor management system diagnostics", CMD_DPP_DIAG_USAGE_SIZE - (sal_strnlen(cmd_dpp_diag_usage, CMD_DPP_DIAG_USAGE_SIZE) + 1));
    sal_strncat(cmd_dpp_diag_usage, "\n\t\tgraph=<program> - print dependency tree for the program", CMD_DPP_DIAG_USAGE_SIZE - (sal_strnlen(cmd_dpp_diag_usage, CMD_DPP_DIAG_USAGE_SIZE) + 1));
    sal_strncat(cmd_dpp_diag_usage, "\n\t\tdeploy=<program> - print deploy decision for the program", CMD_DPP_DIAG_USAGE_SIZE - (sal_strnlen(cmd_dpp_diag_usage, CMD_DPP_DIAG_USAGE_SIZE) + 1));
    sal_strncat(cmd_dpp_diag_usage, "\nfabric         display fabric related diagnostic", CMD_DPP_DIAG_USAGE_SIZE - (sal_strnlen(cmd_dpp_diag_usage, CMD_DPP_DIAG_USAGE_SIZE) + 1));
    sal_strncat(cmd_dpp_diag_usage, "\n\t\tgci - display gci and gci-backoff related diagnostic", CMD_DPP_DIAG_USAGE_SIZE - (sal_strnlen(cmd_dpp_diag_usage, CMD_DPP_DIAG_USAGE_SIZE) + 1));
#endif
    sal_strncat(cmd_dpp_diag_usage, "\ndump_signals [core=<core_id>] - display signals", CMD_DPP_DIAG_USAGE_SIZE - (sal_strnlen(cmd_dpp_diag_usage, CMD_DPP_DIAG_USAGE_SIZE) + 1));
    sal_strncat(cmd_dpp_diag_usage, "\n                  add \"parse [file_path]\" to the command in order to create a dump file of the packet signals, parsed to match the verilog format. Default file name:\"diag_parsed_signals.txt\" in current directory. if you want to dump to the shell, put \"stdout\" as file_path", CMD_DPP_DIAG_USAGE_SIZE - (sal_strnlen(cmd_dpp_diag_usage, CMD_DPP_DIAG_USAGE_SIZE) + 1));
    sal_strncat(cmd_dpp_diag_usage, "\nfc             flow control", CMD_DPP_DIAG_USAGE_SIZE - (sal_strnlen(cmd_dpp_diag_usage, CMD_DPP_DIAG_USAGE_SIZE) + 1));
    sal_strncat(cmd_dpp_diag_usage, "\n\t\t module     - NONE(default),NBI,CFC,EGQ,SCH,ALL", CMD_DPP_DIAG_USAGE_SIZE - (sal_strnlen(cmd_dpp_diag_usage, CMD_DPP_DIAG_USAGE_SIZE) + 1));
    sal_strncat(cmd_dpp_diag_usage, "\n\t\t interface  - NONE(default),XAUI,RXAUI,SPI,ILKN_INband,ILKN_OUTband,ALL", CMD_DPP_DIAG_USAGE_SIZE - (sal_strnlen(cmd_dpp_diag_usage, CMD_DPP_DIAG_USAGE_SIZE) + 1));
    sal_strncat(cmd_dpp_diag_usage, "\n\t\t type       - NONE(default),LLFC,PFC,NIF,ALL", CMD_DPP_DIAG_USAGE_SIZE - (sal_strnlen(cmd_dpp_diag_usage, CMD_DPP_DIAG_USAGE_SIZE) + 1));
#ifdef DUNE_UI
    sal_strncat(cmd_dpp_diag_usage, "\ndune_ui_free_bits - see which bits are free to add new dune ui functions", CMD_DPP_DIAG_USAGE_SIZE - (sal_strnlen(cmd_dpp_diag_usage, CMD_DPP_DIAG_USAGE_SIZE) + 1));
#endif
    sal_strncat(cmd_dpp_diag_usage, "\n", CMD_DPP_DIAG_USAGE_SIZE - (sal_strnlen(cmd_dpp_diag_usage, CMD_DPP_DIAG_USAGE_SIZE) + 1));

    return;
}


#ifdef BCM_PETRA_SUPPORT
STATIC
cmd_result_t
_cmd_dpp_serdes_str_get(int unit, pbmp_t phy_ports, char *serdes_str, uint32 qsgmii_ind, int is_over_fab, pbmp_t* phys_aligned)
{
    int port_i, is_range = 0;
    int last_added = -1;
    int ranges_num = 0;
#ifdef BCM_JERICHO_SUPPORT
    int first_phy = -1;
    uint32 phy_count;
#endif
    char tmp_str1[64], tmp_str2[64] = "";
    cmd_result_t rv = CMD_OK;

    PBMP_ITER(phy_ports, port_i){
        /*first time handle*/
        if(last_added == -1){
#ifdef BCM_JERICHO_SUPPORT
            first_phy = (is_over_fab) ? port_i : port_i - 1;
#endif
            last_added = port_i;
            ranges_num++;
            if (is_over_fab) {
                sal_sprintf(tmp_str1, "fabric:%02u", last_added - 1); 
            } else {
                sal_sprintf(tmp_str1, "%02u", last_added - 1); 
            }
            sal_strncat(tmp_str2, tmp_str1, sizeof(tmp_str2) -  sal_strlen(tmp_str2) - 1);
            continue;
        }
        /*continue of range*/
        if((last_added == port_i - 1)){
            last_added = port_i;
            is_range = 1;
            continue;
        }
        ranges_num++;
        if (is_range) {
            sal_sprintf(tmp_str1, "-%02u", last_added - 1);
            sal_strncat(tmp_str2, tmp_str1, sizeof(tmp_str2) -  sal_strlen(tmp_str2) - 1);
            is_range = 0;
        }
        last_added = port_i;
        sal_sprintf(tmp_str1, ", %02u", last_added - 1);
        sal_strncat(tmp_str2, tmp_str1, sizeof(tmp_str2) -  sal_strlen(tmp_str2) - 1);
    }
    /*close the last range*/
    if (is_range) {
        sal_sprintf(tmp_str1, "-%02u", last_added - 1); 
        sal_strncat(tmp_str2, tmp_str1, sizeof(tmp_str2) -  sal_strlen(tmp_str2) - 1);
    }

    switch(ranges_num){
    case 1:
        if (qsgmii_ind) {
            qsgmii_ind --;
            sal_snprintf(serdes_str, 30, "        %s.%d", tmp_str2, qsgmii_ind); 
        } else if (is_over_fab) {
            sal_snprintf(serdes_str, 30, " %s", tmp_str2); 
        } else {
            sal_snprintf(serdes_str, 30, "        %s", tmp_str2); 
        }
        break;
    case 2:
        sal_snprintf(serdes_str, 30, "     %s", tmp_str2);
        break;
    case 3:
        sal_snprintf(serdes_str, 30, " %s", tmp_str2); 
        break;
    default:
#ifdef BCM_JERICHO_SUPPORT
        SOC_PBMP_COUNT(*phys_aligned, phy_count);
        if (phy_count) {
            int start_lane = 0;
            char tmp_str3[64] = "";
            if (is_over_fab) {
                int nof_fabric_links = SOC_IS_QMX(unit) ? SOC_JER_NIF_NOF_ILKN_OVER_FABRIC_LINKS_QMX : SOC_JER_NIF_NOF_ILKN_OVER_FABRIC_LINKS_JERICHO;
                start_lane = SOC_DPP_DEFS_GET(unit, first_fabric_link_id);
                sal_sprintf(tmp_str1, "f%02u-%02u  ", start_lane, start_lane + nof_fabric_links - 1);
            } else {
                start_lane = (first_phy / JER_NIF_ILKN_MAX_NOF_LANES) * JER_NIF_ILKN_MAX_NOF_LANES;
                sal_sprintf(tmp_str1, "%02u-%02u  ", start_lane, start_lane + JER_NIF_ILKN_MAX_NOF_LANES - 1); 
            }
            sal_strncat(tmp_str3, tmp_str1, sizeof(tmp_str3) -  sal_strlen(tmp_str3) - 1);
            
            sal_sprintf(tmp_str1, "0x%x", SOC_PBMP_WORD_GET(*phys_aligned, 0));
            sal_strncat(tmp_str3, tmp_str1, sizeof(tmp_str3) -  sal_strlen(tmp_str3) - 1);
            sal_snprintf(serdes_str, 30, " %s", tmp_str3); 
        } else
#endif /*BCM_JERICHO_SUPPORT*/
        {
            rv = CMD_FAIL;
        }
    }

    return rv;
}
#endif

#ifdef BCM_PETRA_SUPPORT
void
print_dpp_pll_usage(int unit) {
   char cmd_dpp_lag_usage[] =
      "Usage (DIAG pll):"
      "\n\tDIAGnotsics PLL command\n\t"
      "Usages:\n\t"
      "DIAG pll - Displays the PLLs status and condiguration:\n"
      "\t\tReference clock - Hardcoded, base clock used to calculate the frequencies.\n"
      "\t\tP and N - Pre devider and Feedback devider.\n"
      "\t\tVCO - Voltage Control Oscilator.\n"
      "\t\tM0 to M5 - Dividers used to determine the frequency on the corresponding channel.\n"
      "\t\tCh0 to Ch5 - Output frequency channels.\n"
       "\t\tLocked - Status of the PLL. If the PLL is not locked it means it is disabled.\n"
      "\n";

   cli_out(cmd_dpp_lag_usage);
}

#ifdef COMPILER_HAS_DOUBLE
/* Function to get the information for the relevant PLL and print it. */
STATIC cmd_result_t
print_dpp_pll_val(int unit, char *pll, soc_dpp_pll_info_t *pll_info) {

    soc_dpp_pll_t *current_pll=NULL;
    char m1_div_str[4];
    char ch1_out_str[20]; 
    char m4_div_str[4];
    char ch4_out_str[20];
    char m5_div_str[4];
    char ch5_out_str[20];
    char locked_str[4];
    double  ref_clk;
    double  vco;
    double  ch0_out;
    double  ch1_out;
    double  ch4_out;
    double  ch5_out;
    int  p_div;
    int  n_div;
    int  m0_div;
    int  m1_div;
    int  m4_div;
    int  m5_div;
    int  locked;
    int strnlen_pll;

    /* Find out which PLL will be printed */
    /*
     * Make sure at least one of the strings is NULL terminated.
     */
    strnlen_pll = sal_strnlen(pll,SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH);
    if (!sal_strncasecmp(pll, "CORE", strnlen_pll)) {
        current_pll=&(pll_info->core_pll);
    } else if (!sal_strncasecmp(pll, "UC", strnlen_pll)) {
        current_pll=&(pll_info->uc_pll);
    } else if (!sal_strncasecmp(pll, "TS", strnlen_pll)) {
        current_pll=&(pll_info->ts_pll);
    } else if (!sal_strncasecmp(pll, "BS0", strnlen_pll)) {
        current_pll=&(pll_info->bs_pll[0]);
    } else if (!sal_strncasecmp(pll, "BS1", strnlen_pll)) {
        current_pll=&(pll_info->bs_pll[1]);
    } else if (!sal_strncasecmp(pll, "PMH", strnlen_pll)) {
        current_pll=&(pll_info->pmh_pll);
    } else if (!sal_strncasecmp(pll, "PML0", strnlen_pll)) {
        current_pll=&(pll_info->pml_pll[0]);
    } else if (!sal_strncasecmp(pll, "PML1", strnlen_pll)) {
        current_pll=&(pll_info->pml_pll[1]);
    } else if (!sal_strncasecmp(pll, "FAB0", strnlen_pll)) {
        current_pll=&(pll_info->fab_pll[0]);
    } else if (!sal_strncasecmp(pll, "FAB1", strnlen_pll)) {
        current_pll=&(pll_info->fab_pll[1]);
    } else if (!sal_strncasecmp(pll, "SRD0", strnlen_pll)) {
        current_pll=&(pll_info->srd_pll[0]);
    } else if (!sal_strncasecmp(pll, "SRD1", strnlen_pll)) {
        current_pll=&(pll_info->srd_pll[1]);
    } else if (!sal_strncasecmp(pll, "DDR0", strnlen_pll)) {
        current_pll=&(pll_info->ddr_pll[0]);
    } else if (!sal_strncasecmp(pll, "DDR1", strnlen_pll)) {
        current_pll=&(pll_info->ddr_pll[1]);
    } else if (!sal_strncasecmp(pll, "DDR2", strnlen_pll)) {
        current_pll=&(pll_info->ddr_pll[2]);
    } else if (!sal_strncasecmp(pll, "DDR3", strnlen_pll)) {
        current_pll=&(pll_info->ddr_pll[3]);
    } else {
        cli_out ("Did not find any relevant PLL!");
        return CMD_FAIL;
    }

    /* Ger the information for the relevant PLL from the PLL info structure */
    ref_clk=current_pll->ref_clk;
    p_div=current_pll->p_div;
    n_div=current_pll->n_div;
    m0_div=current_pll->m0_div;
    ch0_out=current_pll->ch0;
    m1_div=current_pll->m1_div;
    ch1_out=current_pll->ch1; 
    m4_div=current_pll->m4_div;
    ch4_out=current_pll->ch4;
    m5_div=current_pll->m5_div;
    ch5_out=current_pll->ch5;
    vco=current_pll->vco;
    locked=current_pll->locked;

    /* Convert some of the variables into strings.
     * This is done because if the M divider is 0, then the output on 
     * the corresponding channel does not exist. In this case it is more 
     * appropriate to print "-" than "0" for this channel
     */
    if (m1_div==0)  { 
        sal_sprintf(m1_div_str,"-  "); 
    } else { 
        sal_sprintf(m1_div_str,"%-3d",m1_div);
    }

    if (ch1_out==0) {
        sal_sprintf(ch1_out_str,"    -   ");
    } else {
        sal_sprintf(ch1_out_str,"%4.2f",ch1_out);
    }

    if (m4_div==0)  { 
        sal_sprintf(m4_div_str,"-  "); 
    } else { 
        sal_sprintf(m4_div_str,"%-3d",m4_div);
    }

    if (ch4_out==0) {
        sal_sprintf(ch4_out_str,"    -   ");
    } else {
        sal_sprintf(ch4_out_str,"%4.2f",ch4_out);
    }

    if (m5_div==0)  { 
        sal_sprintf(m5_div_str,"-  "); 
    } else { 
        sal_sprintf(m5_div_str,"%-3d",m5_div);
    }

    if (ch5_out==0) {
        sal_sprintf(ch5_out_str,"    -   ");
    } else {
        sal_sprintf(ch5_out_str,"%4.2f",ch5_out);
    }

    if (locked==1)  {
        sal_sprintf(locked_str,"+");
    } else {
        sal_sprintf(locked_str,"-");
    }

    cli_out ("| %-4s  |  %7.2f MHz |  %-2d |  %-4d |  %6.0f MHz |  %-3d |  %-8.2f MHz |  %-3s |  %-12s |  %s |  %-12s |  %s |  %-12s |    %s     |\n",pll, ref_clk,p_div,n_div,vco,m0_div,ch0_out,m1_div_str,ch1_out_str,m4_div_str,ch4_out_str,m5_div_str,ch5_out_str,locked_str);

    return CMD_OK;
}
#else
/* Same function a second time because of the COMPILER_HAS_DOUBLE.*/
STATIC cmd_result_t
print_dpp_pll_val(int unit, char *pll, soc_dpp_pll_info_t *pll_info) {

    soc_dpp_pll_t *current_pll=NULL; 
    char m1_div_str[4];
    char ch1_out_str[20]; 
    char m4_div_str[4];
    char ch4_out_str[20];
    char m5_div_str[4];
    char ch5_out_str[20];
    char locked_str[4];
    int  ref_clk;
    int  vco;
    int  ch0_out;
    int  ch1_out;
    int  ch4_out;
    int  ch5_out;
    int  p_div;
    int  n_div;
    int  m0_div;
    int  m1_div;
    int  m4_div;
    int  m5_div;
    int  locked;
    int strnlen_pll;

    /* Find out which PLL will be printed */
    /*
     * Make sure at least one of the strings is NULL terminated.
     */
    strnlen_pll = sal_strnlen(pll,SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH);
    if (!sal_strncasecmp(pll, "CORE", strnlen_pll)) {
        current_pll=&(pll_info->core_pll);
    } else if (!sal_strncasecmp(pll, "UC", strnlen_pll)) {
        current_pll=&(pll_info->uc_pll);
    } else if (!sal_strncasecmp(pll, "TS", strnlen_pll)) {
        current_pll=&(pll_info->ts_pll);
    } else if (!sal_strncasecmp(pll, "BS0", strnlen_pll)) {
        current_pll=&(pll_info->bs_pll[0]);
    } else if (!sal_strncasecmp(pll, "BS1", strnlen_pll)) {
        current_pll=&(pll_info->bs_pll[1]);
    } else if (!sal_strncasecmp(pll, "PMH", strnlen_pll)) {
        current_pll=&(pll_info->pmh_pll);
    } else if (!sal_strncasecmp(pll, "PML0", strnlen_pll)) {
        current_pll=&(pll_info->pml_pll[0]);
    } else if (!sal_strncasecmp(pll, "PML1", strnlen_pll)) {
        current_pll=&(pll_info->pml_pll[1]);
    } else if (!sal_strncasecmp(pll, "FAB0", strnlen_pll)) {
        current_pll=&(pll_info->fab_pll[0]);
    } else if (!sal_strncasecmp(pll, "FAB1", strnlen_pll)) {
        current_pll=&(pll_info->fab_pll[1]);
    } else if (!sal_strncasecmp(pll, "SRD0", strnlen_pll)) {
        current_pll=&(pll_info->srd_pll[0]);
    } else if (!sal_strncasecmp(pll, "SRD1", strnlen_pll)) {
        current_pll=&(pll_info->srd_pll[1]);
    } else if (!sal_strncasecmp(pll, "DDR0", strnlen_pll)) {
        current_pll=&(pll_info->ddr_pll[0]);
    } else if (!sal_strncasecmp(pll, "DDR1", strnlen_pll)) {
        current_pll=&(pll_info->ddr_pll[1]);
    } else if (!sal_strncasecmp(pll, "DDR2", strnlen_pll)) {
        current_pll=&(pll_info->ddr_pll[2]);
    } else if (!sal_strncasecmp(pll, "DDR3", strnlen_pll)) {
        current_pll=&(pll_info->ddr_pll[3]);
    }

    /* Ger the information for the relevant PLL from the PLL info structure */
    ref_clk=current_pll->ref_clk;
    p_div=current_pll->p_div;
    n_div=current_pll->n_div;
    m0_div=current_pll->m0_div;
    ch0_out=current_pll->ch0;
    m1_div=current_pll->m1_div;
    ch1_out=current_pll->ch1; 
    m4_div=current_pll->m4_div;
    ch4_out=current_pll->ch4;
    m5_div=current_pll->m5_div;
    ch5_out=current_pll->ch5;
    vco=current_pll->vco;
    locked=current_pll->locked;


    /* Convert some of the variables into strings.
     * This is done because if the M divider is 0, then the output on 
     * the corresponding channel does not exist. In this case it is more 
     * appropriate to print "-" than "0" for this channel
     */
    if (m1_div==0)  { 
        sal_sprintf(m1_div_str,"-  "); 
    } else { 
        sal_sprintf(m1_div_str,"%-3d",m1_div);
    }

    if (ch1_out==0) {
        sal_sprintf(ch1_out_str,"    -   ");
    } else {
        sal_sprintf(ch1_out_str,"%6d",ch1_out);
    }

    if (m4_div==0)  { 
        sal_sprintf(m4_div_str,"-  "); 
    } else { 
        sal_sprintf(m4_div_str,"%-3d",m4_div);
    }

    if (ch4_out==0) {
        sal_sprintf(ch4_out_str,"    -   ");
    } else {
        sal_sprintf(ch4_out_str,"%6d",ch4_out);
    }

    if (m5_div==0)  { 
        sal_sprintf(m5_div_str,"-  "); 
    } else { 
        sal_sprintf(m5_div_str,"%-3d",m5_div);
    }

    if (ch5_out==0) {
        sal_sprintf(ch5_out_str,"    -   ");
    } else {
        sal_sprintf(ch5_out_str,"%6d",ch5_out);
    }

    if (locked==1)  {
        sal_sprintf(locked_str,"+");
    } else {
        sal_sprintf(locked_str,"-");
    }

    cli_out ("| %-4s  |  %7d MHz |  %-2d |  %-4d |  %6d MHz |  %-3d |  %-8d MHz |  %-3s |  %-12s |  %s |  %-12s |  %s |  %-12s |    %s     |\n",pll, ref_clk,p_div,n_div,vco,m0_div,ch0_out,m1_div_str,ch1_out_str,m4_div_str,ch4_out_str,m5_div_str,ch5_out_str,locked_str);


    return CMD_OK;
}
#endif

STATIC cmd_result_t
cmd_dpp_pll(int unit, args_t *a) {

    char *option;
    char *pll;
    int rv;
    uint32 reg_val;
    soc_dpp_pll_info_t pll_info;

    /*Parse arguments*/
    option = ARG_GET(a);
    if (NULL != option) {
        /*
         * Make sure at least one of the strings is NULL terminated.
         */
        if (!sal_strcasecmp(option, "?")) {
            print_dpp_pll_usage(unit);
            return CMD_OK;
        } else {
        cli_out("Invalid option: %s \n", option);
        print_dpp_pll_usage(unit);
        return CMD_FAIL;
        }
    } else {
        cli_out("\nPLL status and configuration:\n");
        cli_out("-------------------------------\n");
        if (SOC_IS_QUX(unit)) {
            cli_out("--------------------------------------------\n");
            cli_out("This diagnostics is not supported in QUX.\n");
            cli_out("--------------------------------------------\n");
        } else {
            rv = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_pll_info_get, (unit, &pll_info));
            if (rv != BCM_E_NONE) {
                return CMD_FAIL;
            }

            cli_out("\n");
            cli_out ("|-------+--------------+-----+-------+-------------+------+---------------+------+---------------+------+---------------+------+---------------+----------|\n");
            cli_out ("|  PLL  |  Ref. Clock  |  P  |   N   |     VCO     |  M0  |    Ch0 Out    |  M1  |    Ch1 Out    |  M4  |    Ch4 Out    |  M5  |    Ch5 Out    |  Locked  |\n");
            cli_out ("|-------+--------------+-----+-------+-------------+------+---------------+------+---------------+------+---------------+------+---------------+----------|\n");

            /* Call the print function for each PLL*/
            pll="CORE";
            print_dpp_pll_val(unit, pll, &pll_info);

            pll="TS";
            print_dpp_pll_val(unit, pll, &pll_info);

            pll="UC";
            print_dpp_pll_val(unit, pll, &pll_info);

            if (SOC_IS_JERICHO(unit)) {

                pll="PMH";
                print_dpp_pll_val(unit, pll, &pll_info);

                pll="PML0";
                print_dpp_pll_val(unit, pll, &pll_info);

                pll="PML1";
                print_dpp_pll_val(unit, pll, &pll_info);

                pll="BS0";
                print_dpp_pll_val(unit, pll, &pll_info);

                if (SOC_IS_QAX(unit)) {

                    pll="BS1";
                    print_dpp_pll_val(unit, pll, &pll_info);

                } else {

                    pll="FAB0";
                    print_dpp_pll_val(unit, pll, &pll_info);

                    pll="FAB1";
                    print_dpp_pll_val(unit, pll, &pll_info);

                }

            } else if (SOC_IS_ARAD(unit)) {

                pll="SRD0";
                print_dpp_pll_val(unit, pll, &pll_info);

                pll="SRD1";
                print_dpp_pll_val(unit, pll, &pll_info);

                pll="DDR0";
                print_dpp_pll_val(unit, pll, &pll_info);

                pll="DDR1";
                print_dpp_pll_val(unit, pll, &pll_info);

                pll="DDR2";
                print_dpp_pll_val(unit, pll, &pll_info);

                pll="DDR3";
                print_dpp_pll_val(unit, pll, &pll_info);

            }

            cli_out ("|-------+--------------+-----+-------+-------------+------+---------------+------+---------------+------+---------------+------+---------------+----------|\n");
            cli_out("\n");

            if (SOC_IS_JERICHO_PLUS(unit)) {
                if (READ_ECI_CORE_FREQUENCY_VALUEr(unit, &reg_val) != BCM_E_NONE) {
                   return CMD_FAIL;
                }
                cli_out("--------------------------------------------------------------------\n");
                cli_out("Value of the core frequency clock from debug register: %d Hz\n", reg_val);
                cli_out("--------------------------------------------------------------------\n");
            }
        }
    }
return CMD_OK;
}

void
print_dpp_lag_usage(int unit) {
   char cmd_dpp_lag_usage[] =
      "Usage (DIAG lag):"
      "\n\tDIAGnotsics LAG command\n\t"
      "Usages:\n\t"
      "DIAG lag <LAG ID>"
      "If no LAG ID is specified the diag will go through all possible LAG IDs."
      "\n";

   cli_out(cmd_dpp_lag_usage);
}

STATIC cmd_result_t
cmd_dpp_lag(int unit, args_t *a) {

    bcm_error_t rv;
    char *option;
    char *enabled;
    char *psc;
    int member_count, current_member; 
    int tid, tid_min, tid_max, current_tid; 
    int flags, lb_type;
    int option_received_from_user=0;
    bcm_trunk_chip_info_t  trunks;
    bcm_trunk_info_t trunk_info;
    bcm_trunk_member_t  member_array[BCM_TRUNK_MAX_PORTCNT];
    bcm_gport_t gport;
    bcm_trunk_t         trunk_id;

    /*Get max Trunk ID according to chip confiuration*/
    rv=bcm_trunk_chip_info_get(unit, &trunks);
    if (rv != BCM_E_NONE) {
        return CMD_FAIL;
    }

    tid_min=0;
    tid_max=trunks.trunk_id_max;

    /*Parse arguments*/
    option = ARG_GET(a);
    option_received_from_user=(NULL != option);
    if (option_received_from_user) {
        /*
         * Make sure at least one of the strings is NULL terminated.
         */
        if (!sal_strcasecmp(option, "?")) {
            print_dpp_lag_usage(unit);
            return CMD_OK;
        } else {
            tid=sal_ctoi(option,0);
            tid_max=tid;
            tid_min=tid;
        }
    }

    /*Clear member_array*/
    sal_memset(member_array, 0x0, BCM_TRUNK_MAX_PORTCNT * sizeof(bcm_trunk_member_t));

    /*Iterate over Trink IDs*/
    for (current_tid=tid_min; current_tid <= tid_max; current_tid++) 
    {
        /*Get trunk info, member_array and member_count*/
        rv = TRUNK_ACCESS.t_info.trunk_id.get(unit, current_tid, &trunk_id);
        if (rv != BCM_E_NONE) {
            continue;
        }
        if (trunk_id == BCM_TRUNK_INVALID && !option_received_from_user) {
            continue;
        }
        rv = bcm_trunk_get(unit, current_tid, &trunk_info, BCM_TRUNK_MAX_PORTCNT, member_array, &member_count);
        if (rv == BCM_E_NOT_FOUND) {
                if (option_received_from_user) {
                    cli_out("Trunk with ID:%d does not exist!\n", current_tid);
                    break; 
                } else {
                    continue;
                }
        } else if (rv != BCM_E_NONE) {
            return CMD_FAIL;
        }

        /*Determine Port Selection Criteria*/
        lb_type=trunk_info.psc;
        switch (lb_type) {
            case 9:
                psc = "PORTFLOW";
                break;
            case 14:
                psc = "RoundRobin";
                break;
            case 15:
                psc = "DynamicResilient";
                break;
            case 17:
                psc = "SmoothDivision";
                break;
            default:
                psc = NULL;
                cli_out("Invalid PSC");
                return CMD_FAIL;
        }

        /*Print LAG information*/
        cli_out("LAG ID: %d \n", current_tid);
        cli_out("Member count: %d \n", member_count);
        cli_out("Port selection criteria: %s \n", psc);
        cli_out(" ------------------------------------ \n");
        cli_out(" Member ID |  Gport Port  |  Enabled  \n");
        cli_out(" ------------------------------------ \n");
        /*Iterate ovet members in LAG*/
        for (current_member = 0; current_member < member_count; current_member++){
            /*Get member specific information for each member from the member_array*/
            gport=member_array[current_member].gport;
            flags=member_array[current_member].flags;
            if(flags != 0x0)
            {
                enabled="N";
            } else {
                enabled="Y"; 
            }
            cli_out("%6d     |  0x%x   |%6s  \n", current_member, gport, enabled);
        }
    }

    return CMD_OK;
}

STATIC cmd_result_t
cmd_dpp_nif(int unit, args_t *a) {
    bcm_error_t rv;
    cmd_result_t result;
    bcm_pbmp_t pbmp_nif;
    bcm_port_t port;
    uint32 seq_done = 0;
    int is_locked = 0;
    const char *port_type = NULL;
    int enable;
    int serdes_freq_int, serdes_freq_remainder;
    int ch0_out_int = 0, ch0_out_remainder = 0;
    soc_reg_above_64_val_t reg_val;
    uint32 lane, is_initialized;
    uint32 type_of_bit_clk;
    int one_clk_time_measured_int, one_clk_time_measured_remainder;
    soc_port_t master; 
    int serdes_freq_arr[SOC_MAX_NUM_PORTS];
    int serdes_freq_remain_arr[SOC_MAX_NUM_PORTS];
    int i;    
    soc_port_if_t interface_type;
    bcm_port_if_t intf;
    char *interface_types_names[] = SOC_PORT_IF_NAMES_INITIALIZER;
    uint32 reg32_val[1];
    int port_i;
    uint32 nof_lanes, base_lane, ilkn_id;
    pbmp_t phy_ports, phys, phys_aligned;
    char *serdes_num_str, serdes_num_str_buf[30];
    char serdes_freq_str[10];
    char *option;
    int calc_rates = 1;
    uint32 qsgmii_ind = 0;
    int is_over_fab = 0;
    int is_na;
    uint32 ilkn_o_fab_rate_int = 0, ilkn_o_fabe_rate_reminder = 0;
    char seq_done_buffer[16];
#ifdef BCM_JERICHO_SUPPORT
    uint32 physical_phy;
    int phy, is_legacy_phy, nof_lanes_per_nbi = SOC_DPP_DEFS_GET(unit, nof_lanes_per_nbi);
    SOC_JER_NIF_PLL_TYPE pll_type;
    uint32 lane_reg, new_base_lane;
#endif /*BCM_JERICHO_SUPPORT*/
#ifdef BCM_QUX_SUPPORT
    SOC_QUX_NIF_PLL_TYPE qux_pll_type;
#endif
    option = ARG_GET(a);

    while (NULL != option) {
        /*
         * Make sure at least one of the strings is NULL terminated.
         */
        if (!sal_strncasecmp(option, "norates", sal_strnlen(option, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH))) {
            calc_rates = 0;
        }
        option = ARG_GET(a);
    }

    for (i = 0; i < SOC_MAX_NUM_PORTS; ++i) {
        serdes_freq_arr[i] = -1;
        serdes_freq_remain_arr[i] = -1;
    }

    cli_out("Nif status:\n");
    cli_out("--------------------\n");
    cli_out(" Port # |    Port Type    |       SerDes #      |   Rx Seq Done  | Signal Lock | SerDes Rate | Input RefClk \n");
    cli_out(" ----------------------------------------------------------------------------------------------------------  \n");

    BCM_PBMP_CLEAR(pbmp_nif);
    BCM_PBMP_ASSIGN(pbmp_nif, PBMP_PORT_ALL(unit));
    BCM_PBMP_REMOVE(pbmp_nif, PBMP_SFI_ALL(unit));
    BCM_PBMP_REMOVE(pbmp_nif, PBMP_RCY_ALL(unit));

    BCM_PBMP_CLEAR(phys_aligned);

    BCM_PBMP_ITER(pbmp_nif, port) {

        is_over_fab = 0;

        rv = soc_port_sw_db_is_initialized_get(unit, port, &is_initialized);
        if (rv != BCM_E_NONE) {
              return CMD_FAIL;
        }

        if (!is_initialized) {
            continue;
        }
        rv = bcm_port_enable_get(unit, port, &enable);
        if (rv != BCM_E_NONE) {
           return CMD_FAIL;
        }
#ifdef BCM_QUX_SUPPORT
        if (SOC_IS_QUX(unit)) {
            rv = soc_qux_port_pll_type_get(unit, port, &qux_pll_type);
            if (rv != BCM_E_NONE) {
               return CMD_FAIL;
            }
        }
#endif
        /*get speed measured- only if port is master channel (or un-channelized) */
        /* don't measure speed if synce is enabled (because it mess with its registers) or when running simulation (meaningless) */
        if (calc_rates && enable && !(SOC_DPP_CONFIG(unit)->arad->init.synce.enable) && !SAL_BOOT_PLISIM){
            rv = soc_port_sw_db_master_channel_get(unit, port, &master);
            if (rv != BCM_E_NONE) {
                  return CMD_FAIL;
            }
            if (serdes_freq_arr[master] == -1) {
#ifdef BCM_JERICHO_SUPPORT
                if (SOC_IS_JERICHO(unit)) {
                    rv = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_phy_nif_measure, (unit, port, &type_of_bit_clk, &one_clk_time_measured_int, &one_clk_time_measured_remainder, &serdes_freq_int, &serdes_freq_remainder, &lane));
                } else
#endif /*BCM_JERICHO_SUPPORT*/
                {
                    rv = _phy_arad_nif_measure(unit, port, &type_of_bit_clk, &one_clk_time_measured_int, &one_clk_time_measured_remainder, &serdes_freq_int, &serdes_freq_remainder, &lane);
                }
                if (rv != BCM_E_NONE) {
                    return CMD_FAIL;
                }
                if (one_clk_time_measured_int != -1
#ifdef BCM_QUX_SUPPORT
                    || qux_pll_type == SOC_QUX_NIF_PLL_TYPE_PMX
#endif
                    ) {
                    serdes_freq_arr[master] = serdes_freq_int; 
                    serdes_freq_remain_arr[master] = serdes_freq_remainder;
                }
            /*master channel already measured */
            } else { 
                serdes_freq_int = serdes_freq_arr[master];
                serdes_freq_remainder = serdes_freq_remain_arr[master];
            }
            rv = soc_port_sw_db_interface_type_get(unit, port, &interface_type);
            if (rv != BCM_E_NONE) {
                return CMD_FAIL;
            }
            if (one_clk_time_measured_int == -1)
            {
                is_na = 1;
                if (SOC_PORT_IF_ILKN == interface_type) {
                    rv = soc_port_sw_db_protocol_offset_get(unit, port, 0, &ilkn_id);
                    if (rv != BCM_E_NONE) {
                        return CMD_FAIL;
                    }
                    if (SOC_DPP_IS_ILKN_PORT_OVER_FABRIC(unit, ilkn_id)) {
                        if (serdes_freq_arr[master] == -1) {
                            rv = diag_dcmn_phy_measure_port(unit, port, 1, &ilkn_o_fab_rate_int, &ilkn_o_fabe_rate_reminder);
                            serdes_freq_arr[master] = serdes_freq_int = ilkn_o_fab_rate_int;
                            serdes_freq_remain_arr[master] = serdes_freq_remainder = ilkn_o_fabe_rate_reminder;
                            is_na = 0;
                            if (rv != BCM_E_NONE) {
                                return CMD_FAIL;
                            }
                        } else {
                            serdes_freq_int = serdes_freq_arr[master];
                            serdes_freq_remainder = serdes_freq_remain_arr[master];
                            is_na = 0;
                        }
                        sal_sprintf(serdes_freq_str, "%d.%03d", serdes_freq_int, serdes_freq_remainder);
                    }
                }
#ifdef BCM_QUX_SUPPORT
                else if (qux_pll_type == SOC_QUX_NIF_PLL_TYPE_PMX) {
                    sal_sprintf(serdes_freq_str, "%d.%03d", serdes_freq_int, serdes_freq_remainder);
                    is_na = 0;
                }
#endif
                if (is_na) {
                    sal_sprintf(serdes_freq_str, "N/A"); 
                }
            } else {
                sal_sprintf(serdes_freq_str, "%d.%03d", serdes_freq_int, serdes_freq_remainder);
            }
        } else {
            sal_sprintf(serdes_freq_str, "N/A");
        }

        rv = soc_port_sw_db_interface_type_get(unit, port, &interface_type);
        if (rv != BCM_E_NONE) {
            return CMD_FAIL;
        }

        rv = bcm_port_interface_get(unit, port, &intf);
        if (rv != BCM_E_NONE) {
            return CMD_FAIL;
        }
        port_type = interface_types_names[intf];

        /*get serdes #*/
        rv =  soc_port_sw_db_first_phy_port_get(unit, port , &base_lane);
        if (rv != BCM_E_NONE) {
            return CMD_FAIL;
        }
        base_lane--;
        rv =  soc_port_sw_db_phy_ports_get(unit, port , &phy_ports);
        if (rv != BCM_E_NONE) {
            return CMD_FAIL;
        }
        phys = phy_ports;
        qsgmii_ind = 0;
#ifdef BCM_JERICHO_SUPPORT
        physical_phy = base_lane;
        if (SOC_IS_JERICHO(unit)) {
            if (SOC_PORT_IF_ILKN == interface_type) {
                rv = soc_port_sw_db_protocol_offset_get(unit, port, 0, &ilkn_id);
            
                if (rv != BCM_E_NONE) {
                    return CMD_FAIL;
                }
                is_over_fab = SOC_DPP_IS_ILKN_PORT_OVER_FABRIC(unit, ilkn_id);
                rv = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_port_nif_ilkn_phys_aligned_pbmp_get, (unit, port, &phys_aligned, 0));
                if (rv != BCM_E_NONE) {
                    return CMD_FAIL;
                }
            }


            BCM_PBMP_CLEAR(phys);
            PBMP_ITER(phy_ports, phy) {
                physical_phy = phy;
                rv = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_qsgmii_offsets_remove,
                (unit, phy , &physical_phy));
                if (rv != BCM_E_NONE) {
                    return CMD_FAIL;
                } 
                if (is_over_fab) {
                    BCM_PBMP_PORT_ADD(phys, physical_phy + 1 - SOC_DPP_FIRST_FABRIC_PHY_PORT(unit));
                } else {
                    BCM_PBMP_PORT_ADD(phys, physical_phy);
                }
            
            }

            
            if(SOC_PORT_IF_QSGMII == interface_type) {
                rv = soc_port_sw_db_protocol_offset_get(unit, port, 0, &qsgmii_ind);
                if (rv != BCM_E_NONE) {
                    return CMD_FAIL;
                }
                qsgmii_ind = (qsgmii_ind % 4) + 1;
            }
        }
#endif

        rv = soc_port_sw_db_num_lanes_get(unit, port, &nof_lanes);
        if (rv != BCM_E_NONE) {
            return CMD_FAIL;
        }

        if (nof_lanes == 0) {
        /*mis configuration*/
            continue;
        }

        result = _cmd_dpp_serdes_str_get(unit, phys, serdes_num_str_buf, qsgmii_ind, is_over_fab, &phys_aligned);
        if (result != CMD_OK) {
            return result;
        }
        serdes_num_str = serdes_num_str_buf;

        /*get seq_done*/
        seq_done = 0;
#ifdef BCM_JERICHO_SUPPORT
        if (SOC_IS_JERICHO(unit)) {
            phymod_phy_access_t phy[MAX_NOF_PMS_IN_ILKN];
            portmod_access_get_params_t params;
            uint32 lane_seq_done = 0;
            int nof_pm, pm_index;

            for (pm_index = 0; pm_index < MAX_NOF_PMS_IN_ILKN; ++pm_index) {
                rv = phymod_phy_access_t_init(&phy[pm_index]);
                if (rv != BCM_E_NONE) {
                    return CMD_FAIL;
                }
            }
            rv = portmod_access_get_params_t_init(unit, &params);
            if (rv != BCM_E_NONE) {
                return CMD_FAIL;
            }

            for (i = 0; i < nof_lanes; ++i) {
                params.lane = i;

                /* set is_c_port as 0, tri_core_mode is don't care. */
                rv = portmod_port_phy_lane_access_get(unit, port, &params, MAX_NOF_PMS_IN_ILKN, phy, &nof_pm, NULL);
                if (rv != BCM_E_NONE) {
                    return CMD_FAIL;
                }

                rv = portmod_port_is_legacy_ext_phy_present(unit, port, &is_legacy_phy);
                if (rv != BCM_E_NONE) {
                    return CMD_FAIL;
                }

                if (is_legacy_phy) {
                    rv = soc_phyctrl_control_get(unit, port, SOC_PHY_CONTROL_RX_SEQ_DONE, &lane_seq_done);
                    if (rv != BCM_E_NONE) {
                        seq_done = -1;
                    }
                    else {
                        if (lane_seq_done) {
                            if (nof_lanes == 1) {
                                seq_done = lane_seq_done;
                            } else {
                                seq_done |= 1 << params.lane;
                            }
                        }
                    }
                }
                else if (phy[nof_pm-1].access.lane_mask){
                    rv = phymod_phy_rx_pmd_locked_get(&phy[nof_pm-1], &lane_seq_done);
                    if (rv != BCM_E_NONE) {
                        return CMD_FAIL;
                    }

                    if (lane_seq_done) {
                        if (nof_lanes == 1) {
                            seq_done = lane_seq_done;
                        } else {
                            seq_done |= 1 << params.lane;
                        }
                    }
                }
            }

        } else
#endif /*BCM_JERICHO_SUPPORT*/
        {
        rv = READ_NBI_NIF_WC_RX_SEQ_DONEr(unit, reg32_val);
        if (SOC_FAILURE(rv)) {
            cli_out("Failed to read NBI_NIF_WC_RX_SEQ_DONE register\n");
            return CMD_FAIL;
        }
            if (interface_type == SOC_PORT_IF_CAUI) {
            /*for case of swap*/
            SHR_BITCOPY_RANGE(&seq_done, 0, reg32_val, base_lane, 12);
        }
        else{
            i = 0;
            PBMP_ITER(phy_ports, port_i){
                SHR_BITCOPY_RANGE(&seq_done, i , reg32_val, port_i-1, 1);
                i++;
            }
        }
        }

        /*get is_locked*/
#ifdef BCM_QUX_SUPPORT
        if (SOC_IS_QUX(unit)) {
            rv = soc_reg_above_64_get(unit, NIF_PORTS_INDICATIONSr, REG_PORT_ANY, 0, reg_val);
            if (rv != BCM_E_NONE) {
                return CMD_FAIL;
            }
            /* coverity[overrun-local] */
            is_locked = SHR_BITGET(reg_val, base_lane);
        } else
#endif
#ifdef BCM_JERICHO_SUPPORT
        if (SOC_IS_JERICHO(unit)) {
            rv = soc_jer_port_pll_type_get(unit, port, &pll_type);
            if (rv != BCM_E_NONE) {
                return CMD_FAIL;
            }

            if (SOC_PORT_IF_ILKN == interface_type) {
                uint32 offset;

                rv = soc_port_sw_db_protocol_offset_get(unit, port, 0, &offset);
                if (rv != BCM_E_NONE) {
                    return CMD_FAIL;
                }

                if ((offset == 0) || (offset == 1)) { /* ILKN0 & ILKN1 */
                    rv = READ_ILKN_PMH_RX_ILKN_STATUSr(unit, REG_PORT_ANY, offset & 1, reg32_val);
                    if (rv != BCM_E_NONE) {
                        return CMD_FAIL;
                    }
                    is_locked = soc_reg_field_get(unit, ILKN_PMH_RX_ILKN_STATUSr, *reg32_val, RX_N_STAT_ALIGNED_RAWf);
                } else {
                    rv = READ_ILKN_PML_RX_ILKN_STATUSr(unit, (offset / 4), offset & 1, reg32_val);
                    if (rv != BCM_E_NONE) {
                        return CMD_FAIL;
                    }
                    is_locked = soc_reg_field_get(unit, ILKN_PML_RX_ILKN_STATUSr, *reg32_val, RX_N_STAT_ALIGNED_RAWf);
                }
            } else if (SOC_PORT_IF_CAUI == interface_type) {
                if (SOC_IS_JERICHO_PLUS_ONLY(unit) && pll_type == SOC_JER_NIF_PLL_TYPE_PML1) {
                    rv = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_qsgmii_offsets_remove, (unit, base_lane, &new_base_lane));
                    if (rv != BCM_E_NONE) {
                        return CMD_FAIL;
                    }
                    base_lane = new_base_lane % nof_lanes_per_nbi;
                    rv = READ_NBIL_PORTS_INDICATIONSr(unit, 1, reg_val);
                    if (rv != BCM_E_NONE) {
                        return CMD_FAIL;
                    }
                    is_locked = SHR_BITGET(reg_val, base_lane);
                } else {
                    rv = READ_NBIH_PORTS_INDICATIONSr(unit, reg32_val); 
                    if (rv != BCM_E_NONE) {
                        return CMD_FAIL;
                    }
                    is_locked = SHR_BITGET(reg32_val, base_lane);
                }
            } else if(SOC_PORT_IF_QSGMII == interface_type) {
                rv = soc_reg32_get(unit, MAC_MODEr, port, 0, reg32_val);
                if (rv != BCM_E_NONE) {
                    return CMD_FAIL;
                }
                is_locked = soc_reg_field_get(unit, MAC_MODEr, *reg32_val, LINK_STATUSf);
            } else {
                if (pll_type == SOC_JER_NIF_PLL_TYPE_PMH) {
                    lane_reg = base_lane % 4; /* CLPORT */
                } else {
                    rv = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_qsgmii_offsets_remove, (unit, base_lane, &new_base_lane));
                    if (rv != BCM_E_NONE) {
                        return CMD_FAIL;
                    }
                    lane_reg = new_base_lane % 4 + 4; /* XLPORT */
                }
                rv = soc_reg32_get(unit, jer_register_per_lane_num[lane_reg], port, 0, reg32_val);
                if (rv != BCM_E_NONE) {
                    return CMD_FAIL;
                }
                is_locked = soc_reg_field_get(unit, jer_register_per_lane_num[lane_reg], *reg32_val, LINK_STATUSf);
            }
        } else 
#endif /*BCM_JERICHO_SUPPORT*/
        {
            if (SOC_PORT_IF_ILKN == interface_type) {
                rv = soc_port_sw_db_protocol_offset_get(unit, port, 0, &ilkn_id);
                if (rv != BCM_E_NONE) {
                    return CMD_FAIL;
                }

                rv = READ_NBI_RX_ILKN_STATUSr(unit, ilkn_id, reg32_val);
                if (rv != BCM_E_NONE) {
                    return CMD_FAIL;
                }

                is_locked = soc_reg_field_get(unit, NBI_RX_ILKN_STATUSr, *reg32_val, RX_N_STAT_ALIGNED_RAWf);
            } else if (SOC_PORT_IF_CAUI == interface_type) {
                rv = READ_NBI_PORTS_INDICATIONSr(unit, reg32_val);
                if (rv != BCM_E_NONE) {
                    return CMD_FAIL;
                }
                if (base_lane < 12) {
                /*CLP0*/
                is_locked = SHR_BITGET(reg32_val, base_lane);
                } else {
                /*CLP1*/
                is_locked = SHR_BITGET(reg32_val, base_lane - 4 /*xlp0 size*/);
                }
            } else {
                rv = soc_reg32_get(unit, register_per_lane_num[base_lane], port, 0, reg32_val);
                if (rv != BCM_E_NONE) {
                    return CMD_FAIL;
                }
                is_locked = soc_reg_field_get(unit, register_per_lane_num[base_lane], *reg32_val, LINKf);
            }
        }

#ifdef BCM_JERICHO_SUPPORT
        if (SOC_IS_JERICHO(unit)) {
            result = jer_phy_measure_nif_pll(unit, port, &ch0_out_int, &ch0_out_remainder);
            if(result != CMD_OK) { return CMD_FAIL; }
        } else
#endif /*BCM_JERICHO_SUPPORT*/
        {
            /*SRD1 PLL - ch0 out print*/
            if (READ_ECI_SRD_1_PLL_CONFIGr(unit, reg_val) != BCM_E_NONE) {
               return CMD_FAIL;
            }
            phy_measure_ch0_calc(reg_val, &ch0_out_int, &ch0_out_remainder);
        }

        dpp_diag_seq_done_buffer_get(seq_done, seq_done_buffer);

        cli_out("   %3d  | %-15s |%-21s|    %s    |     %3s     |   %6s    |   %3d.%-3d  \n",
                port,
                port_type,
                serdes_num_str,
                seq_done_buffer,
                (is_locked ? " + " : " - "),
                serdes_freq_str,
                ch0_out_int, ch0_out_remainder);
    
    }

    return CMD_OK;

}

STATIC cmd_result_t
cmd_dpp_port_db(int unit, args_t *a) {
   int rv;

   cli_out("Port Software Database:\n");
   cli_out("----------------------\n");

   rv =  soc_port_sw_db_print(unit, 0);
   if (rv != SOC_E_NONE) {
      return CMD_FAIL;
   }

   return CMD_OK;
}

STATIC cmd_result_t
cmd_dpp_arad_retransmit(int unit, args_t *a) {
    int rv = CMD_OK;
    uint32 reg0_val32 = 0, reg1_val32 = 0;
    uint64 reg_val64 = COMPILER_64_INIT(0, 0);

    cli_out("\t\t\t\tILKN0:\t\tILKN1:\n");

    rv = READ_NBI_RX_ILKN_0_SENT_RETRANS_REQ_CNTr(unit, &reg0_val32);
    if (SOC_FAILURE(rv)) {
        cli_out("Failed to read NBI_RX_ILKN_0_SENT_RETRANS_REQ_CNT register\n");
        return CMD_FAIL;
    }
    /* check counter overflow */
    if (((reg0_val32 >> 31) & 0x1) == 0x1) {
        cli_out("NBI_RX_ILKN_0_SENT_RETRANS_REQ_CNT register overflow\n");
    }
    rv = READ_NBI_RX_ILKN_1_SENT_RETRANS_REQ_CNTr(unit, &reg1_val32);
    if (SOC_FAILURE(rv)) {
        cli_out("Failed to read NBI_RX_ILKN_1_SENT_RETRANS_REQ_CNT register\n");
        return CMD_FAIL;
    }
    /* check counter overflow */
    if (((reg1_val32 >> 31) & 0x1) == 0x1) {
        cli_out("NBI_RX_ILKN_1_SENT_RETRANS_REQ_CNT register overflow\n");
    }
    cli_out("SENT REQ CNT \t\t\t%d\t\t%d\n", reg0_val32, reg1_val32);

    rv = READ_NBI_TX_ILKN_0_RECEIVED_RETRANS_REQ_CNTr(unit, &reg0_val32);
    if (SOC_FAILURE(rv)) {
        cli_out("Failed to read NBI_TX_ILKN_0_RECEIVED_RETRANS_REQ_CNT register\n");
        return CMD_FAIL;
    }
    /* check counter overflow */
    if (((reg0_val32 >> 31) & 0x1) == 0x1) {
        cli_out("NBI_TX_ILKN_0_RECEIVED_RETRANS_REQ_CNT register overflow\n");
    }
    rv = READ_NBI_TX_ILKN_1_RECEIVED_RETRANS_REQ_CNTr(unit, &reg1_val32);
    if (SOC_FAILURE(rv)) {
        cli_out("Failed to read NBI_TX_ILKN_1_RECEIVED_RETRANS_REQ_CNT register\n");
        return CMD_FAIL;
    }
    /* check counter overflow */
    if (((reg1_val32 >> 31) & 0x1) == 0x1) {
       cli_out("NBI_TX_ILKN_1_RECEIVED_RETRANS_REQ_CNT register overflow\n");
    }
    cli_out("RECEIVED REQ CNT \t\t%d\t\t%d\n", reg0_val32, reg1_val32);

    rv = READ_NBI_ILKN_RX_0_RETRANSMIT_DEBUG_INFORMATIONr(unit, &reg0_val32);
    if (SOC_FAILURE(rv)) {
        cli_out("Failed to read NBI_ILKN_RX_0_RETRANSMIT_DEBUG_INFORMATION register\n");
        return CMD_FAIL;
    }

    rv = READ_NBI_ILKN_RX_1_RETRANSMIT_DEBUG_INFORMATIONr(unit, &reg1_val32);
    if (SOC_FAILURE(rv)) {
        cli_out("Failed to read NBI_ILKN_RX_0_RETRANSMIT_DEBUG_INFORMATION register\n");
        return CMD_FAIL;
    }

    cli_out("WRAP BEFORE DISC ERR \t\t%d\t\t%d\n",
            soc_reg_field_get(unit, NBI_ILKN_RX_0_RETRANSMIT_DEBUG_INFORMATIONr,
            reg0_val32, ILKN_RX_0_RETRANS_WRAP_B_4_DISC_ERRf),
            soc_reg_field_get(unit, NBI_ILKN_RX_1_RETRANSMIT_DEBUG_INFORMATIONr,
            reg1_val32, ILKN_RX_1_RETRANS_WRAP_B_4_DISC_ERRf));
    cli_out("WRAP AFTER DISC ERR \t\t%d\t\t%d\n",
            soc_reg_field_get(unit, NBI_ILKN_RX_0_RETRANSMIT_DEBUG_INFORMATIONr,
            reg0_val32, ILKN_RX_0_RETRANS_WRAP_AFTER_DISC_ERRf),
            soc_reg_field_get(unit, NBI_ILKN_RX_1_RETRANSMIT_DEBUG_INFORMATIONr,
            reg1_val32, ILKN_RX_1_RETRANS_WRAP_AFTER_DISC_ERRf));
    cli_out("WDOG ERR \t\t\t%d\t\t%d\n",
            soc_reg_field_get(unit, NBI_ILKN_RX_0_RETRANSMIT_DEBUG_INFORMATIONr,
            reg0_val32, ILKN_RX_0_RETRANS_WDOG_ERRf),
            soc_reg_field_get(unit, NBI_ILKN_RX_1_RETRANSMIT_DEBUG_INFORMATIONr,
            reg1_val32, ILKN_RX_1_RETRANS_WDOG_ERRf));
    cli_out("RETRY ERR \t\t\t%d\t\t%d\n",
            soc_reg_field_get(unit, NBI_ILKN_RX_0_RETRANSMIT_DEBUG_INFORMATIONr,
            reg0_val32, ILKN_RX_0_RETRANS_RETRY_ERRf),
            soc_reg_field_get(unit, NBI_ILKN_RX_1_RETRANSMIT_DEBUG_INFORMATIONr,
            reg1_val32, ILKN_RX_1_RETRANS_RETRY_ERRf));
    cli_out("REQ SENT \t\t\t%d\t\t%d\n",
            soc_reg_field_get(unit, NBI_ILKN_RX_0_RETRANSMIT_DEBUG_INFORMATIONr,
            reg0_val32, ILKN_RX_0_RETRANS_REQ_SENTf),
            soc_reg_field_get(unit, NBI_ILKN_RX_1_RETRANSMIT_DEBUG_INFORMATIONr,
            reg1_val32, ILKN_RX_1_RETRANS_REQ_SENTf));
    cli_out("REACHED TIMOUT \t\t\t%d\t\t%d\n",
            soc_reg_field_get(unit, NBI_ILKN_RX_0_RETRANSMIT_DEBUG_INFORMATIONr,
            reg0_val32, ILKN_RX_0_RETRANS_REACHED_TIMOUTf),
            soc_reg_field_get(unit, NBI_ILKN_RX_1_RETRANSMIT_DEBUG_INFORMATIONr,
            reg1_val32, ILKN_RX_1_RETRANS_REACHED_TIMOUTf));
    cli_out("LAST GOOD SUB SEQ NUM \t\t%d\t\t%d\n",
            soc_reg_field_get(unit, NBI_ILKN_RX_0_RETRANSMIT_DEBUG_INFORMATIONr,
            reg0_val32, ILKN_RX_0_RETRANS_LAST_GOOD_SUB_SEQ_NUMf),
            soc_reg_field_get(unit, NBI_ILKN_RX_1_RETRANSMIT_DEBUG_INFORMATIONr,
            reg1_val32, ILKN_RX_1_RETRANS_LAST_GOOD_SUB_SEQ_NUMf));
    cli_out("LAST GOOD SEQ NUM \t\t%d\t\t%d\n",
            soc_reg_field_get(unit, NBI_ILKN_RX_0_RETRANSMIT_DEBUG_INFORMATIONr,
            reg0_val32, ILKN_RX_0_RETRANS_LAST_GOOD_SEQ_NUMf),
            soc_reg_field_get(unit, NBI_ILKN_RX_1_RETRANSMIT_DEBUG_INFORMATIONr,
            reg1_val32, ILKN_RX_1_RETRANS_LAST_GOOD_SEQ_NUMf));
    cli_out("FSM STATES:\n");
    cli_out("\t7 \t\t\t%d\t\t%d\n",
            soc_reg_field_get(unit, NBI_ILKN_RX_0_RETRANSMIT_DEBUG_INFORMATIONr,
            reg0_val32, ILKN_RX_0_RETRANS_FSM_STATE_7f),
            soc_reg_field_get(unit, NBI_ILKN_RX_1_RETRANSMIT_DEBUG_INFORMATIONr,
            reg1_val32, ILKN_RX_1_RETRANS_FSM_STATE_7f));
    cli_out("\t6 \t\t\t%d\t\t%d\n",
            soc_reg_field_get(unit, NBI_ILKN_RX_0_RETRANSMIT_DEBUG_INFORMATIONr,
            reg0_val32, ILKN_RX_0_RETRANS_FSM_STATE_6f),
            soc_reg_field_get(unit, NBI_ILKN_RX_1_RETRANSMIT_DEBUG_INFORMATIONr,
            reg1_val32, ILKN_RX_1_RETRANS_FSM_STATE_6f));
    cli_out("\t5 \t\t\t%d\t\t%d\n",
            soc_reg_field_get(unit, NBI_ILKN_RX_0_RETRANSMIT_DEBUG_INFORMATIONr,
            reg0_val32, ILKN_RX_0_RETRANS_FSM_STATE_5f),
            soc_reg_field_get(unit, NBI_ILKN_RX_1_RETRANSMIT_DEBUG_INFORMATIONr,
            reg1_val32, ILKN_RX_1_RETRANS_FSM_STATE_5f));
    cli_out("\t4 \t\t\t%d\t\t%d\n",
            soc_reg_field_get(unit, NBI_ILKN_RX_0_RETRANSMIT_DEBUG_INFORMATIONr,
            reg0_val32, ILKN_RX_0_RETRANS_FSM_STATE_4f),
            soc_reg_field_get(unit, NBI_ILKN_RX_1_RETRANSMIT_DEBUG_INFORMATIONr,
            reg1_val32, ILKN_RX_1_RETRANS_FSM_STATE_4f));
    cli_out("\t3 \t\t\t%d\t\t%d\n",
            soc_reg_field_get(unit, NBI_ILKN_RX_0_RETRANSMIT_DEBUG_INFORMATIONr,
            reg0_val32, ILKN_RX_0_RETRANS_FSM_STATE_3f),
            soc_reg_field_get(unit, NBI_ILKN_RX_1_RETRANSMIT_DEBUG_INFORMATIONr,
            reg1_val32, ILKN_RX_1_RETRANS_FSM_STATE_3f));
    cli_out("\t2 \t\t\t%d\t\t%d\n",
            soc_reg_field_get(unit, NBI_ILKN_RX_0_RETRANSMIT_DEBUG_INFORMATIONr,
            reg0_val32, ILKN_RX_0_RETRANS_FSM_STATE_2f),
            soc_reg_field_get(unit, NBI_ILKN_RX_1_RETRANSMIT_DEBUG_INFORMATIONr,
            reg1_val32, ILKN_RX_1_RETRANS_FSM_STATE_2f));
    cli_out("\t1 \t\t\t%d\t\t%d\n",
            soc_reg_field_get(unit, NBI_ILKN_RX_0_RETRANSMIT_DEBUG_INFORMATIONr,
            reg0_val32, ILKN_RX_0_RETRANS_FSM_STATE_1f),
            soc_reg_field_get(unit, NBI_ILKN_RX_1_RETRANSMIT_DEBUG_INFORMATIONr,
            reg1_val32, ILKN_RX_1_RETRANS_FSM_STATE_1f));
    cli_out("\t0 \t\t\t%d\t\t%d\n",
            soc_reg_field_get(unit, NBI_ILKN_RX_0_RETRANSMIT_DEBUG_INFORMATIONr,
            reg0_val32, ILKN_RX_0_RETRANS_FSM_STATE_0f),
            soc_reg_field_get(unit, NBI_ILKN_RX_1_RETRANSMIT_DEBUG_INFORMATIONr,
            reg1_val32, ILKN_RX_1_RETRANS_FSM_STATE_0f));
    cli_out("FSM STATE \t\t\t%d\t\t%d\n",
            soc_reg_field_get(unit, NBI_ILKN_RX_0_RETRANSMIT_DEBUG_INFORMATIONr,
            reg0_val32, ILKN_RX_0_RETRANS_FSM_STATEf),
            soc_reg_field_get(unit, NBI_ILKN_RX_1_RETRANSMIT_DEBUG_INFORMATIONr,
            reg1_val32, ILKN_RX_1_RETRANS_FSM_STATEf));
    cli_out("DISC \t\t\t\t%d\t\t%d\n",
            soc_reg_field_get(unit, NBI_ILKN_RX_0_RETRANSMIT_DEBUG_INFORMATIONr,
            reg0_val32, ILKN_RX_0_RETRANS_DISCf),
            soc_reg_field_get(unit, NBI_ILKN_RX_1_RETRANSMIT_DEBUG_INFORMATIONr,
            reg1_val32, ILKN_RX_1_RETRANS_DISCf));
    cli_out("CRC 24 ERR \t\t\t%d\t\t%d\n",
            soc_reg_field_get(unit, NBI_ILKN_RX_0_RETRANSMIT_DEBUG_INFORMATIONr,
            reg0_val32, ILKN_RX_0_RETRANS_CRC_24_ERRf),
            soc_reg_field_get(unit, NBI_ILKN_RX_1_RETRANSMIT_DEBUG_INFORMATIONr,
            reg1_val32, ILKN_RX_1_RETRANS_CRC_24_ERRf));
    cli_out("ACK RECEIVED \t\t\t%d\t\t%d\n",
            soc_reg_field_get(unit, NBI_ILKN_RX_0_RETRANSMIT_DEBUG_INFORMATIONr,
            reg0_val32, ILKN_RX_0_RETRANS_ACK_RECEIVEDf),
            soc_reg_field_get(unit, NBI_ILKN_RX_1_RETRANSMIT_DEBUG_INFORMATIONr,
            reg1_val32, ILKN_RX_1_RETRANS_ACK_RECEIVEDf));

    rv = READ_NBI_ILKN_DEBUG_STATUSr(unit, &reg_val64);
    if (SOC_FAILURE(rv)) {
        cli_out("Failed to read NBI_ILKN_RX_0_RETRANSMIT_DEBUG_INFORMATION register\n");
        return CMD_FAIL;
    }
    cli_out("RX RETRANS LATENCY \t\t%d\t\t%d\n",
            soc_reg_field_get(unit, NBI_ILKN_DEBUG_STATUSr,
            reg0_val32, ILKN_RX_RETRANS_LATENCY_0f),
            soc_reg_field_get(unit, NBI_ILKN_DEBUG_STATUSr,
            reg1_val32, ILKN_RX_RETRANS_LATENCY_1f));

    return rv;
}

#ifdef BCM_JERICHO_SUPPORT
STATIC cmd_result_t
cmd_dpp_jer_retransmit(int unit, args_t *a) {
    int rv = CMD_OK;
    int index = 0;
    uint32 nbl_port = 0;
    uint32 reg0_val32 = 0;
    uint32 reg1_val32 = 0;
    uint32 nbil0_reg0_val32 = 0;
    uint32 nbil0_reg1_val32 = 0;
    uint32 nbil1_reg0_val32 = 0;
    uint32 nbil1_reg1_val32 = 0;
    uint64 reg0_val64 = COMPILER_64_INIT(0, 0);
    uint64 reg1_val64 = COMPILER_64_INIT(0, 0);
    uint64 nbil0_reg0_val64 = COMPILER_64_INIT(0, 0);
    uint64 nbil0_reg1_val64 = COMPILER_64_INIT(0, 0);
    uint64 nbil1_reg0_val64 = COMPILER_64_INIT(0, 0);
    uint64 nbil1_reg1_val64 = COMPILER_64_INIT(0, 0);
    char buffer_reg0[8];
    char buffer_reg1[8];
    char buffer_nbil0_reg0[8];
    char buffer_nbil0_reg1[8];
    char buffer_nbil1_reg0[8];
    char buffer_nbil1_reg1[8];

    cli_out("\t\t\t\tNBIH HRF0:\t\tNBIH HRF1:\t\tNBIL0 HRF0:\t\tNBIL0 HRF1:\t\tNBIL1 HRF0:\t\tNBIL1 HRF1:\n");

    if (SOC_IS_JERICHO_PLUS_ONLY(unit)) {/* 64 bit */
        index = 0;
        rv = READ_NBIH_TX_RECEIVED_RETRANS_REQ_CNT_HRFr(unit, index, &reg0_val64);
        if (SOC_FAILURE(rv)) {
            cli_out("Failed to read NBIH_TX_RECEIVED_RETRANS_REQ_CNT_HRF register.\n");
            return CMD_FAIL;
        }
        sal_memset(buffer_reg0, 0, sizeof(buffer_reg0));
        format_uint64_decimal(buffer_reg0, reg0_val64, ',');

        index = 1;
        rv = READ_NBIH_TX_RECEIVED_RETRANS_REQ_CNT_HRFr(unit, index, &reg1_val64);
        if (SOC_FAILURE(rv)) {
            cli_out("Failed to read NBIH_TX_RECEIVED_RETRANS_REQ_CNT_HRF register.\n");
            return CMD_FAIL;
        }
        sal_memset(buffer_reg1, 0, sizeof(buffer_reg1));
        format_uint64_decimal(buffer_reg1, reg1_val64, ',');

        index = 0;
        nbl_port = 0;
        rv = READ_NBIL_TX_RECEIVED_RETRANS_REQ_CNT_HRFr(unit, nbl_port, index, &nbil0_reg0_val64);
        if (SOC_FAILURE(rv)) {
            cli_out("Failed to read NBIH_TX_RECEIVED_RETRANS_REQ_CNT_HRF register.\n");
            return CMD_FAIL;
        }
        sal_memset(buffer_nbil0_reg0, 0, sizeof(buffer_nbil0_reg0));
        format_uint64_decimal(buffer_nbil0_reg0, nbil0_reg0_val64, ',');

        index = 1;
        nbl_port = 0;
        rv = READ_NBIL_TX_RECEIVED_RETRANS_REQ_CNT_HRFr(unit, nbl_port, index, &nbil0_reg1_val64);
        if (SOC_FAILURE(rv)) {
            cli_out("Failed to read NBIH_TX_RECEIVED_RETRANS_REQ_CNT_HRF register.\n");
            return CMD_FAIL;
        }
        sal_memset(buffer_nbil0_reg1, 0, sizeof(buffer_nbil0_reg1));
        format_uint64_decimal(buffer_nbil0_reg1, nbil0_reg1_val64, ',');

        index = 0;
        nbl_port = 1;
        rv = READ_NBIL_TX_RECEIVED_RETRANS_REQ_CNT_HRFr(unit, nbl_port, index, &nbil1_reg0_val64);
        if (SOC_FAILURE(rv)) {
            cli_out("Failed to read NBIH_TX_RECEIVED_RETRANS_REQ_CNT_HRF register.\n");
            return CMD_FAIL;
        }
        sal_memset(buffer_nbil1_reg0, 0, sizeof(buffer_nbil1_reg0));
        format_uint64_decimal(buffer_nbil1_reg0, nbil1_reg0_val64, ',');

        index = 0;
        nbl_port = 1;
        rv = READ_NBIL_TX_RECEIVED_RETRANS_REQ_CNT_HRFr(unit, nbl_port, index, &nbil1_reg1_val64);
        if (SOC_FAILURE(rv)) {
            cli_out("Failed to read NBIH_TX_RECEIVED_RETRANS_REQ_CNT_HRF register.\n");
            return CMD_FAIL;
        }
        sal_memset(buffer_nbil1_reg1, 0, sizeof(buffer_nbil1_reg1));
        format_uint64_decimal(buffer_nbil1_reg1, nbil1_reg1_val64, ',');

        cli_out("TX RECEIVED RETRANS REQ CNT \t\t%s\t\t\t%s\t\t\t%s\t\t\t%s\t\t\t%s\t\t\t%s\n",
                buffer_reg0, buffer_reg1, buffer_nbil0_reg0, buffer_nbil0_reg1, buffer_nbil1_reg0, buffer_nbil1_reg1);

        index = 0;
        rv = READ_NBIH_TX_IGNORED_RETRANS_REQ_CNT_HRFr(unit, index, &reg0_val64);
        if (SOC_FAILURE(rv)) {
            cli_out("Failed to read NBIH_TX_RECEIVED_RETRANS_REQ_CNT_HRF register.\n");
            return CMD_FAIL;
        }
        sal_memset(buffer_reg0, 0, sizeof(buffer_reg0));
        format_uint64_decimal(buffer_reg0, reg0_val64, ',');

        index = 1;
        rv = READ_NBIH_TX_IGNORED_RETRANS_REQ_CNT_HRFr(unit, index, &reg1_val64);
        if (SOC_FAILURE(rv)) {
            cli_out("Failed to read NBIH_TX_RECEIVED_RETRANS_REQ_CNT_HRF register.\n");
            return CMD_FAIL;
        }
        sal_memset(buffer_reg1, 0, sizeof(buffer_reg1));
        format_uint64_decimal(buffer_reg1, reg1_val64, ',');

        index = 0;
        nbl_port = 0;
        rv = READ_NBIL_TX_IGNORED_RETRANS_REQ_CNT_HRFr(unit, nbl_port, index, &nbil0_reg0_val64);
        if (SOC_FAILURE(rv)) {
            cli_out("Failed to read NBIH_TX_RECEIVED_RETRANS_REQ_CNT_HRF register.\n");
            return CMD_FAIL;
        }
        sal_memset(buffer_nbil0_reg0, 0, sizeof(buffer_nbil0_reg0));
        format_uint64_decimal(buffer_nbil0_reg0, nbil0_reg0_val64, ',');

        index = 1;
        nbl_port = 0;
        rv = READ_NBIL_TX_IGNORED_RETRANS_REQ_CNT_HRFr(unit, nbl_port, index, &nbil0_reg1_val64);
        if (SOC_FAILURE(rv)) {
            cli_out("Failed to read NBIH_TX_RECEIVED_RETRANS_REQ_CNT_HRF register.\n");
            return CMD_FAIL;
        }
        sal_memset(buffer_nbil0_reg1, 0, sizeof(buffer_nbil0_reg1));
        format_uint64_decimal(buffer_nbil0_reg1, nbil0_reg1_val64, ',');

        index = 0;
        nbl_port = 1;
        rv = READ_NBIL_TX_IGNORED_RETRANS_REQ_CNT_HRFr(unit, nbl_port, index, &nbil1_reg0_val64);
        if (SOC_FAILURE(rv)) {
            cli_out("Failed to read NBIH_TX_RECEIVED_RETRANS_REQ_CNT_HRF register.\n");
            return CMD_FAIL;
        }
        sal_memset(buffer_nbil1_reg0, 0, sizeof(buffer_nbil1_reg0));
        format_uint64_decimal(buffer_nbil1_reg0, nbil1_reg0_val64, ',');

        index = 0;
        nbl_port = 1;
        rv = READ_NBIL_TX_IGNORED_RETRANS_REQ_CNT_HRFr(unit, nbl_port, index, &nbil1_reg1_val64);
        if (SOC_FAILURE(rv)) {
            cli_out("Failed to read NBIH_TX_RECEIVED_RETRANS_REQ_CNT_HRF register.\n");
            return CMD_FAIL;
        }
        sal_memset(buffer_nbil1_reg1, 0, sizeof(buffer_nbil1_reg1));
        format_uint64_decimal(buffer_nbil1_reg1, nbil1_reg1_val64, ',');

        cli_out("TX IGNORED RETRANS REQ CNT \t\t%s\t\t\t%s\t\t\t%s\t\t\t%s\t\t\t%s\t\t\t%s\n",
                buffer_reg0, buffer_reg1, buffer_nbil0_reg0, buffer_nbil0_reg1, buffer_nbil1_reg0, buffer_nbil1_reg1);
    } else {/* 32 bit */
        index = 0;
        rv = READ_NBIH_TX_RECEIVED_RETRANS_REQ_CNT_HRFr_REG32(unit, index, &reg0_val32);
        if (SOC_FAILURE(rv)) {
            cli_out("Failed to read NBIH_TX_RECEIVED_RETRANS_REQ_CNT_HRF register.\n");
            return CMD_FAIL;
        }
        /* check counter overflow */
        if (((reg0_val32 >> 31) & 0x1) == 0x1) {
            cli_out("NBIH_TX_RECEIVED_RETRANS_REQ_CNT_HRF register overflow.\n");
        }

        index = 1;
        rv = READ_NBIH_TX_RECEIVED_RETRANS_REQ_CNT_HRFr_REG32(unit, index, &reg1_val32);
        if (SOC_FAILURE(rv)) {
            cli_out("Failed to read NBIH_TX_RECEIVED_RETRANS_REQ_CNT_HRF register.\n");
            return CMD_FAIL;
        }
        /* check counter overflow */
        if (((reg1_val32 >> 31) & 0x1) == 0x1) {
            cli_out("Failed to read NBIH_TX_RECEIVED_RETRANS_REQ_CNT_HRF register.\n");
        }

        index = 0;
        nbl_port = 0;
        rv = READ_NBIL_TX_RECEIVED_RETRANS_REQ_CNT_HRFr_REG32(unit, nbl_port, index, &nbil0_reg0_val32);
        if (SOC_FAILURE(rv)) {
            cli_out("Failed to read NBIH_TX_RECEIVED_RETRANS_REQ_CNT_HRF register.\n");
            return CMD_FAIL;
        }
        /* check counter overflow */
        if (((nbil0_reg0_val32 >> 31) & 0x1) == 0x1) {
            cli_out("NBIH_TX_RECEIVED_RETRANS_REQ_CNT_HRF register overflow.\n");
        }

        index = 1;
        nbl_port = 0;
        rv = READ_NBIL_TX_RECEIVED_RETRANS_REQ_CNT_HRFr_REG32(unit, nbl_port, index, &nbil0_reg1_val32);
        if (SOC_FAILURE(rv)) {
            cli_out("Failed to read NBIH_TX_RECEIVED_RETRANS_REQ_CNT_HRF register.\n");
            return CMD_FAIL;
        }
        /* check counter overflow */
        if (((nbil0_reg1_val32 >> 31) & 0x1) == 0x1) {
            cli_out("Failed to read NBIH_TX_RECEIVED_RETRANS_REQ_CNT_HRF register.\n");
        }

        index = 0;
        nbl_port = 1;
        rv = READ_NBIL_TX_RECEIVED_RETRANS_REQ_CNT_HRFr_REG32(unit, nbl_port, index, &nbil1_reg0_val32);
        if (SOC_FAILURE(rv)) {
            cli_out("Failed to read NBIH_TX_RECEIVED_RETRANS_REQ_CNT_HRF register.\n");
            return CMD_FAIL;
        }
        /* check counter overflow */
        if (((nbil1_reg0_val32 >> 31) & 0x1) == 0x1) {
            cli_out("NBIH_TX_RECEIVED_RETRANS_REQ_CNT_HRF register overflow.\n");
        }

        index = 1;
        nbl_port = 1;
        rv = READ_NBIL_TX_RECEIVED_RETRANS_REQ_CNT_HRFr_REG32(unit, nbl_port, index, &nbil1_reg1_val32);
        if (SOC_FAILURE(rv)) {
            cli_out("Failed to read NBIH_TX_RECEIVED_RETRANS_REQ_CNT_HRF register.\n");
            return CMD_FAIL;
        }
        /* check counter overflow */
        if (((nbil1_reg1_val32 >> 31) & 0x1) == 0x1) {
            cli_out("NBIH_TX_RECEIVED_RETRANS_REQ_CNT_HRF register overflow.\n");
        }
        cli_out("TX RECEIVED RETRANS REQ CNT \t\t%d\t\t\t%d\t\t\t%d\t\t\t%d\t\t\t%d\t\t\t%d\n",
                reg0_val32, reg1_val32, nbil0_reg0_val32, nbil0_reg1_val32, nbil1_reg0_val32, nbil1_reg1_val32);

        index = 0;
        rv = READ_NBIH_TX_IGNORED_RETRANS_REQ_CNT_HRFr_REG32(unit, index, &reg0_val32);
        if (SOC_FAILURE(rv)) {
            cli_out("Failed to read NBIH_TX_RECEIVED_RETRANS_REQ_CNT_HRF register.\n");
            return CMD_FAIL;
        }
        /* check counter overflow */
        if (((reg0_val32 >> 31) & 0x1) == 0x1) {
            cli_out("NBIH_TX_RECEIVED_RETRANS_REQ_CNT_HRF register overflow.\n");
        }

        index = 1;
        rv = READ_NBIH_TX_IGNORED_RETRANS_REQ_CNT_HRFr_REG32(unit, index, &reg1_val32);
        if (SOC_FAILURE(rv)) {
            cli_out("Failed to read NBIH_TX_RECEIVED_RETRANS_REQ_CNT_HRF register.\n");
            return CMD_FAIL;
        }
        /* check counter overflow */
        if (((reg1_val32 >> 31) & 0x1) == 0x1) {
            cli_out("NBIH_TX_RECEIVED_RETRANS_REQ_CNT_HRF register overflow.\n");
        }

        index = 0;
        nbl_port = 0;
        rv = READ_NBIL_TX_IGNORED_RETRANS_REQ_CNT_HRFr_REG32(unit, nbl_port, index, &nbil0_reg0_val32);
        if (SOC_FAILURE(rv)) {
            cli_out("Failed to read NBIH_TX_RECEIVED_RETRANS_REQ_CNT_HRF register.\n");
            return CMD_FAIL;
        }
        /* check counter overflow */
        if (((nbil0_reg0_val32 >> 31) & 0x1) == 0x1) {
            cli_out("Failed to read NBIH_TX_RECEIVED_RETRANS_REQ_CNT_HRF register.\n");
        }

        index = 1;
        nbl_port = 0;
        rv = READ_NBIL_TX_IGNORED_RETRANS_REQ_CNT_HRFr_REG32(unit, nbl_port, index, &nbil0_reg1_val32);
        if (SOC_FAILURE(rv)) {
            cli_out("Failed to read NBIH_TX_RECEIVED_RETRANS_REQ_CNT_HRF register.\n");
            return CMD_FAIL;
        }
        /* check counter overflow */
        if (((nbil0_reg1_val32 >> 31) & 0x1) == 0x1) {
            cli_out("NBIH_TX_RECEIVED_RETRANS_REQ_CNT_HRF register overflow.\n");
        }

        index = 0;
        nbl_port = 1;
        rv = READ_NBIL_TX_IGNORED_RETRANS_REQ_CNT_HRFr_REG32(unit, nbl_port, index, &nbil1_reg0_val32);
        if (SOC_FAILURE(rv)) {
            cli_out("Failed to read NBIH_TX_RECEIVED_RETRANS_REQ_CNT_HRF register.\n");
            return CMD_FAIL;
        }
        /* check counter overflow */
        if (((nbil1_reg0_val32 >> 31) & 0x1) == 0x1) {
            cli_out("NBIH_TX_RECEIVED_RETRANS_REQ_CNT_HRF register overflow.\n");
        }

        index = 1;
        nbl_port = 1;
        rv = READ_NBIL_TX_IGNORED_RETRANS_REQ_CNT_HRFr_REG32(unit, nbl_port, index, &nbil1_reg1_val32);
        if (SOC_FAILURE(rv)) {
            cli_out("Failed to read NBIH_TX_RECEIVED_RETRANS_REQ_CNT_HRF register.\n");
            return CMD_FAIL;
        }
        /* check counter overflow */
        if (((nbil1_reg1_val32 >> 31) & 0x1) == 0x1) {
            cli_out("NBIH_TX_RECEIVED_RETRANS_REQ_CNT_HRF register overflow.\n");
        }
        cli_out("TX IGNORED RETRANS REQ CNT \t\t%d\t\t\t%d\t\t\t%d\t\t\t%d\t\t\t%d\t\t\t%d\n",
                reg0_val32, reg1_val32, nbil0_reg0_val32, nbil0_reg1_val32, nbil1_reg0_val32, nbil1_reg1_val32);
    }

    index = 0;
    rv = READ_ILKN_PMH_ILKN_RX_RETRANSMIT_DEBUG_INFORMATIONr(unit, REG_PORT_ANY, index, &reg0_val32);
    if (SOC_FAILURE(rv)) {
       cli_out("Failed to read ILKN_PMH_ILKN_RX_RETRANSMIT_DEBUG_INFORMATION register\n");
       return CMD_FAIL;
    }

    index = 1;
    rv = READ_ILKN_PMH_ILKN_RX_RETRANSMIT_DEBUG_INFORMATIONr(unit, REG_PORT_ANY, index, &reg1_val32);
    if (SOC_FAILURE(rv)) {
       cli_out("Failed to read ILKN_PMH_ILKN_RX_RETRANSMIT_DEBUG_INFORMATION register\n");
       return CMD_FAIL;
    }

    index = 0;
    nbl_port = 0;
    rv = READ_ILKN_PML_ILKN_RX_RETRANSMIT_DEBUG_INFORMATIONr(unit, nbl_port, index, &nbil0_reg0_val32);
    if (SOC_FAILURE(rv)) {
        cli_out("Failed to read ILKN_PML_ILKN_RX_RETRANSMIT_DEBUG_INFORMATION register\n");
        return CMD_FAIL;
    }

    index = 1;
    nbl_port = 0;
    rv = READ_ILKN_PML_ILKN_RX_RETRANSMIT_DEBUG_INFORMATIONr(unit, nbl_port, index, &nbil0_reg1_val32);
    if (SOC_FAILURE(rv)) {
        cli_out("Failed to read ILKN_PML_ILKN_RX_RETRANSMIT_DEBUG_INFORMATION register\n");
        return CMD_FAIL;
    }

    index = 0;
    nbl_port = 1;
    rv = READ_ILKN_PML_ILKN_RX_RETRANSMIT_DEBUG_INFORMATIONr(unit, nbl_port, index, &nbil1_reg0_val32);
    if (SOC_FAILURE(rv)) {
        cli_out("Failed to read ILKN_PML_ILKN_RX_RETRANSMIT_DEBUG_INFORMATION register\n");
        return CMD_FAIL;
    }

    index = 1;
    nbl_port = 1;
    rv = READ_ILKN_PML_ILKN_RX_RETRANSMIT_DEBUG_INFORMATIONr(unit, nbl_port, index, &nbil1_reg1_val32);
    if (SOC_FAILURE(rv)) {
        cli_out("Failed to read ILKN_PML_ILKN_RX_RETRANSMIT_DEBUG_INFORMATION register\n");
        return CMD_FAIL;
    }

    cli_out("FSM STATES:\n");
    cli_out("\tF \t\t\t\t%d\t\t\t%d\t\t\t%d\t\t\t%d\t\t\t%d\t\t\t%d\n",
           soc_reg_field_get(unit, ILKN_PMH_ILKN_RX_RETRANSMIT_DEBUG_INFORMATIONr,
           reg0_val32, ILKN_RX_N_RETRANS_FSM_STATE_Ff),
           soc_reg_field_get(unit, ILKN_PMH_ILKN_RX_RETRANSMIT_DEBUG_INFORMATIONr,
           reg1_val32, ILKN_RX_N_RETRANS_FSM_STATE_Ff),
           soc_reg_field_get(unit, ILKN_PML_ILKN_RX_RETRANSMIT_DEBUG_INFORMATIONr,
           nbil0_reg0_val32, ILKN_RX_N_RETRANS_FSM_STATE_Ff),
           soc_reg_field_get(unit, ILKN_PML_ILKN_RX_RETRANSMIT_DEBUG_INFORMATIONr,
           nbil0_reg1_val32, ILKN_RX_N_RETRANS_FSM_STATE_Ff),
           soc_reg_field_get(unit, ILKN_PML_ILKN_RX_RETRANSMIT_DEBUG_INFORMATIONr,
           nbil1_reg0_val32, ILKN_RX_N_RETRANS_FSM_STATE_Ff),
           soc_reg_field_get(unit, ILKN_PML_ILKN_RX_RETRANSMIT_DEBUG_INFORMATIONr,
           nbil1_reg1_val32, ILKN_RX_N_RETRANS_FSM_STATE_Ff));
    cli_out("\tA \t\t\t\t%d\t\t\t%d\t\t\t%d\t\t\t%d\t\t\t%d\t\t\t%d\n",
           soc_reg_field_get(unit, ILKN_PMH_ILKN_RX_RETRANSMIT_DEBUG_INFORMATIONr,
           reg0_val32, ILKN_RX_N_RETRANS_FSM_STATE_Af),
           soc_reg_field_get(unit, ILKN_PMH_ILKN_RX_RETRANSMIT_DEBUG_INFORMATIONr,
           reg1_val32, ILKN_RX_N_RETRANS_FSM_STATE_Af),
           soc_reg_field_get(unit, ILKN_PML_ILKN_RX_RETRANSMIT_DEBUG_INFORMATIONr,
           nbil0_reg0_val32, ILKN_RX_N_RETRANS_FSM_STATE_Af),
           soc_reg_field_get(unit, ILKN_PML_ILKN_RX_RETRANSMIT_DEBUG_INFORMATIONr,
           nbil0_reg1_val32, ILKN_RX_N_RETRANS_FSM_STATE_Af),
           soc_reg_field_get(unit, ILKN_PML_ILKN_RX_RETRANSMIT_DEBUG_INFORMATIONr,
           nbil1_reg0_val32, ILKN_RX_N_RETRANS_FSM_STATE_Af),
           soc_reg_field_get(unit, ILKN_PML_ILKN_RX_RETRANSMIT_DEBUG_INFORMATIONr,
           nbil1_reg1_val32, ILKN_RX_N_RETRANS_FSM_STATE_Af));
    cli_out("\t9 \t\t\t\t%d\t\t\t%d\t\t\t%d\t\t\t%d\t\t\t%d\t\t\t%d\n",
           soc_reg_field_get(unit, ILKN_PMH_ILKN_RX_RETRANSMIT_DEBUG_INFORMATIONr,
           reg0_val32, ILKN_RX_N_RETRANS_FSM_STATE_9f),
           soc_reg_field_get(unit, ILKN_PMH_ILKN_RX_RETRANSMIT_DEBUG_INFORMATIONr,
           reg1_val32, ILKN_RX_N_RETRANS_FSM_STATE_9f),
           soc_reg_field_get(unit, ILKN_PML_ILKN_RX_RETRANSMIT_DEBUG_INFORMATIONr,
           nbil0_reg0_val32, ILKN_RX_N_RETRANS_FSM_STATE_9f),
           soc_reg_field_get(unit, ILKN_PML_ILKN_RX_RETRANSMIT_DEBUG_INFORMATIONr,
           nbil0_reg1_val32, ILKN_RX_N_RETRANS_FSM_STATE_9f),
           soc_reg_field_get(unit, ILKN_PML_ILKN_RX_RETRANSMIT_DEBUG_INFORMATIONr,
           nbil1_reg0_val32, ILKN_RX_N_RETRANS_FSM_STATE_9f),
           soc_reg_field_get(unit, ILKN_PML_ILKN_RX_RETRANSMIT_DEBUG_INFORMATIONr,
           nbil1_reg1_val32, ILKN_RX_N_RETRANS_FSM_STATE_9f));
    cli_out("\t8 \t\t\t\t%d\t\t\t%d\t\t\t%d\t\t\t%d\t\t\t%d\t\t\t%d\n",
           soc_reg_field_get(unit, ILKN_PMH_ILKN_RX_RETRANSMIT_DEBUG_INFORMATIONr,
           reg0_val32, ILKN_RX_N_RETRANS_FSM_STATE_8f),
           soc_reg_field_get(unit, ILKN_PMH_ILKN_RX_RETRANSMIT_DEBUG_INFORMATIONr,
           reg1_val32, ILKN_RX_N_RETRANS_FSM_STATE_8f),
           soc_reg_field_get(unit, ILKN_PML_ILKN_RX_RETRANSMIT_DEBUG_INFORMATIONr,
           nbil0_reg0_val32, ILKN_RX_N_RETRANS_FSM_STATE_8f),
           soc_reg_field_get(unit, ILKN_PML_ILKN_RX_RETRANSMIT_DEBUG_INFORMATIONr,
           nbil0_reg1_val32, ILKN_RX_N_RETRANS_FSM_STATE_8f),
           soc_reg_field_get(unit, ILKN_PML_ILKN_RX_RETRANSMIT_DEBUG_INFORMATIONr,
           nbil1_reg0_val32, ILKN_RX_N_RETRANS_FSM_STATE_8f),
           soc_reg_field_get(unit, ILKN_PML_ILKN_RX_RETRANSMIT_DEBUG_INFORMATIONr,
           nbil1_reg1_val32, ILKN_RX_N_RETRANS_FSM_STATE_8f));
    cli_out("\t7 \t\t\t\t%d\t\t\t%d\t\t\t%d\t\t\t%d\t\t\t%d\t\t\t%d\n",
           soc_reg_field_get(unit, ILKN_PMH_ILKN_RX_RETRANSMIT_DEBUG_INFORMATIONr,
           reg0_val32, ILKN_RX_N_RETRANS_FSM_STATE_7f),
           soc_reg_field_get(unit, ILKN_PMH_ILKN_RX_RETRANSMIT_DEBUG_INFORMATIONr,
           reg1_val32, ILKN_RX_N_RETRANS_FSM_STATE_7f),
           soc_reg_field_get(unit, ILKN_PML_ILKN_RX_RETRANSMIT_DEBUG_INFORMATIONr,
           nbil0_reg0_val32, ILKN_RX_N_RETRANS_FSM_STATE_7f),
           soc_reg_field_get(unit, ILKN_PML_ILKN_RX_RETRANSMIT_DEBUG_INFORMATIONr,
           nbil0_reg1_val32, ILKN_RX_N_RETRANS_FSM_STATE_7f),
           soc_reg_field_get(unit, ILKN_PML_ILKN_RX_RETRANSMIT_DEBUG_INFORMATIONr,
           nbil1_reg0_val32, ILKN_RX_N_RETRANS_FSM_STATE_7f),
           soc_reg_field_get(unit, ILKN_PML_ILKN_RX_RETRANSMIT_DEBUG_INFORMATIONr,
           nbil1_reg1_val32, ILKN_RX_N_RETRANS_FSM_STATE_7f));
    cli_out("\t6 \t\t\t\t%d\t\t\t%d\t\t\t%d\t\t\t%d\t\t\t%d\t\t\t%d\n",
           soc_reg_field_get(unit, ILKN_PMH_ILKN_RX_RETRANSMIT_DEBUG_INFORMATIONr,
           reg0_val32, ILKN_RX_N_RETRANS_FSM_STATE_6f),
           soc_reg_field_get(unit, ILKN_PMH_ILKN_RX_RETRANSMIT_DEBUG_INFORMATIONr,
           reg1_val32, ILKN_RX_N_RETRANS_FSM_STATE_6f),
           soc_reg_field_get(unit, ILKN_PML_ILKN_RX_RETRANSMIT_DEBUG_INFORMATIONr,
           nbil0_reg0_val32, ILKN_RX_N_RETRANS_FSM_STATE_6f),
           soc_reg_field_get(unit, ILKN_PML_ILKN_RX_RETRANSMIT_DEBUG_INFORMATIONr,
           nbil0_reg1_val32, ILKN_RX_N_RETRANS_FSM_STATE_6f),
           soc_reg_field_get(unit, ILKN_PML_ILKN_RX_RETRANSMIT_DEBUG_INFORMATIONr,
           nbil1_reg0_val32, ILKN_RX_N_RETRANS_FSM_STATE_6f),
           soc_reg_field_get(unit, ILKN_PML_ILKN_RX_RETRANSMIT_DEBUG_INFORMATIONr,
           nbil1_reg1_val32, ILKN_RX_N_RETRANS_FSM_STATE_6f));
    cli_out("\t5 \t\t\t\t%d\t\t\t%d\t\t\t%d\t\t\t%d\t\t\t%d\t\t\t%d\n",
           soc_reg_field_get(unit, ILKN_PMH_ILKN_RX_RETRANSMIT_DEBUG_INFORMATIONr,
           reg0_val32, ILKN_RX_N_RETRANS_FSM_STATE_5f),
           soc_reg_field_get(unit, ILKN_PMH_ILKN_RX_RETRANSMIT_DEBUG_INFORMATIONr,
           reg1_val32, ILKN_RX_N_RETRANS_FSM_STATE_5f),
           soc_reg_field_get(unit, ILKN_PML_ILKN_RX_RETRANSMIT_DEBUG_INFORMATIONr,
           nbil0_reg0_val32, ILKN_RX_N_RETRANS_FSM_STATE_5f),
           soc_reg_field_get(unit, ILKN_PML_ILKN_RX_RETRANSMIT_DEBUG_INFORMATIONr,
           nbil0_reg1_val32, ILKN_RX_N_RETRANS_FSM_STATE_5f),
           soc_reg_field_get(unit, ILKN_PML_ILKN_RX_RETRANSMIT_DEBUG_INFORMATIONr,
           nbil1_reg0_val32, ILKN_RX_N_RETRANS_FSM_STATE_5f),
           soc_reg_field_get(unit, ILKN_PML_ILKN_RX_RETRANSMIT_DEBUG_INFORMATIONr,
           nbil1_reg1_val32, ILKN_RX_N_RETRANS_FSM_STATE_5f));
    cli_out("\t4 \t\t\t\t%d\t\t\t%d\t\t\t%d\t\t\t%d\t\t\t%d\t\t\t%d\n",
           soc_reg_field_get(unit, ILKN_PMH_ILKN_RX_RETRANSMIT_DEBUG_INFORMATIONr,
           reg0_val32, ILKN_RX_N_RETRANS_FSM_STATE_4f),
           soc_reg_field_get(unit, ILKN_PMH_ILKN_RX_RETRANSMIT_DEBUG_INFORMATIONr,
           reg1_val32, ILKN_RX_N_RETRANS_FSM_STATE_4f),
           soc_reg_field_get(unit, ILKN_PML_ILKN_RX_RETRANSMIT_DEBUG_INFORMATIONr,
           nbil0_reg0_val32, ILKN_RX_N_RETRANS_FSM_STATE_4f),
           soc_reg_field_get(unit, ILKN_PML_ILKN_RX_RETRANSMIT_DEBUG_INFORMATIONr,
           nbil0_reg1_val32, ILKN_RX_N_RETRANS_FSM_STATE_4f),
           soc_reg_field_get(unit, ILKN_PML_ILKN_RX_RETRANSMIT_DEBUG_INFORMATIONr,
           nbil1_reg0_val32, ILKN_RX_N_RETRANS_FSM_STATE_4f),
           soc_reg_field_get(unit, ILKN_PML_ILKN_RX_RETRANSMIT_DEBUG_INFORMATIONr,
           nbil1_reg1_val32, ILKN_RX_N_RETRANS_FSM_STATE_4f));
    cli_out("\t3 \t\t\t\t%d\t\t\t%d\t\t\t%d\t\t\t%d\t\t\t%d\t\t\t%d\n",
           soc_reg_field_get(unit, ILKN_PMH_ILKN_RX_RETRANSMIT_DEBUG_INFORMATIONr,
           reg0_val32, ILKN_RX_N_RETRANS_FSM_STATE_3f),
           soc_reg_field_get(unit, ILKN_PMH_ILKN_RX_RETRANSMIT_DEBUG_INFORMATIONr,
           reg1_val32, ILKN_RX_N_RETRANS_FSM_STATE_3f),
           soc_reg_field_get(unit, ILKN_PML_ILKN_RX_RETRANSMIT_DEBUG_INFORMATIONr,
           nbil0_reg0_val32, ILKN_RX_N_RETRANS_FSM_STATE_3f),
           soc_reg_field_get(unit, ILKN_PML_ILKN_RX_RETRANSMIT_DEBUG_INFORMATIONr,
           nbil0_reg1_val32, ILKN_RX_N_RETRANS_FSM_STATE_3f),
           soc_reg_field_get(unit, ILKN_PML_ILKN_RX_RETRANSMIT_DEBUG_INFORMATIONr,
           nbil1_reg0_val32, ILKN_RX_N_RETRANS_FSM_STATE_3f),
           soc_reg_field_get(unit, ILKN_PML_ILKN_RX_RETRANSMIT_DEBUG_INFORMATIONr,
           nbil1_reg1_val32, ILKN_RX_N_RETRANS_FSM_STATE_3f));
    cli_out("\t2 \t\t\t\t%d\t\t\t%d\t\t\t%d\t\t\t%d\t\t\t%d\t\t\t%d\n",
           soc_reg_field_get(unit, ILKN_PMH_ILKN_RX_RETRANSMIT_DEBUG_INFORMATIONr,
           reg0_val32, ILKN_RX_N_RETRANS_FSM_STATE_2f),
           soc_reg_field_get(unit, ILKN_PMH_ILKN_RX_RETRANSMIT_DEBUG_INFORMATIONr,
           reg1_val32, ILKN_RX_N_RETRANS_FSM_STATE_2f),
           soc_reg_field_get(unit, ILKN_PML_ILKN_RX_RETRANSMIT_DEBUG_INFORMATIONr,
           nbil0_reg0_val32, ILKN_RX_N_RETRANS_FSM_STATE_2f),
           soc_reg_field_get(unit, ILKN_PML_ILKN_RX_RETRANSMIT_DEBUG_INFORMATIONr,
           nbil0_reg1_val32, ILKN_RX_N_RETRANS_FSM_STATE_2f),
           soc_reg_field_get(unit, ILKN_PML_ILKN_RX_RETRANSMIT_DEBUG_INFORMATIONr,
           nbil1_reg0_val32, ILKN_RX_N_RETRANS_FSM_STATE_2f),
           soc_reg_field_get(unit, ILKN_PML_ILKN_RX_RETRANSMIT_DEBUG_INFORMATIONr,
           nbil1_reg1_val32, ILKN_RX_N_RETRANS_FSM_STATE_2f));
    cli_out("\t1 \t\t\t\t%d\t\t\t%d\t\t\t%d\t\t\t%d\t\t\t%d\t\t\t%d\n",
           soc_reg_field_get(unit, ILKN_PMH_ILKN_RX_RETRANSMIT_DEBUG_INFORMATIONr,
           reg0_val32, ILKN_RX_N_RETRANS_FSM_STATE_1f),
           soc_reg_field_get(unit, ILKN_PMH_ILKN_RX_RETRANSMIT_DEBUG_INFORMATIONr,
           reg1_val32, ILKN_RX_N_RETRANS_FSM_STATE_1f),
           soc_reg_field_get(unit, ILKN_PML_ILKN_RX_RETRANSMIT_DEBUG_INFORMATIONr,
           nbil0_reg0_val32, ILKN_RX_N_RETRANS_FSM_STATE_1f),
           soc_reg_field_get(unit, ILKN_PML_ILKN_RX_RETRANSMIT_DEBUG_INFORMATIONr,
           nbil0_reg1_val32, ILKN_RX_N_RETRANS_FSM_STATE_1f),
           soc_reg_field_get(unit, ILKN_PML_ILKN_RX_RETRANSMIT_DEBUG_INFORMATIONr,
           nbil1_reg0_val32, ILKN_RX_N_RETRANS_FSM_STATE_1f),
           soc_reg_field_get(unit, ILKN_PML_ILKN_RX_RETRANSMIT_DEBUG_INFORMATIONr,
           nbil1_reg1_val32, ILKN_RX_N_RETRANS_FSM_STATE_1f));
    cli_out("\t0 \t\t\t\t%d\t\t\t%d\t\t\t%d\t\t\t%d\t\t\t%d\t\t\t%d\n",
           soc_reg_field_get(unit, ILKN_PMH_ILKN_RX_RETRANSMIT_DEBUG_INFORMATIONr,
           reg0_val32, ILKN_RX_N_RETRANS_FSM_STATE_0f),
           soc_reg_field_get(unit, ILKN_PMH_ILKN_RX_RETRANSMIT_DEBUG_INFORMATIONr,
           reg1_val32, ILKN_RX_N_RETRANS_FSM_STATE_9f),
           soc_reg_field_get(unit, ILKN_PML_ILKN_RX_RETRANSMIT_DEBUG_INFORMATIONr,
           nbil0_reg0_val32, ILKN_RX_N_RETRANS_FSM_STATE_9f),
           soc_reg_field_get(unit, ILKN_PML_ILKN_RX_RETRANSMIT_DEBUG_INFORMATIONr,
           nbil0_reg1_val32, ILKN_RX_N_RETRANS_FSM_STATE_9f),
           soc_reg_field_get(unit, ILKN_PML_ILKN_RX_RETRANSMIT_DEBUG_INFORMATIONr,
           nbil1_reg0_val32, ILKN_RX_N_RETRANS_FSM_STATE_9f),
           soc_reg_field_get(unit, ILKN_PML_ILKN_RX_RETRANSMIT_DEBUG_INFORMATIONr,
           nbil1_reg1_val32, ILKN_RX_N_RETRANS_FSM_STATE_9f));
    cli_out("FSM STATE SELECT\t\t\t%d\t\t\t%d\t\t\t%d\t\t\t%d\t\t\t%d\t\t\t%d\n",
           soc_reg_field_get(unit, ILKN_PMH_ILKN_RX_RETRANSMIT_DEBUG_INFORMATIONr,
           reg0_val32, ILKN_RX_N_RETRANS_FSM_STATEf),
           soc_reg_field_get(unit, ILKN_PMH_ILKN_RX_RETRANSMIT_DEBUG_INFORMATIONr,
           reg1_val32, ILKN_RX_N_RETRANS_FSM_STATEf),
           soc_reg_field_get(unit, ILKN_PML_ILKN_RX_RETRANSMIT_DEBUG_INFORMATIONr,
           nbil0_reg0_val32, ILKN_RX_N_RETRANS_FSM_STATEf),
           soc_reg_field_get(unit, ILKN_PML_ILKN_RX_RETRANSMIT_DEBUG_INFORMATIONr,
           nbil0_reg1_val32, ILKN_RX_N_RETRANS_FSM_STATEf),
           soc_reg_field_get(unit, ILKN_PML_ILKN_RX_RETRANSMIT_DEBUG_INFORMATIONr,
           nbil1_reg0_val32, ILKN_RX_N_RETRANS_FSM_STATEf),
           soc_reg_field_get(unit, ILKN_PML_ILKN_RX_RETRANSMIT_DEBUG_INFORMATIONr,
           nbil1_reg1_val32, ILKN_RX_N_RETRANS_FSM_STATEf));

    return rv;
}
#endif

STATIC cmd_result_t
cmd_dpp_retransmit(int unit, args_t *a) {
    int rv = CMD_OK;

    if (SOC_IS_DFE(unit)) {
        cli_out("Diag retransmit unavailable for FE1600 \n");
        return CMD_FAIL;
    }

    if (SOC_IS_QAX(unit)) {
        cli_out("Diag retransmit unavailable for QAX \n");
        return CMD_FAIL;
    }

    cli_out("Retransmit Status:\n");
    cli_out("------------------\n");

#ifdef BCM_JERICHO_SUPPORT
    if (SOC_IS_JERICHO(unit)) {
        rv = cmd_dpp_jer_retransmit(unit, a);
    } else
#endif
    {
        rv = cmd_dpp_arad_retransmit(unit, a);
    }

    return rv;
}

STATIC
void cmd_dpp_multicast_usage(void) {
   char dpp_multicast_usage[] =
      "Usage (DIAG multicast):"
      "DIAG [OPTION] <parameters> ..."
      "Option can be: ingress, egress \n"
      "Used without parameters returns all created Linked list groups of the specified type on the device. \n"
      "Parameters: \n"
      "\t <MCID start> <MCID end> - Specify range. Returns all createded Linked list groups of the specified type in the specified range of IDs.\n"
      "\t <MCID> <option> - Specify a single MCID. \n"
      "\t Options: \n"
      "\t\t members - returns information about the members of the specified MC group.\n"
      "\t\t mcdb - returns the MCDB entry indexes for the specified MC group. \n"
      "Examples: \n"
      "\t diag multicast egress - will show all egress groups that are currently opened \n"
      "\t diag multicast ingress 5678 mcdb - will show the MCDB entry indexes for ingress MC group 5678 \n"
      "\t diag multicast ingress 1234 5678 - will show all ingress groups that are currently opened in the specified range \n"
      "\n";
   cli_out(dpp_multicast_usage);
}

STATIC cmd_result_t
cmd_dpp_multicast_parse(int unit, args_t *a, int *is_egress, int *members, int *mcdb, int *mcid_min, int *mcid_max) {

    char *option;
    int strnlen_option;

    /* Fix for Coverity issue */
    if (is_egress == NULL || members == NULL || mcdb == NULL || mcid_min ==NULL || mcid_max ==NULL) {
        cli_out("ERROR: NULL pointer!");
        return CMD_FAIL;
    }

    /* Parse parameters */
    option = ARG_GET(a);
    if (option == NULL) {
        cmd_dpp_multicast_usage();
        return CMD_FAIL;
    }
    /*
     * Make sure at least one of the strings is NULL terminated.
     */
    strnlen_option = sal_strnlen(option, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH);
    if (!sal_strncasecmp(option, "ingress", strnlen_option)) {
        *is_egress=0;
    } else if (!sal_strncasecmp(option, "egress", strnlen_option)) {
        *is_egress=1;
    } else if (!sal_strcasecmp(option, "?")) {
        cmd_dpp_multicast_usage();
        return CMD_OK;
    } else {
        cli_out("Invalid option: %s \n", option);
        cmd_dpp_multicast_usage();
        return CMD_FAIL;
    }

    option = ARG_GET(a);
    if (option == NULL) {
        *mcid_min=0;
        if (*is_egress) {
            *mcid_max=SOC_DPP_CONFIG(unit)->tm.nof_mc_ids;
        } else {
            *mcid_max=SOC_DPP_CONFIG(unit)->tm.nof_ingr_mc_ids;
        }
    } else {
        *mcid_min=sal_ctoi(option,0);
        option = ARG_GET(a);
        if (option == NULL) {
            cmd_dpp_multicast_usage();
            return CMD_FAIL;
        }
        /*
         * Make sure at least one of the strings is NULL terminated.
         */
        strnlen_option = sal_strnlen(option, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH);
        if (!sal_strncasecmp(option, "members", strnlen_option)) {
            *mcid_max=*mcid_min;
            *members=1;
        } else if (!sal_strncasecmp(option, "mcdb", strnlen_option)) {
            *mcid_max=*mcid_min;
            *mcdb=1;
        } else {
            *mcid_max=sal_ctoi(option,0);
        }
    }

    return CMD_OK;
}

STATIC cmd_result_t
cmd_dpp_multicast(int unit, args_t *a) {

    char *ing_eg=NULL;
    char *gport_type;
    int rv, repl, index, rep_count;
    int mcid_min=0;
    int mcid_max=0;
    int is_egress=0;
    int bitmap_end=0;
    int bitmap_start=0;
    int max_group_size;
    int members=0;
    int mcdb=0;
    int group_count=0;
    int bitmap_count=0;
    int current_mcid=-1;
    uint32 flags=0;
    uint32 flags_ing;
    int replication_max=0;
    bcm_gport_t gport; 
    bcm_if_t encap_id, encap_id2;
    uint16 group_size_array[SOC_DPP_DEFS_MAX(NOF_CORES)];
    bcm_multicast_replication_t *rep_array=NULL;
    uint32 *index_array0=NULL;
    uint32 *index_array1=NULL;

    /* Parse parameters */
    rv=cmd_dpp_multicast_parse(unit, a, &is_egress, &members, &mcdb, &mcid_min, &mcid_max);                   
    if (BCM_FAILURE(rv)) {
        return CMD_FAIL;
    }

    if (is_egress) {
        flags_ing=0;
    } else {
        flags_ing=BCM_MULTICAST_INGRESS;
    }

    /*If a snigle MCID is given */
    if (mcid_min==mcid_max) {
        current_mcid=mcid_min;

        /*Returning information about the members */
        if (members) {
            /* Get flags */
            rv=bcm_multicast_group_get(unit, current_mcid, &flags);                   
            if (BCM_FAILURE(rv)) {
                return CMD_FAIL;
            }

            /* Check if group is created and if it's Egress List of Bitmap */
            if (is_egress && (flags & BCM_MULTICAST_EGRESS_GROUP)) {
                bitmap_end = SOC_DPP_CONFIG(unit)->tm.multicast_egress_bitmap_group_range.mc_id_high;
                bitmap_start = SOC_DPP_CONFIG(unit)->tm.multicast_egress_bitmap_group_range.mc_id_low;
                if (current_mcid>=bitmap_start && current_mcid<bitmap_end) {
                    ing_eg= "Egress-bmp";
                } else {
                    ing_eg= "Egress-list";
                }
            /* Check if group is created and if it's Ingress */
            } else if (!is_egress && (flags & BCM_MULTICAST_INGRESS_GROUP)){
                ing_eg= "Ingress";
            } else {
                cli_out("Multicast group ID: %d does not exist in the specified configuration!  \n", current_mcid);
            }

            /* Get the replications count */
            rv=bcm_multicast_get(unit, current_mcid, flags_ing, replication_max, rep_array, &rep_count);
            if (BCM_FAILURE(rv)) {
                return CMD_FAIL;
            }

            cli_out("Multicast ID: %d \n", current_mcid);
            cli_out("Ingress/Egress: %s \n", ing_eg);
            cli_out("Members count: %d \n", rep_count);

            /* If the group is not empty, allocate memory for the rep_array*/
            if (rep_count) {
                rep_array = (bcm_multicast_replication_t *)sal_alloc(sizeof(bcm_multicast_replication_t)*rep_count,"rep_array");
                if (rep_array == NULL) {
                    cli_out("ERROR: NULL pointer!");
                    return CMD_FAIL;
                }

                /*Fill rep_array with information for the members*/
                rv=bcm_multicast_get(unit, current_mcid, flags_ing, rep_count, rep_array, &rep_count);
                if (BCM_FAILURE(rv)) {
                    return CMD_FAIL;
                }

                cli_out(" --------------------------------------------------------------------------------- \n");
                cli_out("    Member   |  Gport Port  |  Gport Type  | 1st Encap ID/CUD | 2nd Encap ID/CUD |\n");
                cli_out(" --------------------------------------------------------------------------------- \n");
                for (repl=0; repl<rep_count; repl++) {
                    gport=rep_array[repl].port;
                    encap_id=rep_array[repl].encap1;
                    encap_id2=rep_array[repl].encap2;
                    /* Check the type of the Gport*/
                    if (SOC_GPORT_IS_LOCAL(gport)) {
                        gport_type = "Local_Port";
                    } else if (SOC_GPORT_IS_MODPORT(gport)) {
                        gport_type = "Modport";
                    } else if (SOC_GPORT_IS_TRUNK(gport)) {
                        gport_type = "Trunk/LAG";
                    } else if (_SHR_GPORT_IS_SYSTEM_PORT(gport)) {
                        gport_type = "System_Port";
                    } else if (SOC_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
                        gport_type = "UC_Queue";
                    } else if (_SHR_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
                        gport_type = "MC_Queue";
                    } else if (_SHR_GPORT_IS_MCAST(gport)) {
                        gport_type = "Multicast";
                    } else if (SOC_GPORT_IS_LOCAL_CPU(gport)) {
                        gport_type = "Local_CPU";
                    } else if (SOC_GPORT_IS_BLACK_HOLE(gport)) {
                        gport_type = "Black_Hole";
                    } else {
                        gport_type = "Unknown";
                    }
                    cli_out("%8d     |  0x%-10x|%13s |      0x%-10x|      0x%-10x| \n", repl, gport,gport_type, encap_id, encap_id2);
                }
                sal_free(rep_array);
            }
        }

        /*Returning the MCDB indexes*/
        if (mcdb) {
            /* Get flags */
            rv=bcm_multicast_group_get(unit, current_mcid, &flags);                   
            if (BCM_FAILURE(rv)) {
                return CMD_FAIL;
            }

            /* Check if group is created*/
            if(((flags & BCM_MULTICAST_INGRESS_GROUP)&&(!is_egress)) || ((flags & BCM_MULTICAST_EGRESS_GROUP)&&(is_egress))) {
                if (is_egress && (flags & BCM_MULTICAST_EGRESS_GROUP)) {
                    bitmap_start = SOC_DPP_CONFIG(unit)->tm.multicast_egress_bitmap_group_range.mc_id_low;
                    bitmap_end = SOC_DPP_CONFIG(unit)->tm.multicast_egress_bitmap_group_range.mc_id_high;
                    if (!(current_mcid<bitmap_start || current_mcid>bitmap_end)) {
                        cli_out("Multicast IDs from %d to %d are reserved for bitmap groups. \n",bitmap_start, bitmap_end);
                        return CMD_FAIL;
                    }
                }

                /* Set max_group_size depending on device */
                if (SOC_IS_QAX(unit)) {
                    max_group_size= is_egress ? QAX_MULT_MAX_EGRESS_REPLICATIONS : QAX_MULT_MAX_INGRESS_REPLICATIONS;
                } else {
                    max_group_size= is_egress ? DPP_MULT_MAX_EGRESS_REPLICATIONS : DPP_MULT_MAX_INGRESS_REPLICATIONS;
                }
                /* Allocate memory for the inxdex arrays*/
                index_array0 = (uint32 *)sal_alloc(sizeof(uint32)*max_group_size,"index_array0");
                index_array1 = (uint32 *)sal_alloc(sizeof(uint32)*max_group_size,"index_array1");
                sal_memset(index_array0, 0, sizeof(uint32)*max_group_size);
                sal_memset(index_array1, 0, sizeof(uint32)*max_group_size);

                /*Get the MCDB indexes */
                rv=dpp_mcdb_index_get(unit, current_mcid, is_egress, group_size_array, index_array0, index_array1);
                if (BCM_FAILURE(rv)) {
                    sal_free(index_array0);
                    sal_free(index_array1);
                    return CMD_FAIL;
                }
                cli_out("Core 0, Multicast ID: %d \n",current_mcid);
                cli_out(" --------------------- \n");
                cli_out(" Entry |  MCDB index | \n");
                cli_out(" --------------------- \n");
                for (repl=0; repl<group_size_array[0]; repl++) {
                    index=index_array0[repl];
                    if (index != 0) {
                        cli_out("%5d  |%10d   | \n", repl, index);
                    }
                }

                /* If device has 2 active cores also print array for the second core */
                if ((SOC_DPP_DEFS_MAX(NOF_CORES)>1) && is_egress) {
                    cli_out("\n");
                    cli_out("\n");
                    cli_out("Core 1, Multicast ID: %d \n",current_mcid);
                    cli_out(" --------------------- \n");
                    cli_out(" Entry |  MCDB index | \n");
                    cli_out(" --------------------- \n");
                    for (repl=0; repl<group_size_array[1]; repl++) {
                        index=index_array1[repl];
                        if (index != 0) {
                            cli_out("%5d  |%10d   | \n", repl, index);
                        }
                    }
                }
            } else {
                cli_out("Multicast group ID: %d does not exist in the specified configuration!  \n", current_mcid);
            }
            sal_free(index_array0);
            sal_free(index_array1);
        }
    }

    /*If no MCID is specified of if a range of MCIDs is given*/
    if (mcid_min != mcid_max) {

        if (is_egress) {
            cli_out("Showing created Linked List Egress Multicast groups:\n");
        } else {
            cli_out("Showing created Ingress Multicast groups:\n");
        }

        for (current_mcid=mcid_min; current_mcid<mcid_max; current_mcid++) {
            /* Get flags */
            rv=bcm_multicast_group_get(unit, current_mcid, &flags);                   
            if (BCM_FAILURE(rv)) {
                return CMD_FAIL;
            }

            /* Check if group is created*/
            if(((flags & BCM_MULTICAST_INGRESS_GROUP)&&(!is_egress)) || ((flags & BCM_MULTICAST_EGRESS_GROUP)&&(is_egress))) {
                if (is_egress && (flags & BCM_MULTICAST_EGRESS_GROUP)) {
                    bitmap_start=SOC_DPP_CONFIG(unit)->tm.multicast_egress_bitmap_group_range.mc_id_low;
                    bitmap_end=SOC_DPP_CONFIG(unit)->tm.multicast_egress_bitmap_group_range.mc_id_high;
                    if (!(current_mcid<bitmap_start || current_mcid>bitmap_end)) {
                        bitmap_count++;
                    } else {
                        cli_out("Multicast ID: %d  \n",current_mcid);
                        group_count++;
                    }
                }
                if ((!is_egress) && (flags & BCM_MULTICAST_INGRESS_GROUP)) {
                    cli_out("Multicast ID: %d  \n",current_mcid);
                    group_count++;
                }

            }
        }

        if (is_egress) {
            cli_out("Number of created Linked List Egress Multicast groups: %d \n",group_count);
            if (bitmap_count) {
                cli_out(" ------------------------------------------------------------------------------------ \n");
                cli_out("Multicast IDs from %d to %d are reserved for bitmap groups and are created by default.\n",bitmap_start, bitmap_end);
                cli_out("Number of created Bitmap Egress Multicast groups in the specified range: %d \n",bitmap_count);
            }
        } else {
            cli_out("Number of created Ingress Multicast groups: %d \n",group_count);
        }
    }
    return CMD_OK;
}

#endif /* BCM_PETRA_SUPPORT */

static char     *module_list[] = {
   "NONE", "NBI", "CFC", "EGQ", "SCH", "ALL"
};
static char     *interface_list[] = {
   "NONE", "XAUI", "RXAUI", "SPI", "ILKN_INband", "ILKN_OUTband", "ALL"
};
static char     *type_list[] = {
   "NONE", "LLFC", "PFC", "NIF", "ALL"
};

typedef enum {
   none_module_list = 0,
   nbi_module_list,
   cfc_module_list,
   eqg_module_list,
   sch_module_list,
   all_module_list,
   num_of_module_list
} module_list_e;

typedef enum {
   none_interface_list = 0,
   xaui_interface_list,
   rxaui_interface_list,
   spi_interface_list,
   ilkn_inband_interface_list,
   ilkn_outband_interface_list,
   all_interface_list,
   num_of_interface_list
} interface_list_e;

typedef enum {
   none_type_list = 0,
   llfc_type_list,
   pfc_type_list,
   nif_type_list,
   all_type_list,
   num_of_type_list

} type_list_e;

typedef struct diag_fc_s {
   module_list_e    module_mask;
   interface_list_e interface_mask;
   type_list_e      type_mask;
} diag_fc_t;



#define EGQ_MODULE_MASK ( (fc_str.module_mask == eqg_module_list) || (fc_str.module_mask == all_module_list))
#define NBI_MODULE_MASK ( (fc_str.module_mask == nbi_module_list) || (fc_str.module_mask == all_module_list))
#define SCH_MODULE_MASK ( (fc_str.module_mask == sch_module_list)  || (fc_str.module_mask == all_module_list))


#define  ILKN_INBAND_INTERFACE_MASK   ( (fc_str.interface_mask == ilkn_inband_interface_list) || (fc_str.interface_mask == all_interface_list))
#define  ILKN_OUTBAND_INTERFACE_MASK  ( (fc_str.interface_mask == ilkn_outband_interface_list) || (fc_str.interface_mask == all_interface_list))


#define  LLFC_TYPE_MASK  (( fc_str.type_mask == llfc_type_list) || (fc_str.type_mask == all_type_list))
#define  PFC_TYPE_MASK   (( fc_str.type_mask == pfc_type_list) || (fc_str.type_mask == all_type_list))
#define  NIF_TYPE_MASK   (( fc_str.type_mask == nif_type_list) || (fc_str.type_mask == all_type_list))

#define MAX_COUNTER_NAME_LEN 54
STATIC cmd_result_t
dpp_diag_fc(int unit, args_t *a) {

   parse_table_t       pt;
   diag_fc_t           fc_str;
   uint32              read_val;
   uint64              read_val64;
   soc_reg_above_64_val_t read_val_above64;
   char                val_str[20];
   char                dval_str[256];
   uint32              default_val = 0;
   uint8               i;
   int                 rc = SOC_E_NONE;
/*    int                 nprint; */
   /*  Init default values*/
   fc_str.interface_mask = 0;
   fc_str.module_mask = 0;
   fc_str.type_mask = 0;

   /* Read parameters */
   parse_table_init(unit, &pt);
   parse_table_add(&pt, "Module", PQ_DFL | PQ_MULTI, &default_val, &fc_str.module_mask, module_list);
   parse_table_add(&pt, "Interface", PQ_DFL | PQ_MULTI, &default_val, &fc_str.interface_mask, interface_list);
   parse_table_add(&pt, "Type", PQ_DFL | PQ_MULTI, &default_val, &fc_str.type_mask, type_list);


   if (parse_arg_eq(a, &pt) < 0) {
      parse_arg_eq_done(&pt);
      return CMD_USAGE;
   }
   parse_arg_eq_done(&pt);
   cli_out("module:%d interface:%d type:%d \n", fc_str.module_mask, fc_str.interface_mask, fc_str.type_mask);



   cli_out("NIF_TYPE_MASK %d EGQ_MODULE_MASK %d NBI_MODULE_MASK %d\n", NIF_TYPE_MASK, EGQ_MODULE_MASK, NBI_MODULE_MASK);
   rc = READ_CFC_INTERRUPT_REGISTERr(unit, &read_val);
   if (SOC_FAILURE(rc)) {
      cli_out("Failed to read CFC_INTERRUPT_REGISTER register\n");
      return CMD_FAIL;
   }
   cli_out("CFC Interrupt Register = 0x%x\n", read_val);


   /*  ----------- Scheduler -------------------- */
   if (SCH_MODULE_MASK) {
      rc =  READ_SCH_DVS_FC_COUNTERS_CONFIGURATION_REGISTERr(unit, &read_val);
      if (SOC_FAILURE(rc)) {
         cli_out("Failed to read SCH_DVS_FC_COUNTERS_CONFIGURATION_REGISTER register\n");
         return CMD_FAIL;
      }
      cli_out("SCH_DVS_FC_COUNTERS_CONFIGURATION_REGISTER Register = 0x%x\n", read_val);
      rc = READ_SCH_DVS_FLOW_CONTROL_COUNTERr(unit, &read_val);
      if (SOC_FAILURE(rc)) {
         cli_out("Failed to read SCH_DVS_FLOW_CONTROL_COUNTER register\n");
         return CMD_FAIL;
      }
      cli_out("SCH_DVS_FLOW_CONTROL_COUNTER Register = 0x%x\n", read_val);

   }
   /*  ----------- NBI -------------------- */
   if (NBI_MODULE_MASK || PFC_TYPE_MASK) {
      rc = READ_NBI_FC_PFC_DEBUG_INDICATIONSr(unit, &read_val);
      if (SOC_FAILURE(rc)) {
         cli_out("Failed to read NBI_FC_PFC_DEBUG_INDICATIONS register\n");
         return CMD_FAIL;
      }
      cli_out("NBI_FC_PFC_DEBUG_INDICATIONS Register = 0x%x\n", read_val);
   }
   if (NBI_MODULE_MASK || LLFC_TYPE_MASK) {
      rc = READ_NBI_FC_RX_GEN_LLFC_FROM_MLFr(unit, &read_val);
      if (SOC_FAILURE(rc)) {
         cli_out("Failed to read NBI_FC_RX_GEN_LLFC_FROM_MLF register\n");
         return CMD_FAIL;
      }
      cli_out("NBI_FC_RX_GEN_LLFC_FROM_MLF Register = 0x%x\n", read_val);
   }
   if (NBI_MODULE_MASK || PFC_TYPE_MASK) {
      rc = READ_NBI_FC_RX_GEN_PFC_FROM_MLFr(unit, &read_val);
      if (SOC_FAILURE(rc)) {
         cli_out("Failed to read NBI_FC_RX_GEN_PFC_FROM_MLF register\n");
         return CMD_FAIL;
      }
      cli_out("NBI_FC_RX_GEN_PFC_FROM_MLF Register = 0x%x\n", read_val);
   }
   if (NBI_MODULE_MASK || ILKN_INBAND_INTERFACE_MASK) {

      rc = READ_NBI_FC_ILKN_RX_0_CHFC_FROM_PORT_RAWr(unit, &read_val64);
      if (SOC_FAILURE(rc)) {
         cli_out("Failed to read NBI_FC_ILKN_RX_0_CHFC_FROM_PORT_RAW register\n");
         return CMD_FAIL;
      }
      format_uint64(val_str, read_val64);
      cli_out("NBI_FC_ILKN_RX_0_CHFC_FROM_PORT_RAW Register = 0x%s\n", val_str);

      rc = READ_NBI_FC_ILKN_RX_0_CHFC_FROM_PORT_ROCr(unit, &read_val64);
      if (SOC_FAILURE(rc)) {
         cli_out("Failed to read NBI_FC_ILKN_RX_0_CHFC_FROM_PORT_ROC register\n");
         return CMD_FAIL;
      }
      format_uint64(val_str, read_val64);
      cli_out("NBI_FC_ILKN_RX_0_CHFC_FROM_PORT_ROC Register = 0x%s\n", val_str);

      rc = READ_NBI_FC_ILKN_RX_1_CHFC_FROM_PORT_RAWr(unit, &read_val64);
      if (SOC_FAILURE(rc)) {
         cli_out("Failed to read NBI_FC_ILKN_RX_1_CHFC_FROM_PORT_RAW register\n");
         return CMD_FAIL;
      }
      format_uint64(val_str, read_val64);
      cli_out("NBI_FC_ILKN_RX_1_CHFC_FROM_PORT_RAW Register = 0x%s\n", val_str);

      rc = READ_NBI_FC_ILKN_RX_1_CHFC_FROM_PORT_ROCr(unit, &read_val64);
      if (SOC_FAILURE(rc)) {
         cli_out("Failed to read NBI_FC_ILKN_RX_1_CHFC_FROM_PORT_ROC register\n");
         return CMD_FAIL;
      }
      format_uint64(val_str, read_val64);
      cli_out("NBI_FC_ILKN_RX_1_CHFC_FROM_PORT_ROC Register = 0x%s\n", val_str);

   }
   if (NBI_MODULE_MASK || ILKN_INBAND_INTERFACE_MASK || LLFC_TYPE_MASK) {
      rc = READ_NBI_FC_ILKN_RX_0_LLFC_FROM_RX_CNTr(unit, &read_val);
      if (SOC_FAILURE(rc)) {
         cli_out("Failed to read NBI_FC_ILKN_RX_0_LLFC_FROM_RX_CNT register\n");
         return CMD_FAIL;
      }
      cli_out("NBI_FC_ILKN_RX_0_LLFC_FROM_RX_CNT Register = 0x%x\n", read_val);

      rc = READ_NBI_FC_ILKN_RX_1_LLFC_FROM_RX_CNTr(unit, &read_val);
      if (SOC_FAILURE(rc)) {
         cli_out("Failed to read NBI_FC_ILKN_RX_1_LLFC_FROM_RX_CNT register\n");
         return CMD_FAIL;
      }
      cli_out("NBI_FC_ILKN_RX_1_LLFC_FROM_RX_CNT Register = 0x%x\n", read_val);
   }
   if (ILKN_INBAND_INTERFACE_MASK) {
      rc = READ_CFC_ILKN_0_OOB_RX_LANES_STATUSr(unit, &read_val);
      if (SOC_FAILURE(rc)) {
         cli_out("Failed to read CFC_ILKN_0_OOB_RX_LANES_STATUS register\n");
         return CMD_FAIL;
      }
      cli_out("CFC_ILKN_0_OOB_RX_LANES_STATUS Register = 0x%x\n", read_val);

      rc = READ_CFC_ILKN_1_OOB_RX_LANES_STATUSr(unit, &read_val);
      if (SOC_FAILURE(rc)) {
         cli_out("Failed to read CFC_ILKN_1_OOB_RX_LANES_STATUS register\n");
         return CMD_FAIL;
      }
      cli_out("CFC_ILKN_1_OOB_RX_LANES_STATUSRegister = 0x%x\n", read_val);
   }


   if (ILKN_INBAND_INTERFACE_MASK) {
      rc = READ_CFC_ILKN_RX_0_FC_STATUSr(unit, read_val_above64);
      if (SOC_FAILURE(rc)) {
         cli_out("Failed to read CFC_ILKN_RX_0_FC_STATUS register\n");
         return CMD_FAIL;
      }
      format_long_integer(dval_str, read_val_above64, SOC_REG_ABOVE_64_INFO(unit, EGQ_CFC_FLOW_CONTROLr).size);
      cli_out("CFC_ILKN_RX_0_FC_STATUS Register Register = 0x%s\n", dval_str);

      rc = READ_CFC_ILKN_RX_1_FC_STATUSr(unit, read_val_above64);
      if (SOC_FAILURE(rc)) {
         cli_out("Failed to read CFC_ILKN_RX_1_FC_STATUS register\n");
         return CMD_FAIL;
      }
      format_long_integer(dval_str, read_val_above64, SOC_REG_ABOVE_64_INFO(unit, EGQ_CFC_FLOW_CONTROLr).size);
      cli_out("CFC_ILKN_RX_1_FC_STATUS Register Register = 0x%s\n", dval_str);

   }


   if (NBI_MODULE_MASK || LLFC_TYPE_MASK || ILKN_INBAND_INTERFACE_MASK) {
      rc = READ_NBI_FC_RX_MUBITS_TO_CFCr(unit, &read_val);
      if (SOC_FAILURE(rc)) {
         cli_out("Failed to read NBI_FC_RX_MUBITS_TO_CFC register\n");
         return CMD_FAIL;
      }
      cli_out("NBI_FC_RX_MUBITS_TO_CFC Register = 0x%x\n", read_val);
   }

   if (NBI_MODULE_MASK || LLFC_TYPE_MASK || ILKN_INBAND_INTERFACE_MASK) {
      rc = READ_NBI_FC_RX_LLFC_STOP_TX_FROM_MUBITSr(unit, &read_val);
      if (SOC_FAILURE(rc)) {
         cli_out("Failed to read NBI_FC_RX_LLFC_STOP_TX_FROM_MUBITS register\n");
         return CMD_FAIL;
      }
      cli_out("NBI_FC_RX_LLFC_STOP_TX_FROM_MUBITS Register = 0x%x\n", read_val);
   }
   /* NBI TX*/
   if (NBI_MODULE_MASK || LLFC_TYPE_MASK) {
      rc = READ_NBI_FC_TX_GEN_LLFC_FROM_CFCr(unit, &read_val);
      if (SOC_FAILURE(rc)) {
         cli_out("Failed to read NBI_FC_TX_GEN_LLFC_FROM_CFC register\n");
         return CMD_FAIL;
      }
      cli_out("NBI_FC_TX_GEN_LLFC_FROM_CFC Register = 0x%x\n", read_val);
   }

   if (NBI_MODULE_MASK || LLFC_TYPE_MASK) {
      rc = READ_NBI_FC_TX_GEN_LLFC_TO_XMALr(unit, &read_val);
      if (SOC_FAILURE(rc)) {
         cli_out("Failed to read NBI_FC_TX_GEN_LLFC_TO_XMAL register\n");
         return CMD_FAIL;
      }
      cli_out("NBI_FC_TX_GEN_LLFC_TO_XMAL Register = 0x%x\n", read_val);
   }

   if (NBI_MODULE_MASK || LLFC_TYPE_MASK) {
      rc = READ_NBI_FC_TX_LLFC_STOP_TX_FROM_CFCr(unit, &read_val);
      if (SOC_FAILURE(rc)) {
         cli_out("Failed to read NBI_FC_TX_LLFC_STOP_TX_FROM_CFC register\n");
         return CMD_FAIL;
      }
      cli_out("NBI_FC_TX_LLFC_STOP_TX_FROM_CFC Register = 0x%x\n", read_val);
   }


   if (NBI_MODULE_MASK || ILKN_OUTBAND_INTERFACE_MASK) {
      rc = READ_NBI_FC_ILKN_TX_0_GEN_CHFC_ROCr(unit, &read_val64);
      if (SOC_FAILURE(rc)) {
         cli_out("Failed to read NBI_FC_ILKN_TX_0_GEN_CHFC_ROC register\n");
         return CMD_FAIL;
      }
      format_uint64(val_str, read_val64);
      cli_out("NBI_FC_ILKN_TX_0_GEN_CHFC_ROCRegister = 0x%s\n", val_str);

      rc = READ_NBI_FC_ILKN_TX_1_GEN_CHFC_ROCr(unit, &read_val64);
      if (SOC_FAILURE(rc)) {
         cli_out("Failed to read NBI_FC_ILKN_TX_1_GEN_CHFC_ROC register\n");
         return CMD_FAIL;
      }
      format_uint64(val_str, read_val64);
      cli_out("NBI_FC_ILKN_TX_1_GEN_CHFC_ROCRegister = 0x%s\n", val_str);

   }
   if (NBI_MODULE_MASK || ILKN_OUTBAND_INTERFACE_MASK) {
      rc = READ_NBI_FC_TX_MUBITS_FROM_CFCr(unit, &read_val);
      if (SOC_FAILURE(rc)) {
         cli_out("Failed to read NBI_FC_TX_MUBITS_FROM_CFC register\n");
         return CMD_FAIL;
      }
      cli_out("NBI_FC_TX_MUBITS_FROM_CFC Register = 0x%x\n", read_val);
   }

   if (NBI_MODULE_MASK || ILKN_OUTBAND_INTERFACE_MASK || LLFC_TYPE_MASK) {
      rc = READ_NBI_FC_ILKN_TX_0_LLFC_STOP_TX_CNTr(unit, &read_val);
      if (SOC_FAILURE(rc)) {
         cli_out("Failed to read NBI_FC_ILKN_TX_0_LLFC_STOP_TX_CNT register\n");
         return CMD_FAIL;
      }
      cli_out("NBI_FC_ILKN_TX_0_LLFC_STOP_TX_CNT Register = 0x%x\n", read_val);
      rc = READ_NBI_FC_ILKN_TX_1_LLFC_STOP_TX_CNTr(unit, &read_val);
      if (SOC_FAILURE(rc)) {
         cli_out("Failed to read NBI_FC_ILKN_TX_1_LLFC_STOP_TX_CNT register\n");
         return CMD_FAIL;
      }
      cli_out("NBI_FC_ILKN_TX_1_LLFC_STOP_TX_CNT Register = 0x%x\n", read_val);

   }

   if (NBI_MODULE_MASK || ILKN_OUTBAND_INTERFACE_MASK || LLFC_TYPE_MASK) {
      rc = READ_NBI_FC_ILKN_TX_0_GEN_LLFC_CNTr(unit, &read_val);
      if (SOC_FAILURE(rc)) {
         cli_out("Failed to read NBI_FC_ILKN_TX_0_GEN_LLFC_CNT register\n");
         return CMD_FAIL;
      }
      cli_out("NBI_FC_ILKN_TX_0_GEN_LLFC_CNT Register = 0x%x\n", read_val);
      rc = READ_NBI_FC_ILKN_TX_1_GEN_LLFC_CNTr(unit, &read_val);
      if (SOC_FAILURE(rc)) {
         cli_out("Failed to read NBI_FC_ILKN_TX_1_GEN_LLFC_CNT register\n");
         return CMD_FAIL;
      }
      cli_out("NBI_FC_ILKN_TX_1_GEN_LLFC_CNT Register = 0x%x\n", read_val);

   }


/* NBI_FC_LLFC_STOP_TX_FROM_CFC_MASK 
   */

   /*  ----------- NIF -------------------- */
   if (NIF_TYPE_MASK || EGQ_MODULE_MASK) {
       rc = READ_EGQ_NIF_FLOW_CONTROLr(unit, REG_PORT_ANY, read_val_above64);
       if (SOC_FAILURE(rc)) {
           cli_out("Failed to read EGQ_NIF_FLOW_CONTROL register\n");
           return CMD_FAIL;
       }
       format_long_integer(dval_str, read_val_above64, SOC_REG_ABOVE_64_MAX_SIZE_U32);
       cli_out("NIF_FLOW_CONTROL Register = 0x%s\n", dval_str);
   }

   if (NIF_TYPE_MASK) {
       rc = READ_CFC_NIF_AF_FC_STATUSr(unit, read_val_above64);
       if (SOC_FAILURE(rc)) {
           cli_out("Failed to read CFC_NIF_AF_FC_STATUS register\n");
           return CMD_FAIL;
       }
       format_long_integer(dval_str, read_val_above64, SOC_REG_ABOVE_64_MAX_SIZE_U32);
       cli_out("NIF_AF_FC_STATUS Register = 0x%s\n", dval_str);
   }

   if (NIF_TYPE_MASK || PFC_TYPE_MASK) {
      for (i = 0; i < 4; i++) {
         rc = WRITE_CFC_NIF_PFC_STATUS_SELr(unit, i);
         if (SOC_FAILURE(rc)) {
            cli_out("Failed to write CFC_NIF_PFC_STATUS_SEL register\n");
            return CMD_FAIL;
         }
         rc = READ_CFC_NIF_PFC_STATUS_SELr(unit, &read_val);
         if (SOC_FAILURE(rc)) {
            cli_out("Failed to read CFC_NIF_PFC_STATUS_SEL register\n");
            return CMD_FAIL;
         }
         rc = READ_CFC_NIF_PFC_STATUSr(unit, &read_val64);
         if (SOC_FAILURE(rc)) {
            cli_out("Failed to read CFC_NIF_PFC_STATUS register\n");
            return CMD_FAIL;
         }
         format_uint64(val_str, read_val64);
         cli_out("CFC_NIF_PFC Select = 0x%x [Bits: %3d - %3d]   Status = 0x%s\n", read_val, (uint16)64 * i, (uint16)(64 * (i + 1) - 1), val_str);
      }
   }

   if (NIF_TYPE_MASK) {
      rc = READ_CFC_NIF_MUB_STATUSr(unit, &read_val64);
      if (SOC_FAILURE(rc)) {
         cli_out("Failed to read CFC_NIF_MUB_STATUS register\n");
         return CMD_FAIL;
      }
      format_uint64(val_str, read_val64);
      cli_out("CFC_NIF_MUB_STATUS Register = 0x%s\n", val_str);
   }

   if (NIF_TYPE_MASK) {
      rc = READ_CFC_NIF_RT_STATUSr(unit, &read_val);
      if (SOC_FAILURE(rc)) {
         cli_out("Failed to read CFC_NIF_RT_STATUS register\n");
         return CMD_FAIL;
      }
      cli_out("CFC_NIF_RT_STATUS Register = 0x%x\n", read_val);
   }

/*  ----------- EGQ -------------------- */
   if (EGQ_MODULE_MASK) {
      rc = READ_EGQ_CFC_FLOW_CONTROLr(unit, REG_PORT_ANY, read_val_above64);
      if (SOC_FAILURE(rc)) {
         cli_out("Failed to read EGQ_CFC_FLOW_CONTROL register\n");
         return CMD_FAIL;
      }
      format_long_integer(dval_str, read_val_above64, SOC_REG_ABOVE_64_INFO(unit, EGQ_CFC_FLOW_CONTROLr).size);
      cli_out("EGQ_CFC_FLOW_CONTROL Register = 0x%s\n", dval_str);
   }

   if (EGQ_MODULE_MASK) {
      rc = READ_CFC_EGQ_FC_STATUSr(unit, &read_val);
      if (SOC_FAILURE(rc)) {
         cli_out("Failed to read CFC_EGQ_FC_STATUS register\n");
         return CMD_FAIL;
      }
      cli_out("CFC_EGQ_FC_STATUS Register = 0x%x\n", read_val);
   }
   if (EGQ_MODULE_MASK) {
       rc = READ_CFC_EGQ_IF_FC_STATUSr(unit, read_val_above64);
       if (SOC_FAILURE(rc)) {
           cli_out("Failed to read CFC_EGQ_IF_FC_STATUS register\n");
           return CMD_FAIL;
       }
       format_long_integer(dval_str, read_val_above64,SOC_REG_ABOVE_64_MAX_SIZE_U32);
       cli_out("EGQ_IF_FC_STATUS Register = 0x%s\n", dval_str);
   }

   if (EGQ_MODULE_MASK || LLFC_TYPE_MASK) {
       
       rc = READ_CFC_EGQ_CNM_LLFC_STATUSr(unit, read_val_above64);
       if (SOC_FAILURE(rc)) {
           cli_out("Failed to read CFC_EGQ_CNM_LLFC_STATUS register\n");
           return CMD_FAIL;
       }
       format_long_integer(dval_str, read_val_above64, SOC_REG_ABOVE_64_MAX_SIZE_U32);
       cli_out("CFC_EGQ_CNM_LLFC_STATUS Register = 0x%s\n", dval_str);
   }

   if (EGQ_MODULE_MASK || PFC_TYPE_MASK) {
      for (i = 0; i < 4; i++) {
         uint32 sel_val = 0;
         soc_reg_field_set(unit, CFC_EGQ_STATUS_SELr, &sel_val, EGQ_PFC_STATUS_SELf, i);
         rc = WRITE_CFC_EGQ_STATUS_SELr(unit, sel_val);
         if (SOC_FAILURE(rc)) {
            cli_out("Failed to write CFC_EGQ_STATUS_SEL register\n");
            return CMD_FAIL;
         }
         rc = READ_CFC_EGQ_STATUS_SELr(unit, &read_val);
         if (SOC_FAILURE(rc)) {
            cli_out("Failed to read CFC_EGQ_STATUS_SEL register\n");
            return CMD_FAIL;
         }
         rc = READ_CFC_EGQ_PFC_STATUSr(unit, &read_val64);
         if (SOC_FAILURE(rc)) {
            cli_out("Failed to read CFC_EGQ_PFC_STATUS register\n");
            return CMD_FAIL;
         }
         format_uint64(val_str, read_val64);
         cli_out("CFC_EGQ_PFC Select = 0x%x [Bits: %3d - %3d]   Status = 0x%s\n", read_val, (uint16)64 * i, (uint16)(64 * (i + 1) - 1), val_str);
      }
   }

   if (EGQ_MODULE_MASK || PFC_TYPE_MASK) {
      for (i = 0; i < 4; i++) {
         uint32 sel_val = 0;
         soc_reg_field_set(unit, CFC_EGQ_STATUS_SELr, &sel_val, EGQ_CNM_PFC_STATUS_SELf, i);
         rc = WRITE_CFC_EGQ_STATUS_SELr(unit, sel_val);
         if (SOC_FAILURE(rc)) {
            cli_out("Failed to write CFC_EGQ_STATUS_SEL register\n");
            return CMD_FAIL;
         }
         rc = READ_CFC_EGQ_STATUS_SELr(unit, &read_val);
         if (SOC_FAILURE(rc)) {
            cli_out("Failed to read CFC_EGQ_STATUS_SEL register\n");
            return CMD_FAIL;
         }
         rc = READ_CFC_EGQ_CNM_PFC_STATUSr(unit, &read_val64);
         if (SOC_FAILURE(rc)) {
            cli_out("Failed to read CFC_EGQ_CNM_PFC_STATUS register\n");
            return CMD_FAIL;
         }
         format_uint64(val_str, read_val64);
         cli_out("CFC_EGQ_CNM_PFC Select = 0x%x [Bits: %3d - %3d]   Status = 0x%s\n", read_val, (uint16)64 * i, (uint16)(64 * (i + 1) - 1), val_str);
      }
   }

   return CMD_OK;
}


STATIC uint32 counter_value_higher_than_64(soc_reg_above_64_val_t reg_val) {

   int i;

   for (i = 2; i < SOC_REG_ABOVE_64_MAX_SIZE_U32; i++) {
      if (reg_val[i] != 0) {
         /* Return 1 if value is higher than 64 bit */
         return 1;
      }
   }
   return 0;
}

#ifdef BCM_PETRA_SUPPORT
int dpp_diag_gtimer_blocks_start(int unit, char **block_names, int num_of_block_names, int interval) {
   int i;
   int rv = 0;

   /* Enable and Trigger gtimer */
   for (i = 0; i < num_of_block_names; i++) {
      if ((rv = gtimer_enable(unit, block_names[i], interval)) != CMD_OK) {
         cli_out("Failed(%d) enable gtimer in block(%s)\n", rv, block_names[i]);
         break;
      }
      
      if ((rv = gtimer_trigger(unit, block_names[i])) != CMD_OK) {
         cli_out("Failed(%d) trigger gtimer in block(%s)\n", rv, block_names[i]);
         break;
      }
   }
   
   if (rv != CMD_OK){
      for (i = 0; i < num_of_block_names; i++) {
         if ((rv = gtimer_stop(unit, block_names[i])) != CMD_OK) {
            cli_out("Failed(%d) stop gtimer in block(%s)\n", rv, block_names[i]);
            continue;
         }
      }
      
      return CMD_FAIL;
   }
   
   sal_usleep(interval);    /* Wait interval uSec */
   
   return CMD_OK;
}

int dpp_diag_gtimer_blocks_stop(int unit, char **block_names, int num_of_block_names) {
   int i;
   int rv = CMD_OK;

   /* Stop gtimer */
   for (i = 0; i < num_of_block_names; i++) {
      if ((rv = gtimer_stop(unit, block_names[i])) != CMD_OK) {
         cli_out("Failed(%d) stop gtimer in block(%s)\n", rv, block_names[i]);
         continue;
      }
   }
   
   return rv;
}

void
print_counter_voq_usage(void) {
   char cmd_dpp_diag_counter_voq_usage[] =
      "Usage (DIAG counter voq):"
      "\n\tDIAGnotsics counter voq commands\n\t"
      "Usages:\n\t"
      "DIAG counter voq <parameters> ..."
      "OPTION can be:"
      "\nvoq - \tdisplay VOQ programmable counters for single voq."
      "\n\t Parameters required:"
      "\n\t\t queue - queue id"
      "\n\t\t interval - rate diagnostic"
      
      "\nvoq - \tdisplay VOQ programmable counters for voq group."
      "\n\t Parameters required:"
      "\n\t\t basequeue - basequeue id"
      "\n\t\t interval - rate diagnostic"
      
      "\n";

   cli_out(cmd_dpp_diag_counter_voq_usage);
}

void
print_counter_vsq_usage(void) {
   char cmd_dpp_diag_counter_vsq_usage[] =
      "Usage (DIAG counter vsq):"
      "\n\tDIAGnotsics counter vsq commands\n\t"
      "Usages:\n\t"
      "DIAG counter vsq <parameters> ..."
      "OPTION can be:"
      "\nvsq - \tdisplay VSQ programmable counters for single vsq."
      "\n\t Parameters required:"
      "\n\t\t queue - queue id"
      "\n\t\t interval - rate diagnostic"
      "\n";

   cli_out(cmd_dpp_diag_counter_vsq_usage);
}


STATIC int
dpp_diag_counter_voq_print(
    int                        unit,
    uint32                     voq_id,
    uint32                     num_cos
  )
{
   int rv = CMD_OK;
   uint32 val32;
   uint32 voq_enq_packet = 0, voq_enq_packet_ovf = 0;
   uint32 voq_deq_packet = 0, voq_deq_packet_ovf = 0;
   uint32 voq_tot_discarded_packet = 0, voq_tot_discarded_packet_ovf = 0;   
   uint32 voq_deleted_packet = 0, voq_deleted_packet_ovf = 0;      
   uint32 voq_max_oc0_qsize = 0, voq_max_oc0_refresh = 0;
   uint32 sram_enq_packet = 0;
   uint32 sram_rej_packet = 0;
   uint32 sram_to_dram_deq_packet = 0;
   uint32 sram_to_fabric_deq_packet = 0;
   uint32 sram_deleted_packet = 0;
   uint32 dram_deq_packet = 0;
   uint32 dram_deleted_packet = 0;
   uint32 core = 0;
   uint64 val64;
   COMPILER_64_ZERO(val64);

   cli_out("voq[%-3u] num_cosq[%-2u]\n\r",
          voq_id,
          num_cos
          );

  if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {

      if ((rv = soc_reg32_get(unit, IQM_QUEUE_MAXIMUM_OCCUPANCY_QUEUE_SIZE_0r, REG_PORT_ANY, 0, &val32)) != SOC_E_NONE) {
         return CMD_FAIL;
      }
      voq_max_oc0_qsize = soc_reg_field_get(unit, IQM_QUEUE_MAXIMUM_OCCUPANCY_QUEUE_SIZE_0r, val32, Q_MX_OC_QSZf);
      voq_max_oc0_refresh = soc_reg_field_get(unit, IQM_QUEUE_MAXIMUM_OCCUPANCY_QUEUE_SIZE_0r, val32, Q_MX_OC_RFRSHf);

      if ((rv = soc_reg32_get(unit, IQM_QUEUE_ENQUEUE_PACKET_COUNTERr, REG_PORT_ANY, 0, &val32)) != SOC_E_NONE) {
         return CMD_FAIL;
      }
      voq_enq_packet = soc_reg_field_get(unit, IQM_QUEUE_ENQUEUE_PACKET_COUNTERr, val32, Q_ENQ_PKT_CNTf);
      voq_enq_packet_ovf = soc_reg_field_get(unit, IQM_QUEUE_ENQUEUE_PACKET_COUNTERr, val32, Q_ENQ_PKT_CNT_OVFf);

      if ((rv = soc_reg32_get(unit, IQM_QUEUE_DEQUEUE_PACKET_COUNTERr, REG_PORT_ANY, 0, &val32)) != SOC_E_NONE) {
         return CMD_FAIL;
      }
      voq_deq_packet = soc_reg_field_get(unit, IQM_QUEUE_DEQUEUE_PACKET_COUNTERr, val32, Q_DEQ_PKT_CNTf);
      voq_deq_packet_ovf = soc_reg_field_get(unit, IQM_QUEUE_DEQUEUE_PACKET_COUNTERr, val32, Q_DEQ_PKT_CNT_OVFf);

      if ((rv = soc_reg32_get(unit, IQM_QUEUE_TOTAL_DISCARDED_PACKET_COUNTERr, REG_PORT_ANY, 0, &val32)) != SOC_E_NONE) {
         return CMD_FAIL;
      }
      voq_tot_discarded_packet = soc_reg_field_get(unit, IQM_QUEUE_TOTAL_DISCARDED_PACKET_COUNTERr, val32, Q_TOT_DSCRD_PKT_CNTf);
      voq_tot_discarded_packet_ovf = soc_reg_field_get(unit, IQM_QUEUE_TOTAL_DISCARDED_PACKET_COUNTERr, val32, Q_TOT_DSCRD_PKT_CNT_OVFf);

      if ((rv = soc_reg32_get(unit, IQM_QUEUE_DELETED_PACKET_COUNTERr, REG_PORT_ANY, 0, &val32)) != SOC_E_NONE) {
         return CMD_FAIL;
      }
      voq_deleted_packet = soc_reg_field_get(unit, IQM_QUEUE_DELETED_PACKET_COUNTERr, val32, Q_DEQ_DELETE_PKT_CNTf);
      voq_deleted_packet_ovf = soc_reg_field_get(unit, IQM_QUEUE_DELETED_PACKET_COUNTERr, val32, Q_DEQ_DELETE_PKT_CNT_OVFf);

      cli_out("\tvoq max occupancy0 level: %d, refresh: %s\n\r", voq_max_oc0_qsize,(voq_max_oc0_refresh?"true":"false"));
      cli_out("\tvoq enqueue packet: %d[%s]\n\r", voq_enq_packet, (voq_enq_packet_ovf?"ovf":""));
      cli_out("\tvoq dequeue packet: %d[%s]\n\r", voq_deq_packet, (voq_deq_packet_ovf?"ovf":""));
      cli_out("\tvoq total discarded packet: %d[%s]\n\r", voq_tot_discarded_packet, (voq_tot_discarded_packet_ovf?"ovf":""));
      cli_out("\tvoq deleted packet: %d[%s]\n\r", voq_deleted_packet, (voq_deleted_packet_ovf?"ovf":""));

  } else if (SOC_IS_JERICHO_PLUS_AND_BELOW(unit)) {

      for ( core = 0; core < 2; core++) {
          if ((rv = soc_reg_get(unit, IQM_QUEUE_MAXIMUM_OCCUPANCY_QUEUE_SIZEr, core, 0, &val64)) != SOC_E_NONE) {
             return CMD_FAIL;
          }
          voq_max_oc0_qsize = soc_reg64_field32_get(unit, IQM_QUEUE_MAXIMUM_OCCUPANCY_QUEUE_SIZEr, val64, Q_MX_OC_QSZf);
          voq_max_oc0_refresh = soc_reg64_field32_get(unit, IQM_QUEUE_MAXIMUM_OCCUPANCY_QUEUE_SIZEr, val64, Q_MX_OC_RFRSHf);

          if ((rv = soc_reg32_get(unit, IQM_QUEUE_ENQUEUE_PACKET_COUNTERr, core, 0, &val32)) != SOC_E_NONE) {
             return CMD_FAIL;
          }
          voq_enq_packet = soc_reg_field_get(unit, IQM_QUEUE_ENQUEUE_PACKET_COUNTERr, val32, QUEUE_ENQUEUE_PACKET_COUNTERf);

          if ((rv = soc_reg32_get(unit, IQM_QUEUE_DEQUEUE_PACKET_COUNTERr, core, 0, &val32)) != SOC_E_NONE) {
             return CMD_FAIL;
          }
          voq_deq_packet = soc_reg_field_get(unit, IQM_QUEUE_DEQUEUE_PACKET_COUNTERr, val32, QUEUE_DEQUEUE_PACKET_COUNTERf);

          if ((rv = soc_reg32_get(unit, IQM_QUEUE_ENQ_DISCARDED_PACKET_COUNTERr, core, 0, &val32)) != SOC_E_NONE) {
             return CMD_FAIL;
          }
          voq_tot_discarded_packet = soc_reg_field_get(unit, IQM_QUEUE_ENQ_DISCARDED_PACKET_COUNTERr, val32, QUEUE_ENQ_DISCARDED_PACKET_COUNTERf);

          if ((rv = soc_reg32_get(unit, IQM_QUEUE_DELETED_PACKET_COUNTERr, core, 0, &val32)) != SOC_E_NONE) {
             return CMD_FAIL;
          }
          voq_deleted_packet = soc_reg_field_get(unit, IQM_QUEUE_DELETED_PACKET_COUNTERr, val32, QUEUE_DELETED_PACKET_COUNTERf);

         cli_out("core: %d\n\r", core);
         cli_out("\tvoq max occupancy0 level: %d, refresh: %s\n\r", voq_max_oc0_qsize,(voq_max_oc0_refresh?"true":"false"));
         cli_out("\tvoq enqueue packet: %d\n\r", voq_enq_packet);
         cli_out("\tvoq dequeue packet: %d\n\r", voq_deq_packet);
         cli_out("\tvoq total discarded packet: %d\n\r", voq_tot_discarded_packet);
         cli_out("\tvoq deleted packet: %d\n\n\r", voq_deleted_packet);
      }
  } else if (SOC_IS_QAX(unit)) {

      /* SRAM Enqueued packets */
      if ((rv = soc_reg32_get(unit, CGM_VOQ_SRAM_ENQ_PKT_CTRr, REG_PORT_ANY, 0, &val32)) != SOC_E_NONE) {
         return CMD_FAIL;
      }
      sram_enq_packet = soc_reg_field_get(unit, CGM_VOQ_SRAM_ENQ_PKT_CTRr, val32, VOQ_SRAM_ENQ_PKT_CTRf);

      /* SRAM Rejected packets */
      if ((rv = soc_reg32_get(unit, CGM_VOQ_SRAM_ENQ_RJCT_PKT_CTRr, REG_PORT_ANY, 0, &val32)) != SOC_E_NONE) {
         return CMD_FAIL;
      }
      sram_rej_packet = soc_reg_field_get(unit, CGM_VOQ_SRAM_ENQ_RJCT_PKT_CTRr, val32, VOQ_SRAM_ENQ_RJCT_PKT_CTRf);

      /* SRAM to DRAM Dequeued packets */
      if ((rv = soc_reg32_get(unit, CGM_VOQ_SRAM_DEQ_TO_DRAM_PKT_CTRr, REG_PORT_ANY, 0, &val32)) != SOC_E_NONE) {
         return CMD_FAIL;
      }
      sram_to_dram_deq_packet = soc_reg_field_get(unit, CGM_VOQ_SRAM_DEQ_TO_DRAM_PKT_CTRr, val32, VOQ_SRAM_DEQ_TO_DRAM_PKT_CTRf);

      /* SRAM to Fabric Dequeued packets */
      if ((rv = soc_reg32_get(unit, CGM_VOQ_SRAM_DEQ_TO_FABRIC_PKT_CTRr, REG_PORT_ANY, 0, &val32)) != SOC_E_NONE) {
         return CMD_FAIL;
      }
      sram_to_fabric_deq_packet = soc_reg_field_get(unit, CGM_VOQ_SRAM_DEQ_TO_FABRIC_PKT_CTRr, val32, VOQ_SRAM_DEQ_TO_FABRIC_PKT_CTRf);

      /* SRAM Deleted packets */
      if ((rv = soc_reg32_get(unit, CGM_VOQ_SRAM_DEL_PKT_CTRr, REG_PORT_ANY, 0, &val32)) != SOC_E_NONE) {
         return CMD_FAIL;
      }
      sram_deleted_packet = soc_reg_field_get(unit, CGM_VOQ_SRAM_DEL_PKT_CTRr, val32, VOQ_SRAM_DEL_PKT_CTRf);

      /* DRAM Dequeued packets */
      if ((rv = soc_reg32_get(unit, CGM_VOQ_DRAM_DEQ_PKT_CTRr, REG_PORT_ANY, 0, &val32)) != SOC_E_NONE) {
         return CMD_FAIL;
      }
      dram_deq_packet = soc_reg_field_get(unit, CGM_VOQ_DRAM_DEQ_PKT_CTRr, val32, VOQ_DRAM_DEQ_PKT_CTRf);

      /* DRAM Deleted packets */
      if ((rv = soc_reg32_get(unit, CGM_VOQ_DRAM_DEL_PKT_CTRr, REG_PORT_ANY, 0, &val32)) != SOC_E_NONE) {
         return CMD_FAIL;
      }
      dram_deleted_packet = soc_reg_field_get(unit, CGM_VOQ_DRAM_DEL_PKT_CTRr, val32, VOQ_DRAM_DEL_PKT_CTRf);

      cli_out("\tSRAM Enqueued packets: %d\n\r", sram_enq_packet);
      cli_out("\tSRAM Rejected packets: %d\n\r", sram_rej_packet);
      cli_out("\tSRAM to DRAM Dequeued packets: %d\n\r", sram_to_dram_deq_packet);
      cli_out("\tSRAM to Fabric Dequeued packets: %d\n\r", sram_to_fabric_deq_packet);
      cli_out("\tSRAM Deleted packets: %d\n\r", sram_deleted_packet);
      cli_out("\tDRAM Dequeued packets: %d\n\r", dram_deq_packet);
      cli_out("\tDRAM Deleted packets: %d\n\r", dram_deleted_packet);
  }

  return rv;
}

STATIC int
dpp_diag_counter_vsq_print(
    int                        unit,
    uint32                     vsq_id,
    char                       *vsq_group_str,
    int                        vsq_index,
    int                        vsq_group_val
  )
{
   int rv = CMD_OK;
   uint32 val32;
   
   cli_out("vsq[%-3u] group[%s] indexingroup[%d]\n\r",
          vsq_id,
          vsq_group_str,
          vsq_index
          );
  if (SOC_IS_QAX(unit)) {

      uint32 vsq_enq_packet = 0, vsq_deq_packet = 0;
      uint64 val64;
      uint32 vsq_max_oc0_qsize = 0;
      static const soc_reg_t max_vsq_words_reg[] = {
        CGM_MAX_VSQA_WORDS_QSIZE_TRACKr, CGM_MAX_VSQB_WORDS_QSIZE_TRACKr,
        CGM_MAX_VSQC_WORDS_QSIZE_TRACKr, CGM_MAX_VSQD_WORDS_QSIZE_TRACKr,
        CGM_MAX_VSQE_WORDS_QSIZE_TRACKr, CGM_MAX_VSQF_WORDS_QSIZE_TRACKr
      };
      static const soc_field_t max_vsq_words_field[] = {
        MAX_VSQA_WORDS_QSIZEf, MAX_VSQB_WORDS_QSIZEf,
        MAX_VSQC_WORDS_QSIZEf, MAX_VSQD_WORDS_QSIZEf,
        MAX_VSQE_WORDS_QSIZEf, MAX_VSQF_WORDS_QSIZEf
      };

      COMPILER_64_SET(val64, 0, 0);
      if ((rv = soc_reg_get(unit, max_vsq_words_reg[vsq_group_val], REG_PORT_ANY, 0, &val64)) != SOC_E_NONE) {
         return CMD_FAIL;
      }

      vsq_max_oc0_qsize = soc_reg64_field32_get(unit, max_vsq_words_reg[vsq_group_val], val64,
                          max_vsq_words_field[vsq_group_val]);

      if ((rv = soc_reg32_get(unit, CGM_VSQ_SRAM_ENQ_PKT_CTRr, REG_PORT_ANY, 0, &val32)) != SOC_E_NONE) {
         return CMD_FAIL;
      }
      vsq_enq_packet = soc_reg_field_get(unit, CGM_VSQ_SRAM_ENQ_PKT_CTRr, val32, VSQ_SRAM_ENQ_PKT_CTRf);

      if ((rv = soc_reg32_get(unit, CGM_VSQ_SRAM_DEQ_PKT_CTRr, REG_PORT_ANY, 0, &val32)) != SOC_E_NONE) {
         return CMD_FAIL;
      }
      vsq_deq_packet = soc_reg_field_get(unit, CGM_VSQ_SRAM_DEQ_PKT_CTRr, val32, VSQ_SRAM_DEQ_PKT_CTRf);

      cli_out("\tvsq max words qsize: %d\n\r", vsq_max_oc0_qsize);
      cli_out("\tvsq enqueue packet: %d\n\r", vsq_enq_packet);
      cli_out("\tvsq dequeue packet: %d\n\r", vsq_deq_packet);

  } else if (SOC_IS_JERICHO(unit)) {
  
      uint32 vsq_max_oc0_qsize = 0, vsqmax_oc0_refresh = 0;
      uint32 vsq_enq_packet[2] = { 0 };
      uint32 vsq_deq_packet[2] = { 0 };
      uint32 core ;
      if ((rv = soc_reg32_get(unit, IQM_VSQ_MAXIMUM_OCCUPANCY_0r, REG_PORT_ANY, 0, &val32)) != SOC_E_NONE) {
         return CMD_FAIL;
      }
      vsq_max_oc0_qsize = soc_reg_field_get(unit, IQM_VSQ_MAXIMUM_OCCUPANCY_0r, val32, VSQ_MX_OC_QSZf);
      vsqmax_oc0_refresh = soc_reg_field_get(unit, IQM_VSQ_MAXIMUM_OCCUPANCY_0r, val32, VSQ_MX_OC_RFRSHf);

      for ( core = 0; core < 2; core++) {

          if ((rv = soc_reg32_get(unit, IQM_VSQ_ENQUEUE_PACKET_COUNTERr, core, 0, &val32)) != SOC_E_NONE) {
             return CMD_FAIL;
          }
          vsq_enq_packet[core] = soc_reg_field_get(unit, IQM_VSQ_ENQUEUE_PACKET_COUNTERr, val32, VSQ_ENQUEUE_PACKET_COUNTERf);

          if ((rv = soc_reg32_get(unit, IQM_VSQ_DEQUEUE_PACKET_COUNTERr, core, 0, &val32)) != SOC_E_NONE) {
             return CMD_FAIL;
          }
          vsq_deq_packet[core] = soc_reg_field_get(unit, IQM_VSQ_DEQUEUE_PACKET_COUNTERr, val32, VSQ_DEQUEUE_PACKET_COUNTERf);
      }

      cli_out("\tvsq max occupancy0 level: %d, refresh: %s\n\r", vsq_max_oc0_qsize,(vsqmax_oc0_refresh?"true":"false"));
      cli_out("\tvsq enqueue packet[core 0]: %d\n\r", vsq_enq_packet[0]);
      cli_out("\tvsq enqueue packet[core 1]: %d\n\r", vsq_enq_packet[1]);
      cli_out("\tvsq dequeue packet[core 0]: %d\n\r", vsq_deq_packet[0]);
      cli_out("\tvsq dequeue packet[core 1]: %d\n\r", vsq_deq_packet[1]);

  } else {

      uint32 vsq_max_oc0_qsize = 0, vsqmax_oc0_refresh = 0;
      uint32 vsq_enq_packet = 0, vsq_enq_packet_ovf = 0;
      uint32 vsq_deq_packet = 0, vsq_deq_packet_ovf = 0;

      if ((rv = soc_reg32_get(unit, IQM_VSQ_MAXIMUM_OCCUPANCY_0r, REG_PORT_ANY, 0, &val32)) != SOC_E_NONE) {
         return CMD_FAIL;
      }
      vsq_max_oc0_qsize = soc_reg_field_get(unit, IQM_VSQ_MAXIMUM_OCCUPANCY_0r, val32, VSQ_MX_OC_QSZf);
      vsqmax_oc0_refresh = soc_reg_field_get(unit, IQM_VSQ_MAXIMUM_OCCUPANCY_0r, val32, VSQ_MX_OC_QSZf);

      if ((rv = soc_reg32_get(unit, IQM_VSQ_ENQUEUE_PACKET_COUNTERr, REG_PORT_ANY, 0, &val32)) != SOC_E_NONE) {
         return CMD_FAIL;
      }
      vsq_enq_packet = soc_reg_field_get(unit, IQM_VSQ_ENQUEUE_PACKET_COUNTERr, val32, VSQ_ENQ_PKT_CNTf);
      vsq_enq_packet_ovf = soc_reg_field_get(unit, IQM_VSQ_ENQUEUE_PACKET_COUNTERr, val32, VSQ_ENQ_PKT_CNT_OVFf);

      if ((rv = soc_reg32_get(unit, IQM_VSQ_DEQUEUE_PACKET_COUNTERr, REG_PORT_ANY, 0, &val32)) != SOC_E_NONE) {
         return CMD_FAIL;
      }
      vsq_deq_packet = soc_reg_field_get(unit, IQM_VSQ_DEQUEUE_PACKET_COUNTERr, val32, VSQ_DEQ_PKT_CNTf);
      vsq_deq_packet_ovf = soc_reg_field_get(unit, IQM_VSQ_DEQUEUE_PACKET_COUNTERr, val32, VSQ_DEQ_PKT_CNT_OVFf);

      cli_out("\tvsq max occupancy0 level: %d, refresh: %s\n\r", vsq_max_oc0_qsize,(vsqmax_oc0_refresh?"true":"false"));
      cli_out("\tvsq enqueue packet: %d[%s]\n\r", vsq_enq_packet, (vsq_enq_packet_ovf?"ovf":""));
      cli_out("\tvsq dequeue packet: %d[%s]\n\r", vsq_deq_packet, (vsq_deq_packet_ovf?"ovf":""));

  }

  return rv;
}

STATIC cmd_result_t dpp_diag_counter_queue(int unit, args_t *a) {
   bcm_error_t rv = BCM_E_NONE;
   int is_voq = 0;
   char *option = NULL, *sub_option = NULL;
   parse_table_t  pt;
   int queue = -2, base_queue = -2, interval = -2; /* Magic Number is for Invalid */
   bcm_gport_t base_gport = BCM_GPORT_INVALID, phy_port = BCM_GPORT_INVALID;
   int num_cos = 0;
   uint32 cosq_flags;
   uint32 queue_mask = 0;
   uint32 val32;
   uint32 voq_id = 0, vsq_id = 0;
   uint32 vsq_index_in_grp = 0, vsq_group_val = 0;
   SOC_TMC_ITM_VSQ_GROUP     soc_vsq_group_type;
   uint8 is_ocb_only;
   char vsq_group_str[50];
   char *cgm_block_name = "CGM", *iqm_block_name = "IQM";
   char *gtimer_block = NULL;
   uint32 core = 0;
   int strnlen_option;

   ARG_PREV(a); /* Move Pointer Forward */
   option = ARG_GET(a);
   if (NULL == option)
      return CMD_FAIL;
   /*
    * Make sure at least one of the strings is NULL terminated.
    */
   strnlen_option = sal_strnlen(option, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH);
   if (!sal_strncasecmp(option, "voq", strnlen_option)) {
      is_voq = TRUE;
   }
   else if (!sal_strncasecmp(option, "vsq", strnlen_option)) {
      is_voq = FALSE;
   }

   sub_option = ARG_CUR(a);
   /*
    * Make sure at least one of the strings is NULL terminated.
    */
   if ((sub_option != NULL) && (!sal_strcasecmp(sub_option, "?"))) {
      if (is_voq) {
        print_counter_voq_usage();
      }
      else {
        print_counter_vsq_usage();
      }
      return CMD_FAIL;
   }

   parse_table_init(unit, &pt);
   parse_table_add(&pt, "Queue",     PQ_DFL | PQ_INT, &queue, &queue, NULL);
   parse_table_add(&pt, "BaseQueue", PQ_DFL | PQ_INT, &base_queue, &base_queue, NULL);
   parse_table_add(&pt, "Interval",  PQ_DFL | PQ_INT, &interval, &interval, NULL);
   if (0 > parse_arg_eq(a, &pt)) {
      parse_arg_eq_done(&pt);
      return CMD_FAIL;
   }   

   parse_arg_eq_done(&pt);

   if (queue != -2) {
      num_cos = 1;
      if (!is_voq) {
         vsq_id = queue;
         
         /* Get vsq index in grp and vsq group type from vsq id */
         rv = soc_dpp_cosq_vsq_index_global_to_group_get(unit, vsq_id, &soc_vsq_group_type, &vsq_index_in_grp, &is_ocb_only);
         if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_APPL_SHELL,
                      (BSL_META_U(unit,
                                  "\nsoc_dpp_cosq_vsq_index_global_to_group_get failed(%d)!"), rv));
            return CMD_FAIL;
         }
          
         switch(soc_vsq_group_type)
         {
            case SOC_TMC_ITM_VSQ_GROUP_CTGRY:
               vsq_group_val = 0;
               sal_sprintf(vsq_group_str, "%s", "category");
               break;
            case SOC_TMC_ITM_VSQ_GROUP_CTGRY_TRAFFIC_CLS:
               vsq_group_val = 1;
               sal_sprintf(vsq_group_str, "%s", "category and traffic class");               
               break;
            case SOC_TMC_ITM_VSQ_GROUP_CTGRY_2_3_CNCTN_CLS:
               vsq_group_val = 2;
               sal_sprintf(vsq_group_str, "%s", "category 2/3 and connection class");               
               break;
            case SOC_TMC_ITM_VSQ_GROUP_STTSTCS_TAG:
               vsq_group_val = 3;
               sal_sprintf(vsq_group_str, "%s", "statistics tag");               
               break;
            case SOC_TMC_ITM_VSQ_GROUP_LLFC:
               vsq_group_val = 4;
               sal_sprintf(vsq_group_str, "%s", "Link Level Flow Control");               
               break;             
            case SOC_TMC_ITM_VSQ_GROUP_PFC:
               vsq_group_val = 5;
               sal_sprintf(vsq_group_str, "%s", "Port Flow Control");               
               break;               
            default:
               cli_out("Unkonw vsq group type\n");
               return CMD_FAIL;            
         }
      }
      else {
         voq_id = queue;
      }
   }
   else if (base_queue != -2) {
      if (is_voq) {
         do {
            BCM_GPORT_UNICAST_QUEUE_GROUP_SET(base_gport, base_queue);      
            rv = bcm_cosq_gport_get(unit, base_gport, &phy_port, &num_cos, &cosq_flags);
            if (num_cos > 0) {
               break;
            }
      
            BCM_GPORT_MCAST_QUEUE_GROUP_SET(base_gport, base_queue);
            rv = bcm_cosq_gport_get(unit, base_gport, &phy_port, &num_cos, &cosq_flags);
            if (num_cos > 0) {
               break;
            }
   
            BCM_COSQ_GPORT_ISQ_SET(base_gport, base_queue);
            rv = bcm_cosq_gport_get(unit, base_gport, &phy_port, &num_cos, &cosq_flags);
            if (num_cos > 0) {
               break;
            }

            break;
         }while(1);
         if ((rv != BCM_E_NONE) || (num_cos <= 0)) {
            cli_out("Failed get voq num cos for voq(%d)\n", base_queue);
            return CMD_FAIL;            
         }
            
         voq_id = base_queue;
      }
      else {
         cli_out("vsq Requires Queue value\n");
         return CMD_FAIL;
      }      
   }
   else {
      if (is_voq)
         cli_out("voq Requires Queue value or BaseQueue value\n");
      else
         cli_out("vsq Requires Queue value\n");

      return CMD_FAIL;
   }

   queue_mask = num_cos - 1;
   if (is_voq) {
      /* Write program regitster to select queue */
      if (SOC_IS_JERICHO_PLUS_AND_BELOW(unit)) {
          if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
              val32 = 0;
              soc_reg_field_set(unit, IQM_PROGRAMMABLE_COUNTER_QUEUE_SELECT_0r, &val32, PRG_CNT_Qf, voq_id);
              if (( soc_reg32_set(unit, IQM_PROGRAMMABLE_COUNTER_QUEUE_SELECT_0r, REG_PORT_ANY, 0, val32)) != SOC_E_NONE) {
                 return CMD_FAIL;
              }
              val32 = 0;
              soc_reg_field_set(unit, IQM_PROGRAMMABLE_COUNTER_QUEUE_SELECT_1r, &val32, PRG_CNT_MSKf, queue_mask);
              if ((soc_reg32_set(unit, IQM_PROGRAMMABLE_COUNTER_QUEUE_SELECT_1r, REG_PORT_ANY, 0, val32)) != SOC_E_NONE) {
                 return CMD_FAIL;
              }
          } else if (SOC_IS_JERICHO(unit)) {
              uint64 reg64_val;
              COMPILER_64_ZERO(reg64_val);
              soc_reg64_field32_set(unit, IQM_PROGRAMMABLE_COUNTER_QUEUE_SELECTr, &reg64_val, PRG_CNT_Qf, voq_id);
              soc_reg64_field32_set(unit, IQM_PROGRAMMABLE_COUNTER_QUEUE_SELECTr, &reg64_val, PRG_CNT_MSKf, queue_mask);

              for (core = 0; core < 2; core++) {
                  if (( soc_reg_set(unit, IQM_PROGRAMMABLE_COUNTER_QUEUE_SELECTr, core, 0, reg64_val)) != SOC_E_NONE) {
                     return CMD_FAIL;
                  }
              }
          }

          gtimer_block = iqm_block_name;
      }
      else if (SOC_IS_QAX(unit)) {
          uint64 reg64_val;
          COMPILER_64_ZERO(reg64_val);
          soc_reg64_field32_set(unit, CGM_PRG_CTR_SETTINGSr, &reg64_val, VOQ_PRG_CTR_QNUM_INDEXf, voq_id);
          soc_reg64_field32_set(unit, CGM_PRG_CTR_SETTINGSr, &reg64_val, VOQ_PRG_CTR_QNUM_INDEX_MASKf, queue_mask);

          if (( soc_reg_set(unit, CGM_PRG_CTR_SETTINGSr, REG_PORT_ANY, 0, reg64_val)) != SOC_E_NONE) {
             return CMD_FAIL;
          }

          gtimer_block = cgm_block_name;
      }

      /* Enable and trigger gtimer */
      if ( (interval != -2) && (gtimer_block != NULL)) {
         if ((gtimer_enable(unit, gtimer_block, interval)) != CMD_OK) {
            cli_out("Failed(%d) enable gtimer\n", rv);
            return CMD_FAIL;
         }
         if ((gtimer_trigger(unit, gtimer_block)) != CMD_OK) {
            cli_out("Failed(%d) enable gtimer\n", rv);
            return CMD_FAIL;
         }

         cli_out("Start gtimer\n");
         sal_usleep(interval);    /* Wait interval uSec */
      }

      /* Read Counters value */
      if ((dpp_diag_counter_voq_print(unit, voq_id, num_cos)) != CMD_OK) {
         return CMD_FAIL;
      }

      /* Stop gtimer */
      if ( (interval != -2) && (gtimer_block != NULL)) {
          if ((gtimer_stop(unit, gtimer_block)) != CMD_OK) {
              cli_out("Failed(%d) enable gtimer\n", rv);
              return CMD_FAIL;
          }
          else {
              cli_out("Stop gtimer\n");
          }
      }
   }
   else {
      if (!SOC_IS_QAX(unit)) {
          /* Write program regitster to select queue */
          val32 = 0;
          soc_reg_field_set(unit, IQM_VSQ_PROGRAMMABLE_COUNTER_SELECTr, &val32, VSQ_PRG_GRP_SELf, vsq_group_val);
          soc_reg_field_set(unit, IQM_VSQ_PROGRAMMABLE_COUNTER_SELECTr, &val32, VSQ_PRG_CNT_Qf, vsq_index_in_grp);
          soc_reg_field_set(unit, IQM_VSQ_PROGRAMMABLE_COUNTER_SELECTr, &val32, VSQ_PRG_CNT_MSKf, queue_mask);
          if ((soc_reg32_set(unit, IQM_VSQ_PROGRAMMABLE_COUNTER_SELECTr, REG_PORT_ANY, 0, val32)) != SOC_E_NONE) {
              return CMD_FAIL;
          }

          /* Enable and trigger gtimer in IQM */
          if (interval != -2) {
             if ((gtimer_enable(unit, "IQM", interval)) != CMD_OK) {
                cli_out("Failed(%d) enable gtimer\n", rv);
                return CMD_FAIL;
             }
             if ((gtimer_trigger(unit, "IQM")) != CMD_OK) {
                cli_out("Failed(%d) enable gtimer\n", rv);
                return CMD_FAIL;
             }
             cli_out("Start gtimer in IQM\n");

             sal_usleep(interval);    /* Wait interval uSec */
          }
      }
      /* Read Counters value */
      if ((dpp_diag_counter_vsq_print(unit, vsq_id, vsq_group_str, vsq_index_in_grp, vsq_group_val)) != CMD_OK) {
         return CMD_FAIL;
      }

      if (!SOC_IS_QAX(unit)) {
          /* Stop gtimer in IQM */
          if (interval != -2) {
             if ((gtimer_stop(unit, "IQM")) != CMD_OK) {
                cli_out("Failed(%d) enable gtimer\n", rv);
                return CMD_FAIL;
             }
             else {
                cli_out("Stop gtimer in IQM\n");
             }
          }
      }

   }   
   
   return CMD_OK;
}
#endif /* BCM_PETRA_SUPPORT */

STATIC cmd_result_t
dpp_diag_counters_print(int unit, diag_counter_data_t *counters, int ignore_zero, uint32 hex_base, char **block_names, int num_of_block_names) {
   soc_reg_above_64_val_t data, field_val, ovf_field_val;
   char dval_str[256], counter_name[MAX_COUNTER_NAME_LEN];
   uint32 addr;
   uint8 acc_type, any_reg_printed = 0;
   soc_block_type_t last_block_type = SOC_BLOCK_TYPE_INVALID;
   int i, rc = SOC_SAND_OK;
   soc_block_t blk, blk_dummy;
   char    buf_val[32];
   int     commachr = soc_property_get(unit, spn_DIAG_COMMA, ',');
   uint64  val_64;
   bcm_pbmp_t bitmap;

   /*check block ids to be printed according to given block names.*/
   blk = 0;
   BCM_PBMP_CLEAR(bitmap);
   for (blk = 0; SOC_BLOCK_INFO(unit, blk).type != -1; blk++) {
      if (block_names != NULL) {
         int j;
         for (j = 0; j < num_of_block_names; j++) {
            if (0 == sal_strncasecmp(block_names[j],
                   soc_block_name_lookup_ext(SOC_BLOCK_INFO(unit, blk).type, unit), sal_strnlen(block_names[j],SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH))) {
               break;
            }
         }
         if (j == num_of_block_names) continue;
      }
      BCM_PBMP_PORT_ADD(bitmap, blk);
   }

   for (i = 0; INVALIDr != counters[i].reg; i++) {
       soc_block_t block_type;

       if (!SOC_REG_IS_VALID(unit,counters[i].reg)) {
           continue;
       }
      block_type = SOC_REG_INFO(unit, counters[i].reg).block[0]; 
      if (!SOC_IS_ARADPLUS(unit) && counters[i].flags==DIAG_COUNTERS_F_ARADPLUS_ONLY)
      {
          continue;
      }

      if (!SOC_REG_IS_VALID(unit, counters[i].reg)) {
         cli_out("Invalid register for counter: %s\n", counters[i].reg_name);
         continue;
      }
      if (SOC_IS_QAX(unit) && (SOC_BLK_CLP==block_type || SOC_BLK_XLP==block_type)) {
          continue;
      }
     
      if (SOC_IS_QUX(unit) && (SOC_BLK_NBIL == block_type)) {
          continue;
      }
      SOC_BLOCKS_ITER(unit, blk, SOC_REG_INFO(unit, counters[i].reg).block) {
         /*skip blks not mentioned in block_names*/
         if (!BCM_PBMP_MEMBER(bitmap, blk)) continue;

         if (SOC_BLOCK_IN_LIST(unit, SOC_REG_INFO(unit, counters[i].reg).block, SOC_BLK_PORT) && !SOC_PORT_VALID(unit,SOC_BLOCK_INFO(unit, blk).number)) {
             continue;
         }
         /*reads data from a register*/
         if (soc_cpureg == SOC_REG_INFO(unit, counters[i].reg).regtype) { /* CMIC register */
            SOC_REG_ABOVE_64_CLEAR(data);
            addr = soc_reg_addr_get(unit, counters[i].reg, SOC_BLOCK_INFO(unit, blk).number, counters[i].reg_index, SOC_REG_ADDR_OPTION_NONE, &blk_dummy, &acc_type);
            data[0] = soc_pci_read(unit, addr);
         } else { /* regular register */
            rc = soc_reg_above_64_get(unit, counters[i].reg, SOC_BLOCK_INFO(unit, blk).number, counters[i].reg_index, data);
            if (SOC_FAILURE(rc)) {
               cli_out("Failed to read counter: %s\n", counters[i].reg_name);
               continue;
            }
         }

         /*reads specific field from a register*/
         soc_reg_above_64_field_get(unit, counters[i].reg, data, counters[i].cnt_field, field_val);
         if (INVALIDf != counters[i].overflow_field) {
            soc_reg_above_64_field_get(unit, counters[i].reg, data, counters[i].overflow_field, ovf_field_val);
         }

         /* Skip if the counter is zero and we should ignore zero counters */
         if (ignore_zero && SOC_REG_ABOVE_64_IS_ZERO(field_val)) {
            continue;
         }

         if (last_block_type != counters[i].block) {
            last_block_type = counters[i].block;
            cli_out("\n%4s counters:\n==============\n",
                    /*SOC_BLOCK_NAME(unit, SOC_REG_INFO(unit, counters[i].reg).block),*/
                    soc_block_name_lookup_ext(last_block_type, unit));
         }



         /* print field to string according to register type/length */
         if (SOC_REG_IS_ABOVE_64(unit, counters[i].reg)) {
            /* SOC_REG_IS_ABOVE_64 only checks counter size, and not the value.
               If value is higher than 64 bit print as hex, otherwise decimal */
            if ((hex_base == 1) || counter_value_higher_than_64(field_val)) {
               format_long_integer(dval_str, field_val, SOC_REG_ABOVE_64_INFO(unit, counters[i].reg).size);
            } else {
               /*print only hex else print decimal*/
               COMPILER_64_SET(val_64, field_val[1], field_val[0]);
               format_uint64_decimal(buf_val, val_64, commachr);
               sal_snprintf(dval_str, sizeof(dval_str), "%s", buf_val);
            }
         } else if ((SOC_REG_IS_64(unit, counters[i].reg))) {
            if (hex_base == 1) {
               format_long_integer(dval_str, field_val, 2);
            } else {
               COMPILER_64_SET(val_64, field_val[1], field_val[0]);
               format_uint64_decimal(buf_val, val_64, commachr);
               sal_snprintf(dval_str, sizeof(dval_str), "%s", buf_val);
            }
         } else {
            if (hex_base == 1) {
               format_long_integer(dval_str, field_val, 1);
            } else {
               COMPILER_64_SET(val_64, 0, field_val[0]);
               format_uint64_decimal(buf_val, val_64, commachr);
               sal_snprintf(dval_str, sizeof(dval_str), "%s", buf_val);
            }
         }

         sal_snprintf(counter_name, MAX_COUNTER_NAME_LEN, "%5s %s                                                ",
                      SOC_BLOCK_NAME(unit, blk), counters[i].reg_name);
         cli_out("%40.40s: %s%s\n", counter_name, dval_str,
                 ((!SOC_REG_ABOVE_64_IS_ZERO(field_val) || !ignore_zero) && INVALIDf != counters[i].overflow_field && ovf_field_val[0]) ? " ovf" : "");
         any_reg_printed = 1;
      }
   }
   /*If all registers were filtered print warning */
   if (!any_reg_printed) {
      int j;
      cli_out("No matches found for block names:");
      if(block_names != NULL) {
          for (j = 0; j < num_of_block_names; j++) {
             cli_out(" %4s", block_names[j]);
          }
      }
      if (ignore_zero) {
         cli_out(" with non zero values.");
      }
      cli_out("\n");
   }

   return CMD_OK;
}

STATIC cmd_result_t
dpp_diag_counters(int unit, args_t *a) {
   diag_counter_data_t *counters = 0;
#ifdef BCM_DFE_SUPPORT
   int rv;
#endif
   uint32 ignore_zero = 0, packet_flow = 0, graphical = 0, show_in_hex_base = 0;
   char *option;
   char *block_names[50];
   int num_of_block_names = 0;

#ifdef BCM_JERICHO_SUPPORT
   uint32 core_disp = 0;
#endif

   parse_table_t  pt;
   cmd_result_t   retCode = CMD_OK;
#ifdef BCM_PETRA_SUPPORT
   uint32 interval = 0;
   char *block_names_arad[] = {"EGQ", "EPNI", "FDR", "FDT", "IRE", "IPT", "IQM", "NBI"};
   char *block_names_jericho[] = {"EGQ", "EPNI", "FDR", "FDT", "IRE", "IPT", "IQM", "NBIL", "NBIH"};
   char *block_names_gtimer_all[] = {"CFC", "CRPS", "ECI", "EGQ", "EPNI", "FCR", "FCT", "FDR", "FDT", "FMAC", 
      "IDR", "IHB", "IHP", "IPT", "IQM", "IRE", "IRR", "MMU", "NBI", "OAMP", "OCB", "OLP"};
   char **block_names_cur = NULL;
   int block_num = 0;
   int rv1 = CMD_OK;
#endif
   

   option = ARG_GET(a);
#ifdef BCM_PETRA_SUPPORT
   /*
    * Make sure at least one of the strings is NULL terminated.
    */
   if ((NULL != option) && (!sal_strncasecmp(option, "voq", sal_strnlen(option, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH)) || 
       !sal_strncasecmp(option, "vsq", sal_strnlen(option, sal_strnlen(option, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH))))) {
      return dpp_diag_counter_queue(unit, a);
   }
#endif
   
   parse_table_init(unit, &pt);
#ifdef BCM_PETRA_SUPPORT
   if (SOC_IS_ARAD(unit) || SOC_IS_JERICHO(unit)) {
       parse_table_add(&pt, "Interval", PQ_DFL | PQ_INT, &interval, &interval, NULL); 
   }
#endif

   if (0 > parse_arg_eq(a, &pt)) {
      parse_arg_eq_done(&pt);
      return retCode;
   }
   parse_arg_eq_done(&pt);

   while (NULL != option) {
      int strnlen_option;
      /*
       * Make sure at least one of the strings is NULL terminated.
       */
      strnlen_option = sal_strnlen(option, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH);
      if (!sal_strncasecmp(option, "nz", strnlen_option)) {
         ignore_zero = 1;
      } else if (!sal_strncasecmp(option, "packet_flow", strnlen_option)) {
         packet_flow = 1;
      } else if (!sal_strncasecmp(option, "graphical", strnlen_option)) {
         graphical = 1;
      } else if (!sal_strncasecmp(option, "hex", strnlen_option)) {
         show_in_hex_base = 1;
#ifdef BCM_JERICHO_SUPPORT
      } else if ((!sal_strncasecmp(option, "cdsp", strnlen_option)) || 
                 (!sal_strncasecmp(option, "core_disp", strnlen_option))) {
         core_disp = 1;
#endif
#ifdef BCM_PETRA_SUPPORT
      } else if (sal_strchr(option, '=')) {
         /* Nothing to do. Ignore current option */
#endif         
      } else {
         block_names[num_of_block_names] = option;
         num_of_block_names++;
      }
      option = ARG_GET(a);
   }

   if ((packet_flow == 0) && (graphical == 0)) {
      counters = diag_counters_get(unit);
      if (NULL == counters) {
         cli_out("Not supported for unit %d \n", unit);
         return CMD_FAIL;
      }
   } else if ((SOC_IS_ARAD(unit) || SOC_IS_JERICHO(unit)) && graphical == 1) {
#ifdef BCM_PETRA_SUPPORT
      if (interval != 0) {

          if (SOC_IS_JERICHO(unit)) {
             block_names_cur = block_names_jericho;
             block_num = sizeof(block_names_jericho)/sizeof(char *);
          } else {
             block_names_cur = block_names_arad;
             block_num = sizeof(block_names_arad)/sizeof(char *);
          }

            
         /* Enable and Trigger gtimer */
         if ((rv1 = dpp_diag_gtimer_blocks_start(unit, block_names_cur, block_num, interval)) == CMD_FAIL) {
            return CMD_FAIL;
         }
      }
      if (SOC_IS_QUX(unit)) {
#ifdef BCM_QUX_SUPPORT
          rv1 = qux_counters_graphic_print(unit, ignore_zero, show_in_hex_base, (uint32) 0);
#endif
      } else if (SOC_IS_QAX(unit)) {
#ifdef BCM_88470_A0
          rv1 = qax_counters_graphic_print(unit, ignore_zero, show_in_hex_base, (uint32) 0);
#endif
      } else if (SOC_IS_JERICHO(unit)) {
#ifdef BCM_JERICHO_SUPPORT
          rv1 = jericho_counters_graphic_print(unit, ignore_zero, show_in_hex_base, core_disp);
#endif
      } else if (SOC_IS_ARAD(unit)) {
          rv1 = arad_packet_flow_counter_get(unit, ignore_zero, show_in_hex_base); 
      }
     
      if (interval != 0) {
         /* Stop gtimer */
         rv1 |= dpp_diag_gtimer_blocks_stop(unit, block_names_cur, block_num);
      }
      if (rv1 != CMD_OK) return CMD_FAIL;
#endif 
      
   } else if (SOC_IS_DFE(unit) && graphical == 1){
#ifdef BCM_DFE_SUPPORT
       rv = diag_dfe_fabric_counters_print(unit);
       if (BCM_FAILURE(rv)) {
           cli_out("ERRROR: soc_dfe_counters_print failed.\n");
           return CMD_FAIL;
       }

#endif      
   } else { /*packet flow*/
#ifdef BCM_PETRA_SUPPORT
      if(SOC_IS_ARDON(unit)){
         counters = ardon_packet_flow_counters_data;
      } else if (SOC_IS_ARAD(unit)) {
         counters = arad_packet_flow_counters_data;
      }
#endif
#ifdef BCM_88750_SUPPORT
      if (SOC_IS_FE1600(unit)) {
         counters = fe1600_packet_flow_counters_data;
      }
#endif
#ifdef BCM_JERICHO_SUPPORT
      if (SOC_IS_JERICHO(unit)) {
         counters = jericho_counters_for_graphic_display;
      }
#endif
#ifdef BCM_88470_A0
      if (SOC_IS_QAX(unit)) {
         counters = qax_counters_for_graphic_display;
      }
#endif
#ifdef BCM_QUX_SUPPORT
      if (SOC_IS_QUX(unit)) {
          counters = qux_counters_for_graphic_display;
      }
#endif

   }

   /*either packet flow or not graphical*/
   if (counters) {
#ifdef BCM_PETRA_SUPPORT
      if (interval != 0) {
         if (num_of_block_names == 0) {
            block_names_cur = block_names_gtimer_all;
            block_num = sizeof(block_names_gtimer_all)/sizeof(char *);
         }
         else {
            block_names_cur = block_names; 
            block_num = num_of_block_names;
         }
            
         /* Enable and Trigger gtimer */
         if ((rv1 = dpp_diag_gtimer_blocks_start(unit, block_names_cur, block_num, interval)) == CMD_FAIL) {
            return CMD_FAIL;
         }
      }
#endif    
      dpp_diag_counters_print(unit, counters, ignore_zero, show_in_hex_base, (num_of_block_names == 0 ? NULL : block_names), num_of_block_names);
#ifdef BCM_PETRA_SUPPORT
      if (interval != 0) {
         /* Stop gtimer */
         if ((rv1 = dpp_diag_gtimer_blocks_stop(unit, block_names_cur, block_num)) == CMD_FAIL) {
            return CMD_FAIL;
         }
      }
#endif 

   }
   return CMD_OK;

}


#ifdef BCM_PETRA_SUPPORT
STATIC int _dpp_mem_total_size(int unit, soc_mem_t mem)
{
    int             ii, rc = 0;
    soc_mem_info_t* meminfo;

    if (!SOC_MEM_IS_VALID(unit, mem))
    {
        return 0;
    }

    meminfo = &SOC_MEM_INFO(unit, mem);
    for (ii=0; ii<meminfo->nFields; ii++)
    {
        rc += meminfo->fields[ii].len;
    }

    return rc;
}

STATIC int _dpp_block_type_is_pp(int blk_type)
{
    int     rc;
    switch (blk_type)
    {
        case SOC_BLK_EDB:
        case SOC_BLK_EPNI:
        case SOC_BLK_IHB:
        case SOC_BLK_IHP:
        case SOC_BLK_PPDB_A:
        case SOC_BLK_PPDB_B:
        case SOC_BLK_KAPS:
        case SOC_BLK_OAMP:
        case SOC_BLK_OLP:
            rc = 1;
            break;
        default:
            rc = 0;
            break;
    }

    return rc;
}



STATIC int dpp_diag_ser_tbl_info_dump(int unit, soc_mem_t mem, void* data)
{
    int                 rv = SOC_E_NONE;
    char*               delimiters;
    uint16              block;
    int                 blk_type;
    int                 protect;
    char*               str_protect[] = {"none", "PARITY", "ECC"};
    soc_mem_severe_t    severity;
    int                 numels, entry_dw, index_cnt, cache_size;
    int                 copyno;
    uint8               acc_type;
    uint32              address, end_address;
    soc_mem_t           mem_alias;
    int                 bit_length, is_mem_alias;
    char*               ser_action;

    delimiters = (char*)data;
    if (!SOC_MEM_IS_VALID(unit, mem))
    {
        return rv;
    }

    is_mem_alias = 0;
    mem_alias = SOC_MEM_ALIAS_MAP(unit,mem);
    if (SOC_MEM_IS_VALID(unit, mem_alias))
    {
        is_mem_alias = 1;
    }
    else
    {
        mem_alias = mem;
    }

    /*meminfo = &SOC_MEM_INFO(unit, mem);*/

    /* Memory Name */
    cli_out("%s", SOC_MEM_NAME(unit, mem));
    cli_out("%s", delimiters);

    /* Block */
    /* SOC_BLOCK_TYPE ?? */
    block = SOC_MEM_BLOCK_MIN(unit, mem_alias);
    blk_type = SOC_BLOCK_TYPE(unit, block);
    cli_out("%s", soc_block_name_lookup_ext(blk_type, unit));
    cli_out("%s", delimiters);

    /* Block-Type */
    if (_dpp_block_type_is_pp(blk_type))
    {
        cli_out("PP%s", delimiters);
    }
    else
    {
        cli_out("TM%s", delimiters);
    }

    /* ReadOnly */
    if (soc_mem_is_readonly(unit, mem_alias))
    {
        cli_out("TRUE%s", delimiters);
    }
    else
    {
        cli_out("FALSE%s", delimiters);
    }

    /* WriteOnly */
    if (soc_mem_is_writeonly(unit, mem_alias))
    {
        cli_out("TRUE%s", delimiters);
    }
    else
    {
        cli_out("FALSE%s", delimiters);
    }

    /* Internal */
    if (SOC_MEM_IS_INTERNAL(unit, mem_alias))
    {
        cli_out("TRUE%s", delimiters);
    }
    else
    {
        cli_out("FALSE%s", delimiters);
    }

    /* Dynamic */
    if ((SOC_MEM_INFO(unit, mem_alias).flags & SOC_MEM_FLAG_SIGNAL))
    {
        cli_out("TRUE%s", delimiters);
    }
    else
    {
        cli_out("FALSE%s", delimiters);
    }

    /* Cacheable */
    if (soc_mem_is_cachable(unit, mem_alias))
    {
        cli_out("TRUE%s", delimiters);
    }
    else
    {
        cli_out("FALSE%s", delimiters);
    }

    /* Use FF & Protection */
    protect = 0;
    if (SOC_MEM_FIELD_VALID(unit, mem_alias, PARITYf) ||
        (SOC_IS_ARADPLUS_AND_BELOW(unit) && (mem_alias == SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDSm)))
    {
        protect = 1;
    }
    if (SOC_MEM_FIELD_VALID(unit, mem_alias, ECCf))
    {
        protect = 2;
    }
    /* Use FF */
    if (protect == 0)
    {
        cli_out("TRUE%s", delimiters);
    }
    else
    {
        cli_out("FALSE%s", delimiters);
    }
    /* Protection */
    cli_out("%s%s", str_protect[protect], delimiters);

    /* Type: Config/Data/Control ???? */
    cli_out("%s", delimiters);

    /* Severity */
    severity = SOC_MEM_SEVERITY(unit, mem_alias);
    if (severity == SOC_MEM_SEVERE_LOW)
    {
        cli_out("Low%s", delimiters);
    }
    else if (severity == SOC_MEM_SEVERE_MEDIUM)
    {
        cli_out("Medium%s", delimiters);
    }
    else if (severity == SOC_MEM_SEVERE_HIGH)
    {
        cli_out("Severe%s", delimiters);
    }
    else
    {
        cli_out("Other%s", delimiters);
    }

    /* Alias*/
    if (is_mem_alias)
    {
        cli_out("%s", SOC_MEM_NAME(unit, mem_alias));
        cli_out("%s", delimiters);
    }
    else
    {
        cli_out("%s", delimiters);
    }

    /*

    */
    bit_length = _dpp_mem_total_size(unit, mem_alias);
    numels = 1;
    if (SOC_MEM_IS_ARRAY(unit, mem_alias))
    {
        numels = SOC_MEM_ARRAY_INFO(unit, mem_alias).numels;
    }
    entry_dw    = soc_mem_entry_words(unit, mem_alias);
    index_cnt   = soc_mem_index_count(unit, mem_alias);

    /* Size(bits) */
    cli_out("%d%s", bit_length*numels*index_cnt, delimiters);

    /* CacheSize(bits) */
    cache_size = 0;
    if (soc_mem_is_cachable(unit, mem_alias))
    {
        cache_size  = numels * index_cnt * entry_dw * 4 * 8;
    }
    cli_out("%d%s", cache_size, delimiters);

    /*soc_mem_info_t* meminfo;*/
    /* Address */
    copyno = SOC_MEM_BLOCK_ANY(unit, mem_alias);
    address = soc_mem_addr_get(unit, mem_alias, 0, copyno, 0, &acc_type);
    cli_out("0x%x%s", address, delimiters);

    /* End Address */
    end_address = address + index_cnt - 1;
    cli_out("0x%x%s", end_address, delimiters);

    /* Lines */
    cli_out("%d%s", index_cnt, delimiters);

    /* Length(bits) */
    bit_length = _dpp_mem_total_size(unit, mem_alias);
    cli_out("%d%s", bit_length, delimiters);

    /* SER Action -- See dcmn_mem_decide_corrective_action */
    if (is_mem_alias)
    {
        cli_out("Alias, No Action%s%s%s%s", delimiters, delimiters, delimiters, delimiters);
    }
    else
    {
        if (protect == 0)
        {
            cli_out("Un-protected RegArray, No Action%s%s%s%s", delimiters, delimiters, delimiters, delimiters);
        }
        else if (protect == 1)
        {
            /* PARITY */
            cli_out("PARITY protected%s", delimiters);
            ser_action = dcmn_mem_ser_ecc_action_info_get(unit, mem, 0);
            cli_out("%s%s%s%s", ser_action, delimiters, delimiters, delimiters);
        }
        else
        {
            /* ECC */
            cli_out("ECC protected%s%s", delimiters, delimiters);
            ser_action = dcmn_mem_ser_ecc_action_info_get(unit, mem, 1);
            cli_out("%s%s", ser_action, delimiters);
            ser_action = dcmn_mem_ser_ecc_action_info_get(unit, mem, 0);
            cli_out("%s%s", ser_action, delimiters);
        }
    }

    cli_out("\n");
    return rv;
}


#define MEM_DUMP_FLAG_DYNAMIC       0x1
#define MEM_DUMP_FLAG_STATIC        0x2
#define MEM_DUMP_FLAG_INTERNAL      0x4
#define MEM_DUMP_FLAG_READONLY      0x8
#define MEM_DUMP_FLAG_WRITEONLY     0x10
#define MEM_DUMP_FLAG_ACCESSABLE    0x20
#define MEM_DUMP_FLAG_SOFT_ALIAS    0x40
#define MEM_DUMP_FLAG_HARD_ALLAS    0x80
#define MEM_DUMP_FLAG_NO_ALLAS      0x100
#define MEM_DUMP_FLAG_SPECIAL_BLOCK 0x200
#define MEM_DUMP_FLAG_ARRAY_ONLY    0x400
#define MEM_DUMP_FLAG_SEVERE_ONLY   0x800
#define MEM_DUMP_FLAG_CACHE_ONLY    0x1000
#define MEM_DUMP_FLAG_SHADOWED      0x2000
#define MEM_DUMP_FLAG_EXPORT_MODE   0x4000


#define MEM_HARD_ALIAS_MAX_NUM      32

typedef struct diag_hard_alias_s
{
    int         blk_type;
    uint32_t    address;

    uint32_t    mem_number;
    uint32_t    alias_number;
    uint32_t    notAlias_number;
    soc_mem_t   mem_list[MEM_HARD_ALIAS_MAX_NUM];
}diag_hard_alias_t;

typedef struct diag_mem_dump_cfg_s
{
    uint32_t    mem_dump_flag;
    int         block_type;
    uint32_t    show_idx;

    uint32_t            hard_addr_num;
    diag_hard_alias_t   hard_alias_info[NUM_SOC_MEM];
}diag_mem_dump_cfg_t;


STATIC int dpp_diag_hard_alias_mem_export(int unit, diag_mem_dump_cfg_t* dump_cfg)
{
    char*       delimiters = "&";
    int         ii, jj;
    soc_mem_t   mem, mem_alias;
    uint32_t    mem_number, notAlias_number;

    for (ii=0; ii<dump_cfg->hard_addr_num; ii++)
    {
        mem_number      = dump_cfg->hard_alias_info[ii].mem_number;
        notAlias_number = dump_cfg->hard_alias_info[ii].notAlias_number;
        if ((mem_number > 1) && (notAlias_number > 1))
        {
            cli_out("%s%s%08X\n",
                soc_block_name_lookup_ext(dump_cfg->hard_alias_info[ii].blk_type, unit),
                delimiters, dump_cfg->hard_alias_info[ii].address);
            for (jj=0; jj<mem_number; jj++)
            {
                mem = dump_cfg->hard_alias_info[ii].mem_list[jj];
                mem_alias = SOC_MEM_ALIAS_MAP(unit, mem);
                cli_out("%s%s%s%s", delimiters, delimiters, SOC_MEM_NAME(unit, mem), delimiters);

                if (SOC_MEM_IS_VALID(unit, mem_alias))
                {
                    cli_out("%s\n", SOC_MEM_NAME(unit, mem_alias));
                }
                else if (SOC_MEM_IS_ALIAS(unit, mem))
                {
                    cli_out("***%s\n", SOC_MEM_NAME(unit, mem_alias));
                }
                else
                {
                    cli_out("%s", delimiters);

                    /* Dynamic */
                    if (soc_mem_is_signal(unit, mem))
                    {
                        cli_out("%s%s", "TRUE", delimiters);
                    }
                    else
                    {
                        cli_out("%s%s", "FALSE", delimiters);
                    }

                    /*Protected*/
                    if (SOC_MEM_FIELD_VALID(unit, mem, PARITYf) ||
                        (SOC_IS_ARADPLUS_AND_BELOW(unit) && (mem_alias == SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDSm))) 
                    {
                        cli_out("PARITY%s", delimiters);
                    }
                    else if (SOC_MEM_FIELD_VALID(unit, mem, ECCf))
                    {
                        cli_out("ECC%s", delimiters);
                    }
                    else
                    {
                        cli_out("NONE%s", delimiters);
                    }

                    /* Entries & bytes */
                    cli_out("%d%s%d%s", soc_mem_index_count(unit, mem), delimiters,
                                soc_mem_entry_bytes(unit, mem), delimiters);

                    if (SOC_MEM_IS_ARRAY(unit, mem))
                    {
                        cli_out("ARRAY-%d%s\n", SOC_MEM_NUMELS(unit, mem), delimiters);
                    }
                    else
                    {
                        cli_out("%s\n", delimiters);
                    }
                }
            }
        }
    }

    return CMD_OK;
}

STATIC int dpp_diag_hard_alias_mem_info_dump(int unit, diag_mem_dump_cfg_t* dump_cfg)
{
    int         ii, jj;
    soc_mem_t   mem, mem_alias;
    uint32_t    mem_number, alias_number, notAlias_number;
    char        srt_entry_info[32] = {0};

    if (dump_cfg->mem_dump_flag & MEM_DUMP_FLAG_EXPORT_MODE)
    {
        return dpp_diag_hard_alias_mem_export(unit, dump_cfg);
    }

    cli_out("\nhard_addr_num=%d\n", dump_cfg->hard_addr_num);

    for (ii=0; ii<dump_cfg->hard_addr_num; ii++)
    {
        mem_number      = dump_cfg->hard_alias_info[ii].mem_number;
        alias_number    = dump_cfg->hard_alias_info[ii].alias_number;
        notAlias_number = dump_cfg->hard_alias_info[ii].notAlias_number;
        if ((mem_number > 1) && (notAlias_number > 1))
        {
            cli_out("%s-- %08X,     mem_number=%d, alias_number=%d, notAlias_number=%d\n",
                soc_block_name_lookup_ext(dump_cfg->hard_alias_info[ii].blk_type, unit),
                dump_cfg->hard_alias_info[ii].address,
                mem_number, alias_number, notAlias_number);
            if (mem_number > MEM_HARD_ALIAS_MAX_NUM)
            {
                mem_number = MEM_HARD_ALIAS_MAX_NUM;
            }
            for (jj=0; jj<mem_number; jj++)
            {
                mem = dump_cfg->hard_alias_info[ii].mem_list[jj];
                mem_alias = SOC_MEM_ALIAS_MAP(unit, mem);
                if (SOC_MEM_IS_VALID(unit, mem_alias))
                {
                    cli_out("    %-44s ->  %s\n", SOC_MEM_NAME(unit, mem), SOC_MEM_NAME(unit, mem_alias));
                }
                else if (SOC_MEM_IS_ALIAS(unit, mem))
                {
                    mem_alias = petra_mem_alias_to_orig(unit, mem);
                    cli_out("    %-43s *->  %s\n", SOC_MEM_NAME(unit, mem), SOC_MEM_NAME(unit, mem_alias));
                }
                else
                {
                    if (SOC_MEM_IS_ARRAY(unit, mem))
                    {
                        sal_sprintf(srt_entry_info, "%d/%d/%d/%d",
                            soc_mem_index_count(unit, mem), SOC_MEM_NUMELS(unit, mem),
                            soc_mem_entry_bytes(unit, mem), SOC_MEM_ELEM_SKIP(unit, mem));
                    }
                    else
                    {
                        sal_sprintf(srt_entry_info, "%d/%d",
                            soc_mem_index_count(unit, mem), soc_mem_entry_bytes(unit, mem));
                    }
                    cli_out("    %-48s %-18s", SOC_MEM_NAME(unit, mem), srt_entry_info);

                    /* Dynamic */
                    if (soc_mem_is_signal(unit, mem))
                    {
                        cli_out("Dynamic/");
                    }
                    /* ReadOnly */
                    if (soc_mem_is_readonly(unit, mem))
                    {
                        cli_out("ReadOnly/");
                    }
                    /* WriteOnly */
                    else if (soc_mem_is_writeonly(unit, mem))
                    {
                        cli_out("WritedOnly/");
                    }

                    /* Internal */
                    if (SOC_MEM_IS_INTERNAL(unit, mem))
                    {
                        cli_out("Internal/");
                    }
                    /* Cacheable */
                    if (soc_mem_is_cachable(unit, mem))
                    {
                        cli_out("Cacheable/");
                    }

                    /* shadowed */
                    if (soc_mem_cache_get(unit, mem, SOC_BLOCK_ALL))
                    {
                        cli_out("Shadowed/");
                    }

                    /* Protection */
                    if (SOC_MEM_FIELD_VALID(unit, mem, PARITYf) ||
                        (SOC_IS_ARADPLUS_AND_BELOW(unit) && (mem_alias == SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDSm))) 
                    {
                        cli_out("Parity/");
                    }
                    if (SOC_MEM_FIELD_VALID(unit, mem, ECCf))
                    {
                        cli_out("ECC/");
                    }
                    cli_out("\n");
                }
            }
        }
    }

    return CMD_OK;
}


STATIC int _dpp_diag_mem_address_record(int unit, soc_mem_t mem, int blk_type, uint32 address, diag_mem_dump_cfg_t* dump_cfg)
{
    uint32  ii, mem_number;
    int     idx = -1;

    for (ii=0; ii<dump_cfg->hard_addr_num; ii++)
    {
        if ((dump_cfg->hard_alias_info[ii].blk_type == blk_type) && (dump_cfg->hard_alias_info[ii].address == address))
        {
            idx = ii;
            mem_number = dump_cfg->hard_alias_info[idx].mem_number;
            if (mem_number < MEM_HARD_ALIAS_MAX_NUM)
            {
                dump_cfg->hard_alias_info[idx].mem_list[mem_number] = mem;
            }
            dump_cfg->hard_alias_info[idx].mem_number++;
            break;
        }
    }

    if (idx == -1)
    {
        idx  = dump_cfg->hard_addr_num;
        dump_cfg->hard_alias_info[idx ].address = address;
        dump_cfg->hard_alias_info[idx ].blk_type = blk_type;
        dump_cfg->hard_alias_info[idx ].mem_number = 1;
        dump_cfg->hard_alias_info[idx ].mem_list[0] = mem;
        dump_cfg->hard_addr_num++;
    }

    if (SOC_MEM_IS_VALID(unit, SOC_MEM_ALIAS_MAP(unit,mem)))
    {
        dump_cfg->hard_alias_info[idx].alias_number++;
    }
    else
    {
        dump_cfg->hard_alias_info[idx].notAlias_number++;
    }

    return 0;
}


STATIC int dpp_diag_check_mem_need_dump(int unit, soc_mem_t mem, diag_mem_dump_cfg_t* dump_cfg)
{
    uint16      block;
    int         is_mem_alias, blk_type, copyno;
    soc_mem_t   mem_alias;
    uint8       acc_type;
    uint32      address;

    is_mem_alias = 0;
    mem_alias = SOC_MEM_ALIAS_MAP(unit,mem);
    if (SOC_MEM_IS_VALID(unit, mem_alias))
    {
        is_mem_alias = 1;
    }
    else
    {
        mem_alias = mem;
    }
    block = SOC_MEM_BLOCK_MIN(unit, mem_alias);
    blk_type = SOC_BLOCK_TYPE(unit, block);
    copyno = SOC_MEM_BLOCK_ANY(unit, mem_alias);
    address = soc_mem_addr_get(unit, mem_alias, 0, copyno, 0, &acc_type);

    _dpp_diag_mem_address_record(unit, mem, blk_type, address, dump_cfg);

    /* dynamic check */
    if (soc_mem_is_signal(unit, mem_alias))
    {
        if (!(dump_cfg->mem_dump_flag & MEM_DUMP_FLAG_DYNAMIC))
        {
            return FALSE;
        }
    }
    else
    {
        if (!(dump_cfg->mem_dump_flag & MEM_DUMP_FLAG_STATIC))
        {
            return FALSE;
        }
    }


    if (soc_mem_is_readonly(unit, mem_alias))
    {
        if (!(dump_cfg->mem_dump_flag & MEM_DUMP_FLAG_READONLY))
        {
            return FALSE;
        }
    }
    else if (soc_mem_is_writeonly(unit, mem_alias))
    {
        if (!(dump_cfg->mem_dump_flag & MEM_DUMP_FLAG_WRITEONLY))
        {
            return FALSE;
        }
    }
    else
    {
        if (!(dump_cfg->mem_dump_flag & MEM_DUMP_FLAG_ACCESSABLE))
        {
            return FALSE;
        }
    }

    if (is_mem_alias)
    {
        if (!(dump_cfg->mem_dump_flag & MEM_DUMP_FLAG_SOFT_ALIAS))
        {
            return FALSE;
        }
    }
    else
    {
        if (!(dump_cfg->mem_dump_flag & MEM_DUMP_FLAG_NO_ALLAS))
        {
            return FALSE;
        }
    }

    if (dump_cfg->mem_dump_flag & MEM_DUMP_FLAG_ARRAY_ONLY)
    {
        if (!SOC_MEM_IS_ARRAY(unit, mem_alias))
        {
            return FALSE;
        }
    }

    if (dump_cfg->mem_dump_flag & MEM_DUMP_FLAG_SEVERE_ONLY)
    {
        if (SOC_MEM_SEVERITY(unit, mem_alias) != SOC_MEM_SEVERE_HIGH)
        {
            return FALSE;
        }
    }

    if (dump_cfg->mem_dump_flag & MEM_DUMP_FLAG_CACHE_ONLY)
    {
        if (!soc_mem_is_cachable(unit, mem_alias))
        {
            return FALSE;
        }
    }

    if (dump_cfg->mem_dump_flag & MEM_DUMP_FLAG_SPECIAL_BLOCK)
    {
        if (dump_cfg->block_type != blk_type)
        {
            return FALSE;
        }
    }

    if (dump_cfg->mem_dump_flag & MEM_DUMP_FLAG_SHADOWED)
    {
        if (!soc_mem_cache_get(unit, mem_alias, SOC_BLOCK_ALL))
        {
            return FALSE;
        }
    }

    return TRUE;
}



STATIC int dpp_diag_mem_info_dump(int unit, soc_mem_t mem, void* data)
{
    int                     rv = SOC_E_NONE;
    uint16                  block;
    int                     is_mem_alias, blk_type;
    char                    str_mem_info[256] = {0};
    char                    srt_entry_info[32] = {0};
    soc_mem_severe_t        severity;
    soc_mem_t               mem_alias;
    diag_mem_dump_cfg_t*    dump_cfg;
    int                     copyno;
    uint8                   acc_type;
    uint32                  address, element_skip;
    int                     index_cnt, numels, entry_bytes;

    dump_cfg = (diag_mem_dump_cfg_t*)data;
    if (!SOC_MEM_IS_VALID(unit, mem))
    {
        return rv;
    }

    if (!dpp_diag_check_mem_need_dump(unit, mem, dump_cfg))
    {
        return SOC_E_NONE;
    }

    is_mem_alias = 0;
    mem_alias = SOC_MEM_ALIAS_MAP(unit,mem);
    if (SOC_MEM_IS_VALID(unit, mem_alias))
    {
        is_mem_alias = 1;
    }
    else
    {
        mem_alias = mem;
    }
 
    /* Block */
    /* SOC_BLOCK_TYPE ?? */
    block = SOC_MEM_BLOCK_MIN(unit, mem_alias);
    blk_type = SOC_BLOCK_TYPE(unit, block);
    copyno = SOC_MEM_BLOCK_ANY(unit, mem_alias);
    address = soc_mem_addr_get(unit, mem_alias, 0, copyno, 0, &acc_type);

    /* memory info */
    if (is_mem_alias)
    {
        sal_sprintf(str_mem_info + sal_strlen(str_mem_info), "Alias->%s/", SOC_MEM_NAME(unit, mem_alias));
    }

    /* Dynamic */
    if (soc_mem_is_signal(unit, mem_alias))
    {
        sal_sprintf(str_mem_info + sal_strlen(str_mem_info), "Dynamic/");
    }
    /* ReadOnly */
    if (soc_mem_is_readonly(unit, mem_alias))
    {
        sal_sprintf(str_mem_info + sal_strlen(str_mem_info), "ReadOnly/");
    }
    /* WriteOnly */
    else if (soc_mem_is_writeonly(unit, mem_alias))
    {
        sal_sprintf(str_mem_info + sal_strlen(str_mem_info), "WritedOnly/");
    }

    /* Internal */
    if (SOC_MEM_IS_INTERNAL(unit, mem_alias))
    {
        sal_sprintf(str_mem_info + sal_strlen(str_mem_info), "Internal/");
    }
    /* Cacheable */
    if (soc_mem_is_cachable(unit, mem_alias))
    {
        sal_sprintf(str_mem_info + sal_strlen(str_mem_info), "Cacheable/");
    }
    if (soc_mem_cache_get(unit, mem_alias, SOC_BLOCK_ALL))
    {
        sal_sprintf(str_mem_info + sal_strlen(str_mem_info), "Shadowed/");
    }

    /* Protection */
    if (SOC_MEM_FIELD_VALID(unit, mem_alias, PARITYf) ||
        (SOC_IS_ARADPLUS_AND_BELOW(unit) && (mem_alias == SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDSm)))
    {
        sal_sprintf(str_mem_info + sal_strlen(str_mem_info), "Parity/");
    }
    if (SOC_MEM_FIELD_VALID(unit, mem_alias, ECCf))
    {
        sal_sprintf(str_mem_info + sal_strlen(str_mem_info), "ECC/");
    }
    /* Severity */
    severity = SOC_MEM_SEVERITY(unit, mem_alias);
    if (severity == SOC_MEM_SEVERE_HIGH)
    {
        sal_sprintf(str_mem_info + sal_strlen(str_mem_info), "Severe/");
    }
    if (SOC_MEM_IS_ARRAY(unit, mem_alias))
    {
        sal_sprintf(str_mem_info + sal_strlen(str_mem_info), "Array/");
    }

    entry_bytes = soc_mem_entry_bytes(unit, mem_alias);
    index_cnt   = soc_mem_index_count(unit, mem_alias);
    if (SOC_MEM_IS_ARRAY(unit, mem_alias))
    {
        numels       = SOC_MEM_NUMELS(unit, mem_alias);
        element_skip = SOC_MEM_ELEM_SKIP(unit, mem_alias);
        sal_sprintf(srt_entry_info, "%d/%d/%d/%d", index_cnt, numels, entry_bytes, element_skip);
    }
    else
    {
        sal_sprintf(srt_entry_info, "%d/%d", index_cnt, entry_bytes);
    }

    dump_cfg->show_idx++;
    cli_out("%-6d%-50s  %-10s%08X  %-20s%s\n", dump_cfg->show_idx,
                SOC_MEM_NAME(unit, mem),
                soc_block_name_lookup_ext(blk_type, unit),
                address, srt_entry_info, str_mem_info);
    return rv;
}


STATIC int dpp_diag_parse_block_type(char* str_block)
{
    int     ii;
    int     blktype = SOC_BLK_NONE;

    for (ii=0; soc_dpp_block_names[ii].blk != SOC_BLK_NONE; ii++)
    {
        if (sal_strcasecmp(soc_dpp_block_names[ii].name, str_block) == 0)
        {
            blktype = soc_dpp_block_names[ii].blk;
        }
    }

    return blktype;
}


STATIC cmd_result_t dpp_diag_mem_info(int unit, args_t *a)
{
    char*               delimiters = "&";
    parse_table_t       pt;
    diag_mem_dump_cfg_t*    dump_cfg;
    int                 rv = CMD_OK;
    int                 ser_tbl_info = 0;
    int                 dynamic=-1, is_static=-1;
    int                 readonly=-1, writeonly=-1, accessable=-1;
    int                 alias=-1, hard_alias=-1, no_alias=-1;
    int                 array=-1, severe=-1, cache=-1, shadow=-1, export=-1;
    int                 blktype= SOC_BLK_NONE;
    char*               str_block = NULL;

    dump_cfg = sal_alloc(sizeof(diag_mem_dump_cfg_t), "diag_mem_dump_cfg_t");
    if (dump_cfg == NULL)
    {
        cli_out("ERROR at %s %d\n", __func__, __LINE__);
        return CMD_FAIL;
    }
    sal_memset(dump_cfg, 0, sizeof(diag_mem_dump_cfg_t));

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "ser-tbl",  PQ_BOOL | PQ_NO_EQ_OPT, 0, &ser_tbl_info, NULL);
    parse_table_add(&pt, "dynamic",  PQ_BOOL | PQ_NO_EQ_OPT, 0, &dynamic, NULL);
    parse_table_add(&pt, "static",   PQ_BOOL | PQ_NO_EQ_OPT, 0, &is_static, NULL);
    parse_table_add(&pt, "readonly",  PQ_BOOL | PQ_NO_EQ_OPT, 0, &readonly, NULL);
    parse_table_add(&pt, "writeonly",   PQ_BOOL | PQ_NO_EQ_OPT, 0, &writeonly, NULL);
    parse_table_add(&pt, "accessable",  PQ_BOOL | PQ_NO_EQ_OPT, 0, &accessable, NULL);
    parse_table_add(&pt, "array",  PQ_BOOL | PQ_NO_EQ_OPT, 0, &array, NULL);
    parse_table_add(&pt, "severe",  PQ_BOOL | PQ_NO_EQ_OPT, 0, &severe, NULL);
    parse_table_add(&pt, "cache",  PQ_BOOL | PQ_NO_EQ_OPT, 0, &cache, NULL);
    parse_table_add(&pt, "shadow",  PQ_BOOL | PQ_NO_EQ_OPT, 0, &shadow, NULL);
    parse_table_add(&pt, "export",  PQ_BOOL | PQ_NO_EQ_OPT, 0, &export, NULL);

    parse_table_add(&pt, "alias", PQ_BOOL | PQ_NO_EQ_OPT, 0, &alias, NULL);
    parse_table_add(&pt, "hard-alias", PQ_BOOL | PQ_NO_EQ_OPT, 0, &hard_alias, NULL);
    parse_table_add(&pt, "no-alias", PQ_BOOL | PQ_NO_EQ_OPT, 0, &no_alias, NULL);

    parse_table_add(&pt, "block",   PQ_STRING, 0, &str_block, NULL);
    if (parse_arg_eq(a, &pt) < 0)
    {
        cli_out("%s: Invalid option: %s\n", ARG_CMD(a), ARG_CUR(a));
        sal_free(dump_cfg);
        parse_arg_eq_done(&pt);
        return CMD_FAIL;
    }

    if (ser_tbl_info)
    {
        if ((soc_mem_iterate(unit, dpp_diag_ser_tbl_info_dump, delimiters)) < 0)
        {
            rv = CMD_FAIL;
        }
    }
    else
    {
        if (str_block != NULL)
        {
            blktype = dpp_diag_parse_block_type(str_block);
        }

        if (blktype != SOC_BLK_NONE)
        {
            dump_cfg->mem_dump_flag |= MEM_DUMP_FLAG_SPECIAL_BLOCK;
            dump_cfg->block_type = blktype;
        }

        if (dynamic == 1)
        {
            dump_cfg->mem_dump_flag |= MEM_DUMP_FLAG_DYNAMIC;
        }
        else if (is_static == 1)
        {
            dump_cfg->mem_dump_flag |= MEM_DUMP_FLAG_STATIC;
        }
        else
        {
            dump_cfg->mem_dump_flag |= MEM_DUMP_FLAG_DYNAMIC;
            dump_cfg->mem_dump_flag |= MEM_DUMP_FLAG_STATIC;
        }

        if (readonly == 1)
        {
            dump_cfg->mem_dump_flag |= MEM_DUMP_FLAG_READONLY;
        }
        else if (writeonly == 1)
        {
            dump_cfg->mem_dump_flag |= MEM_DUMP_FLAG_WRITEONLY;
        }
        else if (accessable == 1)
        {
            dump_cfg->mem_dump_flag |= MEM_DUMP_FLAG_ACCESSABLE;
        }
        else
        {
            dump_cfg->mem_dump_flag |= MEM_DUMP_FLAG_READONLY;
            dump_cfg->mem_dump_flag |= MEM_DUMP_FLAG_WRITEONLY;
            dump_cfg->mem_dump_flag |= MEM_DUMP_FLAG_ACCESSABLE;
        }

        if (alias == 1)
        {
            dump_cfg->mem_dump_flag |= MEM_DUMP_FLAG_SOFT_ALIAS;
        }
        else if (hard_alias == 1)
        {
            dump_cfg->mem_dump_flag |= MEM_DUMP_FLAG_HARD_ALLAS;
        }
        else if (no_alias == 1)
        {
            dump_cfg->mem_dump_flag |= MEM_DUMP_FLAG_NO_ALLAS;
        }
        else
        {
            dump_cfg->mem_dump_flag |= MEM_DUMP_FLAG_SOFT_ALIAS;
            dump_cfg->mem_dump_flag |= MEM_DUMP_FLAG_NO_ALLAS;
        }

        if (array == 1)
        {
            dump_cfg->mem_dump_flag |= MEM_DUMP_FLAG_ARRAY_ONLY;
        }
        if (severe == 1)
        {
            dump_cfg->mem_dump_flag |= MEM_DUMP_FLAG_SEVERE_ONLY;
        }
        if (cache == 1)
        {
            dump_cfg->mem_dump_flag |= MEM_DUMP_FLAG_CACHE_ONLY;
        }
        if (shadow == 1)
        {
            dump_cfg->mem_dump_flag |= MEM_DUMP_FLAG_SHADOWED;
        }
        if (export == 1)
        {
            dump_cfg->mem_dump_flag |= MEM_DUMP_FLAG_EXPORT_MODE;
        }

        cli_out("%-6s%-50s  %-10s%-10s%-20s%s\n", "index", "MEM_NAME", "BLOCK", "Address", "", "MemInfo");
        cli_out("------------------------------------------------------------------------");
        cli_out("------------------------------------------------------------------------\n");
        if ((soc_mem_iterate(unit, dpp_diag_mem_info_dump, dump_cfg)) < 0)
        {
            rv = CMD_FAIL;
        }

        if (dump_cfg->mem_dump_flag & MEM_DUMP_FLAG_HARD_ALLAS)
        {
            dpp_diag_hard_alias_mem_info_dump(unit, dump_cfg);
        }
    }

    sal_free(dump_cfg);
    parse_arg_eq_done(&pt);
    return rv;
}


/*
    diag tcam dump bank
*/
STATIC cmd_result_t dpp_diag_tcam(int unit, args_t *a)
{
    parse_table_t       pt;
    int                 rv = CMD_OK;
    uint32              tcam_dump, bank, entry, cache=0, set_cache=0, enable, rewrite, summary;
    uint32              shadow_compare;

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "summary", PQ_BOOL | PQ_NO_EQ_OPT, (void*)0, &summary, NULL);
    parse_table_add(&pt, "shadow-compare", PQ_BOOL | PQ_NO_EQ_OPT, (void*)0, &shadow_compare, NULL);
    parse_table_add(&pt, "dump", PQ_BOOL | PQ_NO_EQ_OPT, (void*)0, &tcam_dump, NULL);
    parse_table_add(&pt, "rewrite", PQ_BOOL | PQ_NO_EQ_OPT, (void*)0, &rewrite, NULL);
    parse_table_add(&pt, "set-cache", PQ_BOOL | PQ_NO_EQ_OPT, (void*)0, &set_cache, NULL);
    parse_table_add(&pt, "cache", PQ_BOOL | PQ_NO_EQ_OPT, (void*)0, &cache, NULL);
    parse_table_add(&pt, "bank", PQ_INT, (void*)0, &bank, NULL);
    parse_table_add(&pt, "entry", PQ_INT, (void*)0xffff, &entry, NULL);
    parse_table_add(&pt, "enable", PQ_INT, (void*)2, &enable, NULL);
    
    if (parse_arg_eq(a, &pt) < 0)
    {
        cli_out("%s: Invalid option: %s\n", ARG_CMD(a), ARG_CUR(a));
        parse_arg_eq_done(&pt);
        return CMD_FAIL;
    }

    if (summary)
    {
        arad_tcam_bank_shadow_info_summary(unit);
    }
    else if (shadow_compare)
    {
        arad_tcam_shadow_content_compare_validate(unit);
    }
    else if (tcam_dump)
    {
        arad_tcam_bank_entry_shadow_info_dump(unit, bank, entry, cache);
    }
    else if (set_cache)
    {
        arad_set_tcam_cache(unit, enable);
        if ((enable !=0) && (enable != 1))
        {
            cli_out("diag tcam set-cache enable=0/1\n");
            return CMD_USAGE;
        }
    }
    else if (rewrite)
    {
        arad_tcam_bank_entry_rewrite_from_shadow(unit, bank, entry);
    }

    parse_arg_eq_done(&pt);
    return rv;
}

#endif


/*----------------------------------------------------------------------------------------------------*/
#ifdef BCM_PETRA_SUPPORT

STATIC cmd_result_t
cmd_dpp_diag_rate(int unit, args_t *a) {
   char  *option = ARG_GET(a);
   int strnlen_option;
   char  *sub_option = NULL;
   int strnlen_sub_option;
   int   port_num = -2, tc_index = -2, if_num = -2, check_bw_scheme = -2, check_bw_num = 0, flow_id = -2;
   parse_table_t  pt;
   cmd_result_t   retCode = CMD_OK;
   int mode_given = 0;
   
   /*
    * Make sure at least one of the strings is NULL terminated.
    */
   if ((NULL != option) && (!sal_strcasecmp(option, "SCH"))){
      sub_option = ARG_GET(a);
   }
   
   parse_table_init(unit, &pt);
   parse_table_add(&pt, "port", PQ_DFL | PQ_INT, &port_num, &port_num, NULL);
   parse_table_add(&pt, "tc",    PQ_DFL | PQ_INT, &tc_index, &tc_index, NULL);
   parse_table_add(&pt, "if",     PQ_DFL | PQ_INT, &if_num, &if_num, NULL);
   parse_table_add(&pt, "scheme", PQ_DFL | PQ_INT, &check_bw_scheme, &check_bw_scheme, NULL);
   parse_table_add(&pt, "bw",     PQ_DFL | PQ_INT, &check_bw_num, &check_bw_num, NULL);
   parse_table_add(&pt, "flowid", PQ_DFL | PQ_INT, &flow_id, &flow_id, NULL);
   if (0 > parse_arg_eq(a, &pt)) {
      parse_arg_eq_done(&pt);
      return CMD_FAIL;
   }


   /*determine and apply*/
   while (NULL != option) {
      /*
       * Make sure at least one of the strings is NULL terminated.
       */
      if (!sal_strcasecmp(option, "?")) {
         mode_given++;
         parse_arg_eq_done(&pt);
         return CMD_USAGE;
      }      
      /*
       * Make sure at least one of the strings is NULL terminated.
       */
      strnlen_option = sal_strnlen(option, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH);
      if (!sal_strncasecmp(option, "SCH", strnlen_option)) {
         mode_given++;
         if (NULL == sub_option) return CMD_USAGE;
         strnlen_sub_option = sal_strnlen(sub_option, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH);
         if (!sal_strncasecmp(sub_option, "PS", strnlen_sub_option)) {
            if (port_num == -2) {
               cli_out("option SCH PS requires port value\n");
               parse_arg_eq_done(&pt);
               return CMD_FAIL;
            }
            
            if (calc_sch_ps_rate(unit, port_num) != CMD_OK) retCode = CMD_FAIL;
         }
         else if (!sal_strncasecmp(sub_option, "FLOW", strnlen_sub_option)) {
            if (flow_id == -2) {
               cli_out("option SCH FLOW requires flow id\n");
               parse_arg_eq_done(&pt);
               return CMD_FAIL;
            }

            if (calc_sch_flow_bw(unit, flow_id) != CMD_OK) retCode = CMD_FAIL;
         }
         else {
            parse_arg_eq_done(&pt);
            return CMD_USAGE;            
         }
      }
      /*
       * Make sure at least one of the strings is NULL terminated.
       */
      else if (!sal_strncasecmp(option, "EGQ", strnlen_option)) {
         mode_given++;
         if (port_num == -2 || tc_index == -2) {
            cli_out("option EGQ requires port and tc values\n");
            parse_arg_eq_done(&pt);
            return CMD_FAIL;
         }
         if (calc_egq_rate(unit, port_num, tc_index) != CMD_OK) retCode = CMD_FAIL;
      } else if (!sal_strncasecmp(option, "PQP", strnlen_option)) {
         mode_given++;
         if (port_num == -2 || tc_index == -2) {
            cli_out("option PQP requires port and tc values\n");
            parse_arg_eq_done(&pt);
            return CMD_FAIL;
         }
         if (calc_pqp_rate(unit, port_num, tc_index) != CMD_OK) retCode = CMD_FAIL;
      } else if (!sal_strncasecmp(option, "EPEP", strnlen_option)) {
         mode_given++;
         if (port_num == -2 || tc_index == -2) {
            cli_out("option EPEP requires port and tc values\n");
            parse_arg_eq_done(&pt);
            return CMD_FAIL;
         }
         if (calc_epe_port_rate(unit, port_num, tc_index) != CMD_OK) retCode = CMD_FAIL;
      } else if (!sal_strncasecmp(option, "EPNI", strnlen_option)) {
         mode_given++;
         if (check_bw_scheme == -2) {
            cli_out("option EPNI requires scheme value\n");
            parse_arg_eq_done(&pt);
            return CMD_FAIL;
         }
         if (calc_epni_rate(unit, check_bw_scheme, check_bw_num) != CMD_OK) retCode = CMD_FAIL;
      } else if (!sal_strncasecmp(option, "IRE", strnlen_option)) {
         mode_given++;
         if (port_num == -2) {
            cli_out("option IRE requires port value\n");
            parse_arg_eq_done(&pt);
            return CMD_FAIL;
         }
         if (calc_ire_rate(unit, port_num) != CMD_OK) retCode = CMD_FAIL;
      } else if (!SOC_IS_QAX(unit) && !sal_strncasecmp(option, "IQM", strnlen_option)) {
         mode_given++;
         if (port_num == -2) {
            cli_out("option IQM requires port value\n");
            parse_arg_eq_done(&pt);
            return CMD_FAIL;
         }
         if (calc_iqm_rate(unit, port_num) != CMD_OK) retCode = CMD_FAIL;
      } else if (SOC_IS_QAX(unit) && !sal_strncasecmp(option, "CGM", strnlen_option)) {
         mode_given++;
         if (calc_cgm_rate(unit) != CMD_OK) retCode = CMD_FAIL;
      } 


      else if (!SOC_IS_QAX(unit) && !sal_strncasecmp(option, "IPT", strnlen_option)) {
         mode_given++;
         if (calc_ipt_rate(unit) != CMD_OK) retCode = CMD_FAIL;
      }
      else if (SOC_IS_QAX(unit) && !sal_strncasecmp(option, "PTS", strnlen_option)) {
         mode_given++;
         if (calc_pts_rate(unit) != CMD_OK) retCode = CMD_FAIL;
      }
      else if (SOC_IS_QAX(unit) && !sal_strncasecmp(option, "TXQ", strnlen_option)) {
         mode_given++;
         if (calc_txq_rate(unit) != CMD_OK) retCode = CMD_FAIL;
      }
      option = ARG_GET(a);
      if (retCode == CMD_FAIL) {
         parse_arg_eq_done(&pt);
         return CMD_FAIL;
      }
   }
   parse_arg_eq_done(&pt);
   if (mode_given == 0) {
      return CMD_USAGE;
   }
   return CMD_OK;
}

#endif /*BCM_PETRA_SUPPORT*/

STATIC cmd_result_t
cmd_dpp_diag_wb(int unit, args_t *a) {
   char  *option = ARG_GET(a);
   char  *file = NULL;
#if BCM_WARM_BOOT_SUPPORT
   char  *log = "log quiet=yes file=";
   char  log_command[256] = "";
#endif
   int   buf_id = -1, var_id = -1, engine_id = 0;
   parse_table_t  pt;
   cmd_result_t   retCode = CMD_OK;
   int mode_given = 0;

   parse_table_init(unit, &pt);
   parse_table_add(&pt, "buffer_id",PQ_DFL | PQ_INT, &buf_id, &buf_id, NULL);
   parse_table_add(&pt, "var_id",     PQ_DFL | PQ_INT, &var_id, &var_id, NULL);
   parse_table_add(&pt, "engine_id",     PQ_DFL | PQ_INT, &engine_id, &engine_id, NULL);
   parse_table_add(&pt, "file", PQ_DFL | PQ_STRING, &file, &file, NULL);
   if (0 > parse_arg_eq(a, &pt)) {
      parse_arg_eq_done(&pt);
      return CMD_FAIL;
   }


   /*determine and apply*/
   while (NULL != option) {
      /*
       * Make sure at least one of the strings is NULL terminated.
       */
      if (!sal_strcasecmp(option, "?")) {
         mode_given++;
         parse_arg_eq_done(&pt);
         return CMD_USAGE;
      }
      if (!sal_strncasecmp(option, "DUMP", sal_strnlen(option, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH))) {
#if BCM_WARM_BOOT_SUPPORT
         if ( file == NULL ) {
                file = "delete_me.txt";
         } 
         
         sal_strncat(log_command, log, sizeof(log_command) - sal_strlen(log_command) - 1);
         sal_strncat(log_command, file, sizeof(log_command) - sal_strlen(log_command) - 1);
         sh_process_command(unit,"console off");
         sh_process_command(unit,log_command);
         
         mode_given++;
         if (soc_wb_engine_dump(unit, engine_id, 0, var_id, buf_id, 0, file, "w") != CMD_OK) retCode = CMD_FAIL;
         
         sh_process_command(unit,"log off");
         sh_process_command(unit,"console on");
         
#else /*BCM_WARM_BOOT_SUPPORT*/
         cli_out("sdk is not compiled with warmboot flags\n");
         parse_arg_eq_done(&pt);
         return CMD_FAIL;
#endif /*BCM_WARM_BOOT_SUPPORT*/
     }
      option = ARG_GET(a);
      if (retCode == CMD_FAIL) {
         parse_arg_eq_done(&pt);
         return CMD_FAIL;
      }
   }
   parse_arg_eq_done(&pt);
   if (mode_given == 0) {
      return CMD_USAGE;
   }
   return CMD_OK;
}

/*----------------------------------------------------------------------------------------------------*/
#ifdef BCM_PETRA_SUPPORT
void
print_egr_congestion_usage(void) {
   char cmd_dpp_diag_egr_congestion_usage[] =
      "Usage (DIAG counter vsq):"
      "\n\tDIAGnotsics egr_congestion commands\n\t"
      "Usages:\n\t"
      "DIAG egr_congestion <parameters> ..."
      "\n\tOPTION can be:"
      "\ncurrent\t\t print current values"
      "\nmax\t\t print maximum values"
      "\nall\t\t print both current and maximum values"
      "\ncontinuous\t\t disable maximum statistics updates"
      "\nz\t\t print 0 values"
      "\nglobal\t\t print only global values"
      "\n\nAdditional options: \tport=<id> - per OTM port, queue=<id> - per queue, interface=<id> - per interface"
      "\n";

   cli_out(cmd_dpp_diag_egr_congestion_usage);
}

STATIC void
dpp_diag_print_egr_congestion_statistics(int unit,
        ARAD_EGR_CGM_CONGENSTION_STATS *stats,
        int is_max,
        int is_non_zero,
        int port,
        int queue,
        int interface,
        int global,
        int all) {
    int i, skip, first = 1;
    cli_out("\nEgress congestion %s statistics: PD-packet descriptor  DB-data buffer\n\n", is_max ? "maximum" : "current");
    skip = ((unsigned)stats->pd ||
            (unsigned)stats->db ||
            (unsigned)stats->uc_pd ||
            (unsigned)stats->mc_pd ||
            (unsigned)stats->uc_db ||
            (unsigned)stats->mc_db ||
            !is_non_zero) ? 0 : 1;
    if (!skip && (global || all)) { /* if is_non_zero skip printing zero */
        cli_out("%5u total PDs\n%5u total DBs\n%5u UC PDs\n%5u MC PDs\n%5u UC DBs\n%5u MC DBs\n",
                (unsigned)stats->pd,
                (unsigned)stats->db,
                (unsigned)stats->uc_pd,
                (unsigned)stats->mc_pd,
                (unsigned)stats->uc_db,
                (unsigned)stats->mc_db);
    }
    if (0 <= interface && interface < SOC_DPP_IMP_DEFS_GET(unit, nof_core_interfaces) && all != 1) { /* if interface was specified */
        cli_out("Per interface: number of PDs or size measured in 256B (DBs per replication) for UC, MC\nintf UC_PD MC_PD UC_DB  MC_DB\n");
        cli_out("%3u: %5u %5u %5u %6u\n", interface,
                (unsigned)stats->uc_pd_if[interface],
                (unsigned)stats->mc_pd_if[interface],
                (unsigned)stats->uc_size_256_if[interface],
                (unsigned)stats->mc_size_256_if[interface]);
    } else if (interface == -1 && all == 1) { /* else print all interfaces*/
        first = 1;
        for (i = 0; i < SOC_DPP_IMP_DEFS_GET(unit, nof_core_interfaces); ++i) {
            skip = ((unsigned)stats->uc_pd_if[i] ||
                    (unsigned)stats->mc_pd_if[i] ||
                    (unsigned)stats->uc_size_256_if[i] ||
                    (unsigned)stats->mc_size_256_if[i] ||
                    !is_non_zero) ? 0 : 1;
            if (!skip) {  /* if is_non_zero skip printing zero */
                if (first) {
                    cli_out("Per interface: number of PDs or size measured in 256B (DBs per replication) for UC, MC\nintf UC_PD MC_PD UC_DB  MC_DB\n");
                    first = 0;
                }
                cli_out("%3u: %5u %5u %5u %6u\n", i,
                        (unsigned)stats->uc_pd_if[i],
                        (unsigned)stats->mc_pd_if[i],
                        (unsigned)stats->uc_size_256_if[i],
                        (unsigned)stats->mc_size_256_if[i]);
            }
        }
    }
    if (0 <= port && port < ARAD_EGR_CGM_OTM_PORTS_NUM && all != 1) { /* if port was specified */
        cli_out("Per OTM port: number of UC PDs, MC PDs, UC DBs, MC size measured in 256B (DBs per replication)\nport UC_PD MC_PD  UC_DB  MC_DB\n");
        cli_out("%3u: %5u %5u %6u %6u\n", port,
                (unsigned)stats->uc_pd_port[port],
                (unsigned)stats->mc_pd_port[port],
                (unsigned)stats->uc_db_port[port],
                (unsigned)stats->mc_db_port[port]);
    } else if (port == -1 && all == 1) { /* else print all ports*/
        first = 1;
        for (i = 0; i < ARAD_EGR_CGM_OTM_PORTS_NUM; ++i) {
            skip = ((unsigned)stats->uc_pd_port[i] ||
                    (unsigned)stats->mc_pd_port[i] ||
                    (unsigned)stats->uc_db_port[i] ||
                    (unsigned)stats->mc_db_port[i] ||
                    !is_non_zero) ? 0 : 1;
            if (!skip) { /* if is_non_zero skip printing zero */
                if (first) {
                    cli_out("Per OTM port: number of UC PDs, MC PDs, UC DBs, MC size measured in 256B (DBs per replication)\nport UC_PD MC_PD  UC_DB  MC_DB\n");
                    first = 0;
                }
                cli_out("%3u: %5u %5u %6u %6u\n", i,
                        (unsigned)stats->uc_pd_port[i],
                        (unsigned)stats->mc_pd_port[i],
                        (unsigned)stats->uc_db_port[i],
                        (unsigned)stats->mc_db_port[i]);
            }
        }
    }
    if (0 <= queue && queue < ARAD_EGR_CGM_QUEUES_NUM && all != 1) {  /* if queue was specified */
        cli_out("Per queue (port+TC): number of UC PDs, MC PDs, UC DBs, MC size measured in 256B (DBs per replication)\nqueue UC_PD MC_PD  UC_DB  MC_DB\n");
        cli_out("%4u: %5u %5u %6u %6u\n", queue,
                (unsigned)stats->uc_pd_queue[queue],
                (unsigned)stats->mc_pd_queue[queue],
                (unsigned)stats->uc_db_queue[queue],
                (unsigned)stats->mc_db_queue[queue]);
    } else if (queue == -1 && all == 1) { /* else print all queues*/
        first = 1;
        for (i = 0; i < ARAD_EGR_CGM_QUEUES_NUM; ++i) {
            skip = ((unsigned)stats->uc_pd_queue[i] ||
                    (unsigned)stats->mc_pd_queue[i] ||
                    (unsigned)stats->uc_db_queue[i] ||
                    (unsigned)stats->mc_db_queue[i] ||
                    !is_non_zero) ? 0 : 1;
            if (!skip) { /* if is_non_zero skip printing zero */
                if (first) {
                    cli_out("Per queue (port+TC): number of UC PDs, MC PDs, UC DBs, MC size measured in 256B (DBs per replication)\nqueue UC_PD MC_PD  UC_DB  MC_DB\n");
                    first = 0;
                }
                cli_out("%4u: %5u %5u %6u %6u\n", i,
                        (unsigned)stats->uc_pd_queue[i],
                        (unsigned)stats->mc_pd_queue[i],
                        (unsigned)stats->uc_db_queue[i],
                        (unsigned)stats->mc_db_queue[i]);
            }
        }
    }
    skip = ((unsigned)stats->mc_pd_sp[0] ||
            (unsigned)stats->mc_pd_sp[1] ||
            (unsigned)stats->mc_db_sp[0] ||
            (unsigned)stats->mc_db_sp[1] ||
            !is_non_zero) ? 0 : 1;
    if (!skip && (global || all)) { /* if is_non_zero skip printing zero */
        cli_out("Per service pool: number of MC PDs and DBs\nMC_PD[0]:%u MC_PD[1]:%u  MC_DB[0]:%u  MC_DB[1]:%u\n",
                (unsigned)stats->mc_pd_sp[0],
                (unsigned)stats->mc_pd_sp[1],
                (unsigned)stats->mc_db_sp[0],
                (unsigned)stats->mc_db_sp[1]);
    }
    first = 1;
    for (i = 0; i < 16; ++i) {
        skip = ((unsigned)stats->mc_pd_sp_tc[i] ||
                (unsigned)stats->mc_db_sp_tc[i] ||
                !is_non_zero) ? 0 : 1;
        if (!skip && (global || all)) { /* if is_non_zero skip printing zero */
            if (first) {
                cli_out("Per SP per TC: number of MC SDs, MC size measured in 256B (DBs per replication)\nSP TC MC_PD MC_DB\n");
                first = 0;
            }
            cli_out("%2u%2u: %5u %5u\n", i >> 3, i % 8,
                    (unsigned)stats->mc_pd_sp_tc[i],
                    (unsigned)stats->mc_db_sp_tc[i]);
        }
    }
    if (!is_max) { /* supported only for current values and not for max values */
        skip = ((unsigned)stats->mc_rsvd_pd_sp[0] ||
                (unsigned)stats->mc_rsvd_pd_sp[1] ||
                (unsigned)stats->mc_rsvd_db_sp[0] ||
                (unsigned)stats->mc_rsvd_db_sp[1] ||
                !is_non_zero) ? 0 : 1;
        if (!skip && (global || all)) { /* if is_non_zero skip printing zero */
            cli_out("Per service pool: number of reserved MC PDs/DBs\nMC_PD[0]:%u MC_PD[1]:%u  MC_DB[0]:%u  MC_DB[1]:%u\n",
                    (unsigned)stats->mc_rsvd_pd_sp[0],
                    (unsigned)stats->mc_rsvd_pd_sp[1],
                    (unsigned)stats->mc_rsvd_db_sp[0],
                    (unsigned)stats->mc_rsvd_db_sp[1]);
        }
    }
}

STATIC cmd_result_t
dpp_diag_egr_congestion(int unit, args_t *a) {
    int rc;
    char      *option;
    int strnlen_option;
    ARAD_EGR_CGM_CONGENSTION_STATS *stats = NULL;
    unsigned char current = 1, max = 1, all = 1, non_zero = 1, global = 0; /* default options */
    uint32 port, queue, interface;
    int no_updates = 0;                 /* default option stop updates during mac value collection */
    parse_table_t  pt;
    cmd_result_t res = CMD_OK;
    int core;
    int def = -1;
    port = def; queue = def; interface = def; /* default options = -1*/
    parse_table_init(unit, &pt);
    parse_table_add(&pt, "port", PQ_DFL | PQ_INT, &def, &port, NULL);
    parse_table_add(&pt, "queue", PQ_DFL | PQ_INT, &def, &queue, NULL);
    parse_table_add(&pt, "interface", PQ_DFL | PQ_INT, &def, &interface, NULL);
    if (0 > parse_arg_eq(a, &pt)) {
        parse_arg_eq_done(&pt);
        return res;
    }
    parse_arg_eq_done(&pt);

    option = ARG_GET(a);
    while (NULL != option) {
        /*
         * Make sure at least one of the strings is NULL terminated.
         */
        strnlen_option = sal_strnlen(option, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH);
        if (!sal_strcasecmp(option, "?")) {
            print_egr_congestion_usage();
            return CMD_OK;
        } else if (!sal_strncasecmp(option, "current", strnlen_option)) {
            current = 1; max = 0;
        } else if (!sal_strncasecmp(option, "max", strnlen_option)) {
            current = 0; max = 1;
        } else if (!sal_strcasecmp(option, "all")) {
            current = 1; max = 1;
        } else if (!sal_strncasecmp(option, "continuous", strnlen_option)) {
            no_updates = 1;
        } else if (!sal_strcasecmp(option, "z")) {
            non_zero = 0;
        } else if (!sal_strncasecmp(option, "global", strnlen_option)) {
            global = 1; all = 0;
        } else {
            cli_out("argument \"%s\" not recognized\n", option);
            res = CMD_USAGE;
        }
        option = ARG_GET(a);
    }
    all = (port == -1) && (queue == -1) && (interface == -1) && all ? 1 : 0; /* print all if no port, queue or interface where specified*/
    if (!SOC_IS_ARAD(unit)) {
        cli_out("egress congestion statistics not supported for this device\n");
        res = CMD_NOTIMPL;
    }
    if (res == CMD_OK) {
        stats = sal_alloc(sizeof(ARAD_EGR_CGM_CONGENSTION_STATS), "dpp_diag_egr_congestion.stats");
        if (!stats) {
            cli_out("Memory Allocation failue\n");
            return CMD_FAIL;
        }

        SOC_DPP_CORES_ITER(SOC_CORE_ALL, core) {
            cli_out("\nCore %d:\n", core);
            cli_out("=======");
            if (current) {
                rc = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_egr_congestion_statistics_get, (unit, core, stats, 0, 0, 1));
                if (SOC_FAILURE(rc)) {
                    cli_out("\nFailed to get current statistics\n\n");
                    res = CMD_FAIL;
                } else {
                    dpp_diag_print_egr_congestion_statistics(unit,
                            stats,
                            0,
                            non_zero,
                            port,
                            queue,
                            interface,
                            global,
                            all);
                }
            }

            if (max) {
                rc = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_egr_congestion_statistics_get, (unit, core, 0, stats, 0, no_updates));
                if (SOC_FAILURE(rc)) {
                    cli_out("\nFailed to get maximum statistics\n\n");
                    res = CMD_FAIL;
                } else {
                    dpp_diag_print_egr_congestion_statistics(unit,
                            stats,
                            1,
                            non_zero,
                            port,
                            queue,
                            interface,
                            global,
                            all);
                }
            }
        }
        sal_free(stats);
    }

    return res;
}

STATIC cmd_result_t
dpp_diag_reassembly_context(int unit, args_t *a) {
    int rc;
    uint32 port;
    cmd_result_t res = CMD_OK;
    uint32 reassembly_context = 0, port_termination_context = 0;
    soc_pbmp_t valid_ports;

    if (!SOC_IS_ARAD(unit)) {
        cli_out("reassembly context not supported for this device\n");
        res = CMD_NOTIMPL;
    }
    if (res == CMD_OK) {
        rc = soc_port_sw_db_valid_ports_get(unit, 0, &valid_ports);
        if (SOC_FAILURE(rc)) {
            cli_out("\nsoc_port_sw_db_valid_ports_get failed\n\n");
            return CMD_FAIL;
        }

        cli_out(" Port | Reassembly Context | Port Termination Context\n");
        cli_out(" ----------------------------------------------------\n");
        BCM_PBMP_ITER(valid_ports, port) {
            rc = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_port_ingr_reassembly_context_get, (unit, port, &port_termination_context, &reassembly_context));
            if (SOC_FAILURE(rc)) {
                cli_out("\nFailed to get reassembly context for port %d\n\n", port);
                res = CMD_FAIL;
                break;
            }

            if (!(port_termination_context == 0xffffffff || reassembly_context == 0xffffffff)) {
                cli_out(" %3u  |         %3u        |          %3u\n", port, reassembly_context, port_termination_context);
            }
        }
    }

    return res;
}

STATIC cmd_result_t
dpp_diag_egr_calendars(int unit, args_t *a) {
    int rc;
    uint32 port;
    cmd_result_t res = CMD_OK;
    soc_pbmp_t valid_ports;

    bcm_gport_t gport;
    int mode, weight;
    char priority[5];

    uint32 high_cal_id;

    uint32 low_cal_id;

    uint32 tm_port, egress_offset = 0;
    int core;

    soc_reg_above_64_val_t tbl_data;
    uint32 e2e_offset;

    bcm_cosq_gport_info_t gport_info;
    bcm_gport_t           interface_gport;
    uint32                min_rate, egq_port_rate, egq_interface_rate, flags;

    uint32                e2e_port_rate, e2e_interface_rate;

    soc_port_if_t   nif_type;

    if (unit < 0 || unit >= BCM_LOCAL_UNITS_MAX) {
        cli_out("Invalid unit\n");
        return CMD_FAIL;
    }

    if (!SOC_IS_ARAD(unit)) {
        cli_out("egress calendars not supported for this device\n");
        res = CMD_NOTIMPL;
    }
    if (res == CMD_OK) {
        rc = soc_port_sw_db_valid_ports_get(unit, 0, &valid_ports);
        if (SOC_FAILURE(rc)) {
            cli_out("\nsoc_port_sw_db_valid_ports_get failed\n\n");
            return CMD_FAIL;
        }

        cli_out(" Port | Priority | High Calendar | Low Calendar | EGQ IF | E2E IF | EGQ Port Rate | EGQ IF Rate | E2E Port Rate | E2E IF Rate\n");
        cli_out(" ----------------------------------------------------------------------------------------------------------------------------\n");
        BCM_PBMP_ITER(valid_ports, port) {

            rc = soc_port_sw_db_interface_type_get(unit, port, &nif_type);
            if (SOC_FAILURE(rc)) {
                 return CMD_FAIL;
            }
            rc = soc_port_sw_db_flags_get(unit, port, &flags);
            if (SOC_FAILURE(rc)) {
                 return CMD_FAIL;
            }

            if (nif_type == SOC_PORT_IF_ERP || SOC_PORT_IS_VIRTUAL_RCY_INTERFACE(flags) || SOC_PORT_IS_LB_MODEM(flags)
                    || SOC_PORT_IS_ELK_INTERFACE(flags)) {
                continue;
            }

            /* Port priority */
            BCM_GPORT_LOCAL_SET(gport, port);
            rc = bcm_cosq_gport_sched_get(unit, gport, 0, &mode, &weight);
            if (SOC_FAILURE(rc)) {
                cli_out("\nFailed to get priority of port %d\n", port);
                return CMD_FAIL;
            }

            if (BCM_COSQ_SP0 == mode) {
                sal_strncpy(priority, "HIGH", sizeof(priority));
            } else if (BCM_COSQ_SP1 == mode) {
                sal_strncpy(priority, "LOW", sizeof(priority));
            }

            /* High calendar */
            rc = soc_port_sw_db_high_priority_cal_get(unit, port, &high_cal_id);
            if (SOC_FAILURE(rc)) {
                cli_out("\nFailed to get high priority calendar of port %d\n", port);
                return CMD_FAIL;
            }

            /* Low calendar */
            rc = soc_port_sw_db_low_priority_cal_get(unit, port, &low_cal_id);
            if (SOC_FAILURE(rc)) {
                cli_out("\nFailed to get low priority calendar of port %d\n", port);
                return CMD_FAIL;
            }

            /* Egress interface */
            rc = soc_port_sw_db_local_to_tm_port_get(unit, port, &tm_port, &core);
            if (SOC_FAILURE(rc)) {
                cli_out("\nFailed to get egress interface of port %d\n", port);
                return CMD_FAIL;
            }

            rc = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_port2egress_offset, (unit, core, tm_port, &egress_offset));
            if (SOC_FAILURE(rc)) {
                cli_out("\nFailed to get egress interface of port %d\n", port);
                return CMD_FAIL;
            }

            /* E2E interface */
            SOC_REG_ABOVE_64_CLEAR(tbl_data);

            rc = READ_SCH_FC_MAP_FCMm(unit, SCH_BLOCK(unit, core), egress_offset, &tbl_data);
            if (SOC_FAILURE(rc)) {
                cli_out("\nFailed to get e2e interface of port %d\n", port);
                return CMD_FAIL;
            }
            e2e_offset = soc_SCH_FC_MAP_FCMm_field32_get(unit, &tbl_data, FC_MAP_FCMf);

            /* EGQ rate */
            gport_info.in_gport = port;
            rc = bcm_cosq_gport_handle_get(unit, bcmCosqGportTypeLocalPort, &gport_info);
            if (SOC_FAILURE(rc)) {
                return CMD_FAIL;
            }
            rc = bcm_fabric_port_get(unit, gport_info.out_gport, 0, &interface_gport);
            if (SOC_FAILURE(rc)) {
                return CMD_FAIL;
            }
            rc = bcm_cosq_gport_bandwidth_get(unit, gport_info.out_gport, 0, &min_rate, &egq_port_rate, &flags);
            if (SOC_FAILURE(rc)) {
                cli_out("\nFailed to get egress port rate of port %d\n", port);
                return CMD_FAIL;
            }
            rc = bcm_cosq_gport_bandwidth_get(unit, interface_gport, 0, &min_rate, &egq_interface_rate, &flags);
            if (SOC_FAILURE(rc)) {
                cli_out("\nFailed to get egress interface rate of port %d\n", port);
                return CMD_FAIL;
            }

            /* E2E rate */
            gport_info.in_gport = port;
            rc = bcm_cosq_gport_handle_get(unit, bcmCosqGportTypeE2EPort, &gport_info);
            if (SOC_FAILURE(rc)) {
                return CMD_FAIL;
            }
            rc = bcm_fabric_port_get(unit, gport_info.out_gport, 0, &interface_gport);
            if (SOC_FAILURE(rc)) {
                return CMD_FAIL;
            }
            rc = bcm_cosq_gport_bandwidth_get(unit, gport_info.out_gport, 0, &min_rate, &e2e_port_rate, &flags);
            if (SOC_FAILURE(rc)) {
                cli_out("\nFailed to get e2e port rate of port %d\n", port);
                return CMD_FAIL;
            }
            rc = bcm_cosq_gport_bandwidth_get(unit, interface_gport, 0, &min_rate, &e2e_interface_rate, &flags);
            if (SOC_FAILURE(rc)) {
                cli_out("\nFailed to get e2e interface rate of port %d\n", port);
                return CMD_FAIL;
            }

            cli_out(" %3u  |   %4s   |       %3u     |      %3u     |  %3u   |  %3u   |   %9u   |  %9u  |   %9u   |  %9u  \n",
                    port, priority, high_cal_id, low_cal_id, egress_offset, e2e_offset, egq_port_rate, egq_interface_rate, e2e_port_rate, e2e_interface_rate);
        }
    }

    return res;
}

#ifdef BCM_JERICHO_SUPPORT
STATIC
cmd_result_t
_cmd_dpp_center_num2str_get(uint32 num, uint32 width, char *s)
{
    int length = 0, i = 0;
    char str[11] = "";

    if (s == NULL) {
        return CMD_FAIL;
    }

    sal_sprintf(str, "%u", num);
    length = sal_strlen(str);

    for (i = 0; i < (width - length) / 2; ++i) {
        sal_sprintf(&s[i], " ");
    }
    sal_strcat(s, str);
    for (i += length; i < width; ++i) {
        sal_sprintf(&s[i], " ");
    }

    return CMD_OK;
}

STATIC void
print_resource_allocation_usage(void) {
   char cmd_dpp_diag_resource_allocation_usage[] =
      "Usage (DIAG resource_allocation):"
      "\n\tDIAGnotsics RESOURCE_allocation type=options\n"
      "\n\tOPTION can be:\n"
      "\t\tsrc - show source based VSQs resource allocation.\n"
      "\t\tdst - show destination based VSQs resource allocation.\n"
      "\n";

   cli_out(cmd_dpp_diag_resource_allocation_usage);
}

STATIC cmd_result_t
dpp_diag_resource_allocation(int unit, args_t *a)
{
    int rc;
    cmd_result_t res = CMD_OK;
    char *type = NULL;
    parse_table_t pt;

    if (unit < 0 || unit >= BCM_LOCAL_UNITS_MAX) {
        cli_out("Invalid unit\n");
        return CMD_FAIL;
    }

    if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
        cli_out("Resource allocation is not supported for this device\n");
        res = CMD_NOTIMPL;
    }

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "type", PQ_STRING, "src", &type, NULL);

    if (0 > parse_arg_eq(a, &pt)) {
        cli_out("\nFailed to get type of resource allocation\n\n");
        print_resource_allocation_usage();
        res = CMD_FAIL;
    }

    if (res == CMD_OK) {
        int strnlen_type;
        /*
         * Make sure at least one of the strings is NULL terminated.
         */
        strnlen_type = sal_strnlen(type, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH);
        if (!sal_strncasecmp(type, "src", strnlen_type)) {
            SOC_TMC_ITM_INGRESS_CONGESTION_MGMT ingress_congestion_mgmt;
            uint32 dram_not_used = 0, ocb_not_used = 0, not_used = 0;
            int core_index;
            uint32 width = (SOC_IS_QAX(unit)) ? 10 : 7; /* QAX/QUX total byes have 16B resolution */

            cli_out("VSQ source based global resource allocation\n");
            cli_out("===========================================\n\n");

            BCM_DPP_CORES_ITER(BCM_CORE_ALL, core_index) {
                char str_total_memory[3][11] = {{0}},
                     str_pool_0_size[3][11] = {{0}},
                     str_pool_1_size[3][11] = {{0}},
                     str_headroom_size[3][11] = {{0}},
                     str_nominal_headroom_size[3][11] = {{0}},
                     str_reserved_size[3][11] = {{0}},
                     str_not_used_size[3][11] = {{0}};

                sal_memset(&ingress_congestion_mgmt, 0, sizeof(ingress_congestion_mgmt));

                rc = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_itm_global_resource_allocation_get, (unit, core_index, &ingress_congestion_mgmt));
                if (BCM_FAILURE(rc)) {
                    return CMD_FAIL;
                }
                
                if (SOC_IS_QAX(unit)) {
                    SOC_TMC_INGRESS_THRESHOLD_TYPE_E rsrc_type = SOC_TMC_INGRESS_THRESHOLD_INVALID;
                    for (rsrc_type = 0; rsrc_type < SOC_TMC_INGRESS_THRESHOLD_NOF_TYPES; ++rsrc_type) {
                        not_used = ingress_congestion_mgmt.global[rsrc_type].total -
                            ingress_congestion_mgmt.global[rsrc_type].headroom -
                            ingress_congestion_mgmt.global[rsrc_type].pool_0 -
                            ingress_congestion_mgmt.global[rsrc_type].pool_1 -
                            ingress_congestion_mgmt.global[rsrc_type].reserved;

                        _cmd_dpp_center_num2str_get(ingress_congestion_mgmt.global[rsrc_type].nominal_headroom, width, str_nominal_headroom_size[rsrc_type]);
                        _cmd_dpp_center_num2str_get(ingress_congestion_mgmt.global[rsrc_type].headroom, width, str_headroom_size[rsrc_type]);
                        _cmd_dpp_center_num2str_get(ingress_congestion_mgmt.global[rsrc_type].pool_0, width, str_pool_0_size[rsrc_type]);
                        _cmd_dpp_center_num2str_get(ingress_congestion_mgmt.global[rsrc_type].pool_1, width, str_pool_1_size[rsrc_type]);
                        _cmd_dpp_center_num2str_get(ingress_congestion_mgmt.global[rsrc_type].reserved, width, str_reserved_size[rsrc_type]);
                        _cmd_dpp_center_num2str_get(ingress_congestion_mgmt.global[rsrc_type].total, width, str_total_memory[rsrc_type]);
                        _cmd_dpp_center_num2str_get(not_used, width, str_not_used_size[rsrc_type]);
                    }

                    cli_out("CORE %d:\n", core_index);
                    cli_out("-------\n");
                    cli_out("                      Words             SRAM-Buffers          SRAM-PDS\n");
                    cli_out("                  =============        =============        =============\n");
                    cli_out(" Nominal Headroom | %s |        | %s |        | %s |\n", str_nominal_headroom_size[0], str_nominal_headroom_size[1], str_nominal_headroom_size[2]);
                    cli_out("                  +-----------+        +-----------+        +-----------+\n");
                    cli_out(" (Max Headroom)   | %s |        | %s |        | %s |\n", str_headroom_size[0], str_headroom_size[1], str_headroom_size[2]);
                    cli_out("                  +-----------+        +-----------+        +-----------+\n");
                    cli_out(" Pool 1           | %s |        | %s |        | %s |\n", str_pool_1_size[0], str_pool_1_size[1], str_pool_1_size[2]);
                    cli_out("                  +-----------+        +-----------+        +-----------+\n");
                    cli_out(" Pool 0           | %s |        | %s |        | %s |\n", str_pool_0_size[0], str_pool_0_size[1], str_pool_0_size[2]);
                    cli_out("                  +-----------+        +-----------+        +-----------+\n");
                    cli_out(" Reserved         | %s |        | %s |        | %s |\n", str_reserved_size[0], str_reserved_size[1], str_reserved_size[2]);
                    cli_out("                  +-----------+        +-----------+        +-----------+\n");
                    cli_out(" Unused Memory    | %s |        | %s |        | %s |\n", str_not_used_size[0], str_not_used_size[1], str_not_used_size[2]);
                    cli_out("                  +-----------+        +-----------+        +-----------+\n");
                    cli_out(" Total Memory     # %s #        # %s #        # %s #\n\n", str_total_memory[0], str_total_memory[1], str_total_memory[2]);
                } else {

                    dram_not_used = ingress_congestion_mgmt.dram.total - 
                        ingress_congestion_mgmt.dram.pool_0 - 
                        ingress_congestion_mgmt.dram.pool_1 - 
                        ingress_congestion_mgmt.dram.reserved - 
                        ingress_congestion_mgmt.dram.headroom;
                    ocb_not_used = ingress_congestion_mgmt.ocb.total - 
                        ingress_congestion_mgmt.ocb.pool_0 - 
                        ingress_congestion_mgmt.ocb.pool_1 - 
                        ingress_congestion_mgmt.ocb.reserved - 
                        ingress_congestion_mgmt.ocb.headroom;

                    _cmd_dpp_center_num2str_get(ingress_congestion_mgmt.dram.headroom, width, str_headroom_size[0]);
                    _cmd_dpp_center_num2str_get(ingress_congestion_mgmt.ocb.headroom, width, str_headroom_size[1]);
                    _cmd_dpp_center_num2str_get(ingress_congestion_mgmt.dram.pool_0, width, str_pool_0_size[0]);
                    _cmd_dpp_center_num2str_get(ingress_congestion_mgmt.ocb.pool_0, width, str_pool_0_size[1]);
                    _cmd_dpp_center_num2str_get(ingress_congestion_mgmt.dram.pool_1, width, str_pool_1_size[0]);
                    _cmd_dpp_center_num2str_get(ingress_congestion_mgmt.ocb.pool_1, width, str_pool_1_size[1]);
                    _cmd_dpp_center_num2str_get(ingress_congestion_mgmt.dram.reserved, width, str_reserved_size[0]);
                    _cmd_dpp_center_num2str_get(ingress_congestion_mgmt.ocb.reserved, width, str_reserved_size[1]);
                    _cmd_dpp_center_num2str_get(ingress_congestion_mgmt.dram.total, width, str_total_memory[0]);
                    _cmd_dpp_center_num2str_get(ingress_congestion_mgmt.ocb.total, width, str_total_memory[1]);
                    _cmd_dpp_center_num2str_get(dram_not_used, width, str_not_used_size[0]);
                    _cmd_dpp_center_num2str_get(ocb_not_used, width, str_not_used_size[1]);

                    cli_out("CORE %d:\n", core_index);
                    cli_out("-------\n");
                    cli_out("               DRAM Mixed          OCB Only\n");
                    cli_out("               ==========          ========\n");
                    cli_out(" Headroom      | %s |        | %s |\n", str_headroom_size[0], str_headroom_size[1]);
                    cli_out("               +---------+        +---------+\n");
                    cli_out(" Pool 1        | %s |        | %s |\n", str_pool_1_size[0], str_pool_1_size[1]);
                    cli_out("               +---------+        +---------+\n");
                    cli_out(" Pool 0        | %s |        | %s |\n", str_pool_0_size[0], str_pool_0_size[1]);
                    cli_out("               +---------+        +---------+\n");
                    cli_out(" Reserved      | %s |        | %s |\n", str_reserved_size[0], str_reserved_size[1]);
                    cli_out("               +---------+        +---------+\n");
                    cli_out(" Unused Memory | %s |        | %s |\n", str_not_used_size[0], str_not_used_size[1]);
                    cli_out("               +---------+        +---------+\n");
                    cli_out(" Total Memory  # %s #        # %s #\n\n", str_total_memory[0], str_total_memory[1]);
                }
            }
        /*
         * Make sure at least one of the strings is NULL terminated.
         */
        } else if (!sal_strncasecmp(type, "dst", strnlen_type)) {
            cli_out("Resource allocation for destination based VSQs is not yet implemented.\n");
            res = CMD_NOTIMPL;
        } else {
            print_resource_allocation_usage();
            res = CMD_OK;
        }
    }

    parse_arg_eq_done(&pt);

    return res;
}
#endif

#if ARAD_DEBUG
STATIC cmd_result_t
dpp_diag_dump_signals(int unit, args_t *a) {
   cmd_result_t res = CMD_OK;
   char* option;
   char* filename;
   parse_table_t    pt;
   int              core=0;
   uint32 sand_res;

   if (!SOC_IS_ARAD(unit)) {
      cli_out("egress congestion statistics not supported for this device\n");
      res = CMD_NOTIMPL;
   }
   option = ARG_CUR(a);

   /*
    * Make sure at least one of the strings is NULL terminated.
    */
   if (option && (!sal_strncasecmp(option,"parse", sal_strnlen(option, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH)))) {
       ARG_NEXT(a);
       filename =  ARG_GET(a);
       parse_table_init(unit, &pt);
       parse_table_add(&pt, "core", PQ_DFL | PQ_INT, 0, &core, NULL);

       if (0 > parse_arg_eq(a, &pt)) {
           cli_out("\nFailed to get core in cosq flow and up statistics\n\n");
           res = CMD_FAIL;
       } else {
           if(core < 0 || core >= SOC_DPP_DEFS_GET(unit, nof_cores)){
                cli_out("error ilegal core ID for device\n");
                parse_arg_eq_done(&pt);
                return CMD_FAIL;
            }

           res = cmd_dpp_diag_parsed_signals_get(unit, core, filename);
       }
   } 
   else /*diag dump_signals*/
   {
       parse_table_init(unit, &pt);
       parse_table_add(&pt, "core", PQ_DFL | PQ_INT, 0, &core, NULL);

       if (0 > parse_arg_eq(a, &pt)) {
           cli_out("\nFailed to get core in cosq flow and up statistics\n\n");
           res = CMD_FAIL;
       } else {
           if(core < 0 || core >= SOC_DPP_DEFS_GET(unit, nof_cores)){
                cli_out("error ilegal core ID for device\n");
                parse_arg_eq_done(&pt);
                return CMD_FAIL;
            }

           
           sand_res = arad_diag_signals_dump(unit, core, 0);
           if (SOC_SAND_FAILURE(sand_res))
           {
               cli_out("diag signals dump failed \n");
               parse_arg_eq_done(&pt);
               return CMD_FAIL;
           }
       }
    }

    if (SOC_FAILURE(res)) {
        cli_out("\nFailed to getcosq flow and up statistics\n\n");
        res = CMD_FAIL;
    }

    parse_arg_eq_done(&pt);
    return res;
}
#endif

STATIC void
dpp_diag_print_ing_congestion_statistics(int unit, SOC_TMC_ITM_CGM_CONGENSTION_STATS *stats) {

   if (SOC_IS_QAX(unit)) {
       cli_out("Free DRAM BDBs: %d\n", stats->bdb_free); 
       cli_out("Free minimum DRAM BDBs: %d\n", stats->min_bdb_free); 
       cli_out("Free SRAM free buffers: %d\n", stats->sram_buf_free); 
       cli_out("Free minimum SRAM free buffers: %d\n", stats->sram_buf_min_free); 
       cli_out("Free SRAM PDBs: %d\n", stats->sram_pdbs_free); 
       cli_out("Free minimum SRAM PDBs: %d\n", stats->sram_pdbs_min_free); 
   } else {
       if (!SOC_IS_JERICHO(unit)) {
           cli_out("Free BDBs: %d\n", stats->bdb_free); 
       }
       cli_out("Occupied BDs: %d\n", stats->bd_occupied);
       cli_out("Free BDs: %d\n", stats->bd2_free);
       if (!SOC_IS_JERICHO(unit)) {
           cli_out("Occupied Unicast Type Dbuffs: %d\n", stats->db_uni_occupied); 
           cli_out("Free Unicast Type Dbuffs: %d\n", stats->db_uni_free);
       }
       cli_out("Free Full-Multicast Type Dbuffs: %d\n", stats->db_full_mul_free);
       cli_out("Free Mini-Multicast Type Dbuffs: %d\n", stats->db_mini_mul_free);

       cli_out("Free BDBs minumum occupancy indication: %d\n", stats->free_bdb_mini_occu);
       if (!SOC_IS_JERICHO(unit)) {
           cli_out("Free Unicast Type Dbuffs minimal occupancy level: %d\n", stats->free_db_uni_mini_occu); 
       }
       cli_out("Free Full-Multicast Type Dbuffs minimal occupancy level: %d\n", stats->free_bdb_full_mul_mini_occu);
       cli_out("Free Mini-Multicast Type Dbuffs minimal occupancy level: %d\n", stats->free_bdb_mini_mul_mini_occu);
   }

}


STATIC cmd_result_t
dpp_diag_ing_congestion(int unit, args_t *a) {
   uint32 ret = 0;
   int core;
   SOC_TMC_ITM_CGM_CONGENSTION_STATS stats = {0};

   cmd_result_t res = CMD_OK;
   cli_out("\nIngress congestion statistics: \n\n");
   BCM_DPP_CORES_ITER(BCM_CORE_ALL, core) {
       ret = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_itm_congestion_statistics_get, (unit, core, &stats));
       if (ret != SOC_E_NONE) {
          cli_out("\nFailed to get current statistics\n\n");
          res = CMD_FAIL;
          break;
       } else {
          cli_out("CORE %d:\n-----------", core);
          dpp_diag_print_ing_congestion_statistics(unit, &stats);
       }
   }
   return res;
}

    STATIC cmd_result_t
dpp_diag_last_packet_info(int unit, args_t *a) {
   uint32 res = CMD_OK;
   unsigned i;
   SOC_TMC_DIAG_LAST_PACKET_INFO info;
   parse_table_t  pt;
   int prm_core = -1;
   int core, first_core, last_core;


   
   parse_table_init(unit, &pt);
   parse_table_add(&pt, "core", PQ_DFL | PQ_INT, 0, &prm_core, NULL);
   if (0 > parse_arg_eq(a, &pt)) {
      parse_arg_eq_done(&pt);
      return res;
   }
   parse_arg_eq_done(&pt);

   if (!SOC_IS_DPP(unit)) {
      cli_out("last packet information not supported for this device\n");
      return CMD_NOTIMPL;
   }
   if ((prm_core < -1 ) || 
       (prm_core >= SOC_DPP_DEFS_GET(unit, nof_cores))) {

        cli_out("error ilegal core ID for device\n");
        return CMD_FAIL;
   } 
   if (prm_core == -1) {
       first_core = 0;
       last_core = SOC_DPP_DEFS_GET(unit, nof_cores);
   } else {
       first_core = prm_core;
       last_core = prm_core+1;
   }

   for (core=first_core; core < last_core; core++) {
       cli_out("\n%s\n",get_core_str(unit,core));
       res = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_diag_last_packet_info_get, (unit, core,&info));
       if (soc_sand_get_error_code_from_error_word(res) != SOC_SAND_OK) {
          cli_out("\nFailed to get the last packet information\n\n");
          return CMD_FAIL;
       }

       /* print the retrieved information */
        /*
         * COVERITY
         *
         * The variable info is assigned inside MBCM_DPP_DRIVER_CALL.
         */
        /* coverity[uninit_use_in_call] */
       cli_out("Last packet information: is_valid=%u  tm_port=%u\npp_port=%lu  src_syst_port=%lu  port_header_type=%s packet_size=%d\n",
               (unsigned)info.is_valid, (unsigned)info.tm_port, (unsigned long)info.pp_port,
               (unsigned long)info.src_syst_port, SOC_TMC_PORT_HEADER_TYPE_to_string(info.port_header_type), info.packet_size);
       cli_out("Packet start, offset in bytes:");
       for (i = 0; i < SOC_TMC_DIAG_LAST_PCKT_SNAPSHOT_LEN_BYTES_MAX; ++i) {
          if (!(i % 4)) { /* every 4th byte */
             if (!(i % 32)) { /* every 32nd byte */
                cli_out("\n%.2x: ", i);
             } else {
                cli_out(" ");
             }
          }
          cli_out("%2.2x", info.buffer[i]);
       }
   }

   cli_out("\n");

   return CMD_OK;
}

/*************************************** 
* Writes last packet information 
* to info parameter
 */
    cmd_result_t
dpp_diag_last_packet_info_get(int unit, int core, SOC_TMC_DIAG_LAST_PACKET_INFO *info) {
   uint32 res = CMD_OK;
   
   if (!SOC_IS_DPP(unit)) {
      cli_out("last packet information not supported for this device\n");
      return CMD_NOTIMPL;
   }

   /*The variable info is assigned inside MBCM_DPP_DRIVER_CALL*/
   res = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_diag_last_packet_info_get, (unit, core, info));
   if (soc_sand_get_error_code_from_error_word(res) != SOC_SAND_OK) {
      cli_out("\nFailed to get the last packet information\n\n");
      return CMD_FAIL;
   }

   return CMD_OK;
}


typedef struct iqm_most_congested_queue_s {
   uint32 queue_id;
   uint32 size;
} iqm_most_congested_queue_t;

STATIC
void iqm_add_to_most_congested_array(iqm_most_congested_queue_t *arr, int length, uint32 queue_id, uint32 current_queue_size) {
   int i;
   int arr_index_of_min = 0;
   uint32 value_of_min = arr->size;
   for (i = 0; i < length; i++) {
      if ((arr + i)->size < value_of_min) {
         arr_index_of_min = i;
         value_of_min = (arr + i)->size;
      }
   }
   if (value_of_min < current_queue_size) {
      (arr + arr_index_of_min)->queue_id = queue_id;
      (arr + arr_index_of_min)->size = current_queue_size;
   }
}

STATIC
void iqm_most_congested_array_init(iqm_most_congested_queue_t *arr, int length) {
   int i;
   for (i = 0; i < length; i++) {
      (arr + i)->queue_id = 0;
      (arr + i)->size = 0;
   }
}

#define NUM_OF_QUEUES_PER_ITERATION 1024

STATIC cmd_result_t
dpp_diag_voq_non_empty_print(int unit, int core, uint32 specific_queue,uint32 is_flow_also,uint32 num_most_congested,uint32 poll_nof_times,uint32 poll_delay) {
    uint32 i=0,j=0,k=0;
    soc_error_t rc;
    cmd_result_t result;
    uint32 nof_queues_filled=0,next_queue=0,reached_end=0,queue_to_read_from=0;
    int total_printed;
    soc_ips_queue_info_t* queues;
    iqm_most_congested_queue_t* most_congested_arr = NULL;

    /*alloc memory for queues and most congested*/
    queues=(soc_ips_queue_info_t*)sal_alloc(NUM_OF_QUEUES_PER_ITERATION*sizeof(soc_ips_queue_info_t), "Non empty queues");
    if(queues == NULL) {
        cli_out("\nFailed to alloc memory for non empty queues");
        return CMD_FAIL;
    }
    if (num_most_congested)
    {
        most_congested_arr=(iqm_most_congested_queue_t*)sal_alloc(num_most_congested*sizeof(iqm_most_congested_queue_t), "Most congested");
        if(most_congested_arr == NULL) {
            sal_free(queues);
            cli_out("\nFailed to alloc memory for most congested queues");
            return CMD_FAIL;
        }
        /*initialize array*/
        iqm_most_congested_array_init(most_congested_arr,num_most_congested);
    }
     
    for (j=0;j<poll_nof_times;j++)
    {
        if (j>0) {                                  /*first iteration won't sleep*/
            sal_msleep(poll_delay);
        }

        queue_to_read_from = 0;                       /*reading from the queues from beginning*/
        reached_end = 0; 
        total_printed = 0;       
        
        if(specific_queue != SAL_UINT32_MAX) {
           queue_to_read_from = specific_queue;             
        }

        while(!reached_end)
        {
        
            /*Get queues information*/
            rc = soc_dpp_cosq_non_empty_queues_info_get(
                   unit,
                   core,
                   queue_to_read_from,
                   NUM_OF_QUEUES_PER_ITERATION,
                   queues,
                   &nof_queues_filled,
                   &next_queue,
                   &reached_end);
            if (SOC_FAILURE(rc)) {
                sal_free(queues);
                if (most_congested_arr != NULL)
                {
                    sal_free(most_congested_arr);
                }
                cli_out("\nsoc_dpp_cosq_non_empty_queues_print failed!");
                return CMD_FAIL;
            }

            queue_to_read_from=next_queue;                  /* next queue to read from */

            /*print queues*/
            for (i=0;i<nof_queues_filled;i++)
            {
                if(specific_queue != SAL_UINT32_MAX) {
                    if(specific_queue == queues[i].queue_id ) {
                        cli_out("VOQ %d holds %dB.\n", queues[i].queue_id, queues[i].queue_byte_size);
                    }
                    break; /*specific queue can apear only in slot 0*/
                } else if (!num_most_congested) { /* printing the queues*/ 
                      if(total_printed == 0) {                      /* printing headline before queues*/
                          cli_out("Ingress VOQs Sizes (format: [queue_id(queue_size)]):");   
                      }

                      if(total_printed%4==0) {
                          cli_out("\n");
                      }

                      cli_out("[%d(%dB)]\t", queues[i].queue_id, queues[i].queue_byte_size);
                      total_printed++;

                } else {
                      iqm_add_to_most_congested_array(most_congested_arr, num_most_congested, queues[i].queue_id, queues[i].queue_byte_size); /* calculating most congested queues*/
                }
            }
        
        }

        if (total_printed) { 
            cli_out("\n");
        }

        /*print most congested*/
        total_printed = 0;
        for (k = 0; k < num_most_congested; k++) { /*printing the most congested queues*/
            if (most_congested_arr[k].size != 0) {
                if (total_printed==0) {
                    cli_out("Ingress VOQs Sizes (format: [queue_id(queue_size)]):");
                }
                if (total_printed % 4 == 0) {
                    cli_out("\n");
                }
                cli_out("[%d(%dB)]\t", most_congested_arr[k].queue_id, most_congested_arr[k].size);
                total_printed++;
            }
        }

        if (total_printed) { 
            cli_out("\n");
        }

        /*print flow and up*/
        if(is_flow_also && nof_queues_filled > 0) {
            if (queues[nof_queues_filled-1].got_flow_info) {
              cli_out("Queue ID %d is connected to Flow ID %d.   \n\r"
                      " Following, Flow status flow level and up:\n\r"
                      "------------------------------------------\n\r",
                      queues[nof_queues_filled-1].queue_id, queues[nof_queues_filled-1].target_flow_id
                      );

              result = dpp_diag_cosq_flow_and_up_print(
                 unit,
                 core,
                 TRUE,
                 queues[nof_queues_filled-1].target_flow_id,
                 FALSE,
                 FALSE
                 );
              if (result != CMD_OK)
              {
                  sal_free(queues);
                  if (most_congested_arr != NULL)
                  {
                      sal_free(most_congested_arr);
                  }
                  return result;
              }
            }
        }
        
    }

    sal_free(queues);
    if (most_congested_arr != NULL)
    {
        sal_free(most_congested_arr);
    }

    return CMD_OK;
}

void
  diag_dpp_voq_credit_request_class_print(
    int mode
  )
{
   cli_out("Credit request type: ");
   switch(mode)
   {
      case BCM_COSQ_DELAY_TOLERANCE_NORMAL:
         cli_out("BCM_COSQ_DELAY_TOLERANCE_NORMAL Delay tolerance is normal\n");
         break;
      case BCM_COSQ_DELAY_TOLERANCE_LOW_DELAY:
         cli_out("BCM_COSQ_DELAY_TOLERANCE_LOW_DELAY Delay tolerance is low delay\n");
         break;
      case BCM_COSQ_DELAY_TOLERANCE_02:
      case BCM_COSQ_DELAY_TOLERANCE_03:
      case BCM_COSQ_DELAY_TOLERANCE_04:        
      case BCM_COSQ_DELAY_TOLERANCE_05:
      case BCM_COSQ_DELAY_TOLERANCE_06:
      case BCM_COSQ_DELAY_TOLERANCE_07:
      case BCM_COSQ_DELAY_TOLERANCE_08:
      case BCM_COSQ_DELAY_TOLERANCE_09:
      case BCM_COSQ_DELAY_TOLERANCE_10:
      case BCM_COSQ_DELAY_TOLERANCE_11:
      case BCM_COSQ_DELAY_TOLERANCE_12:
      case BCM_COSQ_DELAY_TOLERANCE_13:        
      case BCM_COSQ_DELAY_TOLERANCE_14:
      case BCM_COSQ_DELAY_TOLERANCE_15:
         cli_out("BCM_COSQ_DELAY_TOLERANCE_(%.2d) Flexible delay tolerance level\n", mode - BCM_COSQ_DELAY_TOLERANCE_NORMAL);
         break;        
      case BCM_COSQ_DELAY_TOLERANCE_10G_SLOW_ENABLED:
         cli_out("BCM_COSQ_DELAY_TOLERANCE_10G_SLOW_ENABLED Adjusted for slow enabled 10Gb ports\n");
         break;
      case BCM_COSQ_DELAY_TOLERANCE_10G_LOW_DELAY:
         cli_out("BCM_COSQ_DELAY_TOLERANCE_10G_LOW_DELAY Adjusted for low delay 10Gb ports\n");
         break;
      case BCM_COSQ_DELAY_TOLERANCE_1G:
         cli_out("BCM_COSQ_DELAY_TOLERANCE_1G Adjusted for 1Gb ports\n");
         break;
      case BCM_COSQ_DELAY_TOLERANCE_40G_SLOW_ENABLED:
         cli_out("BCM_COSQ_DELAY_TOLERANCE_40G_SLOW_ENABLED Adjusted for slow enabled 40Gb ports\n");
         break;
      case BCM_COSQ_DELAY_TOLERANCE_40G_LOW_DELAY:
         cli_out("BCM_COSQ_DELAY_TOLERANCE_40G_LOW_DELAY Adjusted for low delay 40Gb ports\n");
         break;
      case BCM_COSQ_DELAY_TOLERANCE_100G_SLOW_ENABLED:
         cli_out("BCM_COSQ_DELAY_TOLERANCE_100G_SLOW_ENABLED Adjusted for slow enabled 100Gb ports\n");
         break;
      case BCM_COSQ_DELAY_TOLERANCE_100G_LOW_DELAY:
         cli_out("BCM_COSQ_DELAY_TOLERANCE_100G_LOW_DELAY Adjusted for low delay 100Gb ports\n");
         break;
      case BCM_COSQ_DELAY_TOLERANCE_200G_SLOW_ENABLED:
         cli_out("BCM_COSQ_DELAY_TOLERANCE_200G_SLOW_ENABLED Adjusted for slow enabled 200Gb ports\n");
         break;
      case BCM_COSQ_DELAY_TOLERANCE_200G_LOW_DELAY:
         cli_out("BCM_COSQ_DELAY_TOLERANCE_200G_LOW_DELAY Adjusted for low delay 200Gb ports\n");
         break;
      case BCM_COSQ_DELAY_TOLERANCE_SET_COMMON_STATUS_MSG:
         cli_out("BCM_COSQ_DELAY_TOLERANCE_SET_COMMON_STATUS_MSG common status message mode\n");
         break;
      default:
         cli_out("Unknown delay tolerance\n\r");
         break;
   }
}

void
  diag_dpp_voq_credit_request_threshold_print(
    int unit, 
    bcm_cosq_delay_tolerance_t *dt
  )
{
    int slow_level;
   /* 2.b Credit watchdog message time,
               Delete time 
               Credit-Balance-Thresholds,
               Empty-Queue-Credit-Balance-Thresholds,
               Queue-Size-Thresholds */
                    
   cli_out("\tCredit watchdog message time: %d\n", dt->credit_request_watchdog_status_msg_gen);
   cli_out("\tDelete queue time: %d\n", dt->credit_request_watchdog_delete_queue_thresh);
    
   cli_out("\tBackoff enter queue credit balance threshold: %d\n", dt->credit_request_satisfied_backoff_enter_thresh);
   cli_out("\tBackoff exit queue credit balance threshold: %d\n", dt->credit_request_satisfied_backoff_exit_thresh);
   cli_out("\tBacklog enter queue credit balance threshold: %d\n", dt->credit_request_satisfied_backlog_enter_thresh);
   cli_out("\tBacklog exit queue credit balance threshold: %d\n", dt->credit_request_satisfied_backlog_enter_thresh);
    
   cli_out("\tEmpty queue satisfied credit balance threshold: %d\n", dt->credit_request_satisfied_empty_queue_thresh);   
   cli_out("\tMax empty queue credit balance threshold: %d\n", dt->credit_request_satisfied_empty_queue_max_balance_thresh);     
   cli_out("\tExceed max empty queue credit balance threshold: %d\n", dt->credit_request_satisfied_empty_queue_exceed_thresh);   
    
   cli_out("\tOff-To-Slow credit balance threshold: %d\n", dt->credit_request_hungry_off_to_slow_thresh);    
   cli_out("\tOff-To-Normal credit balance threshold: %d\n", dt->credit_request_hungry_off_to_normal_thresh);    
   cli_out("\tSlow-To-Normal credit balance threshold: %d\n", dt->credit_request_hungry_slow_to_normal_thresh);      
   cli_out("\tNormal-To-Slow credit balance threshold: %d\n", dt->credit_request_hungry_normal_to_slow_thresh);

   if (SOC_IS_JERICHO(unit)) {

        if (dt->flags && BCM_COSQ_DELAY_TOLERANCE_TOLERANCE_OCB_ONLY == BCM_COSQ_DELAY_TOLERANCE_TOLERANCE_OCB_ONLY)
            cli_out("\tDelay Tolerance is OCB only\n");
        else
            cli_out("\tDelay Tolerance is NOT OCB only\n");
        if (dt->flags && BCM_COSQ_DELAY_TOLERANCE_HIGH_Q_PRIORITY == BCM_COSQ_DELAY_TOLERANCE_HIGH_Q_PRIORITY)
            cli_out("\tDelay Tolerance is High Q Priority\n");
        else
            cli_out("\tDelay Tolerance is NOT High Q Priority\n");
        for (slow_level = 0; slow_level < BCM_COSQ_DELAY_TOLERANCE_SLOW_LEVELS; slow_level++) {
            cli_out("\tSlow Level Thresh Down in slow level number: %d is: %d\n",slow_level ,dt->slow_level_thresh_down[slow_level]);
        }
        for (slow_level = 0; slow_level < BCM_COSQ_DELAY_TOLERANCE_SLOW_LEVELS; slow_level++) {
            cli_out("\tSlow Level Thresh Up in slow level number: %d is: %d\n",slow_level ,dt->slow_level_thresh_up[slow_level]);
        }
    }
}

void
  diag_dpp_voq_admission_test_print(
    int admission_test
  )
{
    if (admission_test & BCM_COSQ_CONTROL_ADMISSION_CT)
       cli_out("'category' ");
    if (admission_test & BCM_COSQ_CONTROL_ADMISSION_CTTC)
       cli_out("'category, traffic class' ");
    if (admission_test & BCM_COSQ_CONTROL_ADMISSION_CTCC)
       cli_out("'category, connection class' ");
    if (admission_test & BCM_COSQ_CONTROL_ADMISSION_ST)
       cli_out("'statistics tag' ");  
}


STATIC cmd_result_t
dpp_diag_voq_details(int unit, int core_id, int voq_id)
{
   cmd_result_t                 res = CMD_OK;
   soc_error_t rc;
   bcm_error_t rv;
   
   
   int isisq = 0;
   char voqtype_str[10];
   SOC_TMC_ITM_QUEUE_INFO q_info;
   int voq_category;
   int queue_range_start;
   int pri_profile_id, pri_count, voq_priority = -1;
   bcm_cosq_gport_priority_profile_t pri_profile;   
   SOC_TMC_IPQ_QUARTET_MAP_INFO queue_map_info;
   bcm_gport_t base_gport = BCM_GPORT_INVALID, phy_port = BCM_GPORT_INVALID;
   int num_cos = 0, i, base_queue, queue_index;
   int ispushqueue = 0;
   int mode, weight;
   int local_credit_worth, remote_credit_worth;
   bcm_cosq_delay_tolerance_t dt;
   int watchdogqueuemin, watchdogqueuemax, watchdogmode, isinwatchdogrange = 0;
   int creditdiscount = 0;
   bcm_cosq_gport_discard_t wred[DPP_DEVICE_COSQ_ING_NOF_DP];
   uint32 color_flags[4] = {BCM_COSQ_DISCARD_COLOR_GREEN, BCM_COSQ_DISCARD_COLOR_YELLOW, BCM_COSQ_DISCARD_COLOR_RED, BCM_COSQ_DISCARD_COLOR_BLACK};
   bcm_color_t colors[4] = {bcmColorGreen, bcmColorYellow, bcmColorRed, bcmColorBlack};
   char * colors_str[4] = {"Green ", "Yellow", "Red   ", "Black "};

   int valid_flags_qax[] = {BCM_COSQ_GPORT_SIZE_BYTES, BCM_COSQ_GPORT_SIZE_BYTES|BCM_COSQ_GPORT_SIZE_SRAM, 
                            BCM_COSQ_GPORT_SIZE_PACKET_DESC|BCM_COSQ_GPORT_SIZE_SRAM};
   int valid_flags_jer[] = {BCM_COSQ_GPORT_SIZE_BYTES, BCM_COSQ_GPORT_SIZE_BUFFER_DESC};
   int* valid_flags;

   char* thresh_type_names_qax[3] = {"bytes", "SRAM bytes", "SRAM PDs"};
   char* thresh_type_names_jer[2] = {"bytes", "BDs"};
   char** thresh_type_names;
   int thresh_type_size, thresh_type_indx;

   bcm_cosq_gport_size_t size_params[DPP_DEVICE_COSQ_ING_NOF_DP][3];

   int is_fadt_enable = 0;
   bcm_cosq_gport_discard_t ecn_config_bds, ecn_config_bytes;
   bcm_cosq_gport_discard_t *ecn_config;
   int signature;
   SOC_TMC_ITM_ADMIT_TSTS test_tmplt[DPP_DEVICE_COSQ_ING_NOF_DP] = {0};
   int admission_test_a, admission_test_b;
   SOC_TMC_ITM_QUEUE_DYN_INFO q_dyn_info;
   uint32 cosq_flags;
   int voq_connector = -1;
   bcm_cosq_threshold_t threshold;
   int is_ocb_eligibility = 0;
   bcm_cosq_gport_connection_t gport_connect;



   
   unit = (unit);

   if (core_id != BCM_CORE_ALL && (core_id < 0 || core_id >= SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores)) {
      cli_out("\ncore id (%d) is not in valid range!\n", core_id);
      return CMD_FAIL;
   }

   if (SOC_IS_QAX(unit)) {
       thresh_type_names = thresh_type_names_qax;
       valid_flags = valid_flags_qax;
       thresh_type_size = 3;
   } else {
       thresh_type_names = thresh_type_names_jer;
       valid_flags = valid_flags_jer;
       thresh_type_size = 2;
   }


   /* 1.a Get Q type(FMQ/VOQ/ISQ) */
   /* 1.b. Base queue info*/
   for (i = 0; i < 8; i++) {
      base_queue = voq_id - i;
      if (base_queue < 0) break;
      if ((base_queue % 4) != 0) continue;

      BCM_GPORT_UNICAST_QUEUE_GROUP_CORE_QUEUE_SET(base_gport, core_id, base_queue);

      rv = bcm_cosq_gport_get(unit, base_gport, &phy_port, &num_cos, &cosq_flags);
      if (num_cos > 0) {
         sal_sprintf(voqtype_str, "%s", "voq");
         break;
      }
      
      BCM_GPORT_MCAST_QUEUE_GROUP_SET(base_gport, base_queue);
      rv = bcm_cosq_gport_get(unit, base_gport, &phy_port, &num_cos, &cosq_flags);
      if (num_cos > 0) {
         sal_sprintf(voqtype_str, "%s", "fmq");
         break;
      }
      
      BCM_COSQ_GPORT_ISQ_SET(base_gport, base_queue);
      rv = bcm_cosq_gport_get(unit, base_gport, &phy_port, &num_cos, &cosq_flags);
      if (num_cos > 0) {
         isisq = 1;
         sal_sprintf(voqtype_str, "%s", "isq");
         break;
      }      
   }  

   if ((num_cos <= 0) || ((base_queue + num_cos - 1) < voq_id)) {
      cli_out("\nGet base queue failed!");
      return CMD_FAIL;
   }
   
   queue_index = voq_id - base_queue;

  
   /* 2.0 Get Credit Request Type
               Get Credit discount Class
               Get Rate Class
               Get Traffic Class
               Get Connection Class 
               Get Credit Request Class */
   rc = soc_dpp_cosq_ingress_queue_info_get(unit, voq_id, &q_info);
   if (SOC_FAILURE(rc)) {
      cli_out("\nsoc_dpp_cosq_ingress_queue_info_get failed(%d)!", rc);
      return CMD_FAIL;
   }

   if ((rv = bcm_cosq_gport_sched_get(unit, base_gport, queue_index, &mode, &weight)) != BCM_E_NONE) {
      cli_out("\nFailed(%d) to get default credit rate template base_gport(0x%8.8x) queue_index(%d)\n", rc, base_gport, queue_index);
      return CMD_FAIL;
   }

   ispushqueue = (mode == BCM_COSQ_DELAY_TOLERANCE_15 ) ? 1 : 0;

   /* 2.a Credit watchdog message time,
               delete time,
               the device's credit watchdog mode,
               is the queue in the credit watchdog queue range, 
               is credit watchdog active */
   if (!ispushqueue) {
       if ((rv = bcm_fabric_control_get(unit, bcmFabricWatchdogQueueMin, &watchdogqueuemin)) != BCM_E_NONE) {
          cli_out("\nbcm_fabric_control_get type(bcmFabricWatchdogQueueMin) failed(%d)!", rv);
          return CMD_FAIL;
       }
       if ((rv = bcm_fabric_control_get(unit, bcmFabricWatchdogQueueMax, &watchdogqueuemax)) != BCM_E_NONE) {
          cli_out("\nbcm_fabric_control_get type(bcmFabricWatchdogQueueMax) failed(%d)!", rv);
          return CMD_FAIL;
       }
       if ((rv = bcm_fabric_control_get(unit, bcmFabricWatchdogQueueEnable, &watchdogmode)) != BCM_E_NONE) {
          cli_out("\nbcm_fabric_control_get type(bcmFabricWatchdogQueueEnable) failed(%d)!", rv);
          return CMD_FAIL;
       }
       if (voq_id >= watchdogqueuemin && voq_id <= watchdogqueuemax) 
          isinwatchdogrange = 1;
       else
          isinwatchdogrange = 0;
    

      if ((rv = bcm_cosq_delay_tolerance_level_get(unit, mode, &dt)) != BCM_E_NONE) {
         cli_out("\nFailed(%d) to get delay tolerance\n", rc);
         return CMD_FAIL;
      }       

      /* 2.b Credit-Balance-Thresholds,
                    Empty-Queue-Credit-Balance-Thresholds,
                    Queue-Size-Thresholds */
      /* Get from dt */


      /* 2.c The credit value(Arad plus and above) */
      if (SOC_IS_ARADPLUS(unit) || SOC_IS_JERICHO(unit)) {
         if ((rv = bcm_fabric_control_get(unit, bcmFabricCreditSize, &local_credit_worth)) != BCM_E_NONE) {
            cli_out("\nbcm_fabric_control_get type(bcmFabricCreditSize) failed(%d)!", rv);
            return CMD_FAIL;
         }
      
         if ((rv = bcm_fabric_control_get(unit, bcmFabricCreditSizeRemoteDefault, &remote_credit_worth)) != BCM_E_NONE) {
            cli_out("\nbcm_fabric_control_get type(bcmFabricCreditSizeRemoteDefault) failed(%d)!", rv);
            return CMD_FAIL;
         }
      }
   }

   
   /* 3. Credit discount class */
   if (!isisq) {
     if (!SOC_TMC_ITM_PER_PACKET_COMPENSATION_ENABLED(unit) || SOC_TMC_ITM_COMPENSATION_LEGACY_MODE(unit)) {
       if ((rv = bcm_cosq_control_get(unit, base_gport, queue_index, bcmCosqControlPacketLengthAdjust, &creditdiscount)) != BCM_E_NONE) {
         cli_out("\nbcm_cosq_control_get(bcmCosqControlPacketLengthAdjust) base_gport(0x%8.8x) queue_index(%d) failed(%d)!", base_gport, queue_index, rv);
         return CMD_FAIL;
       }
     }
     else {
         bcm_cosq_pkt_size_adjust_info_t adjust_info; 
         
         adjust_info.source_info.source_type = bcmCosqPktSizeAdjustSourceScheduler;
         adjust_info.source_info.source_id = 0;
         
         adjust_info.gport = base_gport;
         adjust_info.cosq = queue_index;
         adjust_info.flags = 0;

         if ((rv =bcm_petra_cosq_gport_pkt_size_adjust_get(unit, &adjust_info, &creditdiscount)) != BCM_E_NONE) {
             cli_out("\nbcm_petra_cosq_gport_pkt_size_adjust_get base_gport(0x%8.8x) queue_index(%d) failed(%d)!", base_gport, queue_index, rv);
             return CMD_FAIL;
         }
     }

      /* 4. Rate class */
      /* 4.a Get WRED per DP queue size */
      for (i = 0; i < DPP_DEVICE_COSQ_ING_NOF_DP; i++) {
         bcm_cosq_gport_discard_t_init(&wred[i]);
         wred[i].flags = BCM_COSQ_DISCARD_BYTES | color_flags[i];

         if ((rv = bcm_cosq_gport_discard_get(unit, base_gport, queue_index, &wred[i])) != BCM_E_NONE) {
            cli_out("\nbcm_cosq_gport_discard_get base_gport(0x%8.8x) queue_index(%d) dp(%d) failed(%d)!", 
                    base_gport, queue_index, i, rv);
            return CMD_FAIL;
         }
      }
   }

   /* 4.b Get tail drop per DP */
   /* 4.c Get guaranteed BDs  */
   for (i = 0; i < DPP_DEVICE_COSQ_ING_NOF_DP; i++) {

       for (thresh_type_indx = 0; thresh_type_indx < thresh_type_size; thresh_type_indx++) {
           if ((rv =  bcm_cosq_gport_color_size_get(unit, base_gport, queue_index, 
                                                    colors[i], valid_flags[thresh_type_indx], &size_params[i][thresh_type_indx])) != BCM_E_NONE) {
               cli_out("\nbcm_cosq_gport_color_size_get(%x) base_gport(0x%8.8x) queue_index(%d) colors(%d) failed(%d)!", 
                       valid_flags[thresh_type_indx], base_gport, queue_index, colors[i], rv);
               return CMD_FAIL;
           }
       }

   }   
   
   /* 4.d Get fair adaptive tail drop alpha value per DP(Arad plus and above) 
               Is the fair adaptive tail drop mechanism enabled/disabled for the device */
   if (SOC_IS_ARADPLUS(unit) && ! SOC_IS_QAX(unit)) {
      if ((rv =  bcm_cosq_control_get(unit, 0, 0, bcmCosqControlDropLimitAlpha, &is_fadt_enable)) != BCM_E_NONE) {
         cli_out("\nbcm_cosq_control_get(bcmCosqControlDropLimitAlpha) failed(%d)!", rv);
         return CMD_FAIL;
      }
   }
      
   /* 4.e Get ECN WRED queue size */
   /* 4.f  Get ECN max queue sizes */
   if (!isisq && !SOC_DPP_CONFIG(unit)->tm.is_petrab_in_system && !SOC_IS_ARAD_A0(unit)) {
       if (!SOC_IS_QAX(unit)) {
           /* not available on QAX and later */
           bcm_cosq_gport_discard_t_init(&ecn_config_bds);
           ecn_config_bds.flags = BCM_COSQ_DISCARD_MARK_CONGESTION | BCM_COSQ_DISCARD_BUFFER_DESC /* queue size in BDs */; 
           if ((rv = bcm_cosq_gport_discard_get(unit, base_gport, queue_index, &ecn_config_bds)) != BCM_E_NONE) {
               cli_out("\nbcm_cosq_gport_discard_get(BCM_COSQ_DISCARD_MARK_CONGESTION) base_gport(0x%8.8x) queue_index(%d) failed(%d)!", base_gport, queue_index, rv);
               return CMD_FAIL;
           }
       }

      bcm_cosq_gport_discard_t_init(&ecn_config_bytes);
      ecn_config_bytes.flags = BCM_COSQ_DISCARD_MARK_CONGESTION | BCM_COSQ_DISCARD_BYTES    /* queue size in Bytes */; 
      if ((rv = bcm_cosq_gport_discard_get(unit, base_gport, queue_index, &ecn_config_bytes)) != BCM_E_NONE) {
         cli_out("\nbcm_cosq_gport_discard_get(BCM_COSQ_DISCARD_MARK_CONGESTION) base_gport(0x%8.8x) queue_index(%d) failed(%d)!", base_gport, queue_index, rv);
         return CMD_FAIL;
      }
   }
   
   /* 5.a Get Priority */
   if (!SOC_IS_JERICHO(unit)) {
       queue_range_start = (voq_id/64)*64;
       if ((rv = bcm_cosq_priority_get(unit, queue_range_start, queue_range_start+63, &pri_profile_id)) != BCM_E_NONE) {
          cli_out("\nbcm_cosq_priority_get failed(%d)!", rv);
          return CMD_FAIL;
       }

       if ((rv = bcm_cosq_priority_profile_get(unit, pri_profile_id, &pri_count, &pri_profile)) != BCM_E_NONE) {
          cli_out("\nbcm_cosq_priority_profile_get failed(%d)!", rv);
          return CMD_FAIL;
       }

       voq_priority = BCM_COSQ_PRI_PROFILE_GET(pri_profile, voq_id%64);
   }

   /* 5.b Queue signature */
   if (!isisq) {
      if ((rv = bcm_cosq_control_get(unit, base_gport, queue_index, bcmCosqControlHeaderUpdateField, &signature)) != BCM_E_NONE) {
         cli_out("\nbcm_cosq_control_get(bcmCosqControlHeaderUpdateField) base_gport(0x%8.8x) queue_index(%d) failed(%d)!", base_gport, queue_index, rv);
         return CMD_FAIL;
      }
   }

   /* 6. Get Information used to associate the queue with a VSQ */
   /* 6.a Get Category class */
   rc = soc_dpp_cosq_ingress_queue_category_get(unit, voq_id, &voq_category);
   if (SOC_FAILURE(rc)) {
      cli_out("\nsoc_dpp_cosq_ingress_queue_category_get failed(%d)!", rc);
      return CMD_FAIL;
   }

   /* 6.b Traffic Class ??? in flow mapped queues? */
   /* Get from q_info.vsq_traffic_cls */


   /* 6.c Get Connection class---q_info.vsq_connection_cls */

   if (!SOC_IS_QAX(unit)) { 
       /* Admission logic template is not relevant for QAX */

       /* 6.d Get admission logic template 
          Note: for now: assume only admission test 00 is used */
       for (i = 0; i < DPP_DEVICE_COSQ_ING_NOF_DP; i++) {
           rc = soc_dpp_cosq_ingress_test_tmplt_get(unit, q_info.rate_cls, i, &test_tmplt[i]);
           if (SOC_FAILURE(rc)) {
               cli_out("\nsoc_dpp_cosq_ingress_queue_info_get failed(%d)!", rc);
               return CMD_FAIL;
           }
       }
       
       if ((rv = bcm_cosq_control_get(unit, 0, 0, bcmCosqControlAdmissionTestProfileA, &admission_test_a)) != BCM_E_NONE) {
           cli_out("\nbcm_cosq_control_get(bcmCosqControlAdmissionTestProfileA) failed(%d)!", rv);
           return CMD_FAIL;
       }
       
       if ((rv = bcm_cosq_control_get(unit, 0, 0, bcmCosqControlAdmissionTestProfileB, &admission_test_b)) != BCM_E_NONE) {
           cli_out("\nbcm_cosq_control_get(bcmCosqControlAdmissionTestProfileA) failed(%d)!", rv);
           return CMD_FAIL;
       }   
   }

   /* 7. Print current queue size in bytes, BDs or buffers */
   rc = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_itm_queue_dyn_info_get,(unit, core_id, voq_id, &q_dyn_info));

   if (SOC_FAILURE(rc)) {
      cli_out("\nsoc_dpp_cosq_ingress_queue_dyn_info_get failed(%d)!", rc);
      return CMD_FAIL;
   }   
   

   /* 8. Get Attached VOQ connector */
   if (!isisq) {
      gport_connect.flags = BCM_COSQ_GPORT_CONNECTION_INGRESS;
      gport_connect.voq = base_gport; 
      
      /* bcm_cosq_gport_connection_get expected to get voq_connector initialized with valid (external) core.
         the core value is not really used by this function, so set it to 0 to have a legal value.
         voq_connector_id is set to -1 (invalid) - to be set by the API */
      BCM_COSQ_GPORT_VOQ_CONNECTOR_CORE_SET(gport_connect.voq_connector,-1,0);
      rv = bcm_cosq_gport_connection_get(unit, &gport_connect);
      if (BCM_FAILURE(rv)) {
         cli_out("\nbcm_cosq_gport_connection_get failed(%d) base_gport(0x%8.8x)!", rv, base_gport);
         return CMD_FAIL;
      }      

      voq_connector = BCM_COSQ_GPORT_VOQ_CONNECTOR_ID_GET(gport_connect.voq_connector);
   }   

   /* 9. Get Destination sys port */
   rc = soc_dpp_cosq_ingress_queue_to_flow_mapping_get(unit, voq_id, &queue_map_info);
   if (SOC_FAILURE(rc)) {
      cli_out("\nsoc_dpp_cosq_ingress_queue_to_flow_mapping_get failed(%d)!", rc);
      return CMD_FAIL;
   }

   /* 10. Get OCB eligibility */
   if (!isisq && !SOC_IS_QAX(unit)) {
       /* OCB eligibility is not relevant for QAX */
      sal_memset(&threshold, 0, sizeof(threshold));
      threshold.flags = BCM_COSQ_THRESHOLD_INGRESS | BCM_COSQ_THRESHOLD_OCB;
      threshold.type = bcmCosqThresholdBytes;
      rv = bcm_cosq_gport_threshold_get(unit, base_gport, 0, &threshold);
      if (BCM_FAILURE(rv)) {
         cli_out("\nbcm_cosq_gport_threshold_get base_gport(0x%8.8x) queue_index(%d) failed(%d)!", base_gport, 0, rv);
         return CMD_FAIL;
      }   
      is_ocb_eligibility = (threshold.flags & BCM_COSQ_THRESHOLD_SET)? 0x1 :0x0;
   } 


   cli_out("Basic info\n");
   cli_out("\tQ type: %s\n", voqtype_str);
   cli_out("\tnum cos: %d, cosq class: %d\n", num_cos, queue_index);
   cli_out("\tBase queue id: %d, base queue gport: 0x%8.8x\n", base_queue, base_gport);

   diag_dpp_voq_credit_request_class_print(mode);
   
   if (!ispushqueue) {
   /* 2.a The device's credit watchdog mode,
               is the queue in the credit watchdog queue range, 
               is credit watchdog active */
      switch(watchdogmode)
      {
         case BCM_FABRIC_WATCHDOG_QUEUE_DISABLE:
            cli_out("\tWatchdog disable\n");
            break;
         case BCM_FABRIC_WATCHDOG_QUEUE_ENABLE_NORMAL:
            cli_out("\tWatchdog enable in normal mode\n");
            break;
         case BCM_FABRIC_WATCHDOG_QUEUE_ENABLE_FAST_STATUS_MESSAGE:
            cli_out("\tWatchdog enable in aggressive status message mode\n");
            break;
         case BCM_FABRIC_WATCHDOG_QUEUE_ENABLE_COMMON_STATUS_MESSAGE:
            cli_out("\tWatchdog enable in common status message mode\n");
            break;
         default:
            cli_out("\tWatchdog mode unknown\n");
            break;
      }

      cli_out("\tIs queue in credit watchdog queue range:%s\n", (isinwatchdogrange?"True":"False"));
      
      /* 2.b Credit watchdog message time,
                    Delete time 
                    Credit-Balance-Thresholds,
                    Empty-Queue-Credit-Balance-Thresholds,
                    Queue-Size-Thresholds */
      diag_dpp_voq_credit_request_threshold_print(unit, &dt);
        
      /* 2.c The credit value(Arad plus and above) */               
      if (SOC_IS_ARADPLUS(unit) || SOC_IS_JERICHO(unit)) {
         cli_out("Credit value(local): %d, Credit value(remote): %d\n", local_credit_worth, remote_credit_worth);
      }
   }

   /* 3. Credit discount class */
   if (!isisq) {
       if (!SOC_TMC_ITM_PER_PACKET_COMPENSATION_ENABLED(unit)  || SOC_TMC_ITM_COMPENSATION_LEGACY_MODE(unit)) {
           cli_out("Credit discount value: %d\n", creditdiscount);
       }
       else {
           cli_out("Per Packet Compensation enabled.\n");
           cli_out("\tPacketLengthAdjust value: %d\n", creditdiscount);
       }
   }

   /* 4. Rate class */
   cli_out("Rate class info\n");   
   /* 4.a Get WRED per DP queue size */
   if (!isisq) {
      cli_out("\tWRED info:\n");
      for (i = 0; i < DPP_DEVICE_COSQ_ING_NOF_DP; i++) {
         cli_out("\t\t%s: enable(%s) min_thresh(%d) max_thresh(%d) drop_probability(%d)\n", 
                 colors_str[i], ((wred[i].flags & BCM_COSQ_DISCARD_ENABLE )? "True" : "False"), wred[i].min_thresh, wred[i].max_thresh, wred[i].drop_probability);
      }
   }

   /* 4.b Get guaranteed BDs  */
   cli_out("\tGuaranteed info:");
   for (thresh_type_indx = 0; thresh_type_indx < thresh_type_size; thresh_type_indx++) {
       /* guaranteed is not per DP */
       cli_out(" guaranteed queue size in %s(%d)", thresh_type_names[thresh_type_indx],  size_params[0][thresh_type_indx].size_min);
   }
   cli_out("\n");

   /* 4.c Get tail drop per DP */
   cli_out("\t");
   if (SOC_IS_QAX(unit)) {
       cli_out("Fair Adaptive ");
   }
   cli_out("Tail drop info:\n");
   for (i = 0; i < DPP_DEVICE_COSQ_ING_NOF_DP; i++) {
       if (SOC_IS_QAX(unit)) {
           cli_out ("\t\t%s:\n ", colors_str[i]);
           for (thresh_type_indx = 0; thresh_type_indx < thresh_type_size; thresh_type_indx++) {
               cli_out ("\t\t\t%s: min size(%d) max_size(%d) alpha(%d)\n", thresh_type_names[thresh_type_indx],
                        size_params[i][thresh_type_indx].size_fadt_min, size_params[i][thresh_type_indx].size_max, size_params[i][thresh_type_indx].size_alpha_max);
           }
       } else {
           cli_out("\t\t%s: max queue size in bytes(%d), max queue size in BDs(%d)\n", colors_str[i], 
                   size_params[i][0].size_max /* bytes */,
                   size_params[i][1].size_max); /* Bds */
       }
   }
   
   
   /* 4.d Get fair adaptive tail drop alpha value per DP(Arad plus and above) 
               Is the fair adaptive tail drop mechanism enabled/disabled for the device */
   if (!SOC_IS_QAX(unit)) {
       if (SOC_IS_ARADPLUS(unit)) {
           cli_out("\tFair adaptive tail drop info:\n");
           cli_out("\t\tEnable: %s\n", (is_fadt_enable ? "True" : "False"));
           
           for (i = 0; i < DPP_DEVICE_COSQ_ING_NOF_DP; i++) {
               /* alpha is for buffer descriptors only */
               cli_out("\t\t%s: alpha(%d)\n", colors_str[i], size_params[i][1].size_alpha_max);
           }
       }
   }

   /* 4.e Get ECN WRED queue size */
   /* 4.f  Get ECN max queue sizes */
   if (!isisq && !SOC_DPP_CONFIG(unit)->tm.is_petrab_in_system && !SOC_IS_ARAD_A0(unit)) {
      cli_out("\tECN max queue size in bytes(%d)",ecn_config_bytes.ecn_thresh);
      if (!SOC_IS_QAX(unit)) {
          /* not available on QAX and later */
          cli_out(", max queue size in BDs(%d)", ecn_config_bds.ecn_thresh);
      }
      cli_out("\n");

      if (SOC_IS_QAX(unit)) {
          ecn_config = &ecn_config_bytes;
      } else {
          ecn_config = &ecn_config_bds;
      }
      
      cli_out("\tECN wred info: enable(%s) min_thresh(%d) max_thresh(%d) drop_probability(%d)\n", 
              ((ecn_config->flags & BCM_COSQ_DISCARD_ENABLE )? "True" : "False"), 
              ecn_config->min_thresh, ecn_config->max_thresh, ecn_config->drop_probability);
   }

   if (!SOC_IS_JERICHO(unit)) {
       /* 5.a Get Priority */
       cli_out("Priority: %s\n", ((voq_priority)?("High"):("Low")));
       
       /* 5.b Queue signature */
       if (!isisq) {
          cli_out("Signature: %d\n", signature);
       }
   }

   /* 6. Get Information used to associate the queue with a VSQ */
   cli_out("VSQ-related\n");
   /* 6.a Get Category class */
   cli_out("\tCategory class: %d\n", voq_category);
   /* 6.b Traffic Class ??? in flow mapped queues? */
   cli_out("\tTraffic class: %d\n", q_info.vsq_traffic_cls);

   /* 6.c Get Connection class---q_info.vsq_connection_cls */
   cli_out("\tConnection class: %d\n", q_info.vsq_connection_cls);


   if (!SOC_IS_QAX(unit)) {
       /* Admission logic template is not supported in QAX */

       /* 6.d Get admission logic template 
          Note: for now: assume only admission test 00 is used */
       for (i = 0; i < DPP_DEVICE_COSQ_ING_NOF_DP; i++) {
           cli_out("\t%s: Admission-test-templates(%d)\n", colors_str[i], test_tmplt[i]);
       }               
       cli_out("\tAdmissionTestProfileA[0]:");
       diag_dpp_voq_admission_test_print(admission_test_a);
       cli_out("\n");
       
       cli_out("\tAdmissionTestProfileB[0]: ");
       diag_dpp_voq_admission_test_print(admission_test_b); 
       cli_out("\n");
   }

   /* 7. Print current queue size in bytes, BDs or buffers */
   cli_out("Current queue size: %d bytes, ", q_dyn_info.pq_inst_que_size[SOC_TMC_INGRESS_THRESHOLD_TOTAL_BYTES]);
   if (!SOC_IS_QAX(unit)) {
       if (SOC_DPP_CONFIG(unit)->tm.guaranteed_q_mode == SOC_DPP_GUARANTEED_Q_RESOURCE_BUFFERS)
           cli_out("current queue buffer size: %d\n", q_dyn_info.pq_inst_que_buff_size);
       else
           cli_out("current queue bds size: %d", q_dyn_info.pq_inst_que_buff_size);
   } else {
       cli_out("current queue SRAM size: %d bytes, ", q_dyn_info.pq_inst_que_size[SOC_TMC_INGRESS_THRESHOLD_SRAM_BYTES]);
       cli_out("current queue SRAM size: %d PDs ", q_dyn_info.pq_inst_que_size[SOC_TMC_INGRESS_THRESHOLD_SRAM_PDS]);
   }
   cli_out("\n");

   /* 8. Get Attached VOQ connector */
   if (!isisq) {
      cli_out("Attached VOQ connector: 0x%8.8x\n", voq_connector);
   }      

   /* 9. Get Destination sys port */
   cli_out("Destination sys port: 0x%8.8x\n", queue_map_info.system_physical_port);

   /* 10. Get OCB eligibility */
   if (!SOC_IS_QAX(unit)) { 
       /* OCB eligibility is not relevant for QAX */
       if (!isisq) {
           cli_out("OCB eligiblity: %s\n", ((is_ocb_eligibility)?("True"):("False")));
       }
   }

   return res;

}


STATIC cmd_result_t
dpp_diag_voq_congestion(int unit, int core, int voq_id, int most) {
   cmd_result_t                 res = CMD_OK;

#if ARAD_DEBUG_IS_LVL1

   if (most < 0) most = 0; /*invalid input: default is 0*/

   res=dpp_diag_voq_non_empty_print(unit,core,voq_id,0,most,1,0);
   if (SOC_FAILURE(res)) {
       cli_out("\nFailed to get cosq non empty queues\n\n");
       res = CMD_FAIL;
   }

#endif
   return res;
}


STATIC cmd_result_t
dpp_diag_voq_global(int unit, int core) {
   bcm_error_t rv;
   bcm_cosq_range_t dram_mix_threshold, multicast_queue_range, watchdog_queue_range, fabric_queue_range, shaper_queue_range;
   int core_gport;


   if (SOC_DPP_CORE_MODE_IS_SYMMETRIC(unit))
       cli_out("Core mode is symmetric!\n");
   else
       cli_out("Core mode is asymmetric!\n");


   sal_memset(&dram_mix_threshold, 0x0, sizeof(dram_mix_threshold));

   BCM_COSQ_GPORT_CORE_SET(core_gport, core);
   if (SOC_IS_JERICHO(unit) && !SOC_IS_QAX(unit)) {
       rv = bcm_cosq_control_range_get(unit, core_gport, 0, bcmCosqThresholdDramMixDbuff, &dram_mix_threshold);
       if (BCM_FAILURE(rv)) {
           cli_out("\nbcm_cosq_control_get(bcmCosqThresholdDramMixDbuff) failed(%d)!", rv);
           return CMD_FAIL;
       }
   }
   rv = bcm_cosq_control_range_get(unit, core_gport, 0, bcmCosqRangeMulticastQueue, &multicast_queue_range);
   if (BCM_FAILURE(rv)) {
       cli_out("\nbcm_cosq_control_range_get(bcmCosqRangeMulticastQueue) failed(%d)!", rv);
       return CMD_FAIL;
   }
   rv = bcm_cosq_control_range_get(unit, core_gport, 0, bcmCosqRangeShaperQueue, &shaper_queue_range);
   if (BCM_FAILURE(rv)) {
       cli_out("\nbcm_cosq_control_range_get(bcmCosqRangeShaperQueue) failed(%d)!", rv);
       return CMD_FAIL;
   }
   rv = bcm_cosq_control_range_get(unit, core_gport, 0, bcmCosqRangeFabricQueue, &fabric_queue_range);
   if (BCM_FAILURE(rv)) {
       cli_out("\nbcm_cosq_control_range_get(bcmCosqRangeQueueMin) failed(%d)!", rv);
       return CMD_FAIL;
   }
   
   /*FIXME: not supported
   rv = bcm_cosq_control_range_get(unit, core_gport, 0, bcmCosqRecycleQueue, &recycle_queue_range);
   if (BCM_FAILURE(rv)) {
       cli_out("\bcm_cosq_control_range_get(bcmCosqRecycleQueue) failed(%d)!", rv);
       return CMD_FAIL;
   }*/
   
   rv = bcm_cosq_control_range_get(unit, core_gport, 0, bcmCosqWatchdogQueue, &watchdog_queue_range);
   if (BCM_FAILURE(rv)) {
       cli_out("\bcm_cosq_control_range_get(bcmCosqWatchdogQueueMin) failed(%d)!", rv);
       return CMD_FAIL;
   }

   if (!SOC_IS_QAX(unit)) {
       cli_out("Dram-mix threshold range is: Min: (%d) and Max: (%d).\n", dram_mix_threshold.range_start, dram_mix_threshold.range_end); 
   }
   cli_out("Multicast Queue is %s. The range is: Min: (%d) and Max: (%d).\n", ((multicast_queue_range.is_enabled)?("enabled"):("not enabled")),multicast_queue_range.range_start, multicast_queue_range.range_end);
   cli_out("Shaper Queue is %s. The range is: Min: (%d) and Max: (%d).\n", ((shaper_queue_range.is_enabled)?("enabled"):("not enabled")),shaper_queue_range.range_start, shaper_queue_range.range_end);
   cli_out("Fabric Queue is %s. The range is: Min: (%d) and Max: (%d).\n", ((fabric_queue_range.is_enabled)?("enabled"):("not enabled")),fabric_queue_range.range_start, fabric_queue_range.range_end);
   cli_out("Credit watchdog Queue is %s. The range is: Min: (%d) and Max: (%d).\n", ((watchdog_queue_range.is_enabled)?("enabled"):("not enabled")),watchdog_queue_range.range_start, watchdog_queue_range.range_end);

   

   
   return CMD_OK;
}



void
  diag_dpp_sch_sub_flow_hr_print(
    SOC_TMC_SCH_SUB_FLOW_HR *info
  )
{

  cli_out("SP class %s \n\r",
          SOC_TMC_SCH_SUB_FLOW_HR_CLASS_to_string(info->sp_class)
          );
  cli_out("Weight: %u\n\r",info->weight);
}

void
  diag_dpp_sch_sub_flow_cl_print(
    SOC_TMC_SCH_SUB_FLOW_CL *info
  )
{
  
  cli_out("Class %s \n\r",
          SOC_TMC_SCH_SUB_FLOW_CL_CLASS_to_string(info->sp_class)
          );
  cli_out("Weight: %u\n\r",info->weight);
}

void
  dpp_diag_sch_sub_flow_se_info_print(
    SOC_TMC_SCH_SUB_FLOW_SE_INFO *info,
    SOC_TMC_SCH_SE_TYPE se_type
  )
{

  switch(se_type)
  {
  case SOC_TMC_SCH_SE_TYPE_HR:
    cli_out("HR, ");
    diag_dpp_sch_sub_flow_hr_print(&(info->hr));
    break;
  case SOC_TMC_SCH_SE_TYPE_CL:
    cli_out("CL, ");
    diag_dpp_sch_sub_flow_cl_print(&(info->cl));
    break;
  case SOC_TMC_SCH_SE_TYPE_FQ:
    cli_out("FQ ");
      break;
  default:
    cli_out("Undefined SE Type!\n\r");
      break;
  }

}

void
  dpp_diag_sch_sub_flow_credit_source_print(
    SOC_TMC_SCH_SUB_FLOW_CREDIT_SOURCE *info
  )
{

  cli_out("    ID: %u, SE-TYPE: ",
          info->id
          );
  dpp_diag_sch_sub_flow_se_info_print(&(info->se_info), info->se_type);

}

void
  dpp_diag_sch_sub_flow_shaper_print(
    SOC_TMC_SCH_SUB_FLOW_SHAPER *info
  )
{

  if (info->max_rate == SOC_TMC_SCH_SUB_FLOW_SHAPE_NO_LIMIT)
  {
    cli_out("Max_rate: not limited, ");
  }
  else
  {
    cli_out("Max_rate:  %u[Kbps], ",info->max_rate);
  }

  if (info->max_burst >= SOC_TMC_SCH_SUB_FLOW_SHAPER_BURST_NO_LIMIT)
  {
    cli_out("Max_burst: not limited. ");
  }
  else if (info->max_burst == 0)
  {
    cli_out("Max_burst: 0 (sub-flow disabled by shaping). ");
  }
  else
  {
    cli_out("Max_burst: %u[Bytes]. ",info->max_burst);
  }

}

void
  dpp_diag_sch_subflow_print(
    SOC_TMC_SCH_SUBFLOW *info,
    uint8 is_table_flow,
    uint32 subflow_id
  )
{
  char
    rate_tbl_style[15],
    burst_tbl_style[15],
    crdt_src_tbl_style[45],
    crdt_src_weight_tbl_style[15];

  if (!is_table_flow)
  {
    if (info->is_valid)
    {
      cli_out("__________________________\n\r");
      cli_out("Shaper: ");
      dpp_diag_sch_sub_flow_shaper_print(&(info->shaper));
      cli_out("Slow rate id %s, ",
              SOC_TMC_SCH_SLOW_RATE_NDX_to_string(info->slow_rate_ndx)
              );
      cli_out("Credit_source:\n\r");
      dpp_diag_sch_sub_flow_credit_source_print(&(info->credit_source));
    }
    else
    {
      cli_out("Disabled:\n\r");
    }
  }
  else
  {
    /*
     *  Prepare shaper description
     */
    if (info->shaper.max_rate == SOC_TMC_SCH_SUB_FLOW_SHAPE_NO_LIMIT)
    {
      sal_sprintf(rate_tbl_style, " No Limit  ");
    }
    else if (info->shaper.max_rate == 0)
    {
      sal_sprintf(rate_tbl_style, "    *0*    ");
    }
    else
    {
      sal_sprintf(rate_tbl_style, " %-8u  ",info->shaper.max_rate);
    }

    if (info->shaper.max_burst >= SOC_TMC_SCH_SUB_FLOW_SHAPER_BURST_NO_LIMIT)
    {
      sal_sprintf(burst_tbl_style, " No Limit  ");
    }
    else if (info->shaper.max_burst == 0)
    {
      sal_sprintf(burst_tbl_style, "   *0*     ");
    }
    else
    {
      sal_sprintf(burst_tbl_style, "  %-6u   ",info->shaper.max_burst);
    }

    /*
     *  Prepare Credit Source description
     */

    /* Credit Source Info */
    switch(info->credit_source.se_type) {
    case SOC_TMC_SCH_SE_TYPE_HR:
      sal_sprintf(
        crdt_src_tbl_style,
        " HR[%-5u], SP-Cls: %s",
        info->credit_source.id,
        SOC_TMC_SCH_SUB_FLOW_HR_CLASS_to_string(info->credit_source.se_info.hr.sp_class)
      );
      break;
    case SOC_TMC_SCH_SE_TYPE_CL:
      sal_sprintf(
        crdt_src_tbl_style,
        " CL[%-5u], Class: %s  ",
        info->credit_source.id,
        SOC_TMC_SCH_SUB_FLOW_CL_CLASS_to_string(info->credit_source.se_info.cl.sp_class)
      );
      break;
    case SOC_TMC_SCH_SE_TYPE_FQ:
      sal_sprintf(
        crdt_src_tbl_style,
        " FQ[%-5u] %s",
        info->credit_source.id,
        "                       "
       );
      break;
    default:
      sal_sprintf(
        crdt_src_tbl_style,
        "%-34s",
        "  ???  "
       );
    }

    /* Credit Source Weight */
    switch(info->credit_source.se_type) {
    case SOC_TMC_SCH_SE_TYPE_HR:
      sal_sprintf(
        crdt_src_weight_tbl_style,
        "  %-4u ",
        info->credit_source.se_info.hr.weight
      );
      break;
    case SOC_TMC_SCH_SE_TYPE_CL:
      sal_sprintf(
        crdt_src_weight_tbl_style,
        "  %-4u ",
        info->credit_source.se_info.cl.weight
      );
      break;
    case SOC_TMC_SCH_SE_TYPE_FQ:
      sal_sprintf(
        crdt_src_weight_tbl_style,
        "  ---  "
       );
      break;
    default:
      sal_sprintf(
        crdt_src_weight_tbl_style,
        "  ???  "
       );
    }

    /* Part of Flow print, table-style */
    cli_out("| %1u |%s|%s|%s|%s|\n\r",
            subflow_id,
            rate_tbl_style,
            burst_tbl_style,
            crdt_src_tbl_style,
            crdt_src_weight_tbl_style
            );
  }
}


STATIC void
dpp_diag_sch_flow_print(SOC_TMC_SCH_FLOW *info)
{
    uint32 ind=0;

    cli_out(" Flow[%u] - %s",
            info->sub_flow[0].id,
            info->sub_flow[0].is_valid?"Enabled, ":"Disabled. \n\r"
            );

    if (info->sub_flow[0].is_valid)
    {
      cli_out("%s, %s, ",
              SOC_TMC_SCH_FLOW_TYPE_to_string(info->flow_type),
              info->is_slow_enabled?"slow-enabled": "not slow-enabled"
              );
      if (info->sub_flow[1].is_valid)
      {
        cli_out("2 subflows: %u, %u: \n\r", info->sub_flow[0].id, info->sub_flow[1].id);
      }
      else
      {
        cli_out("1 subflow: %u: \n\r", info->sub_flow[0].id);
      }

      cli_out("+----------------------------------------------------------------------+\n\r");
      cli_out("|Id |Rate[Kbps] |Burst[Byte]|Credit Src[ID],                   |Weight |\n\r");
      cli_out("+----------------------------------------------------------------------+\n\r");

      for (ind = 0; ind < SOC_TMC_SCH_NOF_SUB_FLOWS; ind++)
      {
        if (info->sub_flow[ind].is_valid)
        {
          dpp_diag_sch_subflow_print(
            &(info->sub_flow[ind]),
            TRUE,
            ind
          );
        }
      }

      cli_out("+----------------------------------------------------------------------+\n\r");

      for (ind = 0; ind < SOC_TMC_SCH_NOF_SUB_FLOWS; ind++)
      {
        if (info->sub_flow[ind].is_valid)
        {
          if (info->sub_flow[ind].shaper.max_rate == 0)
          {
            cli_out("Note: sub-flow[%u] disabled by shaper rate\n\r", ind);
          }
          if (info->sub_flow[ind].shaper.max_burst == 0)
          {
            cli_out("Note: sub-flow[%u] disabled by shaper burst\n\r", ind);
          }
        }
      }
    }
}

STATIC void
dpp_diag_sch_port_print(
    SOC_TMC_SCH_PORT_INFO *info,
    uint32                 port_id
  )
{
  uint32
    ind;


  cli_out("Port[%-2u] - %s, \n\r",
          port_id,
          info->enable?"enabled":"disabled"
          );

  for (ind=0; ind<SOC_TMC_NOF_TRAFFIC_CLASSES; ++ind)
  {
    cli_out("    HR[%u] mode: %s\n\r",ind,SOC_TMC_SCH_SE_HR_MODE_to_string(info->hr_modes[ind]));
  }

  if (info->max_expected_rate != SOC_TMC_SCH_PORT_MAX_EXPECTED_RATE_AUTO)
  {
    cli_out(", Max_expected_rate: %u[Mbps]\n\r",info->max_expected_rate);
  }
  else
  {
    cli_out("\n\r");
  }
}

STATIC void
  dpp_diag_ofp_rate_info_print(
     SOC_TMC_OFP_RATE_INFO *info
  )
{
  cli_out("Port[%-2u] rate: ",info->port_id);
  cli_out("SCH: %-8u[Kbps], ",info->sch_rate);
  cli_out("EGQ: %-8u[Kbps], ",info->egq_rate);

  if (info->max_burst == SOC_TMC_OFP_RATES_BURST_LIMIT_MAX)
  {
    cli_out("Max Burst: No Limit.\n\r");
  }
  else
  {
    cli_out("Max Burst: %-6u[Byte].\n\r",info->max_burst);
  }

}

void
  dpp_diag_sch_se_cl_print(
    SOC_TMC_SCH_SE_CL *info,
    SOC_TMC_SCH_CL_CLASS_WEIGHTS_MODE cl_mode,
    SOC_TMC_SCH_FLOW_IPF_CONFIG_MODE ipf_mode
  )
{
  cli_out("%s", SOC_TMC_SCH_CL_CLASS_WEIGHTS_MODE_to_string(cl_mode));
  if(cl_mode == SOC_TMC_SCH_CL_WEIGHTS_MODE_INDEPENDENT_PER_FLOW){
     cli_out("(%s)", SOC_TMC_SCH_FLOW_IPF_CONFIG_MODE_to_string(ipf_mode));
  }
  cli_out(", Class-id: %d", info->id);
}


void
  dpp_diag_sch_se_hr_print(
    SOC_TMC_SCH_SE_HR *info
  )
{
  cli_out("HR-mode: %s", SOC_TMC_SCH_SE_HR_MODE_to_string(info->mode));
}

void
  dpp_diag_sch_se_info_print(
    SOC_TMC_SCH_SE_INFO *info,
    SOC_TMC_SCH_CL_CLASS_WEIGHTS_MODE cl_mode,
    SOC_TMC_SCH_FLOW_IPF_CONFIG_MODE ipf_mode
  )
{

  cli_out("%s[%u] - %s, ",
          SOC_TMC_SCH_SE_TYPE_to_string(info->type),
          info->id,
          SOC_TMC_SCH_SE_STATE_to_string(info->state)
          );

  if (info->state == SOC_TMC_SCH_SE_STATE_ENABLE)
  {
    switch(info->type)
    {
    case SOC_TMC_SCH_SE_TYPE_CL:
      dpp_diag_sch_se_cl_print(&(info->type_info.cl),  cl_mode, ipf_mode);
      cli_out(", ");
      break;
    case SOC_TMC_SCH_SE_TYPE_FQ:
      break;
    case SOC_TMC_SCH_SE_TYPE_HR:
      dpp_diag_sch_se_hr_print(&(info->type_info.hr));
      cli_out(", ");
      break;
    default:
      break;
    }

    cli_out("%s",(info->is_dual)?"Dual Bucket":"Not Dual Bucket");

    if (info->group != SOC_TMC_SCH_GROUP_AUTO)
    {
      cli_out(", group: %s \n\r",
              SOC_TMC_SCH_GROUP_to_string(info->group)
              );
    }
    else
    {
      cli_out("\n\r");
    }
  }
}


void
  dpp_diag_ofp_rates_tbl_info_print(
    SOC_TMC_OFP_RATES_TBL_INFO *info
  )
{
  uint32
    ind=0,
    nof_zero_rate=0;
  uint8
    zero_rate;


  cli_out("Nof_valid_entries: %u[Entries]\n\r",info->nof_valid_entries);
  cli_out("Rates:\n\r");
  for (ind=0; ind<info->nof_valid_entries; ++ind)
  {
    zero_rate = SOC_SAND_NUM2BOOL((info->rates[ind].egq_rate == 0) && (info->rates[ind].sch_rate == 0));
    if(!zero_rate)
    {
      dpp_diag_ofp_rate_info_print(&(info->rates[ind]));
    }
    else
    {
      nof_zero_rate++;
    }
  }
}

void
  dpp_diag_sch_hr2ps_info_print(
    uint32               hr_se_id,
    soc_hr2ps_info_t     *hr2ps_info
  )
{
  char
    rate_tbl_style[15],
    burst_tbl_style[15],
    crdt_src_tbl_style[45],
    crdt_src_weight_tbl_style[15];

  SOC_TMC_SCH_SE_INFO *se_info = &(hr2ps_info->se_info);
  uint32 max_rate = hr2ps_info->kbits_sec_max;
  uint32 max_burst = hr2ps_info->max_burst;
  uint32 mode = hr2ps_info->mode;
  uint32 weight = hr2ps_info->weight;

  /*
   * SE sch print
   */
  cli_out("Scheduling Element info:"
          "\n\r"
          );

  cli_out("%s[%u] - %s, ",
          SOC_TMC_SCH_SE_TYPE_to_string(se_info->type),
          se_info->id,
          SOC_TMC_SCH_SE_STATE_to_string(se_info->state)
          );
  
  dpp_diag_sch_se_hr_print(&(se_info->type_info.hr));

  cli_out("%s",(se_info->is_dual)?", Dual Bucket":", Not Dual Bucket");

  if (se_info->group != SOC_TMC_SCH_GROUP_AUTO){
    cli_out(", group: %s \n\r",
            SOC_TMC_SCH_GROUP_to_string(se_info->group)
            );
  }
  else {
    cli_out("\n\r");
  }
  
  /*
   *  Prepare shaper description
   */
  if (max_rate == SOC_TMC_SCH_SUB_FLOW_SHAPE_NO_LIMIT)
  {
    sal_sprintf(rate_tbl_style, " No Limit  ");
  }
  else if (max_rate == 0)
  {
    sal_sprintf(rate_tbl_style, "    *0*    ");
  }
  else
  {
    sal_sprintf(rate_tbl_style, " %-8u  ", max_rate);
  }

  if (max_burst >= SOC_TMC_SCH_SUB_FLOW_SHAPER_BURST_NO_LIMIT)
  {
    sal_sprintf(burst_tbl_style, " No Limit  ");
  }
  else if (max_burst == 0)
  {
    sal_sprintf(burst_tbl_style, "   *0*     ");
  }
  else
  {
    sal_sprintf(burst_tbl_style, "  %-6u   ", max_burst);
  }

  /*
   *  Prepare Credit Source description
   */ 
  /* Credit Source Info */
  sal_sprintf(
    crdt_src_tbl_style,
    " TCG[%-2u]                           ",    
    mode
  );
  
  /* Credit Source Weight */
  sal_sprintf(
    crdt_src_weight_tbl_style,
    "  %-4u ",
    weight
  );  

  /* Part of Flow print, table-style */
  cli_out("mapping between hr and tcg in ps:\n\r");  
  cli_out("+----------------------------------------------------------------------+\n\r");
  cli_out("|Id |Rate[Kbps] |Burst[Byte]|Credit Src[ID],                    |Weight |\n\r");
  cli_out("+----------------------------------------------------------------------+\n\r");
  
  cli_out("|%-3u|%s|%s|%s|%s|\n\r",
          hr_se_id,
          rate_tbl_style,
          burst_tbl_style,
          crdt_src_tbl_style,
          crdt_src_weight_tbl_style
          );
  cli_out("\n\r");  
}

STATIC cmd_result_t
dpp_diag_cosq_flow_and_up_print(int unit,
                                       int core,
                                       uint32 is_flow,
                                       uint32 dest_id,
                                       uint32 print_status,
                                       uint32 short_format)
{
    SOC_TMC_SCH_FLOW_AND_UP_INFO    *flow_and_up_info = NULL;
    uint32                          temp_credit_source[10];
    uint32                          temp_credit_source_nof;
    uint32                          level_idx;
    uint32                          credit_source_i;
    SOC_TMC_SCH_FLOW_AND_UP_PORT_INFO *port_sch_info;
    SOC_TMC_SCH_FLOW_AND_UP_SE_INFO   *se_sch_info;
    soc_hr2ps_info_t                  hr2ps_info[SOC_TMC_FLOW_AND_UP_MAX_CREDIT_SOURCES];

    uint32                            hr_se_id[SOC_TMC_FLOW_AND_UP_MAX_CREDIT_SOURCES];
    int                             rv;
    cmd_result_t                    res = CMD_OK;

    level_idx = 0;
    flow_and_up_info = sal_alloc(sizeof(SOC_TMC_SCH_FLOW_AND_UP_INFO), "dpp_diag_cosq_flow_and_up_print.flow_and_up_info");
    if(flow_and_up_info == NULL) {
        goto fail;
    }
    SOC_TMC_SCH_FLOW_AND_UP_INFO_clear(unit,flow_and_up_info, TRUE);
    rv = soc_dpp_cosq_flow_and_up_info_get(unit, core, is_flow, dest_id, print_status, flow_and_up_info);
    if (BCM_FAILURE(rv))
    {
        goto fail;
    }

    /*
     * Queue quartet print
     */
    if (!is_flow)
    {
        cli_out("base_queue_quartet: %u\n\r", flow_and_up_info->base_queue / 4);
        if (
            (flow_and_up_info->qrtt_map_info.flow_quartet_index == SOC_TMC_IPQ_INVALID_FLOW_QUARTET) &&
            (flow_and_up_info->qrtt_map_info.is_composite == 0) &&
            (flow_and_up_info->qrtt_map_info.system_physical_port == SOC_TMC_MAX_SYSTEM_PHYSICAL_PORT_ID)
         )
        {
            cli_out("The queue quartet is unmapped.\n\r");
        }
        else
        {
            cli_out("Flow_quartet_index: %u\n\r",flow_and_up_info->qrtt_map_info.flow_quartet_index);
            cli_out("Is_composite: %d\n\r",flow_and_up_info->qrtt_map_info.is_composite);
            cli_out("System_physical_port: %u\n\r",flow_and_up_info->qrtt_map_info.system_physical_port);
            cli_out("Fap_id: %u\n\r",((unsigned)flow_and_up_info->qrtt_map_info.fap_id));
            cli_out("Fap_port_id: %u\n\r",((unsigned)flow_and_up_info->qrtt_map_info.fap_port_id));
       }
    }


    /*
     * SCH consumer print 
     */
    dpp_diag_sch_flow_print(&(flow_and_up_info->sch_consumer));

    /*
     * Flow status print
     */
    if (print_status)
    {
        cli_out("The flow %d current actual credit rate is %d Kbps\n\r",
                flow_and_up_info->base_queue, flow_and_up_info->credit_rate
                );
    }

    sal_memset(hr2ps_info, 0, (sizeof(soc_hr2ps_info_t)*SOC_TMC_FLOW_AND_UP_MAX_CREDIT_SOURCES));
    sal_memset(hr_se_id, 0, (sizeof(uint32)*SOC_TMC_FLOW_AND_UP_MAX_CREDIT_SOURCES));
    while (flow_and_up_info->credit_sources_nof != 0)
    {

        /*
         * Level print
         */
        cli_out("\n\r"
                "Next Scheduler Level (%02u)\n\r"
                "==========================\n\r",
                ++level_idx
                );

        /*
         * Credit source print
         */
        for (credit_source_i = 0; credit_source_i < flow_and_up_info->credit_sources_nof; credit_source_i++)
        {
            cli_out("Credit Source %u:"
                    "\n\r",
                    credit_source_i
                    );

                if (flow_and_up_info->is_port_sch[credit_source_i])
                {
                /*
                 * connection info between HR and Port sch print
                 */

                    /* 
                     * Get hr2ps info
                     */
                    rv = soc_dpp_cosq_hr2ps_info_get(unit, core,
                           (flow_and_up_info->credit_sources)[credit_source_i], 
                           &(hr2ps_info[credit_source_i]));
                    if (BCM_FAILURE(rv)){
                      goto fail;
                    }

                    if (SOC_IS_ARAD(unit)) {
                      hr_se_id[credit_source_i] = (flow_and_up_info->credit_sources)[credit_source_i] - SOC_TMC_HR_SE_ID_MIN_ARAD;
                    }
                    else
                    dpp_diag_sch_hr2ps_info_print(hr_se_id[credit_source_i], 
                       &(hr2ps_info[credit_source_i]));

                    port_sch_info = &((flow_and_up_info->sch_union_info[credit_source_i]).port_sch_info);

                    /*
                     * Port sch print
                     */

                    dpp_diag_sch_port_print(&(port_sch_info->port_info), flow_and_up_info->sch_port_id[credit_source_i]);
                    if ((port_sch_info->ofp_rate_info).port_id != SAL_UINT32_MAX)
                    {
                        dpp_diag_ofp_rate_info_print(&(port_sch_info->ofp_rate_info));
                    }

                    /*
                     * Credit rate and flow control
                     */
                     if (print_status)
                     {
                          cli_out("Port[%u] HR[%u] actual credit rate: %d Kbps\n\r",
                                  flow_and_up_info->sch_port_id[credit_source_i],
                                  flow_and_up_info->sch_priority_ndx[credit_source_i],
                                  port_sch_info->credit_rate
                                  );

                          if (port_sch_info->fc_cnt == 0)
                          {
                              cli_out("Not under flow control\n\r");
                          }
                          else
                          {
                            cli_out("!!!Under Flow Control: %d%% of the time\n\r",
                                    port_sch_info->fc_percent
                                    );
                          }
                      }


            } else { /*SE*/
                se_sch_info = &((flow_and_up_info->sch_union_info[credit_source_i]).se_sch_info);
                /*
                 * SE sch print
                 */
                cli_out("Scheduling Element info:"
                        "\n\r"
                        );

                dpp_diag_sch_se_info_print(&(se_sch_info->se_info), se_sch_info->cl_mode, se_sch_info->ipf_mode );
                cli_out("\n\r"
                        "Flow info:"
                        "\n\r"
                        );
                 dpp_diag_sch_flow_print(&(se_sch_info->sch_consumer));

            }
        }

        /*
         * Reterive next level
         */
        if (flow_and_up_info->next_level_credit_sources_nof == 0) {
            /*Done*/
            break;

        }
        temp_credit_source_nof = flow_and_up_info->next_level_credit_sources_nof;
        sal_memcpy(temp_credit_source, flow_and_up_info->next_level_credit_sources, temp_credit_source_nof*sizeof(uint32));
        SOC_TMC_SCH_FLOW_AND_UP_INFO_clear(unit, flow_and_up_info, FALSE);
        flow_and_up_info->credit_sources_nof = temp_credit_source_nof;
        sal_memcpy(flow_and_up_info->credit_sources, temp_credit_source, flow_and_up_info->credit_sources_nof*sizeof(uint32));

        rv = soc_dpp_cosq_flow_and_up_info_get(unit, core, is_flow, dest_id, print_status, flow_and_up_info);
        if (BCM_FAILURE(rv))
        {
            goto fail;
        }
    }

#if defined(ARAD_PRINT_FLOW_AND_UP_PRINT_DRM_AND_MAL_RATES) || defined(PETRA_PRINT_FLOW_AND_UP_PRINT_DRM_AND_MAL_RATES)
    if (flow_and_up_info->ofp_rate_valid)
    {
          cli_out("OFP rates table:"
                  "\n\r"
                  );
          dpp_diag_ofp_rates_tbl_info_print(&(flow_and_up_info->ofp_rates_table);
          cli_out("\n\r");
    }
#endif
    sal_free(flow_and_up_info);
    return res;
fail:
    sal_free(flow_and_up_info);
    return CMD_FAIL;
}

STATIC
void print_egq_usage(void) {
   char dpp_diag_egq_usage[] =
      "Usage (DIAG cosq qpair egq):"
      "\n\tDIAGnotsics egq commands"
      "\n\tUsages:"
      "\n\tDIAG cosq qpair egq <OPTION> <parameters> ..."
      "\n\tOPTION can be:"
      "\n\t\tcapture - display last SOP header"
      "\n\t\tmap - display port to egress port scheduler mapping"
      "\n\tParmeters:"
      "\n\t\tps=<id> - display a graphical representation of the port scheduler associated with local port <id>."
      "\n";

   cli_out(dpp_diag_egq_usage);
}

STATIC cmd_result_t
dpp_diag_cosq_qpair(int unit, args_t *a)
{
   cmd_result_t                 res = CMD_OK;

   char           *option;
   int strnlen_option;
   parse_table_t  pt;
   uint8             mode_given = 0;
   int               map_print = 0, last_cell_print = 0;
   uint32            port_print = -1;

   option = ARG_GET(a);
   if (NULL == option) {
       return CMD_USAGE;
   }
   
   parse_table_init(unit, &pt);
   parse_table_add(&pt, "ps", PQ_DFL | PQ_INT, &port_print, &port_print, NULL);

   if (0 > parse_arg_eq(a, &pt)) {
      parse_arg_eq_done(&pt);
      return res;
   }
   /*
    * Make sure at least one of the strings is NULL terminated.
    */
   strnlen_option = sal_strnlen(option, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH);
   if (!sal_strncasecmp(option,"egq", strnlen_option)){
      option=ARG_GET(a);

      while (NULL!=option)
      {
         strnlen_option = sal_strnlen(option, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH);
         if (!sal_strcasecmp(option, "?")) {
            mode_given++;
            parse_arg_eq_done(&pt);
            print_egq_usage();
            return CMD_OK;
         }
         if (!sal_strncasecmp(option, "capture", strnlen_option)) {
            mode_given++;
            last_cell_print = 1;
         }
         if (!sal_strncasecmp(option, "map", strnlen_option)) {
            mode_given++;
            map_print = 1;
         }
         option = ARG_GET(a);
      }

      if (mode_given == 0 && port_print==-1) {
         last_cell_print = 1;
         map_print = 1;
      }
    
      res = dpp_diag_egq_print(unit, last_cell_print, map_print, port_print);

      parse_arg_eq_done(&pt);
      if (res == CMD_USAGE) {
         print_egq_usage();
         return CMD_FAIL;
      }
   }
   else if (!sal_strncasecmp(option,"e2e", strnlen_option)){
      if (port_print==-1) {
         parse_arg_eq_done(&pt);
         return CMD_USAGE;
      }
    
      res = dpp_diag_e2e_ps_graphic(unit, port_print);
      parse_arg_eq_done(&pt);

      if (res == CMD_USAGE) {
         return CMD_USAGE;
      }
   }
   else {
      parse_arg_eq_done(&pt);
      return CMD_USAGE;
   }
   
   return res;
}
#endif /* BCM_PETRA_SUPPORT */

STATIC cmd_result_t
dpp_diag_cosq(int unit, args_t *a) {
   cmd_result_t   res = CMD_OK;
#ifdef BCM_PETRA_SUPPORT
   if (SOC_IS_DPP(unit)) {
      char           *option;
      int strnlen_option;
      parse_table_t  pt;
      uint32           is_voq_conn = 0, is_flow = 0, print_status = 0;
      uint32           dest_id = 1;
      unsigned char  print_flow_and_up = 1; /* default options */
      unsigned char  non_empty_queues = 0;
      unsigned char  credit_watchdog = 0;
      cmd_result_t   retCode = CMD_OK;
      uint32          short_format = 0;
      uint32          is_flow_also = 0, poll_nof_times = 1, poll_delay = 0;
      unsigned char     scheduler_alloc_manager = 0;
      uint32            flow_id = -1, flow_count = 1;
      char*             allocated_resource = NULL;
      unsigned char  print_voqs = 0;
      unsigned char  set_fc = 0, set_egq_port_shaper = 0, set_autocredit = 0;
      uint32           enable = -1, last_queue = -1, first_queue = -1, rate = 0;
      ARAD_DBG_AUTOCREDIT_INFO autocredit_info;
      uint32        exact_rate;
      int                  voq_id = -1;
      int                  most = 0;
      int              print_mapping = 0;
      uint32           detailed = 0;
      uint32           congestion = 0;
      unsigned char  local_to_sys = 0;
      uint32          by_sys =0 , by_mod = 0, by_voq = 0;
      bcm_pbmp_t     bcm_pbmp;
      bcm_pbmp_t     arg_pbmp;
      uint32         port_count;
      int prm_core = -1;
      int core, first_core, last_core;


      option = ARG_GET(a);

      if(!option) {
          return CMD_USAGE;
      }

      /*
       * Make sure at least one of the strings is NULL terminated.
       */
      if (!sal_strncasecmp(option,"QPair", sal_strnlen(option, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH))){
          return dpp_diag_cosq_qpair(unit, a);
      }

      BCM_PBMP_CLEAR(arg_pbmp);
      BCM_PBMP_CLEAR(bcm_pbmp);
      parse_table_init(unit, &pt);
      parse_table_add(&pt, "is_flow", PQ_DFL | PQ_INT, 0, &is_flow, NULL);
      parse_table_add(&pt, "is_voq_conn", PQ_DFL | PQ_INT, 0, &is_voq_conn, NULL);
      parse_table_add(&pt, "dest_id", PQ_DFL | PQ_INT, 0, &dest_id, NULL);
      parse_table_add(&pt, "print_status", PQ_DFL | PQ_INT, 0, &print_status, NULL);
      parse_table_add(&pt, "is_flow_also", PQ_DFL | PQ_INT, 0, &is_flow_also, NULL);
      parse_table_add(&pt, "poll_nof_times", PQ_DFL | PQ_INT, 0, &poll_nof_times, NULL);
      parse_table_add(&pt, "poll_delay", PQ_DFL | PQ_INT, 0, &poll_delay, NULL);
      parse_table_add(&pt, "flow_id", PQ_DFL | PQ_INT, &flow_id, &flow_id, NULL);
      parse_table_add(&pt, "core", PQ_DFL | PQ_INT, &prm_core, &prm_core, NULL);
      parse_table_add(&pt, "flow_count", PQ_DFL | PQ_INT, &flow_count, &flow_count, NULL);
      parse_table_add(&pt, "allocated_resource", PQ_DFL | PQ_STRING, &allocated_resource, &allocated_resource, NULL);
      parse_table_add(&pt, "enable", PQ_DFL | PQ_INT, 0, &enable, NULL);
      parse_table_add(&pt, "first_queue", PQ_DFL | PQ_INT, 0, &first_queue, NULL);
      parse_table_add(&pt, "last_queue", PQ_DFL | PQ_INT, 0, &last_queue, NULL);
      parse_table_add(&pt, "rate", PQ_DFL | PQ_INT, 0, &rate, NULL);
      parse_table_add(&pt, "id", PQ_DFL | PQ_INT, &voq_id, &voq_id, NULL);
      parse_table_add(&pt, "most", PQ_DFL | PQ_INT, &most, &most, NULL);
      parse_table_add(&pt, "detailed", PQ_DFL | PQ_INT, 0, &detailed, NULL);
      parse_table_add(&pt, "congestion", PQ_DFL | PQ_INT, 0, &congestion, NULL);
      parse_table_add(&pt, "by_sys", PQ_DFL | PQ_INT, 0, &by_sys, NULL);
      parse_table_add(&pt, "by_mod", PQ_DFL | PQ_INT, 0, &by_mod, NULL);
      parse_table_add(&pt, "by_voq", PQ_DFL | PQ_INT, 0, &by_voq, NULL);
      parse_table_add(&pt, "pbmp", PQ_DFL | PQ_PBMP | PQ_BCM, 0, &arg_pbmp, NULL);

      if (0 > parse_arg_eq(a, &pt)) {
         parse_arg_eq_done(&pt);
         return retCode;
      }

      if ((prm_core < -1 ) || 
          (prm_core >= SOC_DPP_DEFS_GET(unit, nof_cores))) {

           cli_out("error ilegal core ID for device\n");
           parse_arg_eq_done(&pt);
           return CMD_FAIL;
      } 
      if (prm_core == -1) {
          first_core = 0;
          last_core = SOC_DPP_DEFS_GET(unit, nof_cores);
      } else {
          first_core = prm_core;
          last_core = prm_core+1;
      }

      while (NULL != option) {
         /*
          * Make sure at least one of the strings is NULL terminated.
          */
         strnlen_option = sal_strnlen(option, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH);
         if (!sal_strncasecmp(option, "print_flow_and_up", strnlen_option)) {
            print_flow_and_up = 1;
         } else if (!sal_strncasecmp(option, "non_empty_queues", strnlen_option)) {
            print_flow_and_up = 0;
            non_empty_queues = 1;
         } else if (!sal_strncasecmp(option, "fc", strnlen_option)) {
            set_fc = 1;
            print_flow_and_up = 0;
         } else if (!sal_strncasecmp(option, "egq_port_shaper", strnlen_option)) {
            set_egq_port_shaper = 1;
            print_flow_and_up = 0;
         } else if (!sal_strncasecmp(option, "autocredit", strnlen_option)) {
            set_autocredit = 1;
            print_flow_and_up = 0;
         } else if (!sal_strncasecmp(option, "voq", strnlen_option)) {
            print_voqs = 1;
         } else if (!sal_strncasecmp(option, "local_to_sys", strnlen_option)) {
            print_flow_and_up = 0;
            local_to_sys = 1;
         } else if (!sal_strncasecmp(option, "print_mapping", strnlen_option)) {
            print_flow_and_up = 0;
            print_mapping = 1;
         } else if (!sal_strncasecmp(option, "credit_watchdog", strnlen_option)) {
            print_flow_and_up = 0;
            credit_watchdog = 1;
         } else if (!sal_strncasecmp(option, "scheduler_alloc_manager", strnlen_option)) {
            print_flow_and_up = 0;
            scheduler_alloc_manager = 1;
         } else {
            cli_out("argument \"%s\" not recognized\n", option);
            res = CMD_USAGE;
         }

         option = ARG_GET(a);
      }

      if (res == CMD_OK) {
         for (core=first_core; core < last_core; core++) {
             cli_out("\n%s\n", get_core_str(unit,core));

             if (print_voqs) {
                if (detailed == 1) {
                    res = dpp_diag_voq_details(unit, core, voq_id); 
                } else if (congestion) {
                      res = dpp_diag_voq_congestion(unit, core, voq_id, most);
                } else { 
                    res = dpp_diag_voq_global(unit, core);
                }
                set_autocredit = 0;
                set_egq_port_shaper = 0;
                set_fc = 0;
                non_empty_queues = 0;
                print_flow_and_up = 0;
                credit_watchdog = 0;
                local_to_sys = 0;
             }
             if (!credit_watchdog) { /* different defaults for enable, first/last_queue when not a credit_watchdog command */
                if (enable == (uint32)(-1)) enable = 1;
                if (first_queue == (uint32)(-1)) first_queue = 0;
                if (last_queue == (uint32)(-1)) last_queue = 0;
             }
             if (print_flow_and_up) {
                unit = (unit);
                is_flow = ((is_voq_conn == 0) && (is_flow == 0)) ? 0 : 1;
                res = dpp_diag_cosq_flow_and_up_print(unit, core, is_flow, dest_id, print_status, short_format);
                if (SOC_FAILURE(res)) {
                   cli_out("\nFailed to get cosq flow and up statistics\n\n");
                   res = CMD_FAIL;
                }
             }
             if (non_empty_queues) {
                unit = (unit);
                res=dpp_diag_voq_non_empty_print(unit,core,SAL_UINT32_MAX,is_flow_also,0,poll_nof_times,poll_delay);
                if (SOC_FAILURE(res)) {
                   cli_out("\nFailed to get cosq non empty queues\n\n");
                   res = CMD_FAIL;
                }
             }
             if (set_fc) {
                unit = (unit);
                res = arad_dbg_flow_control_enable_set_unsafe(unit, enable);
                if (SOC_FAILURE(res)) {
                   cli_out("\nFailed to set Flow Control to %d\n\n", enable);
                   res = CMD_FAIL;
                }
             }
             if (set_egq_port_shaper) {
                unit = (unit);
                res = arad_dbg_egress_shaping_enable_set_unsafe(unit, enable);
                if (SOC_FAILURE(res)) {
                   cli_out("\nFailed to set EGQ Port Shaper to %d\n\n", enable);
                   res = CMD_FAIL;
                }
             }
             if (credit_watchdog) {
                int is_enabled, start_queue, end_queue;
                /* get current state */
                if (bcm_fabric_control_get(unit, bcmFabricWatchdogQueueMin, &start_queue) ||
                    bcm_fabric_control_get(unit, bcmFabricWatchdogQueueMax, &end_queue) ||
                    bcm_fabric_control_get(unit, bcmFabricWatchdogQueueEnable, &is_enabled)) {
                   cli_out("\nFailed to get current watchdog configuration\n\n");
                   parse_arg_eq_done(&pt);
                   return CMD_FAIL;
                }
                /* change state if told to */
                if (first_queue != (uint32)(-1) && bcm_fabric_control_set(unit, bcmFabricWatchdogQueueMin, start_queue = first_queue)) {
                   cli_out("\nFailed to set watchdog first queue\n\n");
                   parse_arg_eq_done(&pt);
                   return CMD_FAIL;
                }
                if (last_queue != (uint32)(-1) && bcm_fabric_control_set(unit, bcmFabricWatchdogQueueMax, end_queue = last_queue)) {
                   cli_out("\nFailed to set watchdog last queue\n\n");
                   parse_arg_eq_done(&pt);
                   return CMD_FAIL;
                }
                if (enable != (uint32)(-1) && bcm_fabric_control_set(unit, bcmFabricWatchdogQueueEnable, is_enabled = enable)) {
                   cli_out("\nFailed to set watchdog activeness\n\n");
                   parse_arg_eq_done(&pt);
                   return CMD_FAIL;
                }
                /* print state */
                cli_out("\nCredit Watchdog is %sabled from  queue %d (0x%x)  to  queue %d (0x%x)\n",
                        is_enabled ? "en" : "dis", start_queue, start_queue, end_queue, end_queue);
                if (is_enabled) {
                    uint32 scan_time;
                    cli_out("\tWatchdog mode is ");
                    switch (is_enabled) {
                        case BCM_FABRIC_WATCHDOG_QUEUE_ENABLE_NORMAL:
                            cli_out ("BCM_FABRIC_WATCHDOG_QUEUE_ENABLE_NORMAL\n");
                            break;
                        case BCM_FABRIC_WATCHDOG_QUEUE_ENABLE_FAST_STATUS_MESSAGE:
                            cli_out ("BCM_FABRIC_WATCHDOG_QUEUE_ENABLE_FAST_STATUS_MESSAGE\n");
                            break;
                        case BCM_FABRIC_WATCHDOG_QUEUE_ENABLE_COMMON_STATUS_MESSAGE: {
                            bcm_cosq_delay_tolerance_t dt;

                            cli_out ("BCM_FABRIC_WATCHDOG_QUEUE_ENABLE_COMMON_STATUS_MESSAGE\n");

                            if (bcm_cosq_delay_tolerance_level_get(unit, BCM_COSQ_DELAY_TOLERANCE_SET_COMMON_STATUS_MSG, &dt)) {
                                cli_out("\nFailed to get common status message\n\n");
                                parse_arg_eq_done(&pt);
                                return CMD_FAIL;
                            }
                            cli_out("\tCommon message generation period is %d usec\n", dt.credit_request_watchdog_status_msg_gen  );
                        }
                            break;
                        default:
                            cli_out ("Unknown\n");
                            break;
                    }
                    if (GET_EXACT_CREDIT_WATCHDOG_SCAN_TIME_NANO(unit, scan_time)) {
                        cli_out("\nFailed to get scan time\n\n");
                        parse_arg_eq_done(&pt);
                        return CMD_FAIL;
                    }
                    cli_out ("\tScan time is %d usec\n", scan_time/1000); /* nano sec -> micro sec */
                }
             }

             if (set_autocredit) {
                unit = (unit);
                autocredit_info.first_queue = first_queue;
                autocredit_info.last_queue = last_queue;
                autocredit_info.rate = rate;
                res = arad_dbg_autocredit_set_unsafe(unit, &autocredit_info, &exact_rate);
                if (SOC_FAILURE(res)) {
                   cli_out("\nFailed to set Autocredit\n\n");
                   res = CMD_FAIL;
                }
             }

             if (local_to_sys) {
                bcm_gport_t         sys_gport;
                bcm_gport_t         gport;
                bcm_port_t            local_port_id;
                bcm_port_t             sys_port_id;

                BCM_PBMP_COUNT(arg_pbmp, port_count);
                BCM_PBMP_CLEAR(bcm_pbmp);
                if (port_count == 0) {
                   BCM_PBMP_ASSIGN(bcm_pbmp, PBMP_PORT_ALL(unit));
                } else {
                   BCM_PBMP_ASSIGN(bcm_pbmp, arg_pbmp);
                }

                BCM_PBMP_REMOVE(bcm_pbmp, PBMP_SFI_ALL(unit));
                cli_out("         Local Port           |      System Port       |        NIF port           \n");
                cli_out("-----------------------------------------------------------------------------------\n");
                BCM_PBMP_ITER(bcm_pbmp, local_port_id) {
                   /*convert potr_id to gport(include type)*/
                   BCM_GPORT_LOCAL_SET(gport, local_port_id);
                   /*get system gport*/
                   bcm_stk_gport_sysport_get(unit, gport, &sys_gport);
                   /*get system port*/
                   sys_port_id = BCM_GPORT_SYSTEM_PORT_ID_GET(sys_gport);
                   cli_out("         %-10d           |        %-10d      |         %-10s               \n", local_port_id, sys_port_id, BCM_PORT_NAME(unit, local_port_id));
                }
             }

             if (scheduler_alloc_manager) {
                res = dpp_diag_alloc_cosq_sched_print(unit, core, flow_id, flow_count, allocated_resource);
                if (SOC_FAILURE(res)) {
                   cli_out("\nFailed to get cosq scheduler statistics\n\n");
                   res = CMD_FAIL;
                }
             }
             if (print_mapping) {

                 /*direct or indirect*/
                 if (ARAD_IS_VOQ_MAPPING_DIRECT(unit)) {
                     cli_out("\nMapping mode: DIRECT\n");
                 }
                 else {
                     cli_out("\nMapping mode: INDIRECT\n");
                 }
                 if (by_sys || by_mod || by_voq) {
                     res = dpp_diag_cosq_print_modport_to_sysport_to_voq_mapping(unit, by_sys, by_mod, by_voq);
                     if (SOC_FAILURE(res)) {
                        cli_out("\nFailed to get modport to sysport mapping\n\n");
                        res = CMD_FAIL;
                     }
                 }
             }
             
         }
      }
      parse_arg_eq_done(&pt);  /*Should not be done before scheduler_alloc_manager is used.*/
   }
#endif
    return res;

}

#ifdef BCM_PETRA_SUPPORT
typedef struct sysport_voq_modport_mapping{
    int core_id;
    int sys_id;
    int tm_port;
    int modid;
    int voq;
    int sw_only;
} sysport_voq_modport_mapping;

int cmpfunc_sys (void * a, void * b)
{
    return (((sysport_voq_modport_mapping*)(a))->sys_id - ((sysport_voq_modport_mapping*)(b))->sys_id);
}

int cmpfunc_mod (void * a, void * b)
{
    return (((sysport_voq_modport_mapping*)(a))->tm_port - ((sysport_voq_modport_mapping*)(b))->tm_port);
}

int cmpfunc_voq (void * a, void * b)
{
    return (((sysport_voq_modport_mapping*)(a))->voq - ((sysport_voq_modport_mapping*)(b))->voq);
}

STATIC cmd_result_t
dpp_diag_cosq_print_modport_to_sysport_to_voq_mapping(int unit, int by_sys, int by_mod, int by_voq) {
    bcm_gport_t sysport, mod_gport;
    sysport_voq_modport_mapping *sys_arr = NULL;
    int mod, tm_port, rv, modid, port, sys_id, core_id, i, counter = 0;
    uint8 is_valid = FALSE, is_sw_only = FALSE;
    uint32 base_queue = 0;


    sys_arr = sal_alloc(sizeof(sysport_voq_modport_mapping) * SOC_TMC_NOF_SYS_PHYS_PORTS_ARAD, "dpp_diag_cosq_print_modport_to_sysport_to_voq_mapping.sys_arr");
    if(sys_arr == NULL) {
        cli_out("Memory allocation failure\n");
        return CMD_FAIL;
    }

    SOC_DPP_CORES_ITER(BCM_CORE_ALL, core_id) {
        for (mod = 0; mod < ARAD_MAX_FAP_ID; mod++) {
            for (tm_port = 0; tm_port < SOC_TMC_NOF_FAP_PORTS_ARAD; tm_port++) {
                BCM_GPORT_MODPORT_SET(mod_gport, mod, tm_port);
                modid = BCM_GPORT_MODPORT_MODID_GET(mod_gport);
                port = BCM_GPORT_MODPORT_PORT_GET(mod_gport);
                rv = bcm_stk_gport_sysport_get(unit, mod_gport, &sysport);
                if (rv != BCM_E_NONE) {
                    goto fail;
                }
                if (unit >= BCM_LOCAL_UNITS_MAX){
                    goto fail;
                }
                if (ARAD_SYS_PHYS_PORT_INVALID(unit) == BCM_GPORT_SYSTEM_PORT_ID_GET(sysport)) {
                    continue;
                }
                sys_id = BCM_GPORT_SYSTEM_PORT_ID_GET(sysport);
                rv = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_ipq_destination_id_packets_base_queue_id_get,(unit, core_id, sys_id, &is_valid, &is_sw_only, &base_queue));
                if (SOC_SAND_FAILURE(rv)) {
                    goto fail;
                }
                if(unit >= BCM_LOCAL_UNITS_MAX) {
                    goto fail;
                }
                if (!is_valid) {
                    continue;
                }
                sys_arr[counter].core_id=core_id;
                sys_arr[counter].modid=modid;
                sys_arr[counter].tm_port=port;
                sys_arr[counter].voq=base_queue;
                sys_arr[counter].sys_id=sys_id;
                if (is_sw_only) {
                    sys_arr[counter].sw_only=1;
                }
                counter++;
            }
        }
        if (by_sys) {
            soc_sand_os_qsort(sys_arr, counter, sizeof(sysport_voq_modport_mapping), &cmpfunc_sys);
        }
        else if (by_voq) {
            soc_sand_os_qsort(sys_arr, counter, sizeof(sysport_voq_modport_mapping), &cmpfunc_voq);
        }
        for (i=0; i<counter;i++) {
            cli_out("Core:%3d: baseQ:%5d <-> System port:%6d%1s <-> ModPort:%6d,%4d\n",sys_arr[i].core_id, sys_arr[i].voq, sys_arr[i].sys_id, sys_arr[i].sw_only ? " " : "s" ,sys_arr[i].modid, sys_arr[i].tm_port);
        }
        counter = 0;
    }
    sal_free(sys_arr);
    return CMD_OK;

fail:
    sal_free(sys_arr);
    return CMD_FAIL;
}
#endif /* BCM_PETRA_SUPPORT */


STATIC cmd_result_t
dpp_diag_fabric(int unit, args_t *a) {
    char           *option;

    option = ARG_GET(a);

    /*
     * Make sure at least one of the strings is NULL terminated.
     */
    if (option != NULL && !sal_strncasecmp(option, "gci", sal_strnlen(option, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH))) {
        diag_counter_data_t *gci_backoff_info = NULL;
        diag_counter_data_t *gci_info = NULL;


        if (SOC_IS_ARAD(unit))
        {
#ifdef BCM_PETRA_SUPPORT
            gci_backoff_info = arad_diag_fabric_gci_backoff;
            gci_info = arad_diag_fabric_gci;

#endif  /*BCM_PETRA_SUPPORT*/
        } else {
            cli_out("argument \"%s\" not recognized\n", option);
            return CMD_USAGE;
        }   

        if (gci_backoff_info != NULL)
        {
            /*GCI backoff congestion level maximal value and signal per multicast context*/
            cli_out("Fabric GCI-backoff diagnostic\n");
            cli_out("=============================\n");
            dpp_diag_counters_print(unit, gci_backoff_info, 0, 0, NULL, 0); /*0 for print all*/
            cli_out("\n\n");

        }

        if (gci_info != NULL)
        {
            /*GCI leaky buckets*/
            cli_out("Fabric GCI diagnostic\n");
            cli_out("=====================\n");
            dpp_diag_counters_print(unit, gci_info, 0, 0, NULL, 0); /*0 for print all*/
            cli_out("\n\n");
        }
 
    } else {
        cli_out("argument \"%s\" not recognized\n", option);
        return CMD_USAGE;
    }

   return CMD_OK;
}

#ifdef BCM_PETRA_SUPPORT

#define DIAG_DCMN_COMP_DEFAULT_NETWORK_HDR_SIZE 24

void
print_header_diff_usage(int unit)
{
    char cmd_dpp_diag_header_diff_usage[] =
    "Usage (DIAG headerdiff):"
    "\n\tDIAGnotsics HeaderDiff commands\n\t"
    "Usages:\n\t"
    "DIAG headerdiff [OPTION] "
#ifdef __PEDANTIC__
    "\nFull documentation cannot be displayed with -pendantic compiler\n";
#else
    "\n\tOPTION can be:"
    "\n\t\tingress    display ingress header diff"
    "\n\t\tegress     display egress header diff"
    "\n\n";
#endif   /*COMPILER_STRING_CONST_LIMIT*/
    cli_out(cmd_dpp_diag_header_diff_usage);
}

cmd_result_t
cmd_diag_header_diff(int unit, args_t* a)
{
    char                *subcmd;
    int strnlen_subcmd;
    parse_table_t       pt;
    uint32              hdr_size;
    uint64              pckts_num64;
    uint64              reg64;
    uint32              pckts_num32;
    uint64              bytes_num64;
    uint32              avg_bytes_num1;
    uint32              avg_bytes_num2;
    uint32 interval = 1000000; /* 1 second */
    char *block_names_ingress[] = {NULL, NULL};
    char *block_names_egress[] = {"EGQ", NULL};
    int block_num = 2;
    int core = 0;
    int rv;

    block_names_ingress[0] = SOC_IS_QAX(unit) ? "CGM" : "IQM";
    if(SOC_IS_JERICHO(unit)) {
        if(SOC_IS_QUX(unit)) {
            block_names_ingress[1] = "NIF";
            block_names_egress[1] = "NIF";
        } else {
            block_names_ingress[1] = "NBIH";
            block_names_egress[1] = "NBIH";
}
    } else {
        block_names_ingress[1] = "NBI";
        block_names_egress[1] = "NBI";
    }

    if (SOC_IS_JERICHO_PLUS_ONLY(unit)) {
        cli_out("ERROR: access to registers should be fixed for JERICHO_PLUS at places we used _REG(32|64) access routines\n");
        return BCM_E_FAIL;
    }

    if (!sh_check_attached(ARG_CMD(a), unit)) {
        return CMD_FAIL;
    }

    if ((subcmd = ARG_GET(a)) == NULL) {
        return CMD_USAGE;
    }


    /* parse values */
    parse_table_init(unit, &pt);
    hdr_size = DIAG_DCMN_COMP_DEFAULT_NETWORK_HDR_SIZE;
    parse_table_add(&pt, "NetworkHeaderSize", PQ_DFL | PQ_INT, &hdr_size, &hdr_size, NULL);

    if (parse_arg_eq(a, &pt) < 0 ) {
        cli_out("%s: ERROR: Unknown option: %s\n",
                        ARG_CMD(a), ARG_CUR(a));
        parse_arg_eq_done(&pt);
        return CMD_FAIL;
    }
    parse_arg_eq_done(&pt);


    BCM_DPP_CORES_ITER(BCM_CORE_ALL, core) {
        /*
         * Make sure at least one of the strings is NULL terminated.
         */
        strnlen_subcmd = sal_strnlen(subcmd, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH);
        if (!sal_strncasecmp(subcmd, "Ingress", strnlen_subcmd)) {

            /* init counters */
            COMPILER_64_ZERO(reg64);

#ifdef BCM_JERICHO_SUPPORT
            if (SOC_IS_JERICHO(unit)) {
                if(SOC_IS_QUX(unit)){
                    rv = WRITE_NIF_RX_TOTAL_PKT_COUNTERr(unit, 0);
                    if(rv != SOC_E_NONE) {
                        return CMD_FAIL;
                    }
                    rv = WRITE_NIF_RX_TOTAL_BYTE_COUNTERr(unit, reg64);
                    if(rv != SOC_E_NONE) {
                        return CMD_FAIL;
                    }
                } else {
                    rv = WRITE_NBIH_RX_TOTAL_PKT_COUNTERr_REG32(unit, 0);
                    if(rv != SOC_E_NONE) {
                        return CMD_FAIL;
                    }
                    rv = WRITE_NBIH_RX_TOTAL_BYTE_COUNTERr_REG64(unit, reg64);
                    if(rv != SOC_E_NONE) {
                        return CMD_FAIL;
                    }
                }
            } else
#endif /*BCM_JERICHO_SUPPORT*/
            {
                rv = WRITE_NBI_STATISTICS_RX_BURSTS_OK_CNTr(unit, reg64);
                if(rv != SOC_E_NONE) {
                    return CMD_FAIL;
                }
                rv = WRITE_NBI_STATISTICS_RX_TOTAL_LENG_CNTr(unit, reg64);
                if(rv != SOC_E_NONE) {
                    return CMD_FAIL;
                }
            }
            if (SOC_IS_QAX(unit)) {
                rv = WRITE_CGM_VOQ_SRAM_ENQ_BYTE_CTRr(unit, SOC_CORE_ALL, reg64); 
                if(rv != SOC_E_NONE) {
                    return CMD_FAIL;
                }
                rv = WRITE_CGM_VOQ_SRAM_ENQ_PKT_CTRr(unit, SOC_CORE_ALL, 0);
                if(rv != SOC_E_NONE) {
                    return CMD_FAIL;
                }
            } else {
                rv = WRITE_IQM_ENQUEUE_BYTE_COUNTERr(unit, core, reg64); 
                if(rv != SOC_E_NONE) {
                    return CMD_FAIL;
                }
                rv = WRITE_IQM_ENQUEUE_PACKET_COUNTERr(unit, core, 0);
                if(rv != SOC_E_NONE) {
                    return CMD_FAIL;
                }
            }

            /* Enable and Trigger gtimer */
            if ((rv = dpp_diag_gtimer_blocks_start(unit, block_names_ingress, block_num, interval)) == CMD_FAIL) {
                return CMD_FAIL;
            }

            /* read NBI counters */
#ifdef BCM_JERICHO_SUPPORT
            if (SOC_IS_JERICHO(unit)) {
                uint32 pckts_num32;
                if(SOC_IS_QUX(unit)){
                    rv = READ_NIF_RX_TOTAL_PKT_COUNTERr(unit, &pckts_num32);
                    if(rv != SOC_E_NONE) {
                        return CMD_FAIL;
                    }
                    rv = READ_NIF_RX_TOTAL_BYTE_COUNTERr(unit, &bytes_num64);
                    if(rv != SOC_E_NONE) {
                        return CMD_FAIL;
                    }
                } else {
                    rv = READ_NBIH_RX_TOTAL_PKT_COUNTERr_REG32(unit, &pckts_num32);
                    if(rv != SOC_E_NONE) {
                        return CMD_FAIL;
                    }
                    rv = READ_NBIH_RX_TOTAL_BYTE_COUNTERr_REG64(unit, &bytes_num64);
                    if(rv != SOC_E_NONE) {
                        return CMD_FAIL;
                    }
                }

                /* unset last bit in the counters (gtimer indication bit) */
                SHR_BITCLR(&pckts_num32, 31);
                COMPILER_64_BITCLR(bytes_num64, 61);
                COMPILER_64_SET(pckts_num64, 0, pckts_num32);
            } else
#endif /*BCM_JERICHO_SUPPORT*/
            {
                rv = READ_NBI_STATISTICS_RX_BURSTS_OK_CNTr(unit, &pckts_num64);
                if(rv != SOC_E_NONE) {
                    return CMD_FAIL;
                }
                rv = READ_NBI_STATISTICS_RX_TOTAL_LENG_CNTr(unit, &bytes_num64);
                if(rv != SOC_E_NONE) {
                    return CMD_FAIL;
                }
            }

            if (COMPILER_64_IS_ZERO(pckts_num64)) {
                avg_bytes_num1 = 0;
            } else {
                COMPILER_64_UDIV_64(bytes_num64, pckts_num64);
                COMPILER_64_TO_32_LO(avg_bytes_num1, bytes_num64);
                avg_bytes_num1 += hdr_size;
            }

            if (SOC_IS_QAX(unit)) {
                rv = READ_CGM_VOQ_SRAM_ENQ_PKT_CTRr(unit, SOC_CORE_ALL, &pckts_num32);
                if(rv != SOC_E_NONE) {
                    return CMD_FAIL;
                }
                rv = READ_CGM_VOQ_SRAM_ENQ_BYTE_CTRr(unit, SOC_CORE_ALL, &bytes_num64);
                if(rv != SOC_E_NONE) {
                    return CMD_FAIL;
                }
            } else {
                /* read IQM counters */
                rv = READ_IQM_ENQUEUE_PACKET_COUNTERr(unit, core, &pckts_num32);
                if(rv != SOC_E_NONE) {
                    return CMD_FAIL;
                }
                rv = READ_IQM_ENQUEUE_BYTE_COUNTERr(unit, core, &bytes_num64);
                if(rv != SOC_E_NONE) {
                    return CMD_FAIL;
                }
            }

            /* unset last bit in the counters (gtimer indication bit) */
            if (SOC_IS_JERICHO(unit)) {
                SHR_BITCLR(&pckts_num32, 31);
                COMPILER_64_BITCLR(bytes_num64, 36);
            }

            if (pckts_num32 == 0) {
                avg_bytes_num2 = 0;
                avg_bytes_num1 = 0;
            } else {
                COMPILER_64_SET(pckts_num64, 0, pckts_num32);
                COMPILER_64_UDIV_64(bytes_num64, pckts_num64);
                COMPILER_64_TO_32_LO(avg_bytes_num2, bytes_num64);
            }

            /* stop gtimer*/
            if ((rv = dpp_diag_gtimer_blocks_stop(unit, block_names_ingress, block_num))== CMD_FAIL) {
                return CMD_FAIL;
            }

            avg_bytes_num2 -= avg_bytes_num1;
            cli_out("Core %d: Ingress header diff: %d\n", core, (int)avg_bytes_num2);
        /*
         * Make sure at least one of the strings is NULL terminated.
         */
        } else if (!sal_strncasecmp(subcmd, "Egress", strnlen_subcmd)) {
            /* init counters */
            COMPILER_64_ZERO(reg64);
            rv = WRITE_EPNI_EPE_BYTES_COUNTERr(unit, core, reg64);
            if(rv != SOC_E_NONE) {
                return CMD_FAIL;
            }
            rv = WRITE_EPNI_EPE_PACKET_COUNTERr(unit, core, reg64);
            if(rv != SOC_E_NONE) {
                return CMD_FAIL;
            }
            rv = WRITE_EGQ_PQP_UNICAST_BYTES_COUNTERr(unit, core, reg64);
            if(rv != SOC_E_NONE) {
                return CMD_FAIL;
            }
            rv = WRITE_EGQ_PQP_UNICAST_PACKET_COUNTERr(unit, core, reg64);
            if(rv != SOC_E_NONE) {
                return CMD_FAIL;
            }

            /* Enable and Trigger gtimer */
            if ((rv = dpp_diag_gtimer_blocks_start(unit, block_names_egress, block_num, interval)) == CMD_FAIL) {
                return CMD_FAIL;
            }

            /* read EPNI counters */
            rv = READ_EPNI_EPE_PACKET_COUNTERr(unit, core, &pckts_num64);
            if(rv != SOC_E_NONE) {
                return CMD_FAIL;
            }
            rv = READ_EPNI_EPE_BYTES_COUNTERr(unit, core, &bytes_num64);
            if(rv != SOC_E_NONE) {
                return CMD_FAIL;
            }

            if (COMPILER_64_IS_ZERO(pckts_num64)) {
                avg_bytes_num1 = 0;
            } else {
                COMPILER_64_UDIV_64(bytes_num64, pckts_num64);
                COMPILER_64_TO_32_LO(avg_bytes_num1, bytes_num64);
                avg_bytes_num1 += hdr_size;
            }

            /* read EGQ counters */
            rv = READ_EGQ_PQP_UNICAST_PACKET_COUNTERr(unit, core, &pckts_num64);
            if(rv != SOC_E_NONE) {
                return CMD_FAIL;
            }
            rv = READ_EGQ_PQP_UNICAST_BYTES_COUNTERr(unit, core, &bytes_num64);
            if(rv != SOC_E_NONE) {
                return CMD_FAIL;
            }

            if (COMPILER_64_IS_ZERO(pckts_num64)) {
                avg_bytes_num2 = 0;
            } else {
                COMPILER_64_UDIV_64(bytes_num64, pckts_num64);
                COMPILER_64_TO_32_LO(avg_bytes_num2, bytes_num64);
            }

            /* stop gtimer*/
            if ((rv = dpp_diag_gtimer_blocks_stop(unit, block_names_egress, block_num))== CMD_FAIL) {
                return CMD_FAIL;
            }

            avg_bytes_num1 -= avg_bytes_num2;
            cli_out("Core %d: Egress header diff: %d\n", core, (int)avg_bytes_num1);
        } else {
            return CMD_USAGE;
        }
    }

    return CMD_OK;
}

STATIC cmd_result_t
dpp_diag_prge_all(int unit, args_t *a) {
    if(arad_egr_prog_editor_print_all_programs_data(unit)){
        return CMD_FAIL;
    }
    return CMD_OK;
}



int dnx_diag_prge_last_info_xml_print(int unit, char *xml_file, char **program_name);
STATIC cmd_result_t
dpp_diag_prge_last(int unit, args_t *a) {

    uint32 first_instruction=0;
    int prm_core = -1;
    parse_table_t    pt;
    int              core_id, first_core, last_core;
    /*
    * For printing to xml file
    */
    char                            *prm_file = NULL;
    char                            *prog_name[SOC_DPP_DEFS_MAX(NOF_CORES)] = {NULL};
    uint8                           b_print_to_xml = 0;

    /* Parse input */
    parse_table_init(unit, &pt);
    parse_table_add(&pt, "core", PQ_DFL | PQ_INT, 0, &prm_core, NULL);
    parse_table_add(&pt, "file", PQ_DFL|PQ_STRING, 0,  &prm_file, NULL);

    if (0>parse_arg_eq(a, &pt)) {
        parse_arg_eq_done(&pt);
        return CMD_FAIL;
    }

    if ((prm_core < -1 ) || 
        (prm_core >= SOC_DPP_DEFS_GET(unit, nof_cores))) {
         cli_out("error ilegal core ID for device\n");
         parse_arg_eq_done(&pt);
         return CMD_FAIL;
    } 
    if ((prm_file != NULL) && (dbx_file_get_type(prm_file) == DBX_FILE_XML)) {
        b_print_to_xml = 1;
    }
    if ((prm_core == -1) || b_print_to_xml) { 
        /*
       * If printing to xml or core param not used, read from all cores.
       */
        first_core = 0;
        last_core = SOC_DPP_DEFS_GET(unit, nof_cores);
    } else {
        first_core = prm_core;
        last_core = prm_core+1;
    }

    for (core_id=first_core; core_id < last_core; core_id++) {
        if (!b_print_to_xml) {
            cli_out("%s",get_core_str(unit, core_id));
        }
        if (arad_pp_diag_prge_first_instr_get(unit, core_id, &first_instruction) != 0) {
            cli_out("Not found.\n");
            continue;
        }

        if(arad_egr_prog_editor_print_chosen_program(unit, first_instruction, FALSE /* NOT raw output */, &prog_name[core_id], b_print_to_xml) != SOC_E_NONE) {
            cli_out("Not found.\n");
            continue;
        }
    }
    if (b_print_to_xml) {
       /*
        * Print to XML file
        */
#if !defined(NO_FILEIO)
        dnx_diag_prge_last_info_xml_print(unit, prm_file, prog_name);
#endif
    }
    parse_arg_eq_done(&pt);
    return CMD_OK;
}

#if !defined(NO_FILEIO)
/*
 * Get the root node for the diags list and the top node in the file
 */
int dnx_diag_xml_root_get(int unit, char* xml_file, xml_node *curTop, xml_node *curRoot) {

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK((*curTop = dbx_xml_top_get(xml_file, "top", CONF_OPEN_CREATE)), _SHR_E_RESOURCE, "curTop");

    if (NULL == (*curRoot = dbx_xml_child_get_first(*curTop, "diags"))) {
        /*
         * diags node hasn't been created yet. Create it.
         */
        SHR_NULL_CHECK((*curRoot = dbx_xml_child_add(*curTop, "diags", 1)), _SHR_E_FAIL, "diags");
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * Print last packet(s) PRGE program selected (per core) to an xml file
 */
int dnx_diag_prge_last_info_xml_print(int unit, char *xml_file, char **program_name){

    int core = 0;
    xml_node curTop, curDiags, curDiag, curCores, curCore;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get diags root
     */
    SHR_IF_ERR_EXIT(dnx_diag_xml_root_get(unit, xml_file, &curTop, &curDiags));
    {
        /*
         * Add xml structure:
         *<diag name="prge_last">
         *   <cores list="Yes">
         *       <core id="0" program="ARAD_EGR_PROG_EDITOR_PROG_PP_COPY_HEADER"/>
         *      <core id="1" program="ARAD_EGR_PROG_EDITOR_PROG_PP_COPY_HEADER"/>
         *   </cores>
         *</diag>
         */

        /*
         * diag node
         */
        SHR_NULL_CHECK((curDiag = dbx_xml_child_add(curDiags, "diag", 2)), _SHR_E_FAIL, "curDiag");
        SHR_IF_ERR_EXIT(dbx_xml_property_set_str(curDiag, "name", "prge_last"));
        {
            /*
             * cores node
             */
            SHR_NULL_CHECK((curCores = dbx_xml_child_add(curDiag, "cores", 3)), _SHR_E_FAIL, "curCores");
            SHR_IF_ERR_EXIT(dbx_xml_property_set_str(curCores, "list", "Yes"));
            /*
             * core list
             */
            SOC_DPP_CORES_ITER(SOC_CORE_ALL, core) {
                /*
                 * core node
                 */
                SHR_NULL_CHECK((curCore = dbx_xml_child_add(curCores, "core", 4)), _SHR_E_FAIL, "curCore");
                SHR_IF_ERR_EXIT(dbx_xml_property_set_int(curCore, "id", core));              
                SHR_IF_ERR_EXIT(dbx_xml_property_mod_str(curCore, "program", program_name[core]));
                
            }
            dbx_xml_node_end(curCores, 3);
        }
        dbx_xml_node_end(curDiag, 2);
    }
    dbx_xml_node_end(curDiags, 1);

    dbx_xml_top_save(curTop, xml_file);
    dbx_xml_top_close(curTop);

exit:
    SHR_FUNC_EXIT;
}
#endif

STATIC cmd_result_t
dpp_diag_prge_mgmt(int unit, args_t *a) {
    ARAD_PP_PRGE_MGMT_DIAG_PARAMS key_params = {0};
    parse_table_t    pt;

    /* Parse input */
    parse_table_init(unit, &pt);
    parse_table_add(&pt, "graph", PQ_DFL | PQ_STRING, NULL, &key_params.graph, NULL);
    parse_table_add(&pt, "deploy", PQ_DFL | PQ_STRING, NULL, &key_params.deploy, NULL);

    if (parse_arg_eq(a, &pt) != 1) {
        parse_arg_eq_done(&pt);
        return CMD_USAGE;
    }

    if (arad_egr_prge_mgmt_diag_print(unit, &key_params) != 0) {
        parse_arg_eq_done(&pt);
        cli_out("Diag failed.\n");
    }

    parse_arg_eq_done(&pt);
    return 0;
}




#endif /*BCM_PETRA_SUPPORT*/
/*----------------------------------------------------------------------------------------------------*/


void
print_wb_usage(int unit) {
   char cmd_dpp_diag_wb_usage[] =
      "Usage (DIAG wb):"
      "\n\tDIAGnotsics warmboot commands\n\t"
      "Usages:\n\t"
      "DIAG wb <OPTION> <parameters> ..."
      "OPTION can be:"
      "\nDUMP - \tDUMP wb_engine variables."
      "\n\t optional Parameters:"
      "\n\t\t engine_id - dump vars from wb_engine with ID engine_id"
      "\n\t\t buffer_id - dump only vars from the buffer with id==buffer_id"
      "\n\t\t var_id -  - dump only var with id==var_id"
      "\n\t\t file - output to specified file instead of to screen"
      "\n";

   cli_out(cmd_dpp_diag_wb_usage);
}

#ifdef BCM_PETRA_SUPPORT
void
print_rate_usage(int unit) {
   char cmd_dpp_diag_rate_usage[] =
      "Usage (DIAG rates):"
      "\n\tDIAGnotsics rates commands\n\t"
      "Usages:\n\t"
      "DIAG rates <OPTION> <parameters> ..."
      "OPTION can be:"
      "\nEGQ - \tEGQ rates calculation."
      "\n\t Parameters required:"
      "\n\t\t port - local port id"
      "\n\t\t tc - traffic class index"

      "\nSCH PS-\tSCH port rates calculation."
      "\n\t Parameters required:"
      "\n\t\t port - local port id"

      "\nSCH FLOW-\tSCH flow rates calculation."
      "\n\t Parameters required:"
      "\n\t\t flowid - flow id"

      "\nPQP - \tPQP rates calculation"
      "\n\t Parameters required:"
      "\n\t\t port - local port id"
      "\n\t\t tc - traffic class index"

      "\nEPEP - \tEPE port rates calculation"
      "\n\t Parameters required:"
      "\n\t\t port - local port id"
      "\n\t\t tc - traffic class index"

      "\nEPNI - \tEPNI rates calculation"
      "\n\t Parameters required:"
      "\n\t\t scheme - measure bw scheme (0=measure total, 1=bw on interface, 2=bw on port, 3=bw on Q-pair, 4=bw on channel, 5=bw for mirror/not mirror (according to [bw]))"
      "\n\t\t [bw] - bw id (if not specified uses 0 as default)"

     "\n\nIRE - \tIRE rates calculation."
      "\n\t Parameters required:"
      "\n\t\t port - local port id"
      "\n";

#ifndef COMPILER_STRING_CONST_LIMIT
   char cmd_dpp_diag_rate_additional_usage_jer_and_before [] = 
     "IQM - \tIQM rates calculation."
     "\n\t Parameters required:"
     "\n\t\t port - local port id"
     "\nIPT - \tIPT rates calculation."
     "\n";

   char cmd_dpp_diag_rate_additional_usage_qax [] = 
     "CGM - \tCGM rates calculation." 
     "\nPTS - \tPTS rates calculation."
     "\nTXQ - \tTXQ rates calculation."
     "\n";
#endif /* COMPILER_STRING_CONST_LIMIT*/

   cli_out(cmd_dpp_diag_rate_usage);
#ifndef COMPILER_STRING_CONST_LIMIT
   if (SOC_IS_QAX(unit))
     cli_out(cmd_dpp_diag_rate_additional_usage_qax);
   else 
     cli_out(cmd_dpp_diag_rate_additional_usage_jer_and_before);
#endif
}

#endif /* BCM_PETRA_SUPPORT */

cmd_result_t
cmd_dpp_deprecated(int unit, char* new_command)
{
    cli_out("This command is deprecated. Please use the following command:\n\t%s\n", new_command);
    return CMD_OK;
}


cmd_result_t
cmd_dpp_diag(int unit, args_t *a) {
    char      *function;
    int strnlen_function;

    cmd_result_t result;
    uint32 str_to_func_array_size, field_idx;

    const diag_dnx_table_t diag_str_to_function_array[] = {
    {"wb",           cmd_dpp_diag_wb,            print_wb_usage   ,             DNX_DIAG_ID_UNAVAIL},
#ifdef BCM_PETRA_SUPPORT
    {"pp",           cmd_dpp_diag_pp,            print_pp_usage   ,             DNX_DIAG_ID_UNAVAIL},
    {"field",        cmd_dpp_diag_field,         print_field_usage,             DNX_DIAG_ID_UNAVAIL},
    {"rates",        cmd_dpp_diag_rate,          print_rate_usage ,             DNX_DIAG_ID_UNAVAIL},
    {"oam",          cmd_dpp_diag_oam,           print_oam_usage  ,             DNX_DIAG_ID_UNAVAIL},
    {"alloc",        cmd_dpp_diag_alloc,         print_alloc_usage,             DNX_DIAG_ID_UNAVAIL},
    {"template",     cmd_dpp_diag_tmplt,         print_tmplt_usage,             DNX_DIAG_ID_UNAVAIL},
    {"last_packet",  dpp_diag_last_packet_info,  NULL,                          DNX_DIAG_ID_UNAVAIL},
    {"HeaderDiff",   cmd_diag_header_diff,       print_header_diff_usage,       DNX_DIAG_ID_UNAVAIL},
    {"buffers",      cmd_dpp_diag_buffers,       cmd_dpp_diag_buffers_usage,    DNX_DIAG_ID_UNAVAIL},
    {"mmu",          cmd_dpp_diag_mmu,           cmd_dpp_diag_mmu_usage,        DNX_DIAG_ID_UNAVAIL},
    {"dbal",         cmd_dpp_diag_dbal,          print_dbal_usage,              DNX_DIAG_ID_UNAVAIL},
#endif

#ifdef INCLUDE_IPSEC
    {"ipsec",        cmd_dpp_diag_ipsec,         print_ipsec_usage,             DNX_DIAG_ID_UNAVAIL}
#endif
    };


    function = ARG_GET(a);
    if (!function) {
       return CMD_USAGE;
    }
    strnlen_function = sal_strnlen(function, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH);
#ifdef BCM_PETRA_SUPPORT
#endif /* BCM_PETRA_SUPPORT */
    if (!sal_strncasecmp(function, "reachability", strnlen_function)) {

        cli_out("'diag reachabilty' is supported by the fabric diag pack.\n"
                "type 'fabric usage' for additional information.\n");
        return CMD_OK;

    } else if (!sal_strncasecmp(function, "connectivity", strnlen_function)) {
        cli_out("'diag connectivity' is supported by the fabric diag pack.\n"
                "type 'fabric usage' for additional information.\n");
        return CMD_OK;
    } else if (!sal_strncasecmp(function, "link", strnlen_function)) {
        cli_out("'diag link' is supported by the fabric diag pack.\n"
                "type 'fabric usage' for additional information.\n");
        return CMD_OK;
    }
   
    str_to_func_array_size = sizeof(diag_str_to_function_array)/sizeof(diag_str_to_function_array[0]);
     
    for (field_idx = 0; field_idx < str_to_func_array_size; field_idx++) {
        if ((!sal_strncasecmp(function, (diag_str_to_function_array[field_idx].module_char), strnlen_function))) {
            result = diag_str_to_function_array[field_idx].action_func(unit, a);
            if (result == CMD_USAGE && diag_str_to_function_array[field_idx].usage_func != NULL) {         
                diag_str_to_function_array[field_idx].usage_func(unit);
                return CMD_FAIL;
            }
            return result;
        }
    }

    /*
     * Make sure at least one of the strings is NULL terminated.
     */
    if (!sal_strncasecmp(function, "counters", strnlen_function)) {
          return dpp_diag_counters(unit, a);
       } else if (!sal_strncasecmp(function, "cosq", strnlen_function)) {
          return dpp_diag_cosq(unit, a);
       } else if (!sal_strncasecmp(function, "fc", strnlen_function)) {
          return dpp_diag_fc(unit, a);
       } else if (!sal_strncasecmp(function, "fabric", strnlen_function)) {
          return dpp_diag_fabric(unit, a);
       } else if (!sal_strncasecmp(function, "queues", strnlen_function)) {
           cli_out("'diag queues' is supported by the fabric diag pack.\n"
                   "type 'fabric usage' for additional information.\n");
           return CMD_OK;
       }

#ifdef BCM_PETRA_SUPPORT
    if (!sal_strncasecmp(function, "pll", strnlen_function)) {
      if (SOC_IS_ARAD(unit)) {
         return cmd_dpp_pll(unit, a);
      } else {
         return CMD_USAGE;
      }
    } else if (!sal_strncasecmp(function, "lag", strnlen_function)) {
      if (SOC_IS_ARAD(unit)) {
         return cmd_dpp_lag(unit, a);
      } else {
         return CMD_USAGE;
      }
    } else if (!sal_strncasecmp(function, "nif", strnlen_function)) {
      if (SOC_IS_ARAD(unit)) {
         return cmd_dpp_nif(unit, a);
      } else {
         return CMD_USAGE;
      }
    } else if (!sal_strncasecmp(function, "port_db", strnlen_function)) {
      if (SOC_IS_ARAD(unit)) {
         return cmd_dpp_port_db(unit, a);
      } else {
         return CMD_USAGE;
      }
    } else if (!sal_strncasecmp(function, "retransmit", strnlen_function)) {
      if (SOC_IS_ARAD(unit)) {
         return cmd_dpp_retransmit(unit, a);
      } else {
         return CMD_USAGE;
      }
    } else if (!sal_strncasecmp(function, "multicast", strnlen_function)) {
      if (SOC_IS_ARAD(unit)) {
         return cmd_dpp_multicast(unit, a);
      } else {
         return CMD_USAGE;
      }
    } else if (!sal_strncasecmp(function, "prge_info", strnlen_function)) {
      if (SOC_IS_ARAD(unit)) {
         return dpp_diag_prge_all(unit, a);
      } else {
         return CMD_USAGE;
      }
    } else if (!sal_strncasecmp(function, "prge_last", strnlen_function)) {
      if (SOC_IS_ARAD(unit)) {
         return dpp_diag_prge_last(unit, a);
      } else {
         return CMD_USAGE;
      }
    } else if (!sal_strncasecmp(function, "prge_mgmt", strnlen_function)) {
      if (SOC_IS_ARAD(unit)) {
         return dpp_diag_prge_mgmt(unit, a);
      } else {
         return CMD_USAGE;
      }
    } else if (!sal_strncasecmp(function, "egr_congestion", strnlen_function)) {
      return dpp_diag_egr_congestion(unit, a);
    } else if (!sal_strncasecmp(function, "ing_congestion", strnlen_function)) {
      return dpp_diag_ing_congestion(unit, a);
    } else if (! sal_strncasecmp(function, "reassembly_context", strnlen_function) ) {
        if (SOC_IS_DNX(unit)) {
            return cmd_dpp_deprecated(unit, "tm ingress reassembly");
        }
        return dpp_diag_reassembly_context(unit, a);
    } else if (! sal_strncasecmp(function, "egr_calendars", strnlen_function) ) {
        return dpp_diag_egr_calendars(unit, a);
    }else if (!sal_strncasecmp(function, "mem-info", strnlen_function)) {
        return dpp_diag_mem_info(unit, a);
    }else if (!sal_strncasecmp(function, "tcam", strnlen_function)) {
        return dpp_diag_tcam(unit, a);
    }
#ifdef BCM_JERICHO_SUPPORT
    else if (! sal_strncasecmp(function, "resource_allocation", strnlen_function) ) {
        return dpp_diag_resource_allocation(unit, a);
    }
#endif /* BCM_JERICHO_SUPPORT */
#if ARAD_DEBUG
     else if (!sal_strncasecmp(function, "dump_signals", strnlen_function)) {
      return dpp_diag_dump_signals(unit, a);
    }
#endif
#endif   

#ifdef DUNE_UI
    else if (!sal_strncasecmp(function, "dune_ui_free_bits", strnlen_function)) {
      dune_ui_module_free_bits();
      return 1;
    }
#endif   
    else if (! sal_strncasecmp(function, "th", strnlen_function) ) {
           cli_out("'diag th' is supported by the fabric diag pack.\n"
                   "type 'fabric usage' for additional information.\n");
           return CMD_OK;
    }

    else {
      return CMD_USAGE;
    }
}


/*Diag pack infrastructure: start */

/*
 * Function: 
 *      diag_dnx_usage_print 
 * Purpose: 
 *      print the usage of all the supported commands by this unit. 
 * Parameters: 
 *      unit            - (IN) Unit number.
 *      diag_pack       - (IN) Diag pack table.
 *      diag_id_support - (IN) Callback to a function that checks if diag_id supported.
 */
void 
diag_dnx_usage_print(int unit, 
                     const diag_dnx_table_t *diag_pack,
                     diag_dnx_diag_id_supported_func_ptr_t diag_id_support)
{
    const diag_dnx_table_t *diag_pack_entry;

    /*Iterate over diag pack table*/
    for (diag_pack_entry = diag_pack; diag_pack_entry->action_func != NULL; diag_pack_entry++)
    {
        /*Call to usage func iff diag_id is supported by the device and usage function exist*/
        if (diag_id_support(unit, diag_pack_entry->diag_id) && 
            diag_pack_entry->usage_func != NULL)
        {
            diag_pack_entry->usage_func(unit);
            cli_out("-\n");
        }
    }
}

/*
 * Function: 
 *      diag_dnx_usage_print 
 * Purpose: 
 *      Dispatch according to command name and if the diag_id is supported. 
 * Parameters: 
 *      unit            - (IN) Unit number.
 *      diag_pack       - (IN) Diag pack table.
 *      diag_id_support - (IN) Callback to a function that checks if diag_id supported.
 *      cmd_name        - (IN) Command full name.
 *      args            - (IN) args required by the command.
 */
cmd_result_t 
diag_dnx_dispatch(int unit,
                  const diag_dnx_table_t *diag_pack,
                  diag_dnx_diag_id_supported_func_ptr_t diag_id_support,
                  char *cmd_name,
                  args_t *args)
{
    const diag_dnx_table_t *diag_pack_entry;
    cmd_result_t res;

    if (cmd_name == NULL)
    {
        return CMD_USAGE;
    }

    /*Iterate over diag pack table*/
    for (diag_pack_entry = diag_pack; diag_pack_entry->action_func != NULL && diag_pack_entry->module_char != NULL; diag_pack_entry++)
    {
        /*Call to the first diag that is supported and match to cmd name*/
        if((!sal_strncasecmp(cmd_name, (diag_pack_entry->module_char), sal_strnlen(cmd_name, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH))) && 
           diag_id_support(unit, diag_pack_entry->diag_id)) 
        {
            res = diag_pack_entry->action_func(unit, args);

            /*If usage error -> print usage info (if exist)*/
            if (res == CMD_USAGE && diag_pack_entry->usage_func != NULL) {         
                diag_pack_entry->usage_func(unit);
                return CMD_FAIL;
            }
            return res;
        }
    }

    return CMD_USAGE;
}
            
/*Diag pack infrastructure: end*/
      
