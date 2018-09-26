
#
# Config
#
# target platform: MAKE_TARGET_ARCH=X86,BEAGLEBONE,RPI
MAKE_TARGET_ARCH?=RPI
USE_NETWORK=1
USE_REALCONS=1

# other global cc switches

# lib extension is ".so" on all linuxes
CC_DEFS = -DHAVE_DLOPEN=so

ifeq ($(MAKE_CONFIGURATION),RELEASE)
CC_DBG_FLAGS = -O2 -Wno-unused-result
else
CC_DBG_FLAGS = -ggdb3 -O0
endif

# REALCONS: default for all targets is -UUSE_REALCONS, REALCONS_OPT override for PDP11
ifeq ($(USE_REALCONS),1)
BLINKENLIGHT_COMMON_DIR=$(SRC)/../../../00_common/
BLINKENLIGHT_API_DIR=$(SRC)/../../../07.0_blinkenlight_api/
REALCONS_DIR=$(SRC)/REALCONS/
# generic sources
REALCONS= \
        $(REALCONS_DIR)realcons.c     \
        $(REALCONS_DIR)realcons_simh.c \
        $(BLINKENLIGHT_API_DIR)blinkenlight_api_client.c \
        $(BLINKENLIGHT_API_DIR)/rpcgen_linux/rpc_blinkenlight_api_clnt.c \
        $(BLINKENLIGHT_API_DIR)/rpcgen_linux/rpc_blinkenlight_api_xdr.c \
        $(BLINKENLIGHT_API_DIR)blinkenlight_panels.c \
        $(BLINKENLIGHT_COMMON_DIR)bitcalc.c
# PDP11 part
REALCONS_PDP11= \
	    $(REALCONS_DIR)realcons_console_pdp11_20.c \
	    $(REALCONS_DIR)realcons_console_pdp11_40.c \
	    $(REALCONS_DIR)realcons_console_pdp11_70.c

REALCONS_PDP10= \
        $(REALCONS_DIR)realcons_pdp10_control.c \
        $(REALCONS_DIR)realcons_pdp10.c \
        $(REALCONS_DIR)realcons_pdp10_operpanel.c \
        $(REALCONS_DIR)realcons_pdp10_maintpanel.c

REALCONS_PDP8= \
        $(REALCONS_DIR)realcons_console_pdp8i.c

REALCONS_PDP15= \
        $(REALCONS_DIR)realcons_console_pdp15.c

REALCONS_OPT=-DUSE_REALCONS \
	-DBLINKENLIGHT_CLIENT	\
	-I$(SRC)/REALCONS \
	-I$(BLINKENLIGHT_COMMON_DIR) \
	-I$(BLINKENLIGHT_API_DIR)/rpcgen_linux \
	-I$(BLINKENLIGHT_API_DIR)
#	$(REALCONS)



else
REALCONS_OPT=
REALCONS=
REALCONS_PDP11=
REALCONS_PDP10=
REALCONS_PDP8=
REALCONS_PDP15=
endif


# target platform is controlled by MAKE_TARGET_ARCH
#	X86 -> Linux,
#	BEAGLEBONE
#	RPI
#
# CC Command
#

# ifeq OR: http://osdir.com/ml/gnu.make.windows/2004-03/msg00063.html
ifneq (,$(filter $(MAKE_TARGET_ARCH),X86 X64))
#ifeq ($(MAKE_TARGET_ARCH),X86)
  # Unix Environments
  ifneq (,$(findstring solaris,$(OSTYPE)))
    OS_CCDEFS = -lm -lsocket -lnsl -lrt -lpthread -D_GNU_SOURCE
  else
    ifneq (,$(findstring darwin,$(OSTYPE)))
      OS_CCDEFS = -D_GNU_SOURCE
    else
      OS_CCDEFS = -D_GNU_SOURCE
      LDFLAGS= -lrt -lm -lpthread -ldl
    endif
  endif
  ifeq ($(MAKE_TARGET_ARCH),X86)
	BIN=$(SRC)/../bin-ubuntu-x86/
	OS_CCDEFS += -m32
  else
	BIN=$(SRC)/../bin-ubuntu-x64/
	OS_CCDEFS += -m64
  endif
  CC = gcc -std=c99 -U__STRICT_ANSI__  $(CC_DEFS) $(OS_CCDEFS)\
	-I $(SRC) $(CC_DBG_FLAGS) -UUSE_REALCONS

  ifeq ($(USE_NETWORK),)
  else
    # Ubuntu
    NETWORK_OPT = -DUSE_NETWORK -isystem /usr/local/include# -lpcap
    # !!! install package "libcap-dev"
#    NETWORK_OPT = -DUSE_NETWORK -isystem /usr/include -lpcap
#    NETWORK_OPT = -DUSE_NETWORK -isystem /usr/local/include /usr/local/lib/libpcap.a
  endif
endif

ifeq ($(MAKE_TARGET_ARCH),BBW)
  # fuer beaglebone und sourcery codebench lite
  ### override all compiler settings for ARM cross compilation
  OS_CCDEFS = -D_GNU_SOURCE
  LDFLAGS = -lrt -lm  -lpthread  -ldl
  CC = $(BBW_CC) -std=c99 -U__STRICT_ANSI__  $(CC_DEFS) $(OS_CCDEFS) -I $(SRC)  $(CC_DBG_FLAGS) -UUSE_REALCONS
  NETWORK_OPT = -DUSE_NETWORK -isystem $(SYSROOTS)/usr/local/include -lpcap
  # DO NOT FORGET: "opkg install libpcap" on target
  BIN=$(SRC)/../bin-bbw/
endif

ifeq ($(MAKE_TARGET_ARCH),BBB)
  # fuer beagleboneblack under Debian and linaro cross gcc
  ### override all compiler settings for ARM cross compilation
  OS_CCDEFS = -D_GNU_SOURCE
  LDFLAGS = -lrt -lm  -lpthread  -ldl
  CC = $(BBB_CC) -std=c99 -U__STRICT_ANSI__  $(CC_DEFS) $(OS_CCDEFS) -I $(SRC)  $(CC_DBG_FLAGS) -UUSE_REALCONS
  # DO NOT FORGET: "apt-get install libpcap-dev" on target, 
  # copy /usr/lib/arm-linux-gnueabihf/libpcap.a to beagleboneblack-build
  LIBPCAP=$(SRC)/../bbb-build/libpcap.a
  NETWORK_OPT = -DUSE_NETWORK -isystem $(SYSROOTS)/usr/local/include $(LIBPCAP)
  BIN=$(SRC)/../bin-bbb/
endif

ifeq ($(MAKE_TARGET_ARCH),RPI)
  # fuer beaglebone und sourcery codebench lite
  ### override all compiler settings for ARM cross compilation
  OS_CCDEFS = -D_GNU_SOURCE
  LDFLAGS = -lrt -lm  -lpthread -ldl
  CC = arm-linux-gnueabihf-gcc -std=c99 -U__STRICT_ANSI__  $(CC_DEFS) $(OS_CCDEFS) \
	-I $(SRC) $(CC_DBG_FLAGS) -UUSE_REALCONS
  LIBPCAP=$(SRC)/../rpi-build/libpcap.a
  NETWORK_OPT = -DUSE_NETWORK -isystem $(SYSROOTS)/usr/local/include $(LIBPCAP)
  # NETWORK_OPT = -DUSE_NETWORK -isystem $(SYSROOTS)/usr/local/include -lpcap
  # DO NOT FORGET: "opkg install libpcap" on target
  BIN=$(SRC)/../bin-rpi/
endif

