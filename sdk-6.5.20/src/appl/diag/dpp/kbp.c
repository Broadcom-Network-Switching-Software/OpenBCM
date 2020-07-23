/*
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#if defined(INCLUDE_KBP)

/*************
 * INCLUDES  *
 */

#include <shared/bsl.h>
#include <shared/swstate/access/sw_state_access.h>
#include <sal/appl/sal.h>

#include <soc/i2c.h>

#include <soc/mcm/memregs.h>

#include <soc/dpp/SAND/Management/sand_low_level.h>

#include <soc/dpp/ARAD/arad_kbp.h>
#include <soc/dpp/ARAD/arad_kbp_rop.h>
#include <soc/dpp/ARAD/arad_kbp_recover.h>

#include <bcm_int/dpp/error.h>
#include <bcm_int/dpp/field_int.h>

#include <appl/diag/system.h>
#include <appl/diag/shell.h>
#include <appl/diag/bslfile.h>
#include <appl/diag/dpp/kbp.h>

#ifdef KBP_SW_TEST
#include <soc/dpp/JER/kbp_app.h>
#endif /* KBP_SW_TEST */

#include <appl/dcmn/rx_los/rx_los.h>

#include <soc/dpp/PPD/ppd_api_diag.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_fp.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_diag.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_sw_db.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_flp_init.h>
#include <soc/dpp/JER/JER_PP/jer_pp_kaps_diag.h>
#include <soc/dpp/JER/JER_PP/jer_pp_kaps_entry_mgmt.h>
#include <soc/dpp/JER/JER_PP/jer_pp_kaps_xpt.h>
#include <soc/dpp/JER/jer_sbusdma_desc.h>
#include <soc/phyreg.h>

#include <soc/dpp/ARAD/arad_general.h>
#include <soc/dpp/ARAD/arad_kbp_xpt.h>
#include <soc/dpp/ARAD/arad_kbp_recover.h>

/*************
 * DEFINES   *
 */

#define BYTE_0(x)  ((x)  & 0x000000FF)
#define BYTE_1(x)  (((x) & 0x0000FF00)>>8)
#define BYTE_2(x)  (((x) & 0x00FF0000)>>16)
#define BYTE_3(x)  (((x) & 0xFF000000)>>24)

#define CORE_DEFAULT    0

#define KBP_CMD_DPP_RESET_DEVICE_ACTION_BEFORE 0x1
#define KBP_CMD_DPP_RESET_DEVICE_ACTION_AFTER  0x2
#define KBP_CMD_DPP_RESET_DEVICE_ACTION_BOTH   0x3

#define ARAD_KBP_FRWRD_AND_ACL_BUFFER_SIZE_BITS 1024

static FILE *kbp_file_fp[SOC_MAX_NUM_DEVICES];
static FILE *kaps_file_fp[SOC_MAX_NUM_DEVICES];

#define UINT64_SIZE_IN_BITS 64
#define UINT32_SIZE_IN_BITS 32
#define UINT8_SIZE_IN_BITS 8

#define KEY_NAME_CHAR_LENGTH 20

#define KBP_CMD_DPP_KAPS_ARM_IMAGE_LOAD_BUF_SIZE 5

#define KBP_IPV4_INSERTION_RATE_LIMIT	4500
#define KBP_ACL_INSERTION_RATE_LIMIT	130

#define ARAD_KBP_ROP_STABILITY_TEST_LOOPS    10000

extern genericTblMgrAradAppData *AradAppData[SOC_SAND_MAX_DEVICE];
#ifdef BCM_JERICHO_SUPPORT
extern genericJerAppData *JerAppData[SOC_SAND_MAX_DEVICE];
#endif

extern int flexible_mode_ref_app(int unit, bcm_switch_concat_header_t concat_header, uint8 verbosity_level, uint8 tap_init);

/*************
* FUNCTIONS *
 */

#ifdef KBP_SW_TEST
STATIC cmd_result_t
cmd_kbp_v1(int uint, args_t *a)
{
     int ret = CMD_OK;
     ret = v1_main((a->a_argc-1),&a->a_argv[1]);
     return ret;
}

/*STATIC cmd_result_t
cmd_kbp_custom_rate()
{
     int ret = CMD_OK;
     ret = custom_update_rate_lpm_test();
     return ret;
}
*/
#endif /* KBP_SW_TEST */

extern ARAD_KBP_DEV_HANDLERS dev_handlers_info;

/* This function parse a string of digits to array of uint8 hexadesimal numbers  
 * Example : if data_string is "12345678" (and parse_data_length=8), will return parsed_data = {0x12, 0x34, 0x56, 0x78}
 */
STATIC
int cmd_dpp_kbp_parse_string_of_data(int unit, char* data_string, uint8* parsed_data, uint32 parse_data_length /*in bytes*/)
{
    char temp[5] = "0x00";
    int i;

    if(data_string != NULL && *data_string == '0' && 
    ( *(data_string+1) == 'x' || *(data_string+1) == 'X')) {
        data_string += 2;
    }

    for(i=0; i<parse_data_length; ++i) {
        if( data_string != NULL && *data_string != 0 && *(data_string +1)!=0) {
            temp[2]=*data_string;
            temp[3]=*(data_string + 1);
            parsed_data[i] = (uint8)parse_integer(temp);
            data_string += 2;
        } else {
            return -1;
        }        
    }
    
    return 0;

}

STATIC int 
 dpp_kbp_ipv4_print_timers(int num_of_entries)
{
	uint32
	  cnt_ndx,i;
    uint32
        total_time_32bit;
	COMPILER_UINT64
	  total_time_1000, total_time_100, timer_total ;
	double rate = -1;

	LOG_CLI((BSL_META("\r\n")));
	if (Soc_sand_ll_timer_total == 0){
        LOG_CLI((BSL_META("No timers were hit, total measured execution time: 0\n\r")));
	}else{
        LOG_CLI((BSL_META(" KBP IPV4 Timers Measurements by layers (net time per timer) \n\r")));
        LOG_CLI((BSL_META(" +-----------------------------------------------------------------------------------------+\n\r")));
        LOG_CLI((BSL_META(" | Timer Name                             |Hit Count |Total Time[us] |Per Hit[us] |Percent |\n\r")));
        LOG_CLI((BSL_META(" +-----------------------------------------------------------------------------------------+\n\r")));

        COMPILER_64_SET(timer_total, 0, Soc_sand_ll_timer_cnt[ARAD_KBP_TIMERS_APPL].total_time);
        total_time_32bit = Soc_sand_ll_timer_cnt[ARAD_KBP_TIMERS_APPL].total_time;
        for (cnt_ndx = ARAD_KBP_IPV4_TIMERS_NOF_TIMERS-1; cnt_ndx >= ARAD_KBP_TIMERS_APPL; cnt_ndx--){

            if (Soc_sand_ll_timer_cnt[cnt_ndx].nof_hits != 0){

				if (cnt_ndx > ARAD_KBP_TIMERS_SOC) {
                    /* Its a kbp timer -
                     *  Reduce its total time from the higher layres total time
                     */
					for (i=0;i<=ARAD_KBP_TIMERS_SOC;i++) {
                        Soc_sand_ll_timer_cnt[i].total_time -= Soc_sand_ll_timer_cnt[cnt_ndx].total_time;
					}
				}else{
                    /* Its a soc/bcm/full timer -
                     *  Reduce its total time from the higher layres total time
                     */
					for (i=ARAD_KBP_TIMERS_APPL;i<cnt_ndx;i++) {
                        Soc_sand_ll_timer_cnt[i].total_time -= Soc_sand_ll_timer_cnt[cnt_ndx].total_time;
					}
                }
				COMPILER_64_SET(total_time_1000, 0, Soc_sand_ll_timer_cnt[cnt_ndx].total_time);
                COMPILER_64_SET(total_time_100, 0, Soc_sand_ll_timer_cnt[cnt_ndx].total_time);
                COMPILER_64_UMUL_32(total_time_1000,1000);
                COMPILER_64_UMUL_32(total_time_100,100);
                COMPILER_64_UDIV_64(total_time_1000,timer_total);
                COMPILER_64_UDIV_64(total_time_100,timer_total);
                LOG_CLI((BSL_META(" |%-40s| %-9d|%-15d|%-12d|%3d.%1d%%  |\n\r"), 
                        Soc_sand_ll_timer_cnt[cnt_ndx].name, 
                        Soc_sand_ll_timer_cnt[cnt_ndx].nof_hits,
                        Soc_sand_ll_timer_cnt[cnt_ndx].total_time, 
                        Soc_sand_ll_timer_cnt[cnt_ndx].total_time / Soc_sand_ll_timer_cnt[cnt_ndx].nof_hits,
                        COMPILER_64_LO(total_time_100),
                        COMPILER_64_LO(total_time_1000) - COMPILER_64_LO(total_time_100)*10
                        ));

                LOG_CLI((BSL_META(" +-----------------------------------------------------------------------------------------+\n\r")));
            }
        }
        total_time_32bit -= Soc_sand_ll_timer_cnt[ARAD_KBP_TIMERS_APPL].total_time;
        LOG_CLI((BSL_META(" Total Time for %d Entries: %u[us]\n\r"), num_of_entries,total_time_32bit));
        LOG_CLI((BSL_META(" Total Time per Entry: %8.3f[us]\n\r"), (double)total_time_32bit/(double)num_of_entries));
        rate = (1000000.0 * (double) num_of_entries)/(double) total_time_32bit;
        LOG_CLI((BSL_META(" Entries Insertion Rate: %.0f[1/sec]\n\r"), rate));
        LOG_CLI((BSL_META(" +_________________________________________________________________________________________+\n\r")));

    } /* Timer hits != 0 */
	return (int)rate;
}

STATIC int
 dpp_kbp_acl_print_timers(int num_of_entries)
{
	uint32
	  cnt_ndx,i;
    uint32
        total_time_32bit;
	COMPILER_UINT64
	  total_time_1000, total_time_100, timer_total ;
	double rate = -1;

	LOG_CLI((BSL_META("\r\n")));
	if (Soc_sand_ll_timer_total == 0){
        LOG_CLI((BSL_META("No timers were hit, total measured execution time: 0\n\r")));
	}else{
        LOG_CLI((BSL_META(" KBP ACL Timers Measurements by layers (net time per timer) \n\r")));
        LOG_CLI((BSL_META(" +-----------------------------------------------------------------------------------------+\n\r")));
        LOG_CLI((BSL_META(" | Timer Name                             |Hit Count |Total Time[us] |Per Hit[us] |Percent |\n\r")));
        LOG_CLI((BSL_META(" +-----------------------------------------------------------------------------------------+\n\r")));

        COMPILER_64_SET(timer_total, 0, Soc_sand_ll_timer_cnt[ARAD_KBP_TIMERS_APPL].total_time);
        total_time_32bit = Soc_sand_ll_timer_cnt[ARAD_KBP_TIMERS_APPL].total_time;
        for (cnt_ndx = ARAD_KBP_ACL_TIMERS_NOF_TIMERS-1; cnt_ndx >= ARAD_KBP_TIMERS_APPL; cnt_ndx--){

            if (Soc_sand_ll_timer_cnt[cnt_ndx].nof_hits != 0){

				if (cnt_ndx > ARAD_KBP_TIMERS_SOC) {
                    /* Its a kbp timer -
                     *  Reduce its total time from the higher layres total time
                     */
					for (i=0;i<=ARAD_KBP_TIMERS_SOC;i++) {
                        Soc_sand_ll_timer_cnt[i].total_time -= Soc_sand_ll_timer_cnt[cnt_ndx].total_time;
					}
				}else{
                    /* Its a soc/bcm/full timer -
                     *  Reduce its total time from the higher layres total time
                     */
					for (i=ARAD_KBP_TIMERS_APPL;i<cnt_ndx;i++) {
                        Soc_sand_ll_timer_cnt[i].total_time -= Soc_sand_ll_timer_cnt[cnt_ndx].total_time;
					}
                }
				COMPILER_64_SET(total_time_1000, 0, Soc_sand_ll_timer_cnt[cnt_ndx].total_time);
                COMPILER_64_SET(total_time_100, 0, Soc_sand_ll_timer_cnt[cnt_ndx].total_time);
                COMPILER_64_UMUL_32(total_time_1000,1000);
                COMPILER_64_UMUL_32(total_time_100,100);
                COMPILER_64_UDIV_64(total_time_1000,timer_total);
                COMPILER_64_UDIV_64(total_time_100,timer_total);
                LOG_CLI((BSL_META(" |%-40s| %-9d|%-15d|%-12d|%3d.%1d%%  |\n\r"), 
                        Soc_sand_ll_timer_cnt[cnt_ndx].name, 
                        Soc_sand_ll_timer_cnt[cnt_ndx].nof_hits,
                        Soc_sand_ll_timer_cnt[cnt_ndx].total_time, 
                        Soc_sand_ll_timer_cnt[cnt_ndx].total_time / Soc_sand_ll_timer_cnt[cnt_ndx].nof_hits,
                        COMPILER_64_LO(total_time_100),
                        COMPILER_64_LO(total_time_1000) - COMPILER_64_LO(total_time_100)*10
                        ));

                LOG_CLI((BSL_META(" +-----------------------------------------------------------------------------------------+\n\r")));
            }
        }
        total_time_32bit -= Soc_sand_ll_timer_cnt[ARAD_KBP_TIMERS_APPL].total_time;
        LOG_CLI((BSL_META(" Total Time for %d Entries: %u[us]\n\r"), num_of_entries,total_time_32bit));
        LOG_CLI((BSL_META(" Total Time per Entry: %8.3f[us]\n\r"), (double)total_time_32bit/(double)num_of_entries));
        rate = (1000000.0 * (double) num_of_entries)/(double) total_time_32bit;
        LOG_CLI((BSL_META(" Entries Insertion Rate: %.0f[1/sec]\n\r"), rate));
        LOG_CLI((BSL_META(" +_________________________________________________________________________________________+\n\r")));

    } /* Timer hits != 0 */
	return (int)rate;
}

STATIC cmd_result_t
cmd_dpp_kbp_cpu_record_send(int unit, args_t *a)
{
    soc_reg_above_64_val_t msb, lsb;
    parse_table_t pt; 
    cmd_result_t rv = CMD_OK;
    uint32 
        opcode,
        soc_sand_rv, core;
    int lsb_enable;
    
    SOC_REG_ABOVE_64_CLEAR(msb);
    SOC_REG_ABOVE_64_CLEAR(lsb);

    if (ARG_CNT(a) > 0) {
        parse_table_init(0,&pt);

        parse_table_add(&pt,"Core",PQ_INT , (void *) (0), &core, NULL);
            /* fill msb data */
        parse_table_add(&pt,"msb0",PQ_INT , (void *) (0), &msb[0], NULL);
        parse_table_add(&pt,"msb1",PQ_INT , (void *) (0), &msb[1], NULL);
        parse_table_add(&pt,"msb2",PQ_INT , (void *) (0), &msb[2], NULL);
        parse_table_add(&pt,"msb3",PQ_INT , (void *) (0), &msb[3], NULL);
        parse_table_add(&pt,"msb4",PQ_INT , (void *) (0), &msb[4], NULL);
        parse_table_add(&pt,"msb5",PQ_INT , (void *) (0), &msb[5], NULL);
        parse_table_add(&pt,"msb6",PQ_INT , (void *) (0), &msb[6], NULL);
        parse_table_add(&pt,"msb7",PQ_INT , (void *) (0), &msb[7], NULL);
        parse_table_add(&pt,"msb8",PQ_INT , (void *) (0), &msb[8], NULL);
        parse_table_add(&pt,"msb9",PQ_INT , (void *) (0), &msb[9], NULL);
        parse_table_add(&pt,"msb10",PQ_INT , (void *) (0), &msb[10], NULL);
        parse_table_add(&pt,"msb11",PQ_INT , (void *) (0), &msb[11], NULL);
        parse_table_add(&pt,"msb12",PQ_INT , (void *) (0), &msb[12], NULL);
        parse_table_add(&pt,"msb13",PQ_INT , (void *) (0), &msb[13], NULL);
        parse_table_add(&pt,"msb14",PQ_INT , (void *) (0), &msb[14], NULL);
        parse_table_add(&pt,"msb15",PQ_INT , (void *) (0), &msb[15], NULL);
            /* fill lsb data */
        parse_table_add(&pt,"lsb0",PQ_INT , (void *) (0), &lsb[0], NULL);
        parse_table_add(&pt,"lsb1",PQ_INT , (void *) (0), &lsb[1], NULL);
        parse_table_add(&pt,"lsb2",PQ_INT , (void *) (0), &lsb[2], NULL);
        parse_table_add(&pt,"lsb3",PQ_INT , (void *) (0), &lsb[3], NULL);
        parse_table_add(&pt,"lsb4",PQ_INT , (void *) (0), &lsb[4], NULL);
        parse_table_add(&pt,"lsb5",PQ_INT , (void *) (0), &lsb[5], NULL);
        parse_table_add(&pt,"lsb6",PQ_INT , (void *) (0), &lsb[6], NULL);
        parse_table_add(&pt,"lsb7",PQ_INT , (void *) (0), &lsb[7], NULL);
        parse_table_add(&pt,"lsb8",PQ_INT , (void *) (0), &lsb[8], NULL);
        parse_table_add(&pt,"lsb9",PQ_INT , (void *) (0), &lsb[9], NULL);
        parse_table_add(&pt,"lsb10",PQ_INT , (void *) (0), &lsb[10], NULL);
        parse_table_add(&pt,"lsb11",PQ_INT , (void *) (0), &lsb[11], NULL);
        parse_table_add(&pt,"lsb12",PQ_INT , (void *) (0), &lsb[12], NULL);
        parse_table_add(&pt,"lsb13",PQ_INT , (void *) (0), &lsb[13], NULL);
        parse_table_add(&pt,"lsb14",PQ_INT , (void *) (0), &lsb[14], NULL);
        parse_table_add(&pt,"lsb15",PQ_INT , (void *) (0), &lsb[15], NULL);

        parse_table_add(&pt,"OPcode",PQ_INT , (void *) (0), &opcode, NULL);

        parse_table_add(&pt,"LSBenable",PQ_INT , (void *) (0), &lsb_enable, NULL);
      
        if (parse_arg_eq(a, &pt) < 0) {
            cli_out("%s: Invalid option: %s\n", ARG_CMD(a), ARG_CUR(a));
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }
        parse_arg_eq_done(&pt);
    } else {
        return CMD_USAGE;
    }

    if (SOC_IS_ARADPLUS(unit)) {
        soc_sand_rv = aradplus_kbp_cpu_record_send(unit, core, opcode, msb, lsb, lsb_enable, NULL);
    } else
    {
        soc_sand_rv = arad_kbp_cpu_record_send(unit, opcode, msb, lsb, lsb_enable);
    }

    if(BCM_FAILURE(handle_sand_result(soc_sand_rv))) {
        cli_out("Error: arad_kbp_cpu_record_send(%d , 0x%x /* opcode*/, &msb_data, &lsb_data)\n", unit, opcode);
        return CMD_FAIL;
    }

    return rv;

}

STATIC cmd_result_t
cmd_dpp_kbp_rop_write(int unit, args_t *a)
{
    parse_table_t pt; 
    cmd_result_t rv = CMD_OK;
    char *data_str=NULL, *mask_str=NULL;
    uint32 addr, addr_short, nbo_addr, core;
    arad_kbp_rop_write_t wr_data;
    uint32 soc_sand_rv;
    int i;

    parse_table_init(0,&pt);
    if (ARG_CNT(a) > 0) {

        parse_table_add(&pt,"Core",PQ_INT , (void *) (0), &core, NULL);
        parse_table_add(&pt,"ADdr",PQ_INT , (void *) (0), &addr, NULL);
        parse_table_add(&pt,"ADdrShort",PQ_INT , (void *) (0), &addr_short, NULL);
        parse_table_add(&pt,"DAta",PQ_STRING , (void *) ("00000000000000000000"), &data_str, NULL);
        parse_table_add(&pt,"Mask",PQ_STRING , (void *) ("00000000000000000000"), &mask_str, NULL);
        parse_table_add(&pt,"ValidBit",PQ_INT , (void *) (0), &wr_data.vBit, NULL);
        parse_table_add(&pt,"WriteMode",PQ_INT , (void *) (0), &wr_data.writeMode, NULL); 
        parse_table_add(&pt,"loop",PQ_INT , (void *) (1), &wr_data.loop, NULL); 
        
        if (parse_arg_eq(a, &pt) < 0) {
            cli_out("%s: Invalid option: %s\n", ARG_CMD(a), ARG_CUR(a));
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }
    } else {
        return CMD_USAGE;
    }
    nbo_addr = soc_htonl(addr);
    sal_memcpy(wr_data.addr, &nbo_addr, sizeof(uint32));
    nbo_addr = soc_htonl(addr_short);
    sal_memcpy(wr_data.addr_short, &nbo_addr, sizeof(uint32));


    cli_out("\n");

    /* set DAta */
    if(cmd_dpp_kbp_parse_string_of_data(unit, data_str, wr_data.data, NLM_DATA_WIDTH_BYTES)  != 0 ) {
        cli_out("DAta didn't type correctly, please type 20 digits, with no spaces.\n"
                "Example: DAta=0x00000000001234567890\n");
        parse_arg_eq_done(&pt);
        return CMD_FAIL;
    }
    /* set Mask */
    if(mask_str[0] != 0) {
       if(cmd_dpp_kbp_parse_string_of_data(unit, mask_str, wr_data.mask, NLM_DATA_WIDTH_BYTES)  != 0 ) {
            cli_out("Mask didn't type correctly, please type 20 digits, with no spaces.\n"
                    "Example: Mask=00000000001234567890\n");
            parse_arg_eq_done(&pt);
            return CMD_FAIL;
        } 
    } else {
        sal_memset(wr_data.mask, 0x0, NLM_DATA_WIDTH_BYTES);
    }
    
    for (i=0; i < wr_data.loop; i++) {
        soc_sand_rv = arad_kbp_rop_write(unit, core, &wr_data); 
        if(BCM_FAILURE(handle_sand_result(soc_sand_rv))) {
            cli_out("Error: arad_kbp_rop_write(%d, &wr_data) \n", unit);
            parse_arg_eq_done(&pt);
            return CMD_FAIL;
        }
    }
    parse_arg_eq_done(&pt);
    return rv;

}

STATIC cmd_result_t
cmd_dpp_kbp_rop_read(int unit, args_t *a)
{
    parse_table_t pt; 
    cmd_result_t rv = CMD_OK;
    arad_kbp_rop_read_t rd_data;
    uint32 addr, nbo_addr, core;
    int i;
    uint32 soc_sand_rv;

    if (ARG_CNT(a) > 0) {
        parse_table_init(0,&pt);

        parse_table_add(&pt,"Core",PQ_INT , (void *) (0), &core, NULL);
        parse_table_add(&pt,"ValidBit", PQ_INT , (void *) (0), &rd_data.vBit, NULL);
        parse_table_add(&pt,"TadaType", PQ_INT , (void *) (0), &rd_data.dataType, NULL);
        parse_table_add(&pt,"ADdr", PQ_INT , (void *) (0), &addr, NULL);

        if (parse_arg_eq(a, &pt) < 0) {
            cli_out("%s: Invalid option: %s\n", ARG_CMD(a), ARG_CUR(a));
            parse_arg_eq_done(&pt);
            return CMD_USAGE;

        }
        parse_arg_eq_done(&pt);

      
    } else {
        return CMD_USAGE;
    }

    nbo_addr = soc_htonl(addr);
    sal_memcpy(rd_data.addr, &nbo_addr, sizeof(uint32));
  
    soc_sand_rv = arad_kbp_rop_read(unit, core, &rd_data);
    if(BCM_FAILURE(handle_sand_result(soc_sand_rv))) {
        cli_out("Error: arad_kbp_rop_read(%d, &rd_data) \n", unit);
        return CMD_FAIL;
    }

    cli_out("0x%02x%02x%02x%02x: ", rd_data.addr[0], rd_data.addr[1], rd_data.addr[2], rd_data.addr[3]);

    for(i=0; i<11; ++i) {
        cli_out("0x%02x ", rd_data.data[i]);
    }

    cli_out("\n");

    return rv;

}

