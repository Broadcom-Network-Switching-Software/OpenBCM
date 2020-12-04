/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


/*
 * cmicx_sim_util.c
 *
 * does definitions for stuff in cmicx_sim_util.h. this includes output logging functions, error logging functions, etc.
 */


/* includes */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#ifndef CMICX_STANDALONE_SIM
 #include <../systems/sim/pcid/cmicx_sim_util.h>
 #include <../systems/sim/pcid/cmicx_sim_schan.h>
 #include <../systems/sim/pcid/cmicsim.h>
 #include <../include/soc/schanmsg.h>

#else
 #include "cmicx_sim_util.h"
#endif


/* static global variables for cmicx sim log functions. file pointer for the log file - since we only need this in this file. */
static FILE *cmicx_log_file;
static int   cmicx_current_log_level;
static int   cmicx_cutoff_log_level;

/* external cmicx global pointer and pcid info */
extern pcid_info_t pcid_info;

/* initialize cmicx logging to a certain file */
bool cmicx_logging_init(char *file_name) {
    cmicx_log_file = fopen(file_name,"a");
    if (cmicx_log_file==NULL) {
        return false;
    }
#ifdef CMICX_STANDALONE_SIM
    cmicx_current_log_level   = 3;
    cmicx_cutoff_log_level    = 3;
#else
    cmicx_current_log_level   = 3;
    cmicx_cutoff_log_level    = 3;
#endif
#if defined(BCM_DNX_SUPPORT) || defined(BCM_DNXF_SUPPORT)
    cmicx_current_log_level = cmicx_cutoff_log_level - 1;
#endif
    return true;
}

/* set the log level for subsequently logged messages */
void cmicx_log_level_set(int level) {
    if(pcid_info.opt_verbose == 1) {
        cmicx_current_log_level = level;
    } else {
        cmicx_current_log_level = -1;
    }
}

/* set the current cut-off point for cmicx logging */
void cmicx_log_level_cutoff_set(int level) {
    cmicx_cutoff_log_level = level;
}

/* logs a specified message to the opened log file (and derivative functions) */
void cmicx_log(char *str) {
    cmicx_log_str(str);
}
void cmicx_log_str(char *str) {
    if (cmicx_current_log_level>=cmicx_cutoff_log_level) {
        printf("%s",str);
        if (cmicx_log_file!=NULL) {
            fprintf(cmicx_log_file,"%s",str);
            fflush(cmicx_log_file);
        }
    }
}
void cmicx_log_addr(addr_t addr) {
    if (cmicx_current_log_level>=cmicx_cutoff_log_level) {
        printf("0x%08X",addr);
        if (cmicx_log_file!=NULL) {
            fprintf(cmicx_log_file,"0x%08X",addr);
            fflush(cmicx_log_file);
        }
    }
}
void cmicx_log_reg(reg_t reg) {
    if (cmicx_current_log_level>=cmicx_cutoff_log_level) {
        printf("0x%08X",reg);
        if (cmicx_log_file!=NULL) {
            fprintf(cmicx_log_file,"0x%08X",reg);
            fflush(cmicx_log_file);
        }
    }
}
void cmicx_log_int(int num) {
    if (cmicx_current_log_level>=cmicx_cutoff_log_level) {
        printf("%d",num);
        if (cmicx_log_file!=NULL) {
            fprintf(cmicx_log_file,"%d",num);
            fflush(cmicx_log_file);
        }
    }
}
void cmicx_log_unsigned(unsigned int num) {
    if (cmicx_current_log_level>=cmicx_cutoff_log_level) {
        printf("%u",num);
        if (cmicx_log_file!=NULL) {
            fprintf(cmicx_log_file,"%u",num);
            fflush(cmicx_log_file);
        }
    }
}
void cmicx_log_hex(unsigned int num) {
    if (cmicx_current_log_level>=cmicx_cutoff_log_level) {
        printf("0x%08X",num);
        if (cmicx_log_file!=NULL) {
            fprintf(cmicx_log_file,"0x%08X",num);
            fflush(cmicx_log_file);
        }
    }
}
void cmicx_log_uint64(uint64_t num) {
    if (cmicx_current_log_level>=cmicx_cutoff_log_level) {
        printf("0x%08X%08X",(uint32_t) ((num) >> 32), (uint32_t) num);
        if (cmicx_log_file!=NULL) {
            fprintf(cmicx_log_file,"0x%08X%08X",(uint32_t) ((num) >> 32), (uint32_t) num);
            fflush(cmicx_log_file);
        }
    }
}
void cmicx_log_byte(unsigned char num) {
    if (cmicx_current_log_level>=cmicx_cutoff_log_level) {
        printf("%02X",(unsigned)num);
        if (cmicx_log_file!=NULL) {
            fprintf(cmicx_log_file,"%02X",(unsigned)num);
            fflush(cmicx_log_file);
        }
    }
}
void cmicx_log_long(unsigned long num) {
    if (cmicx_current_log_level>=cmicx_cutoff_log_level) {
        printf("%ld", num);
        if (cmicx_log_file!=NULL) {
            fprintf(cmicx_log_file,"%ld", num);
            fflush(cmicx_log_file);
        }
    }
}
void cmicx_log_newline(void) {
    if (cmicx_current_log_level>=cmicx_cutoff_log_level) {
        printf("\n[SIM] ");
        if (cmicx_log_file!=NULL) {
            fprintf(cmicx_log_file,"\n[SIM] ");
            fflush(cmicx_log_file);
        }
    }
}

/* function to log the elapsed time since the last time this function was called. */
void cmicx_log_elapsed_time(void) {

    static struct timespec last_time;
    struct timespec time;
    struct timespec time_diff;

    clock_gettime(CLOCK_REALTIME,&time);
    cmicx_timespec_diff(&last_time,&time,&time_diff);
    cmicx_log_long(time_diff.tv_sec);
    cmicx_log("s, ");
    cmicx_log_long(time_diff.tv_nsec);
    cmicx_log("ns ");
    last_time = time;

}

/* send an error message to stdout, stderr, and the log file, and terminate the simulation if a \n is recieved */
void cmicx_error(char *str) {

    fprintf(stdout,"%s",str);
    fprintf(stderr,"%s",str);
    if (cmicx_log_file!=NULL) {
        fprintf(cmicx_log_file,"%s",str);
        fflush(cmicx_log_file);
    }

    if (strchr(str,'\n')!=NULL) {
        fclose(cmicx_log_file);
        exit(1);
    }

}


/* returns the timespec formatted difference in time between two timespecs */
void cmicx_timespec_diff(const struct timespec * const start_time,
                         const struct timespec * const end_time,
                         struct timespec * const diff_time) {

    if ((end_time->tv_nsec-start_time->tv_nsec) < 0) {
        diff_time->tv_sec  = end_time->tv_sec - start_time->tv_sec - 1;
        diff_time->tv_nsec = 1000000000 + (end_time->tv_nsec - start_time->tv_nsec);
    }
    else {
        diff_time->tv_sec  = end_time->tv_sec  - start_time->tv_sec;
        diff_time->tv_nsec = end_time->tv_nsec - start_time->tv_nsec;
    }

}


/* function to reverse word endianness */
reg_t cmicx_reverse_word_endianness(reg_t val) {

    reg_t return_val;
    int i;

    for (i=0; i<sizeof(reg_t); i++) {
        ((char *)&return_val)[(sizeof(reg_t)-i-1)] = ((char *)&val)[i];
    }

    return return_val;
}


