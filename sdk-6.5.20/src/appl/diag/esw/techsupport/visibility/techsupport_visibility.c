/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    techsupport_visibility.c
 * Purpose: This file contains the implementation to collect
 *          packet trace information from switch.
*/

#include <appl/diag/parse.h>
#include <shared/bsl.h>
#include <sal/core/alloc.h>
#include <appl/diag/techsupport.h>
#include <bcm_int/esw/instrumentation.h>
#include <appl/diag/system.h>

/* Function:    techsupport_visibility
 * Purpose :    Parse the visibility feature arguments and execute tx command.
 * Parameters:  u - Unit number
 *              a - pointer to argument
 *              techsupport_data_t -structure that maintains per chip per feature debug info.
 * Returns    : CMD_OK :done
 *              CMD_FAIL : INVALID INPUT
 */


typedef struct _bcm_visibility_raw {
  char *file;
  char *data;
  char *port;
  char *options;
} _bcm_visibility_raw_data_t;


STATIC void
_bcm_hexrawdata_dump(uint8 *raw_data, int len)
{
  int  ri;
  uint8 data[8] = {0};

  for (ri = 0; ri + 16 <= len; ) {
      sal_memcpy(&data, &raw_data[ri], 8);
      cli_out("%02x%02x%02x%02x%02x%02x%02x%02x ",
              data[0], data[1], data[2], data[3],
              data[4], data[5], data[6], data[7]);
      ri += 8;
      sal_memcpy(&data, &raw_data[ri], 8);
      cli_out("%02x%02x%02x%02x%02x%02x%02x%02x\n",
              data[0], data[1], data[2], data[3],
              data[4], data[5], data[6], data[7]);
      ri += 8;
  }
}

STATIC void
_bcm_hexpacket_dump(uint8 *raw_data, int len)
{
  int  ri;

  for (ri = 0; ri < len; ri++) {
      if (!(ri & 0x1f)) {
          cli_out("\n%04x  ", ri);
      }
      cli_out("%s%s%02x",
             (!(ri & 0x03)) ? " " : "",
             (!(ri & 0x0f)) ? " " : "",
             raw_data[ri]);
  }
    cli_out("\n");
}

/* parsing string given by user as packet payload */
STATIC uint8 *
_bcm_parse_packet_data(int unit, char *packet_data, int *length)
{
    uint8               *p;
    char                 tmp, data_iter;
    int                  data_len, i, j, pkt_len, data_base;

    /* If string data starts with 0x or 0X, skip it */
    if ((packet_data[0] == '0')
        && (packet_data[1] == 'x' || packet_data[1] == 'X')) {
        data_base = 2;
    } else {
        data_base = 0;
    }

    data_len = strlen(packet_data) - data_base;
    pkt_len = (data_len + 1) / 2;
    if (pkt_len < 1024) {
        pkt_len = 1024;
    }

    p = soc_cm_salloc(unit, pkt_len, "tx_string_packet");
    if (p == NULL) {
        return p;
    }
    sal_memset(p, 0, pkt_len);
    /* Convert char to value */
    i = j = 0;
    while (j < data_len) {
        data_iter = packet_data[data_base + j];
        if (('0' <= data_iter) && (data_iter <= '9')) {
            tmp = data_iter - '0';
        } else if (('a' <= data_iter) && (data_iter <= 'f')) {
            tmp = data_iter - 'a' + 10;
        } else if (('A' <= data_iter) && (data_iter <= 'F')) {
            tmp = data_iter - 'A' + 10;
        } else if (data_iter == ' ') {
            ++j;
            continue;
        } else {
            soc_cm_sfree(unit, p);
            return NULL;
        }

        p[i / 2] |= tmp << (((i + 1) % 2) * 4);
        ++i;
        ++j;
    }

    *length = i / 2 + 4; /* pad for crc */
    return p;
}


#ifndef NO_FILEIO

/* Discard the rest of the line */
STATIC int
_bcm_dis_line(FILE *fp)
{
    int c;
    do {
        if ((c = sal_fgetc(fp)) == EOF) {
            return c;
        }
    } while (c != '\n');
    return c;
}

