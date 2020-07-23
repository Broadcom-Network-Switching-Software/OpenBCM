/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#if defined(INCLUDE_PTP)

#include <bcm/ptp.h>
#include <bcm_int/common/ptp.h>
#include <bcm_int/ptp_common.h>
#include <bcm/error.h>


#ifdef BCM_PTP_EXT_SERVO_SUPPORT
#include "mngApi/mngApiCmdStruct.h"

#define BCM_PTP_EXT_SERVO_TS_RATE_MAX 128 /* IDT servo supports max 128 pps */

#define SET_BIT(dst,pos)   ((dst) |= (1u << pos))
#define CLEAR_BIT(dst,pos) ((dst) &= ~(1u << pos))
#define GET_BIT(src,pos)   (((src)>>(pos)) & (1u))

#define k_FLG_MASTER_SET_LOCK_OUT (0u)

static uint32 servo_debug_mask;
static uint32 servo_syslog_mask;
static uint32 servo_syslog_destip;
static uint32 servo_syslog_destport;

static int    servo_state = E_srvLoStateInvalid;

static const bcm_ptp_port_identity_t portid_all =
    {{0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}, PTP_IEEE1588_ALL_PORTS};

extern bcm_ptp_mbox_tsevent_stats_t mbox_tsevent_stats;
static int  _bcm_ptp_ext_servo_ptsf_set(int unit,  bcm_ptp_stack_id_t ptp_id,
                        int clock_num,   bcm_ptp_port_identity_t *master,
                        T_osBool b_signalFail);

typedef struct master_table_entry_s {
    bcm_ptp_port_identity_t       master_portId;
    bcm_ptp_clock_port_address_t  master_portaddr;
    int                          in_use;
    int                          flag;
} master_table_entry_t;

master_table_entry_t     master_table[BCM_PTP_MAX_UNICAST_MASTER_TABLE_ENTRIES];
bcm_ptp_port_identity_t  bestmaster_portId = {{0,0,0,0,0,0,0,0},0};

T_idtG82651Callback _bcm_ptp_G82651_cb_func;

int _bcm_ptp_ext_servo_master_table_init() {
    int i=0;
	for (i=0; i<BCM_PTP_MAX_UNICAST_MASTER_TABLE_ENTRIES; i++) {
		sal_memset(&master_table[i].master_portId, 0, sizeof(bcm_ptp_port_identity_t));
		sal_memset(&master_table[i].master_portaddr, 0, sizeof(bcm_ptp_clock_port_address_t));
		master_table[i].in_use = 0;
	}
	return BCM_E_NONE;
}

int _bcm_ptp_ext_servo_master_table_entry_add(int unit,
    bcm_ptp_stack_id_t             ptp_id, 
    int                            clock_num, 
    bcm_ptp_port_identity_t *      master_portId,
    bcm_ptp_clock_port_address_t * master_portaddr,
    uint8                          unicast,
    int                            *inst_num)
{
	int i=0;

	for (i=0; i<BCM_PTP_MAX_UNICAST_MASTER_TABLE_ENTRIES; i++) {
        if(_bcm_ptp_port_address_compare(master_portaddr, &master_table[i].master_portaddr) &&
            _bcm_ptp_peer_portid_compare(master_portId, &master_table[i].master_portId) &&
            master_table[i].in_use) {
            master_table[i].in_use++;
            *inst_num = i;
			return BCM_E_EXISTS;
		}
	}
	if(i == BCM_PTP_MAX_UNICAST_MASTER_TABLE_ENTRIES) {
		/* master entry not exists */
		for (i=0; i<BCM_PTP_MAX_UNICAST_MASTER_TABLE_ENTRIES; i++) {
			if(master_table[i].in_use == 0) {
				master_table[i].master_portaddr.addr_type = master_portaddr->addr_type;
				sal_memcpy(&master_table[i].master_portaddr.address, master_portaddr->address, BCM_PTP_MAX_NETW_ADDR_SIZE);
				sal_memcpy(&master_table[i].master_portId, master_portId, sizeof(bcm_ptp_port_identity_t));
				master_table[i].in_use = 1;
				*inst_num = i;
				return BCM_E_NONE;
			}
		}
		if(BCM_PTP_MAX_UNICAST_MASTER_TABLE_ENTRIES) {
			/* NO free entry available */
            LOG_INFO(BSL_LS_BCM_PTP,
                (BSL_META_U(0, " No free entry/instance available\n")));
			return BCM_E_FULL;
		}
	}
	return BCM_E_NONE;
}

int _bcm_ptp_ext_servo_master_table_entry_delete( int unit,
    bcm_ptp_stack_id_t             ptp_id, 
    int                            clock_num, 
    bcm_ptp_port_identity_t *      master_portId,
    bcm_ptp_clock_port_address_t * master_portaddr,
    uint8                          unicast, 
    int                            *inst_num)
{
	int i=0;

	for (i=0; i<BCM_PTP_MAX_UNICAST_MASTER_TABLE_ENTRIES; i++) {
        if(_bcm_ptp_port_address_compare(master_portaddr, &master_table[i].master_portaddr) &&
            _bcm_ptp_peer_portid_compare(master_portId, &master_table[i].master_portId) &&
            master_table[i].in_use) {
            master_table[i].in_use--;
            if (master_table[i].in_use)
                return BCM_E_BUSY;
            LOG_INFO(BSL_LS_BCM_PTP,
                    (BSL_META_U(0, " *** Master deleted at instance number [%d] ***\n"), i));
            *inst_num = i;
			return BCM_E_NONE;
		}
	}
	if(i == BCM_PTP_MAX_UNICAST_MASTER_TABLE_ENTRIES) {
		/* master entry not exists */
		return BCM_E_NOT_FOUND;
	}
    return BCM_E_NONE;
}

void InitializeStackAdaptor()
{
  /* Initialize IDT stack adaptor */
  if( !IDTStackAdaptor_Initialized() )
    {
      IDTStackAdaptor_Init();
    }
}

int _bcm_ptp_ext_servo_mapped_state_get(
    T_srvLoStateId                 servo_state,
    bcm_ptp_servo_state_t *        mapped_state)
{

    int rv = BCM_E_NONE;

    switch(servo_state) {
        case E_srvLoInitialState:
            *mapped_state = bcmPtpServoStateInitial;
            break;
        case E_srvLoUnqualifiedState:
            *mapped_state = bcmPtpServoStateUnqualified;
            break;
        case E_srvLoLockAcqState:
            *mapped_state = bcmPtpServoStateAcquiring ;
            break;
        case E_srvLoFrequencyLockedState:
            *mapped_state = bcmPtpServoStateFreqLocked;
            break;
        case E_srvLoTimeLockedState:
            *mapped_state = bcmPtpServoStateTimeLocked;
            break;
        case E_srvLoHoldoverInSpecState:
            *mapped_state = bcmPtpServoStateHoldoverInSpec;
            break;
        case E_srvLoHoldoverOutOfSpecState:
            *mapped_state = bcmPtpServoStateHoldoverOutOfSpec;
            break;
        case E_srvLoFreeRunState:
            *mapped_state = bcmPtpServoStateFreerun;
            break;
        default:
            rv = BCM_E_INTERNAL;
            break;
    }
    return rv;
}

