/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <soc/types.h>
#include <soc/error.h>
#include <soc/drv.h>
#include <soc/wb_engine.h>
#include <soc/portmod/portmod_internal.h>
#include <soc/portmod/portmod.h>
#include <soc/portmod/portmod_common.h>
#include <soc/portmod/portmod_chain.h>
#include <soc/portmod/portmod_dispatch.h>

#ifdef _ERR_MSG_MODULE_NAME
#error "_ERR_MSG_MODULE_NAME redefined"
#endif 
#define _ERR_MSG_MODULE_NAME BSL_LS_SOC_PORT

typedef void (*portmod_line_to_sys_intf_map_f)(const phymod_phy_access_t*, soc_port_t port, int data_rate,
                                            phymod_interface_t line_intf, phymod_interface_t *sys_intf); 

typedef void (*portmod_port_interface_is_valid_f) (const phymod_phy_access_t* phy_acc, soc_port_t port, int data_rate,
                                  phymod_interface_t line_intf, int num_lanes, uint32_t intf_mode, int *is_valid);

typedef void (*portmod_line_to_ext_line_intf_map_f)(const phymod_phy_access_t*, soc_port_t port,
                                            soc_port_if_t line_intf, soc_port_if_t *sys_intf); 

typedef void (*portmod_port_default_intf_f)(const portmod_port_interface_config_t* config,
                                            soc_port_if_t* interface);

typedef struct _portmod_intf_map_s_
{
    phymod_dispatch_type_t            dispatch_type;
    portmod_line_to_sys_intf_map_f    portmod_intf_map; 
} portmod_intf_map_t;

typedef struct _portmod_line_intf_map_s_
{
    phymod_dispatch_type_t            dispatch_type;
    portmod_line_to_ext_line_intf_map_f    portmod_intf_map; 
} portmod_line_intf_map_t;

typedef struct portmod_intf_valid_s {

    phymod_dispatch_type_t               dispatch_type;
    portmod_port_interface_is_valid_f    portmod_intf_valid;    
} portmod_intf_valid_t;

typedef struct portmod_default_intf_map_s
{
    phymod_dispatch_type_t               dispatch_type;
    portmod_port_default_intf_f          portmod_default_intf;
} portmod_default_intf_map_t;

#ifdef PHYMOD_SESTO_SUPPORT
STATIC
void sesto_sys_intf_map(const phymod_phy_access_t* phy_acc, soc_port_t port, int data_rate,
                   phymod_interface_t line_intf, phymod_interface_t *sys_intf) 
{
    if (data_rate >= 100000) {
         *sys_intf = phymodInterfaceCAUI;
    } else if (data_rate >= 40000) {
        if (PHYMOD_INTF_CONFIG_PHY_GEARBOX_ENABLE_GET(phy_acc)) {
            *sys_intf = phymodInterfaceXLAUI2;
        } else {
            *sys_intf = phymodInterfaceXLAUI;
        }
    } else if (data_rate >= 10000) {
         *sys_intf = phymodInterfaceXFI;
    }
}

STATIC
void sesto_line_intf_map(const phymod_phy_access_t* phy_acc, soc_port_t port,
                   soc_port_if_t line_intf, soc_port_if_t *mapped_line_intf) 
{
    switch(line_intf) {
        case SOC_PORT_IF_CAUI:
        case SOC_PORT_IF_SR4:
        case SOC_PORT_IF_LR4:
        case SOC_PORT_IF_ER4:
        case SOC_PORT_IF_XLAUI:
        case SOC_PORT_IF_XLPPI:
        case SOC_PORT_IF_CAUI4:
        case SOC_PORT_IF_CAUI_C2C:
        case SOC_PORT_IF_CAUI_C2M:
        case SOC_PORT_IF_CR4:
            *mapped_line_intf = SOC_PORT_IF_CR4;
            break;
        default:
            *mapped_line_intf =  SOC_PORT_IF_KR4;
            break;  
    }

}

