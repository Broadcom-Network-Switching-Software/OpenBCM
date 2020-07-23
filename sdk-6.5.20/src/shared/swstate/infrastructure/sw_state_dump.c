/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * INFO: this module implement a dump functionality for the SW State infrastructure layer,
 */
 
#include <shared/bsl.h>
#include <soc/types.h>
#include <shared/swstate/sw_state_dump.h>
#include <shared/swstate/sw_state.h>
#include <shared/swstate/diagnostic/sw_state_diagnostic.h>

#ifdef BCM_WARM_BOOT_API_TEST
#include <bcm_int/dpp/counters.h>
#include <soc/dpp/SAND/Utils/sand_hashtable.h>
#include <soc/dpp/SAND/Utils/sand_sorted_list.h>
#endif

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif
#define _ERR_MSG_MODULE_NAME BSL_LS_SHARED_SWSTATE

#ifdef BCM_WARM_BOOT_API_TEST

typedef long long unsigned int LLU;

char *sw_state_stride_skip_vars[2] = {"sw_state.dpp.soc.arad.pp.kaps_db", "sw_state.dpp.soc.arad.tm.kbp_info"};

sw_state_dump_t sw_state_stride_dump[BCM_MAX_NUM_UNITS];

char sw_state_stride_dump_file_name[BCM_MAX_NUM_UNITS][SW_STATE_STRIDE_DUMP_FILE_NAME_LEN];

FILE *sw_state_stride_dump_output_file[BCM_MAX_NUM_UNITS];


/* return in nof_elements the number of allocated elements pointed by ptr */
int sw_state_get_nof_elements(int unit, void *ptr, uint32 size, uint32* from, uint32 *nof_elements) {
    SOC_INIT_FUNC_DEFS;
    if (ptr == NULL) {
        *nof_elements = 0;
        *from = 0;
        sw_state_stride_dump[unit].skip_once ++;
    }
    else {
        shr_sw_state_allocated_size_get(unit, (uint8*)ptr, nof_elements);
        *nof_elements /= size;
        *from = shr_sw_state_dump_check_all_the_same (unit, ptr, size, *nof_elements) ? (*nof_elements) - 1 : 0;
    }
    SOC_EXIT;
exit:
    SOC_FUNC_RETURN;
}

/* print str followed by n bytes of memory pointed by vp */
void shr_sw_state_print_mem(int unit, void const *vp, uint32 n, char* str)
{
    unsigned char const *p = vp;
    uint32 i;
    char str_to_print[SW_STATE_STRIDE_DUMP_MAX_STRING_LEN] = {0};
    char str_tmp[SW_STATE_STRIDE_DUMP_HEX_LEN] = {0};

    sprintf(str_to_print, str);
    for (i=0; i<n; i++) {
        sprintf(str_tmp,"%02x", p[i]);    
        sal_strcat(str_to_print, str_tmp);
    }
    sal_strcat(str_to_print,"\n");
    shr_sw_state_dump_print(unit, str_to_print);
    return;
}

/* update the stride counting. if the difference between the data in index i-1 to the data in index i equals to the difference
   between the data in index i to the data in index i+1, then the we increase same_diff_cnt.
   If same_diff_cnt reach threshold SW_STATE_DUMP_STRIDE_THRESH then we conclude a stride */
   
void shr_sw_state_dump_update_stride(int unit, uint64 value, uint32 size, char* str) {

    uint32 last_idx       = sw_state_stride_dump[unit].last_idx;
    uint32 curr_idx       = sw_state_stride_dump[unit].curr_idx;
    uint32 *cnt_idx       = &sw_state_stride_dump[unit].cnt_idx;
    uint64 *last_diff     = sw_state_stride_dump[unit].last_diff;
    uint64 *last_val      = sw_state_stride_dump[unit].last_val;
    uint32 *last_size     = sw_state_stride_dump[unit].last_size;
    uint64 *same_diff_cnt = sw_state_stride_dump[unit].same_diff_cnt;
    uint8  *is_stride     = &sw_state_stride_dump[unit].is_stride;

    /* cnt_idx represents the number of reenterants of the same index.
     * for example:
     * a[1].x = 0   -   cnt_idx = 0
     * a[1].y = 0   -   cnt_idx = 1
     * a[1].z = 0   -   cnt_idx = 2
     * a[2].x = 0   -   cnt_idx = 0
     */

    /* update the cnt_idx */

    if (sw_state_stride_dump[unit].skip_once) return;
    if (last_idx != curr_idx) *cnt_idx = 0, sw_state_stride_dump[unit].nof_iterations ++;
    else (*cnt_idx) ++;

    if (last_size[0] == 0) sw_state_stride_dump[unit].nof_iterations = 0, *cnt_idx = 0;

    if (sw_state_stride_dump[unit].nof_iterations > 2) {
        if (value - last_val[*cnt_idx] == last_diff[*cnt_idx]) {
            same_diff_cnt[*cnt_idx]++;
        } else { /* broken stride */
            if (*is_stride) shr_sw_state_dump_broken_stride(unit);
            sal_memset (same_diff_cnt, 0, SW_STATE_DUMP_STRIDE_MAX * sizeof(uint64));
        }
        *is_stride = (same_diff_cnt[0] > SW_STATE_DUMP_STRIDE_THRESH);
    }

    sw_state_stride_dump[unit].last_idx = curr_idx;
    last_diff[*cnt_idx] = value - last_val[*cnt_idx];
    last_val[*cnt_idx] = value;
    last_size[*cnt_idx] = size;
    sal_strcpy(sw_state_stride_dump[unit].last_string[*cnt_idx], str);
}

