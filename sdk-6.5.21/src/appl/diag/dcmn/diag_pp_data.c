/* 
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        diag_pp_data.c
 * Purpose:     Routines for PP data export/import
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_APPL_SHELL

#include <sal/appl/sal.h>
#include <soc/mem.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_dbal.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_vtt.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_flp_init.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_diag.h>
#include <soc/sand/sand_signals.h>

#include <shared/bsl.h>
#include <shared/error.h>
#include <shared/utilex/utilex_str.h>
#include <shared/utilex/utilex_rhlist.h>
#include <shared/dbx/dbx_file.h>
#include <shared/dbx/dbx_xml.h>
#include <shared/swstate/access/sw_state_access.h>

#include <bcm/types.h>
#include <bcm/field.h>
#include <bcm_int/dpp/field_int.h>
#include <bcm_int/common/debug.h>

#include <appl/diag/dcmn/diag_pp_data.h>
#include <appl/diag/dcmn/export.h>
#include <appl/diag/dcmn/diag.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/diag/sand/diag_sand_dsig.h>

#define BSL_LOG_MODULE BSL_LS_APPL_SHELL

/* #define PP_DUMP_DEBUG */
/*************
 * DEFINES   *
 */
#if !defined(__KERNEL__)
/* { */
#define INIT_QUALIFIER(ce_instruction, type, n) { \
        memset(&qualifier[n], 0, sizeof(qualifier_t));       \
        qualifier[n].ce = ce_instruction;                    \
        sprintf(qualifier[n].ce_name, "CE%d-%d", type, n);   \
        qualifier[n].ce_type = type;                         \
        n++;                                                 \
    }

#define QUAL_BUFFER_LSB      0x0
#define QUAL_BUFFER_MSB      0x1

static int pp_dump_parser(int unit, void *curTop, int depth);
static int pp_dump_vt(int unit, void *curTop, int depth);
static int pp_dump_tt(int unit, void *curTop, int depth);
static int pp_dump_flp(int unit, void *curTop, int depth);

/*********************
 * STATIC DATA       *
 */

static char *processing_code_n[] =
{
  [ARAD_PP_ISEM_ACCESS_PROCESSING_CODE_TYPE_TM]                    = "TM",
  [ARAD_PP_ISEM_ACCESS_PROCESSING_CODE_TYPE_MIM]                   = "My-B-MAC",
  [ARAD_PP_ISEM_ACCESS_PROCESSING_CODE_TYPE_COMPATIBLE_MC]         = "Compatible MC",
  [ARAD_PP_ISEM_ACCESS_PROCESSING_CODE_TYPE_IP]                    = "IP",
  [ARAD_PP_ISEM_ACCESS_PROCESSING_CODE_TYPE_TRILL]                 = "TRILL",
  [ARAD_PP_ISEM_ACCESS_PROCESSING_CODE_TYPE_TRILL_DESIGNATED_VLAN] = "TRILL-Designated-Vlan",
  [ARAD_PP_ISEM_ACCESS_PROCESSING_CODE_TYPE_MPLS]                  = "MPLS",
  [ARAD_PP_ISEM_ACCESS_PROCESSING_CODE_TYPE_ARP]                   = "ARP",
  [ARAD_PP_ISEM_ACCESS_PROCESSING_CODE_TYPE_CUSTOM_1]              = "Custom-1",
  [ARAD_PP_ISEM_ACCESS_PROCESSING_CODE_TYPE_CUSTOM_2]              = "Custom-2",
  [ARAD_PP_ISEM_ACCESS_PROCESSING_CODE_TYPE_UNKNOWN_L3]            = "Unknown-L3",
  [ARAD_PP_ISEM_ACCESS_PROCESSING_CODE_TYPE_BRIDGE]                = "Bridge",
  [ARAD_PP_ISEM_ACCESS_PROCESSING_CODE_TYPE_MY_B_MAC_MC_BRIDGE]    = "My-B-MAC-MC-Bridge",
};

typedef struct {
    char        *name;
    int         (*dump_callback)(int, void *, int);
} stage_callback_t;

static stage_callback_t stage_callback[] =
{
    {"Parser",  pp_dump_parser},
    {"VT",      pp_dump_vt},
    {"TT",      pp_dump_tt},
    {"FLP",     pp_dump_flp},
    {NULL,      NULL}
};

static char *bool2str[] =
{
    [0] = "No",
    [1] = "Yes"
};

/*  Verified for Arad+ and Jericho */
static char *tt_result_to_use_n[] =
{
    [0] = "no result selected",
    [1] = "VT-Lookup1",
    [2] = "TT-Lookup0",
    [3] = "TT-Lookup1",
};

static char *lem_1st_lookup_key_select[] =
{
    [0] = "Key-A-LSB",
    [1] = "Key-B-LSB",
    [2] = "Key-C-LSB",
    [3] = "Key-D-LSB",
    [4] = "Unknown",
    [5] = "Unknown",
    [6] = "Unknown",
    [7] = "FID-Link-Layer-SA"
};

static char *lem_2nd_lookup_key_select[] =
{
    [0] = "Key-A-LSB",
    [1] = "Key-B-LSB",
    [2] = "Key-C-LSB",
    [3] = "Key-D-LSB",
    [4] = "Unknown",
    [5] = "Unknown",
    [6] = "Unknown",
    [7] = "FID-Forwarding-DA"
};

static char *lem_1st_lookup_key_type[] =
{
    [0] = "Dst-Lookup",
    [1] = "Src-Lookup-AGET",
};

static char *lpm_1st_lookup_key_select[] =
{
    [0] = "Key-A",
    [1] = "Key-B",
    [2] = "Key-C",
    [3] = "Key-D",
    [4] = "Unknown",
    [5] = "Unknown",
    [6] = "Unknown",
    [7] = "Fwd-Source-IP",
};

static char *lpm_2nd_lookup_key_select[] =
{
    [0] = "Key-A",
    [1] = "Key-B",
    [2] = "Key-C",
    [3] = "Key-D",
    [4] = "Unknown",
    [5] = "Unknown",
    [6] = "Unknown",
    [7] = "Fwd-Dest-IP",
};

static char *lpm_public_lookup_key_select[] =
{
    [0] = "Key-A",
    [1] = "Key-B",
    [2] = "Key-C",
    [3] = "Key-D",
    [4] = "Key-Size-From-Private",
    [5] = "Fwd-Source-IPv4-28",
    [6] = "Fwd-Source-IPv4",
    [7] = "Fwd-Source-IPv6",
};

static char *tcam_lookup_key_select[] =
{
    [0] = "Key-A",
    [1] = "Key-B",
    [2] = "Key-C",
    [3] = "Key-D",
};

static char *learn_key_select[] =
{
    [0] = "Key-A-LSB",
    [1] = "Key-B-LSB",
    [2] = "Key-C-LSB",
    [3] = "Key-D-LSB",
    [4] = "Unknown",
    [5] = "Unknown",
    [6] = "Unknown",
    [7] = "FID-Link-Layer-SA"
};

static char *packet_data_select[] =
{
    [0] = "Header-0",
    [1] = "Header-1",
    [2] = "Header-2",
    [3] = "Header-3",
    [4] = "Header-4",
    [5] = "Header-5",
    [6] = "Forwarding-Header",
    [7] = "Forwarding-Header+1",
};

static char *comparator_mode[] =
{
    [0] = "Reserved-0",
    [1] = "16-LSB-To-16-LSB-Comparator",
    [2] = "16-LSB-To-16-MSB-Comparator",
    [3] = "32-Bits",
    [4] = "16-LSB-To-Ethernet",
    [5] = "8-LSB-To-IP",
    [6] = "Reserved-6",
    [7] = "Reserved-7",
};

static char *unknown_address_control[] =
{
    [0] = "Reset",
    [1] = "Set",
    [2] = "Set-If-Lookup-A-Found",
    [3] = "Reset-If-Lookup-A-Found",
};

static char *ethernet_default_procedure[] =
{
    [0] = "Disable",
    [1] = "Enable",
    [2] = "Enable-If-Lookup-A-Found",
    [3] = "Enable-If-Lookup-A-Not-Found",
};

static char *pd_map_descr[] =
{
    [0] = "KeyA",
    [1] = "",
    [2] = "TCAM",
    [3] = "ISEM-B",
    [4] = "ISEM-A",
    [5] = "TCAM",
};

static int
dpp_qual_stage_size(
  int unit,
  char *stage_n,
  int buffer)
{
  int i, k;
  pp_block_t *cur_pp_block;
  pp_stage_t *cur_pp_stage;
  device_t *device;

  if ((device = sand_signal_device_get(unit)) == NULL)
  {
    return -1;
  }

  for (i = 0; i < device->block_num; i++)
  {
    cur_pp_block = &device->pp_blocks[i];

    for (k = 0; k < cur_pp_block->stage_num; k++)
    {
      cur_pp_stage = &cur_pp_block->stages[k];
      if (!sal_strcasecmp(stage_n, cur_pp_stage->name))
      {
        if (buffer == QUAL_BUFFER_LSB)
          return cur_pp_stage->buffer0_size;
        else if (buffer == QUAL_BUFFER_MSB)
          return cur_pp_stage->buffer1_size;
      }
    }
  }

  cli_out("Failed to find stage:%s buffer:%d\n", stage_n, buffer);
  return -1;
}

int
dpp_qual_signal_get(
  int unit,
  char *stage_n,
  int offset,
  int size,
  int buffer,
  char *qual_name)
{
  int i, k, j;
  pp_block_t *cur_pp_block;
  pp_stage_t *cur_pp_stage;
  internal_signal_t *qual_signal;
  int res = _SHR_E_NOT_FOUND;

  device_t *device;

  if ((device = sand_signal_device_get(unit)) == NULL)
  {
    return res;
  }

  if (device->pp_blocks == NULL)
  {
    /*
     * Not initialized yet
     */
    cli_out("Blocks DB was not initialized\n");
    return res;
  }

  for (i = 0; i < device->block_num; i++)
  {
    cur_pp_block = &device->pp_blocks[i];
    for (k = 0; k < cur_pp_block->stage_num; k++)
    {
      cur_pp_stage = &cur_pp_block->stages[k];
      if (sal_strcasecmp(stage_n, cur_pp_stage->name))
        continue;
      qual_signal = cur_pp_stage->signals;
      for (j = 0; j < cur_pp_stage->number; j++, qual_signal++)
      {
        if (qual_signal->buffer != buffer)
          continue;
        if (qual_signal->offset == offset)
        {
          if (qual_signal->size != size)
            sprintf(qual_name, "%s.Size-%db", qual_signal->name, size);
          else
            strcpy(qual_name, qual_signal->name);
          return _SHR_E_NONE;
        }
        else if ((offset > qual_signal->offset) && (offset < (qual_signal->offset + qual_signal->size)))
        {
          sprintf(qual_name,
            "%s.Offset-%db.Size-%db",
            qual_signal->name, qual_signal->offset - offset, size);
          return _SHR_E_NONE;
        }
      }
    }
  }

  return res;
}

static int pp_get_ce_qualifier(int unit, uint16 ce_value, int ce_type, char *stage_n, qualifier_t *qualifier)
{
    int res = _SHR_E_NOT_FOUND;
    int buffer_size;
    char header_str[8];

    qualifier->valid    = TRUE;
    qualifier->ce       = ce_value;
    qualifier->header   = ce_value & 0x7;

    if(ce_type == CE32_TYPE)
        qualifier->size = ((ce_value >> 11) & 0x1F) + 1;
    else if(ce_type == CE16_TYPE)
        qualifier->size = ((ce_value >> 12) & 0xF) + 1;
    else {
        cli_out("Bad ce type:%d\n", ce_type);
        goto exit;
    }

    if(!(ce_value & INTERNAL_BUFFER_QUALIFIER)) {
        if(qualifier->header < 6)
            sprintf(header_str, "%d", qualifier->header);
        else if(qualifier->header == 6)
            sprintf(header_str, "%s", "Fwd");
        else if(qualifier->header == 7)
            sprintf(header_str, "%s", "PostFwd");

        
        sprintf(qualifier->name, "%s-%s", "PacketQualifier", header_str);
        qualifier->offset = (ce_type == CE32_TYPE) ? ((ce_value >> 4) & 0x7F) * 8 : ((ce_value >> 4) & 0xFF) * 4;
        /* Not an error so keep return valid */
    }
    else {
        /* CE is qual signal */
        if((buffer_size = dpp_qual_stage_size(unit, stage_n, qualifier->header)) < 0) {
            cli_out("Could not get stage:%s buffer:%d\n", stage_n, qualifier->header);
            goto exit;
        }

        if(ce_type == CE32_TYPE)
            qualifier->offset = (buffer_size - INTERNAL_BUFFER_CE32_SHIFT) - ((ce_value >> 4) & 0x7F) * 8;
        else if(ce_type == CE16_TYPE)
            qualifier->offset = (buffer_size - INTERNAL_BUFFER_CE16_SHIFT) - ((ce_value >> 4) & 0xFF) * 4;

        if(dpp_qual_signal_get(unit, stage_n, qualifier->offset, qualifier->size, qualifier->header, qualifier->name) != _SHR_E_NONE)
            strcpy(qualifier->name, "UnresolvedQualifier");
    }
    res = _SHR_E_NONE;

exit:
    return res;
}