STATIC
void sesto_line_intf_check(const phymod_phy_access_t* phy_acc, soc_port_t port, int data_rate,
                   phymod_interface_t line_intf, int num_lanes, uint32_t intf_mode, int *is_valid)
{
    /* num lanes here indicates the number of lanes on the serdes */
    if (num_lanes == 1) {
        switch (line_intf) {
            /* less than 10G speeds are not supported no need to check for data rate */
            /* Optical Interface types supported by Sesto */
            case phymodInterfaceLR:
            case phymodInterfaceSR:
            case phymodInterfaceER:
            case phymodInterfaceSFI:
            /* Cu Interface types supported by Sesto */
            case phymodInterfaceCR:
            /* Back plane interfaces supported by sesto */
            case phymodInterfaceKR:
            case phymodInterfaceXFI:
                *is_valid = 1;
                break;
            default:
            /* unsupported interface type for this port mode */
                *is_valid = 0;
                break;    
        }     
    } else if (num_lanes == 2) {
        switch (line_intf) {
            /* optical interface type */
            case phymodInterfaceSR2:
            case phymodInterfaceLR2:
            case phymodInterfaceER2:
            /* Cu interface type */ 
            case phymodInterfaceCR2:
            /* back plane */
            case phymodInterfaceKR2:
            case phymodInterfaceXLAUI2:
                *is_valid = 1;
                break;
            default:
                *is_valid = 0;
                break;    
        }

    } else if (num_lanes == 4) {
        switch (line_intf) {
            /* optical interface type */
            case phymodInterfaceSR4:
            case phymodInterfaceLR4:
            case phymodInterfaceER4:
            case phymodInterfaceVSR:
            case phymodInterfaceXLPPI:
            /* Cu interface type */ 
            case phymodInterfaceCR4:
            /* back plane */
            case phymodInterfaceKR4:
            case phymodInterfaceXLAUI:
            case phymodInterfaceCAUI4:
            case phymodInterfaceCAUI:
            case phymodInterfaceCAUI4_C2C:
            case phymodInterfaceCAUI4_C2M: /* is this valid for 4 lanes ? */ 
            /* mux mode system side has 4 lanes and line side 2 lanes */
            /* optical interface type */
            case phymodInterfaceSR2:
            case phymodInterfaceLR2:
            case phymodInterfaceER2:
            /* Cu interface type */
            case phymodInterfaceCR2:
            /* back plane */
            case phymodInterfaceKR2:
            case phymodInterfaceXLAUI2:
                *is_valid = 1;
                break;
            default:
                *is_valid = 0;
                break;    
        }

    } else if (num_lanes == 10) {        
        switch (line_intf) {

            

            /* config while connected to pm4x10 */
            case phymodInterfaceSR4:
            case phymodInterfaceLR4:
            case phymodInterfaceER4:
            case phymodInterfaceVSR:
            case phymodInterfaceXLPPI:
            /* Cu interface type */ 
            case phymodInterfaceCR4:
            /* back plane */
            case phymodInterfaceKR4:
            case phymodInterfaceXLAUI:
            case phymodInterfaceCAUI4:
                *is_valid = 1;
                break;

                
            /* interface below are supported only in inverse gearbox mode */    
            
            /* optical interface type */
            case phymodInterfaceSR10:
            case phymodInterfaceLR10:
            /* Cu interface type */ 
            case phymodInterfaceCR10:
            /* back plane */
            case phymodInterfaceKR10:
            case phymodInterfaceCAUI:
            case phymodInterfaceCAUI4_C2C:
            case phymodInterfaceCAUI4_C2M: /* is this valid for 4 lanes ? */ 
                *is_valid = 1;
                break;
            default:
                *is_valid = 0;
                break;    
        }
    } else {
        /* invalid number of lanes */
        *is_valid = 0;
    }
     
}

STATIC
void sesto_default_intf(const portmod_port_interface_config_t* config,
                        soc_port_if_t* interface)
{
    int num_lanes = config->port_num_lanes;
    int is_higig = ((config->encap_mode == SOC_ENCAP_HIGIG2) ||
                    (config->encap_mode == SOC_ENCAP_HIGIG)) ? 1 : 0;

    if (num_lanes == 1) {
        *interface = is_higig ? SOC_PORT_IF_CR : SOC_PORT_IF_XFI;
    } else if (num_lanes == 2) {
        *interface = is_higig ? SOC_PORT_IF_CR2 : SOC_PORT_IF_XLAUI2;
    } else if (num_lanes == 4) {
        *interface = is_higig ? SOC_PORT_IF_CR4 : SOC_PORT_IF_XLAUI;
    } else if (num_lanes == 10) {
        *interface = is_higig ? SOC_PORT_IF_CR10 : SOC_PORT_IF_CAUI;
    } else {
        *interface = SOC_PORT_IF_NULL;
    }
}
#endif