T_osBool _bcm_ptp_ext_servo_G8265p1_set_mstr_lockout_inst_cb(
        const T_osUint16 inst_num,
        T_osBool masterLockoutEnable,
        T_ieee1588PortIdentity *soocMasterPortId,
        T_ieee1588PortIdentity *bestMasterPortId ) {

    T_ieee1588PortIdentity invalid_portId = {{{ 0, 0, 0, 0, 0, 0, 0, 0 }}, 0 };

    *soocMasterPortId = invalid_portId;
    *bestMasterPortId = invalid_portId;

    if (master_table[inst_num].in_use) {
        if(E_osTrue == masterLockoutEnable) {
            SET_BIT(master_table[inst_num].flag, k_FLG_MASTER_SET_LOCK_OUT);
        } else {
            LOG_ERROR(BSL_LS_BCM_COMMON,
                (BSL_META_U(0, "lock out notification recvd with enable set to FALSE \n")));
        }
    } else {
        LOG_ERROR(BSL_LS_BCM_COMMON,
                (BSL_META_U(0, "lock out notification recvd for master not in use \n")));
    }

    return 1;
}

void _bcm_ptp_ext_servo_def_evt_hdlr(int unit,
    bcm_ptp_stack_id_t                   ptp_id,
    int                                  clock_num)
{
    int i=0, j=0;
    uint8 payload[100];

    for (i=0; i<BCM_PTP_MAX_UNICAST_MASTER_TABLE_ENTRIES; i++) {
        if (master_table[i].in_use &&
                GET_BIT(master_table[i].flag, k_FLG_MASTER_SET_LOCK_OUT)) {

            LOG_INFO(BSL_LS_BCM_PTP,
                    (BSL_META_U(0, "Servo_def_evt_hdlr : lock mstr[%d] \n"), i));

            sal_memset(payload, 0, 100);

            /* Notify the application of qualification timer expiry for master */
            j=0;
            _bcm_ptp_uint16_write(payload+j,  _bcm_ptp_qual_timer_exp_event);
            j+=2;

            payload[j] = master_table[i].master_portaddr.addr_type;
            j++;
            if(master_table[i].master_portaddr.addr_type == bcmPTPUDPIPv4) {
                sal_memcpy(payload+j, master_table[i].master_portaddr.address, PTP_IPV4_ADDR_SIZE_BYTES);
                j+=PTP_IPV4_ADDR_SIZE_BYTES;
            } else if(master_table[i].master_portaddr.addr_type == bcmPTPUDPIPv6) {
                sal_memcpy(payload+j, master_table[i].master_portaddr.address, PTP_IPV6_ADDR_SIZE_BYTES);
                j+=PTP_IPV6_ADDR_SIZE_BYTES;
            } else if(master_table[i].master_portaddr.addr_type == bcmPTPIEEE8023) {
                sal_memcpy(payload+j, master_table[i].master_portaddr.address, PTP_MAC_ADDR_SIZE_BYTES);
                j+=PTP_MAC_ADDR_SIZE_BYTES;
            }

            _bcm_ptp_send_event_cb(unit, ptp_id, clock_num, payload, j);

            /* Clear the flag */
            CLEAR_BIT(master_table[i].flag, k_FLG_MASTER_SET_LOCK_OUT);
        }
    }
}

void _bcm_ptp_ext_servo_pdvThresholdExceeded_cb( const T_ieee1588PortIdentity *masterPortId,
                                                 T_srvPdvValues currentPdv,
                                                 T_osBool set )
{
    bcm_ptp_port_identity_t master_portId;
    bcm_ptp_clock_info_t *clock_info;

    clock_info = &_bcm_common_ptp_unit_array[0].stack_array[0].clock_array[0].clock_info;
    if (masterPortId) {
        sal_memcpy(&master_portId.clock_identity, &masterPortId->clockIdentity, sizeof(bcm_ptp_clock_identity_t));
        master_portId.port_number = masterPortId->portNumber;
    }

    if (PTP_CLOCK_IS_G8265P1_PROF(clock_info)) {
        if (BCM_FAILURE(_bcm_ptp_telecom_g8265_packet_master_ptsf_unusable_set( 0, 0, 0, &master_portId, set ))) {
            LOG_ERROR(BSL_LS_BCM_COMMON,
                (BSL_META_U(0, "ptsf_unusable set failed \n")));
        }
    }

    if (PTP_CLOCK_IS_G8275P2_PROF(clock_info)) {
        _bcm_ptp_ext_servo_ptsf_set( 0, 0, 0, &master_portId, set );
    }


    LOG_INFO(BSL_LS_BCM_PTP,
        (BSL_META_U(0, "\n *****  PDV threshold exceeded[%d] for master[%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x.%d] current PDV[DL/UL: %Lf/%Lf] ***** \n"),
             set,
             masterPortId->clockIdentity.clockId[0], masterPortId->clockIdentity.clockId[1],
             masterPortId->clockIdentity.clockId[2], masterPortId->clockIdentity.clockId[3],
             masterPortId->clockIdentity.clockId[4], masterPortId->clockIdentity.clockId[5],
             masterPortId->clockIdentity.clockId[6], masterPortId->clockIdentity.clockId[7],
             masterPortId->portNumber,
             currentPdv.downlink, currentPdv.uplink));
}

void _bcm_ptp_ext_servo_losmStateChanged_cb( T_srvLoStateId prevStateId,
                                             T_srvLoStateId currentStateId,
                                             T_srvLoStateMachineEvent losmEvent )
{
    int rv = BCM_E_NONE;
    bcm_ptp_servo_state_t servo_state;

    if (BCM_FAILURE(rv = _bcm_ptp_ext_servo_mapped_state_get(currentStateId, &servo_state))) {
        PTP_ERROR_FUNC("_bcm_ptp_ext_servo_mapped_state_get()");
    }

    if (BCM_FAILURE(_bcm_ptp_servo_state_set( 0, 0, 0, servo_state))) {
	  LOG_ERROR(BSL_LS_BCM_COMMON,
			  (BSL_META_U(0, "servo state set failed \n")));
    }

    LOG_INFO(BSL_LS_BCM_PTP,
            (BSL_META_U(0, "SERVO STATE CHANGE : [%d -> %d] [%d] Event[%d]\n"), prevStateId, currentStateId, servo_state, losmEvent));

}

T_userCallbacks _bcm_ptp_cb_func = {
                    (T_pdvThresholdExceededCallback)_bcm_ptp_ext_servo_pdvThresholdExceeded_cb,
                    (T_losmStateChangedCallback)_bcm_ptp_ext_servo_losmStateChanged_cb
                    };

