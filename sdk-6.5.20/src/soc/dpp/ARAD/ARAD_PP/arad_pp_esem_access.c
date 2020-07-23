#include <soc/mcm/memregs.h>
#if defined(BCM_88650_A0)
/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_LIF



#include <shared/bsl.h>
#include <soc/dcmn/error.h>

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_low_level.h>
#include <soc/mem.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_general.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_esem_access.h>
#include <soc/dpp/drv.h>
#include <soc/dpp/ARAD/arad_sim_em.h>
#ifdef CRASH_RECOVERY_SUPPORT

#include <soc/hwstate/hw_log.h>

#endif 





#define ARAD_PP_ESEM_ACCESS_DEBUG                                     (0)


#define ARAD_PP_ESEM_FLD_INVALID                               (0xFFFFFFFF)

#define ARAD_PP_ESEM_ENTRY_REQ_TYPE_LSB                        (0)
#define ARAD_PP_ESEM_ENTRY_REQ_TYPE_MSB                        (2)
#define ARAD_PP_ESEM_ENTRY_REQ_TYPE_NOF_BITS                   SOC_SAND_RNG_COUNT(ARAD_PP_ESEM_ENTRY_REQ_TYPE_LSB, ARAD_PP_ESEM_ENTRY_REQ_TYPE_MSB)

#define ARAD_PP_ESEM_ENTRY_REQ_STAMP_LSB                       (3)
#define ARAD_PP_ESEM_ENTRY_REQ_STAMP_MSB                       (10)
#define ARAD_PP_ESEM_ENTRY_REQ_STAMP_NOF_BITS                  SOC_SAND_RNG_COUNT(ARAD_PP_ESEM_ENTRY_REQ_STAMP_LSB, ARAD_PP_ESEM_ENTRY_REQ_STAMP_MSB)

#define ARAD_PP_ESEM_ENTRY_REQ_STAMP_VAL                       (0xAE)

#define ARAD_PP_ESEM_ENTRY_REQ_KEY_LSB                         (11)
#define ARAD_PP_ESEM_ENTRY_REQ_KEY_MSB(unit)                   (SOC_IS_JERICHO(unit) ? 50:47)
#define ARAD_PP_ESEM_ENTRY_REQ_KEY_NOF_BITS(unit)              SOC_SAND_RNG_COUNT(ARAD_PP_ESEM_ENTRY_REQ_KEY_LSB, ARAD_PP_ESEM_ENTRY_REQ_KEY_MSB(unit))

#define ARAD_PP_ESEM_ENTRY_REQ_PAYLOAD_LSB(unit)               (SOC_IS_JERICHO(unit) ? 51:48)
#define ARAD_PP_ESEM_ENTRY_REQ_PAYLOAD_MSB(unit)               (SOC_IS_JERICHO(unit) ? 67:63)
#define ARAD_PP_ESEM_ENTRY_REQ_PAYLOAD_NOF_BITS(unit)          SOC_SAND_RNG_COUNT(ARAD_PP_ESEM_ENTRY_REQ_PAYLOAD_LSB(unit), ARAD_PP_ESEM_ENTRY_REQ_PAYLOAD_MSB(unit))

#define ARAD_PP_ESEM_ENTRY_REQ_SIZE_REGS                       (3)

#define ARAD_PP_ESEM_KEY_CVID_LSB                              (0)
#define ARAD_PP_ESEM_KEY_CVID_MSB                              (11)
#define ARAD_PP_ESEM_KEY_CVID_NOF_BITS                         SOC_SAND_RNG_COUNT(ARAD_PP_ESEM_KEY_CVID_LSB, ARAD_PP_ESEM_KEY_CVID_MSB)

#define ARAD_PP_ESEM_KEY_VSI_LSB                               (12)
#define ARAD_PP_ESEM_KEY_VSI_MSB                               (27)
#define ARAD_PP_ESEM_KEY_VSI_NOF_BITS                          SOC_SAND_RNG_COUNT(ARAD_PP_ESEM_KEY_VSI_LSB, ARAD_PP_ESEM_KEY_VSI_MSB)

#define ARAD_PP_ESEM_KEY_VD_LSB                                (28)
#define ARAD_PP_ESEM_KEY_VD_MSB(unit)                          (SOC_IS_JERICHO(unit) ? 36:35)
#define ARAD_PP_ESEM_KEY_VD_NOF_BITS(unit)                     ((ARAD_PP_ESEM_KEY_VD_MSB(unit) - (ARAD_PP_ESEM_KEY_VD_LSB)) + 1)