#ifdef PHYMOD_DINO_SUPPORT
STATIC
void dino_sys_intf_map(const phymod_phy_access_t* phy_acc, soc_port_t port, int data_rate,
                   phymod_interface_t line_intf, phymod_interface_t *sys_intf) 
{
    if (data_rate >= 100000) {
         *sys_intf = phymodInterfaceCAUI;
    } else if (data_rate >= 40000) {
            *sys_intf = phymodInterfaceXLAUI;
    } else if (data_rate >= 10000) {
         *sys_intf = phymodInterfaceXFI;
    } else {
         *sys_intf = phymodInterfaceSGMII;
    }
}

STATIC
void dino_line_intf_map(const phymod_phy_access_t* phy_acc, soc_port_t port,
                   soc_port_if_t line_intf, soc_port_if_t *mapped_line_intf) 
{
    switch(line_intf) {
        case SOC_PORT_IF_CAUI:
        case SOC_PORT_IF_SR4:
        case SOC_PORT_IF_LR4:
        case SOC_PORT_IF_ER4:
        case SOC_PORT_IF_CR4:
        case SOC_PORT_IF_XLAUI:
        case SOC_PORT_IF_XLPPI:
        case SOC_PORT_IF_CAUI4:
        case SOC_PORT_IF_CAUI_C2C:
        case SOC_PORT_IF_CAUI_C2M:
            *mapped_line_intf = SOC_PORT_IF_CR4;
            break;
        case SOC_PORT_IF_CR10:
            *mapped_line_intf = SOC_PORT_IF_CR10;
            break;
        case SOC_PORT_IF_KR10:
            *mapped_line_intf = SOC_PORT_IF_KR10;
            break;
        default:
            *mapped_line_intf =  SOC_PORT_IF_KR4;
            break;  
    }

}

STATIC
void dino_line_intf_check(const phymod_phy_access_t* phy_acc, soc_port_t port, int data_rate,
                   phymod_interface_t line_intf, int num_lanes, uint32_t intf_mode, int *is_valid)
{
    /* num lanes here indicates the number of lanes on the serdes */
    if (num_lanes == 1) {
        switch (line_intf) {
            /* less than 10G speeds are not supported no need to check for data rate */
            /* Optical Interface types supported by Sesto */
            case phymodInterfaceLR:
            case phymodInterfaceSR:
            case phymodInterfaceER:
            case phymodInterfaceSFI:
            /* Cu Interface types supported by dino */
            case phymodInterfaceCR:
            case phymodInterfaceCX:
            case phymodInterfaceSGMII:
            /* Back plane interfaces supported by dino */
            case phymodInterfaceKR:
            case phymodInterfaceKX:
            case phymodInterfaceXFI:
                *is_valid = 1;
                break;
            default:
            /* unsupported interface type for this port mode */
                *is_valid = 0;
                break;    
        }     
    } else if (num_lanes == 2) {
        switch (line_intf) {
            /* optical interface type */
            case phymodInterfaceSR2:
            case phymodInterfaceLR2:
            case phymodInterfaceER2:
            /* Cu interface type */ 
            case phymodInterfaceCR2:
            /* back plane */
            case phymodInterfaceKR2:
            case phymodInterfaceXLAUI2:
                *is_valid = 1;
                break;
            default:
                *is_valid = 0;
                break;    
        }

    } else if (num_lanes == 4) {
        switch (line_intf) {
            /* optical interface type */
            case phymodInterfaceSR4:
            case phymodInterfaceLR4:
            case phymodInterfaceER4:
            case phymodInterfaceVSR:
            case phymodInterfaceXLPPI:
            /* Cu interface type */ 
            case phymodInterfaceCR4:
            /* back plane */
            case phymodInterfaceKR4:
            case phymodInterfaceXLAUI:
            case phymodInterfaceCAUI4:
            case phymodInterfaceCAUI:
            case phymodInterfaceCAUI4_C2C:
            case phymodInterfaceCAUI4_C2M: /* is this valid for 4 lanes ? */ 
                *is_valid = 1;
                break;
            default:
                *is_valid = 0;
                break;    
        }

    } else if (num_lanes == 10) {        
        switch (line_intf) {

            /* config while connected to pm4x10*/
            case phymodInterfaceSR4:
            case phymodInterfaceLR4:
            case phymodInterfaceER4:
            case phymodInterfaceVSR:
            case phymodInterfaceXLPPI:
            /* Cu interface type */ 
            case phymodInterfaceCR4:
            /* back plane */
            case phymodInterfaceKR4:
            case phymodInterfaceXLAUI:
            case phymodInterfaceCAUI4:
                *is_valid = 1;
                break;
            /* interface below are supported only in inverse gearbox mode */    
            /* optical interface type */
            case phymodInterfaceSR10:
            case phymodInterfaceLR10:
            /* Cu interface type */ 
            case phymodInterfaceCR10:
            /* back plane */
            case phymodInterfaceKR10:
            case phymodInterfaceCAUI:
            case phymodInterfaceCAUI4_C2C:
            case phymodInterfaceCAUI4_C2M: /* is this valid for 4 lanes ? */ 
                *is_valid = 1;
                break;
            default:
                *is_valid = 0;
                break;    
        }
    } else {
        /* invalid number of lanes */
        *is_valid = 0;
    }
}