STATIC cmd_result_t
cmd_dpp_kbp_rop_counter(int unit)
{
    int res;
    uint32 i, core = 0, kbp_port = 0;
    uint32_t flags = SOC_PHY_NOMAP | SOC_PHY_CLAUSE45;
    int num_of_cores = SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores;
    uint32_t data_32[4] = {0, 0, 0, 0};
    uint64 val64;
    int mdio_id = 0, addr[4];
    char        buf_val[32];
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    LOG_CLI((BSL_META_U(unit,
                        "\nInterlaken-ROP Statistics[%s]  \n"),(ARAD_KBP_IS_OP_OR_OP2 ? "Read clear" : "Accumulate")));
    LOG_CLI((BSL_META_U(unit,
                        "--------------------------------------------------------------------------------\n")));
    if (ARAD_KBP_IS_OP_OR_OP2) {
        mdio_id = SOC_DPP_CONFIG(unit)->arad->init.elk.kbp_user_data[0].kbp_mdio_id;

        /*PORT0_RX_ROP_REGS_ROP_RECS2CORE_PORT = 0x1a00a*/
        /*PORT1_RX_ROP_REGS_ROP_RECS2CORE_PORT = 0x1b00a*/
        addr[0] = 0x1a00a;
        addr[1] = 0x1b00a;
        for (kbp_port = 0; kbp_port < 2; kbp_port++ ) {
            for (i = 4; i > 0; i--) {
                res = arad_port_phy_reg_get(unit, mdio_id, flags, addr[kbp_port] + i - 1, &data_32[i-1]);
                SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
            }
            data_32[0] = (data_32[0] & 0xFFFF) | ((data_32[1] << 16) & 0xFFFF0000);
            data_32[1] = (data_32[2] & 0xFFFF) | ((data_32[3] << 16) & 0xFFFF0000);
            COMPILER_64_SET(val64, data_32[1], data_32[0]);
            format_uint64_decimal(buf_val, val64, ',');
            LOG_CLI((BSL_META_U(unit,
                                "KBP PORT[%d] RX ROP Records: %s, \n"), kbp_port, buf_val));
        }
        LOG_CLI((BSL_META_U(unit, "\n")));
        /* TX_ROP_REGS_ROP_REC_CNT_PORT0(2a018) */
        /* TX_ROP_REGS_ROP_REC_CNT_PORT1(2a01c) */
        /* TX_ROP_REGS_ROP_REC_CNT_PORT2(2a030) */
        /* TX_ROP_REGS_ROP_REC_CNT_PORT3(2a040) */
        addr[0] = 0x2a018;
        addr[1] = 0x2a01c;
        addr[2] = 0x2a030;
        addr[3] = 0x2a040;
        for (kbp_port = 0; kbp_port < 4; kbp_port++ ) {
            for (i = 4; i > 0; i--) {
                res = arad_port_phy_reg_get(unit, mdio_id, flags, addr[kbp_port] + i - 1, &data_32[i-1]);
                SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
            }
            data_32[0] = (data_32[0] & 0xFFFF) | ((data_32[1] << 16) & 0xFFFF0000);
            data_32[1] = (data_32[2] & 0xFFFF) | ((data_32[3] << 16) & 0xFFFF0000);
            COMPILER_64_SET(val64, data_32[1], data_32[0]);
            format_uint64_decimal(buf_val, val64, ',');
            LOG_CLI((BSL_META_U(unit,
                                "KBP PORT[%d] TX ROP Records: %s, \n"), kbp_port, buf_val));
        }

        /* TX_ROP_REGS_ROP_PKT_CNT_PORT0(2a008) */
        /* TX_ROP_REGS_ROP_PKT_CNT_PORT1(2a00c) */
        /* TX_ROP_REGS_ROP_PKT_CNT_PORT2(2a028) */
        /* TX_ROP_REGS_ROP_PKT_CNT_PORT3(2a038) */
        addr[0] = 0x2a008;
        addr[1] = 0x2a00c;
        addr[2] = 0x2a028;
        addr[3] = 0x2a038;
        for (kbp_port = 0; kbp_port < 4; kbp_port++ ) {
            for (i = 4; i > 0; i--) {
                res = arad_port_phy_reg_get(unit, mdio_id, flags, addr[kbp_port] + i - 1, &data_32[i-1]);
                SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
            }
            data_32[0] = (data_32[0] & 0xFFFF) | ((data_32[1] << 16) & 0xFFFF0000);
            data_32[1] = (data_32[2] & 0xFFFF) | ((data_32[3] << 16) & 0xFFFF0000);
            COMPILER_64_SET(val64, data_32[1], data_32[0]);
            format_uint64_decimal(buf_val, val64, ',');
            LOG_CLI((BSL_META_U(unit,
                                "KBP PORT[%d] TX ROP Packets: %s, \n"), kbp_port, buf_val));
        }

    } else {
        for (core = 0;core < num_of_cores; core++) {
            mdio_id = SOC_DPP_CONFIG(unit)->arad->init.elk.kbp_user_data[core].kbp_mdio_id;
            /*get rop records to core[15:0]  0x080488*/
            /*get rop records to core[31:16] 0x080489*/
            addr[0] = 0x80488;
            for (i = 2; i > 0; i--) {
                res = arad_port_phy_reg_get(unit, mdio_id, flags, addr[0] + i - 1, &data_32[i-1]);
                SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
            }
            data_32[0] = (data_32[0] & 0xFFFF) | ((data_32[1] << 16) & 0xFFFF0000);

            /*get rop replies to host[15:0] 0x08048c*/
            /*get rop records to host[31:16]0x08048d*/
            addr[0] = 0x8048c;
            for (i = 2; i > 0; i--) {
                res = arad_port_phy_reg_get(unit, mdio_id, flags, addr[0] + i - 1, &data_32[i-1]);
                SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
            }
            data_32[1] = (data_32[0] & 0xFFFF) | ((data_32[1] << 16) & 0xFFFF0000);

            LOG_CLI((BSL_META_U(unit,
                                "Interlaken-ROP Records to core[%d]: %d, \n"), core, data_32[0]));
            LOG_CLI((BSL_META_U(unit,
                                "Interlaken-ROP Replies to host[%d]: %d, \n\n"), core, data_32[1]));
        }
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_kbp_print_rop_counter()", (ARAD_KBP_IS_OP_OR_OP2 ? kbp_port: core),
                                 mdio_id);
}

STATIC cmd_result_t
cmd_dpp_kbp_rop_test(int unit, args_t *a)
{
    parse_table_t pt;
    int res = CMD_OK;
    uint32 i, core_index, loop;
    int core;

    ARAD_INIT_ELK *elk = &SOC_DPP_CONFIG(unit)->arad->init.elk;

    core = SOC_CORE_ALL;
    loop = ARAD_KBP_ROP_STABILITY_TEST_LOOPS;

    if (ARG_CNT(a) > 0) {
        parse_table_init(0,&pt);

        parse_table_add(&pt,"Core",PQ_INT , (void *)INT_TO_PTR(core), &core, NULL);
        parse_table_add(&pt,"Count",PQ_INT , (void *)INT_TO_PTR(loop), &loop, NULL);

        if (parse_arg_eq(a, &pt) < 0) {
            cli_out("%s: Invalid option: %s\n", ARG_CMD(a), ARG_CUR(a));
            parse_arg_eq_done(&pt);
            return CMD_USAGE;

        }
        parse_arg_eq_done(&pt);
    }
    /* validate core in range */
    if((core >= SOC_DPP_DEFS_GET(unit, nof_cores) || (core < 0)) &&
       (core != _SHR_CORE_ALL)) {
        cli_out("Error: Invalid core %d \n", core);
        return CMD_FAIL;
    }

    SOC_DPP_CORES_ITER(core, core_index) {
        for (i = 0; i < loop; i++)
        {
            res = arad_kbp_init_rop_test(unit, core_index);
            /* KBP Recovery */
            if (res != 0 ) {
                res = arad_kbp_recover_run_recovery_sequence(unit, core_index, elk->kbp_user_data, elk->kbp_recover_iter, NULL, 1);
                if (res != 0) {
                    cli_out("ROP stability test failed on core %d\n", core_index);
                    return CMD_FAIL;
                }
            }
        }
        cli_out("ROP stability test PASSED on core %d\n", core_index);
    }

    cli_out("ROP stability test was completed successfully \n");

    return res;
}

STATIC cmd_result_t
cmd_dpp_kbp_reset_device_action(int unit, uint32 action)
{
    cmd_result_t ret = CMD_OK;
#if (defined(__DUNE_GTO_BCM_CPU__) || defined(__DUNE_WRX_BCM_CPU__))
    uint32 before = 0x0, after = 0x0;

    if ((action == KBP_CMD_DPP_RESET_DEVICE_ACTION_BEFORE) || (action == KBP_CMD_DPP_RESET_DEVICE_ACTION_BOTH)) {
        before = 0x1;
    } 
    if ((action == KBP_CMD_DPP_RESET_DEVICE_ACTION_AFTER) || (action == KBP_CMD_DPP_RESET_DEVICE_ACTION_BOTH)) {
        after = 0x1;
    } 
    
    cli_out("%s(): Reset KBP action: before=%d, after=%d\n", FUNCTION_NAME(), before, after);

    if (before == 0x1) {
        /* Set both to Output */
        ret = cpu_i2c_write(0x40, 0x3, CPU_I2C_ALEN_LONG_DLEN_LONG, 0xc0);
        sal_usleep(1000);
        if( ret != CMD_OK){
            cli_out("Error in %s(): cpu_i2c_write(). FAILED !!!\n", FUNCTION_NAME());
            return ret;
        }

        /* Assert both to Low */
        ret = cpu_i2c_write(0x40, 0x2, CPU_I2C_ALEN_LONG_DLEN_LONG, 0xc0);
        if( ret != CMD_OK){
            cli_out("Error in %s(): cpu_i2c_write(). FAILED !!!\n", FUNCTION_NAME());
            return ret;
        }
        sal_usleep(2500);
        ret = cpu_i2c_write(0x40, 0x2, CPU_I2C_ALEN_LONG_DLEN_LONG, 0x0);
        if( ret != CMD_OK){
            cli_out("Error in %s(): cpu_i2c_write(). FAILED !!!\n", FUNCTION_NAME());
            return ret;
        }

        /* Wait for a minimum of 1ms after power rails are stable and de-assert SRST_L. */
        sal_usleep(2000);

        /* Assert SRST (gpio 6) to High (means DeAssert) (right button) */
        ret = cpu_i2c_write(0x40, 0x2, CPU_I2C_ALEN_LONG_DLEN_LONG, 0x40);
        sal_usleep(1000);
        if( ret != CMD_OK){
            cli_out("Error in %s(): cpu_i2c_write(). FAILED !!!\n", FUNCTION_NAME());
            return ret;
        }
    }

    if (after == 0x1) {
        /* Wait for a minimum of 1.5ms after de-assertion of SRST_L then de-assert CRST_L */
        sal_usleep(1500);

        /* Assert CRST (gpio 7) to High (means DeAssert) (left button) */
        ret = cpu_i2c_write(0x40, 0x2, CPU_I2C_ALEN_LONG_DLEN_LONG, 0xc0);
        sal_usleep(1000);
        if( ret != CMD_OK){
            cli_out("Error in %s(): cpu_i2c_write(). FAILED !!!\n", FUNCTION_NAME());
            return ret;
        }
    }

#endif
    return ret;
}


STATIC cmd_result_t
cmd_dpp_kbp_reset_device(int unit, args_t *a)
{
    parse_table_t pt; 
    cmd_result_t ret = CMD_OK;
    uint32 before = 0x1, after = 0x1;

    if (ARG_CNT(a) > 0) {
        parse_table_init(0,&pt);

        parse_table_add(&pt,"Before",PQ_INT , (void *) (1), &before, NULL);
        parse_table_add(&pt,"After",PQ_INT , (void *) (1), &after, NULL);
  
        if (parse_arg_eq(a, &pt) < 0) {
            cli_out("%s: Invalid option: %s\n", ARG_CMD(a), ARG_CUR(a));
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }
        parse_arg_eq_done(&pt);
    } 

    if (before == 0x1) {
        ret = cmd_dpp_kbp_reset_device_action(unit, KBP_CMD_DPP_RESET_DEVICE_ACTION_BEFORE);
        if( ret != CMD_OK){
            cli_out("Error in %s(): cmd_dpp_kbp_reset_device_action() - Before. FAILED !!!\n", FUNCTION_NAME());
            return ret;
        }
    }

    if (after == 0x1) {
        ret = cmd_dpp_kbp_reset_device_action(unit, KBP_CMD_DPP_RESET_DEVICE_ACTION_AFTER);
        if( ret != CMD_OK){
            cli_out("Error in %s(): cmd_dpp_kbp_reset_device_action() - After. FAILED !!!\n", FUNCTION_NAME());
            return ret;
        }
    }
   
    return ret;

}

STATIC int32_t
cmd_dpp_kbp_callback_reset_device(void *handle, int32_t s_reset_low, int32_t c_reset_low){
#if (defined(__DUNE_GTO_BCM_CPU__) || defined(__DUNE_WRX_BCM_CPU__))
    int val = 0, unit;
    cmd_result_t ret;
    int kbp_reset_address, kbp_c_reset_bit, kbp_s_reset_bit;
    ARAD_NIF_ELK_USER_DATA* user_data = handle;

    unit = user_data->device;

    if (SOC_IS_QAX(unit)) {
        kbp_c_reset_bit = 0;
        kbp_s_reset_bit = 4;
        kbp_reset_address = 0x2;
    } else if (SOC_IS_JERICHO_PLUS(unit) && ARAD_KBP_IS_OP_OR_OP2) {
        /* reset pin connections over RJ45 cable are swapped */
        if (user_data->kbp_mdio_id == SOC_DPP_CONFIG(unit)->arad->init.elk.kbp_user_data[0].kbp_mdio_id) {
            kbp_c_reset_bit = 1;
            kbp_s_reset_bit = 0;
        } else {
            /* don't touch second sets of reset pins since we re-purposed them for BCM52311's PCIe reset */
            return KBP_OK;
        }
        kbp_reset_address = 0x1f;
    } else if (SOC_IS_JERICHO(unit)) {
        if (user_data->kbp_mdio_id == SOC_DPP_CONFIG(unit)->arad->init.elk.kbp_user_data[0].kbp_mdio_id) {
            kbp_c_reset_bit = 0; 
            kbp_s_reset_bit = 1;
        } else {
            kbp_c_reset_bit = 2; 
            kbp_s_reset_bit = 3;
        }
        kbp_reset_address = 0x1f;
    } else {
        kbp_c_reset_bit = 7;
        kbp_s_reset_bit = 6;
        kbp_reset_address = 0x2;
    }

    ret = cpu_i2c_read(0x40, kbp_reset_address, CPU_I2C_ALEN_LONG_DLEN_LONG, &val);
    sal_usleep(1000);
    if( ret != CMD_OK){
        return KBP_INTERNAL_ERROR;
    }

    if (!s_reset_low) {
        SHR_BITSET(&val, kbp_s_reset_bit);
    } else {
        SHR_BITCLR(&val, kbp_s_reset_bit);
    }
    if(!c_reset_low){
        SHR_BITSET(&val, kbp_c_reset_bit);
    } else {
        SHR_BITCLR(&val, kbp_c_reset_bit);
    }

    ret = cpu_i2c_write(0x40, kbp_reset_address, CPU_I2C_ALEN_LONG_DLEN_LONG, val);
    sal_usleep(1000);
    if( ret != CMD_OK){
        return KBP_INTERNAL_ERROR;
    }
#endif

    /*in case of out of reset sleep for link lock*/
    if((!c_reset_low) && (!s_reset_low)){
        sal_usleep(150000);
    }
    return KBP_OK;
}

int dpp_kbp_file_open(int unit, char *filename, int device_type)
{
    uint8 is_warmboot;
    char prefixed_file_name[SOC_PROPERTY_NAME_MAX + 256];
    char    *stable_filename = NULL;

    FILE **file_fp = NULL;
    if (NULL == filename) {
        return 0; 
    }

    if (device_type == KBP_DEVICE_12K) {
        file_fp = &kbp_file_fp[unit];
    }
    else if (device_type == KBP_DEVICE_KAPS){
        file_fp = &kaps_file_fp[unit];
    }


    if (file_fp != NULL && *file_fp == NULL) {
        is_warmboot = SOC_WARM_BOOT(unit);

        prefixed_file_name[0] = '\0' ;

        stable_filename = soc_property_get_str(unit, spn_STABLE_FILENAME);

        /* prefixing with unique file name to enable more than one parallel run from same SDK folder  */
        /* assuming stable_filename is unique for each separate run */
        if (NULL != stable_filename) {
            sal_strncat(prefixed_file_name, stable_filename, sizeof(prefixed_file_name) - 1);

            sal_strncat(prefixed_file_name, "_", sizeof(prefixed_file_name) - sal_strlen(prefixed_file_name) - 1);
        }
        sal_strncat(prefixed_file_name, filename, sizeof(prefixed_file_name) - sal_strlen(prefixed_file_name) - 1);


        if ((*file_fp =
             sal_fopen(prefixed_file_name, is_warmboot ? "r+" : "w+")) == 0) {
             cli_out("Error:  sal_fopen() Failed\n");
             return SOC_E_FAIL;
        }
    }

    return SOC_E_NONE;
}

int
dpp_kbp_file_close(int unit)
{
    if (kbp_file_fp[unit]) {
        sal_fclose(kbp_file_fp[unit]);
        kbp_file_fp[unit] = 0;
    }

    return 0;
}

STATIC int
dpp_kbp_file_read_func(void * handle, uint8_t *buffer, uint32_t size, uint32_t offset)
{
    size_t result;

    if (!handle) {
        return SOC_E_FAIL;
    }

    if (0 != fseek(handle, offset, SEEK_SET)) {
        return SOC_E_FAIL;
    }

    result = fread(buffer, 1, size, handle);
    if(result < size) {
        return SOC_E_FAIL;
    }

    return SOC_E_NONE;
}

STATIC int
dpp_kbp_file_write_func(void * handle, uint8_t *buffer, uint32_t size, uint32_t offset)

{
    size_t result;

    if (!handle) {
        return SOC_E_UNIT;
    }

    if (0 != fseek(handle, offset, SEEK_SET)) {
        return SOC_E_FAIL;
    }

    result = fwrite(buffer, 1, size, handle);
    if (result != size) {
        return SOC_E_MEMORY;
    }
    fflush(handle);

    return SOC_E_NONE;
}

uint32 dpp_kbp_rop_dma_enable(int unit)
{
    arad_kbp_rop_dma_enable(unit);
    cli_out("kbp_enable_dma: %d\n", SOC_DPP_CONFIG(unit)->arad->init.elk.kbp_rop_enable_dma);
    return 0;
}

uint32 dpp_kbp_rop_dma_show_stat(int unit)
{
    cli_out("kbp_rop_enable_dma: %d, kbp_rop_dma_total_trans_count: %d, kbp_rop_dma_msg_count: %d,\n",
        SOC_DPP_CONFIG(unit)->arad->init.elk.kbp_rop_enable_dma,
        SOC_DPP_CONFIG(unit)->arad->init.elk.kbp_rop_dma_total_trans_count, 
        SOC_DPP_CONFIG(unit)->arad->init.elk.kbp_rop_dma_msg_count);
    cli_out("kbp_rop_dma_ack_pending: %d, kbp_rop_dma_gathered_trans_count: %d\n",
        SOC_DPP_CONFIG(unit)->arad->init.elk.kbp_rop_dma_ack_pending,
        SOC_DPP_CONFIG(unit)->arad->init.elk.kbp_rop_dma_gathered_trans_count);
    
    return 0;
}
int dpp_kbp_rop_dma_init_appl(int unit)
{
    int rv;

    cli_out("(%d)trying dma_init_appl\n", unit);
    rv = arad_kbp_rop_dma_init(unit);
    if (rv != SOC_E_NONE) {
        cli_out("kbp_rop_dma_init failed, rv: %d\n", rv);
        return rv;
    }
    cli_out("dma_init_appl success\n");
    return 0;
}

int dpp_kbp_dma_configure(int unit, args_t *a)
{
    parse_table_t pt;
    int rv;
    int kbp_trans_len = SOC_DPP_CONFIG(unit)->arad->init.elk.kbp_rop_dma_trans_len;
    int kbp_trans_zero_fill = 0;
    int kbp_trans_change_endian = 0;

    if (ARG_CNT(a) > 0) {
        parse_table_init(0,&pt);
        parse_table_add(&pt,"l",PQ_INT , &kbp_trans_len, &kbp_trans_len, NULL);
        parse_table_add(&pt,"z",PQ_INT , &kbp_trans_zero_fill, &kbp_trans_zero_fill, NULL);
        parse_table_add(&pt,"e",PQ_INT , &kbp_trans_change_endian, &kbp_trans_change_endian, NULL);
        
        if (parse_arg_eq(a, &pt) < 0) {
            cli_out("%s: Invalid option: %s\n", ARG_CMD(a), ARG_CUR(a));
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }
        parse_arg_eq_done(&pt);
    }    

    cli_out("trying dpp_kbp_dma_configure: kbp_trans_len: %d, kbp_trans_zero_fill: %d, kbp_trans_change_endian: %d\n",
        kbp_trans_len, kbp_trans_zero_fill, kbp_trans_change_endian);
    rv = arad_kbp_rop_dma_configure(unit, kbp_trans_len, kbp_trans_zero_fill, kbp_trans_change_endian);
    if (rv != SOC_E_NONE) {
        cli_out("dpp_kbp_dma_configure failed, rv: %d\n", rv);
        return rv;
    }
    cli_out("dpp_kbp_dma_configure success\n");
    return 0;
}

int dpp_kbp_dma_test(int unit, args_t *a)
{
    parse_table_t pt;
    int rv;
    int debug_print = 2;

    if (ARG_CNT(a) > 0) {
        parse_table_init(0,&pt);
        parse_table_add(&pt,"p",PQ_INT , (void *) (0), &debug_print, NULL);
        
        if (parse_arg_eq(a, &pt) < 0) {
            cli_out("%s: Invalid option: %s\n", ARG_CMD(a), ARG_CUR(a));
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }
        parse_arg_eq_done(&pt);
    }    

    cli_out("trying dpp_kbp_dma_test\n");
    rv = arad_kbp_rop_dma_test(unit, debug_print);
    if (rv != SOC_E_NONE) {
        cli_out("dpp_kbp_dma_test failed, rv: %d\n", rv);
        return rv;
    }
    cli_out("dpp_kbp_dma_test success\n");
    return 0;
}

int dpp_kbp_init_appl(int unit, uint32 second_kbp_supported, ARAD_INIT_ELK *elk_ptr) {

    int rv = BCM_E_NONE;
    uint32 ilkn_num_lanes, ilkn_metaframe;
    int ilkn_rate;
    bcm_port_t kbp_port;
    uint32 soc_sand_rv;
    uint32 core = 0;

    /* Give the Interface time to sync/adjust */
    soc_sand_rv = arad_kbp_ilkn_interface_param_get(unit, core, &kbp_port, &ilkn_num_lanes, &ilkn_rate, &ilkn_metaframe);
    rv = handle_sand_result(soc_sand_rv);
    if (BCM_FAILURE(rv)) {
       cli_out("Error:  arad_kbp_ilkn_interface_param_get() Failed\n");
       return rv;
    }

#if defined(INCLUDE_KBP) && !defined(BCM_88030) && (defined(ADAPTER_SERVER_MODE) || defined(INCLUDE_INTR))
    if(!SAL_BOOT_PLISIM)
    {
        rx_los_state_t port_stable_state;
        rv = rx_los_port_stable(unit, kbp_port, KBP_PORT_STABLE_TIMEOUT, &port_stable_state);
        if (BCM_FAILURE(rv)) {
            cli_out("Error:  rx_los_port_stable() Failed\n");
            return rv;
        }
        if ((port_stable_state != rx_los_state_ideal_state) && (port_stable_state != rx_los_states_count)) {
            /* No interface signal */
            cli_out("Error:  No signal from KBP ilkn. port_stable_state=%d\n", port_stable_state);
            return BCM_E_FAIL;
        }
    }
#endif
    /*Check if core 1 has kbp port */
    if (SOC_DPP_CORE_NOF_ACTIVE_CORES(unit) > 1) {
        core = 1;
        soc_sand_rv = arad_kbp_ilkn_interface_param_get(unit, core, &kbp_port, &ilkn_num_lanes, &ilkn_rate, &ilkn_metaframe);
        if (soc_sand_rv != 0) {
            second_kbp_supported = 0;
        }
    }

    soc_sand_rv = dpp_kbp_file_open(unit, "kbp", KBP_DEVICE_12K);
    rv = handle_sand_result(soc_sand_rv);
    if (BCM_FAILURE(rv)) {
        cli_out("Error: dpp_kbp_file_open(%d)\n", unit);
        return rv;
    }

    arad_kbp_warmboot_register(unit, kbp_file_fp[unit], &dpp_kbp_file_read_func, &dpp_kbp_file_write_func);

    soc_sand_rv = arad_kbp_init_app(unit, second_kbp_supported, elk_ptr);
    rv = handle_sand_result(soc_sand_rv);
    if (BCM_FAILURE(rv)) {
        cli_out("Error: arad_kbp_init_appl(%d, &elk)\n", unit);
        return rv;
    }
    

    /* SOC_DPP_CONFIG(unit)->arad->init.elk.kbp_rop_dma_buffer[0] = NULL;
    dpp_kbp_rop_dma_init_appl(unit); */

    return rv;
}

int dpp_kaps_init(int unit) {

    cmd_result_t rv = CMD_OK;
    uint32 soc_sand_rv;

    soc_sand_rv = dpp_kbp_file_open(unit, "kaps", KBP_DEVICE_KAPS);
    rv = handle_sand_result(soc_sand_rv);
    if (BCM_FAILURE(rv)) {
        cli_out("Error: dpp_kbp_file_open(%d)\n", unit);
        return rv;
    }

#ifdef BCM_88675_A0
    jer_kaps_warmboot_register(unit, kaps_file_fp[unit], &dpp_kbp_file_read_func, &dpp_kbp_file_write_func);

    soc_sand_rv = jer_kaps_init_app(unit);
    rv = handle_sand_result(soc_sand_rv);
    if (BCM_FAILURE(rv)) {
        cli_out("Error: jer_kaps_init_app(%d)\n", unit);
        return rv;
    }

#ifndef PLISIM
    if ((soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "kaps_arm_enable", 0) || (soc_property_suffix_num_get(unit, -1, spn_DMA_DESC_AGGREGATOR_ENABLE_SPECIFIC, "KAPS", 0)))
         && !SOC_IS_JERICHO_PLUS(unit)) {
        if (jer_sbusdma_desc_is_enabled(unit) == 0) {
            /* Enable descriptor DMA with default values */
            soc_sand_rv = jer_sbusdma_desc_init(unit, 500, 25600, 1000);
            rv = handle_sand_result(soc_sand_rv);
            if (BCM_FAILURE(rv)) {
                cli_out("Error: jer_sbusdma_desc_init(%d)\n", unit);
                return rv;
            }
        }
        soc_sand_rv = jer_pp_xpt_kaps_arm_image_load_default(unit);
        rv = handle_sand_result(soc_sand_rv);
        if (BCM_FAILURE(rv)) {
            cli_out("Error: jer_pp_xpt_kaps_arm_image_load_default(%d)\n", unit);
            return rv;
        }
    }
#endif /*PLISIM*/
#endif /* BCM_88675_A0 */

    return rv;
}

STATIC cmd_result_t
_cmd_dpp_kbp_init_appl(int unit, uint32 second_elk_enable, args_t *a)
{
    parse_table_t pt; 
    ARAD_INIT_ELK elk, *elk_ptr=NULL;
    uint32 enable;

    sal_memset(&elk, 0x0, sizeof(elk));
    parse_table_init(0,&pt);

    if (ARG_CNT(a) > (0 + second_elk_enable)) {

        parse_table_add(&pt,"ENable",PQ_INT , (void *) (0), &enable, NULL);
        parse_table_add(&pt,"TcamDevType",PQ_INT , (void *) (0), &elk.tcam_dev_type, NULL);
        parse_table_add(&pt,"Ip4UcRpfFwdTableSize",PQ_INT , (void *) (0), &elk.fwd_table_size[ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_RPF_0], NULL);
        parse_table_add(&pt,"Ip4McFwdTableSize",PQ_INT , (void *) (0), &elk.fwd_table_size[ARAD_KBP_FRWRD_TBL_ID_IPV4_MC], NULL);
        parse_table_add(&pt,"Ip6UcRpfFwdTableSize",PQ_INT , (void *) (0), &elk.fwd_table_size[ARAD_KBP_FRWRD_TBL_ID_IPV6_UC_RPF_0], NULL);
        parse_table_add(&pt,"Ip6McFwdTableSize",PQ_INT , (void *) (0), &elk.fwd_table_size[ARAD_KBP_FRWRD_TBL_ID_IPV6_MC], NULL);
        parse_table_add(&pt,"TrillUcFwdTableSize",PQ_INT , (void *) (0), &elk.fwd_table_size[ARAD_KBP_FRWRD_TBL_ID_TRILL_UC], NULL);
        parse_table_add(&pt,"TrillMcFwdTableSize",PQ_INT , (void *) (0), &elk.fwd_table_size[ARAD_KBP_FRWRD_TBL_ID_TRILL_MC], NULL);
        parse_table_add(&pt,"MplsFwdTableSize",PQ_INT , (void *) (0), &elk.fwd_table_size[ARAD_KBP_FRWRD_TBL_ID_LSR], NULL);
  
        if (parse_arg_eq(a, &pt) < 0) {
            cli_out("%s: Invalid option: %s\n", ARG_CMD(a), ARG_CUR(a));
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }

        elk.enable = (uint8)enable;

        elk_ptr = &elk;

    } else {
        elk_ptr = NULL;
    }

    if (dpp_kbp_init_appl(unit, second_elk_enable, elk_ptr) < 0) {
        parse_arg_eq_done(&pt);
        return CMD_FAIL;
    }
    parse_arg_eq_done(&pt);
    return CMD_OK;
}

