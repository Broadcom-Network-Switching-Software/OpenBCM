#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# Check that LibYAML is properly configured.
#

yaml_check:
ifndef YAML
ifneq ($(NO_YAML),1)
	@echo
	@echo '*** LibYAML not configured properly! ***'
	@echo
	@echo 'LibYAML is required to load the SDK configuration files.'
	@echo
	@echo 'If you want to compile without this support, then add NO_YAML=1'
	@echo 'to your make command.'
	@echo
	@echo 'The $$YAML environment variable must be set such that yaml.h can'
	@echo 'be found as $$YAML/include/yaml.h.'
	@echo
	@echo 'If the YAML library (libyaml.so) cannot be found in the default'
	@echo 'library path for the build host, then $$YAML_LIBDIR must be set'
	@echo 'to the appropriate path, e.g. YAML_LIBDIR=/usr/lib64.'
	@echo
	@echo 'When cross-compiling and the target platform uses a different YAML'
	@echo 'library path than the build host does, then $$YAML_TARGET_LIBDIR'
	@echo 'must be set accordingly.'
	@echo
	exit 1
endif
endif

.PHONY: yaml_check