STATIC
void dino_default_intf(const portmod_port_interface_config_t* config,
                       soc_port_if_t* interface)
{
    int num_lanes = config->port_num_lanes;
    int is_higig = ((config->encap_mode == SOC_ENCAP_HIGIG2) ||
                    (config->encap_mode == SOC_ENCAP_HIGIG)) ? 1 : 0;

    if (num_lanes == 1) {
        *interface = is_higig ? SOC_PORT_IF_CR : SOC_PORT_IF_XFI;
    } else if (num_lanes == 2) {
        *interface = is_higig ? SOC_PORT_IF_CR2 : SOC_PORT_IF_XLAUI2;
    } else if (num_lanes == 4) {
        *interface = is_higig ? SOC_PORT_IF_CR4 : SOC_PORT_IF_XLAUI;
    } else if (num_lanes == 10) {
        *interface = is_higig ? SOC_PORT_IF_CR10 : SOC_PORT_IF_CAUI;
    } else {
        *interface = SOC_PORT_IF_NULL;
    }
}
#endif

#ifdef PHYMOD_QUADRA28_SUPPORT
STATIC
void quadra28_sys_intf_map(const phymod_phy_access_t* phy_acc, soc_port_t port, int data_rate,
                   phymod_interface_t line_intf, phymod_interface_t *sys_intf) 
{
    switch(line_intf) {
        case phymodInterfaceLR:
        case phymodInterfaceSR:
        case phymodInterfaceCR:
        case phymodInterfaceKR:
             *sys_intf = phymodInterfaceXFI;
             break;

        case phymodInterfaceLR4:
        case phymodInterfaceSR4:
        case phymodInterfaceCR4:
        case phymodInterfaceKR4:
             *sys_intf = phymodInterfaceXLAUI;
             break;

        case phymodInterfaceXFI:
             *sys_intf = phymodInterfaceKR;
             break;
  
        default: /* by default set system side same as line side */
             *sys_intf = line_intf;
             break;
    }
}

STATIC
void quadra28_line_intf_map(const phymod_phy_access_t* phy_acc, soc_port_t port,
                   soc_port_if_t line_intf, soc_port_if_t *mapped_line_intf) 
{
    switch(line_intf) {
        case SOC_PORT_IF_CAUI:
        case SOC_PORT_IF_SR4:
        case SOC_PORT_IF_LR4:
        case SOC_PORT_IF_ER4:
        case SOC_PORT_IF_XLAUI:
        case SOC_PORT_IF_XLPPI:
        case SOC_PORT_IF_CAUI4:
        case SOC_PORT_IF_CAUI_C2C:
        case SOC_PORT_IF_CAUI_C2M:
            *mapped_line_intf = SOC_PORT_IF_CR4;
            break;
        case SOC_PORT_IF_SGMII:
            *mapped_line_intf = SOC_PORT_IF_SGMII;
            break;
        default:
            *mapped_line_intf =  SOC_PORT_IF_KR4;
            break;  
    }

}