STATIC cmd_result_t
cmd_dpp_kbp_init_appl(int unit, args_t *a)
{
    parse_table_t pt;
    uint32 second_elk_enable=0;

    int num_of_cores = SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores;

    if(num_of_cores == 2){
       second_elk_enable = 1;
    }

    if (ARG_CNT(a) > 0) {
        parse_table_init(0,&pt);
        parse_table_add(&pt,"SecondElkEnable",PQ_INT , (void *) (&second_elk_enable), &second_elk_enable, NULL);
        
        if (parse_arg_eq(a, &pt) < 0) {
            cli_out("%s: Invalid option: %s\n", ARG_CMD(a), ARG_CUR(a));
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }
        parse_arg_eq_done(&pt);
    }

    if (_cmd_dpp_kbp_init_appl(unit, second_elk_enable, a) < 0) {
        return CMD_FAIL;
    }
    return CMD_OK;
}

STATIC cmd_result_t
cmd_dpp_kbp_kaps_init(int unit, args_t *a)
{
    parse_table_t pt; 

    if (ARG_CNT(a) > 0) {
        parse_table_init(0,&pt);

        if (parse_arg_eq(a, &pt) < 0) {
            cli_out("%s: Invalid option: %s\n",
                    ARG_CMD(a), ARG_CUR(a));
            return CMD_USAGE;
        }
    }

    if (dpp_kaps_init(unit) < 0) {
        return CMD_FAIL;
    }
    return CMD_OK;
}

STATIC cmd_result_t
cmd_dpp_kbp_search(int unit, args_t *a)
{
    parse_table_t pt;

    int32 i;
    uint32 ltr_idx = 0;
    uint16 vrf = 0, inrif = 0;
    uint8 vrf_8;
    uint32 dip_v4 = 0, sip_v4 = 0;
    ip6_addr_t dip_v6 = {0}, sip_v6 = {0};

    uint8 master_key[80] = {0};
    uint8 use_master_key = 0;

    cmd_result_t rv = CMD_OK;

    if (ARG_CNT(a) > 0) {
        parse_table_init(0,&pt);

        parse_table_add(&pt,"ltr_idx", PQ_INT, (void *) (0), &ltr_idx,  NULL);

        parse_table_add(&pt,"sip", PQ_IP, (void *) (0), &sip_v4, NULL);
        parse_table_add(&pt,"sip6", PQ_IP6, (void *) (0), &sip_v6, NULL);
        parse_table_add(&pt,"dip", PQ_IP, (void *) (0), &dip_v4, NULL);
        parse_table_add(&pt,"dip6", PQ_IP6, (void *) (0), &dip_v6, NULL);

        parse_table_add(&pt,"inrif", PQ_INT16, (void *) (0), &inrif, NULL);
        parse_table_add(&pt,"vrf", PQ_INT16, (void *) (0), &vrf, NULL);

        /* The master_ option is not exposed to the user, it for internal debug only. */
        parse_table_add(&pt,"master_0", PQ_INT64, (void *) (0), &master_key[0], NULL);
        parse_table_add(&pt,"master_8", PQ_INT64, (void *) (0), &master_key[8], NULL);
        parse_table_add(&pt,"master_16", PQ_INT64, (void *) (0), &master_key[16], NULL);
        parse_table_add(&pt,"master_24", PQ_INT64, (void *) (0), &master_key[24], NULL);
        parse_table_add(&pt,"master_32", PQ_INT64, (void *) (0), &master_key[32], NULL);
        parse_table_add(&pt,"master_40", PQ_INT64, (void *) (0), &master_key[40], NULL);
        parse_table_add(&pt,"master_48", PQ_INT64, (void *) (0), &master_key[48], NULL);
        parse_table_add(&pt,"master_56", PQ_INT64, (void *) (0), &master_key[56], NULL);
        parse_table_add(&pt,"master_64", PQ_INT64, (void *) (0), &master_key[64], NULL);
        parse_table_add(&pt,"master_72", PQ_INT64, (void *) (0), &master_key[72], NULL);

        if (parse_arg_eq(a, &pt) < 0) {
            cli_out("%s: Invalid option: %s\n",ARG_CMD(a), ARG_CUR(a));
            cli_out(
                "\nkbp_search - perform a search in kbp DB according to the following parameters:"
                "\nUsage: kbp kbp_search [list of parameters]:"
                "\n\t\t[ltr_idx]    - The LTR index that its searches should be performed"
                "\n\t\t[vrf]        - VRF"
                "\n\t\t[inrif]        - In-RIF"
                "\n\t\t[sip]        - IPv4 SIP address as 32bit number (hex or dec)"
                "\n\t\t[dip]        - IPv4 DIP address as 32bit number (hex or dec)"
                "\n\t\t[sip6]       - full IPv6 SIP address, i.e: 1111:2222:3333:4444:aaaa:bbbb:cccc:dddd"
                "\n\t\t[dip6]       - full IPv6 DIP address, i.e: 1111:2222:3333:4444:aaaa:bbbb:cccc:dddd"
                "\n\n"
                );
            parse_arg_eq_done(&pt);
            return CMD_OK;
        }
        parse_arg_eq_done(&pt);
    }

    if (ltr_idx >= ARAD_KBP_MAX_NUM_OF_FRWRD_DBS) {
        cli_out("table_id should be smaller than %d\n",ARAD_KBP_MAX_NUM_OF_FRWRD_DBS);
        return CMD_FAIL;
    }

    for (i = 0;i < 80; i++) {
        if (master_key[i] != 0) {
            use_master_key = 1;
            break;
        }
    }

    if (!use_master_key) {

        /* Convert big-endian 32-bit value (16-bit value) to host-endian. */
        sip_v4 = soc_ntohl(sip_v4);
        dip_v4 = soc_ntohl(dip_v4);
        vrf = soc_ntohs(vrf);
        inrif = soc_ntohs(inrif);

        switch (ltr_idx) {
        case ARAD_KBP_FRWRD_DB_TYPE_IPV4_UC:
        case ARAD_KBP_FRWRD_DB_TYPE_IPV4_UC_RPF:
            /* VRF, SIP, DIP*/
            sal_memcpy((void*)&master_key[0],&vrf,2);
            sal_memcpy((void*)&master_key[2],&sip_v4,4);
            sal_memcpy((void*)&master_key[6],&dip_v4,4);
            break;
        case ARAD_KBP_FRWRD_DB_TYPE_IPV4_DC:
            /* VRF_8, SIP, DIP*/
            if (soc_htons(vrf) > 255) {
                cli_out("VRF=%d, IPV4 DC supports vrf<=255 only!\n",soc_htons(vrf));
                return CMD_FAIL;
            }
            vrf_8 = (soc_htons(vrf) & 0x00ff);
            sal_memcpy((void*)&master_key[0],&vrf_8,1);
            sal_memcpy((void*)&master_key[1],&sip_v4,4);
            sal_memcpy((void*)&master_key[5],&dip_v4,4);
            break;
        case ARAD_KBP_FRWRD_DB_TYPE_IPV4_MC_RPF:
            if (SOC_IS_JERICHO(unit)) {
                /* VRF, DIP, SIP, IN-RIF*/
                sal_memcpy((void*)&master_key[0],&vrf,2);
                sal_memcpy((void*)&master_key[2],&dip_v4,4);
                sal_memcpy((void*)&master_key[6],&sip_v4,4);
                sal_memcpy((void*)&master_key[10],&inrif,2);
            } else {
                /* VRF, IN-RIF, SIP, DIP*/
                sal_memcpy((void*)&master_key[0],&vrf,2);
                sal_memcpy((void*)&master_key[2],&inrif,2);
                sal_memcpy((void*)&master_key[4],&sip_v4,4);
                sal_memcpy((void*)&master_key[8],&dip_v4,4);
            }
            break;
        case ARAD_KBP_FRWRD_DB_TYPE_IPV6_UC:
        case ARAD_KBP_FRWRD_DB_TYPE_IPV6_UC_RPF:
            /* VRF, SIP_6, DIP_6*/
            sal_memcpy((void*)&master_key[0],&vrf,2);
            sal_memcpy((void*)&master_key[2],sip_v6,16);
            sal_memcpy((void*)&master_key[18],dip_v6,16);
            break;
        case ARAD_KBP_FRWRD_DB_TYPE_IPV6_MC_RPF:
            if (SOC_IS_JERICHO(unit)) {
                /* VRF, SIP_6, DIP_6, IN-RIF*/
                sal_memcpy((void*)&master_key[0],&vrf,2);
                sal_memcpy((void*)&master_key[2],dip_v6,16);
                sal_memcpy((void*)&master_key[18],sip_v6,16);
                sal_memcpy((void*)&master_key[34],&inrif,2);
            } else {
                /* VRF, IN-RIF, SIP_6, DIP_6*/
                sal_memcpy((void*)&master_key[0],&vrf,2);
                sal_memcpy((void*)&master_key[2],&inrif,2);
                sal_memcpy((void*)&master_key[4],sip_v6,16);
                sal_memcpy((void*)&master_key[20],dip_v6,16);
            }
            break;
        default:
            cli_out("Unsupported LTR index: %d\n",ltr_idx);
            return CMD_FAIL;
        }
    }

    rv = arad_pp_kbp_do_search(unit, ltr_idx, master_key);
    if (BCM_FAILURE(rv)) {
        cli_out("Error: arad_pp_kbp_do_search(%d)\n", unit);
        return rv;
    }
    return rv;
}

#if defined(BCM_JERICHO_SUPPORT)
STATIC cmd_result_t
cmd_dpp_kaps_search(int unit, args_t *a)
{
    parse_table_t pt;
    uint32 ip_search_ids[JER_KAPS_NOF_SEARCHES] = {0};

    uint32 sum_of_searches = 0;
    uint32 search_id = JER_KAPS_NOF_SEARCHES;

    ip6_addr_t dip_v6 = {0}, sip_v6 = {0}, mc_group_v6 = {0}; /*_SHR_L3_IP6_ADDRLEN (16) Bytes*/
    SOC_SAND_PP_IPV6_ADDRESS dip_v6_32,
           sip_v6_32,
           mc_group_v6_32;
    uint32 dip_v4 = 0, sip_v4 = 0, mc_group_v4 = 0, vrf = 0, inrif = 0;
    cmd_result_t rv = CMD_OK;

    int32 i;
    uint8 key[JER_KAPS_MAX_NUM_OF_PARALLEL_SEARCHES][JER_KAPS_KEY_BUFFER_NOF_BYTES];

    JER_KAPS_SEARCH_CONFIG search_cfg;

    uint32  prefix_len;
    SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    uint32 mask[SOC_SAND_PP_IPV6_ADDRESS_NOF_UINT32S] = {SOC_SAND_U32_MAX, SOC_SAND_U32_MAX, SOC_SAND_U32_MAX, SOC_SAND_U32_MAX};
    SOC_DPP_DBAL_SW_TABLE_IDS table_id;
    SOC_DPP_DBAL_TABLE_INFO table;

    uint32 j = 0;
    uint32 raw_mode = 0;
    uint8 raw_data[JER_KAPS_KEY_BUFFER_NOF_BYTES] = {0};
    char* data_str = NULL;

    memset(key, 0, sizeof(key[0][0]) * JER_KAPS_MAX_NUM_OF_PARALLEL_SEARCHES * JER_KAPS_KEY_BUFFER_NOF_BYTES);
    for (i =0; i < JER_KAPS_NOF_SEARCHES; i++) {
        ip_search_ids[i] = 0;
    }

    if (ARG_CNT(a) > 0) {
        parse_table_init(0,&pt);
        parse_table_add(&pt,"IPV4_UC", PQ_INT, (void *) (0), &ip_search_ids[JER_KAPS_IPV4_UC_SEARCH_ID], NULL);
        parse_table_add(&pt,"IPV6_UC", PQ_INT, (void *) (0), &ip_search_ids[JER_KAPS_IPV6_UC_SEARCH_ID], NULL);
        parse_table_add(&pt,"IPV4_MC", PQ_INT, (void *) (0), &ip_search_ids[JER_KAPS_IPV4_MC_SEARCH_ID], NULL);
        parse_table_add(&pt,"IPV6_MC", PQ_INT, (void *) (0), &ip_search_ids[JER_KAPS_IPV6_MC_SEARCH_ID], NULL);
        /*search_id allows to directly pick the number of the desired search*/
        parse_table_add(&pt,"SEARCH_ID", PQ_INT, (void *) (0), &search_id, NULL);
        /*RAW_SEARCH allows to directly pick the search key from RAW_DATA */
        parse_table_add(&pt,"RAW", PQ_INT, (void *) (0), &raw_mode, NULL);

        parse_table_add(&pt,"SIP", PQ_IP, (void *) (0), &sip_v4, NULL);
        parse_table_add(&pt,"SIP6", PQ_IP6, (void *) (0), &sip_v6, NULL);
        parse_table_add(&pt,"DIP", PQ_IP, (void *) (0), &dip_v4, NULL);
        parse_table_add(&pt,"DIP6", PQ_IP6, (void *) (0), &dip_v6, NULL);
        parse_table_add(&pt,"MC_GROUP", PQ_IP, (void *) (0), &mc_group_v4, NULL);
        parse_table_add(&pt,"MC_GROUP6", PQ_IP6, (void *) (0), &mc_group_v6, NULL);
        parse_table_add(&pt,"INRIF", PQ_INT, (void *) (0), &inrif, NULL);
        parse_table_add(&pt,"VRF", PQ_INT, (void *) (0), &vrf, NULL);
        parse_table_add(&pt,"DAta", PQ_STRING, (void *) ("0000000000000000000000000000000000000000"), &data_str, NULL);

        if (parse_arg_eq(a, &pt) < 0) {
            cli_out("%s: Invalid option: %s\n", ARG_CMD(a), ARG_CUR(a));
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }

        /* set DAta */
        if(cmd_dpp_kbp_parse_string_of_data(unit, data_str, raw_data, JER_KAPS_KEY_BUFFER_NOF_BYTES) != 0) {
            cli_out("DAta didn't type correctly, please type 40 digits, with no spaces.\n"
                    "Example: DAta=0xd00000000000000000000000123400000000ff12\n");
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }

        parse_arg_eq_done(&pt);

        for (i =0; i < JER_KAPS_NOF_SEARCHES; i++) {
            if (ip_search_ids[i] != 0) {
                search_id = i;
            }
            sum_of_searches += ip_search_ids[i];
        }

        /*Can only choose one JER_KAPS_IP_SEARCH_ID*/
        if (sum_of_searches > 1 || search_id >= JER_KAPS_NOF_SEARCHES) {
            cli_out("%s: Invalid option: %s\n",
                    ARG_CMD(a), ARG_CUR(a));
            return CMD_USAGE;
        }

        jer_kaps_search_config_get(unit, search_id, &search_cfg);

        if (!search_cfg.valid_tables_num) {
            cli_out("%s: Invalid option: %s\n",
                    ARG_CMD(a), ARG_CUR(a));
            return CMD_USAGE;
        }

        if (raw_mode != 0) {
            /* copy the key */
            for (i = 0; i < JER_KAPS_MAX_NUM_OF_PARALLEL_SEARCHES ; i++){
                for (j = 0; j < JER_KAPS_KEY_BUFFER_NOF_BYTES; j++) {
                    key[i][j] = raw_data[j];
                }
            }
        } else {
            /* Convert from uint8 to uint32*/
            rv = _bcm_l3_bcm_ipv6_addr_to_sand_ipv6_addr(unit, sip_v6, &sip_v6_32);
            if (BCM_FAILURE(rv)) {
                cli_out("Error: jer_kaps_search_generic(%d), uint8 to uint32 failure\n", unit);
                return rv;
            }
            rv = _bcm_l3_bcm_ipv6_addr_to_sand_ipv6_addr(unit, dip_v6, &dip_v6_32);
            if (BCM_FAILURE(rv)) {
                cli_out("Error: jer_kaps_search_generic(%d), uint8 to uint32 failure\n", unit);
                return rv;
            }
            rv = _bcm_l3_bcm_ipv6_addr_to_sand_ipv6_addr(unit, mc_group_v6, &mc_group_v6_32);
            if (BCM_FAILURE(rv)) {
                cli_out("Error: jer_kaps_search_generic(%d), uint8 to uint32 failure\n", unit);
                return rv;
            }

            /* Initialize qual_vals to include all the possible quals*/
            DBAL_QUAL_VALS_CLEAR(qual_vals);
            DBAL_QUAL_VAL_ENCODE_FWD_IPV4_SIP(&qual_vals[0], sip_v4, SOC_SAND_NOF_BITS_IN_UINT32);
            DBAL_QUAL_VAL_ENCODE_IPV6_SIP_LOW(&qual_vals[1], sip_v6_32.address, mask);
            DBAL_QUAL_VAL_ENCODE_IPV6_SIP_HIGH(&qual_vals[2], sip_v6_32.address, mask);
            if (mc_group_v4) { /* The search uses the mc_group or the dip as the DIP qual*/
                DBAL_QUAL_VAL_ENCODE_FWD_IPV4_DIP(&qual_vals[3], mc_group_v4, SOC_SAND_NOF_BITS_IN_UINT32);
            } else {
                DBAL_QUAL_VAL_ENCODE_FWD_IPV4_DIP(&qual_vals[3], dip_v4, SOC_SAND_NOF_BITS_IN_UINT32);
            }
            if (mc_group_v6[0]) {
                DBAL_QUAL_VAL_ENCODE_IPV6_DIP_LOW(&qual_vals[4], mc_group_v6_32.address, mask);
                DBAL_QUAL_VAL_ENCODE_IPV6_DIP_HIGH(&qual_vals[5], mc_group_v6_32.address, mask);
            } else {
                DBAL_QUAL_VAL_ENCODE_IPV6_DIP_LOW(&qual_vals[4], dip_v6_32.address, mask);
                DBAL_QUAL_VAL_ENCODE_IPV6_DIP_HIGH(&qual_vals[5], dip_v6_32.address, mask);
            }
            DBAL_QUAL_VAL_ENCODE_IN_RIF(&qual_vals[6], inrif, SOC_SAND_U32_MAX);

            for (i=0; i < JER_KAPS_MAX_NUM_OF_PARALLEL_SEARCHES; i++) {

                /*vrf!=0 only for private*/
                if (search_cfg.tbl_id[i] < JER_KAPS_IP_PUBLIC_INDEX) {
                    DBAL_QUAL_VAL_ENCODE_VRF(&qual_vals[7], vrf, SOC_SAND_U32_MAX);
                } else{
                    DBAL_QUAL_VAL_ENCODE_VRF(&qual_vals[7], 0, SOC_SAND_U32_MAX);
                }

                rv = jer_pp_kaps_table_id_to_dbal_translate(unit, search_cfg.tbl_id[i], &table_id);
                if (BCM_FAILURE(rv)) {
                    cli_out("Error: jer_kaps_search_generic(%d), table translation failed\n", unit);
                    return rv;
                }

                rv = sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table);
                if (BCM_FAILURE(rv)) {
                    cli_out("Error: jer_kaps_search_generic(%d), key buffer construction failed\n", unit);
                    return rv;
                }

                rv = arad_pp_dbal_entry_key_to_kbp_buffer(unit, &table, JER_KAPS_KEY_BUFFER_NOF_BYTES, qual_vals, &prefix_len, key[i]);
                if (BCM_FAILURE(rv)) {
                    cli_out("Error: jer_kaps_search_generic(%d), key buffer construction failed\n", unit);
                    return rv;
                }
            }
        }
    } else {
        cli_out("Wrong cmd without arg\n");
        return CMD_USAGE;
    }

    rv = jer_kaps_search_generic(unit, search_id, key, NULL, NULL, NULL, NULL); /* No need for return values*/
    if (BCM_FAILURE(rv)) {
        cli_out("Error: jer_kaps_search_generic(%d)\n", unit);
        return rv;
    }

    return rv;
}

STATIC cmd_result_t
cmd_dpp_kaps_show(int unit, args_t *a)
{
    SOC_DPP_DBAL_SW_TABLE_IDS tbl_idx = SOC_DPP_DBAL_SW_TABLE_ID_INVALID;
    uint32 soc_sand_rv;
    parse_table_t pt;
    uint32 count_only = 0;

    SOC_DPP_DBAL_TABLE_INFO dbal_table;

    if (ARG_CNT(a) > 0) {
        parse_table_init(0,&pt);
        parse_table_add(&pt,"DBAL_TBL_ID", PQ_INT, (void *) (SOC_DPP_DBAL_SW_TABLE_ID_INVALID), &tbl_idx, NULL);
        parse_table_add(&pt,"CountOnly", PQ_INT, (void *) (0), &count_only, NULL);

        if (parse_arg_eq(a, &pt) < 0) {
            cli_out("%s: Invalid option: %s\n", ARG_CMD(a), ARG_CUR(a));
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }
        parse_arg_eq_done(&pt);
    }


    if (tbl_idx != SOC_DPP_DBAL_SW_TABLE_ID_INVALID) {
        soc_sand_rv = jer_kaps_show_table(unit, tbl_idx, count_only ? 0 : 1);

        if(BCM_FAILURE(handle_sand_result(soc_sand_rv))) {
            cli_out("Error: dpp_kaps_show(%d)\n", unit);
            return CMD_FAIL;
        }
    } else {
        /*print all if not given a specific table*/
        for (tbl_idx = 0; tbl_idx < SOC_DPP_DBAL_SW_NOF_TABLES; tbl_idx++) {

            soc_sand_rv = sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, tbl_idx, &dbal_table);
            if(BCM_FAILURE(handle_sand_result(soc_sand_rv))) {
                cli_out("Error: dpp_kaps_show(%d) - retrieve dbal table\n", unit);
                return CMD_FAIL;
            }

            /* Only print initiated tables in print all */
            if ((dbal_table.physical_db_type == SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KAPS) && (dbal_table.is_table_initiated)) {
                soc_sand_rv = jer_kaps_show_table(unit, tbl_idx, count_only ? 0 : 1);

                if(BCM_FAILURE(handle_sand_result(soc_sand_rv))) {
                    cli_out("Error: dpp_kaps_show(%d)\n", unit);
                    return CMD_FAIL;
                }
            }
        }
    }
    return CMD_OK;
}

STATIC cmd_result_t
cmd_dpp_kaps_db_stats(int unit, args_t *a){
    SOC_DPP_DBAL_SW_TABLE_IDS tbl_idx = SOC_DPP_DBAL_SW_TABLE_ID_INVALID;
    uint32 soc_sand_rv;
    parse_table_t pt;

    SOC_DPP_DBAL_TABLE_INFO dbal_table;

    if (ARG_CNT(a) > 0) {
        parse_table_init(0,&pt);
        parse_table_add(&pt,"DBAL_TBL_ID", PQ_INT, (void *) (SOC_DPP_DBAL_SW_TABLE_ID_INVALID), &tbl_idx, NULL);

        if (parse_arg_eq(a, &pt) < 0) {
            cli_out("%s: Invalid option: %s\n", ARG_CMD(a), ARG_CUR(a));
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }
        parse_arg_eq_done(&pt);
    }

    cli_out( "\nTable Configuration\n\n" );
    cli_out( " %-13s", "Table-ID" );
    cli_out( " %-23s", "Table-Name" );
    cli_out( " %-10s", "Size" );
    cli_out( " %-13s", "Table Width" );
    cli_out( " %-10s", "AD Width" );
    cli_out( " %-13s", "Entry Count" );
    cli_out( " %-10s", "~Capacity\n" );

    if (tbl_idx != SOC_DPP_DBAL_SW_TABLE_ID_INVALID) {
        soc_sand_rv = jer_kaps_show_db_stats(unit, tbl_idx);

        if(BCM_FAILURE(handle_sand_result(soc_sand_rv))) {
            cli_out("Error: dpp_kaps_db_stats(%d)\n", unit);
            return CMD_FAIL;
        }
    } else {
        /*print all if not given a specific table*/
        for (tbl_idx = 0; tbl_idx < SOC_DPP_DBAL_SW_NOF_TABLES; tbl_idx++) {

            soc_sand_rv = sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, tbl_idx, &dbal_table);
            if(BCM_FAILURE(handle_sand_result(soc_sand_rv))) {
                cli_out("Error: dpp_kaps_db_stats(%d) - retrieve dbal table\n", unit);
                return CMD_FAIL;
            }

            if (dbal_table.physical_db_type == SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KAPS) {
                soc_sand_rv = jer_kaps_show_db_stats(unit, tbl_idx);

                if(BCM_FAILURE(handle_sand_result(soc_sand_rv))) {
                    cli_out("Error: dpp_kaps_db_stats(%d)\n", unit);
                    return CMD_FAIL;
                }
            }
        }
    }
    return CMD_OK;
}

STATIC cmd_result_t
cmd_dpp_kaps_arm_thread(int unit, args_t *a)
{
    uint32 soc_sand_rv;
    parse_table_t pt;
    uint32 enable_dma_thread = 1;
    uint32 print_status = 1;
    uint32 wait_arm = 0;
    uint32 enable_arm = 2;

    if (SOC_IS_JERICHO_PLUS(unit)) {
        cli_out("Error(%d): KAPS ARM is only available in Jericho\n", unit);
        return CMD_FAIL;
    }

    if (ARG_CNT(a) > 0) {
        parse_table_init(0,&pt);

        parse_table_add(&pt,"enable_dma_thread", PQ_INT, (void *) (1), &enable_dma_thread, NULL);
        parse_table_add(&pt,"print_status", PQ_INT, (void *) (1), &print_status, NULL);
        parse_table_add(&pt,"wait_arm", PQ_INT, (void *) (0), &wait_arm, NULL);
        parse_table_add(&pt,"enable_arm", PQ_INT, (void *) (2), &enable_arm, NULL);

        if (parse_arg_eq(a, &pt) < 0) {
            cli_out("%s: Invalid option: %s\n", ARG_CMD(a), ARG_CUR(a));
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }
        parse_arg_eq_done(&pt);
    }

    if (enable_arm == 1) {
        soc_sand_rv = jer_pp_xpt_kaps_arm_image_load_default(unit);
        if(BCM_FAILURE(handle_sand_result(soc_sand_rv))) {
            cli_out("Error: jer_pp_xpt_kaps_arm_image_load_default(%d)\n", unit);
            return CMD_FAIL;
        }
    }

    soc_sand_rv = jer_pp_xpt_dma_state(unit, print_status, enable_dma_thread, wait_arm);
    if(BCM_FAILURE(handle_sand_result(soc_sand_rv))) {
        cli_out("Error: jer_pp_xpt_dma_state(%d)\n", unit);
        return CMD_FAIL;
    }

    if (enable_arm == 0) {
        soc_sand_rv = jer_pp_xpt_arm_deinit(unit);
        if(BCM_FAILURE(handle_sand_result(soc_sand_rv))) {
            cli_out("Error: jer_pp_xpt_arm_deinit(%d)\n", unit);
            return CMD_FAIL;
        }
    }

    return CMD_OK;
}