static shr_error_e pp_data_set_key(int unit, void *curIndex, char *stage_n, char *key_n, int valid, qualifier_t *qualifier, int qualifier_num)
{
    void *curKey, *curQualifier;
    int i;

    SHR_FUNC_INIT_VARS(unit);

    if(valid == 0)
        SHR_EXIT();

    if((curKey = dbx_xml_child_add(curIndex, key_n, 4)) == NULL) {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Failed to add %s entry\n", key_n);
    }

#ifdef PP_DUMP_DEBUG
    RHDATA_SET_HEX(curKey, "Valid", valid);
#endif

    for(i = 0; i < qualifier_num; i++) {
        if(valid & (1 << i)) {
            if(pp_get_ce_qualifier(unit, qualifier[i].ce, qualifier[i].ce_type, stage_n, &qualifier[i]) == -1)
                SHR_EXIT();
        }
    }

    for(i = 0; i < qualifier_num; i++) {
        if(qualifier[i].valid == TRUE) {
            if((curQualifier = dbx_xml_child_add(curKey, qualifier[i].name, 5)) == NULL) {
                SHR_ERR_EXIT(_SHR_E_FAIL, "Failed to add LSB qualifier:%s(%s) entry\n", qualifier[i].name,  qualifier[i].ce_name);
            }
            RHDATA_SET_STR(curQualifier, "ce",    qualifier[i].ce_name);
            RHDATA_SET_HEX(curQualifier, "value", qualifier[i].ce);
            RHDATA_SET_INT(curQualifier, "size",  qualifier[i].size);
            RHDATA_SET_INT(curQualifier, "offset",  qualifier[i].offset);
            qualifier[i].valid = FALSE; /* for reuse in other keys */
        }
    }

    dbx_xml_node_add_str(curKey, "\t\t\t\t");

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e pp_data_set_prefix(void *cur, uint32 and_value, uint32 or_value, int max_size)
{
    char tmp_str[RHNAME_MAX_SIZE];
    int i;

    SHR_FUNC_INIT_VARS(NO_UNIT);

#ifdef PP_DUMP_DEBUG
    RHDATA_SET_HEX(cur, "And-Mask", and_value);
    RHDATA_SET_HEX(cur, "Or-Mask", or_value);
#endif

    for(i = 0; i < max_size; i++) {
        if(and_value & 1)
            and_value = and_value >> 1;
        else
            break;
    }

    or_value = or_value >> i;

    sprintf(tmp_str, "0x%x (length %d)", or_value, max_size - i);
    RHDATA_SET_STR(cur, "Prefix", tmp_str);

 exit:
    SHR_FUNC_EXIT;
}

static shr_error_e pp_attribute_set(void *node, char *attribute, uint32 data, uint32 mask, int data_size)
{
    char selection_field[RHNAME_MAX_SIZE];
    int i;

    uint32 number = 0;

    SHR_FUNC_INIT_VARS(NO_UNIT);

    for(i = 0; i < data_size; i++)
        number |= 1 << i;

    if(number == mask)
        SHR_EXIT();

    if(mask == 0)
        sprintf(selection_field, "0x%x", data);
    else
        sprintf(selection_field, "0x%x & 0x%x", data, mask);

    RHDATA_SET_STR(node, attribute, selection_field);

exit:
    SHR_FUNC_EXIT;
}

static void pp_dump_pd2str(int pd_map, char *pd_descr, int port_default)
{
    int i;
    /* Init string */
    pd_descr[0] = 0;
    for(i = 5; i > 0; i--) {
        if(pd_map & (1 << i)) {
            if(pd_descr[0] != 0)
                strcat(pd_descr, " > ");

            strcat(pd_descr, pd_map_descr[i]);
        }
    }

    if(port_default) {
        if(pd_descr[0] != 0)
            strcat(pd_descr, " > ");

        strcat(pd_descr, "Port Default");
    }
    return;
}

static shr_error_e pp_dump_tables(int unit, void *curStage, int stage, int depth)
{
   int table_id;
   SOC_DPP_DBAL_TABLE_INFO  table;
   void *curSection, *curTable, *curIndex, *curQual;
   char tmp_str[RHNAME_MAX_SIZE], *str;
   int i,j;
   uint32 lem_prefix = 0;
   uint8 key_id;
   uint8 prog_id = 0;

   SHR_FUNC_INIT_VARS(unit);

   /* Create VT sub-sections: Program-Selection */
   if((curSection = dbx_xml_child_add(curStage, "Tables", depth)) == NULL) {
       SHR_ERR_EXIT(_SHR_E_FAIL, "Failed to add Table section to PP-Data\n");
   }

   for (table_id = 0; table_id < SOC_DPP_DBAL_SW_NOF_TABLES; table_id++) {
       sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table);
       if (!table.is_table_initiated)
           continue;

       /* Check if there are tables for the specified stage */
       for(i = 0; i < table.nof_table_programs; i++)
           if(stage == table.table_programs[i].stage)
               break;

       if(i == table.nof_table_programs)
           continue;

       utilex_str_replace_whitespace(tmp_str, table.table_name);
       if((curTable = dbx_xml_child_add(curSection, tmp_str, depth + 1)) == NULL) {
           SHR_ERR_EXIT(_SHR_E_FAIL, "Failed to add VT selection entry:%d\n", i);
       }

       RHDATA_SET_INT(curTable, "ID", table_id);
       RHDATA_SET_STR(curTable, "Memory", (char *)arad_pp_dbal_physical_db_to_string(table.physical_db_type));

       if (table.db_prefix == DBAL_PREFIX_NOT_DEFINED) {
           RHDATA_SET_STR(curTable, "Prefix", "Not Defined");
       }else {
           switch(table.physical_db_type){
           case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM:
               arad_pp_lem_access_app_to_prefix_get(unit, table.db_prefix, &lem_prefix);
               sprintf(tmp_str, "0x%x(0x%x) (length %d)", lem_prefix, table.db_prefix, table.db_prefix_len);
               RHDATA_SET_STR(curTable, "Prefix", tmp_str);
               break;
           case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM:
               RHDATA_SET_HEX(curTable, "Prefix", table.db_prefix);
               break;
           case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A:
           case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B:
           case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KAPS:
           case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KBP:
               sprintf(tmp_str, "0x%x (length %d)", table.db_prefix, table.db_prefix_len);
               RHDATA_SET_STR(curTable, "Prefix", tmp_str);
               break;

           default:
               break;
           }
       }

       RHDATA_SET_INT(curTable, "Entries", table.nof_entries_added_to_table);
       RHDATA_SET_INT(curTable, "Programs", table.nof_table_programs);

       for (i = 0; i < table.nof_table_programs; i++) {
           int rv;
           rv = arad_pp_dbal_program_to_string(unit, table.table_programs[i].stage, table.table_programs[i].program_id, (const char **)&str, &prog_id);
           if (rv)
           {
               cli_out("Failed arad_pp_dbal_program_to_string\n");
           }
           if((curIndex = dbx_xml_child_add(curTable, str, depth + 2)) == NULL) {
               SHR_ERR_EXIT(_SHR_E_FAIL, "Failed to add VT selection entry:%d\n", i);
           }

           RHDATA_SET_INT(curIndex, "ID", table.table_programs[i].program_id);

           key_id = table.table_programs[i].key_id;
           if(!SOC_IS_JERICHO(unit) && (table.physical_db_type == SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM) && (key_id == SOC_DPP_DBAL_PROGRAM_KEY_B))
               key_id = SOC_DPP_DBAL_PROGRAM_KEY_C;
           RHDATA_SET_STR(curIndex, "Key", (char *)arad_pp_dbal_key_id_to_string(unit, key_id));

           for (j = 0; j < table.nof_qualifiers; j++) {
               sprintf(tmp_str, "CE-%d", table.table_programs[i].ce_assigned[j]);
               if((curQual = dbx_xml_child_add(curIndex, tmp_str, depth + 3)) == NULL) {
                   SHR_ERR_EXIT(_SHR_E_FAIL, "Failed to add VT selection entry:%d\n", i);
               }
               str = (char *)SOC_PPC_FP_QUAL_TYPE_to_string(table.qual_info[j].qual_type);
               RHDATA_SET_STR(curQual, "Qualifier", str);
               RHDATA_SET_INT(curQual, "Size", table.qual_info[j].qual_nof_bits);
               RHDATA_SET_INT(curQual, "Offset", table.qual_info[j].qual_offset);
           }
           dbx_xml_node_end(curIndex, depth + 2);
       }
       dbx_xml_node_end(curTable, depth + 1);
   }
   dbx_xml_node_end(curSection, depth);
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e  pp_vt_ce_attribute_set(int unit, void *curKey, uint32 valid_1, uint32 valid_2, ARAD_PP_IHP_VTT1ST_KEY_CONSTRUCTION_TBL_DATA *vt_key_construction_p)
{
    qualifier_t qualifier[VTT_QUALIFIER_NUM];
    int i, k;
    int qualifier_shift, qualifier_num;
    int valid;
    void *curQualifier;

    SHR_FUNC_INIT_VARS(unit);

    qualifier_shift = SOC_IS_JERICHO(unit) ? 6:4;
    qualifier_num   = SOC_IS_JERICHO(unit) ? 12:8;

#ifdef PP_DUMP_DEBUG
    RHDATA_SET_HEX(curKey, "Valids-1", valid_1);
    RHDATA_SET_HEX(curKey, "Valids-2", valid_2);
#endif

    valid = valid_1 | valid_2 << qualifier_shift;
    if(!valid)
        SHR_EXIT();
    k = 0;

    INIT_QUALIFIER(vt_key_construction_p->key_16b_inst0, CE16_TYPE, k);
    INIT_QUALIFIER(vt_key_construction_p->key_16b_inst1, CE16_TYPE, k);
    if(SOC_IS_JERICHO(unit)) {
        INIT_QUALIFIER(vt_key_construction_p->key_16b_inst2, CE16_TYPE, k);
        INIT_QUALIFIER(vt_key_construction_p->key_16b_inst3, CE16_TYPE, k);
    }
    INIT_QUALIFIER(vt_key_construction_p->key_32b_inst0, CE32_TYPE, k);
    INIT_QUALIFIER(vt_key_construction_p->key_32b_inst1, CE32_TYPE, k);

    if(SOC_IS_JERICHO(unit)) {
        INIT_QUALIFIER(vt_key_construction_p->key_16b_inst4, CE16_TYPE, k);
        INIT_QUALIFIER(vt_key_construction_p->key_16b_inst5, CE16_TYPE, k);
        INIT_QUALIFIER(vt_key_construction_p->key_16b_inst6, CE16_TYPE, k);
        INIT_QUALIFIER(vt_key_construction_p->key_16b_inst7, CE16_TYPE, k);
    }
    else {
        INIT_QUALIFIER(vt_key_construction_p->key_16b_inst2, CE16_TYPE, k);
        INIT_QUALIFIER(vt_key_construction_p->key_16b_inst3, CE16_TYPE, k);
    }

    INIT_QUALIFIER(vt_key_construction_p->key_32b_inst2, CE32_TYPE, k);
    INIT_QUALIFIER(vt_key_construction_p->key_32b_inst3, CE32_TYPE, k);

    for(i = 0; i < qualifier_num; i++) {
        if(valid & (1 << i)) {
            if(pp_get_ce_qualifier(unit, qualifier[i].ce, qualifier[i].ce_type, "VT", &qualifier[i]) == -1)
                SHR_EXIT();
        }
    }

    for(i = 0; i < qualifier_num; i++) {
        if(qualifier[i].valid == TRUE) {
            if((curQualifier = dbx_xml_child_add(curKey, qualifier[i].name, 5)) == NULL) {
                SHR_ERR_EXIT(_SHR_E_FAIL, "Failed to add LSB qualifier:%s(%s) entry\n", qualifier[i].name,  qualifier[i].ce_name);
            }
            RHDATA_SET_STR(curQualifier, "ce",    qualifier[i].ce_name);
            RHDATA_SET_HEX(curQualifier, "value", qualifier[i].ce);
            RHDATA_SET_INT(curQualifier, "size",  qualifier[i].size);
            RHDATA_SET_INT(curQualifier, "offset",  qualifier[i].offset);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e pp_tt_ce_attribute_set(int unit, void *curKey, uint32 valid_1, uint32 valid_2, ARAD_PP_IHP_VTT2ND_KEY_CONSTRUCTION_TBL_DATA *tt_key_construction_p)
{
    qualifier_t qualifier[VTT_QUALIFIER_NUM];
    int i, k;
    int qualifier_shift, qualifier_num;
    int valid;
    void *curQualifier;

    SHR_FUNC_INIT_VARS(unit);

    qualifier_shift = SOC_IS_JERICHO(unit) ? 6:4;
    qualifier_num   = SOC_IS_JERICHO(unit) ? 12:8;

#ifdef PP_DUMP_DEBUG
    RHDATA_SET_HEX(curKey, "Valids-1", valid_1);
    RHDATA_SET_HEX(curKey, "Valids-2", valid_2);
#endif

    k = 0;
    INIT_QUALIFIER(tt_key_construction_p->key_16b_inst0, CE16_TYPE, k);
    INIT_QUALIFIER(tt_key_construction_p->key_16b_inst1, CE16_TYPE, k);
    if(SOC_IS_JERICHO(unit)) {
        INIT_QUALIFIER(tt_key_construction_p->key_16b_inst2, CE16_TYPE, k);
        INIT_QUALIFIER(tt_key_construction_p->key_16b_inst3, CE16_TYPE, k);
    }
    INIT_QUALIFIER(tt_key_construction_p->key_32b_inst0, CE32_TYPE, k);
    INIT_QUALIFIER(tt_key_construction_p->key_32b_inst1, CE32_TYPE, k);

    if(SOC_IS_JERICHO(unit)) {
        INIT_QUALIFIER(tt_key_construction_p->key_16b_inst4, CE16_TYPE, k);
        INIT_QUALIFIER(tt_key_construction_p->key_16b_inst5, CE16_TYPE, k);
        INIT_QUALIFIER(tt_key_construction_p->key_16b_inst6, CE16_TYPE, k);
        INIT_QUALIFIER(tt_key_construction_p->key_16b_inst7, CE16_TYPE, k);
    }
    else {
        INIT_QUALIFIER(tt_key_construction_p->key_16b_inst2, CE16_TYPE, k);
        INIT_QUALIFIER(tt_key_construction_p->key_16b_inst3, CE16_TYPE, k);
    }

    INIT_QUALIFIER(tt_key_construction_p->key_32b_inst2, CE32_TYPE, k);
    INIT_QUALIFIER(tt_key_construction_p->key_32b_inst3, CE32_TYPE, k);

    valid = valid_1 | valid_2 << qualifier_shift;

    for(i = 0; i < qualifier_num; i++) {
        if(valid & (1 << i)) {
            if(pp_get_ce_qualifier(unit, qualifier[i].ce, qualifier[i].ce_type, "TT", &qualifier[i]) == -1)
                SHR_EXIT();
        }
    }

    for(i = 0; i < qualifier_num; i++) {
        if(qualifier[i].valid == TRUE) {
            if((curQualifier = dbx_xml_child_add(curKey, qualifier[i].name, 5)) == NULL) {
                SHR_ERR_EXIT(_SHR_E_FAIL, "Failed to add LSB qualifier:%s(%s) entry\n", qualifier[i].name,  qualifier[i].ce_name);
            }
            RHDATA_SET_STR(curQualifier, "ce",    qualifier[i].ce_name);
            RHDATA_SET_HEX(curQualifier, "value", qualifier[i].ce);
            RHDATA_SET_INT(curQualifier, "size",  qualifier[i].size);
            RHDATA_SET_INT(curQualifier, "offset",  qualifier[i].offset);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e pp_dump_vt_selection(int unit, void *curStage, int depth)
{
    ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA    vt_prog_selection;

    int i;
    char *program_name;
    void *curSection, *curIndex;
    uint8 program_id;
    char selection_name[RHNAME_MAX_SIZE];

    SHR_FUNC_INIT_VARS(unit);

    /* Create VT sub-sections: Program-Selection */
    if((curSection = dbx_xml_child_add(curStage, "Program-Selection", depth)) == NULL) {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Failed to add VTT section to PP-Data\n");
    }

    for(i = 0; i < SOC_DPP_DEFS_GET(unit, nof_vtt_program_selection_lines); i++) {
        sal_memset(&vt_prog_selection, 0, sizeof(ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA));
        if (soc_sand_get_error_code_from_error_word(arad_pp_ihp_isem_1st_program_selection_cam_tbl_get_unsafe(
                                                                unit, i, &vt_prog_selection)) != SOC_SAND_OK)
            SHR_EXIT();

        if(!vt_prog_selection.valid)
            continue;

        sprintf(selection_name, "entry.%d", i);
        if((curIndex = dbx_xml_child_add(curSection, selection_name, depth + 1)) == NULL) {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Failed to add VT selection entry:%d\n", i);
        }

        arad_pp_isem_access_program_sel_line_to_program_id(unit, vt_prog_selection.llvp_prog_sel, 1, &program_id);
        program_name = arad_pp_isem_access_print_vt_program_data(unit, program_id, 0);
        RHDATA_SET_STR(curIndex, "Program", program_name);

        pp_attribute_set(curIndex, "Packet-Format-Code", vt_prog_selection.packet_format_code, vt_prog_selection.packet_format_code_mask, 6);
        pp_attribute_set(curIndex, "Leaf-Context", vt_prog_selection.parser_leaf_context, vt_prog_selection.parser_leaf_context_mask, 4);
        pp_attribute_set(curIndex, "Packet-Format-Qualifier-1", vt_prog_selection.packet_format_qualifier_1, vt_prog_selection.packet_format_qualifier_1_mask, 11);
        pp_attribute_set(curIndex, "Packet-Format-Qualifier-2", vt_prog_selection.packet_format_qualifier_2, vt_prog_selection.packet_format_qualifier_2_mask, 11);
        pp_attribute_set(curIndex, "PTC-VT-Profile", vt_prog_selection.ptc_vt_profile , vt_prog_selection.ptc_vt_profile_mask, 2);

        if(SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_ARADPLUS)
            pp_attribute_set(curIndex, "In-PP-Port-VT-Profile", vt_prog_selection.in_pp_port_vt_profile, vt_prog_selection.in_pp_port_vt_profile_mask, 3);
        else {
            pp_attribute_set(curIndex, "In-PP-Port-VT-Profile", vt_prog_selection.in_pp_port_vt_profile, vt_prog_selection.in_pp_port_vt_profile_mask, 5);
            pp_attribute_set(curIndex, "Packet-Format-Qualifier-0", vt_prog_selection.packet_format_qualifier_0, vt_prog_selection.packet_format_qualifier_0_mask, 3);
            pp_attribute_set(curIndex, "Incoming-Tag-Structure", vt_prog_selection.incoming_tag_structure, vt_prog_selection.incoming_tag_structure_mask, 4);
        }
    }
    dbx_xml_node_end(curSection, depth);

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e pp_dump_vt_programs(int unit, void *curStage, int depth)
{
    ARAD_PP_IHP_VTT1ST_KEY_CONSTRUCTION_TBL_DATA            vt_key_construction;

    int i;
    char *program_name;
    void *curSection, *curIndex, *curTable;
    uint8 program_id;
    char tmp_str[RHNAME_MAX_SIZE];

    SHR_FUNC_INIT_VARS(unit);

    if((curSection = dbx_xml_child_add(curStage, "Programs", 2)) == NULL) {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Failed to add VT section to PP-Data\n");
    }

    for(i = 0; i < SOC_DPP_DEFS_GET(unit, nof_vtt_programs); i++) {
        arad_pp_isem_access_program_sel_line_to_program_id(unit, i, 1, &program_id);
        if(program_id == 0xFF)
            continue;
        program_name = arad_pp_isem_access_print_vt_program_data(unit, program_id, 0);

        sal_memset(&vt_key_construction, 0, sizeof(ARAD_PP_IHP_VTT1ST_KEY_CONSTRUCTION_TBL_DATA));
        vt_key_construction.dbal = 1;
        if (soc_sand_get_error_code_from_error_word(arad_pp_ihp_vtt1st_key_construction_tbl_get_unsafe(unit, i, &vt_key_construction)) != SOC_SAND_OK)
            SHR_EXIT();

        if((curIndex = dbx_xml_child_add(curSection, program_name, depth + 1)) == NULL) {
             SHR_ERR_EXIT(_SHR_E_FAIL, "Failed to add VT program entry:%d\n", i);
        }

        RHDATA_SET_INT(curIndex, "Line", i);
        RHDATA_SET_INT(curIndex, "ID", program_id);
        RHDATA_SET_HEX(curIndex, "Profile", vt_key_construction.processing_profile);
        RHDATA_SET_HEX(curIndex, "Key-Variable", vt_key_construction.key_program_variable);
        RHDATA_SET_STR(curIndex, "Use-Strength", BOOLSTR(vt_key_construction.vlan_translation_is_use_strength));

        if(vt_key_construction.vlan_translation_is_use_strength == 0) {
            /* Priority map in use */
            pp_dump_pd2str(vt_key_construction.vlan_translation_0_pd_bitmap, tmp_str, 1);
            RHDATA_SET_STR(curIndex, "Priority-Decoder-0", tmp_str);
            pp_dump_pd2str(vt_key_construction.vlan_translation_1_pd_bitmap, tmp_str, 0);
            RHDATA_SET_STR(curIndex, "Priority-Decoder-1", tmp_str);
        }
        else {
            /* Strength in use */
            if((curTable = dbx_xml_child_add(curIndex, "Priority-Decoder-0", depth + 2)) == NULL) {
                  SHR_ERR_EXIT(_SHR_E_FAIL, "Failed to add TT selection entry:%d\n", i);
            }
/*            RHDATA_SET_HEX(curTable, "Bitmap", vt_key_construction.vlan_translation_0_pd_bitmap); */
            RHDATA_SET_HEX(curTable, "ISA-Strength", vt_key_construction.vlan_translation_0_pd_isa_strength);
            RHDATA_SET_HEX(curTable, "ISB-Strength", vt_key_construction.vlan_translation_0_pd_isb_strength);
            RHDATA_SET_HEX(curTable, "TCAM-Strength", vt_key_construction.vlan_translation_0_pd_tcam_strength);

            if((curTable = dbx_xml_child_add(curIndex, "Priority-Decoder-1", depth + 2)) == NULL) {
                  SHR_ERR_EXIT(_SHR_E_FAIL, "Failed to add TT selection entry:%d\n", i);
            }
/*           RHDATA_SET_HEX(curTable, "Bitmap", vt_key_construction.vlan_translation_1_pd_bitmap); */
            RHDATA_SET_HEX(curTable, "ISA-Strength", vt_key_construction.vlan_translation_1_pd_isa_strength);
            RHDATA_SET_HEX(curTable, "ISB-Strength", vt_key_construction.vlan_translation_1_pd_isb_strength);
            RHDATA_SET_HEX(curTable, "TCAM-Strength", vt_key_construction.vlan_translation_1_pd_tcam_strength);
        }

        if(vt_key_construction.isa_lookup_enable) {
            if((curTable = dbx_xml_child_add(curIndex, "ISEM_A", depth + 2)) == NULL) {
                  SHR_ERR_EXIT(_SHR_E_FAIL, "Failed to add TT selection entry:%d\n", i);
            }

            pp_data_set_prefix(curTable, vt_key_construction.isa_and_mask, vt_key_construction.isa_or_mask, 6);
            pp_vt_ce_attribute_set(unit, curTable, vt_key_construction.isa_key_valids_1, vt_key_construction.isa_key_valids_2, &vt_key_construction);
            dbx_xml_node_end(curTable, depth + 2);
        }
        if(vt_key_construction.isb_lookup_enable) {
            if((curTable = dbx_xml_child_add(curIndex, "ISEM_B", depth + 2)) == NULL) {
                  SHR_ERR_EXIT(_SHR_E_FAIL, "Failed to add TT selection entry:%d\n", i);
            }

            pp_data_set_prefix(curTable, vt_key_construction.isb_and_mask, vt_key_construction.isb_or_mask, 6);
            pp_vt_ce_attribute_set(unit, curTable, vt_key_construction.isb_key_valids_1, vt_key_construction.isb_key_valids_2, &vt_key_construction);
            dbx_xml_node_end(curTable, depth + 2);
        }
        if(vt_key_construction.tcam_db_profile != 0x3F) {
            if((curTable = dbx_xml_child_add(curIndex, "TCAM", depth + 2)) == NULL) {
                  SHR_ERR_EXIT(_SHR_E_FAIL, "Failed to add TT selection entry:%d\n", i);
            }

            RHDATA_SET_HEX(curTable, "Prefix", vt_key_construction.tcam_db_profile);

            pp_vt_ce_attribute_set(unit, curTable, vt_key_construction.tcam_key_valids_1, vt_key_construction.tcam_key_valids_2, &vt_key_construction);
            dbx_xml_node_end(curTable, depth + 2);
        }
        dbx_xml_node_end(curIndex, depth + 1);
    }

    dbx_xml_node_end(curSection, depth);

exit:
    SHR_FUNC_EXIT;
}

static int pp_dump_vt(int unit, void *curStage, int depth)
{
    uint32         res = CMD_FAIL;

    /* Create VT sub-sections: Program-Selection */
    res = pp_dump_vt_selection(unit, curStage, depth);

    /* Create VT sub-sections: Programs */
    res = pp_dump_vt_programs(unit, curStage, depth);

    pp_dump_tables(unit, curStage, SOC_PPC_FP_DATABASE_STAGE_INGRESS_VT, depth);

    return res;
}

static shr_error_e pp_dump_tt_selection(int unit, void *curStage, int depth)
{
    ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA    tt_prog_selection;

    int i;
    char *program_name;
    void *curSection, *curIndex;
    char tmp_str[RHNAME_MAX_SIZE];
    uint8 program_id;

    SHR_FUNC_INIT_VARS(unit);

    if((curSection = dbx_xml_child_add(curStage, "Program-Selection", depth)) == NULL) {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Failed to add VTT section to PP-Data\n");
    }

    for(i = 0; i < SOC_DPP_DEFS_GET(unit, nof_vtt_program_selection_lines); i++) {
        sal_memset(&tt_prog_selection, 0, sizeof(ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA));
        if (soc_sand_get_error_code_from_error_word(arad_pp_ihp_isem_2nd_program_selection_cam_tbl_get_unsafe(unit, i, &tt_prog_selection)) != SOC_SAND_OK)
            SHR_EXIT();

        if(!tt_prog_selection.valid)
            continue;

        sprintf(tmp_str, "entry.%d", i);
        if((curIndex = dbx_xml_child_add(curSection, tmp_str, depth + 1)) == NULL) {
              SHR_ERR_EXIT(_SHR_E_FAIL, "Failed to add VT program entry:%d\n", i);
        }

        arad_pp_isem_access_program_sel_line_to_program_id(unit, tt_prog_selection.llvp_prog_sel, 0, &program_id);
        program_name = arad_pp_isem_access_print_tt_program_data(unit, program_id, 0);
        RHDATA_SET_STR(curIndex, "Program", program_name);

        pp_attribute_set(curIndex, "Packet-Format-Code", tt_prog_selection.packet_format_code, tt_prog_selection.packet_format_code_mask, 6);
        pp_attribute_set(curIndex, "Leaf-Context", tt_prog_selection.parser_leaf_context, tt_prog_selection.parser_leaf_context_mask, 4);
        pp_attribute_set(curIndex, "Packet-Format-Qualifier-1", tt_prog_selection.packet_format_qualifier_1, tt_prog_selection.packet_format_qualifier_1_mask, 11);
        pp_attribute_set(curIndex, "Packet-Format-Qualifier-2", tt_prog_selection.packet_format_qualifier_2, tt_prog_selection.packet_format_qualifier_2_mask, 11);
        pp_attribute_set(curIndex, "PTC-TT-Profile", tt_prog_selection.ptc_tt_profile , tt_prog_selection.ptc_tt_profile_mask, 2);
        pp_attribute_set(curIndex, "My-MAC", tt_prog_selection.my_mac, tt_prog_selection.my_mac_mask, 1);
        pp_attribute_set(curIndex, "My-BMAC-MC", tt_prog_selection.my_b_mac_mc, tt_prog_selection.my_b_mac_mc_mask, 1);
        pp_attribute_set(curIndex, "DA-Is-All-R-Bridges", tt_prog_selection.da_is_all_r_bridges, tt_prog_selection.da_is_all_r_bridges_mask, 1);
        pp_attribute_set(curIndex, "TCAM-Lookup-Match", tt_prog_selection.tcam_lookup_match, tt_prog_selection.tcam_lookup_match_mask, 1);
        pp_attribute_set(curIndex, "Packet-Is-Compatible-MC", tt_prog_selection.packet_is_compatible_mc, tt_prog_selection.packet_is_compatible_mc_mask, 1);
        pp_attribute_set(curIndex, "ISA-Lookup-Found", tt_prog_selection.isa_lookup_found, tt_prog_selection.isa_lookup_found_mask, 1);
        pp_attribute_set(curIndex, "ISB-Lookup-Found", tt_prog_selection.isb_lookup_found, tt_prog_selection.isb_lookup_found_mask, 1);
        pp_attribute_set(curIndex, "VLAN-Is-Designated", tt_prog_selection.vlan_is_designated, tt_prog_selection.vlan_is_designated_mask, 1);
        pp_attribute_set(curIndex, "VT-Processing-Profile", tt_prog_selection.vt_processing_profile, tt_prog_selection.vt_processing_profile_mask, 3);

        if(SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_ARADPLUS) {
            pp_attribute_set(curIndex, "In-PP-Port-TT-Profile", tt_prog_selection.in_pp_port_tt_profile, tt_prog_selection.in_pp_port_tt_profile_mask, 3);
            pp_attribute_set(curIndex, "Packet-Format-Qualifier-3-Outer-VID", tt_prog_selection.packet_format_qualifier_3_outer_vid_bits, tt_prog_selection.packet_format_qualifier_3_outer_vid_bits_mask, 3);
        }
        else { /* Jericho / QMX */
            pp_attribute_set(curIndex, "In-PP-Port-TT-Profile", tt_prog_selection.in_pp_port_tt_profile, tt_prog_selection.in_pp_port_tt_profile_mask, 5);
            pp_attribute_set(curIndex, "Packet-Format-Qualifier-3", tt_prog_selection.packet_format_qualifier_3, tt_prog_selection.packet_format_qualifier_3_mask, 11);
            pp_attribute_set(curIndex, "VT-In-LIF-Profile", tt_prog_selection.vt_in_lif_profile, tt_prog_selection.vt_in_lif_profile_mask, 4);
        }
    }

    dbx_xml_node_end(curSection, depth);

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e pp_dump_tt_programs(int unit, void *curStage, int depth)
{
    ARAD_PP_IHP_VTT2ND_KEY_CONSTRUCTION_TBL_DATA            tt_key_construction;

    int i;
    char *program_name;
    void *curSection, *curIndex, *curTable;
    uint8 program_id;
    char tmp_str[RHNAME_MAX_SIZE];

    SHR_FUNC_INIT_VARS(unit);

    if((curSection = dbx_xml_child_add(curStage, "Programs", depth)) == NULL) {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Failed to add TT-Programs section to PP-Data\n");
    }

    for(i = 0; i < SOC_DPP_DEFS_GET(unit, nof_vtt_programs); i++) {
        arad_pp_isem_access_program_sel_line_to_program_id(unit, i, 0, &program_id);
        if(program_id == 0xFF)
            continue;
        program_name = arad_pp_isem_access_print_tt_program_data(unit, program_id, 0);

        sal_memset(&tt_key_construction, 0, sizeof(ARAD_PP_IHP_VTT2ND_KEY_CONSTRUCTION_TBL_DATA));
        tt_key_construction.dbal = 1;
        if(soc_sand_get_error_code_from_error_word(arad_pp_ihp_vtt2nd_key_construction_tbl_get_unsafe(unit, i, &tt_key_construction)) != SOC_SAND_OK)
            SHR_EXIT();

        if((curIndex = dbx_xml_child_add(curSection, program_name, depth + 1)) == NULL) {
              SHR_ERR_EXIT(_SHR_E_FAIL, "Failed to add TT program entry:%d\n", i);
        }

        RHDATA_SET_INT(curIndex, "Line", i);
        RHDATA_SET_INT(curIndex, "ID", program_id);
        RHDATA_SET_HEX(curIndex, "Profile", tt_key_construction.processing_profile);
        RHDATA_SET_HEX(curIndex, "Key-Variable", tt_key_construction.key_program_variable);
        RHDATA_SET_STR(curIndex, "Processing-Code", (tt_key_construction.processing_code >= ARAD_PP_ISEM_ACCESS_PROCESSING_CODE_TYPE_NUM)
                                                        ? "Unknown" : processing_code_n[tt_key_construction.processing_code]);
        RHDATA_SET_STR(curIndex, "Second-Stage-Parsing", (tt_key_construction.second_stage_parsing == 1) ? "yes" : "no");
        RHDATA_SET_STR(curIndex, "Result-To-Use-Outermost", tt_result_to_use_n[tt_key_construction.result_to_use_0]);
        RHDATA_SET_STR(curIndex, "Result-To-Use-MPLS", tt_result_to_use_n[tt_key_construction.result_to_use_1]);
        RHDATA_SET_STR(curIndex, "Result-To-Use-Innermost", tt_result_to_use_n[tt_key_construction.result_to_use_2]);

        pp_dump_pd2str(tt_key_construction.termination_0_pd_bitmap, tmp_str, 1);
        RHDATA_SET_STR(curIndex, "Priority-Decoder-0", tmp_str);
        pp_dump_pd2str(tt_key_construction.termination_1_pd_bitmap, tmp_str, 0);
        RHDATA_SET_STR(curIndex, "Priority-Decoder-1", tmp_str);

        if(tt_key_construction.isa_lookup_enable) {
            if((curTable = dbx_xml_child_add(curIndex, "ISEM_A", depth + 2)) == NULL) {
                  SHR_ERR_EXIT(_SHR_E_FAIL, "Failed to add TT selection entry:%d\n", i);
            }
            RHDATA_SET_STR(curTable, "Initial-Key-VT", (tt_key_construction.isa_key_initial_from_vt == 1) ? "yes" : "no");

            pp_data_set_prefix(curTable, tt_key_construction.isa_and_mask, tt_key_construction.isa_or_mask, 6);
            pp_tt_ce_attribute_set(unit, curTable, tt_key_construction.isa_key_valids_1, tt_key_construction.isa_key_valids_2, &tt_key_construction);
            dbx_xml_node_end(curTable, depth + 2);
        }
        if(tt_key_construction.isb_lookup_enable) {
            if((curTable = dbx_xml_child_add(curIndex, "ISEM_B", depth + 2)) == NULL) {
                   SHR_ERR_EXIT(_SHR_E_FAIL, "Failed to add TT selection entry:%d\n", i);
            }
            RHDATA_SET_STR(curTable, "Initial-Key-VT", (tt_key_construction.isb_key_initial_from_vt == 1) ? "yes" : "no");

            pp_data_set_prefix(curTable, tt_key_construction.isb_and_mask, tt_key_construction.isb_or_mask, 6);
            pp_tt_ce_attribute_set(unit, curTable, tt_key_construction.isb_key_valids_1, tt_key_construction.isb_key_valids_2, &tt_key_construction);
            dbx_xml_node_end(curTable, depth + 2);
        }
        if(tt_key_construction.tcam_db_profile != 0x3F) {
            if((curTable = dbx_xml_child_add(curIndex, "TCAM", depth + 2)) == NULL) {
                  SHR_ERR_EXIT(_SHR_E_FAIL, "Failed to add TT selection entry:%d\n", i);
            }
            RHDATA_SET_STR(curTable, "Initial-Key-VT", (tt_key_construction.tcam_key_initial_from_vt == 1) ? "yes" : "no");

            RHDATA_SET_HEX(curTable, "Prefix", tt_key_construction.tcam_db_profile);
            pp_tt_ce_attribute_set(unit, curTable, tt_key_construction.tcam_key_valids_1, tt_key_construction.tcam_key_valids_2, &tt_key_construction);
            dbx_xml_node_end(curTable, depth + 2);
        }
        dbx_xml_node_end(curIndex, depth + 1);
    }

    dbx_xml_node_end(curSection, depth);

exit:
    SHR_FUNC_EXIT;
}

static int pp_dump_tt(int unit, void *curStage, int depth)
{
    uint32         res = CMD_OK;

    /* Create TT sub-sections: Program-Selection */
    res = pp_dump_tt_selection(unit, curStage, depth);

    /* Create TT sub-sections: Programs */
    res = pp_dump_tt_programs(unit, curStage, depth);

    pp_dump_tables(unit, curStage, SOC_PPC_FP_DATABASE_STAGE_INGRESS_TT, depth);

    return res;
}

static shr_error_e pp_dump_flp_selection(int unit, void *curStage, int depth)
{
    ARAD_PP_IHB_FLP_PROGRAM_SELECTION_CAM_TBL_DATA    flp_prog_selection;

    int i;
    char *program_name;
    void *curSection, *curIndex;
    uint8 program_id;
    char tmp_str[RHNAME_MAX_SIZE];

    SHR_FUNC_INIT_VARS(unit);

    if((curSection = dbx_xml_child_add(curStage, "Program-Selection", depth)) == NULL) {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Failed to add VTT section to PP-Data\n");
    }

    for(i = 0; i < SOC_DPP_DEFS_GET(unit, nof_flp_program_selection_lines); i++) {
        sal_memset(&flp_prog_selection, 0, sizeof(ARAD_PP_IHB_FLP_PROGRAM_SELECTION_CAM_TBL_DATA));
        if (soc_sand_get_error_code_from_error_word(arad_pp_ihb_flp_program_selection_cam_tbl_get_unsafe(unit, i, &flp_prog_selection)) != SOC_SAND_OK)
            SHR_EXIT();

        if(!flp_prog_selection.valid)
            continue;

        sprintf(tmp_str, "entry.%d", i);
        if((curIndex = dbx_xml_child_add(curSection, tmp_str, depth + 1)) == NULL) {
           SHR_ERR_EXIT(_SHR_E_FAIL, "Failed to add FLP selection entry:%d\n", i);
        }

        if(soc_sand_get_error_code_from_error_word(sw_state_access[unit].dpp.soc.arad.pp.fec.flp_progs_mapping.get(unit, flp_prog_selection.program, &program_id)) != SOC_SAND_OK)
            program_name = "Unknown Program";
        else
            program_name = arad_pp_flp_prog_id_to_prog_name(unit, program_id);

        RHDATA_SET_STR(curIndex, "Program", program_name);
        pp_attribute_set(curIndex, "Parser-Leaf-Context",         flp_prog_selection.parser_leaf_context, flp_prog_selection.parser_leaf_context_mask, 4);
        pp_attribute_set(curIndex, "Port-Profile",                flp_prog_selection.port_profile, flp_prog_selection.port_profile_mask, 5);
        pp_attribute_set(curIndex, "PTC-Profile",                 flp_prog_selection.ptc_profile, flp_prog_selection.ptc_profile_mask, 2);
        pp_attribute_set(curIndex, "Packet-Format-Code",          flp_prog_selection.packet_format_code, flp_prog_selection.packet_format_code_mask, 6);
        pp_attribute_set(curIndex, "Packet-Format-Qualifier",     flp_prog_selection.forwarding_header_qualifier, flp_prog_selection.forwarding_header_qualifier_mask, 11);
        pp_attribute_set(curIndex, "Forwarding-Code",             flp_prog_selection.forwarding_code, flp_prog_selection.forwarding_code_mask, 4);
        pp_attribute_set(curIndex, "Forwarding-Offset-Index",     flp_prog_selection.forwarding_offset_index, flp_prog_selection.forwarding_offset_index_mask, 3);
        pp_attribute_set(curIndex, "L-3-VPN-Default-Routing",     flp_prog_selection.l_3_vpn_default_routing, flp_prog_selection.l_3_vpn_default_routing_mask, 1);
        pp_attribute_set(curIndex, "Trill-MC",                    flp_prog_selection.trill_mc, flp_prog_selection.trill_mc_mask, 1);
        pp_attribute_set(curIndex, "Packet-Is-Compatible-MC",     flp_prog_selection.packet_is_compatible_mc, flp_prog_selection.packet_is_compatible_mc_mask, 1);
        pp_attribute_set(curIndex, "In-RIF-UC-RPF-Enable",        flp_prog_selection.in_rif_uc_rpf_enable, flp_prog_selection.in_rif_uc_rpf_enable_mask, 1);
        pp_attribute_set(curIndex, "LL-Is-Arp",                   flp_prog_selection.ll_is_arp, flp_prog_selection.ll_is_arp_mask, 1);
        pp_attribute_set(curIndex, "Elk-Status",                  flp_prog_selection.elk_status, flp_prog_selection.elk_status_mask, 1);
        pp_attribute_set(curIndex, "Cos-Profile",                 flp_prog_selection.cos_profile, flp_prog_selection.cos_profile_mask,6);
        pp_attribute_set(curIndex, "Service-Type",                flp_prog_selection.service_type, flp_prog_selection.service_type_mask, 3);
        pp_attribute_set(curIndex, "VT-Processing-Profile",       flp_prog_selection.vt_processing_profile, flp_prog_selection.vt_processing_profile_mask, 3);
        pp_attribute_set(curIndex, "VT-Lookup-0-Found",           flp_prog_selection.vt_lookup_0_found, flp_prog_selection.vt_lookup_0_found_mask, 1);
        pp_attribute_set(curIndex, "VT-Lookup-1-Found",           flp_prog_selection.vt_lookup_1_found, flp_prog_selection.vt_lookup_1_found_mask, 1);
        pp_attribute_set(curIndex, "TT-Processing-Profile",       flp_prog_selection.tt_processing_profile, flp_prog_selection.tt_processing_profile_mask, 3);
        pp_attribute_set(curIndex, "TT-Lookup-0-Found",           flp_prog_selection.tt_lookup_0_found, flp_prog_selection.tt_lookup_0_found_mask, 1);
        pp_attribute_set(curIndex, "TT-Lookup-1-Found",           flp_prog_selection.tt_lookup_1_found, flp_prog_selection.tt_lookup_1_found_mask, 1);
        /* Jericho new fields */
        pp_attribute_set(curIndex, "Forwarding-Offset-Index-Ext", flp_prog_selection.forwarding_offset_index_ext, flp_prog_selection.forwarding_offset_index_ext_mask, 2);
        pp_attribute_set(curIndex, "CPU-Trap-Code",               flp_prog_selection.cpu_trap_code, flp_prog_selection.cpu_trap_code_mask, 8);
        pp_attribute_set(curIndex, "In-LIF-Profile",              flp_prog_selection.in_lif_profile, flp_prog_selection.in_lif_profile_mask, 4);
        pp_attribute_set(curIndex, "LLVP-Incoming-Tag-Structure", flp_prog_selection.llvp_incoming_tag_structure, flp_prog_selection.llvp_incoming_tag_structure_mask, 4);
        /* uint32 in_rif_profile; */ /* Handled in arad_tbl_access internally */
        pp_attribute_set(curIndex, "Forwarding-Plus-1-Header-Qualifier", flp_prog_selection.forwarding_plus_1_header_qualifier, flp_prog_selection.forwarding_plus_1_header_qualifier_mask, 11);
        pp_attribute_set(curIndex, "TT-Code",                     flp_prog_selection.tunnel_termination_code, flp_prog_selection.tunnel_termination_code_mask, 4);
        pp_attribute_set(curIndex, "Qualifier-O",                 flp_prog_selection.qualifier_0, flp_prog_selection.qualifier_0_mask, 3);
        pp_attribute_set(curIndex, "In-LIF-Data-Index",           flp_prog_selection.in_lif_data_index, flp_prog_selection.in_lif_data_index_mask, 2);
        /* uint32 in_rif_profile_mask; */ /* Handled in arad_tbl_access internally */
        pp_attribute_set(curIndex, "In-LIF-Data-MSB",             flp_prog_selection.in_lif_data_msb, flp_prog_selection.in_lif_data_msb_mask, 5);

        dbx_xml_node_end(curIndex, depth + 1);
    }

    dbx_xml_node_end(curSection, depth);

exit:
    SHR_FUNC_EXIT;
}

static void pp_dump_flp_set_qualifiers(int unit, int line, qualifier_t *qualifier, int *index, int is_msb, int *valid_a, int *valid_b, int *valid_c, int *valid_d)
{
    ARAD_PP_IHB_FLP_KEY_CONSTRUCTION_TBL_DATA        flp_key_cons;
    int k = *index;

    arad_pp_ihb_flp_key_construction_ext_tbl_get_unsafe(unit, line, is_msb, &flp_key_cons);

    *valid_a = *valid_a | flp_key_cons.key_a_inst_0_to_5_valid << k;
    *valid_b = *valid_b | flp_key_cons.key_b_inst_0_to_5_valid << k;
    *valid_c = *valid_c | flp_key_cons.key_c_inst_0_to_5_valid << k;
    *valid_d = *valid_d | flp_key_cons.key_d_inst_0_to_7_valid << k;

#ifdef PP_DUMP_DEBUG
    cli_out("Line:%02d Index %02d MSB:%d A=%02x B=%02x C=%02x D=%02x\n", line, k, is_msb,
                                                                         flp_key_cons.key_a_inst_0_to_5_valid,
                                                                         flp_key_cons.key_b_inst_0_to_5_valid,
                                                                         flp_key_cons.key_c_inst_0_to_5_valid,
                                                                         flp_key_cons.key_d_inst_0_to_7_valid);
    cli_out("\t A=%02x B=%02x C=%02x D=%02x\n", *valid_a, *valid_b, *valid_c, *valid_d);
#endif

    INIT_QUALIFIER(flp_key_cons.instruction_0_16b, CE16_TYPE, k);
    INIT_QUALIFIER(flp_key_cons.instruction_1_16b, CE16_TYPE, k);
    INIT_QUALIFIER(flp_key_cons.instruction_2_16b, CE16_TYPE, k);
    INIT_QUALIFIER(flp_key_cons.instruction_3_32b, SOC_IS_JERICHO(unit) ? CE16_TYPE : CE32_TYPE, k);
    INIT_QUALIFIER(flp_key_cons.instruction_4_32b, CE32_TYPE, k);
    INIT_QUALIFIER(flp_key_cons.instruction_5_32b, CE32_TYPE, k);
    if(SOC_IS_JERICHO(unit)) {
        INIT_QUALIFIER(flp_key_cons.instruction_6_32b, CE32_TYPE, k);
        INIT_QUALIFIER(flp_key_cons.instruction_7_32b, CE32_TYPE, k);
    }

    *index = k;
}

static shr_error_e pp_dump_flp_programs(int unit, void *curStage, int depth)
{
    ARAD_PP_IHB_FLP_LOOKUPS_TBL_DATA    lookups_tbl;
    ARAD_PP_IHB_FLP_PROCESS_TBL_DATA    process_tbl;

    qualifier_t qualifier[FLP_QUALIFIER_NUM];
    int valid_A, valid_B, valid_C, valid_D;

    int i, k;
    char *program_name;
    void *curSection, *curIndex, *curTable, *curPublicTable;
    uint8 program_id;

    SHR_FUNC_INIT_VARS(unit);

    if((curSection = dbx_xml_child_add(curStage, "Programs", depth)) == NULL) {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Failed to add TT-Programs section to PP-Data\n");
    }

    for(i = 0; i < SOC_DPP_DEFS_GET(unit, nof_flp_programs); i++) {
        if (soc_sand_get_error_code_from_error_word(sw_state_access[unit].dpp.soc.arad.pp.fec.flp_progs_mapping.get(unit, i, &program_id)) != SOC_SAND_OK)
            SHR_EXIT();

        if(program_id == ARAD_PP_FLP_MAP_PROG_NOT_SET)
            continue;

        program_name = arad_pp_flp_prog_id_to_prog_name(unit, program_id);

        if((curIndex = dbx_xml_child_add(curSection, program_name, depth + 1)) == NULL) {
           SHR_ERR_EXIT(_SHR_E_FAIL, "Failed to add FLP program entry:%d\n", i);
        }

        RHDATA_SET_INT(curIndex, "Line", i);
        RHDATA_SET_INT(curIndex, "ID", program_id);

        k = 0;
        valid_A = valid_B = valid_C = valid_D = 0;
        pp_dump_flp_set_qualifiers(unit, i,                                 qualifier, &k, 0, &valid_A, &valid_B, &valid_C, &valid_D);
        pp_dump_flp_set_qualifiers(unit, i + ARAD_PP_FLP_INSTRUCTIONS_NOF,  qualifier, &k, 0, &valid_A, &valid_B, &valid_C, &valid_D);
        pp_dump_flp_set_qualifiers(unit, i,                                 qualifier, &k, 1, &valid_A, &valid_B, &valid_C, &valid_D);
        pp_dump_flp_set_qualifiers(unit, i + ARAD_PP_FLP_INSTRUCTIONS_NOF,  qualifier, &k, 1, &valid_A, &valid_B, &valid_C, &valid_D);

        arad_pp_ihb_flp_lookups_tbl_get_unsafe(unit, i, &lookups_tbl);
        arad_pp_ihb_flp_process_tbl_get_unsafe(unit, i, &process_tbl);

        RHDATA_SET_INT(curIndex, "Profile", process_tbl.fwd_processing_profile);
        RHDATA_SET_STR(curIndex, "Learn", bool2str[process_tbl.learn_enable]);

        /* coverity[stack_use_overflow : FALSE] */
        pp_data_set_key(unit, curIndex, "FLP", "Key-A", valid_A, qualifier, k);
        pp_data_set_key(unit, curIndex, "FLP", "Key-B", valid_B, qualifier, k);
        pp_data_set_key(unit, curIndex, "FLP", "Key-C", valid_C, qualifier, k);
        pp_data_set_key(unit, curIndex, "FLP", "Key-D", valid_D, qualifier, k);
        if(lookups_tbl.lem_1st_lkp_valid) {
            if((curTable = dbx_xml_child_add(curIndex, "LEM-1st", depth + 2)) == NULL) {
                SHR_ERR_EXIT(_SHR_E_FAIL, "Failed to add LEM-1 program entry:%d\n", i);
            }

            RHDATA_SET_STR(curTable, "Key", lem_1st_lookup_key_select[lookups_tbl.lem_1st_lkp_key_select]);
            RHDATA_SET_STR(curTable, "Key-Type", lem_1st_lookup_key_type[lookups_tbl.lem_1st_lkp_key_type]);

            /* Specifies how to modify the 4(6) MSB of Large-Exact Match (LEM)key. First the AND mask is applied and then the OR mask */
            pp_data_set_prefix(curTable, lookups_tbl.lem_1st_lkp_and_value, lookups_tbl.lem_1st_lkp_or_value, 5);
        }

        if(lookups_tbl.lem_2nd_lkp_valid) {
            if((curTable = dbx_xml_child_add(curIndex, "LEM-2nd", depth + 2)) == NULL) {
                SHR_ERR_EXIT(_SHR_E_FAIL, "Failed to add LEM-2 program entry:%d\n", i);
            }

            RHDATA_SET_STR(curTable, "Key", lem_2nd_lookup_key_select[lookups_tbl.lem_2nd_lkp_key_select]);

            /* Specifies how to modify the 4 MSB of Large-Exact Mach (LEM)key. First the AND mask is applied and then the OR mask */
            pp_data_set_prefix(curTable, lookups_tbl.lem_2nd_lkp_and_value, lookups_tbl.lem_2nd_lkp_or_value, 5);
        }

        if(lookups_tbl.lpm_1st_lkp_valid) {
            if((curTable = dbx_xml_child_add(curIndex, "LPM-1st", depth + 2)) == NULL) {
                SHR_ERR_EXIT(_SHR_E_FAIL, "Failed to add LEM-1 program entry:%d\n", i);
            }

            RHDATA_SET_STR(curTable, "Private-Key", lpm_1st_lookup_key_select[lookups_tbl.lpm_1st_lkp_key_select]);
            RHDATA_SET_STR(curTable, "Default-Enabled", bool2str[process_tbl.lpm_1st_lkp_enable_default]);

            /* Specifies how to modify the 4 MSB of Large-Exact Mach (LEM)key. First the AND mask is applied and then the OR mask */
            pp_data_set_prefix(curTable, lookups_tbl.lpm_1st_lkp_and_value, lookups_tbl.lpm_1st_lkp_or_value, 5);
            if(lookups_tbl.lpm_public_1st_lkp_valid) {
                if((curPublicTable = dbx_xml_child_add(curTable, "Public", depth + 3)) == NULL) {
                    SHR_ERR_EXIT(_SHR_E_FAIL, "Failed to add Public entry in :%d\n", i);
                }
                RHDATA_SET_STR(curPublicTable, "Key", lpm_public_lookup_key_select[lookups_tbl.lpm_public_1st_lkp_key_select]);
                RHDATA_SET_STR(curPublicTable, "Default-Enabled", bool2str[process_tbl.lpm_public_1st_lkp_enable_default]);

                if(lookups_tbl.lpm_public_1st_lkp_key_size)
                    RHDATA_SET_HEX(curPublicTable, "Key-Size", lookups_tbl.lpm_public_1st_lkp_key_size);

                pp_data_set_prefix(curPublicTable, lookups_tbl.lpm_public_1st_lkp_and_value, lookups_tbl.lpm_public_1st_lkp_or_value, 5);
            }
        }

        if(lookups_tbl.lpm_2nd_lkp_valid) {
            if((curTable = dbx_xml_child_add(curIndex, "LPM-2nd", depth + 2)) == NULL) {
                SHR_ERR_EXIT(_SHR_E_FAIL, "Failed to add LEM-2 program entry:%d\n", i);
            }

            RHDATA_SET_STR(curTable, "Private-Key", lpm_2nd_lookup_key_select[lookups_tbl.lpm_2nd_lkp_key_select]);
            RHDATA_SET_STR(curTable, "Default-Enabled", bool2str[process_tbl.lpm_2nd_lkp_enable_default]);

            /* Specifies how to modify the 4 MSB of Large-Exact Mach (LEM)key. First the AND mask is applied and then the OR mask */
            pp_data_set_prefix(curTable, lookups_tbl.lpm_2nd_lkp_and_value, lookups_tbl.lpm_2nd_lkp_or_value, 5);

            if(lookups_tbl.lpm_public_2nd_lkp_valid) {
                if((curPublicTable = dbx_xml_child_add(curTable, "Public", depth + 3)) == NULL) {
                    SHR_ERR_EXIT(_SHR_E_FAIL, "Failed to add Public entry in :%d\n", i);
                }

                RHDATA_SET_STR(curPublicTable, "Key", lpm_public_lookup_key_select[lookups_tbl.lpm_public_2nd_lkp_key_select]);
                RHDATA_SET_STR(curPublicTable, "Default-Enabled", bool2str[process_tbl.lpm_public_2nd_lkp_enable_default]);

                if(lookups_tbl.lpm_public_2nd_lkp_key_size)
                    RHDATA_SET_HEX(curPublicTable, "Key-Size", lookups_tbl.lpm_public_2nd_lkp_key_size);
                pp_data_set_prefix(curPublicTable, lookups_tbl.lpm_public_2nd_lkp_and_value, lookups_tbl.lpm_public_2nd_lkp_or_value, 5);
            }
        }

        if (lookups_tbl.tcam_lkp_db_profile != ARAD_TCAM_ACCESS_PROFILE_INVALID ) {
            if((curTable = dbx_xml_child_add(curIndex, "TCAM-1st", depth + 2)) == NULL) {
                SHR_ERR_EXIT(_SHR_E_FAIL, "Failed to add LEM-1 program entry:%d\n", i);
            }

            RHDATA_SET_STR(curTable, "Key", tcam_lookup_key_select[lookups_tbl.tcam_lkp_key_select]);
            RHDATA_SET_HEX(curTable, "Prefix", lookups_tbl.tcam_lkp_db_profile);
        }
        if (lookups_tbl.tcam_lkp_db_profile_1 != ARAD_TCAM_ACCESS_PROFILE_INVALID ) {
            if((curTable = dbx_xml_child_add(curIndex, "TCAM-2nd", depth + 2)) == NULL) {
                SHR_ERR_EXIT(_SHR_E_FAIL, "Failed to add LEM-1 program entry:%d\n", i);
            }

            RHDATA_SET_STR(curTable, "Key", tcam_lookup_key_select[lookups_tbl.tcam_lkp_key_select_1]);
            RHDATA_SET_HEX(curTable, "Prefix", lookups_tbl.tcam_lkp_db_profile_1);
        }

        if(lookups_tbl.elk_lkp_valid) {
            if((curTable = dbx_xml_child_add(curIndex, "ELK", depth + 2)) == NULL) {
                SHR_ERR_EXIT(_SHR_E_FAIL, "Failed to add LEM-1 program entry:%d\n", i);
            }

            RHDATA_SET_HEX(curTable, "Wait-For-Reply",  lookups_tbl.elk_wait_for_reply);
            RHDATA_SET_HEX(curTable, "Opcode",          lookups_tbl.elk_opcode);
            RHDATA_SET_HEX(curTable, "KeyA-LSB-Bytes",  lookups_tbl.elk_key_a_valid_bytes);
            RHDATA_SET_HEX(curTable, "KeyC-LSB-Bytes",  lookups_tbl.elk_key_c_valid_bytes);
            RHDATA_SET_HEX(curTable, "KeyD-LSB-Bytes",  lookups_tbl.elk_key_d_lsb_valid_bytes);
            RHDATA_SET_HEX(curTable, "KeyA-MSB-Bytes",  lookups_tbl.elk_key_a_msb_valid_bytes);
            RHDATA_SET_HEX(curTable, "KeyB-MSB-Bytes",  lookups_tbl.elk_key_b_msb_valid_bytes);
            RHDATA_SET_HEX(curTable, "KeyC-MSB-Bytes",  lookups_tbl.elk_key_c_msb_valid_bytes);
            RHDATA_SET_HEX(curTable, "KeyD-MSB-Bytes",  lookups_tbl.elk_key_d_msb_valid_bytes);
            RHDATA_SET_STR(curTable, "Packet-Data-Select",  packet_data_select[lookups_tbl.elk_packet_data_select]);

            RHDATA_SET_STR(curTable, "FWD Default Enabled", bool2str[process_tbl.elk_fwd_lkp_enable_default]);
            RHDATA_SET_STR(curTable, "EXT Default Enabled", bool2str[process_tbl.elk_ext_lkp_enable_default]);

        }

        if((curTable = dbx_xml_child_add(curIndex, "Result-A", depth + 2)) == NULL) {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Failed to add Result-A for program entry:%d\n", i);
        }
        RHDATA_SET_INT(curTable, "Format",            process_tbl.result_a_format);
        if(lookups_tbl.lem_1st_lkp_valid)
            RHDATA_SET_INT(curTable, "LEM-1st-Strength",  process_tbl.include_lem_1st_in_result_a);
        if(lookups_tbl.lem_2nd_lkp_valid)
            RHDATA_SET_INT(curTable, "LEM-2nd-Strength",  process_tbl.include_lem_2nd_in_result_a);
        if(lookups_tbl.lpm_1st_lkp_valid)
            RHDATA_SET_INT(curTable, "LPM-1st-Strength",  process_tbl.include_lpm_1st_in_result_a);
        if(lookups_tbl.lpm_2nd_lkp_valid)
            RHDATA_SET_INT(curTable, "LPM-2nd-Strength",  process_tbl.include_lpm_2nd_in_result_a);
        if (lookups_tbl.tcam_lkp_db_profile != ARAD_TCAM_ACCESS_PROFILE_INVALID )
            RHDATA_SET_INT(curTable, "TCAM-1st-Strength", process_tbl.include_tcam_in_result_a);
        if (lookups_tbl.tcam_lkp_db_profile_1 != ARAD_TCAM_ACCESS_PROFILE_INVALID )
            RHDATA_SET_INT(curTable, "TCAM-2nd-Strength", process_tbl.include_tcam_1_in_result_a);
        if(lookups_tbl.elk_lkp_valid) {
            RHDATA_SET_INT(curTable, "ELK-FRW-Strength",  process_tbl.include_elk_fwd_in_result_a);
            RHDATA_SET_INT(curTable, "ELK-EXT-Strength",  process_tbl.include_elk_ext_in_result_a);
        }
        if((curTable = dbx_xml_child_add(curIndex, "Result-B", depth + 2)) == NULL) {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Failed to add Result-A for program entry:%d\n", i);
        }
        RHDATA_SET_INT(curTable, "Format",            process_tbl.result_b_format);
        if(lookups_tbl.lem_1st_lkp_valid)
            RHDATA_SET_INT(curTable, "LEM-1st-Strength",  process_tbl.include_lem_1st_in_result_b);
        if(lookups_tbl.lem_2nd_lkp_valid)
            RHDATA_SET_INT(curTable, "LEM-2nd-Strength",  process_tbl.include_lem_2nd_in_result_b);
        if(lookups_tbl.lpm_1st_lkp_valid)
            RHDATA_SET_INT(curTable, "LPM-1st-Strength",  process_tbl.include_lpm_1st_in_result_b);
        if(lookups_tbl.lpm_2nd_lkp_valid)
            RHDATA_SET_INT(curTable, "LPM-2nd-Strength",  process_tbl.include_lpm_2nd_in_result_b);
        if (lookups_tbl.tcam_lkp_db_profile != ARAD_TCAM_ACCESS_PROFILE_INVALID )
            RHDATA_SET_INT(curTable, "TCAM-1st-Strength", process_tbl.include_tcam_in_result_b);
        if (lookups_tbl.tcam_lkp_db_profile_1 != ARAD_TCAM_ACCESS_PROFILE_INVALID )
            RHDATA_SET_INT(curTable, "TCAM-2nd-Strength", process_tbl.include_tcam_1_in_result_b);
        if(lookups_tbl.elk_lkp_valid) {
            RHDATA_SET_INT(curTable, "ELK-FRW-Strength",  process_tbl.include_elk_fwd_in_result_b);
            RHDATA_SET_INT(curTable, "ELK-EXT-Strength",  process_tbl.include_elk_ext_in_result_b);
        }

        RHDATA_SET_STR(curIndex, "Learn-Key",  learn_key_select[lookups_tbl.learn_key_select]);

        if(lookups_tbl.tcam_traps_lkp_db_profile_0 != ARAD_TCAM_ACCESS_PROFILE_INVALID)
            RHDATA_SET_HEX(curIndex, "TCAM-Trap-Profile-0",  lookups_tbl.tcam_traps_lkp_db_profile_0);
        if(lookups_tbl.tcam_traps_lkp_db_profile_1 != ARAD_TCAM_ACCESS_PROFILE_INVALID)
            RHDATA_SET_HEX(curIndex, "TCAM-Trap-Profile-1",  lookups_tbl.tcam_traps_lkp_db_profile_1);
        if(lookups_tbl.enable_tcam_identification_ieee_1588)
            RHDATA_SET_STR(curIndex, "Enable-TCAM-Identification-IEEE1588", "Yes");
        if(lookups_tbl.enable_tcam_identification_oam)
            RHDATA_SET_STR(curIndex, "Enable-TCAM-Identification-OAM",  "Yes");

        if((curTable = dbx_xml_child_add(curIndex, "Misc", depth + 2)) == NULL) {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Failed to add Result-A for program entry:%d\n", i);
        }

        RHDATA_SET_INT(curTable, "Trap-Code",                   process_tbl.not_found_trap_code);
        RHDATA_SET_INT(curTable, "Trap-Strength",               process_tbl.not_found_trap_strength);
        RHDATA_SET_INT(curTable, "Snoop-Strength",              process_tbl.not_found_snoop_strength);

        RHDATA_SET_INT(curTable, "SA-LKP-Result-Select",        process_tbl.sa_lkp_result_select);
        RHDATA_SET_INT(curTable, "Apply-Forward-Result-A",      process_tbl.apply_fwd_result_a);
        RHDATA_SET_INT(curTable, "SA-LKP-Process-Enable",       process_tbl.sa_lkp_process_enable);

        RHDATA_SET_STR(curTable, "Eth-Default-Procedure", ethernet_default_procedure[process_tbl.procedure_ethernet_default]);
        RHDATA_SET_STR(curTable, "Unknown-Address",       unknown_address_control[process_tbl.unknown_address]);

        RHDATA_SET_STR(curTable, "Hair-Pin-Filter",    bool2str[process_tbl.enable_hair_pin_filter]);
        RHDATA_SET_STR(curTable, "RPF-Check",          bool2str[process_tbl.enable_rpf_check]);
        RHDATA_SET_STR(curTable, "MC-Bridge-Fallback", bool2str[process_tbl.compatible_mc_bridge_fallback]);
        RHDATA_SET_STR(curTable, "LSR-P2P-Service",    bool2str[process_tbl.enable_lsr_p2p_service]);

        dbx_xml_node_end(curIndex, depth + 1);
    }

    dbx_xml_node_end(curSection, depth);

exit:
    SHR_FUNC_EXIT;
}

static int pp_dump_flp(int unit, void *curStage, int depth)
{
    uint32         res = CMD_OK;

    /* Create FLP sub-sections: Program-Selection */
    res = pp_dump_flp_selection(unit, curStage, depth);

    /* Create FLP sub-sections: Programs */
    res = pp_dump_flp_programs(unit, curStage, depth);

    pp_dump_tables(unit, curStage, SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP, depth);

    return res;
}

static int
dpp_export_pp_table_priority_get(int unit, int prog_id, SOC_DPP_DBAL_PROGRAM_KEYS key_id, SOC_DPP_DBAL_PHYSICAL_DB_TYPES  physical_db_type, int *priority)
{
    ARAD_PP_IHB_FLP_PROCESS_TBL_DATA flp_process_tbl;

    if (!arad_pp_ihb_flp_process_tbl_get_unsafe(unit, prog_id, &flp_process_tbl)) {
        return CMD_FAIL;
    }

    switch (physical_db_type) {
    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM:
        if (flp_process_tbl.include_lem_1st_in_result_a > 0) {
            *priority = flp_process_tbl.include_lem_1st_in_result_a;
        }
        else if (flp_process_tbl.include_lem_2nd_in_result_a > 0) {
            *priority = flp_process_tbl.include_lem_2nd_in_result_a;
        }
        else if (flp_process_tbl.include_lem_1st_in_result_b > 0) {
            *priority = flp_process_tbl.include_lem_1st_in_result_b;
        }
        else if (flp_process_tbl.include_lem_2nd_in_result_b > 0) {
            *priority = flp_process_tbl.include_lem_2nd_in_result_b;
        }

        break;
    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM:
        if (flp_process_tbl.include_tcam_in_result_a > 0) {
            *priority = flp_process_tbl.include_tcam_in_result_a;
        }
        else {
            *priority = flp_process_tbl.include_tcam_in_result_b;
        }
        break;
    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KBP:

        if (flp_process_tbl.include_elk_fwd_in_result_a > 0) {
            *priority = flp_process_tbl.include_elk_fwd_in_result_a;
        }
        else if (flp_process_tbl.include_elk_ext_in_result_a > 0) {
            *priority = flp_process_tbl.include_elk_ext_in_result_a;
        }
        else if (flp_process_tbl.include_elk_fwd_in_result_b > 0) {
            *priority = flp_process_tbl.include_elk_fwd_in_result_b;
        }
        else if (flp_process_tbl.include_elk_ext_in_result_b > 0) {
            *priority = flp_process_tbl.include_elk_ext_in_result_b;
        }

        break;
    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KAPS:
        if (flp_process_tbl.include_lpm_1st_in_result_a > 0) {
            *priority = flp_process_tbl.include_lpm_1st_in_result_a;
        }
        else if (flp_process_tbl.include_lpm_2nd_in_result_a > 0) {
            *priority = flp_process_tbl.include_lpm_2nd_in_result_a;
        }
        else if (flp_process_tbl.include_lpm_1st_in_result_b > 0) {
            *priority = flp_process_tbl.include_lpm_1st_in_result_b;
        }
        else if (flp_process_tbl.include_lpm_2nd_in_result_b > 0) {
            *priority = flp_process_tbl.include_lpm_2nd_in_result_b;
        }

        break;
    default:
        break;
    }

    return CMD_OK;
}

static int
dpp_dump_stage_table_db(int unit, SOC_PPC_FP_DATABASE_STAGE  stage, dpp_export_pp_stage_info_t *stage_info)
{
    SOC_DPP_DBAL_TABLE_INFO table;
    int program_id, qual, action, table_index, i;

    /* Tables info is taken from DBAL */
    for (table_index = 0; table_index < SOC_DPP_DBAL_SW_NOF_TABLES; table_index++) {
        if (sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_index, &table) != 0) {
            return CMD_FAIL;
        }

        if (!table.is_table_initiated || (table.table_programs[0].stage != stage))
            continue;

        sal_strncpy(stage_info->tables[table_index].name, table.table_name, DPP_EXPORT_CHAR_MAX_LENGTH - 1);  /* Using same name as in DBAL */
        sal_strncpy(stage_info->tables[table_index].database, arad_pp_dbal_physical_db_to_string(table.physical_db_type), DPP_EXPORT_CHAR_MAX_LENGTH - 1);
        for (qual = 0; qual < table.nof_qualifiers; qual++) {
            sal_strncpy(stage_info->tables[table_index].keys[qual].name, SOC_PPC_FP_QUAL_TYPE_to_string(table.qual_info[qual].qual_type), DPP_EXPORT_CHAR_MAX_LENGTH - 1);
        }

        if (table.nof_actions) {
            for (action = 0; action < table.nof_actions; action++) {
                sal_strncpy(stage_info->tables[table_index].actions[action].name, SOC_PPC_FP_ACTION_TYPE_to_string(table.action[action]), DPP_EXPORT_CHAR_MAX_LENGTH - 1);
            }
        }
        else {
            sal_strcpy(stage_info->tables[table_index].actions[0].name, "ClassDestSet");
        }

        dpp_export_pp_table_priority_get(unit,table.table_programs[0].program_id, /* Klear - not unique per table, taking first! */
                                              table.table_programs[0].key_id,
                                              table.physical_db_type,
                                              &stage_info->tables[table_index].priority);

        stage_info->tables[table_index].is_static = 1; /* All Klear tables are static (not configured through Arrkis) */

        /* Programs information is taken from each table */
        for (i = 0; i < table.nof_table_programs; i++) {
            /* Choose only relevant for stage */
            if (table.table_programs[i].stage == stage) {
                program_id = table.table_programs[i].program_id; /* program ID refers to the cam selection line*/
                if(ISEMPTY(stage_info->bundles[program_id].name))
                    cli_out("No program name for program:%d", program_id);
                stage_info->bundles[program_id].tables[stage_info->bundles[program_id].nof_tables].lookup = table.table_programs[i].lookup_number;
                stage_info->bundles[program_id].tables[stage_info->bundles[program_id].nof_tables].table = &stage_info->tables[table_index]; /* Link to the current table */
                stage_info->bundles[program_id].nof_tables++;
                stage_info->bundles[program_id].is_static = 1;
            }
        }

    }

    return CMD_OK;
}

static void dpp_attribute_set(rhlist_t *qual_list, char *attribute, uint32 data, uint32 mask, int data_size)
{
    int i;
    dpp_presel_qual_info_t *presel_qual;
    rhhandle_t temp = NULL;
    uint32 number = 0;

    for(i = 0; i < data_size; i++)
        number |= 1 << i;

    if(number == mask)
        return;

    if(utilex_rhlist_entry_add_tail(qual_list, attribute, RHID_TO_BE_GENERATED, &temp) == -SOC_SAND_ERR) {
        /* No more place any more - return with what you have until now */
        goto exit;
    }

    presel_qual = temp;

    presel_qual->data = data;
    presel_qual->mask = (((~mask) & number) == number) ? SOC_SAND_U32_MAX : ((~mask) & number);
    presel_qual->size = data_size;

exit:
    return;
}

/* Ignoring for now result b in case we have both a and b */

static const char *apptype_text[bcmFieldAppTypeCount] = BCM_FIELD_APPTYPE_STRINGS;

static void dpp_dump_flp_get_apptype(int unit, int program_index, char *name)
{
    bcm_field_AppType_t bcmAppType;

    if(_bcm_dpp_field_app_type_ppd_to_bcm(unit, program_index, &bcmAppType) != BCM_E_PARAM)
        strcpy(name, apptype_text[bcmAppType]);
    else
        strcpy(name, "Unknown");
    return;
}

static uint32 dpp_dump_flp_selection(int unit, int core, dpp_export_pp_stage_info_t *stage)
{
    uint32 res = CMD_OK;
    ARAD_PP_IHB_FLP_PROGRAM_SELECTION_CAM_TBL_DATA    flp_prog_selection;
    int i;
    char *program_name;
    uint8 program_id;
    uint64 flp_selects;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, READ_IHP_DBG_FLP_PROGRAM_SELECTION_CAM_LINEr(unit, core, &flp_selects));

    for(i = 0; i < SOC_DPP_DEFS_GET(unit, nof_flp_program_selection_lines); i++) {
        sal_memset(&flp_prog_selection, 0, sizeof(ARAD_PP_IHB_FLP_PROGRAM_SELECTION_CAM_TBL_DATA));
        if (!arad_pp_ihb_flp_program_selection_cam_tbl_get_unsafe(unit, i, &flp_prog_selection))
            return res;

        if(!flp_prog_selection.valid)
            continue;

        res = sw_state_access[unit].dpp.soc.arad.pp.fec.flp_progs_mapping.get(unit, flp_prog_selection.program, &program_id);
        if(soc_sand_get_error_code_from_error_word(res) != SOC_SAND_OK)
            program_name = "Unknown Program";
        else
            program_name = arad_pp_flp_prog_id_to_prog_name(unit, program_id);

        sal_sprintf(stage->presels[i].name, "CS-%02d-%s", i, program_name + strlen("PROG_FLP_"));
        stage->presels[i].bundle = &stage->bundles[flp_prog_selection.program];
        stage->presels[i].is_static = 1;

        if(i < 32) {
            if(SOC_SAND_GET_BIT(COMPILER_64_LO(flp_selects), i))
                stage->presels[i].selected = 1;
        }
        else {
            if(SOC_SAND_GET_BIT((COMPILER_64_HI(flp_selects)), (i - 32)))
                stage->presels[i].selected = 1;
        }

        dpp_dump_flp_get_apptype(unit, flp_prog_selection.program, stage->presels[i].app_type);
        if((stage->presels[i].qual_list = utilex_rhlist_create("qual_list", sizeof(dpp_presel_qual_info_t), 0)) == NULL) {
            continue;
        }
        dpp_attribute_set(stage->presels[i].qual_list, "Parser-Leaf-Context",         flp_prog_selection.parser_leaf_context, flp_prog_selection.parser_leaf_context_mask, 4);
        dpp_attribute_set(stage->presels[i].qual_list, "Port-Profile",                flp_prog_selection.port_profile, flp_prog_selection.port_profile_mask, 5);
        dpp_attribute_set(stage->presels[i].qual_list, "PTC-Profile",                 flp_prog_selection.ptc_profile, flp_prog_selection.ptc_profile_mask, 2);
        dpp_attribute_set(stage->presels[i].qual_list, "Packet-Format-Code",          flp_prog_selection.packet_format_code, flp_prog_selection.packet_format_code_mask, 6);
        dpp_attribute_set(stage->presels[i].qual_list, "Packet-Format-Qualifier",     flp_prog_selection.forwarding_header_qualifier, flp_prog_selection.forwarding_header_qualifier_mask, 11);
        dpp_attribute_set(stage->presels[i].qual_list, "Forwarding-Code",             flp_prog_selection.forwarding_code, flp_prog_selection.forwarding_code_mask, 4);
        dpp_attribute_set(stage->presels[i].qual_list, "Forwarding-Offset-Index",     flp_prog_selection.forwarding_offset_index, flp_prog_selection.forwarding_offset_index_mask, 3);
        dpp_attribute_set(stage->presels[i].qual_list, "L-3-VPN-Default-Routing",     flp_prog_selection.l_3_vpn_default_routing, flp_prog_selection.l_3_vpn_default_routing_mask, 1);
        dpp_attribute_set(stage->presels[i].qual_list, "Trill-MC",                    flp_prog_selection.trill_mc, flp_prog_selection.trill_mc_mask, 1);
        dpp_attribute_set(stage->presels[i].qual_list, "Packet-Is-Compatible-MC",     flp_prog_selection.packet_is_compatible_mc, flp_prog_selection.packet_is_compatible_mc_mask, 1);
        dpp_attribute_set(stage->presels[i].qual_list, "In-RIF-UC-RPF-Enable",        flp_prog_selection.in_rif_uc_rpf_enable, flp_prog_selection.in_rif_uc_rpf_enable_mask, 1);
        dpp_attribute_set(stage->presels[i].qual_list, "LL-Is-Arp",                   flp_prog_selection.ll_is_arp, flp_prog_selection.ll_is_arp_mask, 1);
        dpp_attribute_set(stage->presels[i].qual_list, "Elk-Status",                  flp_prog_selection.elk_status, flp_prog_selection.elk_status_mask, 1);
        dpp_attribute_set(stage->presels[i].qual_list, "Cos-Profile",                 flp_prog_selection.cos_profile, flp_prog_selection.cos_profile_mask,6);
        dpp_attribute_set(stage->presels[i].qual_list, "Service-Type",                flp_prog_selection.service_type, flp_prog_selection.service_type_mask, 3);
        dpp_attribute_set(stage->presels[i].qual_list, "VT-Processing-Profile",       flp_prog_selection.vt_processing_profile, flp_prog_selection.vt_processing_profile_mask, 3);
        dpp_attribute_set(stage->presels[i].qual_list, "VT-Lookup-0-Found",           flp_prog_selection.vt_lookup_0_found, flp_prog_selection.vt_lookup_0_found_mask, 1);
        dpp_attribute_set(stage->presels[i].qual_list, "VT-Lookup-1-Found",           flp_prog_selection.vt_lookup_1_found, flp_prog_selection.vt_lookup_1_found_mask, 1);
        dpp_attribute_set(stage->presels[i].qual_list, "TT-Processing-Profile",       flp_prog_selection.tt_processing_profile, flp_prog_selection.tt_processing_profile_mask, 3);
        dpp_attribute_set(stage->presels[i].qual_list, "TT-Lookup-0-Found",           flp_prog_selection.tt_lookup_0_found, flp_prog_selection.tt_lookup_0_found_mask, 1);
        dpp_attribute_set(stage->presels[i].qual_list, "TT-Lookup-1-Found",           flp_prog_selection.tt_lookup_1_found, flp_prog_selection.tt_lookup_1_found_mask, 1);
        /* Jericho new fields */
        dpp_attribute_set(stage->presels[i].qual_list, "Forwarding-Offset-Index-Ext", flp_prog_selection.forwarding_offset_index_ext, flp_prog_selection.forwarding_offset_index_ext_mask, 2);
        dpp_attribute_set(stage->presels[i].qual_list, "CPU-Trap-Code",               flp_prog_selection.cpu_trap_code, flp_prog_selection.cpu_trap_code_mask, 8);
        dpp_attribute_set(stage->presels[i].qual_list, "In-LIF-Profile",              flp_prog_selection.in_lif_profile, flp_prog_selection.in_lif_profile_mask, 4);
        dpp_attribute_set(stage->presels[i].qual_list, "LLVP-Incoming-Tag-Structure", flp_prog_selection.llvp_incoming_tag_structure, flp_prog_selection.llvp_incoming_tag_structure_mask, 4);
        /* uint32 in_rif_profile; */ /* Handled in arad_tbl_access internally */
        dpp_attribute_set(stage->presels[i].qual_list, "Forwarding-Plus-1-Header-Qualifier", flp_prog_selection.forwarding_plus_1_header_qualifier, flp_prog_selection.forwarding_plus_1_header_qualifier_mask, 11);
        dpp_attribute_set(stage->presels[i].qual_list, "TT-Code",                     flp_prog_selection.tunnel_termination_code, flp_prog_selection.tunnel_termination_code_mask, 4);
        dpp_attribute_set(stage->presels[i].qual_list, "Qualifier-O",                 flp_prog_selection.qualifier_0, flp_prog_selection.qualifier_0_mask, 3);
        dpp_attribute_set(stage->presels[i].qual_list, "In-LIF-Data-Index",           flp_prog_selection.in_lif_data_index, flp_prog_selection.in_lif_data_index_mask, 2);
        /* uint32 in_rif_profile_mask; */ /* Handled in arad_tbl_access internally */
        dpp_attribute_set(stage->presels[i].qual_list, "In-LIF-Data-MSB",             flp_prog_selection.in_lif_data_msb, flp_prog_selection.in_lif_data_msb_mask, 5);
    }

    COMPILER_64_ALLONES(flp_selects);
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  44,  exit, ARAD_REG_ACCESS_ERR,WRITE_IHP_DBG_FLP_PROGRAM_SELECTION_CAM_LINEr(unit, core, flp_selects));

    return res;
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in dpp_dump_flp_selection()", 0, 0);
}

static int dpp_dump_flp_programs(int unit, int core, dpp_export_pp_stage_info_t *stage)
{
    uint32         res = CMD_OK;
    int i;
    char *program_name;
    uint8 program_id;
    uint32 progs_bmp;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IHP_DBG_FLP_SELECTED_PROGRAMr, core, 0, DBG_FLP_SELECTED_PROGRAMf, &progs_bmp));

    for(i = 0; i < SOC_DPP_DEFS_GET(unit, nof_flp_programs); i++) {
        res = sw_state_access[unit].dpp.soc.arad.pp.fec.flp_progs_mapping.get(unit, i, &program_id);
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

        if(program_id == ARAD_PP_FLP_MAP_PROG_NOT_SET)
            continue;

        program_name = arad_pp_flp_prog_id_to_prog_name(unit, program_id);
        sal_strncpy(stage->bundles[i].name, program_name, DPP_EXPORT_CHAR_MAX_LENGTH - 1);
        stage->bundles[i].is_static = 1;
        if (SOC_SAND_GET_BIT(progs_bmp, i))
            stage->bundles[i].selected = 1;
    }

    /* Set all bits as ones to clear it.
     */
    if(SOC_DPP_DEFS_GET(unit, nof_flp_programs) < 32) {
        progs_bmp = (1 << SOC_DPP_DEFS_GET(unit, nof_flp_programs)) - 1;
    } else {
        progs_bmp = 0xffffffff;
    }
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IHP_DBG_FLP_SELECTED_PROGRAMr, core, 0, DBG_FLP_SELECTED_PROGRAMf, progs_bmp));

exit:
    return res;
}

int dpp_dump_flp(int unit, int core, dpp_export_pp_stage_info_t *stage)
{
    uint32         res = CMD_OK;

    sal_strcpy(stage->name, "IRPP.FLP");
    /* Create FLP sub-sections: Program-Selection */
    res = dpp_dump_flp_selection(unit, core, stage);

    /* Create FLP sub-sections: Programs */
    res = dpp_dump_flp_programs(unit, core, stage);

    res = dpp_dump_stage_table_db(unit, SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP, stage);

    return res;
}

static uint32 dpp_dump_vt_selection(int unit, int core, dpp_export_pp_stage_info_t *stage)
{
    uint32 res = CMD_OK;
    ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA    vt_prog_selection;
    int i;
    char *program_name;
    uint8 program_id;

    for(i = 0; i < SOC_DPP_DEFS_GET(unit, nof_vtt_program_selection_lines); i++) {
        sal_memset(&vt_prog_selection, 0, sizeof(ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA));
        if (!arad_pp_ihp_isem_1st_program_selection_cam_tbl_get_unsafe(unit, i, &vt_prog_selection))
            return res;

        if(!vt_prog_selection.valid)
            continue;

        arad_pp_isem_access_program_sel_line_to_program_id(unit, vt_prog_selection.llvp_prog_sel, 1, &program_id);
        program_name = arad_pp_isem_access_print_vt_program_data(unit, program_id, 0);

        sal_sprintf(stage->presels[i].name, "CS-%02d-%s", i, program_name + strlen("PROG_VT_"));
        stage->presels[i].bundle = &stage->bundles[vt_prog_selection.llvp_prog_sel];
        stage->presels[i].is_static = 1;
        if((stage->presels[i].qual_list = utilex_rhlist_create("qual_list", sizeof(dpp_presel_qual_info_t), 0)) == NULL) {
            continue;
        }
        dpp_attribute_set(stage->presels[i].qual_list, "Packet-Format-Code", vt_prog_selection.packet_format_code, vt_prog_selection.packet_format_code_mask, 6);
        dpp_attribute_set(stage->presels[i].qual_list, "Leaf-Context", vt_prog_selection.parser_leaf_context, vt_prog_selection.parser_leaf_context_mask, 4);
        dpp_attribute_set(stage->presels[i].qual_list, "Packet-Format-Qualifier-1", vt_prog_selection.packet_format_qualifier_1, vt_prog_selection.packet_format_qualifier_1_mask, 11);
        dpp_attribute_set(stage->presels[i].qual_list, "Packet-Format-Qualifier-2", vt_prog_selection.packet_format_qualifier_2, vt_prog_selection.packet_format_qualifier_2_mask, 11);
        dpp_attribute_set(stage->presels[i].qual_list, "PTC-VT-Profile", vt_prog_selection.ptc_vt_profile , vt_prog_selection.ptc_vt_profile_mask, 2);

        if(SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_ARADPLUS)
            dpp_attribute_set(stage->presels[i].qual_list, "In-PP-Port-VT-Profile", vt_prog_selection.in_pp_port_vt_profile, vt_prog_selection.in_pp_port_vt_profile_mask, 3);
        else {
            dpp_attribute_set(stage->presels[i].qual_list, "In-PP-Port-VT-Profile", vt_prog_selection.in_pp_port_vt_profile, vt_prog_selection.in_pp_port_vt_profile_mask, 5);
            dpp_attribute_set(stage->presels[i].qual_list, "Packet-Format-Qualifier-0", vt_prog_selection.packet_format_qualifier_0, vt_prog_selection.packet_format_qualifier_0_mask, 3);
            dpp_attribute_set(stage->presels[i].qual_list, "Incoming-Tag-Structure", vt_prog_selection.incoming_tag_structure, vt_prog_selection.incoming_tag_structure_mask, 4);
        }
    }

    return res;
}

static int dpp_dump_vt_programs(int unit, int core, dpp_export_pp_stage_info_t *stage)
{
    uint32         res = CMD_OK;
    int i;
    char *program_name;
    uint8 program_id;
    uint32 progs_bmp;
    uint64 reg_val64;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_IHP_VTT_PROGRAM_ENCOUNTEREDr(unit, core, &reg_val64));
    progs_bmp = soc_reg64_field32_get(unit, IHP_VTT_PROGRAM_ENCOUNTEREDr, reg_val64, SOC_IS_JERICHO(unit) ? VLAN_TRANSLATION_PROGRAM_ENCOUNTEREDf : VTT_PROGRAM_FIRST_LOOKUPf);
    COMPILER_64_ZERO(reg_val64);
    soc_reg64_field32_set(unit, IHP_VTT_PROGRAM_ENCOUNTEREDr, &reg_val64, SOC_IS_JERICHO(unit) ? VLAN_TRANSLATION_PROGRAM_ENCOUNTEREDf : VTT_PROGRAM_FIRST_LOOKUPf, SOC_SAND_U32_MAX);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_IHP_VTT_PROGRAM_ENCOUNTEREDr(unit, core, reg_val64));

    for(i = 0; i < SOC_DPP_DEFS_GET(unit, nof_vtt_programs); i++) {
        arad_pp_isem_access_program_sel_line_to_program_id(unit, i, 1, &program_id);
        if(program_id == 0xFF)
            continue;
        program_name = arad_pp_isem_access_print_vt_program_data(unit, program_id, 0);

        sal_strncpy(stage->bundles[i].name, program_name, DPP_EXPORT_CHAR_MAX_LENGTH - 1);
        stage->bundles[i].is_static = 1;
        if (SOC_SAND_GET_BIT(progs_bmp, i))
            stage->bundles[i].selected = 1;
    }

    return res;
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in dpp_dump_vt_programs()", 0, 0);
}