/* broken stride - a stride caused by break in the middle of a loop 
 * for example: data[0] = 0, data[1] = 1, ..., data[100] = 100, data[101] = 200, data [102] = 201
 *                                                                /
 *                                                       broken stride
 */
void shr_sw_state_dump_broken_stride(int unit) {
    if (sw_state_stride_dump[unit].is_stride) {
        shr_sw_state_dump_print_stride(unit, TRUE);
    }
    sal_memset (&sw_state_stride_dump[unit], 0, sizeof(sw_state_stride_dump[unit]));
}

/* end of stride - natural end of a loop
 * for example: data[0] = 0, data[1] = 1, ..., data[100] = 100,    new_data[0] = 0
 *                                                             /
 *                                                      end of stride (of data)
 */
void shr_sw_state_dump_end_of_stride(int unit) {
    if (!sw_state_stride_dump[unit].skip_once) {
        if (sw_state_stride_dump[unit].is_stride) {
            shr_sw_state_dump_print_stride(unit, FALSE);
        }
        sal_memset (&sw_state_stride_dump[unit], 0, sizeof(sw_state_stride_dump[unit]));
    }
    if (sw_state_stride_dump[unit].skip_once > 0)
        sw_state_stride_dump[unit].skip_once --;
}

/* check if all of nof_elements pointed by ptr have the same value */
uint8 shr_sw_state_dump_check_all_the_same(int unit, void* ptr, int size, int nof_elements) {
    int j;
    uint8 is_all_zero = TRUE;
    for(j = 0; j < nof_elements * size; j++) {
        if ((*(((uint8*)ptr) + j)) != 0) {
            is_all_zero = FALSE;
            break;
        }
    }
    if (is_all_zero) {
        sw_state_stride_dump[unit].skip_once ++;
        return TRUE;
    }
    for(j = 0; j < nof_elements - 1; j++) {
        if (0 != sal_memcmp((uint8*)(ptr) + j*size, (uint8*)(ptr) + (j+1)*size, size))
            return FALSE;
    }
    return TRUE;
}

/* update the current index of the data being proccessed */
void shr_sw_state_dump_update_current_idx(int unit, int idx) {
    if (!sw_state_stride_dump[unit].skip_once)
        sw_state_stride_dump[unit].curr_idx = idx;
}

/* print the number of indices in the last stride followed by the values of the stride if form of a linear formula */
void shr_sw_state_dump_print_stride(int unit, uint8 is_broken) {
  int nof_members = 0;
  int i;
  int min_same_diff = 0;
  char str_tmp[SW_STATE_STRIDE_DUMP_TEMP_STR_LEN] = {0};
  
  sprintf(str_tmp, "STRIDE X %llu\n", (LLU) (sw_state_stride_dump[unit].same_diff_cnt[0] + 2));
  shr_sw_state_dump_print(unit, str_tmp);

  for (i = 0; i < SW_STATE_DUMP_STRIDE_MAX; i++) {
      if (sw_state_stride_dump[unit].same_diff_cnt[i] > 0) {
          nof_members ++;
          if (sw_state_stride_dump[unit].same_diff_cnt[i] > min_same_diff) min_same_diff = sw_state_stride_dump[unit].same_diff_cnt[i];
      }
  }
  for (i = 0; i < nof_members; i++) {
      char str_to_print[SW_STATE_STRIDE_DUMP_MAX_STRING_LEN] = {0};
      char tmp_string[SW_STATE_STRIDE_DUMP_MAX_STRING_LEN] = {0};
      char value_str[SW_STATE_STRIDE_DUMP_MAX_STRING_LEN] = {0};
      char *open_par  = strrchr(sw_state_stride_dump[unit].last_string[i],'[');
      char *close_par = strrchr(sw_state_stride_dump[unit].last_string[i],']');
      int open_par_idx = open_par - sw_state_stride_dump[unit].last_string[i];
      sal_memcpy(str_to_print, sw_state_stride_dump[unit].last_string[i], open_par_idx + 1);
      sprintf(tmp_string, "t = %u..%u"
          , sw_state_stride_dump[unit].curr_idx - min_same_diff - 1
          , sw_state_stride_dump[unit].curr_idx - 1 + !is_broken);
      strcat(str_to_print, tmp_string);
      strcat(str_to_print, close_par);
      if (sw_state_stride_dump[unit].last_diff[i] == 0) sprintf(value_str, "%llx\n", (LLU) (sw_state_stride_dump[unit].last_val[i]));
      else {
          sprintf(value_str,"%llx + %llx * (t - %u)\n", (LLU) (sw_state_stride_dump[unit].last_val[i] -
              sw_state_stride_dump[unit].last_diff[i] * (sw_state_stride_dump[unit].same_diff_cnt[i] + 1)),
              (LLU) (sw_state_stride_dump[unit].last_diff[i]), sw_state_stride_dump[unit].curr_idx - min_same_diff - 1);
      }
      strcat(str_to_print, value_str);
      shr_sw_state_dump_print(unit, str_to_print);
  }
  for (i = 0; i < nof_members && is_broken && nof_members > 1; i++) {
      char tmp_str[SW_STATE_STRIDE_DUMP_TEMP_STR_LEN] = {0};
      if (sw_state_stride_dump[unit].same_diff_cnt[i] == min_same_diff) {
          shr_sw_state_dump_print(unit, sw_state_stride_dump[unit].last_string[i]);
          sprintf(tmp_str, "%llx\n",(LLU) (sw_state_stride_dump[unit].last_val[i]));
          shr_sw_state_dump_print(unit, tmp_str);
      }
  }

}