STATIC int
_bcm_load_byte(FILE *fp, uint8 *byte)
{
    int         c, d;

    do {
        if ((c = sal_fgetc(fp)) == EOF) {
            return -1;
        }
        else if (c == '#') {
            if((c = _bcm_dis_line(fp)) == EOF) {
                return -1;
            }
        }
    } while (!isxdigit(c));

    do {
        if ((d = sal_fgetc(fp)) == EOF) {
            return -1;
        }
        else if (d == '#') {
            if((d = _bcm_dis_line(fp)) == EOF) {
                return -1;
            }
        }
    } while (!isxdigit(d));

    *byte = (xdigit2i(c) << 4) | xdigit2i(d);

    return 0;
}

STATIC uint8 *
_bcm_parse_packet_file(int unit, char *fname, int *length)
{
    uint8               *p;
    FILE                *fp;
    int                 i;

    if ((p = soc_cm_salloc(unit, 4096, "vs_packet")) == NULL)
        return p;

    if ((fp = sal_fopen(fname, "r")) == NULL) {
        soc_cm_sfree(unit, p);
        return NULL;
    }

    for (i = 0; i < 4096; i++) {
        if (_bcm_load_byte(fp, &p[i]) < 0)
            break;
    }

    *length = i;

    sal_fclose(fp);

    return p;
}
#endif /* NO_FILEIO */

int
_bcm_techsupport_visibility_raw (int unit, _bcm_visibility_raw_data_t vs_raw)
{
  int rc=-1, length=0, i=0;
  uint32 raw_data_size=0, options=0, port=0, print_raw_data=0;
  uint8 *pkt_data     = NULL;
  uint8 *raw_data_buf = NULL;
  char  *str_ptr      = NULL;

  if (vs_raw.port) {
      str_ptr = sal_strstr (vs_raw.port, "=");

     if (!str_ptr) {
         return CMD_USAGE;
        } else {
            port = sal_strtoul (str_ptr+1, NULL, 0);
        }
  }

  if (vs_raw.options) {
     str_ptr = NULL;
     str_ptr = sal_strstr (vs_raw.options, "=");

     if (!str_ptr){
         return CMD_USAGE;
        } else {
            options = sal_strtoul (str_ptr+1, NULL, 0);
     }
  }

  if (vs_raw.data) {
      str_ptr = NULL;
      str_ptr = sal_strstr (vs_raw.data, "=");
      if (!str_ptr) {
          return CMD_USAGE;
         }
         else {
           pkt_data = _bcm_parse_packet_data(unit, str_ptr+1, &length);
         }
  }

  if (vs_raw.file) {
     str_ptr = NULL;
     str_ptr = sal_strstr (vs_raw.file, "=");

     if (!str_ptr) {
         return CMD_USAGE;
        } else {
#ifdef NO_FILEIO
            return CMD_FAIL;
#else
        if ((pkt_data = _bcm_parse_packet_file(unit,
                                str_ptr+1, &length)) == NULL) {
           cli_out("Unable to load packet from file %s\n", str_ptr+1);
           return CMD_FAIL;
        }
           cli_out("Packet from file: %s, length=%d\n", str_ptr+1, length);
#endif
     }
  }

  raw_data_buf = sal_alloc (ARGS_BUFFER,"techsupport visibility raw");
  if (!raw_data_buf) {
      cli_out("Memory allocation failure\n");
      soc_cm_sfree(unit,pkt_data);
      return(CMD_FAIL);
  }
   else {
     sal_memset(raw_data_buf, 0x0, ARGS_BUFFER);
  }

  if ((length >= 16) && pkt_data) {
      rc = bcm_switch_pkt_trace_raw_data_get(unit, options, port, length,
                         pkt_data, ARGS_BUFFER, raw_data_buf, &raw_data_size);
  }
  cli_out("\n=====PACKET TRACE source_port=%s, %s\n",
          BCM_PORT_NAME(unit, port),
          bcm_errmsg(rc));
  if (rc == BCM_E_NONE) {
      cli_out("\n=====RAW PACKET\n");
      _bcm_hexpacket_dump(pkt_data,length);

      for (i = 0; i < ARGS_BUFFER; i++) {
         if (raw_data_buf[i]) {
             print_raw_data = 1;
             break;
         }
      }
      if (print_raw_data) {
          cli_out("\n=====RAW DATA\n");
          _bcm_hexrawdata_dump(raw_data_buf,
                  (int)raw_data_size);
       } else {
          rc = CMD_USAGE;
      }

  }
  sal_free(raw_data_buf);
  soc_cm_sfree(unit,pkt_data);
  return rc;
}