#define ARAD_PP_ESEM_KEY_VSI_DB_ID_LSB(unit)                   (SOC_IS_JERICHO(unit) ? 37:36)
#define ARAD_PP_ESEM_KEY_VSI_DB_ID_MSB(unit)                   (SOC_IS_JERICHO(unit) ? 37:36)
#define ARAD_PP_ESEM_KEY_VSI_DB_ID_NOF_BITS(unit)              SOC_SAND_RNG_COUNT(ARAD_PP_ESEM_KEY_VSI_DB_ID_LSB(unit), ARAD_PP_ESEM_KEY_VSI_DB_ID_MSB(unit))

#define ARAD_PP_ESEM_ENTRY_ESEM_PAYLOAD_NDX_LSB                (0)
#define ARAD_PP_ESEM_ENTRY_ESEM_PAYLOAD_NDX_MSB(unit)          (SOC_IS_JERICHO(unit) ? 17:15)
#define ARAD_PP_ESEM_ENTRY_ESEM_PAYLOAD_NDX_NOF_BITS(unit)     SOC_SAND_RNG_COUNT(ARAD_PP_ESEM_ENTRY_ESEM_PAYLOAD_NDX_LSB, ARAD_PP_ESEM_ENTRY_ESEM_PAYLOAD_NDX_MSB(unit))



#define ARAD_PP_ESEM_KEY_LIF_VSI_LIF_LSB                       (0)
#define ARAD_PP_ESEM_KEY_LIF_VSI_LIF_MSB(unit)                 (ARAD_PP_ESEM_KEY_LIF_VSI_LIF_LSB + SOC_DPP_DEFS_GET(unit, out_lif_nof_bits) - 1) 
#define ARAD_PP_ESEM_KEY_LIF_VSI_LIF_NOF_BITS(unit)             SOC_SAND_RNG_COUNT(ARAD_PP_ESEM_KEY_LIF_VSI_LIF_LSB, ARAD_PP_ESEM_KEY_LIF_VSI_LIF_MSB(unit))


#define ARAD_PP_ESEM_KEY_LIF_VSI_VSI_LSB(unit)                 (ARAD_PP_ESEM_KEY_LIF_VSI_LIF_MSB(unit) + 1)
#define ARAD_PP_ESEM_KEY_LIF_VSI_VSI_MSB(unit)                 (ARAD_PP_ESEM_KEY_LIF_VSI_VSI_LSB(unit) + 16 - 1)
#define ARAD_PP_ESEM_KEY_LIF_VSI_VSI_NOF_BITS(unit)             SOC_SAND_RNG_COUNT(ARAD_PP_ESEM_KEY_LIF_VSI_VSI_LSB(unit), ARAD_PP_ESEM_KEY_LIF_VSI_VSI_MSB(unit))









          




typedef enum
{
  ARAD_PP_ESEM_ACTION_TYPE_DELETE = 0,
  ARAD_PP_ESEM_ACTION_TYPE_INSERT,
  ARAD_PP_ESEM_ACTION_TYPE_DEFRAG,
  ARAD_PP_ESEM_NOF_ACTION_TYPES
}ARAD_PP_ESEM_ACTION_TYPE;


CONST STATIC SOC_PROCEDURE_DESC_ELEMENT
  Arad_pp_procedure_desc_element_esem[] =
{
  
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_ESEM_GET_PROCS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_ESEM_GET_ERRS_PTR),
  
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_ESEM_ENTRY_ADD_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_ESEM_ENTRY_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_ESEM_ENTRY_REMOVE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_ESEM_TBL_WRITE),

  
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

