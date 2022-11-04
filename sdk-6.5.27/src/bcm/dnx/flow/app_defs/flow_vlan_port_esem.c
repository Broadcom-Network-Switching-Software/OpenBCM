#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLOW

#include <soc/sand/shrextend/shrextend_debug.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_flow.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_esem.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_snif.h>
#include <bcm_int/dnx/lif/lif_table_mngr_lib.h>
#include <bcm_int/dnx/vlan/vlan.h>
#include <bcm_int/dnx/flow/flow.h>
#include "../flow_def.h"

/** Supported ESEM Initiator common fields */

#define VALID_VLAN_PORT_ESEM_INITIATOR_COMMON_FIELDS \
            BCM_FLOW_INITIATOR_ELEMENT_STAT_ID_VALID | \
            BCM_FLOW_INITIATOR_ELEMENT_STAT_PP_PROFILE_VALID | \
            BCM_FLOW_INITIATOR_ELEMENT_QOS_MAP_ID_VALID | \
            BCM_FLOW_INITIATOR_ELEMENT_L2_EGRESS_INFO_VALID | \
            BCM_FLOW_INITIATOR_ELEMENT_QOS_EGRESS_MODEL_VALID

#define VALID_VLAN_PORT_ESEM_NAME_SPACE_VSI_INITIATOR_COMMON_FIELDS \
            BCM_FLOW_INITIATOR_ELEMENT_STAT_ID_VALID | \
            BCM_FLOW_INITIATOR_ELEMENT_STAT_PP_PROFILE_VALID

#define VALID_VLAN_PORT_ESEM_NAME_SPACE_PORT_INITIATOR_COMMON_FIELDS \
            VALID_VLAN_PORT_ESEM_INITIATOR_COMMON_FIELDS

#define VALID_HIGH_SCALE_MIRROR_MAPPING_INITIATOR_COMMON_FIELDS \
            VALID_VLAN_PORT_ESEM_INITIATOR_COMMON_FIELDS

/** Supported ESEM Initiator special fields */
#define VALID_VLAN_PORT_ESEM_INITIATOR_SPECIAL_FIELDS \
            FLOW_S_F_VLAN_EDIT_PROFILE, \
            FLOW_S_F_VLAN_EDIT_VID_1, \
            FLOW_S_F_VLAN_EDIT_VID_2, \
            FLOW_S_F_QOS_PRI, \
            FLOW_S_F_QOS_CFI

#define VALID_VLAN_PORT_ESEM_NAMESPACE_VSI_INITIATOR_SPECIAL_FIELDS \
            FLOW_S_F_VLAN_EDIT_PROFILE, \
            FLOW_S_F_VLAN_EDIT_VID_1

#define VALID_VLAN_PORT_ESEM_NAMESPACE_PORT_INITIATOR_SPECIAL_FIELDS \
        VALID_VLAN_PORT_ESEM_INITIATOR_SPECIAL_FIELDS

#define VALID_HIGH_SCALE_MIRROR_MAPPING_INITIATOR_SPECIAL_FIELDS \
        VALID_VLAN_PORT_ESEM_INITIATOR_SPECIAL_FIELDS

#define VALID_VLAN_PORT_ESEM_NW_SCOPED_INITIATOR_KEYS \
            FLOW_S_F_VSI_MATCH, \
            FLOW_S_F_NAME_SPACE_MATCH, \
            FLOW_S_F_VID_MATCH

#define VALID_VLAN_PORT_ESEM_LIF_SCOPED_INITIATOR_KEYS \
            FLOW_S_F_VSI_MATCH, \
            FLOW_S_F_OUT_LIF_MATCH

#define VALID_VLAN_PORT_ESEM_NAMESPACE_VSI_INITIATOR_KEYS \
            FLOW_S_F_VSI_MATCH, \
            FLOW_S_F_NAME_SPACE_MATCH

#define VALID_VLAN_PORT_ESEM_NAMESPACE_PORT_INITIATOR_KEYS \
            FLOW_S_F_SYSTEM_PORT_MATCH, \
            FLOW_S_F_CLASS_ID_MATCH

#define VALID_HIGH_SCALE_MIRROR_MAPPING_INITIATOR_KEYS \
                FLOW_S_F_SYSTEM_PORT_MATCH, \
                FLOW_S_F_NAME_SPACE_MATCH

/** Supported ESEM initiator indications */
#define VALID_VLAN_PORT_ESEM_INITIATOR_INDICATIONS (SAL_BIT(FLOW_APP_INIT_IND_VIRTUAL_LIF_ONLY) | SAL_BIT(FLOW_APP_INIT_IND_NO_LOCAL_LIF))
