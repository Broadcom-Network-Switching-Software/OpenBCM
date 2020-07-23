#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# Attempt to locate yaml.h in standard places.
#

ifndef YAML
ifeq ($(YAML_HDR),)
YAML_CHK = /usr/include
YAML_HDR := $(shell find $(YAML_CHK) -name yaml.h -print)
endif
ifeq ($(YAML_HDR),)
YAML_CHK = /usr/local/include
YAML_HDR := $(shell find $(YAML_CHK) -name yaml.h -print)
endif
ifneq ($(YAML_HDR),)
YAML := $(YAML_CHK)
endif
endif