CONST STATIC SOC_ERROR_DESC_ELEMENT
  Arad_pp_error_desc_element_esem[] =
{
  
  

  {
    ARAD_PP_ESEM_NOT_READY_ERR,
    "ARAD_PP_ESEM_NOT_READY_ERR",
    "Sem is not ready. \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_ESEM_KEY_TYPE_INVALID_ERR,
    "ARAD_PP_ESEM_KEY_TYPE_INVALID_ERR",
    "ESEM Key type out-of-range \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_ESEM_FAIL_ON_DIFFERENT_KEY_ERR,
    "ARAD_PP_ESEM_FAIL_ON_DIFFERENT_KEY_ERR",
    "ESEM operation reported an error,\n\r "
    "but the reported key is different from the one\n\r"
    "for which the ESEM was accessed\n\r",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_ESEM_NOT_READ_TIMEOUT_ERR,
    "ARAD_PP_ESEM_NOT_READ_TIMEOUT_ERR",
    "When reading ESEM-DB,  \n\r "
    "the look-up operation did not end",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  

  
SOC_ERR_DESC_ELEMENT_DEF_LAST
};









STATIC
  uint32
    arad_pp_esem_key2intern(
      SOC_SAND_IN ARAD_PP_ESEM_ACTION_TYPE action_type
    )
{
  uint32
    action_type_intern;

  switch(action_type) {
  case ARAD_PP_ESEM_ACTION_TYPE_DELETE:
    action_type_intern = 0x0;
    break;
  case ARAD_PP_ESEM_ACTION_TYPE_INSERT:
    action_type_intern = 0x1;
    break;
  case ARAD_PP_ESEM_ACTION_TYPE_DEFRAG:
    action_type_intern = 0x4;
    break;
  default:
    action_type_intern = ARAD_PP_ESEM_FLD_INVALID;
  }

  return action_type_intern;
}

uint32
  arad_pp_esem_key_to_buffer(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  ARAD_PP_ESEM_KEY      *esem_key,
    SOC_SAND_OUT uint32                *buffer
  )
{
  uint32
    tmp;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_ESEM_KEY_TO_BUFFER);
  
  ARAD_PP_CLEAR(buffer, uint32, ARAD_PP_ESEM_KEY_SIZE);

  switch(esem_key->key_type)
  {
    case ARAD_PP_ESEM_KEY_TYPE_VD_VSI:
      res = soc_sand_bitstream_set_any_field(&esem_key->key_info.vd_vsi.vd, ARAD_PP_ESEM_KEY_VD_LSB, ARAD_PP_ESEM_KEY_VD_NOF_BITS(unit), buffer);
      SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);
      res = soc_sand_bitstream_set_any_field(&esem_key->key_info.vd_vsi.vsi, ARAD_PP_ESEM_KEY_VSI_LSB, ARAD_PP_ESEM_KEY_VSI_NOF_BITS, buffer);
      SOC_SAND_CHECK_FUNC_RESULT(res, 13, exit);
      tmp = ARAD_PP_ESEM_KEY_VSI_DB_ID_VAL;
      res = soc_sand_bitstream_set_any_field(&tmp, ARAD_PP_ESEM_KEY_VSI_DB_ID_LSB(unit), ARAD_PP_ESEM_KEY_VSI_DB_ID_NOF_BITS(unit), buffer);
      SOC_SAND_CHECK_FUNC_RESULT(res, 14, exit);
      break;
    case ARAD_PP_ESEM_KEY_TYPE_VD_VSI_CVID:
      
      
      res = soc_sand_bitstream_set_any_field(&esem_key->key_info.vd_cvid.vd, ARAD_PP_ESEM_KEY_VD_LSB, ARAD_PP_ESEM_KEY_VD_NOF_BITS(unit), buffer);
      SOC_SAND_CHECK_FUNC_RESULT(res, 16, exit);
      res = soc_sand_bitstream_set_any_field(&esem_key->key_info.vd_cvid.vsi, ARAD_PP_ESEM_KEY_VSI_LSB, ARAD_PP_ESEM_KEY_VSI_NOF_BITS, buffer);
      SOC_SAND_CHECK_FUNC_RESULT(res, 17, exit);
      res = soc_sand_bitstream_set_any_field(&esem_key->key_info.vd_cvid.cvid, ARAD_PP_ESEM_KEY_CVID_LSB, ARAD_PP_ESEM_KEY_CVID_NOF_BITS, buffer);
      SOC_SAND_CHECK_FUNC_RESULT(res, 18, exit);
      break;
  case ARAD_PP_ESEM_KEY_TYPE_LIF_VSI:
      res = soc_sand_bitstream_set_any_field(&esem_key->key_info.lif_vsi.lif, ARAD_PP_ESEM_KEY_LIF_VSI_LIF_LSB, ARAD_PP_ESEM_KEY_LIF_VSI_LIF_MSB(unit), buffer);
      SOC_SAND_CHECK_FUNC_RESULT(res, 19, exit);
      res = soc_sand_bitstream_set_any_field(&esem_key->key_info.lif_vsi.vsi, ARAD_PP_ESEM_KEY_LIF_VSI_VSI_LSB(unit), ARAD_PP_ESEM_KEY_LIF_VSI_VSI_NOF_BITS(unit), buffer);
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
      break;
    default:
      SOC_SAND_SET_ERROR_CODE(ARAD_PP_ESEM_KEY_TYPE_INVALID_ERR, 20, exit)
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_esem_key_to_buffer()", 0, 0);
}