STATIC
void quadra28_line_intf_check(const phymod_phy_access_t* phy_acc, soc_port_t port, int data_rate,
                   phymod_interface_t line_intf, int num_lanes, uint32_t intf_mode, int *is_valid) 
{
     /* num lanes here indicates the number of lanes on the serdes */
    if (num_lanes == 1) {
        switch (line_intf) {
            /* Optical Interface types supported by Quadra */
            case phymodInterfaceLR:
            case phymodInterfaceSR:
            case phymodInterfaceER:
            case phymodInterfaceSFI:
            case phymodInterfaceZR:
            case phymodInterfaceLRM:
            /* Cu Interface types supported by quadra */
            case phymodInterfaceCR:
            case phymodInterfaceCX:
            case phymodInterfaceKX:
            case phymodInterface1000X:
            /* Back plane interfaces supported by quadra */
            case phymodInterfaceKR:
            case phymodInterfaceXFI:
            /* interfaces below 1.1G */
            case phymodInterfaceSGMII:
                *is_valid = 1;
                break;
            default:
            /* unsupported interface type for this port mode */
                *is_valid = 0;
                break;    
        }     
    } else if (num_lanes == 2) {
        switch (line_intf) {
            /* optical interface type */
            case phymodInterfaceSR2:
            case phymodInterfaceLR2:
            case phymodInterfaceER2:
            /* Cu interface type */ 
            case phymodInterfaceCR2:
            /* back plane */
            case phymodInterfaceKR2:
            case phymodInterfaceXLAUI2:
                *is_valid = 1;
                break;
            default:
                *is_valid = 0;
                break;    
        }

    } else if (num_lanes == 4) {
        switch (line_intf) {
            /* optical interface type */
            case phymodInterfaceSR4:
            case phymodInterfaceLR4:
            case phymodInterfaceER4:
            case phymodInterfaceVSR:
            case phymodInterfaceXLPPI:
            /* Cu interface type */ 
            case phymodInterfaceCR4:
            /* back plane */
            case phymodInterfaceKR4:
            case phymodInterfaceXLAUI:
            case phymodInterfaceCAUI4:
            case phymodInterfaceCAUI:
            case phymodInterfaceCAUI4_C2C:
            case phymodInterfaceCAUI4_C2M: /* is this valid for 4 lanes ? */ 
                *is_valid = 1;
                break;
            default:
                *is_valid = 0;
                break;    
        }

    } else if (num_lanes == 10) {        
      /* no 10 lane support */
        *is_valid = 0;
    } else {
        /* invalid number of lanes */
        *is_valid = 0;
    }
}

STATIC
void quadra28_default_intf(const portmod_port_interface_config_t* config,
                       soc_port_if_t* interface)
{
    int num_lanes = config->port_num_lanes;
    int is_higig = ((config->encap_mode == SOC_ENCAP_HIGIG2) ||
                    (config->encap_mode == SOC_ENCAP_HIGIG)) ? 1 : 0;

    if (num_lanes == 1) {
        *interface = is_higig ? SOC_PORT_IF_CR : SOC_PORT_IF_XFI;
        if (config->speed < 1100) {
            *interface = SOC_PORT_IF_SGMII;
        }
    } else if (num_lanes == 2) {
        *interface = is_higig ? SOC_PORT_IF_CR2 : SOC_PORT_IF_XLAUI2;
    } else if (num_lanes == 4) {
        *interface = is_higig ? SOC_PORT_IF_CR4 : SOC_PORT_IF_XLAUI;
    } else {
        *interface = SOC_PORT_IF_NULL;
    }
}
#endif

#ifdef PHYMOD_FURIA_SUPPORT
STATIC
void furia_sys_intf_map(const phymod_phy_access_t* phy_acc, soc_port_t port, int data_rate,
                   phymod_interface_t line_intf, phymod_interface_t *sys_intf)
{
    if (data_rate >= 100000) {
         *sys_intf = phymodInterfaceCAUI4;
    } else if (data_rate >= 40000) {
         *sys_intf = phymodInterfaceXLAUI;
    } else if (data_rate >= 10000) {
         *sys_intf = phymodInterfaceXFI;
    }

}

