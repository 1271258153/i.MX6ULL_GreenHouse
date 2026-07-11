ifeq ($(CONFIG_OF),y)

# Trimmed for this tree: only ship DTS sources that exist locally.
dtb-$(CONFIG_SOC_IMX6ULL) += \
	imx6ull-14x14-evk.dtb \
	imx6ull-14x14-evk-emmc.dtb 
endif

always		:= $(dtb-y)
clean-files	:= *.dtb
