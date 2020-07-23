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

#include <soc/mem.h>
#define _ERR_MSG_MODULE_NAME BSL_SOC_PTP

#include <shared/bsl.h>



#include <soc/dcmn/error.h>

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>


#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_ptp.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_general.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <soc/dpp/ARAD/arad_parser.h>

#include <soc/dpp/ARAD/arad_general.h>

#include <soc/dpp/ARAD/arad_tbl_access.h>

#include <soc/dpp/drv.h>






enum {

	ARAD_PP_1588_IDENTIFICATION_CAM_TABLE_ENTRY_0  =  0,
	ARAD_PP_1588_IDENTIFICATION_CAM_TABLE_ENTRY_1  =  1,
	ARAD_PP_1588_IDENTIFICATION_CAM_TABLE_ENTRY_2  =  2,
	ARAD_PP_1588_IDENTIFICATION_CAM_TABLE_ENTRY_3  =  3,
	ARAD_PP_1588_IDENTIFICATION_CAM_TABLE_ENTRY_4  =  4,
	ARAD_PP_1588_IDENTIFICATION_CAM_TABLE_ENTRY_5  =  5,
	ARAD_PP_1588_IDENTIFICATION_CAM_TABLE_ENTRY_6  =  6,
	ARAD_PP_1588_IDENTIFICATION_CAM_TABLE_ENTRY_7  =  7,
	ARAD_PP_1588_IDENTIFICATION_CAM_TABLE_ENTRY_8  =  8,
	ARAD_PP_1588_IDENTIFICATION_CAM_TABLE_ENTRY_9  =  9,
	ARAD_PP_1588_IDENTIFICATION_CAM_TABLE_ENTRY_10 = 10,
	ARAD_PP_1588_IDENTIFICATION_CAM_TABLE_ENTRY_11 = 11,
	ARAD_PP_1588_IDENTIFICATION_CAM_TABLE_ENTRY_12 = 12,
	ARAD_PP_1588_IDENTIFICATION_CAM_TABLE_ENTRY_13 = 13,
	ARAD_PP_1588_IDENTIFICATION_CAM_TABLE_ENTRY_14 = 14,
	ARAD_PP_1588_IDENTIFICATION_CAM_TABLE_ENTRY_15 = 15,

    ARAD_PP_1588_IDENTIFICATION_CAM_TABLE_ENTRIES_NUM
};



#define ARAD_PP_1588_PROGRAM_FIELD_PACKET_IS_IEEE_1588_SHIFT            0 
#define ARAD_PP_1588_PROGRAM_FIELD_PACKET_IS_IEEE_1588_MASK           0x1 
#define ARAD_PP_1588_PROGRAM_FIELD_ENCAPSULATION_SHIFT                  1
#define ARAD_PP_1588_PROGRAM_FIELD_ENCAPSULATION_MASK                 0x1 
#define ARAD_PP_1588_PROGRAM_FIELD_CARRY_1588_HEADER_INDEX_SHIFT        2
#define ARAD_PP_1588_PROGRAM_FIELD_CARRY_1588_HEADER_INDEX_MASK       0x7 



#define ARAD_PP_1588_ACTION_FIELD_UPDATE_TIMESTAMP_SHIFT               0 
#define ARAD_PP_1588_ACTION_FIELD_UPDATE_TIMESTAMP_MASK              0x1 
#define ARAD_PP_1588_ACTION_FIELD_COMMAND_SHIFT                        1
#define ARAD_PP_1588_ACTION_FIELD_COMMAND_MASK                       0x3 
#define ARAD_PP_1588_ACTION_FIELD_ACTION_INDEX_SHIFT                   3
#define ARAD_PP_1588_ACTION_FIELD_ACTION_INDEX_MASK                  0x7 




#define ARAD_PP_1588_ACTION_TABLE_KEY_MESSAGE_TYPE_SHIFT               1 
#define ARAD_PP_1588_ACTION_TABLE_KEY_MESSAGE_TYPE_MASK              0xf 


enum {

	ARAD_PP_1588_EGRESS_COMMAND_0_NO_STAMPING_NO_RECORDING       = 0,
	ARAD_PP_1588_EGRESS_COMMAND_1_RX_STAMPING_AND_TX_CF_STAMPING = 1,
	ARAD_PP_1588_EGRESS_COMMAND_2_TX_RECORDING                   = 2
};







#define ARAD_PP_1588_SET_PROGRAM_FIELD(program_field, encap, carry_header)   \
    program_field = 0; \
    program_field |= ((1            & ARAD_PP_1588_PROGRAM_FIELD_PACKET_IS_IEEE_1588_MASK)     << ARAD_PP_1588_PROGRAM_FIELD_PACKET_IS_IEEE_1588_SHIFT); \
    program_field |= ((encap        & ARAD_PP_1588_PROGRAM_FIELD_ENCAPSULATION_MASK)           << ARAD_PP_1588_PROGRAM_FIELD_ENCAPSULATION_SHIFT); \
    program_field |= ((carry_header & ARAD_PP_1588_PROGRAM_FIELD_CARRY_1588_HEADER_INDEX_MASK) << ARAD_PP_1588_PROGRAM_FIELD_CARRY_1588_HEADER_INDEX_SHIFT)