uint32
  arad_pp_esem_buffer_to_key(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint8              is_native,
    SOC_SAND_OUT uint32             *buffer,
    SOC_SAND_OUT  ARAD_PP_ESEM_KEY  *esem_key,
    SOC_SAND_OUT  uint32            *vsi_db_id
  )
{  
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = soc_sand_bitstream_get_any_field(buffer, ARAD_PP_ESEM_KEY_VSI_DB_ID_LSB(unit), ARAD_PP_ESEM_KEY_VSI_DB_ID_NOF_BITS(unit), vsi_db_id);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (*vsi_db_id == ARAD_PP_ESEM_KEY_VSI_DB_ID_VAL) {
    
    res = soc_sand_bitstream_get_any_field(buffer, ARAD_PP_ESEM_KEY_VD_LSB, ARAD_PP_ESEM_KEY_VD_NOF_BITS(unit), &(esem_key->key_info.vd_vsi.vd));
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    res = soc_sand_bitstream_get_any_field(buffer, ARAD_PP_ESEM_KEY_VSI_LSB, ARAD_PP_ESEM_KEY_VSI_NOF_BITS, &(esem_key->key_info.vd_vsi.vsi));
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  } else {
    
    if (is_native && SOC_IS_JERICHO_PLUS(unit)) {
        
        res = soc_sand_bitstream_get_any_field(buffer, ARAD_PP_ESEM_KEY_LIF_VSI_VSI_LSB(unit), ARAD_PP_ESEM_KEY_LIF_VSI_VSI_NOF_BITS(unit), &(esem_key->key_info.lif_vsi.vsi));
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

        
        res = soc_sand_bitstream_get_any_field(buffer, ARAD_PP_ESEM_KEY_LIF_VSI_LIF_LSB, ARAD_PP_ESEM_KEY_LIF_VSI_LIF_NOF_BITS(unit), &(esem_key->key_info.lif_vsi.lif));
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    } else {
        
        res = soc_sand_bitstream_get_any_field(buffer, ARAD_PP_ESEM_KEY_VD_LSB, ARAD_PP_ESEM_KEY_VD_NOF_BITS(unit), &(esem_key->key_info.vd_cvid.vd));
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

        res = soc_sand_bitstream_get_any_field(buffer, ARAD_PP_ESEM_KEY_VSI_LSB, ARAD_PP_ESEM_KEY_VSI_NOF_BITS, &(esem_key->key_info.vd_cvid.vsi));
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

        res = soc_sand_bitstream_get_any_field(buffer, ARAD_PP_ESEM_KEY_CVID_LSB, ARAD_PP_ESEM_KEY_CVID_NOF_BITS, &(esem_key->key_info.vd_cvid.cvid));
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_esem_buffer_to_key()", 0, 0);
}


uint32
  arad_pp_esem_entry_to_buffer(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  ARAD_PP_ESEM_ENTRY     *esem_entry,
    SOC_SAND_OUT uint32                 *buffer
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_ESEM_ENTRY_TO_BUFFER);
  
  ARAD_PP_CLEAR(buffer, uint32, ARAD_PP_ESEM_ENTRY_SIZE);

  
  
  res = soc_sand_bitstream_set_any_field(&esem_entry->out_ac, ARAD_PP_ESEM_ENTRY_ESEM_PAYLOAD_NDX_LSB, ARAD_PP_ESEM_ENTRY_ESEM_PAYLOAD_NDX_NOF_BITS(unit), buffer);

  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_esem_entry_to_buffer()", 0, 0);
}

uint32
  arad_pp_esem_entry_from_buffer(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32               *buffer,
    SOC_SAND_OUT ARAD_PP_ESEM_ENTRY   *esem_entry
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_ESEM_ENTRY_FROM_BUFFER);
  
  ARAD_PP_CLEAR(esem_entry, ARAD_PP_ESEM_ENTRY, 1);

  
  
  res = soc_sand_bitstream_get_any_field(buffer, ARAD_PP_ESEM_ENTRY_ESEM_PAYLOAD_NDX_LSB, ARAD_PP_ESEM_ENTRY_ESEM_PAYLOAD_NDX_NOF_BITS(unit), &esem_entry->out_ac);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_esem_entry_from_buffer()", 0, 0);
}

