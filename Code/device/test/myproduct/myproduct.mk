# This is a build configuration for a full-featured build of the
# Open-Source part of the tree. It's geared toward a US-centric
# build quite specifically for the emulator, and might not be
# entirely appropriate to inherit from for on-device configurations.


$(call inherit-product, $(SRC_TARGET_DIR)/product/aosp_base_telephony.mk)
$(call inherit-product, $(SRC_TARGET_DIR)/board/generic/device.mk)

include $(SRC_TARGET_DIR)/product/emulator.mk

# Overrides
PRODUCT_NAME := myproduct
PRODUCT_DEVICE := phdemo
PRODUCT_BRAND := Android_phdemo
PRODUCT_MODEL := phdemo AOSP on ARM Emulator
