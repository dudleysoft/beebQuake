#
# Quake Makefile for Linux 2.0
#
# Aug '98 by Zoid <zoid@idsoftware.com>
#
# ELF only
#

BUILDDIR = .
MOUNT_DIR = .

CC = arm-none-eabi-gcc
LD = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
RM = rm

#CFLAGS := -DVFP -nostartfiles -O2 -mfloat-abi=hard -mfpu=vfp -march=armv6zk -mtune=arm1176jzf-s -fno-delete-null-pointer-checks -fdata-sections -ffunction-sections --specs=nano.specs --specs=nosys.specs -u _printf_float -fno-short-enums
CFLAGS := -DVIDULA -nostartfiles -O2 -mcpu=arm7tdmi -mtune=arm7tdmi -fno-delete-null-pointer-checks -fdata-sections -ffunction-sections --specs=nano.specs --specs=nosys.specs -u _printf_float -fno-short-enums
LDFLAGS := -Wl,--gc-sections -Wl,--no-print-gc-sections -Wl,-T,rpi.X -Wl,-lm -Wl,-no-enum-size-warning

DO_CC=$(CC) $(CFLAGS) -o $@ -c $<
DO_DEBUG_CC=$(CC) $(CFLAGS) -o $@ -c $<

#############################################################################
# SETUP AND BUILD
#############################################################################

TARGETS = bquake

build_release:
	mkdir -p $(BUILDDIR)\quake
	$(MAKE) targets

all: build_release

targets: $(TARGETS)

#############################################################################
# Beeb Quake
#############################################################################

QUAKE_OBJS = \
	$(BUILDDIR)/quake/cl_demo.o \
	$(BUILDDIR)/quake/cl_input.o \
	$(BUILDDIR)/quake/cl_main.o \
	$(BUILDDIR)/quake/cl_parse.o \
	$(BUILDDIR)/quake/cl_tent.o \
	$(BUILDDIR)/quake/chase.o \
	$(BUILDDIR)/quake/cmd.o \
	$(BUILDDIR)/quake/common.o \
	$(BUILDDIR)/quake/console.o \
	$(BUILDDIR)/quake/crc.o \
	$(BUILDDIR)/quake/cvar.o \
	$(BUILDDIR)/quake/draw.o \
	$(BUILDDIR)/quake/d_edge.o \
	$(BUILDDIR)/quake/d_fill.o \
	$(BUILDDIR)/quake/d_init.o \
	$(BUILDDIR)/quake/d_modech.o \
	$(BUILDDIR)/quake/d_part.o \
	$(BUILDDIR)/quake/d_polyse.o \
	$(BUILDDIR)/quake/d_scan.o \
	$(BUILDDIR)/quake/d_sky.o \
	$(BUILDDIR)/quake/d_sprite.o \
	$(BUILDDIR)/quake/d_surf.o \
	$(BUILDDIR)/quake/d_vars.o \
	$(BUILDDIR)/quake/d_zpoint.o \
	$(BUILDDIR)/quake/host.o \
	$(BUILDDIR)/quake/host_cmd.o \
	$(BUILDDIR)/quake/keys.o \
	$(BUILDDIR)/quake/menu.o \
	$(BUILDDIR)/quake/mathlib.o \
	$(BUILDDIR)/quake/model.o \
	$(BUILDDIR)/quake/net_loop.o \
	$(BUILDDIR)/quake/net_main.o \
	$(BUILDDIR)/quake/net_vcr.o \
	$(BUILDDIR)/quake/net_none.o \
	$(BUILDDIR)/quake/nonintel.o \
	$(BUILDDIR)/quake/pr_cmds.o \
	$(BUILDDIR)/quake/pr_edict.o \
	$(BUILDDIR)/quake/pr_exec.o \
	$(BUILDDIR)/quake/r_aclip.o \
	$(BUILDDIR)/quake/r_alias.o \
	$(BUILDDIR)/quake/r_bsp.o \
	$(BUILDDIR)/quake/r_light.o \
	$(BUILDDIR)/quake/r_draw.o \
	$(BUILDDIR)/quake/r_efrag.o \
	$(BUILDDIR)/quake/r_edge.o \
	$(BUILDDIR)/quake/r_misc.o \
	$(BUILDDIR)/quake/r_main.o \
	$(BUILDDIR)/quake/r_sky.o \
	$(BUILDDIR)/quake/r_sprite.o \
	$(BUILDDIR)/quake/r_surf.o \
	$(BUILDDIR)/quake/r_part.o \
	$(BUILDDIR)/quake/r_vars.o \
	$(BUILDDIR)/quake/screen.o \
	$(BUILDDIR)/quake/sbar.o \
	$(BUILDDIR)/quake/sv_main.o \
	$(BUILDDIR)/quake/sv_phys.o \
	$(BUILDDIR)/quake/sv_move.o \
	$(BUILDDIR)/quake/sv_user.o \
	$(BUILDDIR)/quake/zone.o	\
	$(BUILDDIR)/quake/view.o	\
	$(BUILDDIR)/quake/wad.o \
	$(BUILDDIR)/quake/world.o \
	$(BUILDDIR)/quake/cd_null.o \
	$(BUILDDIR)/quake/sys_beeb.o \
	$(BUILDDIR)/quake/vid_beeb.o \
	$(BUILDDIR)/quake/snd_mem.o \
	$(BUILDDIR)/quake/snd_mix.o \
	$(BUILDDIR)/quake/snd_null.o \
	$(BUILDDIR)/quake/in_beeb.o

