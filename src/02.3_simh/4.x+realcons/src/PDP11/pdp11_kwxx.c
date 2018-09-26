/* pdp11_kwxx.c: USC/ISI KW11-XX programmable clock simulator

   KW11-XX Programmable Clock, designed and built at
   USC Information Sciences Institute

   I/O Page Registers:

        17 774 020  read/write	Command/status register
				Bit 15: Spectracom 1PPS signal (read)
				Bit 7:  Interval count done (read)
				Bit 6:  Interval interrupt enable (write)
				Bit 1:  Enable measurement counter clear (write)
        17 774 022  read	Uptime bits 32-39 in bites 0-7
		    write	Clear uptime counter
        17 774 024  read	Uptime bits 16-31
		    write	No op
        17 774 026  read	Uptime bits 0-15
		    write	No op
        17 774 030  read	No op
		    write	Interval bits 16-23 in bits 0-7
        17 774 032  read	No op
		    write	Interval bits 0-15
        17 774 034  read	Measurement timer
		    write	Clear measurement counter
        17 774 036		Unused

   Vector:      0110

   Priority:    BR6L

   ** Theory of Operation **

   A real KW11-XX includes a 10MHz crystal oscillator that would
   normally be phase-locked to the accurate 1MHz reference signal from
   a Spectracom WWVB clock.  The simulated clock simply uses the system
   clock to measure time.  The clock includes the following counters:

   - Uptime counter: 40-bit 25kHz counter that runs continuously
   - Interval timer: 24-bit, 25kHz counter that can interrupt after
     desired intervals
   - Measurement timer: 16-bit, 1MHz counter for accurately measuring
     execution times or real-time events
   - 1Hz signal indicating the beginning of each second

   This software emulator for SIMH implements all of the above using
   the system clock.  Automatic clearing of the measurement timer by
   the 1PPS signal is not implemented.

   Operation of this emulator is rather simplistic as compared to the
   actual device.  The register read and write routines are responsible
   for copying internal state from the simulated device to the operating
   program.  Clock state variables are altered in the write routine.

   The clock service routine (kwxx_svc) is responsible for ticking the
   clock.  The routine updates the internal state according to the options
   selected and signals interrupts when appropriate.

   For a complete description of the device, please see the KW11-XX Unibus
   Clock Module Project Report from the ISI Hardware Development Lab,
   August 1982.

   ** Notes **

   1. The device is disabled by default.

   2. The read and write routines don't do anything with odd address
      accesses.

   3. This is really a Unibus peripheral and thus doesn't actually make
      sense within a J-11 system as there never was a Qbus version of
      this.
*/

#include "pdp11_defs.h"

#define KWXXCSR_RDMASK  0100301                 /* readable */
#define KWXXCSR_WRMASK  0000101                 /* writeable */

/* CSR - 17774020 */

#define CSR_V_CE        0                       /* enable clear */
#define CSR_V_IE        6                       /* interrupt enable */
#define CSR_V_DONE      7                       /* done */
#define CSR_V_1PPS      15                      /* Spectracom 1PPS */
#define CSR_CE          (1u << CSR_V_CE)
#define CSR_IE          (1u << CSR_V_IE)
#define CSR_DONE        (1u << CSR_V_DONE)
#define CSR_1PPS        (1u << CSR_V_1PPS)

extern int32 int_req[IPL_HLVL];

uint32 kwxx_csr = 0;                            /* control/status */
uint32 kwxx_uptime_msb = 0;                     /* uptime counter MSBs */
uint32 kwxx_uptime_mid = 0;                     /* uptime counter middle */
uint32 kwxx_uptime_lsb = 0;                     /* uptime counter LSBs */
uint32 kwxx_interval_msb = 0;                   /* interval counter MSBs */
uint32 kwxx_interval_lsb = 0;                   /* interval counter LSBs */
uint32 kwxx_measure = 0;                        /* measurement counter */
int kwxx_uptime_latched = 0;                    /* nonzero => uptime latched */

double kwxx_uptime_base = 0.0;                  /* sim uptime when reset */
double kwxx_measure_base = 0.0;                 /* sim uptime when cleared */
t_uint64 kwxx_uptime = 0;                       /* latched uptime value */
t_uint64 kwxx_interval = 0;                     /* latched uptime value */