int dpp_dump_vt(int unit, int core, dpp_export_pp_stage_info_t *stage)
{
    uint32         res = CMD_OK;

    sal_strcpy(stage->name, "IRPP.VT");
    /* Create FLP sub-sections: Program-Selection */
    res = dpp_dump_vt_selection(unit, core, stage);

    /* Create FLP sub-sections: Programs */
    res = dpp_dump_vt_programs(unit, core, stage);

    res = dpp_dump_stage_table_db(unit, SOC_PPC_FP_DATABASE_STAGE_INGRESS_VT, stage);

    return res;
}

static uint32 dpp_dump_tt_selection(int unit, int core, dpp_export_pp_stage_info_t *stage)
{
    uint32 res = CMD_OK;
    ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA    tt_prog_selection;
    int i;
    char *program_name;
    uint8 program_id;

    for(i = 0; i < SOC_DPP_DEFS_GET(unit, nof_vtt_program_selection_lines); i++) {
        sal_memset(&tt_prog_selection, 0, sizeof(ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA));
        if (!arad_pp_ihp_isem_2nd_program_selection_cam_tbl_get_unsafe(unit, i, &tt_prog_selection))
            return res;

        if(!tt_prog_selection.valid)
            continue;

        arad_pp_isem_access_program_sel_line_to_program_id(unit, tt_prog_selection.llvp_prog_sel, 0, &program_id);
        program_name = arad_pp_isem_access_print_tt_program_data(unit, program_id, 0);

        sal_sprintf(stage->presels[i].name, "CS-%02d-%s", i, program_name + strlen("PROG_TT_"));
        stage->presels[i].bundle = &stage->bundles[tt_prog_selection.llvp_prog_sel];
        stage->presels[i].is_static = 1;
        if((stage->presels[i].qual_list = utilex_rhlist_create("qual_list", sizeof(dpp_presel_qual_info_t), 0)) == NULL) {
            continue;
        }
        dpp_attribute_set(stage->presels[i].qual_list, "Packet-Format-Code", tt_prog_selection.packet_format_code, tt_prog_selection.packet_format_code_mask, 6);
        dpp_attribute_set(stage->presels[i].qual_list, "Leaf-Context", tt_prog_selection.parser_leaf_context, tt_prog_selection.parser_leaf_context_mask, 4);
        dpp_attribute_set(stage->presels[i].qual_list, "Packet-Format-Qualifier-1", tt_prog_selection.packet_format_qualifier_1, tt_prog_selection.packet_format_qualifier_1_mask, 11);
        dpp_attribute_set(stage->presels[i].qual_list, "Packet-Format-Qualifier-2", tt_prog_selection.packet_format_qualifier_2, tt_prog_selection.packet_format_qualifier_2_mask, 11);
        dpp_attribute_set(stage->presels[i].qual_list, "PTC-TT-Profile", tt_prog_selection.ptc_tt_profile , tt_prog_selection.ptc_tt_profile_mask, 2);
        dpp_attribute_set(stage->presels[i].qual_list, "My-MAC", tt_prog_selection.my_mac, tt_prog_selection.my_mac_mask, 1);
        dpp_attribute_set(stage->presels[i].qual_list, "My-BMAC-MC", tt_prog_selection.my_b_mac_mc, tt_prog_selection.my_b_mac_mc_mask, 1);
        dpp_attribute_set(stage->presels[i].qual_list, "DA-Is-All-R-Bridges", tt_prog_selection.da_is_all_r_bridges, tt_prog_selection.da_is_all_r_bridges_mask, 1);
        dpp_attribute_set(stage->presels[i].qual_list, "TCAM-Lookup-Match", tt_prog_selection.tcam_lookup_match, tt_prog_selection.tcam_lookup_match_mask, 1);
        dpp_attribute_set(stage->presels[i].qual_list, "Packet-Is-Compatible-MC", tt_prog_selection.packet_is_compatible_mc, tt_prog_selection.packet_is_compatible_mc_mask, 1);
        dpp_attribute_set(stage->presels[i].qual_list, "ISA-Lookup-Found", tt_prog_selection.isa_lookup_found, tt_prog_selection.isa_lookup_found_mask, 1);
        dpp_attribute_set(stage->presels[i].qual_list, "ISB-Lookup-Found", tt_prog_selection.isb_lookup_found, tt_prog_selection.isb_lookup_found_mask, 1);
        dpp_attribute_set(stage->presels[i].qual_list, "VLAN-Is-Designated", tt_prog_selection.vlan_is_designated, tt_prog_selection.vlan_is_designated_mask, 1);
        dpp_attribute_set(stage->presels[i].qual_list, "VT-Processing-Profile", tt_prog_selection.vt_processing_profile, tt_prog_selection.vt_processing_profile_mask, 3);

        if(SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_ARADPLUS) {
            dpp_attribute_set(stage->presels[i].qual_list, "In-PP-Port-TT-Profile", tt_prog_selection.in_pp_port_tt_profile, tt_prog_selection.in_pp_port_tt_profile_mask, 3);
            dpp_attribute_set(stage->presels[i].qual_list, "Packet-Format-Qualifier-3-Outer-VID", tt_prog_selection.packet_format_qualifier_3_outer_vid_bits, tt_prog_selection.packet_format_qualifier_3_outer_vid_bits_mask, 3);
        }
        else { /* Jericho / QMX */
            dpp_attribute_set(stage->presels[i].qual_list, "In-PP-Port-TT-Profile", tt_prog_selection.in_pp_port_tt_profile, tt_prog_selection.in_pp_port_tt_profile_mask, 5);
            dpp_attribute_set(stage->presels[i].qual_list, "Packet-Format-Qualifier-3", tt_prog_selection.packet_format_qualifier_3, tt_prog_selection.packet_format_qualifier_3_mask, 11);
            dpp_attribute_set(stage->presels[i].qual_list, "VT-In-LIF-Profile", tt_prog_selection.vt_in_lif_profile, tt_prog_selection.vt_in_lif_profile_mask, 4);
        }
    }

    return res;
}