ifneq ($(WIN32),)
  #Win32 Environments
  LDFLAGS = -lm -lwsock32 -lwinmm  -ldl
  CC = gcc -std=c99 -U__STRICT_ANSI__  $(CC_DEFS) -I $(SRC)
  EXE = .exe
  ifeq ($(USE_NETWORK),)
  else
    NETWORK_OPT = -DUSE_NETWORK -lwpcap -lpacket
  endif
  BIN=$(SRC)/../bin-win32/
endif


# MKDIRBIN cmd used by SimH 4.x rules
#ifneq (binexists,$(shell if $(TEST) -e $(BIN); then echo binexists; fi))
    MKDIRBIN = mkdir -p $(BIN)
#endif


CC_OUTSPEC = -o $@

#
# Common Libraries
#
SRC=./

#BIN = BIN/
SIM = scp.c sim_console.c sim_fio.c sim_timer.c sim_sock.c \
	sim_tmxr.c sim_ether.c sim_tape.c sim_disk.c sim_serial.c \
	sim_video.c sim_imd.c


#
# Emulator source files and compile time options
#
PDP1D = PDP1
PDP1 = ${PDP1D}/pdp1_lp.c ${PDP1D}/pdp1_cpu.c ${PDP1D}/pdp1_stddev.c \
	${PDP1D}/pdp1_sys.c ${PDP1D}/pdp1_dt.c ${PDP1D}/pdp1_drm.c \
	${PDP1D}/pdp1_clk.c ${PDP1D}/pdp1_dcs.c ${PDP1D}/pdp1_dpy.c ${DISPLAYL}
PDP1_OPT = -I ${PDP1D} $(DISPLAY_OPT)


NOVAD = NOVA
NOVA = ${NOVAD}/nova_sys.c ${NOVAD}/nova_cpu.c ${NOVAD}/nova_dkp.c \
	${NOVAD}/nova_dsk.c ${NOVAD}/nova_lp.c ${NOVAD}/nova_mta.c \
	${NOVAD}/nova_plt.c ${NOVAD}/nova_pt.c ${NOVAD}/nova_clk.c \
	${NOVAD}/nova_tt.c ${NOVAD}/nova_tt1.c ${NOVAD}/nova_qty.c
NOVA_OPT = -I ${NOVAD}


ECLIPSE = ${NOVAD}/eclipse_cpu.c ${NOVAD}/eclipse_tt.c ${NOVAD}/nova_sys.c \
	${NOVAD}/nova_dkp.c ${NOVAD}/nova_dsk.c ${NOVAD}/nova_lp.c \
	${NOVAD}/nova_mta.c ${NOVAD}/nova_plt.c ${NOVAD}/nova_pt.c \
	${NOVAD}/nova_clk.c ${NOVAD}/nova_tt1.c ${NOVAD}/nova_qty.c
ECLIPSE_OPT = -I ${NOVAD} -DECLIPSE


PDP18BD = PDP18B
PDP18B = ${PDP18BD}/pdp18b_dt.c ${PDP18BD}/pdp18b_drm.c ${PDP18BD}/pdp18b_cpu.c \
	${PDP18BD}/pdp18b_lp.c ${PDP18BD}/pdp18b_mt.c ${PDP18BD}/pdp18b_rf.c \
	${PDP18BD}/pdp18b_rp.c ${PDP18BD}/pdp18b_stddev.c ${PDP18BD}/pdp18b_sys.c \
	${PDP18BD}/pdp18b_rb.c ${PDP18BD}/pdp18b_tt1.c ${PDP18BD}/pdp18b_fpp.c
PDP4_OPT = -DPDP4 -I ${PDP18BD}
PDP7_OPT = -DPDP7 -I ${PDP18BD}
PDP9_OPT = -DPDP9 -I ${PDP18BD}
PDP15_OPT = -DPDP15 -I ${PDP18BD} ${REALCONS_OPT}


PDP11D = PDP11
PDP11 = ${PDP11D}/pdp11_fp.c ${PDP11D}/pdp11_cpu.c ${PDP11D}/pdp11_dz.c \
	${PDP11D}/pdp11_cis.c ${PDP11D}/pdp11_lp.c ${PDP11D}/pdp11_rk.c \
	${PDP11D}/pdp11_rl.c ${PDP11D}/pdp11_rp.c ${PDP11D}/pdp11_rx.c \
	${PDP11D}/pdp11_stddev.c ${PDP11D}/pdp11_sys.c ${PDP11D}/pdp11_tc.c \
	${PDP11D}/pdp11_tm.c ${PDP11D}/pdp11_ts.c ${PDP11D}/pdp11_io.c \
	${PDP11D}/pdp11_rq.c ${PDP11D}/pdp11_tq.c ${PDP11D}/pdp11_pclk.c \
	${PDP11D}/pdp11_ry.c ${PDP11D}/pdp11_pt.c ${PDP11D}/pdp11_hk.c \
	${PDP11D}/pdp11_xq.c ${PDP11D}/pdp11_xu.c ${PDP11D}/pdp11_vh.c \
	${PDP11D}/pdp11_rh.c ${PDP11D}/pdp11_tu.c ${PDP11D}/pdp11_cpumod.c \
	${PDP11D}/pdp11_cr.c ${PDP11D}/pdp11_rf.c ${PDP11D}/pdp11_dl.c \
	${PDP11D}/pdp11_ta.c ${PDP11D}/pdp11_rc.c ${PDP11D}/pdp11_kg.c \
	${PDP11D}/pdp11_ke.c ${PDP11D}/pdp11_dc.c ${PDP11D}/pdp11_dmc.c \
	${PDP11D}/pdp11_kmc.c ${PDP11D}/pdp11_dup.c ${PDP11D}/pdp11_rs.c \
	${PDP11D}/pdp11_kwxx.c ${PDP11D}/pdp11_wwvb.c ${PDP11D}/pdp11_umc.c \
	${PDP11D}/pdp11_vt.c ${PDP11D}/pdp11_td.c ${PDP11D}/pdp11_io_lib.c $(DISPLAYL) $(DISPLAYVT)
PDP11_OPT = -DVM_PDP11 -I ${PDP11D} ${NETWORK_OPT} $(DISPLAY_OPT) ${REALCONS_OPT}


VAXD = VAX
VAX = ${VAXD}/vax_cpu.c ${VAXD}/vax_cpu1.c ${VAXD}/vax_fpa.c ${VAXD}/vax_io.c \
	${VAXD}/vax_cis.c ${VAXD}/vax_octa.c  ${VAXD}/vax_cmode.c \
	${VAXD}/vax_mmu.c ${VAXD}/vax_stddev.c ${VAXD}/vax_sysdev.c \
	${VAXD}/vax_sys.c  ${VAXD}/vax_syscm.c ${VAXD}/vax_syslist.c \
	${VAXD}/vax_vc.c ${VAXD}/vax_lk.c ${VAXD}/vax_vs.c ${VAXD}/vax_2681.c \
	${PDP11D}/pdp11_rl.c ${PDP11D}/pdp11_rq.c ${PDP11D}/pdp11_ts.c \
	${PDP11D}/pdp11_dz.c ${PDP11D}/pdp11_lp.c ${PDP11D}/pdp11_tq.c \
	${PDP11D}/pdp11_xq.c ${PDP11D}/pdp11_vh.c ${PDP11D}/pdp11_cr.c \
	${PDP11D}/pdp11_td.c ${PDP11D}/pdp11_io_lib.c
VAX_OPT = -DVM_VAX -DUSE_INT64 -DUSE_ADDR64 -DUSE_SIM_VIDEO -I ${VAXD} -I ${PDP11D} ${NETWORK_OPT} ${VIDEO_CCDEFS} ${VIDEO_LDFLAGS}