STATIC
void furia_line_intf_map(const phymod_phy_access_t* phy_acc, soc_port_t port,
                   soc_port_if_t line_intf, soc_port_if_t *mapped_line_intf)
{
    switch(line_intf) {
        case SOC_PORT_IF_CAUI:
        case SOC_PORT_IF_SR4:
        case SOC_PORT_IF_LR4:
        case SOC_PORT_IF_ER4:
        case SOC_PORT_IF_XLAUI:
        case SOC_PORT_IF_XLPPI:
        case SOC_PORT_IF_CAUI4:
        case SOC_PORT_IF_CAUI_C2C:
        case SOC_PORT_IF_CAUI_C2M:
            *mapped_line_intf = SOC_PORT_IF_CR4;
            break;
        default:
            *mapped_line_intf =  SOC_PORT_IF_KR4;
            break;  
    }
   
}

STATIC
void furia_line_intf_check(const phymod_phy_access_t* phy_acc, soc_port_t port, int data_rate,
                   phymod_interface_t line_intf, int num_lanes, uint32_t intf_mode, int *is_valid)
{
      /* num lanes here indicates the number of lanes on the serdes */
    if (num_lanes == 1) {
        switch (line_intf) {
            /* Optical Interface types supported by Quadra */
            case phymodInterfaceLR:
            case phymodInterfaceSR:
            case phymodInterfaceER:
            case phymodInterfaceSFI:
            case phymodInterfaceZR:
            case phymodInterfaceLRM:
            /* Cu Interface types supported by quadra */
            case phymodInterfaceCR:
            case phymodInterfaceCX:
            case phymodInterfaceKX:
            case phymodInterface1000X:
            /* Back plane interfaces supported by quadra */
            case phymodInterfaceKR:
            case phymodInterfaceXFI:
                *is_valid = 1;
                break;
            default:
            /* unsupported interface type for this port mode */
                *is_valid = 0;
                break;    
        }     
    } else if (num_lanes == 2) {
        switch (line_intf) {
            /* optical interface type */
            case phymodInterfaceSR2:
            case phymodInterfaceLR2:
            case phymodInterfaceER2:
            /* Cu interface type */ 
            case phymodInterfaceCR2:
            /* back plane */
            case phymodInterfaceKR2:
            case phymodInterfaceXLAUI2:
                *is_valid = 1;
                break;
            default:
                *is_valid = 0;
                break;    
        }

    } else if (num_lanes == 4) {
        switch (line_intf) {
            /* optical interface type */
            case phymodInterfaceSR4:
            case phymodInterfaceLR4:
            case phymodInterfaceER4:
            case phymodInterfaceVSR:
            case phymodInterfaceXLPPI:
            /* Cu interface type */ 
            case phymodInterfaceCR4:
            /* back plane */
            case phymodInterfaceKR4:
            case phymodInterfaceXLAUI:
            case phymodInterfaceCAUI4:
            case phymodInterfaceCAUI:
            case phymodInterfaceCAUI4_C2C:
            case phymodInterfaceCAUI4_C2M: /* is this valid for 4 lanes ? */ 
                *is_valid = 1;
                break;
            default:
                *is_valid = 0;
                break;    
        }

    } else if (num_lanes == 10) {        
      /* no 10 lane support */
        *is_valid = 0;
    } else {
        /* invalid number of lanes */
        *is_valid = 0;
    }
}

STATIC
void furia_default_intf(const portmod_port_interface_config_t* config,
                       soc_port_if_t* interface)
{
    int num_lanes = config->port_num_lanes;
    int is_higig = ((config->encap_mode == SOC_ENCAP_HIGIG2) ||
                    (config->encap_mode == SOC_ENCAP_HIGIG)) ? 1 : 0;

    if (num_lanes == 1) {
        *interface = is_higig ? SOC_PORT_IF_CR : SOC_PORT_IF_XFI;
    } else if (num_lanes == 2) {
        *interface = is_higig ? SOC_PORT_IF_CR2 : SOC_PORT_IF_XLAUI2;
    } else if (num_lanes == 4) {
        *interface = is_higig ? SOC_PORT_IF_CR4 : SOC_PORT_IF_XLAUI;
    } else {
        *interface = SOC_PORT_IF_NULL;
    }
}
#endif