uint32
  arad_pp_esem_entry_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_ESEM_KEY                            *esem_key,
    SOC_SAND_OUT ARAD_PP_ESEM_ENTRY                          *esem_entry,
    SOC_SAND_OUT uint8                                 *found
  )
{
  uint32
    reg_val,
    found_fld_val,
    payload_fld_val,
    lookup_trigger_val,
    res = SOC_SAND_OK;       
  uint32
    key_buffer[ARAD_PP_ESEM_KEY_SIZE];
  uint32
    entry_buffer[ARAD_PP_ESEM_ENTRY_SIZE];
  uint64
    key_buffer_64,
    diag_key_reg;
  uint8
    lkup_success;
  soc_reg_t diagnostics_reg             = SOC_IS_JERICHO_PLUS_A0(unit)?EDB_ESEM_0_DIAGNOSTICSr:EDB_ESEM_DIAGNOSTICSr;
  soc_reg_t diagnostics_key_reg         = SOC_IS_JERICHO_PLUS_A0(unit)?EDB_ESEM_0_DIAGNOSTICS_KEYr:EDB_ESEM_DIAGNOSTICS_KEYr;
  soc_reg_t diagnostics_lookup_result_reg = SOC_IS_JERICHO_PLUS_A0(unit)?EDB_ESEM_0_DIAGNOSTICS_LOOKUP_RESULTr:EDB_ESEM_DIAGNOSTICS_LOOKUP_RESULTr;
  soc_field_t diagnostics_lookup_field  = SOC_IS_JERICHO_PLUS_A0(unit)?ESEM_0_DIAGNOSTICS_LOOKUPf:ESEM_DIAGNOSTICS_LOOKUPf;
  soc_field_t diagnostics_key_field     = SOC_IS_JERICHO_PLUS_A0(unit)?ESEM_0_DIAGNOSTICS_KEYf:ESEM_DIAGNOSTICS_KEYf;
  soc_field_t diagnostics_entry_found   = SOC_IS_JERICHO_PLUS_A0(unit)?ESEM_0_ENTRY_FOUNDf:ESEM_ENTRY_FOUNDf;
  soc_field_t diagnostics_entry_payload = SOC_IS_JERICHO_PLUS_A0(unit)?ESEM_0_ENTRY_PAYLOADf:ESEM_ENTRY_PAYLOADf;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_ESEM_ENTRY_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(esem_key);
  SOC_SAND_CHECK_NULL_INPUT(esem_entry);
  SOC_SAND_CHECK_NULL_INPUT(found);

  ARAD_PP_CLEAR(entry_buffer, uint32, ARAD_PP_ESEM_ENTRY_SIZE);
  
  res = arad_pp_esem_key_to_buffer(unit, esem_key, key_buffer);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

#ifdef CRASH_RECOVERY_SUPPORT
  
  if (SOC_IS_DONE_INIT(unit))
  {
      if (BCM_UNIT_DO_HW_READ_WRITE(unit))
      {
          if (SOC_DPP_IS_EM_SIM_ENABLE(unit))
          {
                res = chip_sim_exact_match_entry_get_unsafe(
                          unit,
                          ARAD_CHIP_SIM_ESEM_BASE,
                          key_buffer,
                          ARAD_PP_ESEM_KEY_SIZE * sizeof(uint32),
                          entry_buffer,
                          ARAD_PP_ESEM_ENTRY_SIZE * sizeof(uint32),
                          &lkup_success
                      );
                SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);


                res = arad_pp_esem_entry_from_buffer(unit, entry_buffer, esem_entry);
                SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);

                *found = lkup_success;

                
                ARAD_PP_DO_NOTHING_AND_EXIT;
          }
      }
  }
#endif 


  
  COMPILER_64_SET(key_buffer_64,key_buffer[1],key_buffer[0]);
  COMPILER_64_ZERO(diag_key_reg);

  *found = FALSE;
  soc_reg64_field_set(unit, 
                      diagnostics_key_reg,
                      &diag_key_reg, 
                      diagnostics_key_field,
                      key_buffer_64);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1000, exit, soc_reg_set(unit, diagnostics_key_reg, REG_PORT_ANY, 0, diag_key_reg));
                               

  lookup_trigger_val = 0x1;
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  12,  exit, ARAD_REG_ACCESS_ERR,
                                       soc_reg_above_64_field32_modify(unit, 
                                                                       diagnostics_reg, 
                                                                       REG_PORT_ANY, 
                                                                       0, 
                                                                       diagnostics_lookup_field,  
                                                                       lookup_trigger_val));

  res = arad_polling(
          unit,
          ARAD_TIMEOUT,
          ARAD_MIN_POLLS,
          diagnostics_reg,
          REG_PORT_ANY,
          0,
          diagnostics_lookup_field,
          0
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  38,  exit, ARAD_REG_ACCESS_ERR, soc_reg32_get(unit, diagnostics_lookup_result_reg, REG_PORT_ANY, 0, &reg_val)); 
    
  ARAD_FLD_FROM_REG(diagnostics_lookup_result_reg, diagnostics_entry_found, found_fld_val, reg_val, 40, exit);
  lkup_success = SOC_SAND_NUM2BOOL(found_fld_val);

  if (lkup_success)
  {
    ARAD_FLD_FROM_REG(diagnostics_lookup_result_reg, diagnostics_entry_payload, payload_fld_val, reg_val, 42, exit);
    entry_buffer[0] = payload_fld_val;
  }
  else
  {
      if (SOC_DPP_IS_EM_SIM_ENABLE(unit))
      {
          res = chip_sim_exact_match_entry_get_unsafe(
                      unit,
                      ARAD_CHIP_SIM_ESEM_BASE,
                      key_buffer,
                      ARAD_PP_ESEM_KEY_SIZE * sizeof(uint32),
                      entry_buffer,
                      ARAD_PP_ESEM_ENTRY_SIZE * sizeof(uint32),
                      &lkup_success
                  );
            SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);
      }
  }

  res = arad_pp_esem_entry_from_buffer(unit, entry_buffer, esem_entry);
  SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);
  
  *found = lkup_success;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_esem_entry_get_unsafe()", 0, 0);
}