t_stat kwxx_rd (int32 *data, int32 PA, int32 access);
t_stat kwxx_wr (int32 data, int32 PA, int32 access);
t_stat kwxx_svc (UNIT *uptr);
t_stat kwxx_reset (DEVICE *dptr);
const char *kwxx_description (DEVICE *dptr);

/* KWXX data structures

   kwxx_dev     KWXX device descriptor
   kwxx_unit    KWXX unit descriptor
   kwxx_reg     KWXX register list
*/

#define IOLN_KWXX       020

DIB kwxx_dib = {
    IOBA_AUTO, IOLN_KWXX, &kwxx_rd, &kwxx_wr,
    1, IVCL (KWXX), VEC_AUTO, { NULL }
    };

UNIT kwxx_unit = { UDATA (&kwxx_svc, UNIT_IDLE, 0) };

REG kwxx_reg[] = {
    { ORDATA (CLKCSR, kwxx_csr, 16) },
    { ORDATA (UPTMHI, kwxx_uptime_msb, 8) },
    { ORDATA (UPTMMD, kwxx_uptime_mid, 16) },
    { ORDATA (UPTMLO, kwxx_uptime_lsb, 16) },
    { ORDATA (INTMHI, kwxx_interval_lsb, 8) },
    { ORDATA (INTMLO, kwxx_interval_lsb, 16) },
    { ORDATA (MEAS, kwxx_measure, 16) },
    { FLDATA (INT, IREQ (KWXX), INT_V_KWXX) },
    { FLDATA (OPPS, kwxx_csr, CSR_V_1PPS) },
    { FLDATA (DONE, kwxx_csr, CSR_V_DONE) },
    { FLDATA (IE, kwxx_csr, CSR_V_IE) },
    { FLDATA (CE, kwxx_csr, CSR_V_CE) },
    { DRDATA (CURTIM, kwxx_unit.wait, 32), REG_HRO },
    { ORDATA (DEVADDR, kwxx_dib.ba, 32), REG_HRO },
    { ORDATA (DEVVEC, kwxx_dib.vec, 16), REG_HRO },
    { NULL }
    };

MTAB kwxx_mod[] = {
    { MTAB_XTD|MTAB_VDV, 020, "ADDRESS", "ADDRESS",
      &set_addr, &show_addr, NULL, "Bus address" },
    { MTAB_XTD|MTAB_VDV, 0, "VECTOR", "VECTOR",
      &set_vec, &show_vec, NULL, "Interrupt vector" },
    { 0 }
    };

DEVICE kwxx_dev = {
    "KWXX", &kwxx_unit, kwxx_reg, kwxx_mod,
    1, 8, 16, 2, 8, 16,
    NULL, NULL, &kwxx_reset,
    NULL, NULL, NULL,
    &kwxx_dib, DEV_DISABLE | DEV_DIS | DEV_UBUS | DEV_QBUS, 
    0, NULL, NULL, NULL, NULL,
    NULL, NULL, &kwxx_description,
    };

/* Clock I/O address routines */

t_stat kwxx_rd (int32 *data, int32 PA, int32 access)
{
switch ((PA >> 1) & 07) {

    case 00:                                            /* CSR */
        *data = kwxx_csr & KWXXCSR_RDMASK;              /* return CSR */
        break;

    case 01:                                            /* uptime MSBs */
	if (!kwxx_uptime_latched) {
	    double gtime = sim_gtime();
	    double ips = sim_timer_inst_per_sec();
	    kwxx_uptime = (gtime - kwxx_uptime_base) / ips * 25000.;
	    kwxx_uptime_msb = (kwxx_uptime >> 32) & BMASK;
	    kwxx_uptime_mid = (kwxx_uptime >> 16) & DMASK;
	    kwxx_uptime_lsb = kwxx_uptime & DMASK;
	    kwxx_uptime_latched = 1;                    /* latch count */
	    /* KW11-XX does not depend on any register read/write to clear */
	    /* the interupt, it just happens on the bus grant signal, but */
	    /* the interrupt routine reads the uptime. */
	    CLR_INT (KWXX);                                 /* clr intr */
	}
	*data = kwxx_uptime_msb;
        break;

    case 02:                                            /* uptime middle */
        *data = kwxx_uptime_mid;
        break;

    case 03:                                            /* uptime LSBs */
        *data = kwxx_uptime_lsb;
	kwxx_uptime_latched = 0;                        /* release latch */
        break;

    case 04:                                            /* interval MSBs */
        *data = 0;                                      /* are write-only */
        break;

    case 05:                                            /* interval LSBs */
        *data = 0;                                      /* are write-only */
        break;

    case 06:                                            /* meaurement ctr */
	kwxx_measure = (sim_gtime() - kwxx_measure_base) /
	    sim_timer_inst_per_sec() * 1000000.;
        *data = kwxx_measure & DMASK;
        break;

    case 07:                                            /* unused */
        *data = 0;
        break;
        }

return SCPE_OK;
}