T_osBool _bcm_ptp_ext_servo_GetClockId(  T_osUint16 instance, T_ieee1588ClockIdentity *clockIdentity )
{
    _bcm_ptp_clock_cache_t *clock_cache;

	clock_cache = &_bcm_common_ptp_unit_array[0].stack_array[0].clock_array[0];
	sal_memcpy(&clockIdentity->clockId, &clock_cache->clock_info.clock_identity, sizeof(clockIdentity->clockId));

    LOG_VERBOSE(BSL_LS_BCM_PTP,
            (BSL_META_U(0, "\n Servo issued get clock identity[%02x.%02x.%02x.%02x.%02x.%02x.%02x.%02x]\n"),
			    clockIdentity->clockId[0], clockIdentity->clockId[1], clockIdentity->clockId[2],
			    clockIdentity->clockId[3], clockIdentity->clockId[4], clockIdentity->clockId[5],
                clockIdentity->clockId[6], clockIdentity->clockId[7]));

    return E_osTrue;
}

int
_bcm_ptp_servo_init(int unit, 
    bcm_ptp_stack_id_t             ptp_id,
    int                            clock_num,
    bcm_ptp_osc_type_t             osc_type,
    T_srvReferenceTrackerType      tracker_type)
{
  T_mngDeviceAdaptorTsConfig idtTsAdaptorConfig;
  T_mngDeviceAdaptorTS *idtTsAdaptor;
  int rv = BCM_E_NONE;
  int i=0;
  char syslog_ip[SAL_IPADDR_STR_LEN];

  T_cmnConfigParameterSet configParam;

  T_swVerisonInfo swVersionInfo;
  const char *releaseId = NULL, *commitId = NULL;
  T_cmnErrorCode errorCode = E_cmnErrorCode_InvalidApiCommand;
  T_idtStackConfigCallbacks cfgCallbacks;

  if(PTP_SERVO_IS_BCM(unit, ptp_id, clock_num)) {
    LOG_INFO(BSL_LS_BCM_PTP,
        (BSL_META_U(unit, " configured to use BRCM servo \n")));
    return rv ;
  }

  sal_memset(&configParam, 0 , sizeof(configParam));

  cmnInitConfigStructure(&configParam);
 
  LOG_INFO(BSL_LS_BCM_PTP,
          (BSL_META_U(unit, " ConfigParam Init done: frequencyLockThresholdPpb [%Lf]\n"), 
		      configParam.externalConfig.referenceTrackerConfig[0].frequencyLockThresholdPpb)); 

  configParam.externalConfig.stackInstanceConfig[ 0 ].slaveOnly       = E_osTrue;
  configParam.externalConfig.messageLoggingConfig.stdoutSelectionMask = E_cmnSyncErrorMessages | E_cmnErrorMessages;
 
  if(servo_syslog_mask) {

    configParam.externalConfig.messageLoggingConfig.syslog.selectionMask = servo_syslog_mask;
    configParam.externalConfig.messageLoggingConfig.syslog.udpPort = servo_syslog_destport;

    format_ipaddr(syslog_ip, servo_syslog_destip);
    sal_memcpy(configParam.externalConfig.messageLoggingConfig.syslog.ipAddress, syslog_ip, SAL_IPADDR_STR_LEN);

    LOG_VERBOSE(BSL_LS_BCM_PTP,
            (BSL_META_U(unit, "\n Configuring syslog settings: IP[%s] port[%d]"), 
            configParam.externalConfig.messageLoggingConfig.syslog.ipAddress, 
            configParam.externalConfig.messageLoggingConfig.syslog.udpPort));
  }

  configParam.externalConfig.referenceTrackerType                    = tracker_type;
  configParam.externalConfig.tsInstances                             = 1;
  
  switch (osc_type) {
    case bcmPTPOscTypeOCXO:
        configParam.externalConfig.oscillatorType = E_srvOcxo;
        break;
    case bcmPTPOscTypeTCXO: 
        configParam.externalConfig.oscillatorType = E_srvTcxo;
        break;
    case bcmPTPOscTypeMiniOCXO:
        configParam.externalConfig.oscillatorType = E_srvMiniOcxo;
        break;
    default:
        /* Current 3rd party servo supports only OCXO, mini-OCXO, TCXO and SyncE as osc types*/
        configParam.externalConfig.oscillatorType = E_srvOcxo;
        break;
  }

  /* servo ref tracker config */
  for (i=0; i< BCM_PTP_MAX_UNICAST_MASTER_TABLE_ENTRIES; i++) {
    configParam.externalConfig.referenceTrackerConfig[i].stationarityMeasure1LowerBound = 0.4;
    configParam.externalConfig.referenceTrackerConfig[i].stationarityMeasure1UpperBound = 2.5;
    configParam.externalConfig.referenceTrackerConfig[i].stationarityMeasure2UpperBound = 250;
  }

  bsl_printf(" \nLoopFilterMinResponseTimeSeconds : %d ", configParam.externalConfig.referenceTrackerConfig[0].dcoLoopFilterMinResponseTimeSeconds);
  bsl_printf("\n dcoLoopFilterMaxFfoCorrection : %d", configParam.externalConfig.referenceTrackerConfig[0].dcoLoopFilterMaxFfoCorrection);
  bsl_printf("\n dcoLoopFilterIntegralBranchGain : %Lf ", configParam.externalConfig.referenceTrackerConfig[0].dcoLoopFilterIntegralBranchGain);
  bsl_printf("\n dcoLoopFilterBandwidthScalar : %Lf ", configParam.externalConfig.referenceTrackerConfig[0].dcoLoopFilterBandwidthScalar);
  bsl_printf("\n desiredPrecisionSeconds : %Lf \n", configParam.externalConfig.referenceTrackerConfig[0].desiredPrecisionSeconds);
  bsl_printf("\n timeLockThresholdNanoseconds: %Lf ", configParam.externalConfig.referenceTrackerConfig[0].timeLockThresholdNanoseconds);
  bsl_printf("\n phaseSnapThresholdSeconds: %Lf \n", configParam.externalConfig.referenceTrackerConfig[0].phaseSnapThresholdSeconds);
  bsl_printf("\n qualificationTimeoutSeconds: %d \n", configParam.externalConfig.qualificationTimeoutSeconds);

  /***************************/
  /* Initialize time stamper */
  /***************************/
  idtTsAdaptor= mngGetDeviceAdaptorTS();

  sal_memset( &idtTsAdaptorConfig, 0, sizeof(idtTsAdaptorConfig) );

  idtTsAdaptorConfig.tsInstances      = configParam.externalConfig.tsInstances;
  idtTsAdaptorConfig.warmStartEnabled = configParam.externalConfig.warmStartConfig.warmStartEnabled;

  if( !mngDeviceAdaptorTS_Initialize( idtTsAdaptor, &idtTsAdaptorConfig ) )
    {
      LOG_ERROR(BSL_LS_BCM_PTP,
        (BSL_META_U(unit, " Device adaptor TS initialization failed \n")));
      return BCM_E_INIT;
    }

  /****************************************************************/
  /* Initialize IDT stack adaptor once.                           */
  /****************************************************************/
  InitializeStackAdaptor();

  cfgCallbacks.configSetParamFunc_cb = NULL;
  cfgCallbacks.configIfNameFunc_cb = NULL;
  cfgCallbacks.configGetLocalPrio_cb = NULL;
  cfgCallbacks.configGetWaitToRestore_cb = NULL;
  cfgCallbacks.configGetLocalClockIdentity_cb = _bcm_ptp_ext_servo_GetClockId;

  IDTStackAdaptor_RegisterStackConfigCallbacks(&cfgCallbacks );

  _bcm_ptp_G82651_cb_func.setMasterLockoutWithInstanceCallback =
          (T_idtStackAdaptorSetMasterLockoutWithInstanceFunc) _bcm_ptp_ext_servo_G8265p1_set_mstr_lockout_inst_cb;

  if(IDTStackAdaptor_RegisterG82651Callback(&_bcm_ptp_G82651_cb_func)) {
      LOG_ERROR(BSL_LS_BCM_PTP,
          (BSL_META_U(unit, " RegisterG82651Callback failed  \n")));
  } else {
      LOG_INFO(BSL_LS_BCM_PTP,
          (BSL_META_U(unit, " RegisterG82651Callback success  \n")));
  }


  /*******************************/
  /* Start IDT PTP servo session */
  /*******************************/
  mngPtpLoggerStart( &configParam.externalConfig.messageLoggingConfig );

  /* register_cb((T_pdvThresholdExceededCallback)_bcm_ptp_ext_servo_pdvThresholdExceeded_cb, (T_losmStateChangedCallback)_bcm_ptp_ext_servo_losmStateChanged_cb);*/

  mngPtpSessionStart( &configParam, &_bcm_ptp_cb_func );
  
  errorCode = mngApi_GetSoftwareVersion( &releaseId, &commitId );

  if( E_cmnErrorCode_OK == errorCode )
  {
      sal_memset( &swVersionInfo, 0, sizeof(T_swVerisonInfo) );
	  sal_memcpy( swVersionInfo.releaseId, releaseId, SW_RELEASE_ID_MAX_SIZE - 1 );
	  sal_memcpy( swVersionInfo.commitId, commitId, SW_COMMIT_ID_MAX_SIZE - 1 );
      LOG_INFO(BSL_LS_BCM_PTP,
          (BSL_META_U(unit, "Servo version [Release :%s] [Commit :%s]\n"), 
		      (char *)swVersionInfo.releaseId, 
			  (char *)swVersionInfo.commitId));
  } else {
      PTP_ERROR("Failed to get sw version");
  }

  if (servo_debug_mask) {
    if (BCM_FAILURE(rv = _bcm_ptp_servo_debug_level_set(unit, ptp_id, clock_num, E_cmnLog_stdout, servo_debug_mask))) {
        PTP_ERROR_FUNC("_bcm_ptp_servo_debug_level_set()");
	}
  } else {
    errorCode = mngApi_SetMessageLogLevel( E_cmnLog_stdout, E_cmnNonStackMessageAll );
    if( E_cmnErrorCode_OK != errorCode )
    {
      PTP_ERROR("Failed to set message log level ");
    }
  }

  /* Reset servo state */
  servo_state = E_srvLoStateInvalid;
  LOG_INFO(BSL_LS_BCM_PTP, (BSL_META_U(unit, " Servo init complete \n")));

  _bcm_ptp_ext_servo_master_table_init();

  return 0;
}

