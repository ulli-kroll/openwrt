ARCH:=mipsel
SUBTARGET:=sf19a2890
BOARDNAME:=Siflower SF19A2890 based boards
FEATURES+=fpu source-only
CPU_TYPE:=24kc
CPU_SUBTYPE:=24kf

KERNEL_TESTING_PATCHVER:=6.18

KERNELNAME:=vmlinux

define Target/Description
	Build firmware images for Siflower SF19A2890 based boards.
endef