CONST STATIC SOC_PROCEDURE_DESC_ELEMENT
  Arad_pp_procedure_desc_element_ptp[] =
{
  
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_PTP_INIT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_PTP_INIT_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_PTP_PORT_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_PTP_PORT_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_PTP_PORT_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_PTP_PORT_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_PTP_PORT_SET_ACTION_PROFILE),
  
   
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_PTP_PORT_SET_VERIFY),
  
  
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

CONST STATIC SOC_ERROR_DESC_ELEMENT
  Arad_pp_error_desc_element_ptp[] =
{
  
  {
    ARAD_PP_PTP_ACTION_PROFILE_OUT_OF_RANGE_ERR,
    "ARAD_PP_PTP_ACTION_PROFILE_OUT_OF_RANGE_ERR",
 	"ARAD_PP_PTP action profile is out of range. \n\r "
    "The range is: 0 - 3.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },


  

  {
    ARAD_PP_PTP_USER_TRAP_ID_OUT_OF_RANGE_ERR,
    "ARAD_PP_PTP_USER_TRAP_ID_OUT_OF_RANGE_ERR",
    "ARAD_PP_PTP user trap id is out of range. \n\r "
    "user_trap_id should be of bcmRxTrap1588UserX trap only! "
    "Use bcm_rx_trap_type_create to allocate a valid trap_id. \n\r",
    SOC_SAND_SVR_ERR,
    FALSE
  },

  
SOC_ERR_DESC_ELEMENT_DEF_LAST
};








uint32
arad_pp_ptp_init_unsafe(
                        SOC_SAND_IN  int                                 unit
                        )
{
    uint32 res = SOC_SAND_OK;
    uint32 epni_reg = 0;
    uint32 tbl_data[2];
    uint32 program, command;
    uint32 i, stamp_bit_mask, val, mask;
    int egress_command, index_max, core_id;

#define ANY       -1      

#ifdef NONE   
#undef NONE     
#endif

#define NONE      ARAD_PP_L3_NEXT_PRTCL_NDX_NONE    

#define I1588oU1    (ARAD_PARSER_CUSTOM_MACRO_PROTO_10 + 1)     
#define I1588oU2    (ARAD_PARSER_CUSTOM_MACRO_PROTO_11 + 1)     
#define I1588oU     ((I1588oU1)&(I1588oU2))                     
#define I1588oU_MSK ((I1588oU1)^(I1588oU2))                     

#define I1588oE   ARAD_PARSER_ETHER_PROTO_6_1588 + 1
#define UDP       ARAD_PP_L3_NEXT_PRTCL_NDX_UDP     
#define IPV4      ARAD_PP_L3_NEXT_PRTCL_NDX_IPV4    
#define IPV6      ARAD_PP_L3_NEXT_PRTCL_NDX_IPV6    
#define MPLS      ARAD_PP_L3_NEXT_PRTCL_NDX_MPLS    


#define ENC_UDP   0
#define ENC_ETH   1


#define IP        ((IPV4)&(IPV6))                   
#define IP_MSK    ((IPV4)^(IPV6))                   




   int ieee_1588_identification_cam[ARAD_PP_1588_IDENTIFICATION_CAM_TABLE_ENTRIES_NUM][9] =
       
       
       
        {{1,     I1588oE, NONE   , NONE   , NONE   , NONE   , 1,         ENC_ETH, 1 }, 
         {1,     IP     , UDP    , I1588oU, NONE   , NONE   , 1,         ENC_UDP, 3 }, 
         {1,     IP     , UDP    , I1588oU, NONE   , NONE   , 2,         ENC_UDP, 3 }, 
         {1,     IP     , IP     , UDP    , I1588oU, NONE   , 1,         ENC_UDP, 4 }, 
         {1,     IP     , IP     , UDP    , I1588oU, NONE   , 2,         ENC_UDP, 4 }, 
         {1,     IP     , IP     , UDP    , I1588oU, NONE   , 3,         ENC_UDP, 4 }, 
         {1,     MPLS   , ANY    , UDP    , I1588oU, NONE   , 1,         ENC_UDP, 4 }, 
         {1,     MPLS   , ANY    , UDP    , I1588oU, NONE   , 2,         ENC_UDP, 4 }, 
         {1,     MPLS   , ANY    , UDP    , I1588oU, NONE   , 3,         ENC_UDP, 4 }, 
         {1,     MPLS   , ANY    , I1588oE, NONE   , NONE   , 1,         ENC_ETH, 3 }, 
         {1,     MPLS   , ANY    , I1588oE, NONE   , NONE   , 2,         ENC_ETH, 3 }, 
         {1,     MPLS   , ANY    , I1588oE, NONE   , NONE   , 3,         ENC_ETH, 3 }, 
         {1,     MPLS   , ANY    , IP     , UDP    , I1588oU, 1,         ENC_UDP, 5 }, 
         {1,     MPLS   , ANY    , IP     , UDP    , I1588oU, 2,         ENC_UDP, 5 }, 
         {1,     MPLS   , ANY    , IP     , UDP    , I1588oU, 3,         ENC_UDP, 5 }, 
         {0,     NONE   , NONE   , NONE   , NONE   , NONE   , 0,         0      , 0 }  
        };
        

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_PTP_INIT_UNSAFE);

    
#ifdef BCM_88660_A0
    if(SOC_IS_ARADPLUS(unit)) {

        SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_EPNI_CFG_48_BITS_1588_TS_ENABLEr(unit, REG_PORT_ANY, &epni_reg)); 
        soc_reg_field_set(unit, EPNI_CFG_48_BITS_1588_TS_ENABLEr, &epni_reg, CFG_48_BITS_1588_TS_ENABLEf, 
                          (SOC_DPP_CONFIG(unit))->pp.ptp_48b_stamping_enable ? 1 : 0); 
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_EPNI_CFG_48_BITS_1588_TS_ENABLEr(unit, REG_PORT_ANY, epni_reg)); 

     }