BEEB_OBJS =  armc-start.o armtubeio.o armtubeswis.o beebScreen/beebScreen.o audio.o

bquake : $(QUAKE_OBJS) $(BEEB_OBJS)
	$(CC) $(CFLAGS) -o temp $(QUAKE_OBJS) $(BEEB_OBJS) $(LDFLAGS)
	$(OBJCOPY) -O binary temp $@
	$(RM) temp

####

%.o:	%.c
	$(CC) $(CFLAGS) -c $< -o $@

beebScreen/beebScreen.o: beebScreen/beebScreen.c beebScreen/beebCode.c beebScreen/beebScreen.h
	$(CC) $(CFLAGS) -c beebScreen/beebScreen.c -o beebScreen/beebScreen.o

beebScreen/beebCode.c: beebScreen/beebCode.bin
	cd beebScreen; ./mkasm.bat

beebScreen/bagiCode.c: beebScreen/beebCode.asm beebScreen/bagiCode.bin
	cd beebScreen; ./mkasm.bat

beebScreen/beebCode.bin: beebScreen/beebCode.asm

beebScreen/bagiCode.bin: beebScreen/beebCode.asm

%.o: %.s
	$(CC) $(CFLAGS) -c $< -o $@


$(BUILDDIR)/quake/cl_demo.o :  $(MOUNT_DIR)/cl_demo.c
	$(DO_CC)

$(BUILDDIR)/quake/cl_input.o : $(MOUNT_DIR)/cl_input.c
	$(DO_CC)

$(BUILDDIR)/quake/cl_main.o :  $(MOUNT_DIR)/cl_main.c
	$(DO_CC)

$(BUILDDIR)/quake/cl_parse.o : $(MOUNT_DIR)/cl_parse.c
	$(DO_CC)

$(BUILDDIR)/quake/cl_tent.o :  $(MOUNT_DIR)/cl_tent.c
	$(DO_CC)

$(BUILDDIR)/quake/chase.o :    $(MOUNT_DIR)/chase.c
	$(DO_CC)

$(BUILDDIR)/quake/cmd.o :      $(MOUNT_DIR)/cmd.c
	$(DO_CC)

$(BUILDDIR)/quake/common.o :   $(MOUNT_DIR)/common.c
	$(DO_DEBUG_CC)

$(BUILDDIR)/quake/console.o :  $(MOUNT_DIR)/console.c
	$(DO_CC)

$(BUILDDIR)/quake/crc.o :      $(MOUNT_DIR)/crc.c
	$(DO_CC)

$(BUILDDIR)/quake/cvar.o :     $(MOUNT_DIR)/cvar.c
	$(DO_CC)

$(BUILDDIR)/quake/d_edge.o :   $(MOUNT_DIR)/d_edge.c
	$(DO_CC)

$(BUILDDIR)/quake/d_fill.o :   $(MOUNT_DIR)/d_fill.c
	$(DO_CC)

$(BUILDDIR)/quake/d_init.o :   $(MOUNT_DIR)/d_init.c
	$(DO_CC)

$(BUILDDIR)/quake/d_modech.o : $(MOUNT_DIR)/d_modech.c
	$(DO_CC)

$(BUILDDIR)/quake/d_scan.o :   $(MOUNT_DIR)/d_scan.c
	$(DO_CC)