STATIC cmd_result_t
dpp_kaps_arm_image_load(int unit, char *file, int cpu_halt, int load_file, int init_tcm)
{
    int rv = CMD_OK, 
        i = 0, 
        entry_num = 0,
        buf_size = KBP_CMD_DPP_KAPS_ARM_IMAGE_LOAD_BUF_SIZE;

    FILE * volatile fp = NULL;

    uint32 input_32[KBP_CMD_DPP_KAPS_ARM_IMAGE_LOAD_BUF_SIZE + 2];
    uint32 tmp_input_32 = 0x0;

    if (load_file != 0x0) {
        /*Initialize the TCM memories*/
        if (init_tcm) {
            rv = jer_pp_xpt_arm_init(unit);
            if (rv != SOC_SAND_OK) {
                LOG_ERROR(BSL_LS_APPL_SHELL, (BSL_META_U(unit, "Error: Failed to jer_pp_xpt_arm_init()\n")));
                return CMD_FAIL;
            }
        }

        if (file == NULL) {
            LOG_ERROR(BSL_LS_APPL_SHELL, (BSL_META_U(unit, "Error: No file specified\n")));
            return CMD_FAIL;
        }

        /* Open File */
        fp = sal_fopen(file, "rt");
        if (fp == NULL) {
            LOG_ERROR(BSL_LS_APPL_SHELL, (BSL_META_U(unit, "Error: Unable to open file: %s\n"), file));
            return CMD_FAIL;
        }

        /* Clear input buffer */
        sal_memset(input_32, 0x0, buf_size * sizeof(uint32));

        /* Loop on reading the ARM Application. reading line by line. */
        while (sal_fgets((char *)input_32, (buf_size * sizeof(uint32)) + 1, fp)) {

            LOG_VERBOSE(BSL_LS_APPL_SHELL, (BSL_META_U(unit, "entry_num=%d\n"), entry_num));
            LOG_VERBOSE(BSL_LS_APPL_SHELL, (BSL_META_U(unit, "input_32:")));
            for (i = 0; i < ((buf_size * sizeof(uint32)) + 2) / sizeof(uint32); i++) {
                LOG_VERBOSE(BSL_LS_APPL_SHELL, (BSL_META_U(unit, "0x%08x "), input_32[i]));
            }
            LOG_VERBOSE(BSL_LS_APPL_SHELL, (BSL_META_U(unit, "\n")));

            /* Convert the Binary (ASCII) file into HEX, and trim the ECC byte */
            tmp_input_32  = xdigit2i(BYTE_2(input_32[2]));
            tmp_input_32 |= xdigit2i(BYTE_3(input_32[2])) << 4;
            tmp_input_32 |= xdigit2i(BYTE_0(input_32[1])) << 8;
            tmp_input_32 |= xdigit2i(BYTE_1(input_32[1])) << 12;
            tmp_input_32 |= xdigit2i(BYTE_2(input_32[1])) << 16;
            tmp_input_32 |= xdigit2i(BYTE_3(input_32[1])) << 20;
            tmp_input_32 |= xdigit2i(BYTE_0(input_32[0])) << 24;
            tmp_input_32 |= xdigit2i(BYTE_1(input_32[0])) << 28;
            input_32[1] = tmp_input_32;
            tmp_input_32  = xdigit2i(BYTE_2(input_32[4]));
            tmp_input_32 |= xdigit2i(BYTE_3(input_32[4])) << 4;
            tmp_input_32 |= xdigit2i(BYTE_0(input_32[3])) << 8;
            tmp_input_32 |= xdigit2i(BYTE_1(input_32[3])) << 12;
            tmp_input_32 |= xdigit2i(BYTE_2(input_32[3])) << 16;
            tmp_input_32 |= xdigit2i(BYTE_3(input_32[3])) << 20;
            tmp_input_32 |= xdigit2i(BYTE_0(input_32[2])) << 24;
            tmp_input_32 |= xdigit2i(BYTE_1(input_32[2])) << 28;
            input_32[0] = tmp_input_32;

            LOG_VERBOSE(BSL_LS_APPL_SHELL, (BSL_META_U(unit, "entry_num=%d\n"), entry_num));
            LOG_VERBOSE(BSL_LS_APPL_SHELL, (BSL_META_U(unit, "input_32:")));
            for (i = 0; i < ((buf_size * sizeof(uint32)) + 2) / sizeof(uint32); i++) {
                LOG_VERBOSE(BSL_LS_APPL_SHELL, (BSL_META_U(unit, "0x%08x "), input_32[i]));
            }
            LOG_VERBOSE(BSL_LS_APPL_SHELL, (BSL_META_U(unit, "\n")));

            /* Writing the file to KAPS_TCM from offset 0x0 (start of ATCM) using SBUS */
            rv = jer_pp_xpt_arm_load_file_entry(unit, input_32, entry_num);
            if (rv != SOC_SAND_OK) {
                LOG_ERROR(BSL_LS_APPL_SHELL, (BSL_META_U(unit, "Error: Failed to jer_pp_xpt_arm_start()\n")));
                rv = sal_fclose(fp);
                if (rv != SOC_SAND_OK) {
                    LOG_ERROR(BSL_LS_APPL_SHELL, (BSL_META_U(unit, "Error: Failed to sal_fclose()\n")));
                }
                return CMD_FAIL;
            }

            entry_num += 1;
        }

        rv = sal_fclose(fp);
        if (rv != SOC_SAND_OK) {
            LOG_ERROR(BSL_LS_APPL_SHELL, (BSL_META_U(unit, "Error: Failed to sal_fclose()\n")));
            return CMD_FAIL;
        }
    }

    rv = jer_pp_xpt_arm_start_halt(unit, cpu_halt);
    if (rv != SOC_SAND_OK) {
        LOG_ERROR(BSL_LS_APPL_SHELL, (BSL_META_U(unit, "Error: Failed to jer_pp_xpt_arm_start()\n")));
        return CMD_FAIL;
    }

    return CMD_OK;
}

STATIC cmd_result_t
cmd_dpp_kaps_arm_image_load(int unit, args_t *a)
{
    int rv = CMD_OK,
        cpu_halt = 0,
        load_file = 1,
        init_tcm = 1;
    parse_table_t pt;

    char *file = NULL;

    if (SOC_IS_JERICHO_PLUS(unit)) {
        cli_out("Error(%d): KAPS ARM is only available in Jericho\n", unit);
        return CMD_FAIL;
    }
    parse_table_init(0, &pt);

    if (ARG_CNT(a) > 0) {
        parse_table_add(&pt, "File", PQ_STRING, (void *)0, &file, NULL);
        parse_table_add(&pt, "CpuHalt", PQ_INT, (void *)0, &cpu_halt, NULL);
        parse_table_add(&pt, "LoadFile", PQ_INT, (void *)1, &load_file, NULL);
        parse_table_add(&pt, "InitTCM", PQ_INT, (void *)1, &init_tcm, NULL);

        if (parse_arg_eq(a, &pt) < 0) {
            LOG_ERROR(BSL_LS_APPL_SHELL, (BSL_META_U(unit, "%s: Invalid option: %s\n"), ARG_CMD(a), ARG_CUR(a)));
            parse_arg_eq_done(&pt);
            return CMD_FAIL;
        }
    }

    LOG_ERROR(BSL_LS_APPL_SHELL, (BSL_META_U(unit, "%s: buf_size=%d, cpu_halt=%d, load_file=%d, init_tcm=%d\n"), ARG_CMD(a), KBP_CMD_DPP_KAPS_ARM_IMAGE_LOAD_BUF_SIZE, cpu_halt, load_file, init_tcm));

    rv = dpp_kaps_arm_image_load(unit, file, cpu_halt, load_file, init_tcm);
    if (rv != SOC_SAND_OK) {
        LOG_ERROR(BSL_LS_APPL_SHELL, (BSL_META_U(unit, "%s: Error: Failed to dpp_kaps_arm_image_load()\n"), ARG_CMD(a)));
        parse_arg_eq_done(&pt);
        return CMD_FAIL;
    }

    parse_arg_eq_done(&pt);
    return CMD_OK;
}
#endif

STATIC cmd_result_t
cmd_dpp_sdk_ver(int unit, args_t *a)
{
    const char *ver_str;

    if (ARG_CNT(a) > 0) {
        cli_out("%s: Invalid option: %s\n",
                ARG_CMD(a), ARG_CUR(a));
        return CMD_USAGE;
    }

    ver_str = kbp_device_get_sdk_version();

    if (ver_str == NULL) {
        cli_out("Error: dpp_kaps_get_sdk_ver(%d), sdk_ver string pointer is NULL\n", unit);
        return CMD_FAIL;
    }
    cli_out(ver_str);
    cli_out("\n");

    return CMD_OK;
}


STATIC cmd_result_t
cmd_dpp_flexible_mode_ref_app(int unit, args_t *a)
{
    uint32 soc_sand_rv;
    cmd_result_t rv = CMD_OK;
    parse_table_t pt;
    bcm_switch_concat_header_t concat_header = bcmSwitchConcatHeaderNone;
    uint8 verbosity_level = 0;
    uint8 tap_init = 0;

    if (!SOC_IS_JERICHO(unit) || !ARAD_KBP_IS_OP_OR_OP2) {
        cli_out("Error(%d): KBP flexible_mode_ref_app is only available in Jericho and above devices with KBP OP !\n", unit);
        return CMD_FAIL;
    }

    if (!(soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "ext_flexible_mode", 0))) {
        cli_out("Error(%d): KBP flexible_mode_ref_app is supported only with soc property custom_feature_ext_flexible_mode!\n", unit);
        return CMD_FAIL;
    }

    if (ARG_CNT(a) > 0) {
        /* Get parameters */
        parse_table_init(0, &pt);
        parse_table_add(&pt,"concat_header", PQ_INT, (void *) (0), &concat_header,  NULL);
        parse_table_add(&pt,"verbosity", PQ_DFL|PQ_NO_EQ_OPT|PQ_INT8, (void *) (0), &verbosity_level, NULL);
        parse_table_add(&pt,"tap_init", PQ_DFL|PQ_NO_EQ_OPT|PQ_INT8, (void *) (0), &tap_init, NULL);

        if (parse_arg_eq(a, &pt) < 0) {
            cli_out("%s: Invalid option: %s\n",ARG_CMD(a), ARG_CUR(a));
            cli_out(
                "\nflexible_mode_ref_app - kbp flexible mode reference according to the following parameters:"
                "\nUsage: kbp flexible_mode_ref_app [list of parameters]:"
                "\n\t\t[concat_header]    - Decided to concat extra packet header to master key or not "
                "\n\t\t  concat_header value: bcmSwitchConcatHeaderNone(0)/bcmSwitchConcatHeaderForwarding(1)/bcmSwitchConcatHeaderNextForwarding(2)"
                "\n\t\t[verbosity]        - Used to print information for the executed steps"
                "\n\t\t  verbosity value:     None(0) / Info(1) / Verbose(2) / Debug(3)"
                "\n\t\t[tap_init]         - "
                "\n\t\t  tap_init value:      Do not init(0) / Init(1)"
                "\n\n"
                );
            parse_arg_eq_done(&pt);
            return CMD_OK;
        }
        parse_arg_eq_done(&pt);
    }

    /* execute flexible mode reference application */
    soc_sand_rv = flexible_mode_ref_app(unit, concat_header, verbosity_level, tap_init);

    if(BCM_FAILURE(handle_sand_result(soc_sand_rv))) {
        cli_out("Error(%d): flexible_mode_ref_app failed\n", unit);
        rv = CMD_FAIL;
    }

    return rv;
}

STATIC cmd_result_t
cmd_dpp_kbp_deinit_appl(int unit, args_t *a)
{
    uint32 soc_sand_rv;
    cmd_result_t rv = CMD_OK;
    parse_table_t pt;
    uint32 second_elk_enable=0;

    if (ARG_CNT(a) > 0) {
        parse_table_init(0,&pt);
        parse_table_add(&pt,"SecondElkEnable",PQ_INT , (void *) (0), &second_elk_enable, NULL);

        if (parse_arg_eq(a, &pt) < 0) {
            cli_out("%s: Invalid option: %s\n", ARG_CMD(a), ARG_CUR(a));
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }
        parse_arg_eq_done(&pt);
    }

    soc_sand_rv = arad_kbp_deinit_app(unit, second_elk_enable, 0);
    if(BCM_FAILURE(handle_sand_result(soc_sand_rv))) {
        cli_out("Error: arad_kbp_deinit_appl(%d, &elk, 0)\n", unit);
        rv = CMD_FAIL;
    }

    return rv;
}

STATIC cmd_result_t
cmd_dpp_kaps_deinit(int unit, args_t *a)
{
    cmd_result_t rv = CMD_OK;
#ifdef BCM_88675_A0
    uint32 soc_sand_rv;

    soc_sand_rv = jer_kaps_deinit_app(unit, 0);
    if(BCM_FAILURE(handle_sand_result(soc_sand_rv))) {
        cli_out("Error: jer_kaps_deinit_app(%d, 0)\n", unit);
        rv = CMD_FAIL;
    }
#endif

    return rv;
}

int
dpp_kbp_init_prepare(
    int unit,
    uint32 core,
    uint32 kbp_mdio_id,
    uint32 kbp_ilkn_rev)
{
    uint32 soc_sand_rv;
    int rv = SOC_E_NONE;
    ARAD_INIT_ELK* elk;
    elk = &(SOC_DPP_CONFIG(unit)->arad->init.elk);

    /*
     * Register user data
     */
    elk->kbp_user_data[core].kbp_mdio_id = kbp_mdio_id;
    elk->kbp_user_data[core].device = unit;
    /*
     * Init device config, including callback and interface info
     */
    soc_sand_rv = arad_kbp_init_device_config_init(unit, core, kbp_ilkn_rev, cmd_dpp_kbp_callback_reset_device);
    rv = handle_sand_result(soc_sand_rv);
    if(BCM_FAILURE(rv)) {
        cli_out("Error: arad_kbp_init_device_config_init(%d, &elk)\n", unit);
    }

    if (elk->is_master) {

#if (defined(__DUNE_GTO_BCM_CPU__) || defined(__DUNE_WRX_BCM_CPU__))
        soc_sand_rv = arad_kbp_init_gpio_config(unit);
        rv = handle_sand_result(soc_sand_rv);
        if(BCM_FAILURE(rv)) {
            cli_out("Error: arad_kbp_init_gpio_config(%d)\n", unit);
        }
#endif
        /*
         * Only relevant for OP. For Nl12k, the device reset should be done when
         * initializing interface.
         */
        if (ARAD_KBP_IS_OP_OR_OP2) {
            soc_sand_rv = arad_kbp_init_kbp_reset(unit, core);
            rv = handle_sand_result(soc_sand_rv);
            if(BCM_FAILURE(rv)) {
                cli_out("Error: arad_kbp_init_kbp_reset(%d, &elk)\n", unit);
            }
        }
    }
    return rv;
}

int
dpp_kbp_init_kbp_transport(int unit, uint32 core)
{
    int res;
    kbp_status status;
    enum kbp_device_type kbp_type;
    uint32 flags = KBP_DEVICE_DEFAULT;
    ARAD_INIT_ELK *elk = &SOC_DPP_CONFIG(unit)->arad->init.elk;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    /* If there is a valid Transport layer, we don't need to allocate it again. */
    if (AradAppData[unit]->alg_kbp_xpt_p[core] != NULL) {
        SOC_EXIT;
    }

    /* Create the default allocator */
    res = default_allocator_create(&AradAppData[unit]->dalloc_p[core]);
    if (ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
        LOG_ERROR(BSL_LS_SOC_TCAM,
                  (BSL_META_U(unit,
                              "Error in %s(): default_allocator_create failed: %s!\n"),
                              FUNCTION_NAME(),
                              kbp_get_status_string(res)));
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 90, exit);
    }

    /* Legacy allocator for ARAD XPT */
    AradAppData[unit]->alloc_p[core] = NlmCmAllocator__ctor(&AradAppData[unit]->alloc_bdy[core]);
    if (!AradAppData[unit]->alloc_p[core]) {
        LOG_ERROR(BSL_LS_SOC_TCAM,
                  (BSL_META_U(unit,
                              "Error in %s(): Legacy allocator creation Failed!\n"), FUNCTION_NAME()));
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 90, exit);
    }

    if (ARAD_KBP_IS_OP_OR_OP2) {
        if (ARAD_KBP_IS_OP) {
            kbp_type = KBP_DEVICE_OP;
        } else {
            kbp_type = KBP_DEVICE_OP2;
        }

        /* Skip the second core init for OP/OP2 as we connect with only one KBP */
        /* Number of cores check added to fix compilation */
        if ((core == 1) && (SOC_DPP_DEFS_MAX(NOF_CORES) == 2)) {
            AradAppData[unit]->alg_kbp_xpt_p[1] = AradAppData[unit]->alg_kbp_xpt_p[0];
            SOC_EXIT;
        }
        if (elk->is_master) {
            status = kbp_pcie_init(kbp_type, flags,
                                   unit, AradAppData[unit]->dalloc_p[core],
                                   elk->kbp_serdes_pcie_init ? &AradAppData[unit]->device_config[core] : NULL,
                                   NULL,
                                   &AradAppData[unit]->alg_kbp_xpt_p[core]);
             if (status != KBP_OK) {
                LOG_ERROR(BSL_LS_SOC_TCAM,
                          (BSL_META_U(unit, "%s: Error in PCIE device driver init: %s\n"),
                           FUNCTION_NAME(),
                           kbp_get_status_string(status)));
                /*
                 * Stop the sequence if kbp_pcie_init returns an error.
                 * Any error here is fatal and cannot be worked around.
                 */
                return (_SHR_E_INIT);
            }

            /* add kbp_pcie_soft_reset(). in flexible mode, LTR registers have wrong values, so this API is used to clean it */
            status = kbp_pcie_soft_reset(AradAppData[unit]->alg_kbp_xpt_p[core]);
            if (status != KBP_OK) {
                LOG_ERROR(BSL_LS_SOC_TCAM,
                          (BSL_META_U(unit, "%s: Error in kbp_pcie_soft_reset: %s\n"),
                           FUNCTION_NAME(),
                           kbp_get_status_string(status)));
                SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 90, exit);
            }
            /* Only relevant for Dual Host mode. */
            if (ARAD_KBP_OP_IS_DUAL_MODE) {
                dev_handlers_info.xpt_p[core] = AradAppData[unit]->alg_kbp_xpt_p[core];
            }
            /*
             * All update and control path compare operations will go through
             * PCIE. We override the PCIE here to send compares only through
             * ROP path. This is optional
             */
            if (0) {
                
                void *handle;
                handle = arad_kbp_op_xpt_init(unit, core, AradAppData[unit]->dalloc_p[core]);
                status = kbp_pcie_set_property(AradAppData[unit]->alg_kbp_xpt_p[core], KBP_PCIE_REPLACE_SEARCH,
                                               handle, arad_op_search);
                if (status != KBP_OK) {
                    LOG_ERROR(BSL_LS_SOC_TCAM,
                              (BSL_META_U(unit, "%s: Error in overriding PCIE compare function: %s\n"),
                                          FUNCTION_NAME(),
                                          kbp_get_status_string(status)));
                    SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 90, exit);
                }
            }
        } else {
            AradAppData[unit]->alg_kbp_xpt_p[core] = dev_handlers_info.xpt_p[core];
        }
    } else {
        /* Currently Device Manager is flushing out each request immediately. */
        AradAppData[unit]->alg_kbp_xpt_p[core] = arad_kbp_xpt_init(unit,
                                                                   core,
                                                                   AradAppData[unit]->alloc_p[core],      /* General purpose memory allocator */
                                                                   1,  /* Max request count */
                                                                   1,   /* Max result count*/
                                                                   0);
    }

    if (AradAppData[unit]->alg_kbp_xpt_p[core] == NULL) {
        LOG_ERROR(BSL_LS_SOC_TCAM,
                  (BSL_META_U(unit,
                              "Error in %s(): arad_kbp_xpt_init() failed!\n"), FUNCTION_NAME()));
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 90, exit);
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in dpp_kbp_init_kbp_transport()", 0, 0);
}

int
dpp_kbp_deinit_kbp_transport(int unit, uint32 second_kbp_supported)
{
    uint32 core = second_kbp_supported ? 2 : 1;
    int res;
    ARAD_INIT_ELK *elk = &SOC_DPP_CONFIG(unit)->arad->init.elk;
    kbp_status status;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if (AradAppData[unit] == NULL) {
        SOC_EXIT;
    }
   /* destroy kbp_dm , was created in arad_kbp_deinit_nlm_dev_mgr() */
    while(core--) {
        /** Free the xpt pointer */
        if (elk->is_master) {
            if (ARAD_KBP_IS_OP_OR_OP2) {
                if (core == 0) {
                    status = kbp_pcie_destroy(AradAppData[unit]->dalloc_p[core],
                                              AradAppData[unit]->alg_kbp_xpt_p[core]);
                    if (status != KBP_OK) {
                        LOG_ERROR(BSL_LS_SOC_TCAM,
                                  (BSL_META_U(unit, "%s: Error in PCIE device driver init: %s\n"),
                                              FUNCTION_NAME(),
                                              kbp_get_status_string(status)));
                        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 90, exit);
                    }
                }
            } else {
                arad_kbp_xpt_destroy(AradAppData[unit]->alg_kbp_xpt_p[core]);
            }
        }
        /** Free the allocator */
        if (AradAppData[unit]->dalloc_p[core]) {
            res = default_allocator_destroy(AradAppData[unit]->dalloc_p[core]);
            if (ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
                LOG_ERROR(BSL_LS_SOC_TCAM,
                          (BSL_META_U(unit,
                                      "Error in %s(): default_allocator_destroy failed: %s!\n"),
                                      FUNCTION_NAME(),
                                      kbp_get_status_string(res)));
                SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 90, exit);
            }
        }
    }

    if(elk->is_master) {
        if(ARAD_KBP_OP_IS_DUAL_MODE) {
            ARAD_FREE(AradAppData[unit+1]);
        }
        ARAD_FREE(AradAppData[unit]);
    }
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in dpp_kbp_deinit_kbp_transport()", 0, 0);
}

int dpp_kbp_init_kbp_interface(int unit, uint32 core) {

    uint32 soc_sand_rv, flags;
    int rv = BCM_E_NONE;
    soc_pbmp_t ilkn_pbmp, kbp_port_bmp;
    soc_port_t    port, kbp_port = SOC_GPORT_INVALID;
    ARAD_INIT_ELK* elk;
    int tmp_core;
    int core_iter, core_id;

    elk = &(SOC_DPP_CONFIG(unit)->arad->init.elk);

    if (ARAD_KBP_IS_OP_OR_OP2) {
        if (core == 1 || !elk->is_master) {
            return rv;
        }
        core_iter = SOC_CORE_ALL;
    } else {
        core_iter = core;
    }

    SOC_PBMP_CLEAR(kbp_port_bmp);

    if ((elk->kbp_recover_enable) && !SOC_WARM_BOOT(unit)) {
        ilkn_pbmp = PBMP_IL_ALL(unit);
        SOC_DPP_CORES_ITER(core_iter, core_id) {
            SOC_PBMP_ITER(ilkn_pbmp, port) {
                rv = soc_port_sw_db_flags_get(unit, port, &flags);
                if(!BCM_FAILURE(rv)){
                    rv = soc_port_sw_db_core_get(unit, port, &tmp_core);
                    if(BCM_FAILURE(rv)){
                        cli_out("Error in %s():soc_port_sw_db_core_get failed\n", FUNCTION_NAME());
                        return rv;
                    }
                    if (SOC_PORT_IS_ELK_INTERFACE(flags) && (core_id == tmp_core)) {
                        kbp_port = port;
                        break;
                    }
                }
            }

            if (kbp_port == SOC_GPORT_INVALID) {
                /* in case we didn't find a valid port in core 0 - exit with BCM_E_PORT error*/
                if (core_id == 0) {
                    rv = BCM_E_PORT;
                    cli_out("Error in %s(): KBP port was not defined in core 0\n", FUNCTION_NAME());
                } else {  /* in case we didn't find a valid port in core 1 - exit with BCM_E_NONE*/
                    rv = BCM_E_NONE;
                }
                return rv;
            }
            SOC_PBMP_PORT_ADD(kbp_port_bmp, kbp_port);
        }
    }

    if ((elk->kbp_recover_enable) && !SOC_WARM_BOOT(unit)) {

        SOC_PBMP_ITER(kbp_port_bmp, kbp_port) {
            rv = bcm_port_control_set(unit, kbp_port, bcmPortControlRxEnable, 0);/*port=32,47=rx enable enum */
            if(BCM_FAILURE(rv)){
                cli_out("Error: ELK RX disabled, arad_kbp_init_kbp_interface(%d, &elk)\n", unit);
                return rv;   
            }
        }
        sal_usleep(2000);
    }

    /* Device Configuration Using MDIO */
    soc_sand_rv = arad_kbp_init_kbp_interface(unit, core);
    
    if (ARAD_KBP_IS_OP_OR_OP2) {
        rv = arad_kbp_op_phmf_fifo_reset(unit, 0, elk->kbp_user_data[core].kbp_mdio_id);
        if(BCM_FAILURE(rv)){
            cli_out("Error: arad_kbp_op_phmf_fifo_reset for core 0\n");
            return rv;
        }
        rv = arad_kbp_op_phmf_fifo_reset(unit, 1, elk->kbp_user_data[core].kbp_mdio_id);
        if(BCM_FAILURE(rv)){
            cli_out("Error: arad_kbp_op_phmf_fifo_reset for core 1\n");
            return rv;
        }
    }

    if ((elk->kbp_recover_enable) && (!SOC_WARM_BOOT(unit))){
        SOC_PBMP_ITER(kbp_port_bmp, kbp_port) {
            rv = bcm_port_control_set(unit, kbp_port,bcmPortControlRxEnable, 1);/*port=32,47=rx enable enum */
            if(BCM_FAILURE(rv)){
                cli_out("Error: ELK RX enabled, arad_kbp_init_kbp_interface(%d, &elk)\n", unit);
                return rv;
            }
        }
        sal_usleep(2000);
    }

    rv = BCM_E_NONE;
    rv = handle_sand_result(soc_sand_rv);
    if(BCM_FAILURE(rv)){
        cli_out("Error: arad_kbp_init_kbp_interface(%d, &elk)\n", unit);
    }
    return rv;   
}

STATIC cmd_result_t
cmd_dpp_kbp_init_kbp_interface(int unit, args_t *a)
{
    parse_table_t pt; 
    uint32 
        kbp_mdio_id = 0, kbp_ilkn_rev = 0, core = 0;

    if (ARG_CNT(a) > 0) {
        parse_table_init(0,&pt);

       /* KBP mdio ID format (kbp_mdio_id default for Negev is 0x101):
        * bits [5:6,8:9] - bus ID.
        * Arad has 8 external buses (0-7), two of these buses has connection on board (buses 4 and 5).
        * Assuming the KBP connected to bus 4 ==> 4b'0100 ==> bit[8]=1.
        * bit [7] - Internal select. Set to 0 for external phy access.
        * bits [0:4] - phy/kbp id ==> 0x1    
        */ 

        parse_table_add(&pt,"core",PQ_INT , (void *) (0), &core, NULL);
        parse_table_add(&pt,"mdio_id",PQ_INT , (void *) INT_TO_PTR(ARAD_KBP_APPL_MDIO_DEFAULT_ID), &kbp_mdio_id, NULL);
        parse_table_add(&pt,"ilkn_rev",PQ_INT , (void *) (ARAD_KBP_APPL_ILKN_REVERSE_DEFAULT), &kbp_ilkn_rev, NULL);

        if (parse_arg_eq(a, &pt) < 0) {
            cli_out("%s: Invalid option: %s\n", ARG_CMD(a), ARG_CUR(a));
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }
        parse_arg_eq_done(&pt);
    } 
    if (dpp_kbp_init_prepare(unit, core, kbp_mdio_id, kbp_ilkn_rev) < 0) {
        cli_out("Failed to prepare KBP init data. \n");
        return CMD_FAIL;
    }

    if (dpp_kbp_init_kbp_interface(unit, core) < 0) {
        cli_out("Failed to init KBP interface. \n");
        return CMD_FAIL;
    }
    return CMD_OK;
}

