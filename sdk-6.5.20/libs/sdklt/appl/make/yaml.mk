#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# YAML linker configuration.
#
# This makefile defines the linker parameters for YAML support.
#
#   YAML_LDLIBS
#   This is the name of the YAML library preceded by the relevant
#   linker option.
#
#   YAML_LDFLAGS
#   These are the linker options which specify the location of the the
#   YAML library on the build host and on the target platform (if
#   different from the build host).
#
# Although an application can override the above variables, the
# location of the YAML libraries should instead be defined using the
# following variables:
#
#   YAML_LIBDIR
#   The location of the YAML library on the build host.
#
#   YAML_TARGET_LIBDIR
#   The location of the YAML library on the target system if the
#   application uses shared libraries. Note that this variable only
#   needs to be set if it differs from YAML_LIBDIR.
#

# Broadcom settings for YAML
-include $(SDK)/INTERNAL/appl/make/yaml_broadcom.mk

ifdef YAML

# Only export YAML if the library exists
export YAML

ifdef YAML_LIBDIR

# Default YAML library directory on target system
ifndef YAML_TARGET_LIBDIR
YAML_TARGET_LIBDIR = $(YAML_LIBDIR)
endif

ifndef YAML_LDFLAGS
YAML_LDFLAGS += -L$(YAML_LIBDIR) -Xlinker -R$(YAML_TARGET_LIBDIR)
endif

endif # YAML_LIBDIR

ifndef YAML_LDLIBS
YAML_LDLIBS  += -lyaml
endif

endif # YAML