portmod_intf_map_t  portmod_intf_map_array[] =
{
#ifdef PHYMOD_SESTO_SUPPORT
    {phymodDispatchTypeSesto, sesto_sys_intf_map},
#endif
#ifdef PHYMOD_DINO_SUPPORT
    {phymodDispatchTypeDino, dino_sys_intf_map},
#endif
#ifdef PHYMOD_QUADRA28_SUPPORT
    {phymodDispatchTypeQuadra28, quadra28_sys_intf_map},
#endif
#ifdef PHYMOD_FURIA_SUPPORT
        {phymodDispatchTypeFuria, furia_sys_intf_map},
#endif
    {phymodDispatchTypeCount, NULL},
    {phymodDispatchTypeInvalid, NULL}
};

int
portmod_port_line_to_sys_intf_map(int unit, const phymod_phy_access_t* line_phy_acc, const phymod_phy_access_t* sys_phy_acc,
                                  soc_port_t port, const phymod_phy_inf_config_t *config, int ref_clk, 
                                  const uint32 phy_init_flags, phymod_interface_t line_intf, phymod_interface_t *sys_intf)
{
    int count = 0;
    int rv = PHYMOD_E_NONE;
    phymod_phy_inf_config_t tmp_config;
    int sys_if_valid = 1;
    int data_rate = config->data_rate;
    int is_higig = PHYMOD_INTF_MODES_HIGIG_GET(config);
    int is_legacy_phy;
    /* play safe - always make sys_intf same as line intf */
    *sys_intf = line_intf;

    while(portmod_intf_map_array[count].dispatch_type != phymodDispatchTypeCount) {
        if (portmod_intf_map_array[count].dispatch_type == line_phy_acc->type) {
            portmod_intf_map_array[count].portmod_intf_map(line_phy_acc, port, data_rate, line_intf, sys_intf);
            break;
        } 
        count++;
    } 

    /* If the line-side PHY is legacy, the sys-side interface is yet to be determined at this point.
     * The stragegy below is:
     * 1.  When phy_init_flags == PORTMOD_INIT_F_INTERNAL_SERDES_ONLY, line_intf should be provided to
     *     the internal PHY without a doubt. 
     * 2.  When phy_init_flags != PORTMOD_INIT_F_INTERNAL_SERDES_ONLY, we look at whether there is
     *     already a meaningful interface type residing in the internal PHY.
     * 2.1 If there is a meaningful interface type in the internal PHY, we ignore line_intf and put
     *     the existing interface type back into the internal PHY.
     * 2.2 Otherwise we assign line_intf to the internal PHY.
     */
    rv = portmod_xphy_is_legacy_phy_get(unit, line_phy_acc->access.addr, &is_legacy_phy);
    if (rv) return (rv);
    if (portmod_intf_map_array[count].dispatch_type == phymodDispatchTypeCount && is_legacy_phy){
        if (phy_init_flags == PORTMOD_INIT_F_INTERNAL_SERDES_ONLY){
            *sys_intf = line_intf;
        } else{
            rv = phymod_phy_interface_config_get(sys_phy_acc, 0, ref_clk, &tmp_config);
            if (rv) return (rv);

            /* It is possible that the interface type of the internal PHY does not make sense according
             * to the given speed. This happens during the boot-up process. For example,
             * _tsce_speed_id_interface_config_get() returns SGMII if the speed ID does not
             * exist. At the same time the speed given by the external PHY callback function
             * may be 40G. In this case, we honor whatever interface type provided from the line side.
             */
            if (((data_rate > 1100) && tmp_config.interface_type == phymodInterfaceSGMII)||
                ((data_rate <= 1100) && (tmp_config.interface_type != phymodInterfaceSGMII))){
                sys_if_valid = 0;
            } 

            if (tmp_config.interface_type != phymodInterfaceBypass && tmp_config.interface_type != phymodInterfaceCount && sys_if_valid){
                *sys_intf = tmp_config.interface_type;
            } else{
                *sys_intf = line_intf;
            }
        }

        /* Historically a TD2P+G28 40G HG2 port should have XGMII as the TSCE interface type
         * and XLAUI as the ext PHY line-side interface type by default. This is to make sure
         * that a TD2P+G28 40G HG2 port can link up with a 40G HG2 PHY-less port running on
         * a different box with an older version of the SDK.
         */
        if ((data_rate==40000 || data_rate==42000) &&
            is_higig && line_intf== phymodInterfaceXLAUI) {
            *sys_intf = phymodInterfaceXGMII;
        }
    }

    return (SOC_E_NONE);
}