VAX610 = ${VAXD}/vax_cpu.c ${VAXD}/vax_cpu1.c ${VAXD}/vax_fpa.c \
	${VAXD}/vax_cis.c ${VAXD}/vax_octa.c ${VAXD}/vax_cmode.c \
	${VAXD}/vax_mmu.c ${VAXD}/vax_sys.c ${VAXD}/vax_syscm.c \
	${VAXD}/vax610_stddev.c ${VAXD}/vax610_sysdev.c ${VAXD}/vax610_io.c \
	${VAXD}/vax610_syslist.c ${VAXD}/vax610_mem.c ${VAXD}/vax_vc.c \
	${VAXD}/vax_lk.c ${VAXD}/vax_vs.c ${VAXD}/vax_2681.c \
	${PDP11D}/pdp11_rl.c ${PDP11D}/pdp11_rq.c ${PDP11D}/pdp11_ts.c \
	${PDP11D}/pdp11_dz.c ${PDP11D}/pdp11_lp.c ${PDP11D}/pdp11_tq.c \
	${PDP11D}/pdp11_xq.c ${PDP11D}/pdp11_vh.c ${PDP11D}/pdp11_cr.c \
	${PDP11D}/pdp11_td.c ${PDP11D}/pdp11_io_lib.c
VAX610_OPT = -DVM_VAX -DVAX_610 -DUSE_INT64 -DUSE_ADDR64 -DUSE_SIM_VIDEO -I ${VAXD} -I ${PDP11D} ${NETWORK_OPT} ${VIDEO_CCDEFS} ${VIDEO_LDFLAGS}

VAX630 = ${VAXD}/vax_cpu.c ${VAXD}/vax_cpu1.c ${VAXD}/vax_fpa.c \
	${VAXD}/vax_cis.c ${VAXD}/vax_octa.c ${VAXD}/vax_cmode.c \
	${VAXD}/vax_mmu.c ${VAXD}/vax_sys.c ${VAXD}/vax_syscm.c \
	${VAXD}/vax_watch.c ${VAXD}/vax630_stddev.c ${VAXD}/vax630_sysdev.c \
	${VAXD}/vax630_io.c ${VAXD}/vax630_syslist.c ${VAXD}/vax_vc.c \
	${VAXD}/vax_lk.c ${VAXD}/vax_vs.c ${VAXD}/vax_2681.c \
	${PDP11D}/pdp11_rl.c ${PDP11D}/pdp11_rq.c ${PDP11D}/pdp11_ts.c \
	${PDP11D}/pdp11_dz.c ${PDP11D}/pdp11_lp.c ${PDP11D}/pdp11_tq.c \
	${PDP11D}/pdp11_xq.c ${PDP11D}/pdp11_vh.c ${PDP11D}/pdp11_cr.c \
	${PDP11D}/pdp11_td.c ${PDP11D}/pdp11_io_lib.c
VAX620_OPT = -DVM_VAX -DVAX_620 -DUSE_INT64 -DUSE_ADDR64 -I ${VAXD} -I ${PDP11D} ${NETWORK_OPT}
VAX630_OPT = -DVM_VAX -DVAX_630 -DUSE_INT64 -DUSE_ADDR64 -DUSE_SIM_VIDEO -I ${VAXD} -I ${PDP11D} ${NETWORK_OPT} ${VIDEO_CCDEFS} ${VIDEO_LDFLAGS}


VAX730 = ${VAXD}/vax_cpu.c ${VAXD}/vax_cpu1.c ${VAXD}/vax_fpa.c \
	${VAXD}/vax_cis.c ${VAXD}/vax_octa.c  ${VAXD}/vax_cmode.c \
	${VAXD}/vax_mmu.c ${VAXD}/vax_sys.c  ${VAXD}/vax_syscm.c \
	${VAXD}/vax730_stddev.c ${VAXD}/vax730_sys.c \
	${VAXD}/vax730_mem.c ${VAXD}/vax730_uba.c ${VAXD}/vax730_rb.c \
	${VAXD}/vax730_syslist.c \
	${PDP11D}/pdp11_rl.c ${PDP11D}/pdp11_rq.c ${PDP11D}/pdp11_ts.c \
	${PDP11D}/pdp11_dz.c ${PDP11D}/pdp11_lp.c ${PDP11D}/pdp11_tq.c \
	${PDP11D}/pdp11_xu.c ${PDP11D}/pdp11_ry.c ${PDP11D}/pdp11_cr.c \
	${PDP11D}/pdp11_hk.c ${PDP11D}/pdp11_vh.c ${PDP11D}/pdp11_dmc.c \
	${PDP11D}/pdp11_td.c ${PDP11D}/pdp11_dup.c ${PDP11D}/pdp11_io_lib.c
VAX730_OPT = -DVM_VAX -DVAX_730 -DUSE_INT64 -DUSE_ADDR64 -I VAX -I ${PDP11D} ${NETWORK_OPT}


VAX750 = ${VAXD}/vax_cpu.c ${VAXD}/vax_cpu1.c ${VAXD}/vax_fpa.c \
	${VAXD}/vax_cis.c ${VAXD}/vax_octa.c  ${VAXD}/vax_cmode.c \
	${VAXD}/vax_mmu.c ${VAXD}/vax_sys.c  ${VAXD}/vax_syscm.c \
	${VAXD}/vax750_stddev.c ${VAXD}/vax750_cmi.c \
	${VAXD}/vax750_mem.c ${VAXD}/vax750_uba.c ${VAXD}/vax7x0_mba.c \
	${VAXD}/vax750_syslist.c \
	${PDP11D}/pdp11_rl.c ${PDP11D}/pdp11_rq.c ${PDP11D}/pdp11_ts.c \
	${PDP11D}/pdp11_dz.c ${PDP11D}/pdp11_lp.c ${PDP11D}/pdp11_tq.c \
	${PDP11D}/pdp11_xu.c ${PDP11D}/pdp11_ry.c ${PDP11D}/pdp11_cr.c \
	${PDP11D}/pdp11_hk.c ${PDP11D}/pdp11_rp.c ${PDP11D}/pdp11_tu.c \
	${PDP11D}/pdp11_vh.c ${PDP11D}/pdp11_dmc.c ${PDP11D}/pdp11_dup.c \
	${PDP11D}/pdp11_td.c ${PDP11D}/pdp11_io_lib.c
VAX750_OPT = -DVM_VAX -DVAX_750 -DUSE_INT64 -DUSE_ADDR64 -I VAX -I ${PDP11D} ${NETWORK_OPT}


VAX780 = ${VAXD}/vax_cpu.c ${VAXD}/vax_cpu1.c ${VAXD}/vax_fpa.c \
	${VAXD}/vax_cis.c ${VAXD}/vax_octa.c  ${VAXD}/vax_cmode.c \
	${VAXD}/vax_mmu.c ${VAXD}/vax_sys.c  ${VAXD}/vax_syscm.c \
	${VAXD}/vax780_stddev.c ${VAXD}/vax780_sbi.c \
	${VAXD}/vax780_mem.c ${VAXD}/vax780_uba.c ${VAXD}/vax7x0_mba.c \
	${VAXD}/vax780_fload.c ${VAXD}/vax780_syslist.c \
	${PDP11D}/pdp11_rl.c ${PDP11D}/pdp11_rq.c ${PDP11D}/pdp11_ts.c \
	${PDP11D}/pdp11_dz.c ${PDP11D}/pdp11_lp.c ${PDP11D}/pdp11_tq.c \
	${PDP11D}/pdp11_xu.c ${PDP11D}/pdp11_ry.c ${PDP11D}/pdp11_cr.c \
	${PDP11D}/pdp11_rp.c ${PDP11D}/pdp11_tu.c ${PDP11D}/pdp11_hk.c \
	${PDP11D}/pdp11_vh.c ${PDP11D}/pdp11_dmc.c ${PDP11D}/pdp11_dup.c \
	${PDP11D}/pdp11_td.c ${PDP11D}/pdp11_io_lib.c
