LOCAL_DIR := $(GET_LOCAL_DIR)

INCLUDES += \
			-I$(LOCAL_DIR)/include -I$(LK_TOP_DIR)/dev/panel/msm

DEFINES += $(TARGET_XRES)
DEFINES += $(TARGET_YRES)

OBJS += \
	$(LOCAL_DIR)/debug.o \
	$(LOCAL_DIR)/smem.o \
	$(LOCAL_DIR)/smem_ptable.o \
	$(LOCAL_DIR)/jtag_hook.o \
	$(LOCAL_DIR)/jtag.o \
	$(LOCAL_DIR)/partition_parser.o \
	$(LOCAL_DIR)/hsusb.o \
	$(LOCAL_DIR)/boot_stats.o \
	$(LOCAL_DIR)/crc32.o

ifeq ($(ENABLE_SDHCI_SUPPORT),1)
OBJS += \
	$(LOCAL_DIR)/sdhci.o \
	$(LOCAL_DIR)/sdhci_msm.o \
	$(LOCAL_DIR)/mmc_sdhci.o \
	$(LOCAL_DIR)/mmc_wrapper.o
else
OBJS += \
	$(LOCAL_DIR)/mmc.o
endif

ifeq ($(PLATFORM),msm8916)
DEFINES += DISPLAY_TYPE_MDSS=1
	OBJS += $(LOCAL_DIR)/qgic.o \
		$(LOCAL_DIR)/qtimer.o \
		$(LOCAL_DIR)/qtimer_mmap.o \
		$(LOCAL_DIR)/interrupts.o \
		$(LOCAL_DIR)/clock.o \
		$(LOCAL_DIR)/clock_pll.o \
		$(LOCAL_DIR)/clock_lib2.o \
		$(LOCAL_DIR)/uart_dm.o \
		$(LOCAL_DIR)/board.o \
		$(LOCAL_DIR)/spmi.o \
		$(LOCAL_DIR)/scm.o \
		$(LOCAL_DIR)/bam.o \
		$(LOCAL_DIR)/qpic_nand.o \
		$(LOCAL_DIR)/dload_util.o \
		$(LOCAL_DIR)/gpio.o \
		$(LOCAL_DIR)/dev_tree.o \
		$(LOCAL_DIR)/mdp5.o \
		$(LOCAL_DIR)/display.o \
		$(LOCAL_DIR)/mipi_dsi.o \
		$(LOCAL_DIR)/mipi_dsi_phy.o \
		$(LOCAL_DIR)/mipi_dsi_autopll.o \
		$(LOCAL_DIR)/shutdown_detect.o \
		$(LOCAL_DIR)/certificate.o \
		$(LOCAL_DIR)/image_verify.o \
		$(LOCAL_DIR)/crypto_hash.o \
		$(LOCAL_DIR)/crypto5_eng.o \
		$(LOCAL_DIR)/crypto5_wrapper.o \
		$(LOCAL_DIR)/i2c_qup.o

endif

ifeq ($(ENABLE_USB30_SUPPORT),1)
	OBJS += \
		$(LOCAL_DIR)/usb30_dwc.o \
		$(LOCAL_DIR)/usb30_dwc_hw.o \
		$(LOCAL_DIR)/usb30_udc.o \
		$(LOCAL_DIR)/usb30_wrapper.o
endif