STATIC cmd_result_t
cmd_dpp_kbp_test_ip4_rpf_appl(int unit, args_t *a)
{
    uint32 num_entries, record_base_tbl[ARAD_KBP_FRWRD_IP_NOF_TABLES], ad_val_tbl[ARAD_KBP_FRWRD_IP_NOF_TABLES];
    parse_table_t pt; 
    cmd_result_t rv = CMD_OK;
    uint32 soc_sand_rv;

    if (ARG_CNT(a) > 0) {
    
        parse_table_init(0,&pt);

        parse_table_add(&pt,"NumEntries"    ,PQ_INT , (void *) (1)         , &num_entries     , NULL);
        parse_table_add(&pt,"RecordBaseTbl0",PQ_INT , (void *) (0x55551234), &record_base_tbl[0], NULL);
        parse_table_add(&pt,"RecordBaseTbl1",PQ_INT , (void *) (0xeeee1234), &record_base_tbl[1], NULL);
        parse_table_add(&pt,"RecordBaseTbl2",PQ_INT , (void *) (0x66661234), &record_base_tbl[2], NULL);
        parse_table_add(&pt,"RecordBaseTbl3",PQ_INT , (void *) (0x77771234), &record_base_tbl[3], NULL);
        parse_table_add(&pt,"ADValTbl0"     ,PQ_INT , (void *) (0xdead0620), &ad_val_tbl[0]     , NULL);
        parse_table_add(&pt,"ADValTbl1"     ,PQ_INT , (void *) (0xbeaf8321), &ad_val_tbl[1]     , NULL);
        parse_table_add(&pt,"ADValTbl2"     ,PQ_INT , (void *) (0x01020304), &ad_val_tbl[2]     , NULL);
        parse_table_add(&pt,"ADValTbl3"     ,PQ_INT , (void *) (0x0a0b0c0d), &ad_val_tbl[3]     , NULL);

        if (parse_arg_eq(a, &pt) < 0) {
            cli_out("%s: Invalid option: %s\n",  ARG_CMD(a), ARG_CUR(a));
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }
        parse_arg_eq_done(&pt);

        cli_out("num_entries=%d, record_base_tbl[0]=0x%x, record_base_tbl[1]=0x%x, record_base_tbl[2]=0x%x, record_base_tbl[3]=0x%x, "
                "ad_val_tbl[0]=0x%x, ad_val_tbl[1]=0x%x, ad_val_tbl[2]=0x%x, ad_val_tbl[3]=0x%x\n", 
                num_entries, record_base_tbl[0], record_base_tbl[1], record_base_tbl[2], record_base_tbl[3], 
                ad_val_tbl[0], ad_val_tbl[1], ad_val_tbl[2], ad_val_tbl[3]);

        soc_sand_rv = arad_kbp_test_ip4_rpf_NlmGenericTableManager(unit, num_entries, record_base_tbl, ad_val_tbl);
        if(BCM_FAILURE(handle_sand_result(soc_sand_rv))) {
            cli_out("Error: arad_kbp_test_ip4_rpf_NlmGenericTableManager(%d)\n", unit);
            return CMD_FAIL;
        } 
   
    }

    return rv;
}

STATIC cmd_result_t
cmd_dpp_kbp_test_add_rate(int unit, args_t *a)
{
    parse_table_t pt; 
    bcm_error_t rv = CMD_OK;

    int i;
    int entry_num;
    int entry_num_print_mod;

    int fec = 0x20000400;
    int encap_id = 0x40001000;
    int vrf = 0;
    int host = 0x0a00ff01;
    /* bcm_mac_t next_hop_mac  = {0x00, 0x00, 0x00, 0x03, 0x00, 0x01}; */
    bcm_mac_t next_hop_mac  = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    bcm_l3_host_t l3host;

    if (ARG_CNT(a) > 0) {
    
        parse_table_init(0,&pt);

        parse_table_add(&pt,"Host"              ,PQ_INT , (void *) (0x0a00ff01)         , &host     , NULL);
        parse_table_add(&pt,"Vrf"               ,PQ_INT , (void *) (0x0)                , &vrf      , NULL);
        parse_table_add(&pt,"Fec"               ,PQ_INT , (void *) (0x20000400)         , &fec      , NULL);
        parse_table_add(&pt,"EncapId"           ,PQ_INT , (void *) (0x40001000)         , &encap_id , NULL);
        parse_table_add(&pt,"EntryNum"          ,PQ_INT , (void *) (5000)               , &entry_num , NULL);
        parse_table_add(&pt,"EntryNumPrintMod"  ,PQ_INT , (void *) (1000)               , &entry_num_print_mod , NULL);

        if (parse_arg_eq(a, &pt) < 0) {
            cli_out("%s: Invalid option: %s\n", ARG_CMD(a), ARG_CUR(a));
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }
        parse_arg_eq_done(&pt);
        
        cli_out("host=0x%x, vrf=0x%x, fec=0x%x, encap_id=0x%x, entry_num=0x%x, entry_num_print_mod=0x%x\n", 
                host, vrf, fec, encap_id, entry_num, entry_num_print_mod);

    } else {
        return CMD_USAGE;   
    }

    soc_sand_ll_timer_clear();

    bcm_l3_host_t_init(&l3host);
    l3host.l3a_flags = 0;
    l3host.l3a_ip_addr = host;
    l3host.l3a_vrf = vrf;
    l3host.l3a_intf = fec; /* point to FEC to get out-interface  */
    l3host.l3a_modid = 0;
    /* set encap id to point to MAC address */
    /* as encap id is valid (!=0), host will point to FEC + outlif (MAC), and FEC will be "fixed" not to overwrite outlif */
    l3host.encap_id = encap_id;
    sal_memcpy(l3host.l3a_nexthop_mac, next_hop_mac, 6); /* next hop mac attached directly */

    soc_sand_ll_timer_set("Full", 1);

    for (i = 0; i < entry_num; i ++)
    {
        /* printf("******************** bcm_l3_host_add i=%d, l3host.l3a_ip_addr=0x%x\n", i, l3host.l3a_ip_addr, rv); */
        rv = bcm_l3_host_add(unit, &l3host);
        if (rv != BCM_E_NONE) {
             printf("Error, bcm_l3_host_add Failed, l3host.l3a_ip_addr=0x%x, rv=0x%x\n", l3host.l3a_ip_addr, rv);
        }
        
        if ((i % entry_num_print_mod) == 1) {
            printf("time=%u[us] i=%d, l3host.l3a_ip_addr=0x%x\n", sal_time_usecs(), i, l3host.l3a_ip_addr);
        }

        l3host.l3a_ip_addr ++;
    }

    /* sand_ll_timer_stop(1); */

    soc_sand_ll_timer_stop_all();
    soc_sand_ll_timer_print_all();

    return (rv == BCM_E_NONE) ? CMD_OK : CMD_FAIL;
}

STATIC cmd_result_t
cmd_dpp_kbp_test_acl_add_rate(int unit, args_t *a)
{
    int i;
    int entry_num;
    int entry_num_print_mod;
    int prio_mode;
    int priority;
    int cached_entries_num;
    parse_table_t pt;

    bcm_field_group_t group_id_elk;
    bcm_field_entry_t ent_elk;

    if (ARG_CNT(a) > 0) {
    
        parse_table_init(unit,&pt);

        parse_table_add(&pt,"PrioMode"          ,PQ_INT , (void *) (0x0)                , &prio_mode            , NULL);
        parse_table_add(&pt,"EntryNum"          ,PQ_INT , (void *) (64*1024)            , &entry_num            , NULL);
        parse_table_add(&pt,"EntryNumPrintMod"  ,PQ_INT , (void *) (1000)               , &entry_num_print_mod  , NULL);
        parse_table_add(&pt,"CachedEntNum"      ,PQ_INT , (void *) (0)                  , &cached_entries_num   , NULL);
        parse_table_add(&pt,"GroupId"           ,PQ_INT , (void *) (40)                 , &group_id_elk         , NULL);

        if (parse_arg_eq(a, &pt) < 0) {
            cli_out("%s: Invalid option: %s\n", ARG_CMD(a), ARG_CUR(a));
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }
        parse_arg_eq_done(&pt);
        
        cli_out("PrioMode=0x%x, EntryNum=0x%x, EntryNumPrintMod=0x%x, CachedEntNum=0x%x GroupId=0x%x\n", 
                prio_mode, entry_num, entry_num_print_mod, cached_entries_num, group_id_elk);

    } else {
        return CMD_USAGE;   
    }

    soc_sand_ll_timer_clear();
    soc_sand_ll_timer_set("Full", 1);

    if (cached_entries_num > 0) {
        bcm_switch_control_set(unit, bcmSwitchFieldCache, 1);
    }

    /* step1: Add into Elk group */
    cli_out("step1: Add entries into Elk group\n");
    for (i = 0; i < entry_num; i++)
    {
        switch (prio_mode)
        {
           case 0:
               priority = 10;
               break;
           case 1:
               priority = 10 + i;
               break;
           case 2:
               priority = entry_num - i;
               break;
           default:
               cli_out("Invalid PrioMode.\n");
               return CMD_USAGE;
        }
        if (bcm_field_entry_create(unit, group_id_elk, &ent_elk) != BCM_E_NONE) {
            cli_out("Failed to create entry for Elk group %d\n", group_id_elk);
            return CMD_FAIL;
        }
        if (bcm_field_qualify_SrcIp(unit, ent_elk, i + 1, 0xffffffff) != BCM_E_NONE) {
            cli_out("Failed to qualify SrcIp 0x%08x for entry %d\n", i + 1, ent_elk);
            return CMD_FAIL;
        }
        if (bcm_field_action_add(unit, ent_elk, bcmFieldActionExternalValue3Set, entry_num - i, 0) != BCM_E_NONE) {
            cli_out("Failed to add action ExternalValue3Set 0x%08x for entry %d\n", entry_num - i, ent_elk);
            return CMD_FAIL;
        }
        if (bcm_field_entry_prio_set(unit, ent_elk, priority) != BCM_E_NONE) {
            cli_out("Failed to set priority %d for entry %d\n", priority, ent_elk);
            return CMD_FAIL;
        }
        if (bcm_field_entry_install(unit, ent_elk) != BCM_E_NONE) {
            cli_out("Failed to install entry %d\n", ent_elk);
            return CMD_FAIL;
        }
        if ((cached_entries_num > 0) && ((i % cached_entries_num) == (cached_entries_num-1))) {
            bcm_switch_control_set(unit, bcmSwitchFieldCommit, 0);
        }
        if ((entry_num_print_mod > 0) && ((i % entry_num_print_mod) == (entry_num_print_mod-1))) {
            soc_sand_ll_timer_stop_all();
            soc_sand_ll_timer_print_all();
            soc_sand_ll_timer_clear();
            soc_sand_ll_timer_set("Full", 1);
        }
    }

    /* step2: Destroy all the entries */
    cli_out("step2: Remove entries from Elk group\n");
    for (i = _BCM_DPP_FIELD_ENT_BIAS(unit, ExternalTcam);
         i < (_BCM_DPP_FIELD_ENT_BIAS(unit, ExternalTcam) + entry_num);
         i++)
    {
        if (bcm_field_entry_destroy(unit, i) != BCM_E_NONE) {
            cli_out("Failed to remove entry %d from Elk group\n", i - _BCM_DPP_FIELD_ENT_BIAS(unit, ExternalTcam));
            return CMD_FAIL;
        }
    }

    if (cached_entries_num > 0) {
        bcm_switch_control_set(unit, bcmSwitchFieldCache, 0);
    }

    return CMD_OK;
}

/* measure the rate of entries insertion to kbp for IPV4 application*/
STATIC int
cmd_dpp_kbp_test_add_measure_rate(int unit, args_t *a)
{
    parse_table_t pt; 
    bcm_error_t rv = CMD_OK;

    int i,rate=0;
    int entry_num;
    int entry_num_print_mod;

    int fec = 0x20000400;
    int encap_id = 0x40001000;
    int vrf = 0;
    int host = 0x0a00ff01;
    int cache_mode = 0;
	int dc_mode = 0;
    /* bcm_mac_t next_hop_mac  = {0x00, 0x00, 0x00, 0x03, 0x00, 0x01}; */
    bcm_mac_t next_hop_mac  = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    bcm_l3_host_t l3host;

    if (ARG_CNT(a) >= 0) {
    
        parse_table_init(0,&pt);

        parse_table_add(&pt,"Host"              ,PQ_INT , (void *) (0x0a00ff01)         , &host     , NULL);
        parse_table_add(&pt,"Vrf"               ,PQ_INT , (void *) (0x0)                , &vrf      , NULL);
        parse_table_add(&pt,"DC"	            ,PQ_INT , (void *) (0x0)                , &dc_mode  , NULL);
        parse_table_add(&pt,"Fec"               ,PQ_INT , (void *) (0x20000400)         , &fec      , NULL);
        parse_table_add(&pt,"EncapId"           ,PQ_INT , (void *) (0x40001000)         , &encap_id , NULL);
        parse_table_add(&pt,"EntryNum"          ,PQ_INT , (void *) (5000)               , &entry_num , NULL);
        parse_table_add(&pt,"EntryNumPrintMod"  ,PQ_INT , (void *) (1000)               , &entry_num_print_mod , NULL);
        parse_table_add(&pt,"ChachMode"         ,PQ_INT , (void *) (0)                  , &cache_mode , NULL);

        if (parse_arg_eq(a, &pt) < 0) {
            cli_out("%s: Invalid option: %s\n", ARG_CMD(a), ARG_CUR(a));
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }
        parse_arg_eq_done(&pt);
        
        cli_out("host=0x%x, vrf=0x%x, fec=0x%x, encap_id=0x%x, entry_num=0x%x, entry_num_print_mod=0x%x cache_mode=%d DC=%d\n", 
                host, vrf, fec, encap_id, entry_num, entry_num_print_mod,cache_mode,dc_mode);

    } else {
        return CMD_USAGE;   
    }

    soc_sand_ll_timer_clear();
    soc_sand_ll_timer_set("ARAD_KBP_TIMERS_APPL", ARAD_KBP_TIMERS_APPL);

    if(cache_mode != 0){
        soc_sand_ll_timer_set("ARAD_KBP_TIMERS_BCM", ARAD_KBP_TIMERS_BCM);
        bcm_switch_control_set(unit, bcmSwitchFieldCache, 1);
        soc_sand_ll_timer_stop(ARAD_KBP_TIMERS_BCM);
    }

    bcm_l3_host_t_init(&l3host);
    l3host.l3a_flags = 0;
    l3host.l3a_ip_addr = host;
    l3host.l3a_vrf = vrf;
    l3host.l3a_intf = 0; /* point to FEC to get out-interface  */
    l3host.l3a_modid = 0;
	if (dc_mode == 1) {
		l3host.l3a_flags2 = BCM_L3_FLAGS2_SCALE_ROUTE;
		l3host.l3a_vrf = vrf&0x00FF;
	}

    l3host.l3a_port_tgid = 201; /* egress port to send packet to */

    /* set encap id to point to MAC address */
    /* as encap id is valid (!=0), host will point to FEC + outlif (MAC), and FEC will be "fixed" not to overwrite outlif */
    l3host.encap_id = encap_id;
    sal_memcpy(l3host.l3a_nexthop_mac, next_hop_mac, 6); /* next hop mac attached directly */

    for (i = 0; i < entry_num; i ++)
    {
        /* printf("******************** bcm_l3_host_add i=%d, l3host.l3a_ip_addr=0x%x\n", i, l3host.l3a_ip_addr, rv); */
		soc_sand_ll_timer_set("ARAD_KBP_TIMERS_BCM", ARAD_KBP_TIMERS_BCM);
		rv = bcm_l3_host_add(unit, &l3host);
		soc_sand_ll_timer_stop(ARAD_KBP_TIMERS_BCM);

		if (rv != BCM_E_NONE) {
             printf("Error, bcm_l3_host_add Failed, l3host.l3a_ip_addr=0x%x, rv=0x%x\n", l3host.l3a_ip_addr, rv);
        }
        
        if ((i % entry_num_print_mod) == 1) {
            printf("time=%u[us] i=%d, l3host.l3a_ip_addr=0x%x\n", sal_time_usecs(), i, l3host.l3a_ip_addr);
        }

        l3host.l3a_ip_addr++;
    }

    if(cache_mode != 0){
        soc_sand_ll_timer_set("ARAD_KBP_TIMERS_BCM", ARAD_KBP_TIMERS_BCM);
        bcm_switch_control_set(unit, bcmSwitchFieldCommit, 0);
        soc_sand_ll_timer_stop(ARAD_KBP_TIMERS_BCM);
    }

    soc_sand_ll_timer_stop(ARAD_KBP_TIMERS_APPL);

    soc_sand_ll_timer_stop_all();
    /*soc_sand_ll_timer_print_all();*/

	rate = dpp_kbp_ipv4_print_timers(entry_num);

    return rate;
}

STATIC uint32	
get_kbp_entry_params(uint32 					unit,
					 uint32						frwrd_table_id,
					 struct kbp_db 				**db_p,
					 struct kbp_ad_db			**ad_db_p)
{
	int32 res;
	SOC_SAND_INIT_ERROR_DEFINITIONS(0);
   
    res = arad_kbp_alg_kbp_db_get(
              unit,
              frwrd_table_id, 
              db_p
          );

    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    SOC_SAND_CHECK_NULL_PTR(db_p, 30, exit);

    res =  arad_kbp_alg_kbp_ad_db_get(
            unit,
            frwrd_table_id, 
            ad_db_p
        );

    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

	SOC_SAND_CHECK_NULL_PTR(ad_db_p, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in get_kbp_entry_params()",0,0);

}

STATIC int
/* measure the rate of entries direct insertion to kbp for IPV4 application
 * Call directly the Transport layer function (imstead of BCM->SOC->transport 
*/

cmd_dpp_kbp_test_add_measure_rate_direct_call(int unit, args_t *a)
{
    parse_table_t pt; 

	int32 res;
    int i=0,rate=0;
	int entry_num			= 5000;
	int host 				= 0x0a00ff00;
	int vrf 				= 0;

	uint8 data[100]		 = {0};
	uint8 assoData[100]	 = {0};
	uint32 prefix_len;
	uint32 tblIndex;
	uint32 host_32bit;
	uint16 vrf_16bit;

    struct kbp_db 		*db_p 		= NULL;
    struct kbp_ad_db 	*ad_db_p 	= NULL;
    struct kbp_entry 	*db_entry 	= NULL;
    struct kbp_ad 		*ad_entry 	= NULL;

	SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if (ARG_CNT(a) >= 0) {
    
        parse_table_init(0,&pt);

        parse_table_add(&pt,"Host"              ,PQ_INT , (void *) (0x0a00ff00)         , &host     , NULL);
        parse_table_add(&pt,"Vrf"               ,PQ_INT , (void *) (0x0)                , &vrf      , NULL);
        parse_table_add(&pt,"EntryNum"          ,PQ_INT , (void *) (5000)               , &entry_num , NULL);

        if (parse_arg_eq(a, &pt) < 0) {
            cli_out("%s: Invalid option: %s\n", ARG_CMD(a), ARG_CUR(a));
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }
        parse_arg_eq_done(&pt);
        
        cli_out("host=0x%x, vrf=0x%x, entry_num=0x%x\n", host, vrf, entry_num);

    } else {
        return CMD_USAGE;   
    }

	for(i=0;i<100;i++) {
		assoData[i] = i;
	}
	prefix_len = 48;
	tblIndex   = 0;

	host_32bit = (uint32)(host&0xffffffff);
	vrf_16bit  = (uint16)(vrf&0xffff);

	soc_sand_ll_timer_clear();
    soc_sand_ll_timer_set("ARAD_KBP_TIMERS_APPL", ARAD_KBP_TIMERS_APPL);
    soc_sand_ll_timer_set("ARAD_KBP_TIMERS_BCM", ARAD_KBP_TIMERS_BCM);
	res = get_kbp_entry_params(unit,tblIndex,&db_p,&ad_db_p);

	for (i = 0; i < entry_num; i++) {
		host_32bit++;
		data[0] = (vrf_16bit>>8);
		data[1] = (vrf_16bit&0xff);
		data[2] = (host_32bit & 0xff000000)>>24;
		data[3] = (host_32bit & 0x00ff0000)>>16;
		data[4] = (host_32bit & 0x0000ff00)>>8;
		data[5] = (host_32bit & 0x000000ff);

		res = kbp_db_add_prefix(
                db_p, 
                (uint8*)data, 
                prefix_len,
                &db_entry);

		if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
            SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 40, exit);
        }

		res = kbp_ad_db_add_entry(
                ad_db_p,
                (uint8*)assoData,
                &ad_entry
              );

       if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
            SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 60, exit);
        }

	   res = kbp_entry_add_ad(
			db_p,
			db_entry,
			ad_entry
			);

	   if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
			SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 70, exit);
		}

		soc_sand_ll_timer_set("ARAD_KBP_IPV4_TIMERS_LPM_ROUTE_ADD", ARAD_KBP_IPV4_TIMERS_LPM_ROUTE_ADD);

		res = kbp_db_install(db_p);

        soc_sand_ll_timer_stop(ARAD_KBP_IPV4_TIMERS_LPM_ROUTE_ADD);

        if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
			SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 80, exit);

    	}
	}

    soc_sand_ll_timer_stop(ARAD_KBP_TIMERS_BCM);
	soc_sand_ll_timer_stop(ARAD_KBP_TIMERS_APPL);
    soc_sand_ll_timer_stop_all();
	rate = dpp_kbp_ipv4_print_timers(entry_num);

    return rate;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in cmd_dpp_kbp_test_add_measure_rate_direct_call()",0,0);

}

STATIC int
/* measure the rate of entries insertion to kbp for ACLs*/
cmd_dpp_kbp_test_acl_add_measure_rate(int unit, args_t *a)
{
    int i;
    int entry_num;
    int prio_mode;
    int priority;
    int cache_mod;
    parse_table_t pt;
    int is_serial_rpf = 0; 
    bcm_field_action_t external_value_action;
    cmd_result_t rv = CMD_OK;
	int rate = 0;

    bcm_field_group_t group_id_elk;
    bcm_field_entry_t ent_elk;

    if (SOC_IS_ARADPLUS(unit)) {
        is_serial_rpf = (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "ext_rpf_fwd_parallel", 0) == 0);
    }

    if (is_serial_rpf) {
        external_value_action = bcmFieldActionExternalValue1Set;
    }
    else {
        external_value_action = bcmFieldActionExternalValue2Set;
    }

    if (ARG_CNT(a) >= 0) {
    
        parse_table_init(0,&pt);

        parse_table_add(&pt,"PrioMode"          ,PQ_INT , (void *) (0x0)                , &prio_mode            , NULL);
        parse_table_add(&pt,"EntryNum"          ,PQ_INT , (void *) (64*1024)            , &entry_num            , NULL);
        parse_table_add(&pt,"CacheMode"        ,PQ_INT , (void *) (0)                   , &cache_mod            , NULL);
        parse_table_add(&pt,"GroupId"           ,PQ_INT , (void *) (100)                , &group_id_elk         , NULL);

        if (parse_arg_eq(a, &pt) < 0) {
            cli_out("%s: Invalid option: %s\n", ARG_CMD(a), ARG_CUR(a));
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }
        parse_arg_eq_done(&pt);
        
        cli_out("PrioMode=0x%x, EntryNum=0x%x, CacheMode=0x%x GroupId=0x%x\n", 
                prio_mode, entry_num, cache_mod, group_id_elk);

    } else {
        return CMD_USAGE;   
    }

    soc_sand_ll_timer_clear();
    soc_sand_ll_timer_set("ARAD_KBP_TIMERS_APPL", ARAD_KBP_TIMERS_APPL);

    if (cache_mod != 0) {
        soc_sand_ll_timer_set("ARAD_KBP_TIMERS_BCM", ARAD_KBP_TIMERS_BCM);
        bcm_switch_control_set(unit, bcmSwitchFieldCache, 1);
        soc_sand_ll_timer_stop(ARAD_KBP_TIMERS_BCM);
    }

    /* step1: Add into Elk group */
    cli_out("step1: Add entries into Elk group\n");
    for (i = 0; i < entry_num; i++)
    {
        switch (prio_mode)
        {
           case 0:
               priority = 10;
               break;
           case 1:
               priority = 10 + i;
               break;
           case 2:
               priority = entry_num - i;
               break;
           default:
               cli_out("Invalid PrioMode.\n");
               rv = CMD_USAGE;
               return rv;
        }

        soc_sand_ll_timer_set("ARAD_KBP_TIMERS_BCM",ARAD_KBP_TIMERS_BCM);
        rv = bcm_field_entry_create(0, group_id_elk, &ent_elk);
        if (rv != CMD_OK) {
            cli_out("Error in bcm_field_entry_create\n");
            return CMD_FAIL;
        }
        rv = bcm_field_qualify_SrcIp(0, ent_elk, i+1, 0xffffffff);
        if (rv != CMD_OK) {
            cli_out("Error in bcm_field_qualify_SrcIp\n");
            return CMD_FAIL;
        }
        rv = bcm_field_action_add(0, ent_elk, external_value_action, entry_num - i, 0);
        if (rv != CMD_OK) {
            cli_out("Error in bcm_field_action_add\n");
            return CMD_FAIL;
        }
        rv = bcm_field_entry_prio_set(0, ent_elk, priority);
        if (rv != CMD_OK) {
            cli_out("Error in bcm_field_entry_prio_set\n");
            return CMD_FAIL;
        }
        rv = bcm_field_entry_install(0, ent_elk);
        if (rv != CMD_OK) {
            cli_out("Error in bcm_field_entry_install\n");
            return CMD_FAIL;
        }
        soc_sand_ll_timer_stop(ARAD_KBP_TIMERS_BCM);
    }

    if (cache_mod != 0) {
        soc_sand_ll_timer_set("ARAD_KBP_TIMERS_BCM", ARAD_KBP_TIMERS_BCM);
        bcm_switch_control_set(unit, bcmSwitchFieldCommit, 0);
        soc_sand_ll_timer_stop(ARAD_KBP_TIMERS_BCM);
    }

    soc_sand_ll_timer_stop(ARAD_KBP_TIMERS_APPL);

    soc_sand_ll_timer_stop_all();
    rate = dpp_kbp_acl_print_timers(entry_num);

    /* step2: Destroy all the entries */
    cli_out("step2: Remove entries from Elk group\n");
    for (i=_BCM_DPP_FIELD_ENT_BIAS(unit, ExternalTcam);i<(_BCM_DPP_FIELD_ENT_BIAS(unit, ExternalTcam) + entry_num); i++)
    {
        bcm_field_entry_destroy(0,i);
    }

    soc_sand_ll_timer_stop_all();
    soc_sand_ll_timer_clear();


    return rate;
}