int
_bcm_ptp_servo_deinit(int unit, 
    bcm_ptp_stack_id_t             ptp_id,
    int                            clock_num)
{
  int rv = BCM_E_NONE;
  _bcm_ptp_timestamp_queue_t *queue;
  int callback_running;

  if(PTP_SERVO_IS_BCM(unit, ptp_id, clock_num)) {
    LOG_VERBOSE(BSL_LS_BCM_PTP,
        (BSL_META_U(unit, " configured to use BRCM servo \n")));
    return rv ;
  }

  queue = &_bcm_common_ptp_info[unit].stack_info[ptp_id].timestamp_queue;

  callback_running = queue->rdpc.running;
  if (callback_running)
    shr_rdpc_callback_stop(&queue->rdpc);

  if( IDTStackAdaptor_Initialized() ) {
    mngPtpSessionStop();
    IDTStackAdaptor_DeInit();
  }

  if (callback_running)
    rv = shr_rdpc_callback_start(&queue->rdpc, 0, INT_TO_PTR(unit), INT_TO_PTR(ptp_id), INT_TO_PTR(0), INT_TO_PTR(0));

  LOG_INFO(BSL_LS_BCM_PTP,
        (BSL_META_U(unit, " Servo de-init complete\n")));

  return rv;
}


/*
 * Function:
 *     _bcm_ptp_servo_master_add 
 * Purpose:
 *      Notify 3rd-party servo of the 1588 master addition.
 * Parameters:
 *      unit               - (IN) Unit number.
 *      ptp_id             - (IN) PTP stack ID.
 *      clock_num          - (IN) PTP clock number.
 *      inst_num           - (IN) Instance/Master number.
 *      master_portId      - (IN) master port identity.
 *      master_portaddr    - (IN) master port address.
 *      unicast            - (IN) unicast master.
 *      clockClass         - (IN) clock class of the master.
 *      best_master_portId - (IN) best master identity.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 *   best_master_portId can be NULL only for G.8265.1 profile.
 */
int
_bcm_ptp_servo_master_add(int unit, 
    bcm_ptp_stack_id_t             ptp_id, 
    int                            clock_num, 
    uint16                         inst_num, 
    bcm_ptp_port_identity_t *      master_portId,
    bcm_ptp_clock_port_address_t * master_portaddr,
    uint8                          unicast,
    unsigned char                  clockClass)
{
    int rv = BCM_E_NONE;
    int instance=0;
    T_ieee1588PortIdentity best_master = {{{ 0, 0, 0, 0, 0, 0, 0, 0 }}, 0 };
	T_idtMasterInfo master_info;

    bcm_ptp_clock_info_t *clock_info;

	if(PTP_SERVO_IS_BCM(unit, ptp_id, clock_num)) {
        return rv ;
    }

    if(!master_portId ) { 
        return BCM_E_PARAM;
	}

    if(!IDTStackAdaptor_Initialized() ) {
        return BCM_E_PARAM;
    }

    clock_info = &_bcm_common_ptp_unit_array[unit].stack_array[ptp_id].clock_array[clock_num].clock_info;

    sal_memset(&master_info, 0 , sizeof(master_info));

    sal_memcpy(&master_info.portIdentity.clockIdentity, &master_portId->clock_identity, sizeof(bcm_ptp_clock_identity_t));
    master_info.portIdentity.portNumber = master_portId->port_number; 
    /* Port address not being used. skip it */
	master_info.isUnicast = unicast;
    /* master_info.clockClass = clockClass;*/

    
    if (inst_num == PTP_EXT_SERVO_MSTR_INST_UNKNOWN) {
        if(PTP_CLOCK_IS_G8275P1_PROF(clock_info)) {
            /* G.8275.1 profile shall NOT use ext servo */
            return BCM_E_PARAM;
        } else {
            rv = _bcm_ptp_ext_servo_master_table_entry_add(unit, ptp_id, clock_num, master_portId, master_portaddr,1, &instance);
            if (rv == BCM_E_EXISTS)
                return BCM_E_NONE;
            if(BCM_FAILURE(rv)) {
                PTP_ERROR_FUNC("_bcm_ptp_ext_servo_master_table_entry_add()");
            }
            inst_num = (uint16)instance;
	        LOG_VERBOSE(BSL_LS_BCM_PTP, (BSL_META_U(unit, "\n _bcm_ptp_ext_servo_master_table_entry_add returned inst_num[%d] instance[%d]\n"), inst_num, instance));
        }
    }

    sal_memcpy(&best_master.clockIdentity, bestmaster_portId.clock_identity, sizeof(bcm_ptp_clock_identity_t));
    best_master.portNumber = bestmaster_portId.port_number;

    rv = IDTStackAdaptor_AddMaster(inst_num, &master_info, &best_master, NULL );

    LOG_VERBOSE(BSL_LS_BCM_PTP,
                  (BSL_META_U(unit, "\n AddMaster[%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x.%d]  \
                      \nBestMaster[%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x.%d] \n Instance[%d] rv[%d]\n"),
				      master_info.portIdentity.clockIdentity.clockId[0], master_info.portIdentity.clockIdentity.clockId[1],
				      master_info.portIdentity.clockIdentity.clockId[2], master_info.portIdentity.clockIdentity.clockId[3],
				      master_info.portIdentity.clockIdentity.clockId[4], master_info.portIdentity.clockIdentity.clockId[5],
				      master_info.portIdentity.clockIdentity.clockId[6], master_info.portIdentity.clockIdentity.clockId[7],
					  master_info.portIdentity.portNumber,
				      best_master.clockIdentity.clockId[0], best_master.clockIdentity.clockId[1],
				      best_master.clockIdentity.clockId[2], best_master.clockIdentity.clockId[3],
				      best_master.clockIdentity.clockId[4], best_master.clockIdentity.clockId[5],
				      best_master.clockIdentity.clockId[6], best_master.clockIdentity.clockId[7],
					  best_master.portNumber, inst_num, rv));

    if (rv) {
        return BCM_E_FAIL;
    }

    return rv ;
}