t_stat kwxx_wr (int32 data, int32 PA, int32 access)
{
int32 old_csr = kwxx_csr;

switch ((PA >> 1) & 07) {

    case 00:                                            /* CSR */
        kwxx_csr = (kwxx_csr & ~KWXXCSR_WRMASK) | (data & KWXXCSR_WRMASK);
        CLR_INT (KWXX);                                 /* clr intr */
        break;

    case 01:                                            /* uptime MSBs */
	kwxx_uptime_base = sim_gtime();                 /* clear counter */
	break;

    case 02:                                            /* uptime mid */
    case 03:                                            /* uptime LSBs */
	break;                                          /* read-only */

    case 04:                                            /* interval MSBs */
        kwxx_interval_msb = data & BMASK;               /* store count */
        kwxx_csr = kwxx_csr | CSR_DONE;                 /* set done (per hwr) */
	sim_cancel (&kwxx_unit);                        /* cancel old interval*/
        CLR_INT (KWXX);                                 /* clr intr */
        break;

    case 05:                                            /* interval LSBs */
        kwxx_interval_lsb = data & DMASK;               /* store count */
	kwxx_csr = kwxx_csr & ~CSR_DONE;                /* clear done (per hwr)*/
	kwxx_interval = (kwxx_interval_msb << 16) + kwxx_interval_lsb;
	sim_cancel (&kwxx_unit);                        /* cancel old interval*/
	sim_activate_after (&kwxx_unit, kwxx_interval * 40);  /* start clock */
        break;

    case 06:                                            /* measurement ctr */
	kwxx_measure = 0;                               /* reset counter */
	kwxx_measure_base = sim_gtime();                /* time from now */
        break;

    case 07:                                            /* unused */
        break;
        }

return SCPE_OK;
}

/* Clock service */

t_stat kwxx_svc (UNIT *uptr)
{
    kwxx_csr = kwxx_csr | CSR_DONE;                     /* set done bit*/
    if (kwxx_csr & CSR_IE)                              /* if IE, set int */
        SET_INT (KWXX);
    return SCPE_OK;
}

/* Clock reset */

t_stat kwxx_reset (DEVICE *dptr)
{
    kwxx_csr = 0;                                       /* clear reg */
    kwxx_uptime_msb = 0;                                /* clear uptime */
    kwxx_uptime_mid = 0;
    kwxx_uptime_lsb = 0;
    kwxx_interval_msb = 0;                              /* clear interval */
    kwxx_interval_lsb = 0;
    kwxx_measure = 0;                                   /* clear measurement */
    kwxx_uptime_latched = 0;                            /* not latched */
    kwxx_uptime_base = sim_gtime();                     /* sim uptime when reset */
    kwxx_measure_base = kwxx_uptime_base;               /* sim uptime when reset */
    kwxx_uptime = 0;                                    /* latched uptime value */
    kwxx_interval = 0;                                  /* latched uptime value */
    CLR_INT (KWXX);                                     /* clear int */
    sim_cancel (&kwxx_unit);                            /* cancel */
    kwxx_unit.wait = 1;                                 /* reset delay */
    return auto_config (0, 0);
}

const char *kwxx_description (DEVICE *dptr)
{
    return "KW11-XX programmable real time clock at USC/ISI";
}