static int dpp_dump_tt_programs(int unit, int core, dpp_export_pp_stage_info_t *stage)
{
    uint32         res = CMD_OK;
    int i;
    char *program_name;
    uint8 program_id;
    uint32 progs_bmp;
    uint64 reg_val64;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_IHP_VTT_PROGRAM_ENCOUNTEREDr(unit, core, &reg_val64));
    progs_bmp = soc_reg64_field32_get(unit, IHP_VTT_PROGRAM_ENCOUNTEREDr, reg_val64, SOC_IS_JERICHO(unit) ? TUNNEL_TERMINATION_PROGRAM_ENCOUNTEREDf : VTT_PROGRAM_SECOND_LOOKUPf);
    COMPILER_64_ZERO(reg_val64);
    soc_reg64_field32_set(unit, IHP_VTT_PROGRAM_ENCOUNTEREDr, &reg_val64, SOC_IS_JERICHO(unit) ? TUNNEL_TERMINATION_PROGRAM_ENCOUNTEREDf : VTT_PROGRAM_SECOND_LOOKUPf, SOC_SAND_U32_MAX);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_IHP_VTT_PROGRAM_ENCOUNTEREDr(unit, core, reg_val64));

    for(i = 0; i < SOC_DPP_DEFS_GET(unit, nof_vtt_programs); i++) {
        arad_pp_isem_access_program_sel_line_to_program_id(unit, i, 0, &program_id);
        if(program_id == 0xFF)
            continue;
        program_name = arad_pp_isem_access_print_tt_program_data(unit, program_id, 0);

        sal_strncpy(stage->bundles[i].name, program_name, DPP_EXPORT_CHAR_MAX_LENGTH - 1);
        stage->bundles[i].is_static = 1;
        if (SOC_SAND_GET_BIT(progs_bmp, i))
            stage->bundles[i].selected = 1;
    }

    return res;
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in dpp_dump_tt_programs()", 0, 0);
}