/*
 * Function:
 *     _bcm_ptp_servo_master_remove
 * Purpose:
 *      Notify 3rd-party servo of the 1588 master removal.
 * Parameters:
 *      unit               - (IN) Unit number.
 *      ptp_id             - (IN) PTP stack ID.
 *      clock_num          - (IN) PTP clock number.
 *      master_portId      - (IN) master port identity.
 *      best_master_portId - (IN) new best master identity.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 *   best_master_portId can be NULL only for G.8265.1 profile.
 */
int
_bcm_ptp_servo_master_remove(int unit, 
    bcm_ptp_stack_id_t             ptp_id, 
    int                            clock_num, 
    uint16                         inst_num, 
    uint8                          unicast,
    bcm_ptp_port_identity_t *      master_portId,
    bcm_ptp_port_identity_t *      best_master_portId,
    bcm_ptp_clock_port_address_t * master_portaddr)
{
    int rv = BCM_E_NONE;

	T_ieee1588PortIdentity master;
    T_ieee1588PortIdentity best_master = {{{ 0, 0, 0, 0, 0, 0, 0, 0 }}, 0 };
	int instance;

	if(PTP_SERVO_IS_BCM(unit, ptp_id, clock_num)) {
        return rv ;
    }

    if(!master_portId) { 
        return BCM_E_PARAM;
	}

    if(!IDTStackAdaptor_Initialized() ) {
        return BCM_E_PARAM;
    }

    if (inst_num == PTP_EXT_SERVO_MSTR_INST_UNKNOWN) {
        rv = _bcm_ptp_ext_servo_master_table_entry_delete(unit, ptp_id, clock_num, master_portId, master_portaddr, 1, &instance);
        if (rv == BCM_E_BUSY)
            return BCM_E_NONE;
        if (BCM_FAILURE(rv)) {
            PTP_ERROR_FUNC("_bcm_ptp_ext_servo_master_table_entry_delete()");
            inst_num = 0;
        }
        inst_num = (uint16) instance;
	}

    sal_memcpy(&master.clockIdentity, &master_portId->clock_identity, IDT_CLKID_BYTE_SIZE);
    master.portNumber = master_portId->port_number; 

    sal_memcpy(&best_master.clockIdentity, bestmaster_portId.clock_identity, sizeof(bcm_ptp_clock_identity_t));
    best_master.portNumber = bestmaster_portId.port_number;

    rv = IDTStackAdaptor_DeleteMaster(&master, &best_master, NULL);

    LOG_VERBOSE(BSL_LS_BCM_PTP,
                  (BSL_META_U(unit, "\n DeleteMaster[%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x.%d] 	\
                       \nBestMaster[%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x.%d] \n rv[%d] \n"),
                       master.clockIdentity.clockId[0], master.clockIdentity.clockId[1],
                       master.clockIdentity.clockId[2], master.clockIdentity.clockId[3],
                       master.clockIdentity.clockId[4], master.clockIdentity.clockId[5],
                       master.clockIdentity.clockId[6], master.clockIdentity.clockId[7],
                       master.portNumber,
                       best_master.clockIdentity.clockId[0], best_master.clockIdentity.clockId[1],
                       best_master.clockIdentity.clockId[2], best_master.clockIdentity.clockId[3],
                       best_master.clockIdentity.clockId[4], best_master.clockIdentity.clockId[5],
                       best_master.clockIdentity.clockId[6], best_master.clockIdentity.clockId[7],
                       best_master.portNumber, rv));

    if (rv) {
        return BCM_E_FAIL;
    }
    return rv ;
}
/*
 * Function:
 *    _bcm_ptp_servo_best_master_update 
 * Purpose:
 *      Notify 3rd-party servo of the new best master selected.
 * Parameters:
 *      unit               - (IN) Unit number.
 *      ptp_id             - (IN) PTP stack ID.
 *      clock_num          - (IN) PTP clock number.
 *      best_master_portId - (IN) best master identity.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */

int
_bcm_ptp_servo_best_master_update(int unit,
    bcm_ptp_stack_id_t             ptp_id,
    int                            clock_num,
    bcm_ptp_port_identity_t *      best_master_portId)
{
    int rv = BCM_E_NONE;

    T_ieee1588PortIdentity best_master = {{{ 0, 0, 0, 0, 0, 0, 0, 0 }}, 0 };
    bcm_ptp_clock_info_t *clock_info;
	bcm_ptp_port_identity_t clk_portid;

    clock_info = &_bcm_common_ptp_unit_array[unit].stack_array[ptp_id].clock_array[clock_num].clock_info;

	sal_memcpy(&clk_portid.clock_identity, &clock_info->clock_identity, sizeof(bcm_ptp_clock_identity_t));
	clk_portid.port_number = 0;

	if(PTP_SERVO_IS_BCM(unit, ptp_id, clock_num)) {
        return rv ;
    }

    if(!best_master_portId) {
        return BCM_E_PARAM;
	}

    if(!IDTStackAdaptor_Initialized() ) {
        return BCM_E_PARAM;
    }

    if (best_master_portId) {
        if (_bcm_ptp_peer_portid_compare(best_master_portId, &clk_portid)) {
            sal_memset(&bestmaster_portId.clock_identity, 0, sizeof(bcm_ptp_clock_identity_t));
            bestmaster_portId.port_number = 0;
			/* retain best_master as 0's */
		} else {
            sal_memcpy(&bestmaster_portId.clock_identity, &best_master_portId->clock_identity, sizeof(bcm_ptp_clock_identity_t));
            bestmaster_portId.port_number = best_master_portId->port_number;
        }
        sal_memcpy(&best_master.clockIdentity, &bestmaster_portId.clock_identity, sizeof(bcm_ptp_clock_identity_t));
        best_master.portNumber = bestmaster_portId.port_number;
    }
    rv = IDTStackAdaptor_UpdateBestMasterSelection(&best_master);

    LOG_INFO(BSL_LS_BCM_PTP,
                  (BSL_META_U(unit, "\n UpdatedBestMaster[%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x.%d] rv[%d] \n"),
                       best_master.clockIdentity.clockId[0], best_master.clockIdentity.clockId[1],
                       best_master.clockIdentity.clockId[2], best_master.clockIdentity.clockId[3],
                       best_master.clockIdentity.clockId[4], best_master.clockIdentity.clockId[5],
                       best_master.clockIdentity.clockId[6], best_master.clockIdentity.clockId[7],
                       best_master.portNumber, rv));

    if (rv) {
        return BCM_E_FAIL;
    }
    
    return rv ;
}

int
_bcm_ptp_servo_master_replace(int unit, 
    bcm_ptp_stack_id_t             ptp_id, 
    int                            clock_num, 
    uint16                         inst_num, 
    bcm_ptp_port_identity_t *      prev_master_portId,
    bcm_ptp_port_identity_t *      master_portId,
    uint8                          unicast)
{
    int rv = BCM_E_NONE;
    T_ieee1588PortIdentity best_master = {{{ 0, 0, 0, 0, 0, 0, 0, 0 }}, 0 };
    T_ieee1588PortIdentity prev_master = {{{ 0, 0, 0, 0, 0, 0, 0, 0 }}, 0 };
	T_idtMasterInfo master_info;

	if(PTP_SERVO_IS_BCM(unit, ptp_id, clock_num)) {
        return rv ;
    }

    if(!master_portId || !prev_master_portId) { 
        return BCM_E_PARAM;
	}

    if(!IDTStackAdaptor_Initialized() ) {
        return BCM_E_PARAM;
    }

    sal_memset(&master_info, 0 , sizeof(master_info));

    sal_memcpy(&master_info.portIdentity.clockIdentity, &master_portId->clock_identity, sizeof(bcm_ptp_clock_identity_t));
    master_info.portIdentity.portNumber = master_portId->port_number; 
    /* Port address not being used. skip it */
	master_info.isUnicast = unicast;

    sal_memcpy(&best_master.clockIdentity, bestmaster_portId.clock_identity, sizeof(bcm_ptp_clock_identity_t));
    best_master.portNumber = bestmaster_portId.port_number;

    if (prev_master_portId) {
        sal_memcpy(&prev_master.clockIdentity, prev_master_portId->clock_identity, sizeof(bcm_ptp_clock_identity_t));
        prev_master.portNumber = prev_master_portId->port_number;
    }

    rv = IDTStackAdaptor_ReplaceMaster(inst_num, &master_info, &prev_master, &best_master, NULL );

    LOG_VERBOSE(BSL_LS_BCM_PTP,
                  (BSL_META_U(unit, "\n ReplacingMaster[%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x.%d]  \
                      \nPrevMaster[%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x.%d] \n Instance[%d] rv[%d]\n"),
				      master_info.portIdentity.clockIdentity.clockId[0], master_info.portIdentity.clockIdentity.clockId[1],
				      master_info.portIdentity.clockIdentity.clockId[2], master_info.portIdentity.clockIdentity.clockId[3],
				      master_info.portIdentity.clockIdentity.clockId[4], master_info.portIdentity.clockIdentity.clockId[5],
				      master_info.portIdentity.clockIdentity.clockId[6], master_info.portIdentity.clockIdentity.clockId[7],
					  master_info.portIdentity.portNumber,
				      prev_master.clockIdentity.clockId[0], prev_master.clockIdentity.clockId[1],
				      prev_master.clockIdentity.clockId[2], prev_master.clockIdentity.clockId[3],
				      prev_master.clockIdentity.clockId[4], prev_master.clockIdentity.clockId[5],
				      prev_master.clockIdentity.clockId[6], prev_master.clockIdentity.clockId[7],
					  prev_master.portNumber, inst_num, rv));

    if (rv) {
        return BCM_E_FAIL;
    }

    return rv ;
}


/*
 * Function:
 *    _bcm_ptp_servo_pdv_threshold_set 
 * Purpose:
 *      Set the PDV threshold.
 * Parameters:
 *      unit               - (IN) Unit number.
 *      ptp_id             - (IN) PTP stack ID.
 *      clock_num          - (IN) PTP clock number.
 *      PDV threshold      - (IN) PDV threshold value.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */

int
_bcm_ptp_servo_pdv_threshold_set(int unit, 
    bcm_ptp_stack_id_t             ptp_id, 
    int                            clock_num, 
    bcm_ptp_telecom_g8265_pktstats_t thresholds)
{
    int rv = BCM_E_NONE;
    int i=0;
    T_srvPdvValues pdvThreshold;

	if(PTP_SERVO_IS_BCM(unit, ptp_id, clock_num)) {
        return rv ;
    }

    pdvThreshold.downlink = thresholds.pdv_scaled_allan_var;
    pdvThreshold.uplink = thresholds.pdv_scaled_allan_var;

    for (i=0; i<IDT_COMPILE_MAX_NUM_STACK_INSTANCES; i++) {
        rv = mngApi_ReferenceTracker_SetPdvThreshold(i, &pdvThreshold);
    }

    if (rv) {
        return BCM_E_FAIL;
    }
    
    return rv ;
}

#if 0
int
_bcm_ptp_servo_master_ptsf_set(int unit, 
    bcm_ptp_stack_id_t             ptp_id, 
    int                            clock_num, 
    T_ieee1588PortIdentity *      master_portId,
    int                            ptsf_state)
{
    int rv = BCM_E_NONE;

    bcm_ptp_port_identity_t       pkt_master_portId;

    if (master_portId) {
        sal_memcpy(&pkt_master_portId.clock_identity, &master_portId->clockIdentity, sizeof(bcm_ptp_clock_identity_t));
        pkt_master_portId.port_number = master_portId->portNumber;
    }
    if (BCM_FAILURE(rv = _bcm_ptp_telecom_g8265_packet_master_ptsf_unusable_set(unit, ptp_id, clock_num, &pkt_master_portId, ptsf_state))){
        PTP_ERROR_FUNC("_bcm_ptp_telecom_g8265_packet_master_ptsf_unusable_set()");
    }
    
    return rv ;
}
#endif