VAX780_OPT = -DVM_VAX -DVAX_780 -DUSE_INT64 -DUSE_ADDR64 -I VAX -I ${PDP11D} ${NETWORK_OPT}

VAX8600 = ${VAXD}/vax_cpu.c ${VAXD}/vax_cpu1.c ${VAXD}/vax_fpa.c \
	${VAXD}/vax_cis.c ${VAXD}/vax_octa.c  ${VAXD}/vax_cmode.c \
	${VAXD}/vax_mmu.c ${VAXD}/vax_sys.c  ${VAXD}/vax_syscm.c \
	${VAXD}/vax860_stddev.c ${VAXD}/vax860_sbia.c \
	${VAXD}/vax860_abus.c ${VAXD}/vax780_uba.c ${VAXD}/vax7x0_mba.c \
	${VAXD}/vax860_syslist.c \
	${PDP11D}/pdp11_rl.c ${PDP11D}/pdp11_rq.c ${PDP11D}/pdp11_ts.c \
	${PDP11D}/pdp11_dz.c ${PDP11D}/pdp11_lp.c ${PDP11D}/pdp11_tq.c \
	${PDP11D}/pdp11_xu.c ${PDP11D}/pdp11_ry.c ${PDP11D}/pdp11_cr.c \
	${PDP11D}/pdp11_rp.c ${PDP11D}/pdp11_tu.c ${PDP11D}/pdp11_hk.c \
	${PDP11D}/pdp11_vh.c ${PDP11D}/pdp11_dmc.c ${PDP11D}/pdp11_dup.c \
	${PDP11D}/pdp11_td.c ${PDP11D}/pdp11_io_lib.c
VAX8600_OPT = -DVM_VAX -DVAX_860 -DUSE_INT64 -DUSE_ADDR64 -I VAX -I ${PDP11D} ${NETWORK_OPT}



PDP10D = PDP10
PDP10 = ${PDP10D}/pdp10_fe.c ${PDP11D}/pdp11_dz.c ${PDP10D}/pdp10_cpu.c \
	${PDP10D}/pdp10_ksio.c ${PDP10D}/pdp10_lp20.c ${PDP10D}/pdp10_mdfp.c \
	${PDP10D}/pdp10_pag.c ${PDP10D}/pdp10_rp.c ${PDP10D}/pdp10_sys.c \
	${PDP10D}/pdp10_tim.c ${PDP10D}/pdp10_tu.c ${PDP10D}/pdp10_xtnd.c \
	${PDP11D}/pdp11_pt.c ${PDP11D}/pdp11_ry.c ${PDP11D}/pdp11_cr.c \
	${PDP11D}/pdp11_dup.c ${PDP11D}/pdp11_dmc.c ${PDP11D}/pdp11_kmc.c \
	${PDP11D}/pdp11_xu.c
PDP10_OPT = -DVM_PDP10 -DUSE_INT64 -I ${PDP10D} -I ${PDP11D} ${NETWORK_OPT} ${REALCONS_OPT}
# git without NETWORK? PDP10_OPT = -DVM_PDP10 -DUSE_INT64 -I ${PDP10D} -I ${PDP11D}


PDP8D = PDP8
PDP8 = ${PDP8D}/pdp8_cpu.c ${PDP8D}/pdp8_clk.c ${PDP8D}/pdp8_df.c \
	${PDP8D}/pdp8_dt.c ${PDP8D}/pdp8_lp.c ${PDP8D}/pdp8_mt.c \
	${PDP8D}/pdp8_pt.c ${PDP8D}/pdp8_rf.c ${PDP8D}/pdp8_rk.c \
	${PDP8D}/pdp8_rx.c ${PDP8D}/pdp8_sys.c ${PDP8D}/pdp8_tt.c \
	${PDP8D}/pdp8_ttx.c ${PDP8D}/pdp8_rl.c ${PDP8D}/pdp8_tsc.c \
	${PDP8D}/pdp8_td.c ${PDP8D}/pdp8_ct.c ${PDP8D}/pdp8_fpp.c
PDP8_OPT = -DVM_PDP8 -I ${PDP8D}  ${REALCONS_OPT}


H316D = H316
H316 = ${H316D}/h316_stddev.c ${H316D}/h316_lp.c ${H316D}/h316_cpu.c \
	${H316D}/h316_sys.c ${H316D}/h316_mt.c ${H316D}/h316_fhd.c \
	${H316D}/h316_dp.c ${H316D}/h316_rtc.c ${H316D}/h316_imp.c \
	${H316D}/h316_hi.c ${H316D}/h316_mi.c ${H316D}/h316_udp.c
H316_OPT = -I ${H316D} -D VM_IMPTIP


HP2100D = HP2100
HP2100 = ${HP2100D}/hp2100_stddev.c ${HP2100D}/hp2100_dp.c ${HP2100D}/hp2100_dq.c \
	${HP2100D}/hp2100_dr.c ${HP2100D}/hp2100_lps.c ${HP2100D}/hp2100_ms.c \
	${HP2100D}/hp2100_mt.c ${HP2100D}/hp2100_mux.c ${HP2100D}/hp2100_cpu.c \
	${HP2100D}/hp2100_fp.c ${HP2100D}/hp2100_sys.c ${HP2100D}/hp2100_lpt.c \
	${HP2100D}/hp2100_ipl.c ${HP2100D}/hp2100_ds.c ${HP2100D}/hp2100_cpu0.c \
	${HP2100D}/hp2100_cpu1.c ${HP2100D}/hp2100_cpu2.c ${HP2100D}/hp2100_cpu3.c \
	${HP2100D}/hp2100_cpu4.c ${HP2100D}/hp2100_cpu5.c ${HP2100D}/hp2100_cpu6.c \
	${HP2100D}/hp2100_cpu7.c ${HP2100D}/hp2100_fp1.c ${HP2100D}/hp2100_baci.c \
	${HP2100D}/hp2100_mpx.c ${HP2100D}/hp2100_pif.c ${HP2100D}/hp2100_di.c \
	${HP2100D}/hp2100_di_da.c ${HP2100D}/hp_disclib.c
HP2100_OPT = -DHAVE_INT64 -I ${HP2100D}


I1401D = I1401
I1401 = ${I1401D}/i1401_lp.c ${I1401D}/i1401_cpu.c ${I1401D}/i1401_iq.c \
	${I1401D}/i1401_cd.c ${I1401D}/i1401_mt.c ${I1401D}/i1401_dp.c \
	${I1401D}/i1401_sys.c
I1401_OPT = -I ${I1401D}


I1620D = I1620
I1620 = ${I1620D}/i1620_cd.c ${I1620D}/i1620_dp.c ${I1620D}/i1620_pt.c \
	${I1620D}/i1620_tty.c ${I1620D}/i1620_cpu.c ${I1620D}/i1620_lp.c \
	${I1620D}/i1620_fp.c ${I1620D}/i1620_sys.c
I1620_OPT = -I ${I1620D}


I7094D = I7094
I7094 = ${I7094D}/i7094_cpu.c ${I7094D}/i7094_cpu1.c ${I7094D}/i7094_io.c \
	${I7094D}/i7094_cd.c ${I7094D}/i7094_clk.c ${I7094D}/i7094_com.c \
	${I7094D}/i7094_drm.c ${I7094D}/i7094_dsk.c ${I7094D}/i7094_sys.c \
	${I7094D}/i7094_lp.c ${I7094D}/i7094_mt.c ${I7094D}/i7094_binloader.c
I7094_OPT = -DUSE_INT64 -I ${I7094D}