STATIC int
cmd_dpp_kbp_test_ipv4_random_measure_rate(int unit, args_t *a)
{
    int             i;
    int             entry_num = 64*1024;
    int             cached_mode = 0;
    parse_table_t   pt; 
    bcm_l3_route_t  routeInfo; 
    bcm_ip_t        dip;
    bcm_vrf_t       vrf = 0;
    bcm_if_t        fec_idx = 2000;
    bcm_error_t     rv = BCM_E_NONE;
    int             prefix_len = 32;
	int rate;

    if (ARG_CNT(a) > 0) {
        parse_table_init(0,&pt);

        parse_table_add(&pt,"EntryNum"          ,PQ_INT , (void *) (64*1024)            , &entry_num            , NULL);
        parse_table_add(&pt,"CachedMode"      	,PQ_INT , (void *) (0)                  , &cached_mode   		, NULL);

        if (parse_arg_eq(a, &pt) < 0) {
            cli_out("%s: Invalid option: %s\n", ARG_CMD(a), ARG_CUR(a));
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }
        parse_arg_eq_done(&pt);
    }
    
    cli_out("EntryNum=0x%x, CachedMode=0x%x\n", entry_num, cached_mode);

    soc_sand_ll_timer_clear();
    soc_sand_ll_timer_set("ARAD_KBP_TIMERS_APPL", ARAD_KBP_TIMERS_APPL);

	if (cached_mode != 0) {
		soc_sand_ll_timer_set("ARAD_KBP_TIMERS_BCM", ARAD_KBP_TIMERS_BCM);
        bcm_switch_control_set(unit, bcmSwitchFieldCache, 1);
		soc_sand_ll_timer_stop(ARAD_KBP_TIMERS_BCM);
    }

    cli_out("step1: Add routes\n");
    for (i = 0; i < entry_num; i++)
    {
			bcm_l3_route_t_init(&routeInfo);
            dip = (1<<(32-prefix_len))*((i/15)+1);
            routeInfo.l3a_vrf = vrf;
            routeInfo.l3a_intf = fec_idx;
            routeInfo.l3a_subnet = dip;
            routeInfo.l3a_ip_mask = 0xffffffff<<(32-prefix_len);
			soc_sand_ll_timer_set("ARAD_KBP_TIMERS_BCM", ARAD_KBP_TIMERS_BCM);
            rv = bcm_l3_route_add(unit,&routeInfo);
			soc_sand_ll_timer_stop(ARAD_KBP_TIMERS_BCM);
			if (rv != BCM_E_NONE) {
                printf("Error, bcm_l3_route_add Failed, routeInfo.l3a_subnet=0x%x, rv=0x%x\n", routeInfo.l3a_subnet, rv);
                return CMD_FAIL;
            }

            prefix_len--;
            if (prefix_len <= 17)
            {
                prefix_len = 32;
            }
    }
	soc_sand_ll_timer_stop(ARAD_KBP_TIMERS_APPL);
	soc_sand_ll_timer_stop_all();
	rate = dpp_kbp_ipv4_print_timers(entry_num);
    return rate;
}

/*generates array of 60k ipv6 routes with full mask*/
STATIC void
generate_ipv6_routes_array(bcm_l3_route_t* routesArray,int numRoutes)
{
    bcm_ip6_t Ipv6Mask = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff,0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    bcm_ip6_t currRoute = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00,0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    int counterA = 0,counterB = 0,totalCounter = 0;
    int startTime  = 0,endTime  = 0;

    printf("Started creating routes array\n...\n");
    startTime = sal_time_usecs();

    for(counterA = 0 ; counterA < 235 ; counterA ++){
        for(counterB = 0 ; counterB < 256 ; counterB ++){
            currRoute[totalCounter%16] = counterA;
            currRoute[(totalCounter+1)%16] = counterB;
            bcm_l3_route_t_init(&routesArray[totalCounter]);
            sal_memcpy(routesArray[totalCounter].l3a_ip6_net, currRoute, 16);
            sal_memcpy(routesArray[totalCounter].l3a_ip6_mask, Ipv6Mask, 16);
            routesArray[totalCounter].l3a_flags=BCM_L3_IP6;
            totalCounter++;
            if(totalCounter == numRoutes){
                break;
            }
        }
        if(totalCounter == numRoutes){
            break;
        }
    }

    endTime = sal_time_usecs();

    printf("Finished creating routes array after %d us\n\n",endTime-startTime);
}

STATIC int
cmd_dpp_kbp_test_ipv6_performance_measure(int unit, args_t *a)
{
    int retVal  = 0;
    int startTime = 0;
    int endTime = 0;
    int IPv6Number = 0;
    int loopCounter = 0;
    int innerLoopCounter = 0;
    int avgPerformanceArray[30];
    double sum = 0,num = 0;
    double avgPerformance = 0;
    int minTestNum = 2000;
    bcm_l3_route_t *routesArray = NULL;

    routesArray = (bcm_l3_route_t *)sal_alloc(sizeof(bcm_l3_route_t)*60000,"routes_array");

    if(SOC_IS_QUX(unit)){
        IPv6Number = 8000;
    }   
    else if(SOC_IS_QAX(unit)){
        IPv6Number = 30000;
    }
    else{
        IPv6Number = 60000;
    }
    generate_ipv6_routes_array(routesArray,IPv6Number);

    /*add route*/
    printf("now testing bcm_l3_route_add :\n");
    for(loopCounter = 0 ; loopCounter < (IPv6Number/minTestNum) ; loopCounter++) /*measure performance for each 2k entries alone*/
    {
        startTime = sal_time_usecs();

        for(innerLoopCounter = 0 ; innerLoopCounter < minTestNum ; innerLoopCounter++)
        {
            retVal = bcm_l3_route_add(0, &routesArray[innerLoopCounter + minTestNum*loopCounter]);
            if(retVal != 0)
            {
                printf("error occurred while adding route, index %d ,retVal %d\n",innerLoopCounter + minTestNum*loopCounter,retVal);
                sal_free(routesArray);
                return BCM_E_FAIL;
            }
        }

        endTime = sal_time_usecs();

        printf("    %3dst 2K routes add done in : %d us\n",loopCounter+1,endTime-startTime);
        avgPerformanceArray[loopCounter] = endTime-startTime;
    }

    for(loopCounter = 0 ; loopCounter < (IPv6Number/minTestNum) ; loopCounter++){ /*measure average performance*/
        sum += avgPerformanceArray[loopCounter];
        num++;
    }
    avgPerformance = sum/num;
    if(avgPerformance > 500000.0){
        printf("ERROR : the average time for bcm_l3_route_add is higher than 500000 us for 2k entries : %f us\n\n",avgPerformance);
        sal_free(routesArray);
        return BCM_E_FAIL;
    }
    else{
        printf("the average time for 2k bcm_l3_route_add : %f us\n\n",avgPerformance);
    }

    /*update route*/
    sum = 0;
    num = 0;

    printf("now testing bcm_l3_route_add (Update) :\n");
    for(loopCounter = 0 ; loopCounter < (IPv6Number/minTestNum) ; loopCounter++)
    {
        startTime = sal_time_usecs();

        for(innerLoopCounter = 0 ; innerLoopCounter < minTestNum ; innerLoopCounter++)
        {
            retVal = bcm_l3_route_add(0, &routesArray[innerLoopCounter + minTestNum*loopCounter]);
            if(retVal != 0)
            {
                printf("error Update route, index %d , retVal %d\n",innerLoopCounter + minTestNum*loopCounter,retVal);
                sal_free(routesArray);
                return BCM_E_FAIL;
            }
        }

        endTime = sal_time_usecs();

        printf("    %3dst 2K routes update done in : %d us\n",loopCounter+1,endTime-startTime);
        avgPerformanceArray[loopCounter] = endTime-startTime;
    }

    for(loopCounter = 0 ; loopCounter < (IPv6Number/minTestNum) ; loopCounter++){ /*measure average performance*/
        sum += avgPerformanceArray[loopCounter];
        num++;
    }
    avgPerformance = sum/num;
    if(avgPerformance > 400000.0){
        printf("ERROR : the average time for bcm_l3_route_add (Update) is higher than 400000 us for 2k entries : %f us\n\n",avgPerformance);
        sal_free(routesArray);
        return BCM_E_FAIL;
    }
    else{
        printf("the average time for 2k bcm_l3_route_add (Update) : %f us\n\n",avgPerformance);
    }

    /*delete route*/
    sum = 0;
    num = 0;

    printf("now testing bcm_l3_route_delete :\n");
    for(loopCounter = 0 ; loopCounter < (IPv6Number/minTestNum) ; loopCounter++)
    {
        startTime = sal_time_usecs();

        for(innerLoopCounter = 0 ; innerLoopCounter < minTestNum ; innerLoopCounter++)
        {
            retVal = bcm_l3_route_delete(0, &routesArray[innerLoopCounter + minTestNum*loopCounter]);
            if(retVal != 0)
            {
                printf("error while deleting route, index %d , retVal %d\n",innerLoopCounter + minTestNum*loopCounter,retVal);
                sal_free(routesArray);
                return BCM_E_FAIL;
            }
        }

        endTime = sal_time_usecs();

        printf("    %3dst 2K routes delete done in : %d us\n",loopCounter+1,endTime-startTime);
        avgPerformanceArray[loopCounter] = endTime-startTime;
    }

    for(loopCounter = 0 ; loopCounter < (IPv6Number/minTestNum) ; loopCounter++){ /*measure average performance*/
        sum += avgPerformanceArray[loopCounter];
        num++;
    }
    avgPerformance = sum/num;
    if(avgPerformance > 600000.0){
        printf("ERROR : the average time for bcm_l3_route_delete is higher than 600000 us for 2k entries : %f us\n\n",avgPerformance);
        sal_free(routesArray);
        return BCM_E_FAIL;
    }
    else{
        printf("the average time for 10k bcm_l3_route_delete (Update) : %f us\n\n",avgPerformance);
    }

    sal_free(routesArray);
    return BCM_E_NONE;
}

STATIC int test_kbp_entry_add(int unit, uint32 table_id, uint32 prefix_len, uint8 *data, uint8 *asso_data)
{
    int res = BCM_E_NONE;
    struct kbp_db       *db_p = NULL;
    struct kbp_ad_db    *ad_db_p = NULL;
    struct kbp_entry    *db_entry = NULL;
    struct kbp_ad       *ad_entry = NULL;


    res = KBP_INFO.db_info.db_p.get(unit, table_id, &db_p);
    if(res) {
        printf("sw_state db_p get failed\n");
        return BCM_E_FAIL;
    }

    res = KBP_INFO.db_info.ad_db_p.get(unit, table_id, &ad_db_p);
    if(res) {
        printf("sw_state ad_db_p get failed\n");
        return BCM_E_FAIL;
    }

    kbp_db_get_prefix_handle(db_p, data, prefix_len, &db_entry);

    if(db_entry == NULL) { /* create */

        if(kbp_db_add_prefix(db_p, data, prefix_len, &db_entry) != KBP_OK) {
            printf("kbp_db_add_prefix failed\n");
            return BCM_E_FAIL;
        }

        if(kbp_ad_db_add_entry(ad_db_p, asso_data, &ad_entry) != KBP_OK) {
            kbp_db_delete_entry(db_p, db_entry);
            printf("kbp_ad_db_add_entry failed\n");
            return BCM_E_FAIL;
        }

        if(kbp_entry_add_ad(db_p, db_entry, ad_entry) != KBP_OK) {
            kbp_db_delete_entry(db_p, db_entry);
            kbp_ad_db_delete_entry(ad_db_p, ad_entry);
            printf("kbp_entry_add_ad failed\n");
            return BCM_E_FAIL;
        }

    } else { /* update */
        if (kbp_entry_get_ad(db_p, db_entry, &ad_entry) != KBP_OK) {
            printf("kbp_entry_get_ad failed\n");
            return BCM_E_FAIL;
        }
        if (kbp_ad_db_update_entry(ad_db_p, ad_entry, asso_data) != KBP_OK) {
            printf("kbp_ad_db_update_entry failed\n");
            return BCM_E_FAIL;
        }
    }

    if(kbp_db_install(db_p) != KBP_OK) {
        if (kbp_db_delete_entry(db_p, db_entry) != KBP_OK) {
            printf("kbp_db_delete_entry failed after kbp_db_install() failed\n");
            return BCM_E_FAIL;
        }
        if (kbp_ad_db_delete_entry(ad_db_p, ad_entry) != KBP_OK) {
            printf("kbp_ad_db_delete_entry failed after kbp_ad_db_delete_entry() failed\n");
            return BCM_E_FAIL;
        }
        printf("kbp_db_install failed\n");
        return BCM_E_FAIL;
    }

    return BCM_E_NONE;
}

STATIC int test_kbp_entry_get(int unit, uint32 table_id, uint32 prefix_len, uint8 *data, uint8 *asso_data, uint8 *found)
{
    int res = BCM_E_NONE;
    struct kbp_db       *db_p = NULL;
    struct kbp_ad_db    *ad_db_p = NULL;
    struct kbp_entry    *db_entry = NULL;
    struct kbp_ad       *ad_entry = NULL;
    uint32 table_asso_width;


    *found = FALSE;

    res = KBP_INFO.db_info.db_p.get(unit, table_id, &db_p);
    if(res) {
        printf("sw_state db_p get failed\n");
        return BCM_E_FAIL;
    }

    res = KBP_INFO.db_info.ad_db_p.get(unit, table_id, &ad_db_p);
    if(res) {
        printf("sw_state ad_db_p get failed\n");
        return BCM_E_FAIL;
    }

    res = KBP_INFO.db_info.table_asso_width.get(unit, table_id, &table_asso_width);
    if(res) {
        printf("sw_state table_asso_width get failed\n");
        return BCM_E_FAIL;
    }

    kbp_db_get_prefix_handle(db_p, data, prefix_len, &db_entry);
    if(db_entry == NULL) {
        return BCM_E_NONE;
    }

    if(kbp_entry_get_ad(db_p, db_entry, &ad_entry) != KBP_OK) {
        printf("kbp_entry_get_ad failed\n");
        return BCM_E_FAIL;
    } else if(ad_entry == NULL) {
        printf("Entry not found, ad_entry\n");
        return BCM_E_NONE;
    }

    sal_memset(asso_data, 0x0, (table_asso_width / 8));

    if(kbp_ad_db_get(ad_db_p, ad_entry, asso_data) != KBP_OK) {
        printf("kbp_ad_db_get failed\n");
        return BCM_E_FAIL;
    }

    *found = TRUE;
    return BCM_E_NONE;
}

STATIC int test_kbp_entry_remove(int unit, uint32 table_id, uint32 prefix_len, uint8 *data)
{
    int res = BCM_E_NONE;
    struct kbp_db       *db_p = NULL;
    struct kbp_ad_db    *ad_db_p = NULL;
    struct kbp_entry    *db_entry = NULL;
    struct kbp_ad       *ad_entry = NULL;


    res = KBP_INFO.db_info.db_p.get(unit, table_id, &db_p);
    if(res) {
        printf("sw_state db_p get failed\n");
        return BCM_E_FAIL;
    }

    res = KBP_INFO.db_info.ad_db_p.get(unit, table_id, &ad_db_p);
    if(res) {
        printf("sw_state ad_db_p get failed\n");
        return BCM_E_FAIL;
    }

    if(kbp_db_get_prefix_handle(db_p, data, prefix_len, &db_entry) != KBP_OK) {
        printf("kbp_db_get_prefix_handle failed\n");
        return BCM_E_FAIL;
    } else if(db_entry == NULL) {
        printf("Entry not found, db_entry\n");
        return BCM_E_FAIL;
    }

    if(kbp_entry_get_ad(db_p, db_entry, &ad_entry) != KBP_OK) {
        printf("kbp_entry_get_ad failed\n");
        return BCM_E_FAIL;
    } else if (ad_entry == NULL) {
        printf("Entry not found, ad_entry\n");
        return BCM_E_FAIL;
    }

    if(kbp_db_delete_entry(db_p, db_entry) != KBP_OK) {
        printf("kbp_db_delete_entry failed\n");
        return BCM_E_FAIL;
    }

    if(kbp_ad_db_delete_entry(ad_db_p, ad_entry) != KBP_OK) {
        printf("kbp_ad_db_delete_entry failed\n");
        return BCM_E_FAIL;
    }

    if(kbp_db_install(db_p) != KBP_OK) {
        printf("kbp_db_install failed\n");
        return BCM_E_FAIL;
    }

    return BCM_E_NONE;
}

STATIC cmd_result_t
cmd_dpp_kbp_test_ipv6_group_source_inversion(int unit, args_t *a)
{
    int    i;
    uint32 table_id = 5; /* ARAD_KBP_FRWRD_TBL_ID_IPV6_MC */
    uint32 prefix_len = 272; /* example prefix; VRF, SOURCE and GROUP are valid */
    uint8  vrf[2] = { 0x00, 0x64 };
    uint8  source[16] = { 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55 };
    uint8  group[16]  = { 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd };
    uint8  data[40];
    uint8  asso_data[8];
    uint8  asso_data_expected[8];
    uint8  found;


    if(!ARAD_KBP_IPV6MC_INVERT_GROUP_SOURCE) {
        printf("IPv6 MC group and source are not inverted. Test will not run.\n");
        return BCM_E_UNAVAIL;
    }

    sal_memset(data, 0, 40);
    sal_memset(asso_data, 0xa5, 8);
    sal_memset(asso_data_expected, 0xa5, 8);

    /* build data */
    sal_memcpy(data, vrf, 2);
    sal_memcpy(data + 2, source, 16);
    sal_memcpy(data + 2 + 16, group, 16);

    if(test_kbp_entry_add(unit, table_id, prefix_len, data, asso_data)) {
        printf("IPv6 MC entry add failed\n");
        return BCM_E_FAIL;
    }
    if(test_kbp_entry_get(unit, table_id, prefix_len, data, asso_data, &found)) {
        printf("IPv6 MC entry get after add failed\n");
        return BCM_E_FAIL;
    }
    if(!found) {
        printf("IPv6 MC entry expected to be found, but it is not\n");
        return BCM_E_FAIL;
    }
    for(i = 0; i < 8; i++) {
        if(asso_data[i] != asso_data_expected[i]) {
            printf("IPv6 MC entry asso_data mismatch\n");
            printf("Expected ");
            for(i = 0; i < 8; i++) {
                printf("%02x", asso_data_expected[i]);
            }
            printf(" | Actual ");
            for(i = 0; i < 8; i++) {
                printf("%02x", asso_data[i]);
            }
            printf("\n");
            return BCM_E_FAIL;
        }
    }
    if(test_kbp_entry_remove(unit, table_id, prefix_len, data)) {
        printf("IPv6 MC entry remove failed\n");
        return BCM_E_FAIL;
    }
    if(test_kbp_entry_get(unit, table_id, prefix_len, data, asso_data, &found)) {
        printf("IPv6 MC entry get after remove failed\n");
        return BCM_E_FAIL;
    }
    if(found) {
        printf("IPv6 MC entry expected not to be found, but it is\n");
        return BCM_E_FAIL;
    }

    printf("Test pass\n");
    return CMD_OK;
}

STATIC cmd_result_t
cmd_dpp_kbp_test_ipv4_random(int unit, args_t *a)
{
    int             i,j;
    int             entry_num = 64*1024;
    int             entry_num_print_mod = 8*1024;
    int             cached_entries_num = 0;
    parse_table_t   pt; 
    bcm_l3_route_t  routeInfo; 
    bcm_ip_t        dip;
    bcm_vrf_t       vrf = 0;
    bcm_if_t        fec_idx = 2000;
    int             prefix_len = 32;
    int             do_warmboot = 0;
    int             do_update = 1;
    int             do_delete = 1;
    bcm_error_t     rv = BCM_E_NONE;
    int             cache_value = 0;

    if (ARG_CNT(a) > 0) {
        parse_table_init(0,&pt);

        parse_table_add(&pt,"EntryNum"          ,PQ_INT , (void *) (64*1024)            , &entry_num            , NULL);
        parse_table_add(&pt,"EntryNumPrintMod"  ,PQ_INT , (void *) (8*1024)             , &entry_num_print_mod  , NULL);
        parse_table_add(&pt,"CachedEntNum"      ,PQ_INT , (void *) (0)                  , &cached_entries_num   , NULL);
        parse_table_add(&pt,"WarmBoot"          ,PQ_INT , (void *) (0)                  , &do_warmboot          , NULL);
        parse_table_add(&pt,"Update"            ,PQ_INT , (void *) (1)                  , &do_update            , NULL);
        parse_table_add(&pt,"Delete"            ,PQ_INT , (void *) (1)                  , &do_delete            , NULL);

        if (parse_arg_eq(a, &pt) < 0) {
            cli_out("%s: Invalid option: %s\n", ARG_CMD(a), ARG_CUR(a));
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }
        parse_arg_eq_done(&pt);
    }
    
    cli_out("EntryNum=0x%x, EntryNumPrintMod=0x%x, CachedEntNum=0x%x\n", 
            entry_num, entry_num_print_mod, cached_entries_num);

    soc_sand_ll_timer_clear();

    if (cached_entries_num > 0) {
        if (SOC_IS_JERICHO_PLUS(unit) && !SOC_DPP_IS_ELK_ENABLE(unit)) {
            rv = bcm_switch_control_get(unit, bcmSwitchL3RouteCache, &cache_value);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_switch_control_get Failed for bcmSwitchL3RouteCache, rv=0x%x\n", rv);
                return CMD_FAIL;
            }
            if (cache_value == 0) {
                rv = bcm_switch_control_set(unit, bcmSwitchL3RouteCache, cached_entries_num);
                if (rv != BCM_E_NONE) {
                    printf("Error, bcm_switch_control_set Failed for bcmSwitchL3RouteCache, rv=0x%x\n", rv);
                    return CMD_FAIL;
                }
            }
        } else {
             rv = bcm_switch_control_set(unit, bcmSwitchFieldCache, 1);
             if (rv != BCM_E_NONE) {
                 printf("Error, bcm_switch_control_set Failed for bcmSwitchFieldCache, rv=0x%x\n", rv);
                 return CMD_FAIL;
             }
        }
    }

    cli_out("step1: Add routes\n");
    for (i = 0; i < entry_num/entry_num_print_mod; i++)
    {
        soc_sand_ll_timer_set("Full", 1);
        for(j=0; j<entry_num_print_mod; j++)
        {
            bcm_l3_route_t_init(&routeInfo);
            dip = (1<<(32-prefix_len))*((j+i*entry_num_print_mod)/15+1);
            routeInfo.l3a_vrf = vrf;
            routeInfo.l3a_intf = fec_idx;
            routeInfo.l3a_subnet = dip;
            routeInfo.l3a_ip_mask = 0xffffffff<<(32-prefix_len);
            rv = bcm_l3_route_add(unit,&routeInfo);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_l3_route_add Failed, routeInfo.l3a_subnet=0x%x, rv=0x%x\n", routeInfo.l3a_subnet, rv);
                return CMD_FAIL;
            }

            prefix_len--;
            if (prefix_len <= 17)
            {
                prefix_len = 32;
            }
        }

        if ((SOC_IS_JERICHO(unit) && !SOC_DPP_IS_ELK_ENABLE(unit))) {
            bcm_switch_control_set(unit, bcmSwitchL3RouteCommit, 1);
        }

        soc_sand_ll_timer_stop_all();
        soc_sand_ll_timer_print_all();
        soc_sand_ll_timer_clear();
    }

    if ((SOC_IS_JERICHO(unit) && !SOC_DPP_IS_ELK_ENABLE(unit))) {
        bcm_switch_control_set(unit, bcmSwitchL3RouteCommit, 1);
        if ((cache_value == 0) && (cached_entries_num > 0)) {
            /*deinit the caching mechanism*/
            bcm_switch_control_set(unit, bcmSwitchL3RouteCache, 0);
        }
    }

    if (do_warmboot) {
        sh_process_command(unit, "tr 141 warmboot=1");
    }

    if (do_update) {
        cli_out("step2: Update routes\n");
        fec_idx = 3000;
        prefix_len = 32;
        for (i = 0; i < entry_num/entry_num_print_mod; i++)
        {
            soc_sand_ll_timer_set("Full", 1);
            for(j=0; j<entry_num_print_mod; j++)
            {
                bcm_l3_route_t_init(&routeInfo);
                dip = (1<<(32-prefix_len))*((j+i*entry_num_print_mod)/15+1);
                routeInfo.l3a_vrf = vrf;
                routeInfo.l3a_intf = fec_idx;
                routeInfo.l3a_subnet = dip;
                routeInfo.l3a_ip_mask = 0xffffffff<<(32-prefix_len);
                routeInfo.l3a_flags |= BCM_L3_REPLACE;
                rv = bcm_l3_route_add(unit,&routeInfo);
                if (rv != BCM_E_NONE) {
                    printf("Error, bcm_l3_route_add Failed, routeInfo.l3a_subnet=0x%x, rv=0x%x\n", routeInfo.l3a_subnet, rv);
                    return CMD_FAIL;
                }

                prefix_len--;
                if (prefix_len <= 17)
                {
                    prefix_len = 32;
                }
            }
            soc_sand_ll_timer_stop_all();
            soc_sand_ll_timer_print_all();
            soc_sand_ll_timer_clear();
        }
    }

    if ((SOC_IS_JERICHO(unit) && !SOC_DPP_IS_ELK_ENABLE(unit))) {
        bcm_switch_control_set(unit, bcmSwitchL3RouteCommit, 1);
    }

    if (do_warmboot) {
        sh_process_command(unit, "tr 141 warmboot=1");
    }

    if (do_delete) {
        cli_out("step3: Delete routes\n");
        prefix_len = 32;
        for (i = 0; i < entry_num/entry_num_print_mod; i++)
        {
            soc_sand_ll_timer_set("Full", 1);
            for(j=0; j<entry_num_print_mod; j++)
            {
                bcm_l3_route_t_init(&routeInfo);
                dip = (1<<(32-prefix_len))*((j+i*entry_num_print_mod)/15+1);
                routeInfo.l3a_vrf = vrf;
                routeInfo.l3a_subnet = dip;
                routeInfo.l3a_ip_mask = 0xffffffff<<(32-prefix_len);
                rv = bcm_l3_route_delete(unit,&routeInfo);
                if (rv != BCM_E_NONE) {
                    printf("Error, bcm_l3_route_delete Failed, routeInfo.l3a_subnet=0x%x, rv=0x%x\n", routeInfo.l3a_subnet, rv);
                    return CMD_FAIL;
                }

                prefix_len--;
                if (prefix_len <= 17)
                {
                    prefix_len = 32;
                }
            }
            soc_sand_ll_timer_stop_all();
            soc_sand_ll_timer_print_all();
            soc_sand_ll_timer_clear();
        }
    }

    if ((SOC_IS_JERICHO(unit) && !SOC_DPP_IS_ELK_ENABLE(unit))) {
        bcm_switch_control_set(unit, bcmSwitchL3RouteCommit, 1);
    }

    return CMD_OK;
}