int
techsupport_visibility(int unit, args_t *a, techsupport_data_t *techsupport_data)
{
  char *str=NULL;
  char *tx_str= "tx 1 ";
  _bcm_visibility_raw_data_t pt_raw = {NULL, NULL, NULL, NULL};
  int rc=0, char_cnt=0;
  uint32 arg_cnt=0, i=0, arg_flag=0;
  arg_cnt=ARG_CNT(a);

  if (!arg_cnt || arg_cnt < 2) {
     return(CMD_USAGE);
   }

  str = sal_alloc (ARGS_BUFFER,"techsupport visibility");
  if (str == NULL) {
     cli_out("Memory allocation failure\n");
     return(CMD_FAIL);
  }
  else {
     sal_memset(str, 0x0, ARGS_BUFFER);
  }

  sal_strlcpy (str, tx_str, ARGS_BUFFER);

  for (i=0; i< arg_cnt; i++) {
      char *ptr;
      char_cnt=ARGS_BUFFER - sal_strlen(str);
      ptr = ARG_GET(a);
      if (char_cnt <= (sal_strlen(ptr) + sal_strlen(COMMAND_VISIBILITY) + 1)) {
         cli_out("Input limit exceeded ignoring argument/s:%s\n",ptr);
         break;
      }
      if ( 0 == sal_strncasecmp(ptr,"RAW",strlen("RAW"))){
         arg_flag |= 0x10;
      }
      else if ( !(arg_flag & 0x02) && (0 == sal_strncasecmp(ptr,"FILE",strlen("FILE")))) {
         arg_flag |= 0x1;
         sal_strncat(str, " ", char_cnt);
         sal_strncat(str, ptr, char_cnt-1);
         pt_raw.file = ptr;
      }
      else if ( !(arg_flag & 0x01) && (0 == sal_strncasecmp(ptr,"DATA",strlen("DATA")))){
         arg_flag |= 0x02;
         sal_strncat(str, " ", char_cnt);
         sal_strncat(str, ptr, char_cnt-1);
         pt_raw.data = ptr;
      }
      else if ( !(arg_flag & 0x04) && (0 == sal_strncasecmp(ptr,"SOURCEPORT",strlen("SOURCEPORT")))){
         arg_flag |= 0x04;
         sal_strncat(str, " "COMMAND_VISIBILITY, char_cnt);
         sal_strncat(str, ptr, char_cnt-sal_strlen(" "COMMAND_VISIBILITY));
         pt_raw.port = ptr;
      }
      else if ( !(arg_flag & 0x08) && (0 == sal_strncasecmp(ptr,"OPTIONS",strlen("OPTIONS")))){
         arg_flag |= 0x08;
         sal_strncat(str, " "COMMAND_VISIBILITY, char_cnt);
         sal_strncat(str, ptr, char_cnt-sal_strlen(" "COMMAND_VISIBILITY));
         pt_raw.options = ptr;
      }

   }

  if (!( arg_flag & 0x04 && (arg_flag & 0x02 || arg_flag & 0x01 ))) {
     sal_free(str);
     return(CMD_USAGE);
  }

  if ((arg_flag & 0x10) && (SOC_IS_TRIDENT3X(unit))) {
  rc = _bcm_techsupport_visibility_raw (unit, pt_raw);
  }
  else {
  rc = sh_process_command (unit, str);
  }
  sal_free(str);
  return rc;
}