IBM1130D = Ibm1130
IBM1130 = ${IBM1130D}/ibm1130_cpu.c ${IBM1130D}/ibm1130_cr.c \
	${IBM1130D}/ibm1130_disk.c ${IBM1130D}/ibm1130_stddev.c \
	${IBM1130D}/ibm1130_sys.c ${IBM1130D}/ibm1130_gdu.c \
	${IBM1130D}/ibm1130_gui.c ${IBM1130D}/ibm1130_prt.c \
	${IBM1130D}/ibm1130_fmt.c ${IBM1130D}/ibm1130_ptrp.c \
	${IBM1130D}/ibm1130_plot.c ${IBM1130D}/ibm1130_sca.c \
	${IBM1130D}/ibm1130_t2741.c
IBM1130_OPT = -I ${IBM1130D}
ifneq ($(WIN32),)
IBM1130_OPT += -DGUI_SUPPORT -lgdi32
endif


ID16D = Interdata
ID16 = ${ID16D}/id16_cpu.c ${ID16D}/id16_sys.c ${ID16D}/id_dp.c \
	${ID16D}/id_fd.c ${ID16D}/id_fp.c ${ID16D}/id_idc.c ${ID16D}/id_io.c \
	${ID16D}/id_lp.c ${ID16D}/id_mt.c ${ID16D}/id_pas.c ${ID16D}/id_pt.c \
	${ID16D}/id_tt.c ${ID16D}/id_uvc.c ${ID16D}/id16_dboot.c ${ID16D}/id_ttp.c
ID16_OPT = -I ${ID16D}


ID32D = Interdata
ID32 = ${ID32D}/id32_cpu.c ${ID32D}/id32_sys.c ${ID32D}/id_dp.c \
	${ID32D}/id_fd.c ${ID32D}/id_fp.c ${ID32D}/id_idc.c ${ID32D}/id_io.c \
	${ID32D}/id_lp.c ${ID32D}/id_mt.c ${ID32D}/id_pas.c ${ID32D}/id_pt.c \
	${ID32D}/id_tt.c ${ID32D}/id_uvc.c ${ID32D}/id32_dboot.c ${ID32D}/id_ttp.c
ID32_OPT = -I ${ID32D}


S3D = S3
S3 = ${S3D}/s3_cd.c ${S3D}/s3_cpu.c ${S3D}/s3_disk.c ${S3D}/s3_lp.c \
	${S3D}/s3_pkb.c ${S3D}/s3_sys.c
S3_OPT = -I ${S3D}


ALTAIRD = ALTAIR
ALTAIR = ${ALTAIRD}/altair_sio.c ${ALTAIRD}/altair_cpu.c ${ALTAIRD}/altair_dsk.c \
	${ALTAIRD}/altair_sys.c
ALTAIR_OPT = -I ${ALTAIRD}


ALTAIRZ80D = AltairZ80
ALTAIRZ80 = ${ALTAIRZ80D}/altairz80_cpu.c ${ALTAIRZ80D}/altairz80_cpu_nommu.c \
	${ALTAIRZ80D}/altairz80_dsk.c ${ALTAIRZ80D}/disasm.c \
	${ALTAIRZ80D}/altairz80_sio.c ${ALTAIRZ80D}/altairz80_sys.c \
	${ALTAIRZ80D}/altairz80_hdsk.c ${ALTAIRZ80D}/altairz80_net.c \
	${ALTAIRZ80D}/flashwriter2.c ${ALTAIRZ80D}/i86_decode.c \
	${ALTAIRZ80D}/i86_ops.c ${ALTAIRZ80D}/i86_prim_ops.c \
	${ALTAIRZ80D}/i8272.c ${ALTAIRZ80D}/insnsd.c ${ALTAIRZ80D}/altairz80_mhdsk.c \
	${ALTAIRZ80D}/mfdc.c ${ALTAIRZ80D}/n8vem.c ${ALTAIRZ80D}/vfdhd.c \
	${ALTAIRZ80D}/s100_disk1a.c ${ALTAIRZ80D}/s100_disk2.c ${ALTAIRZ80D}/s100_disk3.c \
	${ALTAIRZ80D}/s100_fif.c ${ALTAIRZ80D}/s100_mdriveh.c \
	${ALTAIRZ80D}/s100_mdsad.c ${ALTAIRZ80D}/s100_selchan.c \
	${ALTAIRZ80D}/s100_ss1.c ${ALTAIRZ80D}/s100_64fdc.c \
	${ALTAIRZ80D}/s100_scp300f.c \
	${ALTAIRZ80D}/wd179x.c ${ALTAIRZ80D}/s100_hdc1001.c \
	${ALTAIRZ80D}/s100_if3.c ${ALTAIRZ80D}/s100_adcs6.c \
	${ALTAIRZ80D}/m68kcpu.c ${ALTAIRZ80D}/m68kdasm.c \
	${ALTAIRZ80D}/m68kopac.c ${ALTAIRZ80D}/m68kopdm.c \
	${ALTAIRZ80D}/m68kopnz.c ${ALTAIRZ80D}/m68kops.c ${ALTAIRZ80D}/m68ksim.c
ALTAIRZ80_OPT = -I ${ALTAIRZ80D} -DUSE_SIM_IMD


GRID = GRI
GRI = ${GRID}/gri_cpu.c ${GRID}/gri_stddev.c ${GRID}/gri_sys.c
GRI_OPT = -I ${GRID}


LGPD = LGP
LGP = ${LGPD}/lgp_cpu.c ${LGPD}/lgp_stddev.c ${LGPD}/lgp_sys.c
LGP_OPT = -I ${LGPD}


SDSD = SDS
SDS = ${SDSD}/sds_cpu.c ${SDSD}/sds_drm.c ${SDSD}/sds_dsk.c ${SDSD}/sds_io.c \
	${SDSD}/sds_lp.c ${SDSD}/sds_mt.c ${SDSD}/sds_mux.c ${SDSD}/sds_rad.c \
	${SDSD}/sds_stddev.c ${SDSD}/sds_sys.c
SDS_OPT = -I ${SDSD}


SWTP6800D = swtp6800/swtp6800
SWTP6800C = swtp6800/common
SWTP6800MP-A = ${SWTP6800C}/mp-a.c ${SWTP6800C}/m6800.c ${SWTP6800C}/m6810.c \
	${SWTP6800C}/bootrom.c ${SWTP6800C}/dc-4.c ${SWTP6800C}/mp-s.c ${SWTP6800D}/mp-a_sys.c \
	${SWTP6800C}/mp-b2.c ${SWTP6800C}/mp-8m.c
SWTP6800MP-A2 = ${SWTP6800C}/mp-a2.c ${SWTP6800C}/m6800.c ${SWTP6800C}/m6810.c \
	${SWTP6800C}/bootrom.c ${SWTP6800C}/dc-4.c ${SWTP6800C}/mp-s.c ${SWTP6800D}/mp-a2_sys.c \
	${SWTP6800C}/mp-b2.c ${SWTP6800C}/mp-8m.c ${SWTP6800C}/i2716.c
SWTP6800_OPT = -I ${SWTP6800D}


ISYS8010D = Intel-Systems/isys8010
ISYS8010C = Intel-Systems/common
ISYS8010 = ${ISYS8010C}/i8080.c ${ISYS8010D}/isys8010_sys.c \
	${ISYS8010C}/i8251.c ${ISYS8010C}/i8255.c \
	${ISYS8010C}/ieprom.c ${ISYS8010C}/iram8.c \
	${ISYS8010C}/multibus.c ${ISYS8010C}/isbc80-10.c	\
	${ISYS8010C}/isbc064.c ${ISYS8010C}/isbc208.c
ISYS8010_OPT = -I ${ISYS8010D}