int dpp_dump_tt(int unit, int core, dpp_export_pp_stage_info_t *stage)
{
    uint32         res = CMD_OK;

    sal_strcpy(stage->name, "IRPP.TT");
    /* Create FLP sub-sections: Program-Selection */
    res = dpp_dump_tt_selection(unit, core, stage);

    /* Create FLP sub-sections: Programs */
    res = dpp_dump_tt_programs(unit, core, stage);

    res = dpp_dump_stage_table_db(unit, SOC_PPC_FP_DATABASE_STAGE_INGRESS_TT, stage);

    return res;
}

static int next_segment_map[PARSER_PROGRAM_SEGMENT_NUM] = {0};

static shr_error_e pp_dump_parser_print_single(int unit, void *curTop, int depth, int base, int macro, int first, int last, int pfc, int plc)
{
    char *entry_n;
    char entry_str[RHNAME_MAX_SIZE];
    void *cur;

    SHR_FUNC_INIT_VARS(unit);

    if(first == last)
        sprintf(entry_str, "Entry:%d", first);
    else
        sprintf(entry_str, "Entry:%d-%d", first, last);

    if((cur = dbx_xml_child_add(curTop, entry_str, depth)) == NULL) {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Failed to add Program Selection section to IRPP Parser\n");
    }

    if(base == PARSER_PROGRAM_END)
        entry_n = "End";
    else if(base == PARSER_PROGRAM_FIN)
        entry_n = "Fin";
    else
        entry_n = "Continue";
    RHDATA_SET_STR(cur, "Type", entry_n);

    if(base != PARSER_PROGRAM_END) {
        sprintf(entry_str, "%s(%d)", arad_parser_get_macro_str(unit, macro), macro);
        RHDATA_SET_STR(cur, "Macro",  entry_str);
    }

    if((base == PARSER_PROGRAM_END) || (base == PARSER_PROGRAM_FIN)) {
        sprintf(entry_str, "%s(0x%X)", dpp_parser_pfc_string_by_hw(unit, pfc), pfc);
        RHDATA_SET_STR(cur, "PFC", entry_str);
        sprintf(entry_str, "%s(0x%X)", dpp_parser_plc_string_by_hw(unit, pfc, plc), plc);
        RHDATA_SET_STR(cur, "PLC", entry_str);
    }
    else
    {
        RHDATA_SET_STR(cur, "Next-Segment", parser_segment_name[base >> 2]);
    }
exit:
    SHR_FUNC_EXIT;
}