#endif 


    
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_EPNI_IEEE_1588r(unit, REG_PORT_ANY, &epni_reg)); 
    soc_reg_field_set(unit, EPNI_IEEE_1588r, &epni_reg, IEEE_1588_TS_OFFSET_FIXf, 0); 

    
    stamp_bit_mask = 0;
    stamp_bit_mask |= 0 << ARAD_PP_1588_EGRESS_COMMAND_0_NO_STAMPING_NO_RECORDING;
    stamp_bit_mask |= 1 << ARAD_PP_1588_EGRESS_COMMAND_1_RX_STAMPING_AND_TX_CF_STAMPING;
    stamp_bit_mask |= 0 << ARAD_PP_1588_EGRESS_COMMAND_2_TX_RECORDING;
    
    soc_reg_field_set(unit, EPNI_IEEE_1588r, &epni_reg, IEEE_1588_RX_STAMPf     , stamp_bit_mask ); 
    stamp_bit_mask = 0;
    stamp_bit_mask |= 0 << ARAD_PP_1588_EGRESS_COMMAND_0_NO_STAMPING_NO_RECORDING;
    stamp_bit_mask |= 1 << ARAD_PP_1588_EGRESS_COMMAND_1_RX_STAMPING_AND_TX_CF_STAMPING;
    stamp_bit_mask |= 0 << ARAD_PP_1588_EGRESS_COMMAND_2_TX_RECORDING;
    
    soc_reg_field_set(unit, EPNI_IEEE_1588r, &epni_reg, IEEE_1588_TX_STAMP_CFf  , stamp_bit_mask ); 
    stamp_bit_mask = 0;
    stamp_bit_mask |= 0 << ARAD_PP_1588_EGRESS_COMMAND_0_NO_STAMPING_NO_RECORDING;
    stamp_bit_mask |= 0 << ARAD_PP_1588_EGRESS_COMMAND_1_RX_STAMPING_AND_TX_CF_STAMPING;
    stamp_bit_mask |= 1 << ARAD_PP_1588_EGRESS_COMMAND_2_TX_RECORDING;
    
    soc_reg_field_set(unit, EPNI_IEEE_1588r, &epni_reg, IEEE_1588_RECORD_TSf  , stamp_bit_mask   ); 

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_EPNI_IEEE_1588r(unit, REG_PORT_ANY, epni_reg)); 


    sal_memset(tbl_data, 0, sizeof (tbl_data));

    
    
    index_max = soc_mem_index_max(unit, IHP_IEEE_1588_ACTIONm);

    for(i = 0; i <= index_max ; i++) {

        egress_command = ARAD_PP_1588_EGRESS_COMMAND_0_NO_STAMPING_NO_RECORDING;

        command = 0;
        command |= ((0              & ARAD_PP_1588_ACTION_FIELD_UPDATE_TIMESTAMP_MASK) << ARAD_PP_1588_ACTION_FIELD_UPDATE_TIMESTAMP_SHIFT);
        command |= ((egress_command & ARAD_PP_1588_ACTION_FIELD_COMMAND_MASK)          << ARAD_PP_1588_ACTION_FIELD_COMMAND_SHIFT);
        command |= ((0              & ARAD_PP_1588_ACTION_FIELD_ACTION_INDEX_MASK)     << ARAD_PP_1588_ACTION_FIELD_ACTION_INDEX_SHIFT);

        soc_IHP_IEEE_1588_ACTIONm_field32_set(unit, tbl_data, ACTIONf,   0);
        soc_IHP_IEEE_1588_ACTIONm_field32_set(unit, tbl_data, ACTION_1f, 0);
        soc_IHP_IEEE_1588_ACTIONm_field32_set(unit, tbl_data, ACTION_2f, 0);
        soc_IHP_IEEE_1588_ACTIONm_field32_set(unit, tbl_data, ACTION_3f, 0);


        SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, 
                                     WRITE_IHP_IEEE_1588_ACTIONm(unit, 
                                                                 MEM_BLOCK_ANY, 
                                                                 i,
                                                                 tbl_data));
   }


    
    index_max = soc_mem_index_max(unit, IHB_PINFO_LBPm);

    SOC_DPP_CORES_ITER(SOC_CORE_ALL, core_id) {
        for(i = 0; i <= index_max ; i++) {

            SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, READ_IHB_PINFO_LBPm(unit,  IHB_BLOCK(unit, core_id), i, tbl_data));
            soc_IHB_PINFO_LBPm_field32_set(unit, tbl_data, INGRESS_P2P_DELAYf, 0);
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 40, exit, WRITE_IHB_PINFO_LBPm(unit, IHB_BLOCK(unit, core_id), i, tbl_data));

        }
    }
 

    sal_memset(tbl_data, 0, sizeof (tbl_data));

    
    for(i = 0; i < ARAD_PP_1588_IDENTIFICATION_CAM_TABLE_ENTRIES_NUM; i++) {
        
        
        if (ieee_1588_identification_cam[i][0] == 0) {
            
            soc_IHP_IEEE_1588_IDENTIFICATION_CAMm_field32_set(unit, tbl_data, VALIDf, 0);
            continue;
        }

        
        soc_IHP_IEEE_1588_IDENTIFICATION_CAMm_field32_set(unit, tbl_data, VALIDf, 1);

        
        
        
        
        
        mask = (ANY == ieee_1588_identification_cam[i][1]) ? 0xf : 
            ((IP == ieee_1588_identification_cam[i][1]) ? IP_MSK : ((I1588oU == ieee_1588_identification_cam[i][1]) ? I1588oU_MSK : 0)); 
        val  = (ANY == ieee_1588_identification_cam[i][1]) ? 0   : ieee_1588_identification_cam[i][1];
        soc_IHP_IEEE_1588_IDENTIFICATION_CAMm_field32_set(unit, tbl_data, QUALIFIER_1_NEXT_PROTOCOL_MASKf, mask);
        soc_IHP_IEEE_1588_IDENTIFICATION_CAMm_field32_set(unit, tbl_data, QUALIFIER_1_NEXT_PROTOCOLf,      val);

        mask = (ANY == ieee_1588_identification_cam[i][2]) ? 0xf : 
            ((IP == ieee_1588_identification_cam[i][2]) ? IP_MSK : ((I1588oU == ieee_1588_identification_cam[i][2]) ? I1588oU_MSK : 0)); 
        val  = (ANY == ieee_1588_identification_cam[i][2]) ? 0   : ieee_1588_identification_cam[i][2];
        soc_IHP_IEEE_1588_IDENTIFICATION_CAMm_field32_set(unit, tbl_data, QUALIFIER_2_NEXT_PROTOCOL_MASKf, mask);
        soc_IHP_IEEE_1588_IDENTIFICATION_CAMm_field32_set(unit, tbl_data, QUALIFIER_2_NEXT_PROTOCOLf,      val);

        mask = (ANY == ieee_1588_identification_cam[i][3]) ? 0xf : 
            ((IP == ieee_1588_identification_cam[i][3]) ? IP_MSK : ((I1588oU == ieee_1588_identification_cam[i][3]) ? I1588oU_MSK : 0)); 
        val  = (ANY == ieee_1588_identification_cam[i][3]) ? 0   : ieee_1588_identification_cam[i][3];
        soc_IHP_IEEE_1588_IDENTIFICATION_CAMm_field32_set(unit, tbl_data, QUALIFIER_3_NEXT_PROTOCOL_MASKf, mask);
        soc_IHP_IEEE_1588_IDENTIFICATION_CAMm_field32_set(unit, tbl_data, QUALIFIER_3_NEXT_PROTOCOLf,      val);

        mask = (ANY == ieee_1588_identification_cam[i][4]) ? 0xf : 
            ((IP == ieee_1588_identification_cam[i][4]) ? IP_MSK : ((I1588oU == ieee_1588_identification_cam[i][4]) ? I1588oU_MSK : 0)); 
        val  = (ANY == ieee_1588_identification_cam[i][4]) ? 0   : ieee_1588_identification_cam[i][4];
        soc_IHP_IEEE_1588_IDENTIFICATION_CAMm_field32_set(unit, tbl_data, QUALIFIER_4_NEXT_PROTOCOL_MASKf, mask);
        soc_IHP_IEEE_1588_IDENTIFICATION_CAMm_field32_set(unit, tbl_data, QUALIFIER_4_NEXT_PROTOCOLf,      val);

        mask = (ANY == ieee_1588_identification_cam[i][5]) ? 0xf : 
            ((IP == ieee_1588_identification_cam[i][5]) ? IP_MSK : ((I1588oU == ieee_1588_identification_cam[i][5]) ? I1588oU_MSK : 0)); 
        val  = (ANY == ieee_1588_identification_cam[i][5]) ? 0   : ieee_1588_identification_cam[i][5];
        soc_IHP_IEEE_1588_IDENTIFICATION_CAMm_field32_set(unit, tbl_data, QUALIFIER_5_NEXT_PROTOCOL_MASKf, mask);
        soc_IHP_IEEE_1588_IDENTIFICATION_CAMm_field32_set(unit, tbl_data, QUALIFIER_5_NEXT_PROTOCOLf,      val);

        
        soc_IHP_IEEE_1588_IDENTIFICATION_CAMm_field32_set(unit, tbl_data, FORWARDING_OFFSET_INDEX_MASKf, 0);
        soc_IHP_IEEE_1588_IDENTIFICATION_CAMm_field32_set(unit, tbl_data, FORWARDING_OFFSET_INDEXf     , ieee_1588_identification_cam[i][6]);

        
        soc_IHP_IEEE_1588_IDENTIFICATION_CAMm_field32_set(unit, tbl_data, PARSER_LEAF_CONTEXT_MASKf, ARAD_PARSER_PLC_MATCH_ANY);
        soc_IHP_IEEE_1588_IDENTIFICATION_CAMm_field32_set(unit, tbl_data, PARSER_LEAF_CONTEXTf, 0);

        
        
        
        
        
        
        
        
        ARAD_PP_1588_SET_PROGRAM_FIELD(program, ieee_1588_identification_cam[i][7], ieee_1588_identification_cam[i][8]);
        soc_IHP_IEEE_1588_IDENTIFICATION_CAMm_field32_set(unit, tbl_data, PROGRAMf, program);


 
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 40, exit, 
                                     WRITE_IHP_IEEE_1588_IDENTIFICATION_CAMm(unit, 
                                                                             MEM_BLOCK_ANY, 
                                                                             i,
                                                                             tbl_data));
    } 


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ptp_init_unsafe()", 0, 0);
}