ISYS8020D = Intel-Systems/isys8020
ISYS8020C = Intel-Systems/common
ISYS8020 = ${ISYS8020C}/i8080.c ${ISYS8020D}/isys8020_sys.c \
	${ISYS8020C}/i8251.c ${ISYS8020C}/i8255.c \
	${ISYS8020C}/ieprom.c ${ISYS8020C}/iram8.c \
	${ISYS8020C}/multibus.c ${ISYS8020C}/isbc80-20.c	\
	${ISYS8020C}/isbc064.c ${ISYS8020C}/isbc208.c \
	${ISYS8020C}/i8259.c
ISYS8020_OPT = -I ${ISYS8020D}


TX0D = TX-0
TX0 = ${TX0D}/tx0_cpu.c ${TX0D}/tx0_dpy.c ${TX0D}/tx0_stddev.c \
      ${TX0D}/tx0_sys.c ${TX0D}/tx0_sys_orig.c ${DISPLAYL}
TX0_OPT = -I ${TX0D} $(DISPLAY_OPT)


SSEMD = SSEM
SSEM = ${SSEMD}/ssem_cpu.c ${SSEMD}/ssem_sys.c
SSEM_OPT = -I ${SSEMD}

###
### Experimental simulators
###

BESM6D = BESM6
BESM6 = ${BESM6D}/besm6_cpu.c ${BESM6D}/besm6_sys.c ${BESM6D}/besm6_mmu.c \
        ${BESM6D}/besm6_arith.c ${BESM6D}/besm6_disk.c ${BESM6D}/besm6_drum.c \
        ${BESM6D}/besm6_tty.c ${BESM6D}/besm6_panel.c ${BESM6D}/besm6_printer.c \
        ${BESM6D}/besm6_punch.c

ifneq (,$(and ${VIDEO_LDFLAGS}, $(BESM6_BUILD)))
    ifeq (,${FONTFILE})
        FONTPATH += /usr/share/fonts /Library/Fonts /usr/lib/jvm /System/Library/Frameworks/JavaVM.framework/Versions C:/Windows/Fonts
        FONTPATH := $(dir $(foreach dir,$(strip $(FONTPATH)),$(wildcard $(dir)/.)))
        FONTNAME += DejaVuSans.ttf LucidaSansRegular.ttf FreeSans.ttf AppleGothic.ttf tahoma.ttf
        $(info font paths are: $(FONTPATH))
        $(info font names are: $(FONTNAME))
        find_fontfile = $(strip $(firstword $(foreach dir,$(strip $(FONTPATH)),$(wildcard $(dir)/$(1))$(wildcard $(dir)/*/$(1))$(wildcard $(dir)/*/*/$(1))$(wildcard $(dir)/*/*/*/$(1)))))
        find_font = $(abspath $(strip $(firstword $(foreach font,$(strip $(FONTNAME)),$(call find_fontfile,$(font))))))
        ifneq (,$(call find_font))
            FONTFILE=$(call find_font)
        else
            $(info ***)
            $(info *** No font file available, BESM-6 video panel disabled.)
            $(info ***)
            $(info *** To enable the panel display please specify one of:)
            $(info ***          a font path with FONTNAME=path)
            $(info ***          a font name with FONTNAME=fontname.ttf)
            $(info ***          a font file with FONTFILE=path/fontname.ttf)
            $(info ***)
        endif
    endif
endif
ifeq (,$(and ${VIDEO_LDFLAGS}, ${FONTFILE}))
    BESM6_OPT = -I ${BESM6D} -DUSE_INT64
else ifneq (,$(and $(findstring SDL2,${VIDEO_LDFLAGS}),$(call find_include,SDL2/SDL_ttf),$(call find_lib,SDL2_ttf)))
    $(info using libSDL2_ttf: $(call find_lib,SDL2_ttf) $(call find_include,SDL2/SDL_ttf))
    $(info ***)
    BESM6_OPT = -I ${BESM6D} -DFONTFILE=${FONTFILE} -DUSE_INT64 ${VIDEO_CCDEFS} ${VIDEO_LDFLAGS} -lSDL2_ttf
else ifneq (,$(and $(call find_include,SDL/SDL_ttf),$(call find_lib,SDL_ttf)))
    $(info using libSDL_ttf: $(call find_lib,SDL_ttf) $(call find_include,SDL/SDL_ttf))
    $(info ***)
    BESM6_OPT = -I ${BESM6D} -DFONTFILE=${FONTFILE} -DUSE_INT64 ${VIDEO_CCDEFS} ${VIDEO_LDFLAGS} -lSDL_ttf
else
    BESM6_OPT = -I ${BESM6D} -DUSE_INT64
endif

###
### Unsupported/Incomplete simulators
###

SIGMAD = sigma
SIGMA = ${SIGMAD}/sigma_cpu.c ${SIGMAD}/sigma_sys.c ${SIGMAD}/sigma_cis.c \
	${SIGMAD}/sigma_coc.c ${SIGMAD}/sigma_dk.c ${SIGMAD}/sigma_dp.c \
	${SIGMAD}/sigma_fp.c ${SIGMAD}/sigma_io.c ${SIGMAD}/sigma_lp.c \
	${SIGMAD}/sigma_map.c ${SIGMAD}/sigma_mt.c ${SIGMAD}/sigma_pt.c \
    ${SIGMAD}/sigma_rad.c ${SIGMAD}/sigma_rtc.c ${SIGMAD}/sigma_tt.c
SIGMA_OPT = -I ${SIGMAD}

ALPHAD = alpha
ALPHA = ${ALPHAD}/alpha_500au_syslist.c ${ALPHAD}/alpha_cpu.c \
    ${ALPHAD}/alpha_ev5_cons.c ${ALPHAD}/alpha_ev5_pal.c \
    ${ALPHAD}/alpha_ev5_tlb.c ${ALPHAD}/alpha_fpi.c \
    ${ALPHAD}/alpha_fpv.c ${ALPHAD}/alpha_io.c \
    ${ALPHAD}/alpha_mmu.c ${ALPHAD}/alpha_sys.c
ALPHA_OPT = -I ${ALPHAD} -DUSE_ADDR64 -DUSE_INT64

SAGED = SAGE
SAGE = ${SAGED}/sage_cpu.c ${SAGED}/sage_sys.c ${SAGED}/sage_stddev.c \
    ${SAGED}/sage_cons.c ${SAGED}/sage_fd.c ${SAGED}/sage_lp.c \
    ${SAGED}/m68k_cpu.c ${SAGED}/m68k_mem.c ${SAGED}/m68k_scp.c \
    ${SAGED}/m68k_parse.tab.c ${SAGED}/m68k_sys.c \
    ${SAGED}/i8251.c ${SAGED}/i8253.c ${SAGED}/i8255.c ${SAGED}/i8259.c ${SAGED}/i8272.c
SAGE_OPT = -I ${SAGED} -DHAVE_INT64 -DUSE_SIM_IMD

PDQ3D = PDQ-3
PDQ3 = ${PDQ3D}/pdq3_cpu.c ${PDQ3D}/pdq3_sys.c ${PDQ3D}/pdq3_stddev.c \
    ${PDQ3D}/pdq3_mem.c ${PDQ3D}/pdq3_debug.c ${PDQ3D}/pdq3_fdc.c
PDQ3_OPT = -I ${PDQ3D} -DUSE_SIM_IMD


#
# Build everything (not the unsupported/incomplete simulators)
#
# ALL = pdp11 pdp10 pdp8 #vax
# ALL = pdp1 pdp4 pdp7 pdp8 pdp9 pdp15 pdp11 pdp10 \
# 	vax microvax3900 microvax1 rtvax1000 microvax2 vax730 vax750 vax780 vax8600 \
# 	nova eclipse hp2100 i1401 i1620 s3 altair altairz80 gri \
# 	i7094 ibm1130 id16 id32 sds lgp h316 \
# 	swtp6800mp-a swtp6800mp-a2 tx-0 ssem isys8010 isys8020