portmod_intf_valid_t  portmod_line_intf_check_array[] =
{
#ifdef PHYMOD_SESTO_SUPPORT
    {phymodDispatchTypeSesto, sesto_line_intf_check},
#endif
#ifdef PHYMOD_DINO_SUPPORT
    {phymodDispatchTypeDino, dino_line_intf_check},
#endif
#ifdef PHYMOD_QUADRA28_SUPPORT
    {phymodDispatchTypeQuadra28, quadra28_line_intf_check},
#endif
#ifdef PHYMOD_FURIA_SUPPORT
    {phymodDispatchTypeFuria, furia_line_intf_check},
#endif
    {phymodDispatchTypeCount, NULL},
    {phymodDispatchTypeInvalid, NULL}
};


portmod_line_intf_map_t  portmod_line_intf_map_array[] =
{
#ifdef PHYMOD_SESTO_SUPPORT
    {phymodDispatchTypeSesto, sesto_line_intf_map},
#endif
#ifdef PHYMOD_DINO_SUPPORT
    {phymodDispatchTypeDino, dino_line_intf_map},
#endif
#ifdef PHYMOD_QUADRA28_SUPPORT
    {phymodDispatchTypeQuadra28, quadra28_line_intf_map},
#endif
#ifdef PHYMOD_FURIA_SUPPORT
    {phymodDispatchTypeFuria, furia_line_intf_map},
#endif
    {phymodDispatchTypeCount, NULL},
    {phymodDispatchTypeInvalid, NULL}
};


portmod_default_intf_map_t portmod_phymod_xphy_default_intf_array[] =
{
#ifdef PHYMOD_SESTO_SUPPORT
    {phymodDispatchTypeSesto, sesto_default_intf},
#endif
#ifdef PHYMOD_DINO_SUPPORT
    {phymodDispatchTypeDino, dino_default_intf},
#endif
#ifdef PHYMOD_QUADRA28_SUPPORT
    {phymodDispatchTypeQuadra28, quadra28_default_intf},
#endif
#ifdef PHYMOD_FURIA_SUPPORT
    {phymodDispatchTypeFuria, furia_default_intf},
#endif
    {phymodDispatchTypeCount, NULL},
    {phymodDispatchTypeInvalid, NULL}
};

/* validates the given inetrface type and returns 1 if the interface is valid 
 * else returns 0
*/

int
portmod_port_line_interface_is_valid(int unit, const phymod_phy_access_t* phy_acc, soc_port_t port, int data_rate,
                                  phymod_interface_t line_intf, int num_lanes, uint32_t intf_mode, int *is_valid)
{
    int count = 0;

    while(portmod_line_intf_check_array[count].dispatch_type != phymodDispatchTypeCount) {
        if (portmod_line_intf_check_array[count].dispatch_type == phy_acc->type) {
            portmod_line_intf_check_array[count].portmod_intf_valid(phy_acc, port, data_rate, line_intf, num_lanes, intf_mode, is_valid);
            break;
        } 
        count++;
    } 
    return (SOC_E_NONE);
}

int
portmod_port_line_interface_map_for_xphy(int unit, const phymod_phy_access_t* phy_acc, soc_port_t port,
                                  soc_port_if_t line_intf, soc_port_if_t *mapped_line_intf)
{
    int count = 0;

    while(portmod_line_intf_map_array[count].dispatch_type != phymodDispatchTypeCount) {
        if (portmod_line_intf_map_array[count].dispatch_type == phy_acc->type) {
            portmod_line_intf_map_array[count].portmod_intf_map(phy_acc, port, line_intf, mapped_line_intf);
            break;
        }
        count++;
    }
    return (SOC_E_NONE);
}

int
portmod_port_phymod_xphy_default_interface_get(int unit, const phymod_phy_access_t* phy_acc,
                                               const portmod_port_interface_config_t* config,
                                               soc_port_if_t* interface)
{
    int count = 0;

    while(portmod_phymod_xphy_default_intf_array[count].dispatch_type != phymodDispatchTypeCount) {
        if (portmod_phymod_xphy_default_intf_array[count].dispatch_type == phy_acc->type) {
            portmod_phymod_xphy_default_intf_array[count].portmod_default_intf(config, interface);
            break;
        }
        count++;
    }
    return (SOC_E_NONE);
}

#undef _ERR_MSG_MODULE_NAME