static void pp_dump_parser_print_entries(int unit, void *curTop, int depth, int offset, int base, int macro, int first, int last)
{
    if((base == PARSER_PROGRAM_END) || (base == PARSER_PROGRAM_FIN)) {
        uint32 first_pfc = PP_INVLAID_VALUE, curr_pfc;
        uint32 first_plc = PP_INVLAID_VALUE, curr_plc; /* parser leaf context */
        int first_addr = 0;
        int i;
        SOC_SAND_OUT ARAD_PP_IHP_PACKET_FORMAT_TABLE_TBL_DATA packet_format;

        for(i = first; i <= last; i++)  {
            if(i + offset >= 0x800) {
                cli_out("Bad address on offset:0x%04x index:%d\n", offset, i);
                continue;
            }
            arad_pp_ihp_packet_format_table_tbl_get_unsafe(unit, i + offset, &packet_format);
            curr_pfc = packet_format.packet_format_code;
            curr_plc = packet_format.parser_leaf_context;
            if(i == first) {
                /* Beginning of new range */
                first_pfc = curr_pfc;
                first_plc = curr_plc;
                first_addr = i;
            }
            else if((first_pfc != curr_pfc) || (first_plc != curr_plc)) {
                /* Print previous pfc&plc */
                pp_dump_parser_print_single(unit, curTop, depth, base, macro, first_addr, i - 1, first_pfc, first_plc);
                first_pfc = curr_pfc;
                first_plc = curr_plc;
                first_addr = i;
            }
            if(i == last) {
                /* Last element - print pfc&plc anyway */
                pp_dump_parser_print_single(unit, curTop, depth, base, macro, first_addr, last, first_pfc, curr_plc);
            }
        }
    }
    else
    {
        pp_dump_parser_print_single(unit, curTop, depth, base, macro, first, last, 0, 0);
        next_segment_map[base >> 2]++;
    }

    return;
}