# all : ${ALL}
all : pdp11_realcons pdp8_realcons pdp10_realcons pdp15_realcons vax

clean :
ifeq ($(WIN32),)
	${RM} -r ${BIN}
else
	if exist BIN\*.exe del /q BIN\*.exe
	if exist BIN rmdir BIN
endif

${BIN}BuildROMs${EXE} :
	${MKDIRBIN}
ifeq (agcc,$(findstring agcc,$(firstword $(CC))))
	gcc $(wordlist 2,1000,${CC}) sim_BuildROMs.c $(CC_OUTSPEC)
else
	${CC} sim_BuildROMs.c $(CC_OUTSPEC)
endif
ifeq ($(WIN32),)
	$@
	${RM} $@
  ifeq (Darwin,$(OSTYPE)) # remove Xcode's debugging symbols folder too
	${RM} -rf $@.dSYM
  endif
else
	$(@D)\$(@F)
	del $(@D)\$(@F)
endif

#
# Individual builds
#
pdp1 : ${BIN}pdp1${EXE}

${BIN}pdp1${EXE} : ${PDP1} ${SIM}
	${MKDIRBIN}
	${CC} ${PDP1} ${SIM} ${PDP1_OPT} $(CC_OUTSPEC) ${LDFLAGS}

pdp4 : ${BIN}pdp4${EXE}

${BIN}pdp4${EXE} : ${PDP18B} ${SIM}
	${MKDIRBIN}
	${CC} ${PDP18B} ${SIM} ${PDP4_OPT} $(CC_OUTSPEC) ${LDFLAGS}

pdp7 : ${BIN}pdp7${EXE}

${BIN}pdp7${EXE} : ${PDP18B} ${SIM}
	${MKDIRBIN}
	${CC} ${PDP18B} ${SIM} ${PDP7_OPT} $(CC_OUTSPEC) ${LDFLAGS}

pdp8_realcons : ${BIN}pdp8_realcons${EXE}

${BIN}pdp8_realcons${EXE} : ${PDP8} ${SIM} $(REALCONS) $(REALCONS_PDP8)
	${MKDIRBIN}
	${CC} ${PDP8} ${SIM} $(REALCONS)  $(REALCONS_PDP8) ${PDP8_OPT} $(CC_OUTSPEC) ${LDFLAGS}
	file ${BIN}pdp8_realcons${EXE}

pdp9 : ${BIN}pdp9${EXE}

${BIN}pdp9${EXE} : ${PDP18B} ${SIM}
	${MKDIRBIN}
	${CC} ${PDP18B} ${SIM} ${PDP9_OPT} $(CC_OUTSPEC) ${LDFLAGS}

pdp15_realcons : ${BIN}pdp15_realcons${EXE}

${BIN}pdp15_realcons${EXE} : ${PDP18B} ${SIM} $(REALCONS) $(REALCONS_PDP15)
	${MKDIRBIN}
	${CC} ${PDP18B} ${SIM} $(REALCONS) $(REALCONS_PDP15) ${PDP15_OPT} $(CC_OUTSPEC) ${LDFLAGS}
	file ${BIN}pdp15_realcons${EXE}

pdp10_realcons : ${BIN}pdp10_realcons${EXE}

${BIN}pdp10_realcons${EXE} : ${PDP10} ${SIM} $(REALCONS) $(REALCONS_PDP10)
	${MKDIRBIN}
	${CC} ${PDP10} ${SIM} $(REALCONS) $(REALCONS_PDP10) ${PDP10_OPT} $(CC_OUTSPEC) ${LDFLAGS}
	file ${BIN}pdp10_realcons${EXE}

pdp11_realcons : ${BIN}pdp11_realcons${EXE}

${BIN}pdp11_realcons${EXE} : ${PDP11} ${SIM} $(REALCONS) $(REALCONS_PDP11)
	${MKDIRBIN}
	${CC} ${PDP11} ${SIM} $(REALCONS) $(REALCONS_PDP11) ${PDP11_OPT} $(CC_OUTSPEC) ${LDFLAGS}
	file ${BIN}pdp11_realcons${EXE}

vax : microvax3900

microvax3900 : ${BIN}microvax3900${EXE}

${BIN}microvax3900${EXE} : ${VAX} ${SIM} ${BUILD_ROMS}
	${MKDIRBIN}
	${CC} ${VAX} ${SIM} ${VAX_OPT} $(CC_OUTSPEC) ${LDFLAGS}
ifeq ($(WIN32),)
	cp ${BIN}microvax3900${EXE} ${BIN}vax${EXE}
else
	copy $(@D)\microvax3900${EXE} $(@D)\vax${EXE}
endif

microvax1 : ${BIN}microvax1${EXE}

${BIN}microvax1${EXE} : ${VAX610} ${SIM} ${BUILD_ROMS}
	${MKDIRBIN}
	${CC} ${VAX610} ${SIM} ${VAX610_OPT} -o $@ ${LDFLAGS}

rtvax1000 : ${BIN}rtvax1000${EXE}

${BIN}rtvax1000${EXE} : ${VAX630} ${SIM} ${BUILD_ROMS}
	${MKDIRBIN}
	${CC} ${VAX630} ${SIM} ${VAX620_OPT} -o $@ ${LDFLAGS}

microvax2 : ${BIN}microvax2${EXE}

${BIN}microvax2${EXE} : ${VAX630} ${SIM} ${BUILD_ROMS}
	${MKDIRBIN}
	${CC} ${VAX630} ${SIM} ${VAX630_OPT} -o $@ ${LDFLAGS}

vax730 : ${BIN}vax730${EXE}

${BIN}vax730${EXE} : ${VAX730} ${SIM} ${BUILD_ROMS}
	${MKDIRBIN}
	${CC} ${VAX730} ${SIM} ${VAX730_OPT} -o $@ ${LDFLAGS}

vax750 : ${BIN}vax750${EXE}

${BIN}vax750${EXE} : ${VAX750} ${SIM} ${BUILD_ROMS}
	${MKDIRBIN}
	${CC} ${VAX750} ${SIM} ${VAX750_OPT} -o $@ ${LDFLAGS}

vax780 : ${BIN}vax780${EXE}

${BIN}vax780${EXE} : ${VAX780} ${SIM} ${BUILD_ROMS}
	${MKDIRBIN}
	${CC} ${VAX780} ${SIM} ${VAX780_OPT} $(CC_OUTSPEC) ${LDFLAGS}

vax8600 : ${BIN}vax8600${EXE}

${BIN}vax8600${EXE} : ${VAX8600} ${SIM} ${BUILD_ROMS}
	${MKDIRBIN}
	${CC} ${VAX8600} ${SIM} ${VAX8600_OPT} $(CC_OUTSPEC) ${LDFLAGS}

nova : ${BIN}nova${EXE}

${BIN}nova${EXE} : ${NOVA} ${SIM}
	${MKDIRBIN}
	${CC} ${NOVA} ${SIM} ${NOVA_OPT} $(CC_OUTSPEC) ${LDFLAGS}

eclipse : ${BIN}eclipse${EXE}

${BIN}eclipse${EXE} : ${ECLIPSE} ${SIM}
	${MKDIRBIN}
	${CC} ${ECLIPSE} ${SIM} ${ECLIPSE_OPT} $(CC_OUTSPEC) ${LDFLAGS}

h316 : ${BIN}h316${EXE}

${BIN}h316${EXE} : ${H316} ${SIM}
	${MKDIRBIN}
	${CC} ${H316} ${SIM} ${H316_OPT} $(CC_OUTSPEC) ${LDFLAGS}

hp2100 : ${BIN}hp2100${EXE}

