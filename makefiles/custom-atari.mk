###################################################################
# Atari
###################################################################
ifeq ($(DEBUG),true)
    $(info >>>Starting custom-atari.mk)
endif

################################################################
# COMPILE FLAGS
# reserved memory for graphics
# LDFLAGS += -Wl -D,__RESERVED_MEMORY__=0x2000

LDFLAGS += -C src/atari/atank.atari-xex.cfg --cpu 6502

################################################################
# DISK creation

SUFFIX = .xex
DISK_TASKS += .atr
DISK_UTIL = atr
DISKS_DIR = disks/
BASE_DISK = base.atr
PICOBOOT_DOWNLOAD_URL = https://github.com/FujiNetWIFI/assets/releases/download/picobin/picoboot.bin
DATA_DIR = data/maps
MAP_FILE = z2.atm

# atari cache dir
ATARI_CACHE_DIR := $(CACHE_DIR)/atari

ifeq ($(DISK_UTIL),dir2atr)

.atr:
	@which dir2atr > /dev/null 2>&1 ; \
	if [ $$? -ne 0 ] ; then \
		echo -e "\nERROR! You must compile and install dir2atr from https://github.com/HiassofT/AtariSIO to create atari disks\n" ; \
		exit 1 ; \
	fi
	$(call MKDIR,$(DIST_DIR)/atr)
	$(call MKDIR,$(CACHE_DIR))
	$(call MKDIR,$(ATARI_CACHE_DIR))
	cp $(DIST_DIR)/$(PROGRAM_TGT)$(SUFFIX) $(DIST_DIR)/atr/$(PROGRAM)$(SUFFIX)
	@if [ ! -f $(ATARI_CACHE_DIR)/picoboot.bin ] ; then \
		echo "Downloading picoboot.bin"; \
		curl -sL $(PICOBOOT_DOWNLOAD_URL) -o $(ATARI_CACHE_DIR)/picoboot.bin; \
	fi
	dir2atr -m -S -B $(ATARI_CACHE_DIR)/picoboot.bin $(DIST_DIR)/$(PROGRAM).atr $(DIST_DIR)/atr
	rm -rf $(DIST_DIR)/atr

else

.atr:
	$(call MKDIR,$(DIST_DIR)/atr)
	$(call MKDIR,$(CACHE_DIR))
	$(call MKDIR,$(ATARI_CACHE_DIR))
	cp $(DIST_DIR)/$(PROGRAM_TGT)$(SUFFIX) $(DIST_DIR)/atr/$(PROGRAM)$(SUFFIX)
	cp $(DATA_DIR)/$(MAP_FILE) $(DIST_DIR)/atr/$(MAP_FILE)
	cp $(DISKS_DIR)/$(BASE_DISK) $(DIST_DIR)/$(PROGRAM).atr
	@if [ ! -f $(ATARI_CACHE_DIR)/picoboot.bin ] ; then \
		echo "Downloading picoboot.bin"; \
		curl -sL $(PICOBOOT_DOWNLOAD_URL) -o $(ATARI_CACHE_DIR)/picoboot.bin; \
	fi
	atr $(DIST_DIR)/$(PROGRAM).atr put $(DIST_DIR)/atr/$(PROGRAM)$(SUFFIX) autorun.sys
	atr $(DIST_DIR)/$(PROGRAM).atr put $(DATA_DIR)/$(MAP_FILE) $(MAP_FILE)
	rm -rf $(DIST_DIR)/atr	

endif

################################################################
# TESTING / EMULATOR

# Specify ATARI_EMULATOR=[ALTIRRA|ATARI800] to set which one to run, default is ALTIRRA

ALTIRRA ?= altirra \
   $(XS)/debugcmd: ".loadsym build\$(PROGRAM).$(CURRENT_TARGET).lbl"
#   $(XS)/debugcmd: "bp _read_or_create_appkey" \
#ALTIRRA ?= $(ALTIRRA_HOME)/Altirra64.exe \
#  $(XS)/portable $(XS)/portablealt:altirra-debug.ini \

# Additional args that can be copied into the above lines
#   $(XS)/debug \
#   $(XS)/debugcmd: ".loadsym build\$(PROGRAM).$(CURRENT_TARGET).lbl" \
#   $(XS)/debugcmd: "bp _debug" \

ATARI800 ?= $(ATARI800_HOME)/atari800 \
  -xl -nobasic -ntsc -xl-rev custom -config atari800-debug.cfg -run

atari_EMUCMD := $($(ATARI_EMULATOR))

ifeq ($(ATARI_EMULATOR),)
atari_EMUCMD := $(ALTIRRA)
endif