STATIC cmd_result_t
cmd_dpp_kbp_test_ipv4_dc_lem(int unit, args_t *a)
{
    uint32	i,j;
    uint32  kbp_entry_num = 2097152; /* 2M */
	uint32  lem_entry_num = 768000; /* 768K */

	parse_table_t   pt;
	bcm_error_t     rv   = BCM_E_NONE;

	int host     = 0x0a00ff01;
	int l3_intr  = 0x20001000;
	bcm_vrf_t vrf  = 0;
    bcm_mac_t next_hop_mac  = {0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc};
	int iter = 1;

	bcm_l3_host_t l3host;

    if (ARG_CNT(a) > 0) {
        parse_table_init(0,&pt);

        parse_table_add(&pt,"KbpEntryNum"       ,PQ_INT , (void *) (2097152)            , &kbp_entry_num        , NULL);
		parse_table_add(&pt,"LemEntryNum"       ,PQ_INT , (void *) (768000)             , &lem_entry_num        , NULL);
        parse_table_add(&pt,"VRF" 			    ,PQ_INT , (void *) (17)                 , &vrf   				, NULL);
        parse_table_add(&pt,"host" 			    ,PQ_INT , (void *) (0x0a00ff01)         , &host   				, NULL);
		parse_table_add(&pt,"iter" 			    ,PQ_INT , (void *) (1)         			, &iter   				, NULL);

		if (parse_arg_eq(a, &pt) < 0) {
            cli_out("%s: Invalid option: %s\n", ARG_CMD(a), ARG_CUR(a));
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }
        parse_arg_eq_done(&pt);
    }
    cli_out("KbpEntryNum=%d, LemEntryNum=%d, vrf=%d host=0x%08x, iterations=%d\n", kbp_entry_num, lem_entry_num, vrf, host, iter);
	    

	for (j=0;j<iter;j++) {
		bcm_l3_host_t_init(&l3host);
		l3host.l3a_ip_addr = host;
		l3host.l3a_vrf = vrf&0xFF;
		l3host.l3a_port_tgid = 201; /* egress port to send packet to */
		l3host.l3a_intf = l3_intr;
		l3host.l3a_flags2 = BCM_L3_FLAGS2_SCALE_ROUTE;
		sal_memcpy(l3host.l3a_nexthop_mac, next_hop_mac, 6); /* next hop mac attached directly */

		cli_out("Iteration #%d\n", j);
		cli_out("step1: Add to KBP DC\n");
		soc_sand_ll_timer_clear();
		soc_sand_ll_timer_set("DC - 65K entires", 1);

		for (i = 1; i <= kbp_entry_num; i++)
		{
			rv = bcm_l3_host_add(unit, &l3host);
			if (rv != BCM_E_NONE) {
				printf("Error, bcm_l3_host_add Failed, l3host.l3a_ip_addr=0x%x, rv=0x%x\n", l3host.l3a_ip_addr, rv);
				return CMD_FAIL;
			}
			if ((i&0xFFFF) == 0) {
				soc_sand_ll_timer_stop_all();
				soc_sand_ll_timer_print_all();
				printf("entry_id=%d, l3host.l3a_ip_addr=0x%08x\n", i, l3host.l3a_ip_addr);
				if (i == 1048576) {
					l3host.l3a_intf++;
				}
				soc_sand_ll_timer_clear();
				soc_sand_ll_timer_set("DC - 65K entires", 1);
			}
			l3host.l3a_ip_addr++;
		}

		soc_sand_ll_timer_stop_all();
		soc_sand_ll_timer_print_all();
		printf("entry_id=%d, l3host.l3a_ip_addr=0x%08x\n", i, l3host.l3a_ip_addr);
		soc_sand_ll_timer_clear();
		soc_sand_ll_timer_set("LEM - 65K entires", 1);

		cli_out("step2: Add to LEM\n");

		bcm_l3_host_t_init(&l3host);
		l3host.l3a_ip_addr = host;
		l3host.l3a_vrf = vrf&0xFF;
		l3host.l3a_port_tgid = 201; /* egress port to send packet to */
		l3host.l3a_intf = l3_intr+2;
		l3host.l3a_flags = BCM_L3_HOST_LOCAL;

		sal_memcpy(l3host.l3a_nexthop_mac, next_hop_mac, 6); /* next hop mac attached directly */

		for (i = 1; i <= lem_entry_num; i++)
		{
			rv = bcm_l3_host_add(unit, &l3host);
			if (rv != BCM_E_NONE) {
				printf("Error, bcm_l3_host_add Failed, l3host.l3a_ip_addr=0x%x, rv=0x%x\n", l3host.l3a_ip_addr, rv);
				return CMD_FAIL;
			}
			if ((i&0xFFFF) == 0) {
				soc_sand_ll_timer_stop_all();
				soc_sand_ll_timer_print_all();
				printf("entry_id=%d, l3host.l3a_ip_addr=0x%08x\n", i, l3host.l3a_ip_addr);
				soc_sand_ll_timer_clear();
				soc_sand_ll_timer_set("LEM - 65K entires", 1);
			}
			l3host.l3a_ip_addr++;
		}

		soc_sand_ll_timer_stop_all();
		soc_sand_ll_timer_print_all();

		if (j != (iter-1)) {

			printf("delete KBP entries\n");
			soc_sand_ll_timer_clear();
			soc_sand_ll_timer_set("KBP - delete 2M entries", 1);
			bcm_l3_host_t_init(&l3host);
			l3host.l3a_flags2 = BCM_L3_FLAGS2_SCALE_ROUTE;
			l3host.l3a_ip_addr = host;
			l3host.l3a_vrf = vrf&0xFF;
			for (i = 1; i <= kbp_entry_num; i++)
			{
				rv = bcm_l3_host_delete(unit, &l3host);
					if (rv != BCM_E_NONE) {
						printf("Error, bcm_l3_host_delete Failed, l3host.l3a_ip_addr=0x%x, rv=0x%x\n", l3host.l3a_ip_addr, rv);
						return CMD_FAIL;
					}
					l3host.l3a_ip_addr++;
			}
			soc_sand_ll_timer_stop_all();
			soc_sand_ll_timer_print_all();

			printf("delete LEM entries\n");
			soc_sand_ll_timer_clear();
			soc_sand_ll_timer_set("LEM - delete 750K entries", 1);
			bcm_l3_host_t_init(&l3host);
			l3host.l3a_ip_addr = host;
			l3host.l3a_vrf = vrf&0xFF;
			l3host.l3a_flags = BCM_L3_HOST_LOCAL;
			for (i = 1; i <= lem_entry_num; i++){
				rv = bcm_l3_host_delete(unit, &l3host);
				if (rv != BCM_E_NONE) {
					printf("Error, bcm_l3_host_delete Failed, l3host.l3a_ip_addr=0x%x, rv=0x%x\n", l3host.l3a_ip_addr, rv);
					return CMD_FAIL;
				}
				l3host.l3a_ip_addr++;
			}
			soc_sand_ll_timer_stop_all();
			soc_sand_ll_timer_print_all();
		}
	}
    return CMD_OK;
}

STATIC cmd_result_t
cmd_dpp_kbp_test_ip_uc_scale_capacity(int unit, args_t *a)
{
    int             i, j;
    int             route_num = 64*1024;
    int             lem_route_num = 64*1024;
    parse_table_t   pt;
    bcm_l3_route_t  routeInfo;
    bcm_l3_host_t   hostInfo;
    bcm_ip_t        dip;
    bcm_if_t        fec_idx = 2000;
    int             prefix_len = 32;
    int             ipv6 = 0;
    bcm_error_t     rv = BCM_E_NONE;

    if (ARG_CNT(a) > 0) {
        parse_table_init(0,&pt);
        parse_table_add(&pt,"RouteNum"          ,PQ_INT , (void *) (64*1024)            , &route_num            , NULL);
        parse_table_add(&pt,"LEMRouteNum"       ,PQ_INT , (void *) (64*1024)            , &lem_route_num        , NULL);
        parse_table_add(&pt,"IPv6"              ,PQ_INT , (void *) (0)                  , &ipv6                 , NULL);

        if (parse_arg_eq(a, &pt) < 0) {
            cli_out("%s: Invalid option: %s\n", ARG_CMD(a), ARG_CUR(a));
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }
        parse_arg_eq_done(&pt);
    }

    if (((SOC_DPP_CONFIG(unit)->pp.enhanced_fib_scale_prefix_length) == 0) && (SOC_DPP_CONFIG(unit)->pp.enhanced_fib_scale_prefix_length_ipv6_long == 0)) {
        cli_out("This capacity test is meant to be used with enhanced_fib_scale_prefix_length soc_property.");
        return CMD_USAGE;
    }

#ifdef BCM_88675_A0
    cli_out("Adding and searching KAPS routes.\n");
    rv = jer_pp_kaps_search_test(unit, 1/*add_entries*/, 1/*search_entries*/, 0/*delete_entries*/, 1/*loop_test*/, 0/*cache_test*/);
    if (rv != BCM_E_NONE) {
        printf("Error, jer_pp_kaps_search_test Failed, rv=0x%x\n", rv);
        return CMD_FAIL;
    }
#endif /* BCM_88675_A0 */

    cli_out("Adding %d routes (50/50 hosts/routes) to the LEM and %d routes to the KAPS.\n", lem_route_num, route_num);

    cli_out("step1: Add KAPS routes\n");
    soc_sand_ll_timer_set("Add KAPS Routes", 1);
    for(j=0; j<route_num; j++)
    {
        bcm_l3_route_t_init(&routeInfo);
        dip = (1<<(32-prefix_len))*(j/15+1);
        routeInfo.l3a_flags2 = BCM_L3_FLAGS2_SCALE_ROUTE;
        routeInfo.l3a_vrf = j % 0x4000;
        routeInfo.l3a_intf = fec_idx;
        if (ipv6) {
            routeInfo.l3a_flags = BCM_L3_IP6;
            for (i=0; i<16; i++) {
                routeInfo.l3a_ip6_net[i] = (dip >> ((i%4)*8)) & 0xFF;
            }
            bcm_ip6_mask_create(routeInfo.l3a_ip6_mask, prefix_len);
        } else {
            routeInfo.l3a_subnet = dip;
            routeInfo.l3a_ip_mask = 0xffffffff<<(32-prefix_len);
        }
        rv = bcm_l3_route_add(unit,&routeInfo);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_l3_route_add Failed, routeInfo.l3a_subnet=0x%x, rv=0x%x\n", routeInfo.l3a_subnet, rv);
            return CMD_FAIL;
        }

        prefix_len--;
        if (ipv6) {
            if ((prefix_len <= 20) && (prefix_len < SOC_DPP_CONFIG(unit)->pp.enhanced_fib_scale_prefix_length_ipv6_short))
            {
                prefix_len = 128;
            }
            if (prefix_len == SOC_DPP_CONFIG(unit)->pp.enhanced_fib_scale_prefix_length_ipv6_long) {
                prefix_len = SOC_DPP_CONFIG(unit)->pp.enhanced_fib_scale_prefix_length_ipv6_short;
            }
        } else {
            if (prefix_len <= 17)
            {
                prefix_len = 32;
            }
        }
    }
    soc_sand_ll_timer_stop_all();
    soc_sand_ll_timer_print_all();
    soc_sand_ll_timer_clear();

#ifdef BCM_88675_A0
    cli_out("Searching KAPS routes.\n");
    rv = jer_pp_kaps_search_test(unit, 1/*add_entries*/, 1/*search_entries*/, 0/*delete_entries*/, 1/*loop_test*/, 0/*cache_test*/);
    if (rv != BCM_E_NONE) {
        printf("Error, jer_pp_kaps_search_test Failed, rv=0x%x\n", rv);
        return CMD_FAIL;
    }
#endif /* BCM_88675_A0 */

    prefix_len = 24;
    cli_out("step2: Add LEM routes\n");
    soc_sand_ll_timer_set("Add LEM Routes", 2);
    for(j=0; j<lem_route_num; j++)
    {
        bcm_l3_host_t_init(&hostInfo);
        dip = (1<<(32-prefix_len))*(j/15+1);
        hostInfo.l3a_flags2 = j % 2 ? BCM_L3_FLAGS2_SCALE_ROUTE : 0;
        hostInfo.l3a_vrf = j % 0x4000;
        hostInfo.l3a_intf = fec_idx;
        if (ipv6) {
            dip = j;
            hostInfo.l3a_flags = BCM_L3_IP6;
            for (i=0; i<16; i++) {
                hostInfo.l3a_ip6_addr[i] = (dip >> ((i%4)*8)) & 0xFF;
            }
        } else {
            hostInfo.l3a_ip_addr = dip;
        }
        rv = bcm_l3_host_add(unit,&hostInfo);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_l3_host_add Failed, hostInfo.l3a_ip_addr=0x%x, rv=0x%x\n", hostInfo.l3a_ip_addr, rv);
            return CMD_FAIL;
        }
    }
    soc_sand_ll_timer_stop_all();
    soc_sand_ll_timer_print_all();
    soc_sand_ll_timer_clear();

    fec_idx = 3000;
    prefix_len = 32;

    cli_out("step3: Update KAPS routes\n");
    soc_sand_ll_timer_set("Update KAPS Routes", 3);
    for(j=0; j<route_num; j++)
    {
        bcm_l3_route_t_init(&routeInfo);
        dip = (1<<(32-prefix_len))*(j/15+1);
        routeInfo.l3a_flags2 = BCM_L3_FLAGS2_SCALE_ROUTE;
        routeInfo.l3a_vrf = j % 0x4000;
        routeInfo.l3a_intf = fec_idx;
        if (ipv6) {
            routeInfo.l3a_flags = BCM_L3_IP6;
            for (i=0; i<16; i++) {
                routeInfo.l3a_ip6_net[i] = (dip >> ((i%4)*8)) & 0xFF;
            }
            bcm_ip6_mask_create(routeInfo.l3a_ip6_mask, prefix_len);
        } else {
            routeInfo.l3a_subnet = dip;
            routeInfo.l3a_ip_mask = 0xffffffff<<(32-prefix_len);
        }
        rv = bcm_l3_route_add(unit,&routeInfo);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_l3_route_add Failed, routeInfo.l3a_subnet=0x%x, rv=0x%x\n", routeInfo.l3a_subnet, rv);
            return CMD_FAIL;
        }

        prefix_len--;
        if (ipv6) {
            if ((prefix_len <= 20) && (prefix_len < SOC_DPP_CONFIG(unit)->pp.enhanced_fib_scale_prefix_length_ipv6_short))
            {
                prefix_len = 128;
            }
            if (prefix_len == SOC_DPP_CONFIG(unit)->pp.enhanced_fib_scale_prefix_length_ipv6_long) {
                prefix_len = SOC_DPP_CONFIG(unit)->pp.enhanced_fib_scale_prefix_length_ipv6_short;
            }
        } else {
            if (prefix_len <= 17)
            {
                prefix_len = 32;
            }
        }
    }
    soc_sand_ll_timer_stop_all();
    soc_sand_ll_timer_print_all();
    soc_sand_ll_timer_clear();

    prefix_len = 24;
    cli_out("step4: Update LEM routes\n");
    soc_sand_ll_timer_set("Update LEM Routes", 4);
    for(j=0; j<lem_route_num; j++)
    {
        bcm_l3_host_t_init(&hostInfo);
        dip = (1<<(32-prefix_len))*(j/15+1);
        hostInfo.l3a_flags2 = j % 2 ? BCM_L3_FLAGS2_SCALE_ROUTE : 0;
        hostInfo.l3a_vrf = j % 0x4000;
        hostInfo.l3a_intf = fec_idx;
        if (ipv6) {
            dip = j;
            hostInfo.l3a_flags = BCM_L3_IP6;
            for (i=0; i<16; i++) {
                hostInfo.l3a_ip6_addr[i] = (dip >> ((i%4)*8)) & 0xFF;
            }
        } else {
            hostInfo.l3a_ip_addr = dip;
        }
        rv = bcm_l3_host_add(unit,&hostInfo);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_l3_host_add Failed, hostInfo.l3a_ip_addr=0x%x, rv=0x%x\n", hostInfo.l3a_ip_addr, rv);
            return CMD_FAIL;
        }
    }
    soc_sand_ll_timer_stop_all();
    soc_sand_ll_timer_print_all();
    soc_sand_ll_timer_clear();

#ifdef BCM_88675_A0
    cli_out("Searching and deleting KAPS routes.\n");
    rv = jer_pp_kaps_search_test(unit, 1/*add_entries*/, 1/*search_entries*/, 1/*delete_entries*/, 1/*loop_test*/, 0/*cache_test*/);
    if (rv != BCM_E_NONE) {
        printf("Error, jer_pp_kaps_search_test Failed, rv=0x%x\n", rv);
        return CMD_FAIL;
    }

    cli_out("Adding and searching KAPS routes.\n");
    rv = jer_pp_kaps_search_test(unit, 1/*add_entries*/, 1/*search_entries*/, 0/*delete_entries*/, 1/*loop_test*/, 0/*cache_test*/);
    if (rv != BCM_E_NONE) {
        printf("Error, jer_pp_kaps_search_test Failed, rv=0x%x\n", rv);
        return CMD_FAIL;
    }
#endif /* BCM_88675_A0 */

    prefix_len = 32;
    cli_out("step5: Delete KAPS routes\n");
    soc_sand_ll_timer_set("Delete KAPS Routes", 5);
    for(j=0; j<route_num; j++)
    {
        bcm_l3_route_t_init(&routeInfo);
        dip = (1<<(32-prefix_len))*(j/15+1);
        routeInfo.l3a_flags2 = BCM_L3_FLAGS2_SCALE_ROUTE;
        routeInfo.l3a_vrf = j % 0x4000;
        if (ipv6) {
            routeInfo.l3a_flags = BCM_L3_IP6;
            for (i=0; i<16; i++) {
                routeInfo.l3a_ip6_net[i] = (dip >> ((i%4)*8)) & 0xFF;
            }
            bcm_ip6_mask_create(routeInfo.l3a_ip6_mask, prefix_len);
        } else {
            routeInfo.l3a_subnet = dip;
            routeInfo.l3a_ip_mask = 0xffffffff<<(32-prefix_len);
        }
        rv = bcm_l3_route_delete(unit,&routeInfo);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_l3_route_add Failed, routeInfo.l3a_subnet=0x%x, rv=0x%x\n", routeInfo.l3a_subnet, rv);
            return CMD_FAIL;
        }

        prefix_len--;
        if (ipv6) {
            if ((prefix_len <= 20) && (prefix_len < SOC_DPP_CONFIG(unit)->pp.enhanced_fib_scale_prefix_length_ipv6_short))
            {
                prefix_len = 128;
            }
            if (prefix_len == SOC_DPP_CONFIG(unit)->pp.enhanced_fib_scale_prefix_length_ipv6_long) {
                prefix_len = SOC_DPP_CONFIG(unit)->pp.enhanced_fib_scale_prefix_length_ipv6_short;
            }
        } else {
            if (prefix_len <= 17)
            {
                prefix_len = 32;
            }
        }
    }
    soc_sand_ll_timer_stop_all();
    soc_sand_ll_timer_print_all();
    soc_sand_ll_timer_clear();

    prefix_len = 24;
    cli_out("step6: Delete LEM routes\n");
    soc_sand_ll_timer_set("Delete LEM Routes", 6);
    for(j=0; j<lem_route_num; j++)
    {
        bcm_l3_host_t_init(&hostInfo);
        dip = (1<<(32-prefix_len))*(j/15+1);
        hostInfo.l3a_flags2 = j % 2 ? BCM_L3_FLAGS2_SCALE_ROUTE : 0;
        hostInfo.l3a_vrf = j % 0x4000;
        if (ipv6) {
            dip = j;
            hostInfo.l3a_flags = BCM_L3_IP6;
            for (i=0; i<16; i++) {
                hostInfo.l3a_ip6_addr[i] = (dip >> ((i%4)*8)) & 0xFF;
            }
        } else {
            hostInfo.l3a_ip_addr = dip;
        }
        rv = bcm_l3_host_delete(unit,&hostInfo);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_l3_host_delete Failed, hostInfo.l3a_ip_addr=0x%x, rv=0x%x\n", hostInfo.l3a_ip_addr, rv);
            return CMD_FAIL;
        }
    }
    soc_sand_ll_timer_stop_all();
    soc_sand_ll_timer_print_all();
    soc_sand_ll_timer_clear();

#ifdef BCM_88675_A0
    cli_out("Searching and deleting KAPS routes.\n");
    rv = jer_pp_kaps_search_test(unit, 1/*add_entries*/, 1/*search_entries*/, 1/*delete_entries*/, 1/*loop_test*/, 0/*cache_test*/);
    if (rv != BCM_E_NONE) {
        printf("Error, jer_pp_kaps_search_test Failed, rv=0x%x\n", rv);
        return CMD_FAIL;
    }
#endif /* BCM_88675_A0 */

    return CMD_OK;
}

char cmd_dpp_kbp_usage[] =
    "Usage:\n"
    "\tkbp [options]\n" 
    "Usage options:\n"
    "\tcpu_record_send <LSBenable> <OPcode> <MSB> <LSB>- \n"
    "\t\tMSB LSB - msb lsb data, 0 by default, add index: MSB0, MSB1 ... MSB15\n"
    "\t\tOPcode - 251 (PIORDY), 252 (PIORDX), 253 (PIOWR), 254 (RD_LUT), 255 (WR_LUT)\n "
    "\tcpu_lookup_reply - arad_kbp_cpu_read\n"
    "\trop_write <core><ADdr> <DAta> <Mask> <ADdrShort> <ValidBit> <WriteMode> - write to rop.\n"
    "\t\tADdr - Address of location where data should be written. includes all :vbit, wrmode ...\n"
    "\t\tDAta Mask - data & mask , 0 by default\n"
    "\t\tADdrShort - Address of location where data should be written.\n"
    "\t\tValidBit - The valid bit which indicates if the database entry should be enabled or disabled\n"
    "\t\tWriteMode - needs to be 0 for DATABASE_DM or 1 for DATABASE_XY \n"
    "\trop_read <core><ADdr> <ValidBit> <TadaType> - \n"
    "\t\tADdr ValidBit see rop_write, TadaType - 0 for X and 1 for Y.\n"
    "\trop_counter\n"
    "\t\t       Print number of ROP packets/records.\n"
    "\tinit_appl <ENable> <TcamDevType> <Ip4UcFwdTableSize>\n"
    "\tdeinit_appl\n"
    "\t\tENable - If TRUE, the ELK feature is active.\n"
    "\t\tTcamDevType - Indicate the External lookup Device type\n"
    "\t\tIp4UcFwdTableSize - IPv4 unicast forward table size. if 0x0 forwarding done in internal DB, else done External\n"
    "\tkbp print <File=FILE_NAME>\n"
    "\t\tFile - If specified, print KBP device configuration to file (HTML format - supported in chrome and firefox).\n"
    "\t\t       Otherwise print KBP software configuration of DBs and Instructions (available before and after calling kbp init_appl).\n"
    "\tkbp print master\n"
    "\t\t       Print the diagnostics of the master key.\n"
    "\ttest_ip4_rpf_appl <NumEntries> <RecordBaseTbl> <ADValTbl0>\n"
    "\tkbp dump_xml <FullDump=0> <Filename=./kbp_dump.xml> <KAPS=0> - default: Prints to console for external KBP\n"
    "\tinit_kbp_interface <mdio_id> - Init KBP interface (DCR, ILKN...)\n"
    "\t\tmdio_id - KBP mdio ID format. Default for Negev is 0x101\n"
    "\t\t          bits [5:6,8:9] - bus ID.\n"
    "\t\t          bit [7] - Internal select. Set to 0 for external phy access.\n"
    "\t\t          bits [0:4] - phy/kbp id.\n"
    "\tinit_arad_interface - Init Arad interface towards the KBP (EGW)\n"
    "\tmdio_read_16 mdio_id=0x163 device_id=0x00 addr=0x004C\n"
    "\tmdio_read_64 mdio_id=0x163 device_id=0x00 addr=0x004C\n"
    "\top_status_registers mdio_id=0x163 num_lanes=12 <File=filename>\n"
    "\tsdk_ver - prints the KBP lib version\n"
    "\tkaps_arm <File=filename> <LoadFile=0/1> <CpuHalt=0/1> - Load KAPS ARM FW file to memory and run it.\n"
    "\t\tFile     - ARM FW load file name.\n"
    "\t\tLoadFile - If clear, ARM FW file will not be loaded. Default: 1.\n"
    "\t\tCpuHalt  - If set, ARM CPU will stay in Halt. Default: 0.\n"
#if defined(BCM_JERICHO_SUPPORT)
    "\tkbp kaps_show <DBAL_TBL_ID> <CountOnly=1> - Prints the table contents in 0xData/valid_num_of_bits/max_num_of_bits format. \n"
    "\tkbp kaps_db_stats <DBAL_TBL_ID> - Prints kaps table stats \n"
    "\tkbp kaps_hitbit <table=DBAL_TBL_NAME> <enable=0/1> - Enable hitbit bookkeeping per DBAL KAPS table. Note it also clears the table. \n"
    "\tkbp kaps_search <IPV4_UC=1>/<IPV4_MC=1> <vrf=3> <dip=1.2.3.4> <sip=1.2.5.5> <mc_group=0xE0E0E001> <inrif=2> \n"
    "\tkbp kaps_search <IPV6_UC=1>/<IPV6_MC=1> <vrf=3> <inrif=2> <sip6=0100:1600:3500:6400:0000:0000:0000:0000> <dip6=0100:1600:5500:7800:0000:0000:0000:0000> \n"
    "\t\t\t<mc_group6=ff1e:0d0c:0b0a:0908:0706:0504:0302:0100> \n"
    "\tkbp kaps_arm_thread <enable_dma_thread=1> <print_status=1> - Enables and disables the KAPS DMA thread. Also prints thread related information.\n"
    "\tkbp TestAddMeasureRate \n"
    "\tkbp TestAclAddMeasureRate \n"
    "\tIn order to allow insertion rate for KBP, uncomment the flag ARAD_PP_KBP_TIME_MEASUREMENTS under sand_low_level.h\n"
    "\tAnd compile again.\n"
#endif /*defined(BCM_JERICHO_SUPPORT)*/
    "Examples:\n"
    "\tkbp cpu_record_send OPcode=255 MSB5=1 LSBenable=0\n"
    "\tkbp cpu_lookup_reply\n"
    "\tkbp rop_read addr=0x00000102\n"
    "\tkbp rop_write addr=0x0000010b ADdrShort=0x00000000 data=0x0000000000000000ffff\n"
    "\tkbp init_kbp_interface mdio_id=257 ilkn_rev=1\n"
    "\tkbp test_ip4_rpf_appl NumEntries=2048 RecordBaseTbl0=0x55551234 RecordBaseTbl1=0xeeee1234 ADValTbl0=0xdead0620 ADValTbl1=0xbeaf8321\n"
    "\tkbp init_appl enable=1 tcamdevtype=1 Ip4UcFwdTableSize=8192 Ip4McFwdTableSize=8192\n"
    "\tkbp kaps_arm file=opsw_test_secded.hex00\n"
    "\tkbp dma_init\n"
    "\tkbp dma_enable\n"
    "\tkbp dma_config\n"
    "\tkbp dma_test\n"
    "\tkbp dma_show\n"
    "\tkbp kaps_hitbit table=\"FLP IPv4 UC KAPS\" enable=1\n"
    ;