${BIN}hp2100${EXE} : ${HP2100} ${SIM}
	${MKDIRBIN}
	${CC} ${HP2100} ${SIM} ${HP2100_OPT} $(CC_OUTSPEC) ${LDFLAGS}

i1401 : ${BIN}i1401${EXE}

${BIN}i1401${EXE} : ${I1401} ${SIM}
	${MKDIRBIN}
	${CC} ${I1401} ${SIM} ${I1401_OPT} $(CC_OUTSPEC) ${LDFLAGS}

i1620 : ${BIN}i1620${EXE}

${BIN}i1620${EXE} : ${I1620} ${SIM}
	${MKDIRBIN}
	${CC} ${I1620} ${SIM} ${I1620_OPT} $(CC_OUTSPEC) ${LDFLAGS}

i7094 : ${BIN}i7094${EXE}

${BIN}i7094${EXE} : ${I7094} ${SIM}
	${MKDIRBIN}
	${CC} ${I7094} ${SIM} ${I7094_OPT} $(CC_OUTSPEC) ${LDFLAGS}

ibm1130 : ${BIN}ibm1130${EXE}

${BIN}ibm1130${EXE} : ${IBM1130}
	${MKDIRBIN}
ifneq ($(WIN32),)
	windres ${IBM1130D}/ibm1130.rc $(BIN)ibm1130.o
	${CC} ${IBM1130} ${SIM} ${IBM1130_OPT} $(BIN)ibm1130.o $(CC_OUTSPEC) ${LDFLAGS}
	del BIN\ibm1130.o
else
	${CC} ${IBM1130} ${SIM} ${IBM1130_OPT} $(CC_OUTSPEC) ${LDFLAGS}
endif

s3 : ${BIN}s3${EXE}

${BIN}s3${EXE} : ${S3} ${SIM}
	${MKDIRBIN}
	${CC} ${S3} ${SIM} ${S3_OPT} $(CC_OUTSPEC) ${LDFLAGS}

altair : ${BIN}altair${EXE}

${BIN}altair${EXE} : ${ALTAIR} ${SIM}
	${MKDIRBIN}
	${CC} ${ALTAIR} ${SIM} ${ALTAIR_OPT} $(CC_OUTSPEC) ${LDFLAGS}

altairz80 : ${BIN}altairz80${EXE}

${BIN}altairz80${EXE} : ${ALTAIRZ80} ${SIM}
	${MKDIRBIN}
	${CC} ${ALTAIRZ80} ${SIM} ${ALTAIRZ80_OPT} $(CC_OUTSPEC) ${LDFLAGS}

gri : ${BIN}gri${EXE}

${BIN}gri${EXE} : ${GRI} ${SIM}
	${MKDIRBIN}
	${CC} ${GRI} ${SIM} ${GRI_OPT} $(CC_OUTSPEC) ${LDFLAGS}

lgp : ${BIN}lgp${EXE}

${BIN}lgp${EXE} : ${LGP} ${SIM}
	${MKDIRBIN}
	${CC} ${LGP} ${SIM} ${LGP_OPT} $(CC_OUTSPEC) ${LDFLAGS}

id16 : ${BIN}id16${EXE}

${BIN}id16${EXE} : ${ID16} ${SIM}
	${MKDIRBIN}
	${CC} ${ID16} ${SIM} ${ID16_OPT} $(CC_OUTSPEC) ${LDFLAGS}

id32 : ${BIN}id32${EXE}

${BIN}id32${EXE} : ${ID32} ${SIM}
	${MKDIRBIN}
	${CC} ${ID32} ${SIM} ${ID32_OPT} $(CC_OUTSPEC) ${LDFLAGS}

sds : ${BIN}sds${EXE}

${BIN}sds${EXE} : ${SDS} ${SIM}
	${MKDIRBIN}
	${CC} ${SDS} ${SIM} ${SDS_OPT} $(CC_OUTSPEC) ${LDFLAGS}

swtp6800mp-a : ${BIN}swtp6800mp-a${EXE}

${BIN}swtp6800mp-a${EXE} : ${SWTP6800MP-A} ${SIM} ${BUILD_ROMS}
	${MKDIRBIN}
	${CC} ${SWTP6800MP-A} ${SIM} ${SWTP6800_OPT} $(CC_OUTSPEC) ${LDFLAGS}

swtp6800mp-a2 : ${BIN}swtp6800mp-a2${EXE}

${BIN}swtp6800mp-a2${EXE} : ${SWTP6800MP-A2} ${SIM} ${BUILD_ROMS}
	${MKDIRBIN}
	${CC} ${SWTP6800MP-A2} ${SIM} ${SWTP6800_OPT} $(CC_OUTSPEC) ${LDFLAGS}


isys8010: ${BIN}isys8010${EXE}

${BIN}isys8010${EXE} : ${ISYS8010} ${SIM} ${BUILD_ROMS}
	${MKDIRBIN}
	${CC} ${ISYS8010} ${SIM} ${ISYS8010_OPT} $(CC_OUTSPEC) ${LDFLAGS}

isys8020: ${BIN}isys8020${EXE}

${BIN}isys8020${EXE} : ${ISYS8020} ${SIM} ${BUILD_ROMS}
	${MKDIRBIN}
	${CC} ${ISYS8020} ${SIM} ${ISYS8020_OPT} $(CC_OUTSPEC) ${LDFLAGS}

tx-0 : ${BIN}tx-0${EXE}

${BIN}tx-0${EXE} : ${TX0} ${SIM}
	${MKDIRBIN}
	${CC} ${TX0} ${SIM} ${TX0_OPT} $(CC_OUTSPEC) ${LDFLAGS}

ssem : ${BIN}ssem${EXE}

${BIN}ssem${EXE} : ${SSEM} ${SIM}
	${MKDIRBIN}
	${CC} ${SSEM} ${SIM} ${SSEM_OPT} $(CC_OUTSPEC) ${LDFLAGS}

besm6 : ${BIN}besm6${EXE}

${BIN}besm6${EXE} : ${BESM6} ${SIM}
	${MKDIRBIN}
	${CC} ${BESM6} ${SIM} ${BESM6_OPT} $(CC_OUTSPEC) ${LDFLAGS}

sigma : ${BIN}sigma${EXE}

${BIN}sigma${EXE} : ${SIGMA} ${SIM}
	${MKDIRBIN}
	${CC} ${SIGMA} ${SIM} ${SIGMA_OPT} $(CC_OUTSPEC) ${LDFLAGS}

alpha : ${BIN}alpha${EXE}

${BIN}alpha${EXE} : ${ALPHA} ${SIM}
	${MKDIRBIN}
	${CC} ${ALPHA} ${SIM} ${ALPHA_OPT} $(CC_OUTSPEC) ${LDFLAGS}

sage : ${BIN}sage${EXE}

${BIN}sage${EXE} : ${SAGE} ${SIM}
	${MKDIRBIN}
	${CC} ${SAGE} ${SIM} ${SAGE_OPT} $(CC_OUTSPEC) ${LDFLAGS}

pdq3 : ${BIN}pdq3${EXE}

${BIN}pdq3${EXE} : ${PDQ3} ${SIM}
	${MKDIRBIN}
	${CC} ${PDQ3} ${SIM} ${PDQ3_OPT} $(CC_OUTSPEC) ${LDFLAGS}

# Front Panel API Demo/Test program

frontpaneltest : ${BIN}frontpaneltest${EXE}

${BIN}frontpaneltest${EXE} : frontpanel/FrontPanelTest.c sim_sock.c sim_frontpanel.c
	${MKDIRBIN}
	${CC} frontpanel/FrontPanelTest.c sim_sock.c sim_frontpanel.c $(CC_OUTSPEC) ${LDFLAGS}