STATIC uint32
  arad_pp_ptp_port_set_action_profile(
    SOC_SAND_IN  int                                    unit,
    SOC_SAND_IN  SOC_PPC_PTP_PORT_INFO                  *info,
    SOC_SAND_IN  SOC_PPC_PTP_IN_PP_PORT_PROFILE         profile
                              
  )
{
    uint32
        res = SOC_SAND_OK;
    int action_field = 0, i;
    int egress_command, index_max, action_index = 0, msg_type;
    uint32 command, is_hw_trap_id;
    uint32 tbl_data[2] = {0};

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_PTP_PORT_SET_ACTION_PROFILE);

    index_max = soc_mem_index_max(unit, IHP_IEEE_1588_ACTIONm);

    for(i = 0; i < index_max + 1; i++) {

        msg_type = ((i >> ARAD_PP_1588_ACTION_TABLE_KEY_MESSAGE_TYPE_SHIFT) & ARAD_PP_1588_ACTION_TABLE_KEY_MESSAGE_TYPE_MASK);

        
        
        
        
        
        
        
        
        

        
        
        egress_command = ARAD_PP_1588_EGRESS_COMMAND_0_NO_STAMPING_NO_RECORDING;

        
        if(0 != (info->flags & SOC_PPC_PTP_PORT_TIMESYNC_ONE_STEP_TIMESTAMP)) {
            
            if(SOC_PPC_PTP_1588_PKT_SYNC        == msg_type ||
               SOC_PPC_PTP_1588_PKT_DELAY_REQ   == msg_type ||
               SOC_PPC_PTP_1588_PKT_PDELAY_REQ  == msg_type ||
               SOC_PPC_PTP_1588_PKT_PDELAY_RESP == msg_type) {
                
                egress_command = ARAD_PP_1588_EGRESS_COMMAND_1_RX_STAMPING_AND_TX_CF_STAMPING;
            }
        } else if(0 != (info->flags & SOC_PPC_PTP_PORT_TIMESYNC_TWO_STEP_TIMESTAMP)) {
            
            if(SOC_PPC_PTP_1588_PKT_SYNC        == msg_type ||
               SOC_PPC_PTP_1588_PKT_DELAY_REQ   == msg_type ||
               SOC_PPC_PTP_1588_PKT_PDELAY_REQ  == msg_type ||
               SOC_PPC_PTP_1588_PKT_PDELAY_RESP == msg_type) {
                
                egress_command = ARAD_PP_1588_EGRESS_COMMAND_2_TX_RECORDING;
            }
        }

        

        if( ((info->pkt_drop & (1 << msg_type)) == 0) && ((info->pkt_tocpu & (1 << msg_type)) == 0) ) {
            action_index = SOC_PPC_PTP_ACTION_FIELD_ACTION_INDEX_FWD;
        }
        else if( ((info->pkt_drop & (1 << msg_type)) != 0) ) {
            action_index = SOC_PPC_PTP_ACTION_FIELD_ACTION_INDEX_DROP;
        }
        else if( ((info->pkt_tocpu & (1 << msg_type)) != 0) ) {            
            is_hw_trap_id = soc_property_get(unit, spn_BCM886XX_RX_USE_HW_TRAP_ID, 0);            
            if (is_hw_trap_id && (info->user_trap_id >= SOC_PPC_TRAP_CODE_INTERNAL_FLP_IEEE_1588_TRAP3) &&
                                 (info->user_trap_id <= SOC_PPC_TRAP_CODE_INTERNAL_FLP_IEEE_1588_TRAP7))
            {
                action_index = info->user_trap_id - SOC_PPC_TRAP_CODE_INTERNAL_FLP_IEEE_1588_TRAP0;
            }
            else if (!is_hw_trap_id && (info->user_trap_id >= SOC_PPC_TRAP_CODE_1588_PACKET_3) &&
                                       (info->user_trap_id <= SOC_PPC_TRAP_CODE_1588_PACKET_7))
            {
                action_index = info->user_trap_id - SOC_PPC_TRAP_CODE_1588_PACKET_0;
            }
            else
            {
                action_index = SOC_PPC_PTP_ACTION_FIELD_ACTION_INDEX_TRAP;
            }                
        }


        
        
        
        
        
        
        
        
        
          
        command = 0;
        
        command |= ((action_index   & ARAD_PP_1588_ACTION_FIELD_ACTION_INDEX_MASK)     << ARAD_PP_1588_ACTION_FIELD_ACTION_INDEX_SHIFT);
        
        command |= ((egress_command & ARAD_PP_1588_ACTION_FIELD_COMMAND_MASK)          << ARAD_PP_1588_ACTION_FIELD_COMMAND_SHIFT);
        
        command |= ((1              & ARAD_PP_1588_ACTION_FIELD_UPDATE_TIMESTAMP_MASK) << ARAD_PP_1588_ACTION_FIELD_UPDATE_TIMESTAMP_SHIFT);


        switch(profile) {
            case SOC_PPC_PTP_IN_PP_PORT_PROFILE_0:
                action_field = ACTIONf;
                break;
            case SOC_PPC_PTP_IN_PP_PORT_PROFILE_1:
                action_field = ACTION_1f;
                break;
            case SOC_PPC_PTP_IN_PP_PORT_PROFILE_2:
                action_field = ACTION_2f;
                break;
            case SOC_PPC_PTP_IN_PP_PORT_PROFILE_3:
                action_field = ACTION_3f;
                break;
            case SOC_PPC_PTP_IN_PP_PORT_PROFILES_NUM:
            default: 
                SOC_SAND_SET_ERROR_CODE(ARAD_PP_PTP_ACTION_PROFILE_OUT_OF_RANGE_ERR, 10, exit);
        }


        
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, READ_IHP_IEEE_1588_ACTIONm(unit, MEM_BLOCK_ANY, i, tbl_data));
        soc_IHP_IEEE_1588_ACTIONm_field32_set(unit, tbl_data, action_field, command);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, WRITE_IHP_IEEE_1588_ACTIONm(unit, MEM_BLOCK_ANY, i, tbl_data));
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ptp_port_set_action_profile()", 0, 0);
}