STATIC cmd_result_t
cmd_read_mdio(int unit, int count, args_t *a)
{
    int i;
    uint16_t data[4] = {0, 0, 0, 0};
    int mdio_id, device_id, addr;
    ARAD_NIF_ELK_USER_DATA handle;

    mdio_id = 0x163;
    device_id = addr = 0;
    if (ARG_CNT(a) > 0) {
        parse_table_t pt;
        parse_table_init(0, &pt);
        parse_table_add(&pt, "mdio_id", PQ_HEX, (void *) (0x0), &mdio_id, NULL);
        parse_table_add(&pt, "device_id", PQ_HEX, (void *) (0x0), &device_id, NULL);
        parse_table_add(&pt, "addr", PQ_HEX, (void *) (0x0), &addr, NULL);

        if (parse_arg_eq(a, &pt) < 0) {
            cli_out("%s: Invalid option: %s\n", ARG_CMD(a), ARG_CUR(a));
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }
        parse_arg_eq_done(&pt);
    }

    handle.device = unit;
    handle.kbp_mdio_id = mdio_id;

    for (i = count; i > 0; i--) {
        arad_kbp_callback_mdio_read(&handle, 0, device_id, addr + i - 1, &data[i - 1]);
    }

    if (count == 1) {
        cli_out("Unit %d MDIO Read 16-bit KBP MDIO ID %x Device ID %01x Addr %02x Data %02x\n",
                unit, mdio_id, device_id, addr, data[0]);
    } else {
        cli_out("Unit %d MDIO Read 64-bit KBP MDIO ID %x Device ID %01x Addr %02x Data %02x%02x%02x%02x\n",
                unit, mdio_id, device_id, addr, data[3], data[2], data[1], data[0]);
    }

    return CMD_OK;
}

STATIC cmd_result_t
cmd_write_mdio(int unit, int count, args_t *a)
{
    uint32_t data_32[2]={0};
    uint32_t data = 0;
    int mdio_id, device_id, addr;
    ARAD_NIF_ELK_USER_DATA handle;
    uint16_t value_temp;

    mdio_id = 0x163;
    device_id = addr = 0;
    if (ARG_CNT(a) > 0) {
        parse_table_t pt;
        parse_table_init(0, &pt);
        parse_table_add(&pt, "mdio_id", PQ_HEX, (void *) (0x0), &mdio_id, NULL);
        parse_table_add(&pt, "device_id", PQ_HEX, (void *) (0x0), &device_id, NULL);
        parse_table_add(&pt, "addr", PQ_HEX, (void *) (0x0), &addr, NULL);

        if (count == 1) {
            parse_table_add(&pt, "data", PQ_HEX, (void *) (0x0), &data, NULL);
        } else {
            parse_table_add(&pt, "data_64", PQ_HEX | PQ_INT64, (void *) (0x0), &data_32, NULL);
        }

        if (parse_arg_eq(a, &pt) < 0) {
            cli_out("%s: Invalid option: %s\n",
                    ARG_CMD(a), ARG_CUR(a));
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }
        parse_arg_eq_done(&pt);
    }

    handle.device = unit;
    handle.kbp_mdio_id = mdio_id;

    if (count == 1) {
        arad_kbp_callback_mdio_write(&handle, 0, device_id, addr, data);
    } else {
        arad_kbp_callback_mdio_write(&handle, 0, device_id, addr, data_32[1] & 0xFFFF);
        arad_kbp_callback_mdio_write(&handle, 0, device_id, addr + 1, (data_32[1] >> 16) & 0xFFFF);
        arad_kbp_callback_mdio_write(&handle, 0, device_id, addr + 2, (data_32[0]) & 0xFFFF);
        arad_kbp_callback_mdio_write(&handle, 0, device_id, addr + 3, (data_32[0] >> 16) & 0xFFFF);
    }
    /** dummy read to avoid OP MDIO lock-up */
    arad_kbp_callback_mdio_read(&handle, 0, 0, 0, &value_temp);

    if (count == 1) {
        cli_out("Unit %d MDIO Write 16-bit KBP MDIO ID %x Device ID %01x Addr %02x Data %02x\n",
                unit, mdio_id, device_id, addr, data);
    } else {
        cli_out("Unit %d MDIO Write 64-bit KBP MDIO ID %x Device ID %01x Addr %02x Data %08x %08x\n",
                unit, mdio_id, device_id, addr, data_32[1], data_32[0]);
    }

    return CMD_OK;
}

STATIC cmd_result_t
cmd_prbs(int unit, args_t *a)

{
    int kbp_mdio_id, enable, prbs_poly, prbs_print, num_lanes;
    ARAD_NIF_ELK_USER_DATA handle;
    struct kbp_device_config kbp_config = KBP_DEVICE_CONFIG_DEFAULT;
    uint32_t flags = KBP_DEVICE_DEFAULT;
    ARAD_INIT_ELK *elk = &SOC_DPP_CONFIG(unit)->arad->init.elk;

    kbp_mdio_id = prbs_print = 0;
    enable = 1;
    prbs_poly = KBP_PRBS_31;
    num_lanes = 12;

    if (ARG_CNT(a) > 0) {
        parse_table_t pt;
        parse_table_init(0, &pt);
        parse_table_add(&pt, "mdio_id", PQ_INT , (void *) INT_TO_PTR(JER_KBP_OP_APPL_MDIO_DEFAULT_ID), &kbp_mdio_id, NULL);
        parse_table_add(&pt, "enable", PQ_INT , (void *) INT_TO_PTR(1), &enable, NULL);
        parse_table_add(&pt, "prbs_poly", PQ_INT , (void *) INT_TO_PTR(KBP_PRBS_31), &prbs_poly, NULL);
        parse_table_add(&pt, "prbs_print", PQ_INT , (void *) INT_TO_PTR(0), &prbs_print, NULL);
        parse_table_add(&pt, "num_lanes", PQ_INT , (void *) INT_TO_PTR(12), &num_lanes, NULL);
        if (parse_arg_eq(a, &pt) < 0) {
            cli_out("%s: Invalid option: %s\n",
                    ARG_CMD(a), ARG_CUR(a));
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }
        parse_arg_eq_done(&pt);
    }

    if (!ARAD_KBP_IS_OP_OR_OP2) {
        cli_out("Only support Optimus Prime!\n");
        return CMD_USAGE;
    }

    /* Search for the core ID using the MDIO ID */

    handle.device = unit;
    handle.kbp_mdio_id = kbp_mdio_id;
    kbp_config.handle = &handle;

    /*callbacks set*/
    kbp_config.mdio_read = arad_kbp_callback_mdio_read;
    kbp_config.mdio_write = arad_kbp_callback_mdio_write;
    kbp_config.port_map[0].start_lane = elk->kbp_start_lane[0];
    kbp_config.port_map[0].num_lanes = num_lanes;
    kbp_config.port_map[1].start_lane = elk->kbp_start_lane[1];
    kbp_config.port_map[1].num_lanes = num_lanes;
    kbp_config.usleep = arad_kbp_usleep_callback;

    if (!ARAD_KBP_OP_IS_DUAL_MODE) {
        flags |= KBP_DEVICE_DUAL_PORT;
    } else {
        kbp_config.port_map[2].start_lane = elk->kbp_start_lane[2];
        kbp_config.port_map[2].num_lanes = num_lanes;
        kbp_config.port_map[3].start_lane = elk->kbp_start_lane[3];
        kbp_config.port_map[3].num_lanes = num_lanes;
        flags |= KBP_DEVICE_QUAD_PORT;
    }

    if (prbs_print) {
        kbp_device_interface_prbs_print(KBP_DEVICE_OP, flags, &kbp_config);
        return CMD_OK;
    }

    if (enable) {
        cli_out("Enabling PRBS MDIO ID 0x%x, PRBS poly %d\n", kbp_mdio_id, prbs_poly);
        kbp_device_interface_prbs(KBP_DEVICE_OP, flags, &kbp_config, prbs_poly, 1);
        cli_out("Clearing PRBS counters, ignore\n");
        kbp_device_interface_prbs_print(KBP_DEVICE_OP, flags, &kbp_config);
        return CMD_OK;
    } else {
        cli_out("Disabling PRBS\n");
        kbp_device_interface_prbs(KBP_DEVICE_OP, flags, &kbp_config, prbs_poly, 0);
    }
    return CMD_OK;
}

STATIC cmd_result_t
cmd_dump_status(int unit, args_t *a)
{
    int mdio_id, num_lanes;
    ARAD_NIF_ELK_USER_DATA handle;
    struct kbp_device_config kbp_config = KBP_DEVICE_CONFIG_DEFAULT;
    uint32_t flags = KBP_DEVICE_DEFAULT;
    ARAD_INIT_ELK *elk = &SOC_DPP_CONFIG(unit)->arad->init.elk;
    char *file = NULL;
    FILE * fp = NULL;
    int rv = CMD_OK;
    mdio_id = 0x163;
    num_lanes = 12;

    if (ARG_CNT(a) > 0) {
        parse_table_t pt;
        parse_table_init(0, &pt);
        parse_table_add(&pt, "File", PQ_DFL|PQ_STRING, (void *)0, &file, NULL);
        parse_table_add(&pt, "mdio_id", PQ_HEX, (void *) INT_TO_PTR(mdio_id), &mdio_id, NULL);
        parse_table_add(&pt, "num_lanes", PQ_INT, (void *) INT_TO_PTR(num_lanes), &num_lanes, NULL);

        if (parse_arg_eq(a, &pt) < 0) {
            cli_out("%s: Invalid option: %s\n",
                    ARG_CMD(a), ARG_CUR(a));
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }
        if (file != NULL) {
            /* Open File */
            fp = sal_fopen(file, "w+");
            if (fp == NULL) {
                LOG_ERROR(BSL_LS_APPL_SHELL, (BSL_META_U(unit, "Error: Unable to open file: %s\n"), file));
                parse_arg_eq_done(&pt);
                return CMD_FAIL;
            }
        }
        parse_arg_eq_done(&pt);
    }

    if (fp == NULL) {
        if (bslfile_is_enabled() && bslfile_fp()) {
            fp = bslfile_fp();
        } else {
            fp = stdout;
        }
    }

    handle.device = unit;
    handle.kbp_mdio_id = mdio_id;
    kbp_config.handle = &handle;

    /*callbacks set*/
    kbp_config.mdio_read = arad_kbp_callback_mdio_read;
    kbp_config.mdio_write = arad_kbp_callback_mdio_write;
    kbp_config.port_map[0].start_lane = elk->kbp_start_lane[0];
    kbp_config.port_map[0].num_lanes = num_lanes;
    kbp_config.port_map[1].start_lane = elk->kbp_start_lane[1];
    kbp_config.port_map[1].num_lanes = num_lanes;
    kbp_config.usleep = arad_kbp_usleep_callback;

    if (elk->kbp_connect_mode == ARAD_NIF_ELK_TCAM_CONNECT_MODE_SINGLE) {
        flags |= KBP_DEVICE_DUAL_PORT;
    } else {
        kbp_config.port_map[2].start_lane = elk->kbp_start_lane[2];
        kbp_config.port_map[2].num_lanes = num_lanes;
        kbp_config.port_map[3].start_lane = elk->kbp_start_lane[3];
        kbp_config.port_map[3].num_lanes = num_lanes;
        flags |= KBP_DEVICE_QUAD_PORT;
    }

    kbp_device_interface_print_regs(KBP_DEVICE_OP, flags, &kbp_config, fp);

    if(fp != stdout && fp != bslfile_fp()) {
        rv = sal_fclose(fp);
        if (rv != SOC_SAND_OK) {
            LOG_ERROR(BSL_LS_APPL_SHELL, (BSL_META_U(unit, "Error: Failed to sal_fclose()\n")));
            return CMD_FAIL;
        }
        cli_out("Print op status registers to file[OK]!\n");
    }

    return CMD_OK;
}

STATIC cmd_result_t
cmd_dump_eyescan(int unit, args_t *a)
{
    kbp_status status;
    int kbp_mdio_id, num_lanes;
    ARAD_NIF_ELK_USER_DATA handle;
    struct kbp_device_config kbp_config = KBP_DEVICE_CONFIG_DEFAULT;
    uint32_t flags = KBP_DEVICE_DEFAULT;
    ARAD_INIT_ELK *elk = &SOC_DPP_CONFIG(unit)->arad->init.elk;

    kbp_mdio_id = 0;
    num_lanes = 12;

    if (ARG_CNT(a) > 0) {
        parse_table_t pt;
        parse_table_init(0, &pt);
        parse_table_add(&pt, "mdio_id",PQ_INT , (void *) INT_TO_PTR(JER_KBP_OP_APPL_MDIO_DEFAULT_ID), &kbp_mdio_id, NULL);
        parse_table_add(&pt, "num_lanes",PQ_INT , (void *) INT_TO_PTR(12), &num_lanes, NULL);

        if (parse_arg_eq(a, &pt) < 0) {
            cli_out("%s: Invalid option: %s\n",
                    ARG_CMD(a), ARG_CUR(a));
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }
        parse_arg_eq_done(&pt);
    }

    if (!ARAD_KBP_IS_OP_OR_OP2) {
        cli_out("Only support Optimus Prime!\n");
        return CMD_USAGE;
    }

    handle.device = unit;
    handle.kbp_mdio_id = kbp_mdio_id;
    kbp_config.handle = &handle;

    /*callbacks set*/
    kbp_config.mdio_read = arad_kbp_callback_mdio_read;
    kbp_config.mdio_write = arad_kbp_callback_mdio_write;
    kbp_config.port_map[0].start_lane = elk->kbp_start_lane[0];
    kbp_config.port_map[0].num_lanes = num_lanes;
    kbp_config.port_map[1].start_lane = elk->kbp_start_lane[1];
    kbp_config.port_map[1].num_lanes = num_lanes;
    kbp_config.usleep = arad_kbp_usleep_callback;

    if (!ARAD_KBP_OP_IS_DUAL_MODE) {
        flags |= KBP_DEVICE_DUAL_PORT;
    } else {
        kbp_config.port_map[2].start_lane = elk->kbp_start_lane[2];
        kbp_config.port_map[2].num_lanes = num_lanes;
        kbp_config.port_map[3].start_lane = elk->kbp_start_lane[3];
        kbp_config.port_map[3].num_lanes = num_lanes;
        flags |= KBP_DEVICE_QUAD_PORT;
    }

    status = kbp_device_interface_eyescan_print(KBP_DEVICE_OP, flags, &kbp_config);
    if (status != KBP_OK) {
        LOG_ERROR(BSL_LS_APPL_SHELL, (BSL_META_U(unit, "Error: kbp_device_interface_eyescan_print API fail: %s\n"),
                                                 kbp_get_status_string(status)));
    }
    return CMD_OK;
}

STATIC cmd_result_t cmd_dpp_kbp_dump_xml(int unit, args_t *a)
{
    char *filename = NULL;
    int fulldump = 0;
    int kaps_dump = 0;
    int rv = CMD_OK;
    FILE *kbp_fp;
    parse_table_t pt;

    if (ARG_CNT(a) > 0) {
        parse_table_init(0, &pt);
        parse_table_add(&pt, "FullDump",PQ_INT , (void *)0, &fulldump, NULL);
        parse_table_add(&pt, "KAPS",PQ_INT , (void *)0, &kaps_dump, NULL);
        parse_table_add(&pt, "Filename", PQ_STRING, (void *)("./kbp_dump.xml"), &filename, NULL);

        if (parse_arg_eq(a, &pt) < 0) {
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }
        if (ARG_CNT(a) > 0) {
            cli_out("%s: Error: Unknown argument %s\n", ARG_CMD(a), ARG_CUR(a));
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }
        cli_out("@@@@@@ filename is %s\n", filename);
        kbp_fp = sal_fopen(filename,"w");

        if (kbp_fp == NULL) {
            cli_out("Error: Unable to open file: %s\n", filename);
            parse_arg_eq_done(&pt);
            return CMD_FAIL;
        }
    } else {
        kbp_fp = stdout;
    }

    if (kaps_dump == 1) {
#if defined(BCM_JERICHO_SUPPORT)
        if (JerAppData[unit]->kaps_device_p == NULL) {
            cli_out("There is no avaliable KAPS device!\n");
            rv = CMD_OK;
            goto close_file;
        }
        if (kbp_device_dump(JerAppData[unit]->kaps_device_p, fulldump, kbp_fp) < 0) {
            cli_out("Failed to dump KAPS configuration! \n");
            rv = CMD_FAIL;
        }
#else
        cli_out("KAPS is only supported on Jericho and beyond! \n");
#endif
    } else {
        if (AradAppData[unit]->device_p[0] == NULL) {
            cli_out("There is no avaliable KBP device!\n");
            rv = CMD_OK;
            goto close_file;
        }
        if (kbp_device_dump(AradAppData[unit]->device_p[0], fulldump, kbp_fp) < 0) {
            cli_out("Failed to dump kbp configuration! \n");
            rv = CMD_FAIL;
        }
    }
close_file:
    if (kbp_fp != stdout) {
        sal_fclose(kbp_fp);
    }
    return rv;
}

STATIC cmd_result_t cmd_dpp_kbp_read_last_result(int unit, args_t *a)
{
    char *filename = "./kbp_read_last.log";
    FILE *kbp_fp;
    parse_table_t pt;

    if (AradAppData[unit]->device_p[0] == NULL) {
        cli_out("There is no avaliable KBP device!\n");
        return CMD_OK;
    }

    if (ARG_CNT(a) > 0) {
        parse_table_init(0, &pt);
        parse_table_add(&pt, "Filename", PQ_STRING, (void *)0, &filename, NULL);

        if (parse_arg_eq(a, &pt) < 0) {
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }
        if (ARG_CNT(a) > 0) {
            cli_out("%s: Error: Unknown argument %s\n", ARG_CMD(a), ARG_CUR(a));
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }
        parse_arg_eq_done(&pt);
    }
    cli_out("@@@@@@ filename is %s\n", filename);

    kbp_fp = sal_fopen(filename, "w");
    if (kbp_fp == NULL) {
        cli_out("Error: Unable to open file: %s\n", filename);
        return CMD_FAIL;
    }
    if (kbp_device_read_last_result(AradAppData[unit]->device_p[0], 0, kbp_fp) < 0) {
        cli_out("Failed to dump last result! \n");
        sal_fclose(kbp_fp);
        return CMD_FAIL;
    }
    sal_fclose(kbp_fp);

    return CMD_OK;
}

cmd_result_t
cmd_dpp_kbp(int unit, args_t *a)
{
    char *func;
    soc_reg_above_64_val_t data;
    cmd_result_t rv = CMD_OK;
    int i;
    uint32 soc_sand_rv;

    if (!(func = ARG_GET(a))) {    /* Nothing to do    */
        return(CMD_USAGE);      /* Print usage line */
    }

    if(!sal_strcasecmp(func, "cpu_record_send")) {

        return cmd_dpp_kbp_cpu_record_send(unit, a);

    } else if(!sal_strcasecmp(func, "cpu_lookup_reply")) {

        soc_sand_rv = arad_kbp_cpu_lookup_reply(unit, 0, data);
        if(BCM_FAILURE(handle_sand_result(soc_sand_rv))) {
            cli_out("Error: arad_kbp_cpu_lookup_reply(%d, &data)\n", unit);
            return CMD_FAIL;
        }

        cli_out("Reply Data:  ");

        for(i=0; i<11; ++i) {
            cli_out("0x%x ", data[i]);
        }

        cli_out("\n");

    } else if(!sal_strcasecmp(func, "rop_write")) {

       return cmd_dpp_kbp_rop_write(unit, a);

    } else if(!sal_strcasecmp(func, "rop_read")) {

        return cmd_dpp_kbp_rop_read(unit, a);

    } else if(!sal_strcasecmp(func, "rop_counter")) {

        return cmd_dpp_kbp_rop_counter(unit);

    } else if(!sal_strcasecmp(func, "rop_test")) {

        return cmd_dpp_kbp_rop_test(unit, a);

    } else if (!sal_strcasecmp(func, "dma_init")) {

        dpp_kbp_rop_dma_init_appl(unit);

    } else if (!sal_strcasecmp(func, "dma_enable")) {

        dpp_kbp_rop_dma_enable(unit);

    } else if (!sal_strcasecmp(func, "dma_test")) {

        dpp_kbp_dma_test(unit, a);

    } else if (!sal_strcasecmp(func, "dma_config")) {

        dpp_kbp_dma_configure(unit, a);

    } else if (!sal_strcasecmp(func, "dma_show")) {

        dpp_kbp_rop_dma_show_stat(unit);

    } else if (!sal_strcasecmp(func, "reset_device")) {

        return cmd_dpp_kbp_reset_device(unit, a);

    } else if(!sal_strcasecmp(func, "init_appl")) {

        return cmd_dpp_kbp_init_appl(unit, a);
      
    } else if(!sal_strcasecmp(func, "deinit_appl")) {

        return cmd_dpp_kbp_deinit_appl(unit, a);
      
    } else if (!sal_strcasecmp(func, "init_kbp_interface")) {

        return cmd_dpp_kbp_init_kbp_interface(unit, a);

    } else if (!sal_strcasecmp(func, "init_arad_interface")) {
        soc_sand_rv = arad_kbp_init_arad_interface(unit);
        if(BCM_FAILURE(handle_sand_result(soc_sand_rv))) {
            cli_out("Error: arad_kbp_init_arad_interface(%d)\n", unit);
            return CMD_FAIL;
        }
    } else if(!sal_strcasecmp(func, "test_ip4_rpf_appl")) {

        return cmd_dpp_kbp_test_ip4_rpf_appl(unit, a);        
		
    } else if (!sal_strcasecmp(func, "TestAddMeasureRate")) {

		int rate = cmd_dpp_kbp_test_add_measure_rate(unit, a);
        if(rate < KBP_IPV4_INSERTION_RATE_LIMIT)
		{
			printf("\nRate is lower than expected\nRate = %d, Expected limit = %d\n\n",rate,KBP_IPV4_INSERTION_RATE_LIMIT);
			return CMD_FAIL;
		}
		return CMD_OK;

    } else if (!sal_strcasecmp(func, "TestDirectCallAddMeasureRate")) {

		int rate = cmd_dpp_kbp_test_add_measure_rate_direct_call(unit, a);
        if(rate < KBP_ACL_INSERTION_RATE_LIMIT)
		{
			printf("\nRate is lower than expected\nRate = %d, Expected limit = %d\n\n",rate,KBP_ACL_INSERTION_RATE_LIMIT);
			return CMD_FAIL;
		}
		return CMD_OK;

    } else if (!sal_strcasecmp(func, "TestAclAddMeasureRate")) {

		int rate = cmd_dpp_kbp_test_acl_add_measure_rate(unit, a);
        if(rate < KBP_ACL_INSERTION_RATE_LIMIT)
		{
			printf("\nRate is lower than expected\nRate = %d, Expected limit = %d\n\n",rate,KBP_ACL_INSERTION_RATE_LIMIT);
			return CMD_FAIL;
		}
		return CMD_OK;

    } else if (!sal_strcasecmp(func, "test_ipv4_measure_rate_random")) {

		int rate = cmd_dpp_kbp_test_ipv4_random_measure_rate(unit, a);
        if(rate < KBP_IPV4_INSERTION_RATE_LIMIT)
		{
			printf("\nRate is lower than expected\nRate = %d, Expected limit = %d\n\n",rate,KBP_ACL_INSERTION_RATE_LIMIT);
			return CMD_FAIL;
		}
		return CMD_OK;
    } else if(!sal_strcasecmp(func, "dump_xml")) {

        return cmd_dpp_kbp_dump_xml(unit, a);

    } else if(!sal_strcasecmp(func, "dump_last")) {

        return cmd_dpp_kbp_read_last_result(unit, a);

    } else if (!sal_strcasecmp(func, "TestAddRate")) {

        return cmd_dpp_kbp_test_add_rate(unit, a);

    } else if (!sal_strcasecmp(func, "TestIPv6Performance")) {

        return cmd_dpp_kbp_test_ipv6_performance_measure(unit, a);

    } else if (!sal_strcasecmp(func, "TestIPv6GroupSourceInvertion")) {

        return cmd_dpp_kbp_test_ipv6_group_source_inversion(unit, a);

    } else if (!sal_strcasecmp(func, "TestAclAddRate")) {

        return cmd_dpp_kbp_test_acl_add_rate(unit, a);

    } else if (!sal_strcasecmp(func, "test_ipv4_random")) {

        return cmd_dpp_kbp_test_ipv4_random(unit, a);

    } else if (!sal_strcasecmp(func, "test_ipv4_dc_lem")) {

        return cmd_dpp_kbp_test_ipv4_dc_lem(unit, a);

    } else if (!sal_strcasecmp(func, "test_ip_scale_capacity")) {

        return cmd_dpp_kbp_test_ip_uc_scale_capacity(unit, a);

    } else if(!sal_strcasecmp(func, "sdk_ver")) {

            return cmd_dpp_sdk_ver(unit, a);

    } else if(!sal_strcasecmp(func, "flexible_mode_ref_app")) {

            return cmd_dpp_flexible_mode_ref_app(unit, a);

    } else if(!sal_strcasecmp(func, "mdio_read_16")) {
        return cmd_read_mdio(unit, 1, a);
    } else if(!sal_strcasecmp(func, "mdio_read_64")) {
        return cmd_read_mdio(unit, 4, a);
    } else if(!sal_strcasecmp(func, "mdio_write_16")) {
        return cmd_write_mdio(unit, 1, a);
    } else if(!sal_strcasecmp(func, "mdio_write_64")) {
        return cmd_write_mdio(unit, 4, a);
    } else if(!sal_strcasecmp(func, "op_status_registers")) {
        return cmd_dump_status(unit, a);
    } else if (!sal_strcasecmp(func, "op_prbs")) {
        return cmd_prbs(unit, a);
    } else if(!sal_strcasecmp(func, "op_eyescan_print")) {
        return cmd_dump_eyescan(unit, a);
    } else if(!sal_strcasecmp(func, "kbp_search")) {
        return cmd_dpp_kbp_search(unit, a);
    } else if (JER_KAPS_ENABLE(unit)) {
        if(!sal_strcasecmp(func, "kaps_init")) {
            return cmd_dpp_kbp_kaps_init(unit, a);
        }
        else if(!sal_strcasecmp(func, "kaps_deinit")) {
            return cmd_dpp_kaps_deinit(unit, a);
        }
#if defined(BCM_88675_A0)
        else if(!sal_strcasecmp(func, "kaps_search")) {
            return cmd_dpp_kaps_search(unit, a); 
        }
        else if(!sal_strcasecmp(func, "kaps_show")) {
            return cmd_dpp_kaps_show(unit, a);
        }
        else if(!sal_strcasecmp(func, "kaps_db_stats")) {
            return cmd_dpp_kaps_db_stats(unit, a);
        }
        else if(!sal_strcasecmp(func, "kaps_arm_thread")) {
            return cmd_dpp_kaps_arm_thread(unit, a);
        }
        else if(!sal_strcasecmp(func, "kaps_arm")) {
            return cmd_dpp_kaps_arm_image_load(unit, a);
        }
#endif

#ifdef KBP_SW_TEST
        else if(!sal_strcasecmp(func, "v1_test")) {
            return cmd_kbp_v1(unit, a);
        }
/*************
        else if(!sal_strcasecmp(func, "custom_rate")) {

            return cmd_kbp_custom_rate(unit, a);

        }
*/
#endif /* KBP_SW_TEST */

        else {

            return(CMD_USAGE);      /* Print usage line */

        }
    } else {

        return(CMD_USAGE);      /* Print usage line */
    }

    return rv;

}

#endif /* #ifdef INCLUDE_KBP */