STATIC
  void
    arad_pp_esem_intern2fail(
      SOC_SAND_IN   uint32 intern_fail,
      SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE   *soc_sand_success_status
    )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);

  *soc_sand_success_status = SOC_SAND_SUCCESS;


  switch(intern_fail)
    {
    case 0x001:
    case 0x002:
    case 0x004:
    case 0x008:
    case 0x080:
    case 0x200:
    case 0x400:
      *soc_sand_success_status = SOC_SAND_FAILURE_OUT_OF_RESOURCES_2;
    break;
    default:
    break;  
  }
#if ARAD_PP_ESEM_ACCESS_DEBUG
  if (reason)
  {
    switch(reason)
    {
    case 0x001:
      LOG_INFO(BSL_LS_SOC_LIF,
               (BSL_META_U(unit,
                           "Notice: Change non-exist from self  ")));
      break;
    case 0x002:
      LOG_INFO(BSL_LS_SOC_LIF,
               (BSL_META_U(unit,
                           "Notice: Change non-exist from other ")));
      break;
    case 0x004:
      LOG_INFO(BSL_LS_SOC_LIF,
               (BSL_META_U(unit,
                           "Notice: Change request over static  ")));
      break;
    case 0x008:
      LOG_INFO(BSL_LS_SOC_LIF,
               (BSL_META_U(unit,
                           "Notice: Change-fail non exist       ")));
      break;
    case 0x010:
      LOG_INFO(BSL_LS_SOC_LIF,
               (BSL_META_U(unit,
                           "Notice: Learn over existing         ")));
      break;
    case 0x020:
      LOG_INFO(BSL_LS_SOC_LIF,
               (BSL_META_U(unit,
                           "Notice: Learn request over static   ")));
      break;
    case 0x040:
      LOG_INFO(BSL_LS_SOC_LIF,
               (BSL_META_U(unit,
                           "Notice: Inserted existing           ")));
      break;
    case 0x080:
      LOG_INFO(BSL_LS_SOC_LIF,
               (BSL_META_U(unit,
                           "Notice: Reached max entry limit     ")));
      break;
    case 0x100:
      LOG_INFO(BSL_LS_SOC_LIF,
               (BSL_META_U(unit,
                           "Notice: Delete unknown key          ")));
      break;
    case 0x200:
      LOG_INFO(BSL_LS_SOC_LIF,
               (BSL_META_U(unit,
                           "Notice: Table coherence             ")));
      break;
    case 0x400:
      LOG_INFO(BSL_LS_SOC_LIF,
               (BSL_META_U(unit,
                           "Notice: Cam table full              ")));
      break;
    }
  }
#endif

  SOC_SAND_VOID_EXIT_AND_SEND_ERROR("Error, in arad_pp_esem_intern2fail", 0, 0);
}