uint32
  arad_pp_ptp_port_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_PORT                        local_port_ndx,
    SOC_SAND_IN  SOC_PPC_PTP_PORT_INFO               *info,
    SOC_SAND_IN  SOC_PPC_PTP_IN_PP_PORT_PROFILE      profile
  )
{

  uint32 is_hw_trap_id;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_PTP_PORT_SET_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_ERR_IF_ABOVE_MAX(local_port_ndx, ARAD_NOF_LOCAL_PORTS(unit), ARAD_FAP_PORT_ID_INVALID_ERR, 10, exit);
  if(0 != info->ptp_enabled) {
      SOC_SAND_ERR_IF_OUT_OF_RANGE(profile, SOC_PPC_PTP_IN_PP_PORT_PROFILE_0, SOC_PPC_PTP_IN_PP_PORT_PROFILE_3, 
                                   ARAD_PP_PTP_ACTION_PROFILE_OUT_OF_RANGE_ERR, 20, exit);
  }

  if ((info->pkt_tocpu != 0) && (info->user_trap_id != 0))
  {
      is_hw_trap_id = soc_property_get(unit, spn_BCM886XX_RX_USE_HW_TRAP_ID, 0);            
      if (is_hw_trap_id)
      {
          SOC_SAND_ERR_IF_OUT_OF_RANGE(info->user_trap_id, SOC_PPC_TRAP_CODE_INTERNAL_FLP_IEEE_1588_TRAP3, 
                                       SOC_PPC_TRAP_CODE_INTERNAL_FLP_IEEE_1588_TRAP7, 
                                       ARAD_PP_PTP_USER_TRAP_ID_OUT_OF_RANGE_ERR, 30, exit);
      }
      else
      {
          SOC_SAND_ERR_IF_OUT_OF_RANGE(info->user_trap_id, SOC_PPC_TRAP_CODE_1588_PACKET_3, 
                                       SOC_PPC_TRAP_CODE_1588_PACKET_7, 
                                       ARAD_PP_PTP_USER_TRAP_ID_OUT_OF_RANGE_ERR, 30, exit);
      }      
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ptp_port_set_verify()", 0, 0);
}


uint32
  arad_pp_ptp_port_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_PORT                           local_port_ndx,
    SOC_SAND_IN  SOC_PPC_PTP_PORT_INFO                  *info,
    SOC_SAND_IN  SOC_PPC_PTP_IN_PP_PORT_PROFILE         profile
  )
{
    uint32
        res = SOC_SAND_OK;
    uint32 entry[4];
    uint8 ext_mac_enable = FALSE;
    char *propval;
    int core=0;
    uint32 pp_port;
    soc_mem_t
        pinfo_flp_mem = SOC_IS_JERICHO(unit) ? IHP_PINFO_FLP_1m : IHB_PINFO_FLPm;

    soc_mem_t
        packet_processing_port_mem = SOC_IS_JERICHO_PLUS(unit) ? EPNI_PACKETPROCESSING_PORT_CONFIGURATION_TABLEm : EPNI_PACKET_PROCESSING_PORT_CONFIGURATION_TABLE_PP_PCTm;

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_PTP_PORT_SET_UNSAFE);

         
    res = arad_pp_ptp_port_set_action_profile(unit, info, profile);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    res = soc_port_sw_db_local_to_pp_port_get(unit, local_port_ndx, &pp_port, &core);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    
    if (SOC_IS_JERICHO(unit)) {
        res = soc_mem_read(unit, pinfo_flp_mem, IHP_BLOCK(unit, core), pp_port, entry);
    }else {
        res = soc_mem_read(unit, pinfo_flp_mem, MEM_BLOCK_ANY, pp_port, entry);
    }
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    soc_mem_field32_set(unit, pinfo_flp_mem, entry, IEEE_1588_PROFILEf, profile);
    if (SOC_IS_JERICHO(unit)) {
        res = soc_mem_write(unit, pinfo_flp_mem, IHP_BLOCK(unit, core), pp_port, entry);
    }else {
        res = soc_mem_write(unit, pinfo_flp_mem, MEM_BLOCK_ANY, pp_port, entry);
    }
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

    propval = soc_property_port_get_str(unit, pp_port, spn_EXT_1588_MAC_ENABLE);    
    if (propval) {
        if (sal_strcmp(propval, "1") == 0) {
            ext_mac_enable = TRUE;
        } else if (sal_strcmp(propval, "TRUE") == 0) {
            ext_mac_enable = TRUE;
        }
    } 
        
    if(0 != info->ptp_enabled) {
        if(TRUE == ext_mac_enable) {
            
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, READ_IHB_PINFO_LBPm(unit,  IHB_BLOCK(unit, core), pp_port, entry));
                        
            soc_IHB_PINFO_LBPm_field32_set(unit, entry, RESET_TIME_STAMPf,  1);
                        
            soc_IHB_PINFO_LBPm_field32_set(unit, entry, EXTERNAL_BRCM_MACf, 1);
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 40, exit, WRITE_IHB_PINFO_LBPm(unit, IHB_BLOCK(unit, core), pp_port, entry));
        }else {
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, READ_IHB_PINFO_LBPm(unit,  IHB_BLOCK(unit, core), pp_port, entry));           
            soc_IHB_PINFO_LBPm_field32_set(unit, entry, RESET_TIME_STAMPf,  0);            
            soc_IHB_PINFO_LBPm_field32_set(unit, entry, EXTERNAL_BRCM_MACf, 0);
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 40, exit, WRITE_IHB_PINFO_LBPm(unit, IHB_BLOCK(unit, core), pp_port, entry));
        }

        
        res = soc_mem_read(unit, packet_processing_port_mem, EPNI_BLOCK(unit, core), pp_port, entry);
        SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
        soc_mem_field32_set(unit, packet_processing_port_mem, entry, IEEE_1588_MAC_ENABLEf, ext_mac_enable ? 0 : 1);
        res = soc_mem_write(unit, packet_processing_port_mem, EPNI_BLOCK(unit, core), pp_port, entry);
        SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
    }else {
        if (TRUE == ext_mac_enable) {
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, READ_IHB_PINFO_LBPm(unit,  IHB_BLOCK(unit, core), pp_port, entry));           
            soc_IHB_PINFO_LBPm_field32_set(unit, entry, RESET_TIME_STAMPf,  0);            
            soc_IHB_PINFO_LBPm_field32_set(unit, entry, EXTERNAL_BRCM_MACf, 0);
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 40, exit, WRITE_IHB_PINFO_LBPm(unit, IHB_BLOCK(unit, core), pp_port, entry));
        }else {
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, READ_IHB_PINFO_LBPm(unit,  IHB_BLOCK(unit, core), pp_port, entry));           
            soc_IHB_PINFO_LBPm_field32_set(unit, entry, RESET_TIME_STAMPf,  1);            
            soc_IHB_PINFO_LBPm_field32_set(unit, entry, EXTERNAL_BRCM_MACf, 1);
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 40, exit, WRITE_IHB_PINFO_LBPm(unit, IHB_BLOCK(unit, core), pp_port, entry));
        }

        
        res = soc_mem_read(unit, packet_processing_port_mem, EPNI_BLOCK(unit, core), pp_port, entry);
        SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
        soc_mem_field32_set(unit, packet_processing_port_mem, entry, IEEE_1588_MAC_ENABLEf, 0);
        res = soc_mem_write(unit, packet_processing_port_mem, EPNI_BLOCK(unit, core), pp_port, entry);
        SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
    }


    

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ptp_port_set_unsafe()", 0, 0);
}