/*
 * Function:
 *    
 * Purpose:
 *      Notify 3rd-party servo of the new best master selected.
 * Parameters:
 *      unit               - (IN) Unit number.
 *      ptp_id             - (IN) PTP stack ID.
 *      clock_num          - (IN) PTP clock number.
 *      best_master_portId - (IN) best master identity.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */

int
_bcm_ptp_servo_master_lockout_set(int unit, 
    bcm_ptp_stack_id_t             ptp_id, 
    int                            clock_num, 
    int                            lock_out, 
    bcm_ptp_port_identity_t *      master_portId)
{
    int rv = BCM_E_NONE;

	T_ieee1588PortIdentity master;

	if(PTP_SERVO_IS_BCM(unit, ptp_id, clock_num)) {
        return rv ;
    }

    if(!master_portId) { 
        return BCM_E_PARAM;
	}

    if(!IDTStackAdaptor_Initialized() ) {
        return BCM_E_PARAM;
    }

    if (master_portId) {
        sal_memcpy(&master.clockIdentity, &master_portId->clock_identity, sizeof(bcm_ptp_clock_identity_t));
        master.portNumber = master_portId->port_number;
    }

    if (lock_out) {
        rv = IDTStackAdaptor_SuspendMaster(&master);
    } else {
        rv = IDTStackAdaptor_ResumeMaster(&master);
    }

    LOG_INFO(BSL_LS_BCM_PTP,
                  (BSL_META_U(unit, "\n LockOutMaster[%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x.%d] lock_out[%d] rv[%d] \n "),
				      master.clockIdentity.clockId[0], master.clockIdentity.clockId[1],
				      master.clockIdentity.clockId[2], master.clockIdentity.clockId[3],
				      master.clockIdentity.clockId[4], master.clockIdentity.clockId[5],
				      master.clockIdentity.clockId[6], master.clockIdentity.clockId[7],
					  master.portNumber, lock_out, rv));


    if (rv) {
        return BCM_E_FAIL;
    }
    
    return rv ;
}

int
_bcm_ptp_servo_debug_level_set(int unit,
    bcm_ptp_stack_id_t             ptp_id,
    int                            clock_num,
    uint16                         log_id,
    uint32                         log_level_mask)
{
    int rv = BCM_E_NONE;
    T_cmnMessageLevelRegister messageLevelMask = 0;
  
    if( !IDTStackAdaptor_Initialized() ) {
        servo_debug_mask = log_level_mask;
        return rv;
    }

    /* log_id:0 is STDOUT */
	if(PTP_SERVO_IS_BCM(unit, ptp_id, clock_num)) {
        return rv ;
    }
 
	/* bitmap in cmd_ptp_debug defined inline with Extservo log levels */
	messageLevelMask = (log_level_mask >>16) & 0xFF;

    LOG_VERBOSE(BSL_LS_BCM_PTP,
            (BSL_META_U(unit, " LogLevelmask[0x%0x] MessagelevelMask[0x%0x] \n"), log_level_mask, messageLevelMask));

    rv = mngApi_SetMessageLogLevel(log_id, messageLevelMask);

    if (rv) {
        return BCM_E_FAIL;
    }

    return rv ;
}

int
_bcm_ptp_servo_sys_log_set(int unit,
    bcm_ptp_stack_id_t             ptp_id,
    int                            clock_num,
    uint32                         log_udp_mask,
    bcm_ip_t                       dest_addr,
    uint16                         port)
{
    int rv = BCM_E_NONE;

    char syslog_ip[SAL_IPADDR_STR_LEN];
  
    if( !IDTStackAdaptor_Initialized() ) {
        /* backup the syslog settings and use at init time */
        servo_syslog_mask = log_udp_mask;
        servo_syslog_destip = dest_addr;
        servo_syslog_destport = port;
        return rv;
    } else {
        if(PTP_SERVO_IS_BCM(unit, ptp_id, clock_num)) {
            return rv ;
        }

        format_ipaddr(syslog_ip, dest_addr);

        /* Close the sys log socket, if already configured */
        rv = mngApi_CloseSyslog();
        if (rv) {
            return BCM_E_FAIL;
        }
        LOG_INFO(BSL_LS_BCM_PTP,
            (BSL_META_U(unit, " syslog socket closed \n")));

        if (log_udp_mask) {
          rv = mngApi_OpenSyslog(syslog_ip, port, log_udp_mask);
          if (rv) {
            return BCM_E_FAIL;
          }
          LOG_INFO(BSL_LS_BCM_PTP,
              (BSL_META_U(unit, " syslog socket opened \n")));
        }
    }
    return rv ;
}

uint32 _bcm_ptp_servo_usecintv_get(int8 log_int) {
  switch(log_int) 
  {
    case -4:
      return 62500; 
    case -5:
      return 31250;
    case -6:
      return 15625;
    case -7:
      /* 128pps - Interval is 7812.5 usec */
      return 7812;
    case -3:
      return 125000;
    case -2:
      return 250000;
    case -1:
      return 500000;
    case 0:
      return 1000000;
    case 1:
      return 2000000;
    case 2:
      return 4000000;
    case 3:
      return 8000000;
    case 4:
      return 16000000;
    default:
      return 0;
  }
}