$(BUILDDIR)/quake/d_sky.o :    $(MOUNT_DIR)/d_sky.c
	$(DO_CC)

$(BUILDDIR)/quake/d_sprite.o : $(MOUNT_DIR)/d_sprite.c
	$(DO_CC)

$(BUILDDIR)/quake/d_surf.o :   $(MOUNT_DIR)/d_surf.c
	$(DO_CC)

$(BUILDDIR)/quake/d_vars.o :   $(MOUNT_DIR)/d_vars.c
	$(DO_CC)

$(BUILDDIR)/quake/d_zpoint.o : $(MOUNT_DIR)/d_zpoint.c
	$(DO_CC)

$(BUILDDIR)/quake/host.o :     $(MOUNT_DIR)/host.c
	$(DO_CC)

$(BUILDDIR)/quake/host_cmd.o : $(MOUNT_DIR)/host_cmd.c
	$(DO_CC)

$(BUILDDIR)/quake/keys.o :     $(MOUNT_DIR)/keys.c
	$(DO_CC)

$(BUILDDIR)/quake/menu.o :     $(MOUNT_DIR)/menu.c
	$(DO_CC)

$(BUILDDIR)/quake/mathlib.o :  $(MOUNT_DIR)/mathlib.c
	$(DO_CC)

$(BUILDDIR)/quake/model.o :    $(MOUNT_DIR)/model.c
	$(DO_CC)

$(BUILDDIR)/quake/net_dgrm.o : $(MOUNT_DIR)/net_dgrm.c
	$(DO_CC)

$(BUILDDIR)/quake/net_loop.o : $(MOUNT_DIR)/net_loop.c
	$(DO_CC)

$(BUILDDIR)/quake/net_main.o : $(MOUNT_DIR)/net_main.c
	$(DO_CC)

$(BUILDDIR)/quake/net_vcr.o :  $(MOUNT_DIR)/net_vcr.c
	$(DO_CC)

$(BUILDDIR)/quake/net_udp.o :  $(MOUNT_DIR)/net_udp.c
	$(DO_CC)

$(BUILDDIR)/quake/net_bsd.o :  $(MOUNT_DIR)/net_bsd.c
	$(DO_CC)

$(BUILDDIR)/quake/net_none.o :  $(MOUNT_DIR)/net_none.c
	$(DO_CC)

$(BUILDDIR)/quake/nonintel.o : $(MOUNT_DIR)/nonintel.c
	$(DO_CC)

$(BUILDDIR)/quake/pr_cmds.o :  $(MOUNT_DIR)/pr_cmds.c
	$(DO_CC)

$(BUILDDIR)/quake/pr_edict.o : $(MOUNT_DIR)/pr_edict.c
	$(DO_CC)

$(BUILDDIR)/quake/pr_exec.o :  $(MOUNT_DIR)/pr_exec.c
	$(DO_CC)

$(BUILDDIR)/quake/r_aclip.o :  $(MOUNT_DIR)/r_aclip.c
	$(DO_CC)

$(BUILDDIR)/quake/r_bsp.o :    $(MOUNT_DIR)/r_bsp.c
	$(DO_CC)

$(BUILDDIR)/quake/r_light.o :  $(MOUNT_DIR)/r_light.c
	$(DO_CC)

$(BUILDDIR)/quake/r_efrag.o :  $(MOUNT_DIR)/r_efrag.c
	$(DO_CC)

$(BUILDDIR)/quake/r_misc.o :   $(MOUNT_DIR)/r_misc.c
	$(DO_CC)

$(BUILDDIR)/quake/r_main.o :   $(MOUNT_DIR)/r_main.c
	$(DO_CC)

$(BUILDDIR)/quake/r_sky.o :    $(MOUNT_DIR)/r_sky.c
	$(DO_CC)

$(BUILDDIR)/quake/r_sprite.o : $(MOUNT_DIR)/r_sprite.c
	$(DO_CC)

$(BUILDDIR)/quake/r_surf.o :   $(MOUNT_DIR)/r_surf.c
	$(DO_CC)

$(BUILDDIR)/quake/r_part.o :   $(MOUNT_DIR)/r_part.c
	$(DO_CC)

$(BUILDDIR)/quake/screen.o :   $(MOUNT_DIR)/screen.c
	$(DO_CC)

