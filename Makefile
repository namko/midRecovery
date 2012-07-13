# Path for ARM C++ cross compiler.
ARM-CXX := arm-none-linux-gnueabi-g++

# Path to the kernel used for building recovery.
KERNELDIR := /mnt/android/kernel

# Recovery binary version
RECOVERY_VERSION := "\"v0.92 (Beta)\""

# Files
DIRCHECK := .dircheck
BINARY := midRecovery
CPIO := recovery.cpio
CPIO_FILES := files.txt
RECOVERY := recovery

# Directories
SRCDIR := src
OBJDIR := out
RECOVERYDIR := ramdisk
CONFIGDIR := config
SRCDIRS := $(SRCDIR) $(SRCDIR)/hw $(SRCDIR)/ui

SRCS := $(shell find $(SRCDIRS) -maxdepth 1 -name '*.cpp')
OBJS := $(patsubst $(SRCDIR)%.cpp, $(OBJDIR)%.o, $(SRCS))
OBJDIRS := $(subst $(SRCDIR), $(OBJDIR), $(SRCDIRS))

# This Makefile requires GNU Make to set variables for targets.
# For other versions of make, set the TARGET and CXX
# variables manually.
-include targets.mk

ifeq ($(filter clean recovery_clean,$(MAKECMDGOALS)),)
    -include $(OBJS:.o=.d)
endif

$(OBJDIR)/$(RECOVERY): $(OBJDIR)/$(CPIO)
	mv $(OBJDIR)/$(CPIO) $(KERNELDIR)/$(CPIO)
	cp $(CONFIGDIR)/$(TARGET).config $(KERNELDIR)/.config
	@+make -C $(KERNELDIR)
	cp $(KERNELDIR)/arch/arm/boot/zImage $(OBJDIR)/$(RECOVERY)

$(OBJDIR)/$(CPIO): $(OBJDIR)/$(BINARY)
	cp $(OBJDIR)/$(BINARY) $(RECOVERYDIR)/sbin
	cd $(RECOVERYDIR); \
	 cpio -o -H newc < $(CPIO_FILES) > ../$(OBJDIR)/$(CPIO)

$(OBJDIR)/$(BINARY): $(OBJS)
	$(CXX) $^ -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(OBJDIR)/$(DIRCHECK)
	$(CXX) -c $< -o $@ -DTARGET=$(TARGET) -DRECOVERY_VERSION=$(RECOVERY_VERSION)

$(OBJDIR)/%.d: $(SRCDIR)/%.cpp $(OBJDIR)/$(DIRCHECK)
	$(SHELL) -ec "printf '$(dir $@)' > $@; $(CXX) -MM $< | sed 's|\($$*\)\.o[ :]*|\1.o $@: |g' >> $@;"

$(OBJDIR)/$(DIRCHECK):
	mkdir -p $(OBJDIRS)
	echo > $(OBJDIR)/$(DIRCHECK)

clean:
	rm -f $(RECOVERYDIR)/sbin/$(BINARY)
	rm -rf $(OBJDIR)/$(DIRCHECK)
	rm -rf $(OBJDIR)/*

recovery_clean:
	rm -f $(KERNELDIR)/$(CPIO)
	@+make -C $(KERNELDIR) clean