uint32
  arad_pp_ptp_port_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_PORT                           local_port_ndx,
    SOC_SAND_OUT SOC_PPC_PTP_IN_PP_PORT_PROFILE        *profile
  )
{
    uint32
        res = SOC_SAND_OK;
    int core=0;
    uint32 pp_port;
    uint32 entry[4];
    soc_mem_t
        pinfo_flp_mem = SOC_IS_JERICHO(unit) ? IHP_PINFO_FLP_1m : IHB_PINFO_FLPm;

     SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_PTP_PORT_GET_UNSAFE);

     res = soc_port_sw_db_local_to_pp_port_get(unit, local_port_ndx, &pp_port, &core);
     SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

     
     if (SOC_IS_JERICHO(unit)) {
        res = soc_mem_read(unit, pinfo_flp_mem, IHP_BLOCK(unit, core), pp_port, entry);
     }else {
        res = soc_mem_read(unit, pinfo_flp_mem, MEM_BLOCK_ANY, pp_port, entry);
     }
     SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
     *profile = soc_mem_field32_get(unit, pinfo_flp_mem, entry, IEEE_1588_PROFILEf);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ptp_port_get_unsafe()", 0, 0);
}








CONST SOC_PROCEDURE_DESC_ELEMENT*
  arad_pp_ptp_get_procs_ptr(void)
{
  return Arad_pp_procedure_desc_element_ptp;
}