$(BUILDDIR)/quake/sbar.o :     $(MOUNT_DIR)/sbar.c
	$(DO_CC)

$(BUILDDIR)/quake/sv_main.o :  $(MOUNT_DIR)/sv_main.c
	$(DO_CC)

$(BUILDDIR)/quake/sv_phys.o :  $(MOUNT_DIR)/sv_phys.c
	$(DO_CC)

$(BUILDDIR)/quake/sv_move.o :  $(MOUNT_DIR)/sv_move.c
	$(DO_CC)

$(BUILDDIR)/quake/sv_user.o :  $(MOUNT_DIR)/sv_user.c
	$(DO_CC)

$(BUILDDIR)/quake/zone.o	:   $(MOUNT_DIR)/zone.c
	$(DO_CC)

$(BUILDDIR)/quake/view.o	:   $(MOUNT_DIR)/view.c
	$(DO_CC)

$(BUILDDIR)/quake/wad.o :      $(MOUNT_DIR)/wad.c
	$(DO_CC)

$(BUILDDIR)/quake/world.o :    $(MOUNT_DIR)/world.c
	$(DO_CC)

$(BUILDDIR)/quake/cd_linux.o : $(MOUNT_DIR)/cd_linux.c
	$(DO_CC)

$(BUILDDIR)/quake/cd_null.o : $(MOUNT_DIR)/cd_null.c
	$(DO_CC)

$(BUILDDIR)/quake/in_null.o : $(MOUNT_DIR)/in_null.c
	$(DO_CC)

$(BUILDDIR)/quake/sys_linux.o :$(MOUNT_DIR)/sys_linux.c
	$(DO_CC)

$(BUILDDIR)/quake/sys_beeb.o :$(MOUNT_DIR)/sys_beeb.c
	$(DO_CC)

$(BUILDDIR)/quake/vid_svgalib.o:$(MOUNT_DIR)/vid_svgalib.c
	$(DO_O_CC)

$(BUILDDIR)/quake/snd_dma.o :  $(MOUNT_DIR)/snd_dma.c
	$(DO_CC)

$(BUILDDIR)/quake/snd_mem.o :  $(MOUNT_DIR)/snd_mem.c
	$(DO_CC)

$(BUILDDIR)/quake/snd_mix.o :  $(MOUNT_DIR)/snd_mix.c
	$(DO_CC)

$(BUILDDIR)/quake/snd_linux.o :$(MOUNT_DIR)/snd_linux.c
	$(DO_CC)

$(BUILDDIR)/quake/snd_null.o :$(MOUNT_DIR)/snd_null.c
	$(DO_CC)

$(BUILDDIR)/quake/draw.o :$(MOUNT_DIR)/draw.c
	$(DO_CC)

$(BUILDDIR)/quake/vid_beeb.o :$(MOUNT_DIR)/vid_beeb.c
	$(DO_CC)

$(BUILDDIR)/quake/in_beeb.o :$(MOUNT_DIR)/in_beeb.c
	$(DO_CC)

$(BUILDDIR)/quake/d_part.o :$(MOUNT_DIR)/d_part.c
	$(DO_CC)

$(BUILDDIR)/quake/d_polyse.o :$(MOUNT_DIR)/d_polyse.c
	$(DO_CC)

$(BUILDDIR)/quake/r_alias.o :$(MOUNT_DIR)/r_alias.c
	$(DO_CC)

$(BUILDDIR)/quake/r_draw.o :$(MOUNT_DIR)/r_draw.c
	$(DO_CC)

$(BUILDDIR)/quake/r_edge.o :$(MOUNT_DIR)/r_edge.c
	$(DO_CC)

$(BUILDDIR)/quake/r_vars.o :$(MOUNT_DIR)/r_vars.c
	$(DO_CC)


#############################################################################
# MISC
#############################################################################

clean: clean-debug clean-release
	rm -f quake.spec glquake.spec quake.x11.spec

clean-debug:
	$(MAKE) clean2 BUILDDIR=$(BUILD_DEBUG_DIR) CFLAGS="$(DEBUG_CFLAGS)"

clean-release:
	$(MAKE) clean2 BUILDDIR=$(BUILD_RELEASE_DIR) CFLAGS="$(DEBUG_CFLAGS)"

clean2:
	-rm -f $(quake_OBJS) $(X11_OBJS) $(GLQUAKE_OBJS) $(GLSVGA_OBJS) \
		$(GLX_OBJS)