/* check if the string should be printed (not contained in the set of variables that defined to be skipped) */

uint8 shr_sw_state_stride_dump_skip(char *format) {
    int i = 0, skip = 0;
    int nof_skip_vars = 0;
    char *curr = format;
    char basic_str[SW_STATE_STRIDE_DUMP_MAX_STRING_LEN] = {0};
    char *basic_str_curr = basic_str;
    /* strip parentheses and replace -> by . */
    do {
        switch(*curr) {
            case '[':
            case ']':
                skip = 1 - skip;
                break;
            case '-':
                break;
            case '>':
                *(basic_str_curr++) = '.';
                break;
            default:
                if (skip == 0) *(basic_str_curr++) = *curr;
                break;
        }
    } while (*(++curr) != '\0');
    nof_skip_vars = sizeof(sw_state_stride_skip_vars) / sizeof(char*);
    for (i = 0; i < nof_skip_vars; ++i) {
        if (strstr(basic_str, sw_state_stride_skip_vars[i]) != NULL) return 1;
    }
    return 0;
}

/* determine the file to print the data to
 * if not called, then STDOUT is the default
 */
void shr_sw_state_dump_attach_file(int unit, char* filename) {
    sal_memset(sw_state_stride_dump_file_name[unit], 0, sizeof(sw_state_stride_dump_file_name[unit])/sizeof(sw_state_stride_dump_file_name[unit][0]));
    sal_strcpy(sw_state_stride_dump_file_name[unit], filename);
    if ((sw_state_stride_dump_output_file[unit] = sal_fopen(sw_state_stride_dump_file_name[unit], "w")) == 0) {
        cli_out("Error opening sw dump file %s\n", sw_state_stride_dump_file_name[unit]);
        return;
    }
}

/* restore the file resources associated with the sw state stride dump functionality */
void shr_sw_state_dump_detach_file(int unit) {
    sal_memset(sw_state_stride_dump_file_name[unit], 0, sizeof(sw_state_stride_dump_file_name[unit])/sizeof(sw_state_stride_dump_file_name[unit][0]));
    sal_fclose(sw_state_stride_dump_output_file[unit]);
    sw_state_stride_dump_output_file[unit] = NULL;
}

/* print the data to the monitor or to file, determined the shr_sw_state_dump_attach_file */
void shr_sw_state_dump_print(int unit, char* str) {
    if (sw_state_stride_dump_output_file[unit]) {
        sal_fprintf(sw_state_stride_dump_output_file[unit],str);
    } else {
        cli_out(str);
    }
}


void shr_sw_state_dump_clear_tmps(int unit) {
    if (bcm_dpp_counter_state_diag_mask(unit) !=0 ){
        cli_out("Error masking out counters state diagnostics\n");
    }
    /*
     * These procedures are for DPP only as they are located in soc/dpp/SAND/Utils
     * Their DNX equivalents are in shared/utilex
     */
    if (handle_sand_result(soc_sand_hash_table_clear_all_tmps(unit)) != 0){
        cli_out("Error masking out sand_hash_tables tmp buffers state\n");
    }
    if (handle_sand_result(soc_sand_sorted_list_clear_all_tmps(unit)) != 0){
        cli_out("Error masking out sand_sorted_lists tmp buffers state\n");
    }
}

/* print all the data of the sw_state to the monitor / to file determined by shr_sw_state_dump_attach_file */
void shr_sw_state_stride_dump(int unit, char* filename) {
    /* masking */
    shr_sw_state_dump_clear_tmps(unit);
    shr_sw_state_dump_attach_file(unit, filename);
    sw_state_diagnostic[unit].dpp.dump(unit);
    shr_sw_state_dump_detach_file(unit);
    /* unmasking */
    if (bcm_dpp_counter_state_diag_unmask(unit) != 0) {
        cli_out("Error unmasking counters state diagnostics\n");
    }
}

#endif /* BCM_WARM_BOOT_API_TEST */