static shr_error_e pp_dump_parser_programs(int unit, void *curTop, int depth)
{
    int i_entry, i_seg;
    SOC_SAND_OUT ARAD_PP_IHP_PARSER_PROGRAM_TBL_DATA  tbl_data;
    int first = PP_INVLAID_VALUE, last = 0;
    int prev_base = PP_INVLAID_VALUE, curr_base;
    int prev_macro = PP_INVLAID_VALUE, curr_macro;
    int offset;
    void *curSegment;

    SHR_FUNC_INIT_VARS(unit);

    for(i_seg = 0; i_seg < PARSER_PROGRAM_SEGMENT_NUM; i_seg++)
    {
        if((curSegment = dbx_xml_child_add(curTop, parser_segment_name[i_seg], depth)) == NULL) {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Failed to add Program Selection section to IRPP Parser\n");
        }

        offset = i_seg * PARSER_PROGRAM_SEGMENT_SIZE;

        for(i_entry = 0; i_entry < PARSER_PROGRAM_SEGMENT_SIZE; i_entry++) {
            arad_pp_ihp_parser_program_tbl_get_unsafe(unit, 0, offset + i_entry, &tbl_data);

            curr_base = tbl_data.next_addr_base;
            curr_macro = tbl_data.macro_sel;
            if(i_entry == 0) {
                /* First Element in segment - never printing waiting for next */
                last = first = i_entry;
                prev_base = curr_base;
                prev_macro = curr_macro;
            }
            else if((prev_base == curr_base) && (prev_macro == curr_macro))
                /* Next element in chains of equal, not printing waiting for next, unless last in segment */
                last = i_entry;
            else {
                /* Different element, so we need to print previous one */
                pp_dump_parser_print_entries(unit, curSegment, depth + 1, offset, prev_base, prev_macro, first, last);
                /* And start new one counting */
                last = first = i_entry;
                prev_base = curr_base;
                prev_macro = curr_macro;
            }
            if(i_entry == (PARSER_PROGRAM_SEGMENT_SIZE - 1))
                /* print anyway current */
                pp_dump_parser_print_entries(unit, curSegment, depth + 1, offset, curr_base, curr_macro, first, last);
       }
       dbx_xml_node_end(curSegment, depth);
    }

    for(i_seg = 0; i_seg < PARSER_PROGRAM_SEGMENT_NUM; i_seg++)
    {
        curSegment = dbx_xml_child_get_first(curTop, parser_segment_name[i_seg]);
        RHDATA_SET_INT(curSegment, "References", next_segment_map[i_seg]);
        RHDATA_SET_INT(curSegment, "ID", i_seg);
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e pp_dump_parser_ports(int unit, void *curTop, int depth)
{
    void *cur;
    bcm_pbmp_t pbmp_nif;
    bcm_port_t port;
    uint32 pp_port;
    int core_id;
    ARAD_PP_IHP_PP_PORT_INFO_TBL_DATA tbl_data;
    char entry_n[RHNAME_MAX_SIZE];

    SHR_FUNC_INIT_VARS(unit);

    BCM_PBMP_CLEAR(pbmp_nif);
    BCM_PBMP_ASSIGN(pbmp_nif, PBMP_PORT_ALL(unit));
    BCM_PBMP_REMOVE(pbmp_nif, PBMP_SFI_ALL(unit));
    BCM_PBMP_REMOVE(pbmp_nif, PBMP_RCY_ALL(unit));

    BCM_PBMP_ITER(pbmp_nif, port) {
        if(soc_port_sw_db_local_to_pp_port_get(unit, port, &pp_port, &core_id) != SOC_E_NONE) {
            SHR_ERR_EXIT(_SHR_E_FAIL, "soc_port_sw_db_local_to_pp_port_get Failed\n");
        }

        if (soc_sand_get_error_code_from_error_word(arad_pp_ihp_pp_port_info_tbl_get_unsafe(unit, core_id, pp_port, &tbl_data)) != SOC_SAND_OK)
            SHR_EXIT();

        sprintf(entry_n, "Port:%d", port);
        if((cur = dbx_xml_child_add(curTop, entry_n, depth)) == NULL) {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Failed to add Program Selection section to IRPP Parser\n");
        }
        RHDATA_SET_INT(cur, "Program", tbl_data.default_parser_program_pointer);
        RHDATA_SET_INT(cur, "TPID-Profile",   tbl_data.tpid_profile);
        RHDATA_SET_INT(cur, "Header-Offset",  tbl_data.pp_port_outer_header_start);
        dbx_xml_node_end(cur, depth);
    }

exit:
    SHR_FUNC_EXIT;
}

static uint32 pp_dump_parser_macros(int unit, void *curTop, int depth)
{
    int i;
    void *cur;
    char    entry_n[RHNAME_MAX_SIZE];
    ARAD_PP_IHP_PARSER_CUSTOM_MACRO_PARAMETERS_TBL_DATA custom_macro_parameters;
    ARAD_PP_IHP_PARSER_CUSTOM_MACRO_PROTOCOLS_TBL_DATA  custom_macro_protocols;
    ARAD_PP_IHP_PARSER_ETH_PROTOCOLS_TBL_DATA           eth_protocols;
    ARAD_PP_IHP_PARSER_IP_PROTOCOLS_TBL_DATA            ip_protocols;

    SHR_FUNC_INIT_VARS(unit);

    /* Eth Protocols */
    if((cur = dbx_xml_child_add(curTop, "Eth-Protocols", depth)) == NULL) {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Failed to add Eth-Protocols section\n");
    }
    for(i = 0; i < 7; i++) {
        arad_pp_ihp_parser_eth_protocols_tbl_get_unsafe(unit, i, &eth_protocols);
        if(eth_protocols.eth_type_protocol) {
            sprintf(entry_n, "Entry.%d-Eth2", i);
            RHDATA_SET_HEX(cur,  entry_n, eth_protocols.eth_type_protocol);
        }
        if(eth_protocols.eth_sap_protocol) {
            sprintf(entry_n, "Entry.%d-802.3", i);
            RHDATA_SET_HEX(cur,  entry_n, eth_protocols.eth_sap_protocol);
        }
    }

    /* IP Protocols */
    if((cur = dbx_xml_child_add(curTop, "IP-Protocols", depth)) == NULL) {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Failed to add IP-Protocols section\n");
    }
    for(i = 0; i < 7; i++) {
        arad_pp_ihp_parser_ip_protocols_tbl_get_unsafe(unit, i, &ip_protocols);
        sprintf(entry_n, "Protocol:%d", i);
        RHDATA_SET_INT(cur,  entry_n, ip_protocols.ip_protocol);
    }

    /* Custom Protocols */
    if((cur = dbx_xml_child_add(curTop, "Macro-Protocols", depth)) == NULL) {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Failed to add Macro-Protocols section\n");
    }
    for(i = 0; i < 15; i++) {
        arad_pp_ihp_parser_custom_macro_protocols_tbl_get_unsafe(unit, i, &custom_macro_protocols);
        sprintf(entry_n, "Protocol:%d", i);
        RHDATA_SET_HEX(cur,  entry_n, custom_macro_protocols.cstm_protocol);
    }

    /* Custom Macro Parameters */
    for(i = 0; i < 8; i++) {
        arad_pp_ihp_parser_custom_macro_parameters_tbl_get_unsafe(unit, i, &custom_macro_parameters);
        sprintf(entry_n, "Macro:%d", i);
        if((cur = dbx_xml_child_add(curTop, entry_n, depth)) == NULL) {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Failed to add Macro section\n");
        }
        RHDATA_SET_INT(cur,  "Word-Offset",           custom_macro_parameters.cstm_word_select); /* bytes from the start of header - word 32bit */
        RHDATA_SET_INT(cur,  "LSB-Bits-Masked",       custom_macro_parameters.cstm_mask_right);  /* first LSB bits of word are masked */
        RHDATA_SET_INT(cur,  "MSB-Bits-Masked",       custom_macro_parameters.cstm_mask_left);   /* first MSB bits of word are masked */
        RHDATA_SET_INT(cur,  "Shift-Select",          custom_macro_parameters.cstm_shift_sel);   /* Boolean indication approach to choose header size */
        RHDATA_SET_HEX(cur,  "Header-Size-A",         custom_macro_parameters.cstm_shift_a);     /* Use in creating Header Size for the stage */
        RHDATA_SET_HEX(cur,  "Header-Size-B",         custom_macro_parameters.cstm_shift_b);     /* Use in creating Header Size for the stage */
        RHDATA_SET_HEX(cur,  "Comparator-Mask",       custom_macro_parameters.cstm_comparator_mask); /* Each bit indicates if specific comparator participates in comparison */
        RHDATA_SET_STR(cur,  "Comparator-Mode",       comparator_mode[custom_macro_parameters.cstm_comparator_mode]); /* Mode which comparator table choose */
        RHDATA_SET_HEX(cur,  "Condition-Select",      custom_macro_parameters.cstm_condition_select);  /* Used when Shift Select is Zero to choose Header Size */
        RHDATA_SET_HEX(cur,  "Header-Qualifier-Mask", custom_macro_parameters.header_qualifier_mask);  /* relevant for augmented stage */
        RHDATA_SET_BOOL(cur, "Augment-Previous",      custom_macro_parameters.augment_previous_stage); /* Work on the same qualifier */
        RHDATA_SET_HEX(cur,  "Qualifier-Shift",       custom_macro_parameters.qualifier_shift);        /* Used in creating qualifier */
        RHDATA_SET_HEX(cur,  "Qualifier-Or-Mask",     custom_macro_parameters.qualifier_or_mask);      /* Used in creating qualifier */
        RHDATA_SET_HEX(cur,  "Qualifier-And-Mask",    custom_macro_parameters.qualifier_and_mask);     /* Used in creating qualifier */
        dbx_xml_node_end(cur, depth);
    }

exit:
    SHR_FUNC_EXIT;
}

static int pp_dump_parser(int unit, void *curStage, int depth)
{
    void *curSection;

    SHR_FUNC_INIT_VARS(unit);

    RHDATA_SET_STR(curStage, "command_line", "export pp stage=parser");

    if((curSection = dbx_xml_child_add(curStage, "Programs", depth)) == NULL) {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Failed to add Program Selection section to IRPP Parser\n");
    }

    pp_dump_parser_programs(unit, curSection, depth + 1);

    dbx_xml_node_end(curSection, depth);

    if((curSection = dbx_xml_child_add(curStage, "Ports", depth)) == NULL) {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Failed to add Port section to IRPP Parser\n");
    }
    pp_dump_parser_ports(unit, curSection, depth + 1);
    dbx_xml_node_end(curSection, depth);

    if((curSection = dbx_xml_child_add(curStage, "Custom-Macros", depth)) == NULL) {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Failed to add Custom-Macros section to IRPP Parser\n");
    }
    pp_dump_parser_macros(unit, curSection, depth + 1);
    dbx_xml_node_end(curSection, depth);

exit:
    SHR_FUNC_EXIT;
}

/*
 * Mapping Functions FLP -> Export
 */

/* Fill Export DB using FP and FLP DBs*/
static int
dpp_export_pp_build_db(int unit, dpp_export_pp_info_t *dpp_export_pp_info)
{
    SOC_PPC_FP_DATABASE_STAGE stage;
    int program_id, key, action, table_index;
    int program_selection;
    SOC_PPC_FP_RESOURCE_DIAG_INFO *fp_info = NULL;
    int key_idx, key_in_db_idx;

    BCMDNX_INIT_FUNC_DEFS;

    /* Init */
    sal_memset(dpp_export_pp_info, 0, sizeof(dpp_export_pp_info_t));

    dpp_dump_flp(unit, 0, &dpp_export_pp_info->stages[SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP]);

    dpp_dump_vt(unit, 0, &dpp_export_pp_info->stages[SOC_PPC_FP_DATABASE_STAGE_INGRESS_VT]);

    dpp_dump_tt(unit, 0, &dpp_export_pp_info->stages[SOC_PPC_FP_DATABASE_STAGE_INGRESS_TT]);

    /* FP - PMF+FLP */
    BCMDNX_ALLOC(fp_info, sizeof(SOC_PPC_FP_RESOURCE_DIAG_INFO), "SOC_PPC_FP_RESOURCE_DIAG_INFO");
    if (fp_info == NULL) {
        return CMD_FAIL;
    }

    sal_strcpy(dpp_export_pp_info->stages[SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF].name, "IRPP.PMF");

    SOC_PPC_FP_RESOURCE_DIAG_INFO_clear(fp_info);
    if (!arad_pp_fp_resource_diag_get_unsafe(unit, SOC_PPC_FP_RESOURCE_MODE_USAGE, fp_info)) {
        return CMD_FAIL;
    }

    /* Tables */
    for (table_index = 0; table_index < SOC_PPC_FP_NOF_DBS; table_index++) {
        if (fp_info->db[table_index].valid) {

            stage = fp_info->db[table_index].stage;

            /* Currently supporting only TCAM */
                
                sal_snprintf(dpp_export_pp_info->stages[stage].tables[table_index].name, DBAL_MAX_NAME_LENGTH, "User Defined table %d", table_index);
                sal_strncpy(dpp_export_pp_info->stages[stage].tables[table_index].database, SOC_PPC_FP_DATABASE_TYPE_to_string(SOC_PPC_FP_DB_TYPE_TCAM), DPP_EXPORT_CHAR_MAX_LENGTH - 1);

                key = 0;
                for (key_in_db_idx = 0; key_in_db_idx < SOC_PPC_FP_KEY_NOF_KEYS_PER_DB_MAX; key_in_db_idx++) {
                    for (key_idx = 0; key_idx < fp_info->db[table_index].nof_ces; key_idx++) {
                        if ((key_in_db_idx == 0 && !fp_info->db[table_index].key_qual[key_idx].is_second_key) || (key_in_db_idx == 1 && fp_info->db[table_index].key_qual[key_idx].is_second_key)) {
                            sal_strncpy(dpp_export_pp_info->stages[stage].tables[table_index].keys[key].name,
                                       SOC_PPC_FP_QUAL_TYPE_to_string(fp_info->db[table_index].key_qual[key_idx].qual_type), DPP_EXPORT_CHAR_MAX_LENGTH - 1);
                            key = key + 1;
                        }
                    }
                }

                for (action = 0; action < fp_info->db[table_index].db_tcam.nof_actions; action++) {
                    sal_strncpy(dpp_export_pp_info->stages[stage].tables[table_index].actions[action].name,
                               SOC_PPC_FP_ACTION_TYPE_to_string(fp_info->db[table_index].db_tcam.action[action].action_type), DPP_EXPORT_CHAR_MAX_LENGTH - 1); /* For Export - check the value. Set to Forwarding??? */
                }

                dpp_export_pp_info->stages[stage].tables[table_index].priority = fp_info->db[table_index].db_priority;
                dpp_export_pp_info->stages[stage].tables[table_index].is_static = 1/*(arad_export_fp_field_group_id_to_name[stage][table_index] == NULL)*/; /* is configured through Arrakis */
        }
    }

    /* Pre Selectors */
    for (stage = 0; stage < SOC_PPC_NOF_FP_DATABASE_STAGES; stage++) {
        if ((stage != SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF) && (stage != SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP)) {
            continue;
        }

        for (program_selection = 0; program_selection < SOC_PPC_FP_NOF_PS_LINES; program_selection++) {
            if (fp_info->presel[stage][program_selection].is_valid) {

                program_id = fp_info->presel[stage][program_selection].pmf_pgm;

                sal_snprintf(dpp_export_pp_info->stages[stage].presels[program_selection].name, DBAL_MAX_NAME_LENGTH, "User Defined Presel %d", program_selection); 
                sal_snprintf(dpp_export_pp_info->stages[stage].presels[program_selection].app_type, DBAL_MAX_NAME_LENGTH, "User Defined Presel %d", program_selection); 
                dpp_export_pp_info->stages[stage].presels[program_selection].is_static = 1; 

                dpp_export_pp_info->stages[stage].presels[program_selection].bundle = &dpp_export_pp_info->stages[stage].bundles[program_id];

                /* Program */
                sal_snprintf(dpp_export_pp_info->stages[stage].bundles[program_id].name, DBAL_MAX_NAME_LENGTH, "User Defined Program %d", program_id); 

                dpp_export_pp_info->stages[stage].bundles[program_id].is_static = 1; 

                for (table_index = 0; table_index < SOC_PPC_FP_NOF_DBS; ++table_index) {
                    if (SHR_BITGET(fp_info->presel[stage][program_selection].db_bmp, table_index)) {
                        if(!ISEMPTY(dpp_export_pp_info->stages[stage].tables[table_index].name)) {

                            
                            dpp_export_pp_info->stages[stage].bundles[program_id].tables[dpp_export_pp_info->stages[stage].bundles[program_id].nof_tables].table =
                               &dpp_export_pp_info->stages[stage].tables[table_index];
                            dpp_export_pp_info->stages[stage].bundles[program_id].tables[dpp_export_pp_info->stages[stage].bundles[program_id].nof_tables].lookup = 1; /* One stage is always first lookup */
                            dpp_export_pp_info->stages[stage].bundles[program_id].tables[dpp_export_pp_info->stages[stage].bundles[program_id].nof_tables].is_static = 1; 

                            dpp_export_pp_info->stages[stage].bundles[program_id].nof_tables++;
                        }
                    }
                }
            }

        }
    }

    BCM_FREE(fp_info);

    /*dpp_export_pp_build_db_dump(unit, dpp_export_pp_info); DEBUG */

    return CMD_OK;

exit:
    BCMDNX_FUNC_RETURN;
}

static shr_error_e pp_dump_export_data(int unit, dpp_export_pp_stage_info_t *stage_info, void *curTop, int depth)
{
    void *curPresel, *curIndex, *curBundle, *curTable, *curKey, *curAction;
    int program_selection;
    int program_id, key, action, table_index;
    dpp_presel_qual_info_t *presel_qual;

    SHR_FUNC_INIT_VARS(unit);

    RHDATA_SET_STR(curTop, "name", stage_info->name);

    /* Program-Selection */
    for (program_selection = 0; program_selection < DPP_EXPORT_PP_PRESEL_MAX; program_selection++) {
        if (!ISEMPTY(stage_info->presels[program_selection].name)) {
            if((curPresel = dbx_xml_child_add(curTop, "contextselect", depth)) == NULL) {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Failed to add FLP selection entry:%d\n", program_selection);
            }

            RHDATA_SET_STR(curPresel, "name", stage_info->presels[program_selection].name);
            RHDATA_SET_STR(curPresel, "context", stage_info->presels[program_selection].bundle->name);
            RHDATA_SET_INT(curPresel, "user", !stage_info->presels[program_selection].is_static);
            RHDATA_SET_INT(curPresel, "priority", program_selection); /*ADD IMPLEMENTATION HERE!!! - I assume we can use TCAM index */
            if(stage_info->presels[program_selection].selected)
                RHDATA_SET_INT(curPresel, "select", 1);

            if((curIndex = dbx_xml_child_add(curPresel, "field", depth + 1)) == NULL) {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Failed to add FLP selection entry:%d\n", program_selection);
            }
            if(!ISEMPTY(stage_info->presels[program_selection].app_type)) {
                RHDATA_SET_STR(curIndex, "name", "AppType");
                RHDATA_SET_STR(curIndex, "value", stage_info->presels[program_selection].app_type);
            }
            RHITERATOR(presel_qual, stage_info->presels[program_selection].qual_list) {
                if((curIndex = dbx_xml_child_add(curPresel, "field", depth + 1)) == NULL) {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "Failed to add FLP selection entry:%d\n", program_selection);
                }
                RHDATA_SET_STR(curIndex, "name", RHNAME(presel_qual));
                RHDATA_SET_HEX(curIndex, "value", presel_qual->data);
                if(presel_qual->mask != SOC_SAND_U32_MAX)
                    RHDATA_SET_HEX(curIndex, "mask", presel_qual->mask);
            }

            dbx_xml_node_end(curPresel, depth);
        }
    }

    /* Bundles */
    for (program_id = 0; program_id < DPP_EXPORT_PP_BUNDLE_TABLE_MAX; program_id++) {
        if (!ISEMPTY(stage_info->bundles[program_id].name)) {

            if((curBundle = dbx_xml_child_add(curTop, "context", depth)) == NULL) {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Failed to add FLP selection entry:%d\n", program_selection);
            }

            RHDATA_SET_STR(curBundle, "name", stage_info->bundles[program_id].name);
            RHDATA_SET_INT(curBundle, "user", !stage_info->bundles[program_id].is_static);
            if(stage_info->bundles[program_id].selected)
                RHDATA_SET_INT(curBundle, "select", 1);

            for (table_index = 0; table_index < stage_info->bundles[program_id].nof_tables; table_index++) {
                if (stage_info->bundles[program_id].tables[table_index].table) {

                    if((curTable = dbx_xml_child_add(curBundle, "table", depth + 1)) == NULL) {
                        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Failed to add FLP selection entry:%d\n", program_selection);
                    }

                    RHDATA_SET_STR(curTable, "name", stage_info->bundles[program_id].tables[table_index].table->name);
                    RHDATA_SET_INT(curTable, "lookup", stage_info->bundles[program_id].tables[table_index].lookup);
                    RHDATA_SET_INT(curTable, "user", !stage_info->bundles[program_id].tables[table_index].is_static);
                }
            }

            dbx_xml_node_end(curBundle, depth);
        }
    }

    /* Tables */
    for (table_index = 0; table_index < DPP_EXPORT_PP_TABLE_MAX; table_index++) {
        if (!ISEMPTY(stage_info->tables[table_index].name)) {

            if((curTable = dbx_xml_child_add(curTop, "table", depth)) == NULL) {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Failed to add FLP selection entry:%d\n", program_selection);
            }

            RHDATA_SET_STR(curTable, "name", stage_info->tables[table_index].name);
            RHDATA_SET_STR(curTable, "memory", stage_info->tables[table_index].database);
            RHDATA_SET_INT(curTable, "priority", stage_info->tables[table_index].priority);
            RHDATA_SET_INT(curTable, "user", !stage_info->tables[table_index].is_static);

            for (key = 0; key < SOC_PPC_FP_NOF_QUALS_PER_DB_MAX; key++) {
                if (!ISEMPTY(stage_info->tables[table_index].keys[key].name)) {
                    if((curKey = dbx_xml_child_add(curTable, "field", depth + 1)) == NULL) {
                        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Failed to add FLP selection entry:%d\n", program_selection);
                    }

                    RHDATA_SET_STR(curKey, "value", stage_info->tables[table_index].keys[key].name);
                }
            }

            for (action = 0; action < SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX; action++) {
                if (!ISEMPTY(stage_info->tables[table_index].actions[action].name)) {
                    if((curAction = dbx_xml_child_add(curTable, "action", depth + 1)) == NULL) {
                        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Failed to add FLP selection entry:%d\n", program_selection);
                    }

                    RHDATA_SET_STR(curAction, "value", stage_info->tables[table_index].actions[action].name);
                }
            }
            dbx_xml_node_end(curTable, depth);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static cmd_result_t pp_scheme_export(int unit, void *curTop)
{
    uint32 res = CMD_OK;
    void *curStage = NULL, *Stages;
    dpp_export_pp_info_t *dpp_export_pp_info;

    if((dpp_export_pp_info = sal_alloc(sizeof(dpp_export_pp_info_t), "pp_info")) == NULL) {
        res = CMD_FAIL;
        goto exit;
    }

    if (dpp_export_pp_build_db(unit, dpp_export_pp_info) != CMD_OK) {
        res = CMD_FAIL;
        goto exit;
    }

    /* Stages */
    if ((Stages = dbx_xml_child_add(curTop, "stages", 1)) == NULL) {
        cli_out("Failed to add stages\n");
        res = CMD_FAIL;
        goto exit;
    }

    /* VT */
    if ((curStage = dbx_xml_child_add(Stages, "stage", 2)) == NULL) {
        cli_out("Failed to add VT section to dump\n");
        res = CMD_FAIL;
        goto exit;
    }

    pp_dump_export_data(unit, &dpp_export_pp_info->stages[SOC_PPC_FP_DATABASE_STAGE_INGRESS_VT], curStage, 3);
    dbx_xml_node_end(curStage, 2);

    /* TT */
    if ((curStage = dbx_xml_child_add(Stages, "stage", 2)) == NULL) {
        cli_out("Failed to add TT section to dump\n");
        res = CMD_FAIL;
        goto exit;
    }

    pp_dump_export_data(unit, &dpp_export_pp_info->stages[SOC_PPC_FP_DATABASE_STAGE_INGRESS_TT], curStage, 3);
    dbx_xml_node_end(curStage, 2);

    /* FLP */
    if ((curStage = dbx_xml_child_add(Stages, "stage", 2)) == NULL) {
        cli_out("Failed to add FLP section to dump\n");
        res = CMD_FAIL;
        goto exit;
    }

    pp_dump_export_data(unit, &dpp_export_pp_info->stages[SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP], curStage, 3);
    dbx_xml_node_end(curStage, 2);

    /* PMF */
    if ((curStage = dbx_xml_child_add(Stages, "stage", 2)) == NULL) {
        cli_out("Failed to add PMF section to dump\n");
        res = CMD_FAIL;
        goto exit;
    }

    pp_dump_export_data(unit, &dpp_export_pp_info->stages[SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF], curStage, 3);
    sal_free(dpp_export_pp_info);
    dbx_xml_node_end(curStage, 2);
    dbx_xml_node_end(Stages, 1);

exit:
    return res;
}

/* } */
#endif /* !defined(__KERNEL__) */

cmd_result_t dpp_export_pp(int unit, char *stage_n, char *filename)
{
#if !defined(__KERNEL__)
    uint32 res = CMD_OK;

    int i;
    void *curTop = NULL;
    char auto_filename[RHNAME_MAX_SIZE];

    if ((curTop = dbx_xml_top_create("top")) == NULL) {
        cli_out("Fail to create top\n");
        return CMD_FAIL;
    }

    if(ISEMPTY(stage_n)) {
        /* No stage specified - dumping all */
        /* Here we call to scheme export */
        if((res = pp_scheme_export(unit, curTop)) != CMD_OK)
            goto exit;
    }
    else {
        /* Find pp stage */
        for(i = 0; stage_callback[i].name != NULL; i++) {
            if(!sal_strcasecmp(stage_n, stage_callback[i].name)) {
                if(stage_callback[i].dump_callback) {
                    void *curStage;
                    if ((curStage = dbx_xml_child_add(curTop, "command", 1)) == NULL) {
                        cli_out("Failed to add stages\n");
                        res = CMD_FAIL;
                        goto exit;
                    }
                    stage_callback[i].dump_callback(unit, curStage, 2);
                }
                break;
            }
        }
        if(stage_callback[i].name == NULL) {
            cli_out("No stage:%s\n", stage_n);
            return CMD_FAIL;
        }
    }

    if(ISEMPTY(filename)) {
        for(i = 0; i < 100; i++) {
            sal_snprintf(auto_filename, RHNAME_MAX_SIZE - 1, "%s-%d.xml", "PP-Data", i);
            filename = auto_filename;
            if(dbx_file_exists(filename) == FALSE)
                /* autofile do not exist - we may use the name */
                break;
        }
    }
    else {
        if(dbx_file_exists(filename) == TRUE) {
            cli_out("Overwriting %s\n", filename);
            /* file already exists - but we overwrite it
            return CMD_FAIL;
            */
        }
    }

    if(dbx_file_save_xml(unit, filename, curTop, CONF_OPEN_CURRENT_LOC) != _SHR_E_NONE)
        goto exit;

    dbx_xml_top_close(curTop);

    cli_out("Saving to:%s\n", filename);

exit:
    return res;
#else
    return CMD_NOTIMPL;
#endif /* KERNEL */
}