CONST SOC_ERROR_DESC_ELEMENT*
  arad_pp_ptp_get_errs_ptr(void)
{
  return Arad_pp_error_desc_element_ptp;
}


soc_error_t
  arad_pp_ptp_p2p_delay_set(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_PPC_PORT               local_port_ndx,
    SOC_SAND_IN  int                        value
  )
{
    uint32 entry[2];
    int rv;
    int core=0;
    uint32 pp_port;

    SOCDNX_INIT_FUNC_DEFS;	  

    rv = soc_port_sw_db_local_to_pp_port_get(unit, local_port_ndx, &pp_port, &core);
    SOCDNX_IF_ERR_EXIT(rv);

    rv = READ_IHB_PINFO_LBPm(unit,  IHB_BLOCK(unit, core), pp_port, entry);
    SOCDNX_IF_ERR_EXIT(rv);
    soc_IHB_PINFO_LBPm_field32_set(unit, entry, INGRESS_P2P_DELAYf, value);
    
    rv = WRITE_IHB_PINFO_LBPm(unit, IHB_BLOCK(unit, core), pp_port, entry);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t
  arad_pp_ptp_p2p_delay_get(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_PPC_PORT               local_port_ndx,
    SOC_SAND_OUT  int*                      value
  )
{
    uint32 entry[2];
    int rv;
    int core=0;
    uint32 pp_port;

    SOCDNX_INIT_FUNC_DEFS;	  

    rv = soc_port_sw_db_local_to_pp_port_get(unit, local_port_ndx, &pp_port, &core);
    SOCDNX_IF_ERR_EXIT(rv);

    rv = READ_IHB_PINFO_LBPm(unit,  IHB_BLOCK(unit, core), pp_port, entry);
    SOCDNX_IF_ERR_EXIT(rv);

    *value = soc_mem_field32_get(unit, IHB_PINFO_LBPm, entry, INGRESS_P2P_DELAYf);

exit:
    SOCDNX_FUNC_RETURN;
}



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif 


