CURRENT_DIR := $(dir $(abspath $(lastword ${MAKEFILE_LIST})))

SerialServer_CFILES := $(wildcard $(CURRENT_DIR)/src/*.c)
SerialServer_HFILES := $(wildcard $(CURRENT_DIR)/configurations/*.h)