uint32
  arad_pp_esem_tbl_write(
    SOC_SAND_IN    int             unit,
    SOC_SAND_IN    uint32              key_buffer[ARAD_PP_ESEM_KEY_SIZE],
    SOC_SAND_INOUT uint32              esem_buffer[ARAD_PP_ESEM_ENTRY_REQ_SIZE_REGS],
    SOC_SAND_OUT   SOC_SAND_SUCCESS_FAILURE  *success
  )
{
  uint32    
    fail_valid_val,
    fail_reason_val;
  uint64
    key_buffer_64,
    fail_key_val,
    failure;  
  SOC_SAND_SUCCESS_FAILURE
    success_status;
  uint32 res;
  soc_reg_t interrupt_reg             = SOC_IS_JERICHO_PLUS_A0(unit)?EDB_ESEM_0_INTERRUPT_REGISTER_ONEr:EDB_ESEM_INTERRUPT_REGISTER_ONEr;
  soc_field_t esem_management_completed_field     = SOC_IS_JERICHO_PLUS_A0(unit)?ESEM_0_MANAGEMENT_COMPLETEDf:ESEM_MANAGEMENT_COMPLETEDf;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_ESEM_TBL_WRITE);

  SOC_SAND_CHECK_NULL_INPUT(success);
  SOC_SAND_CHECK_NULL_INPUT(esem_buffer);
   

  if(SOC_IS_JERICHO_PLUS_A0(unit)){
      
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 1100, exit, READ_EDB_ESEM_0_MANAGEMENT_UNIT_FAILUREr(unit,&failure));
      
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 1110, exit, WRITE_EDB_ESEM_0_MANAGEMENT_REQUESTm(unit, MEM_BLOCK_ALL, 0, esem_buffer));
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 1110, exit, WRITE_EDB_ESEM_1_MANAGEMENT_REQUESTm(unit, MEM_BLOCK_ALL, 0, esem_buffer));
  }
  else{
      
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 1100, exit, READ_EDB_ESEM_MANAGEMENT_UNIT_FAILUREr(unit,&failure));
      
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 1110, exit, WRITE_EDB_ESEM_MANAGEMENT_REQUESTm(unit, MEM_BLOCK_ALL, 0, esem_buffer));
  }

  


  res = arad_polling(
          unit,
          ARAD_TIMEOUT,
          ARAD_MIN_POLLS,
          interrupt_reg,
          REG_PORT_ANY,
          0,
          esem_management_completed_field,
          1
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

#ifdef CRASH_RECOVERY_SUPPORT
  if (SOC_IS_DONE_INIT(unit))
  {
      if (BCM_UNIT_DO_HW_READ_WRITE(unit))
      {
          
          *success = SOC_SAND_SUCCESS;
          ARAD_PP_DO_NOTHING_AND_EXIT;
      }
  }
#endif 

  if(SOC_IS_JERICHO_PLUS_A0(unit)){
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 40, exit, READ_EDB_ESEM_0_MANAGEMENT_UNIT_FAILUREr(unit,&failure));
      fail_valid_val = soc_reg64_field32_get(unit, EDB_ESEM_0_MANAGEMENT_UNIT_FAILUREr, failure, ESEM_0_MNGMNT_UNIT_FAILURE_VALIDf);
  }
  else{
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 40, exit, READ_EDB_ESEM_MANAGEMENT_UNIT_FAILUREr(unit,&failure));
      fail_valid_val = soc_reg64_field32_get(unit, EDB_ESEM_MANAGEMENT_UNIT_FAILUREr, failure, ESEM_MNGMNT_UNIT_FAILURE_VALIDf);
  }

    if (fail_valid_val)
    {
      
      if(SOC_IS_JERICHO_PLUS_A0(unit)){
          fail_key_val = soc_reg64_field_get(unit, EDB_ESEM_0_MANAGEMENT_UNIT_FAILUREr, failure, ESEM_0_MNGMNT_UNIT_FAILURE_KEYf);
      }
      else{
          fail_key_val = soc_reg64_field_get(unit, EDB_ESEM_MANAGEMENT_UNIT_FAILUREr, failure, ESEM_MNGMNT_UNIT_FAILURE_KEYf);
      }
      
      
      COMPILER_64_SET(key_buffer_64,key_buffer[1],key_buffer[0]);

      if (COMPILER_64_NE(fail_key_val,key_buffer_64))
      {
        SOC_SAND_SET_ERROR_CODE(ARAD_PP_ESEM_FAIL_ON_DIFFERENT_KEY_ERR, 50, exit);
      }

      if(SOC_IS_JERICHO_PLUS_A0(unit)){
          fail_reason_val = soc_reg64_field32_get(unit, EDB_ESEM_0_MANAGEMENT_UNIT_FAILUREr, failure, ESEM_0_MNGMNT_UNIT_FAILURE_REASONf);
      }
      else{
          fail_reason_val = soc_reg64_field32_get(unit, EDB_ESEM_MANAGEMENT_UNIT_FAILUREr, failure, ESEM_MNGMNT_UNIT_FAILURE_REASONf);
      }
      arad_pp_esem_intern2fail(fail_reason_val, &success_status);
      *success = success_status;
    }
    else
    {
      *success = SOC_SAND_SUCCESS;
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_esem_tbl_write()", 0, 0);
}