#ifdef BCM_PTP_EXT_SERVO_ABS_TS_SUPPORT
/* absolute timestamps */
int _bcm_ptp_servo_AddFullTimestamps(int unit,
    bcm_ptp_stack_id_t             ptp_id,
    int                            clock_num,
    bcm_ptp_port_identity_t *      master_portId,
    int                            num_TS,
    uint8*                         ts_ptr,
    _bcm_ptp_ts_type_t             direction,
    uint32                         int_usec)
{

    int rv = BCM_E_NONE;
    int idx=0;

	T_ieee1588PortIdentity master;
    T_ieee1588Timestamp txTS, rxTS;

	if(PTP_SERVO_IS_BCM(unit, ptp_id, clock_num)) {
        return rv ;
    }

    if(!master_portId) { 
        return BCM_E_PARAM;
	}

    if(!IDTStackAdaptor_Initialized()) {
        return BCM_E_PARAM;
    }

    sal_memcpy(&master.clockIdentity, &master_portId->clock_identity, sizeof(bcm_ptp_clock_identity_t));
    master.portNumber = master_portId->port_number; 

    LOG_DEBUG(BSL_LS_BCM_PTP,
            (BSL_META_U(unit, "Add Timestamps to servo: int[%dusec] num_TS[%d] direction[%d]\n"), int_usec, num_TS, direction));

    for (idx=0; idx<num_TS; idx++) {
		txTS.seconds = _bcm_ptp_uint64_read(ts_ptr);
		ts_ptr+=8;
		txTS.nanoseconds = _bcm_ptp_uint32_read(ts_ptr);
		ts_ptr+=4;
		rxTS.seconds = _bcm_ptp_uint64_read(ts_ptr);
		ts_ptr+=8;
		rxTS.nanoseconds = _bcm_ptp_uint32_read(ts_ptr);
		ts_ptr+=4;

		LOG_DEBUG(BSL_LS_BCM_PTP,
		    (BSL_META_U(unit, "txTS[%llx.%x] rxTS[%llx.%x] \n"), (long long unsigned int)txTS.seconds, txTS.nanoseconds, 
		                                                (long long unsigned int)rxTS.seconds, rxTS.nanoseconds));

        if (direction == _bcm_ptp_ts_type_downlink) {
            rv = IDTStackAdaptor_AddDownlinkTimeStamps( &master,  txTS, 0, rxTS, int_usec);
            if (rv) {
                PTP_ERROR_FUNC("IDTStackAdaptor_AddDownlinkTimeStamps( )");
                return BCM_E_FAIL;
            }
        } else {
            rv = IDTStackAdaptor_AddUplinkTimeStamps( &master,  txTS, 0, rxTS, int_usec);
            if (rv) {
                PTP_ERROR_FUNC("IDTStackAdaptor_AddDownlinkTimeStamps( )");
                return BCM_E_FAIL;
            }
        }
    }
    return rv;
}
#else
/* timestamp differences */
int _bcm_ptp_servo_AddTimestampDiffs(int unit,
    bcm_ptp_stack_id_t             ptp_id,
    int                            clock_num,
    bcm_ptp_port_identity_t *      master_portId,
    int                            num_TS,
    bcm_ptp_timestamp_t            first_TS,
    uint8*                         ts_ptr,
    _bcm_ptp_ts_type_t             direction,
    uint32                         int_usec)
{

    int rv = BCM_E_NONE;

	T_ieee1588PortIdentity master;
    T_ieee1588Timestamp firstTS = {0, 0};
	int i=0;
    int64_t ts_diff_arr[num_TS];

	if(PTP_SERVO_IS_BCM(unit, ptp_id, clock_num)) {
        return rv ;
    }

    if(!master_portId) { 
        return BCM_E_PARAM;
	}

    if(!IDTStackAdaptor_Initialized()) {
        return BCM_E_PARAM;
    }

    LOG_DEBUG(BSL_LS_BCM_PTP,
            (BSL_META_U(unit, "\nAdd Timestamps to servo: int[%dusec] num_TS[%d] direction[%d]\n"), int_usec, num_TS, direction));

    LOG_DEBUG(BSL_LS_BCM_PTP,
            (BSL_META_U(unit, " first_TS[0x%016llx %lldsec.%unsec] \n"), (long long int)first_TS.seconds, (long long int)first_TS.seconds, first_TS.nanoseconds));

    for (i=0; i<num_TS; i++) {
		ts_diff_arr[i] = _bcm_ptp_int64_read(ts_ptr +(i*8));
        LOG_DEBUG(BSL_LS_BCM_PTP,
            (BSL_META_U(unit, " [%d][0x%016llx/%lldnsec] \n"), i, (long long int)ts_diff_arr[i], (long long int)ts_diff_arr[i]));
	}

    sal_memcpy(&master.clockIdentity, &master_portId->clock_identity, sizeof(bcm_ptp_clock_identity_t));
    master.portNumber = master_portId->port_number; 

#if 0
    /* Enable only for debugging with first TS pair */
    firstTS.seconds = first_TS.seconds;
    firstTS.nanoseconds = first_TS.nanoseconds;
#endif

    if(num_TS > BCM_PTP_EXT_SERVO_TS_RATE_MAX) {
        LOG_VERBOSE(BSL_LS_BCM_PTP,
            (BSL_META_U(unit, "\n ***** NUM_TS exceeded max limit : int[%dusec] num_TS[%d] direction[%d] ***** \n"), int_usec, num_TS, direction));
        if (direction == _bcm_ptp_ts_type_downlink) {
            mbox_tsevent_stats.event_dl_ts_above_thres++;
        } else {
            mbox_tsevent_stats.event_ul_ts_above_thres++;
        }
        num_TS = BCM_PTP_EXT_SERVO_TS_RATE_MAX;
        /* return BCM_E_FAIL; */
    }

    if (direction == _bcm_ptp_ts_type_downlink) {
        mbox_tsevent_stats.event_dl_ts++;
        rv = IDTStackAdaptor_AddDownlinkTimeStampDifferences( &master, firstTS, num_TS, (T_osInt64 *)ts_diff_arr, int_usec );
        mbox_tsevent_stats.event_dl_ts_handled++;
        if (rv) {
            return BCM_E_FAIL;
        }
    } else {
        mbox_tsevent_stats.event_ul_ts++;
        rv = IDTStackAdaptor_AddUplinkTimeStampDifferences( &master, firstTS, num_TS, (T_osInt64 *)ts_diff_arr, int_usec );
        mbox_tsevent_stats.event_ul_ts_handled++;
        if (rv) {
           return BCM_E_FAIL;
        }
    }
    return rv;
}
#endif /* BCM_PTP_EXT_SERVO_ABS_TS_SUPPORT */

/*
 * Function:
 *      _bcm_ptp_ext_servo_ptsf_set
 * Purpose:
 *      set PTP servo state and status information.
 * Parameters:
 *      unit      - (IN)  Unit number.
 *      ptp_id    - (IN)  PTP stack ID.
 *      clock_num - (IN)  PTP clock number.
 *      state     - (IN)  PTP ext-servo's state.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
static int
_bcm_ptp_ext_servo_ptsf_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_port_identity_t *master,
    T_osBool b_signalFail)
{
    int rv = BCM_E_UNAVAIL;

    uint8 payload[PTP_MGMTMSG_PAYLOAD_PTSF_UNUSABLE_MSG_SIZE_OCTETS] = {0};
    uint8 resp[PTP_MGMTMSG_RESP_MAX_SIZE_OCTETS];
    int resp_len = PTP_MGMTMSG_RESP_MAX_SIZE_OCTETS;
    int i = 0;

    bcm_ptp_port_identity_t portid;

    if (BCM_FAILURE(rv = _bcm_ptp_function_precheck(unit, ptp_id,
            clock_num, PTP_CLOCK_PORT_NUMBER_DEFAULT))) {
        return rv;
    }

    if (BCM_FAILURE(rv = bcm_common_ptp_clock_port_identity_get(unit, ptp_id,
            clock_num, PTP_IEEE1588_ALL_PORTS, &portid))) {
        /* On error, target message to (all clocks, all ports) portIdentity. */
        portid = portid_all;
    }

    i = 0;
    sal_memcpy(payload,
               master->clock_identity,
               sizeof(bcm_ptp_clock_identity_t));
    i += sizeof(bcm_ptp_clock_identity_t);

    _bcm_ptp_uint16_write(payload + i, master->port_number);
    i += 2;

    payload[i] = b_signalFail ? 1 : 0;

    if (BCM_FAILURE(rv = _bcm_ptp_management_message_send(unit, ptp_id,
            clock_num, &portid,
            PTP_MGMTMSG_SET, PTP_MGMTMSG_ID_PORT_PTSF_UNUSABLE,
            payload, PTP_MGMTMSG_PAYLOAD_PTSF_UNUSABLE_MSG_SIZE_OCTETS,
            resp, &resp_len))) {
        return rv;
    }
    return rv;
}

#endif /* BCM_PTP_EXT_SERVO_SUPPORT */

#endif /* defined(INCLUDE_PTP)*/