uint32
  arad_pp_esem_entry_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_ESEM_KEY                            *esem_key,
    SOC_SAND_IN  ARAD_PP_ESEM_ENTRY                          *esem_entry,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    key_type,
    tmp,
    buffer[ARAD_PP_ESEM_ENTRY_REQ_SIZE_REGS];
  uint32
    key_buffer[ARAD_PP_ESEM_KEY_SIZE];
  uint32
    entry_buffer[ARAD_PP_ESEM_ENTRY_SIZE];       
  SOC_SAND_SUCCESS_FAILURE
    success_status;
  uint8
    is_success = TRUE;


  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_ESEM_ENTRY_ADD_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(esem_key);
  SOC_SAND_CHECK_NULL_INPUT(esem_entry);
  SOC_SAND_CHECK_NULL_INPUT(success); 
  
  success_status = SOC_SAND_FAILURE_OUT_OF_RESOURCES;
  ARAD_PP_CLEAR(buffer, uint32, ARAD_PP_ESEM_ENTRY_REQ_SIZE_REGS);

  res = arad_pp_esem_key_to_buffer(unit, esem_key, key_buffer);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  res = arad_pp_esem_entry_to_buffer(unit, esem_entry, entry_buffer);
  SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

  key_type = arad_pp_esem_key2intern(ARAD_PP_ESEM_ACTION_TYPE_INSERT);
  res = soc_sand_bitstream_set_any_field(
         &key_type,
         ARAD_PP_ESEM_ENTRY_REQ_TYPE_LSB,
         ARAD_PP_ESEM_ENTRY_REQ_TYPE_NOF_BITS,
         buffer
       );
  SOC_SAND_CHECK_FUNC_RESULT(res, 14, exit);

  tmp = ARAD_PP_ESEM_ENTRY_REQ_STAMP_VAL;
  res = soc_sand_bitstream_set_any_field(
         &tmp,
         ARAD_PP_ESEM_ENTRY_REQ_STAMP_LSB,
         ARAD_PP_ESEM_ENTRY_REQ_STAMP_NOF_BITS,
         buffer
       );
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);
  
  
  res = soc_sand_bitstream_set_any_field(
          key_buffer,
          ARAD_PP_ESEM_ENTRY_REQ_KEY_LSB,
          ARAD_PP_ESEM_ENTRY_REQ_KEY_NOF_BITS(unit),
          buffer
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 16, exit);

  res = soc_sand_bitstream_set_any_field(
          entry_buffer,
          ARAD_PP_ESEM_ENTRY_REQ_PAYLOAD_LSB(unit),
          ARAD_PP_ESEM_ENTRY_REQ_PAYLOAD_NOF_BITS(unit),
          buffer
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 18, exit);

  res = arad_pp_esem_tbl_write(
          unit,
          key_buffer,
          buffer,
          &success_status
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

   if (SOC_SAND_SUCCESS2BOOL(success_status) && SOC_DPP_IS_EM_SIM_ENABLE(unit))
   {
      res = chip_sim_exact_match_entry_add_unsafe(
                  unit,
                  ARAD_CHIP_SIM_ESEM_BASE,
                  key_buffer,
                  ARAD_PP_ESEM_KEY_SIZE * sizeof(uint32),
                  entry_buffer,
                  ARAD_PP_ESEM_ENTRY_SIZE * sizeof(uint32),
                  &is_success
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

      success_status = SOC_SAND_BOOL2SUCCESS(is_success);
   }
    
   
  *success = success_status;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_esem_entry_add_unsafe()", 0, 0);
}

uint32
  arad_pp_esem_entry_remove_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_ESEM_KEY                            *esem_key,
    SOC_SAND_OUT uint8                                 *success
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    key_type,
    tmp,
    buffer[ARAD_PP_ESEM_ENTRY_REQ_SIZE_REGS];
  uint32
    key_buffer[ARAD_PP_ESEM_KEY_SIZE];   
  SOC_SAND_SUCCESS_FAILURE
    success_status;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_ESEM_ENTRY_REMOVE_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(esem_key);
  SOC_SAND_CHECK_NULL_INPUT(success);  

  ARAD_PP_CLEAR(buffer, uint32, ARAD_PP_ESEM_ENTRY_REQ_SIZE_REGS);

  res = arad_pp_esem_key_to_buffer(unit, esem_key, key_buffer);
  SOC_SAND_CHECK_FUNC_RESULT(res, 16, exit);

  key_type = arad_pp_esem_key2intern(ARAD_PP_ESEM_ACTION_TYPE_DELETE);

  res = soc_sand_bitstream_set_any_field(
          &key_type,
          ARAD_PP_ESEM_ENTRY_REQ_TYPE_LSB,
          ARAD_PP_ESEM_ENTRY_REQ_TYPE_NOF_BITS,
          buffer
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 18, exit);

  tmp = ARAD_PP_ESEM_ENTRY_REQ_STAMP_VAL;
  res = soc_sand_bitstream_set_any_field(
         &tmp,
         ARAD_PP_ESEM_ENTRY_REQ_STAMP_LSB,
         ARAD_PP_ESEM_ENTRY_REQ_STAMP_NOF_BITS,
         buffer
       );
  SOC_SAND_CHECK_FUNC_RESULT(res, 14, exit);

  
  
  res = soc_sand_bitstream_set_any_field(
          key_buffer,
          ARAD_PP_ESEM_ENTRY_REQ_KEY_LSB,
          ARAD_PP_ESEM_ENTRY_REQ_KEY_NOF_BITS(unit),
          buffer
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = arad_pp_esem_tbl_write(
          unit,
          key_buffer,
          buffer,
          &success_status
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  if (SOC_SAND_SUCCESS2BOOL(success_status) && SOC_DPP_IS_EM_SIM_ENABLE(unit))
  {
        res = chip_sim_exact_match_entry_remove_unsafe(
                unit,
                ARAD_CHIP_SIM_ESEM_BASE,
                key_buffer,
                ARAD_PP_ESEM_KEY_SIZE * sizeof(uint32)
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 190, exit);

        success_status = SOC_SAND_SUCCESS;
  }
    
  *success = SOC_SAND_SUCCESS2BOOL(success_status);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_esem_entry_remove_unsafe()", 0, 0);
}


CONST SOC_PROCEDURE_DESC_ELEMENT*
  arad_pp_esem_get_procs_ptr(void)
{
  return Arad_pp_procedure_desc_element_esem;
}

CONST SOC_ERROR_DESC_ELEMENT*
  arad_pp_esem_get_errs_ptr(void)
{
  return Arad_pp_error_desc_element_esem;
}



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif 


