/* pdp11_wwvb.c: Simulation of DL11 connected to Spectracom WWVB clock
*/

#include "pdp11_defs.h"
#include "sim_tmxr.h"
#include <time.h>

#define WWVBICSR_IMP      (CSR_DONE + CSR_IE)           /* terminal input */
#define WWVBICSR_RW       (CSR_IE)
#define WWVBOCSR_IMP      (CSR_DONE + CSR_IE)           /* terminal output */
#define WWVBOCSR_RW       (CSR_IE)

extern int32 int_req[IPL_HLVL];
extern uint32 cpu_type;

int32 wwvbi_csr = 0;                                    /* control/status */
uint32 wwvbi_buftime;                                   /* time input character arrived */
int32 wwvbo_csr = 0;                                    /* control/status */
char time_string[30];                                   /* time as from Spectracom */
char *time_ptr;

t_stat wwvbi_rd (int32 *data, int32 PA, int32 access);
t_stat wwvbi_wr (int32 data, int32 PA, int32 access);
t_stat wwvbi_svc (UNIT *uptr);
t_stat wwvbi_reset (DEVICE *dptr);
t_stat wwvbo_rd (int32 *data, int32 PA, int32 access);
t_stat wwvbo_wr (int32 data, int32 PA, int32 access);
t_stat wwvbo_svc (UNIT *uptr);
t_stat wwvbo_reset (DEVICE *dptr);

/* WWVBI data structures

   wwvbi_dev      WWVBI device descriptor
   wwvbi_unit     WWVBI unit descriptor
   wwvbi_reg      WWVBI register list
*/

#define VEC_WWVBI       0360
#define VEC_WWVBO       0364

#define IOLN_WWVBI      004

DIB wwvbi_dib = {
    IOBA_AUTO, IOLN_WWVBI, &wwvbi_rd, &wwvbi_wr,
    1, IVCL (WWVBI), VEC_AUTO, { NULL }, IOLN_WWVBI,
    };

UNIT wwvbi_unit = { UDATA (&wwvbi_svc, UNIT_IDLE, 0), TMLN_SPD_9600_BPS };

REG wwvbi_reg[] = {
    { HRDATAD (BUF,       wwvbi_unit.buf,          8, "last data item processed") },
    { HRDATAD (CSR,            wwvbi_csr,         16, "control/status register") },
    { FLDATAD (INT,         IREQ (WWVBI),  INT_V_WWVBI, "interrupt pending flag") },
    { FLDATAD (DONE,           wwvbi_csr, CSR_V_DONE, "device done flag (CSR<7>)") },
    { FLDATAD (ERR,            wwvbi_csr,  CSR_V_ERR, "device error flag (CSR<15>)") },
    { FLDATAD (IE,             wwvbi_csr,   CSR_V_IE, "interrupt enable flag (CSR<6>)") },
    { DRDATAD (POS,       wwvbi_unit.pos,   T_ADDR_W, "number of characters input"), PV_LEFT },
    { DRDATAD (TIME,     wwvbi_unit.wait,         24, "input polling interval"), PV_LEFT },
    { NULL }
    };

MTAB wwvbi_mod[] = {
    { MTAB_XTD|MTAB_VDV, 4, "ADDRESS", "ADDRESS",
      &set_addr, &show_addr, NULL, "Bus address" },
    { MTAB_XTD|MTAB_VDV, 0, "VECTOR", "VECTOR",
      &set_vec, &show_vec, NULL, "Interrupt vector" },
    { 0 }
    };

DEVICE wwvbi_dev = {
    "WWVBI", &wwvbi_unit, wwvbi_reg, wwvbi_mod,
    1, 10, 31, 1, 8, 8,
    NULL, NULL, &wwvbi_reset,
    NULL, NULL, NULL,
    &wwvbi_dib, DEV_DISABLE | DEV_UBUS | DEV_QBUS
    };

/* WWVBO data structures

   wwvbo_dev      WWVBO device descriptor
   wwvbo_unit     WWVBO unit descriptor
   wwvbo_reg      WWVBO register list
*/

#define IOLN_WWVBO      004

DIB wwvbo_dib = {
    IOBA_AUTO, IOLN_WWVBO, &wwvbo_rd, &wwvbo_wr,
    1, IVCL (WWVBO), VEC_AUTO, { NULL }, IOLN_WWVBO,
    };

UNIT wwvbo_unit = { UDATA (&wwvbo_svc, TT_MODE_7P, 0), SERIAL_OUT_WAIT };

REG wwvbo_reg[] = {
    { ORDATA (BUF, wwvbo_unit.buf, 8) },
    { ORDATA (CSR, wwvbo_csr, 16) },
    { FLDATA (INT, IREQ (WWVBO), INT_V_WWVBO) },
    { FLDATA (ERR, wwvbo_csr, CSR_V_ERR) },
    { FLDATA (DONE, wwvbo_csr, CSR_V_DONE) },
    { FLDATA (IE, wwvbo_csr, CSR_V_IE) },
    { DRDATA (POS, wwvbo_unit.pos, T_ADDR_W), PV_LEFT },
    { DRDATA (TIME, wwvbo_unit.wait, 24), PV_LEFT },
    { NULL }
    };

MTAB wwvbo_mod[] = {
    { MTAB_XTD|MTAB_VDV, 4, "ADDRESS", "ADDRESS", 
      &set_addr, &show_addr, NULL, "Bus address" },
    { MTAB_XTD|MTAB_VDV, 0, "VECTOR", "VECTOR",
      &set_vec, &show_vec, NULL, "Interrupt vector" },
    { 0 }
    };

DEVICE wwvbo_dev = {
    "WWVBO", &wwvbo_unit, wwvbo_reg, wwvbo_mod,
    1, 10, 31, 1, 8, 8,
    NULL, NULL, &wwvbo_reset,
    NULL, NULL, NULL,
    &wwvbo_dib, DEV_DISABLE | DEV_UBUS | DEV_QBUS
    };

/* Terminal input address routines */
// Spectracom 8170 time sting format (22 ASCII printing characters):
// <cr><lf>i  ddd hh:mm:ss  TZ=zz<cr><lf>
// on-time = first <cr>
// i = synchronization flag (' ' = in synch, '?' = out synch)
// hh:mm:ss = hours, minutes, seconds

t_stat wwvbi_rd (int32 *data, int32 PA, int32 access)
{
switch ((PA >> 1) & 01) {                               /* decode PA<1> */

    case 00:                                            /* wwvbi csr */
        *data = wwvbi_csr & WWVBICSR_IMP;
        return SCPE_OK;

    case 01:                                            /* wwvbi buf */
        wwvbi_csr = wwvbi_csr & ~CSR_DONE;
        CLR_INT (WWVBI);
        *data = *time_ptr;
	if (*(time_ptr+1) != '\0') {
	    ++time_ptr;
	    ++wwvbi_unit.pos;
	    wwvbi_csr = wwvbi_csr | CSR_DONE;
	    if (wwvbi_csr & CSR_IE)
		SET_INT (WWVBI);
	}
        return SCPE_OK;
        }                                               /* end switch PA */

return SCPE_NXM;
}

t_stat wwvbi_wr (int32 data, int32 PA, int32 access)
{
switch ((PA >> 1) & 01) {                               /* decode PA<1> */

    case 00:                                            /* wwvbi csr */
        if (PA & 1)
            return SCPE_OK;
        if ((data & CSR_IE) == 0)
            CLR_INT (WWVBI);
        else if ((wwvbi_csr & (CSR_DONE + CSR_IE)) == CSR_DONE)
            SET_INT (WWVBI);
        wwvbi_csr = (wwvbi_csr & ~WWVBICSR_RW) | (data & WWVBICSR_RW);
        return SCPE_OK;

    case 01:                                            /* wwvbi buf */
        return SCPE_OK;
        }                                               /* end switch PA */

return SCPE_NXM;
}

/* Terminal input service */

t_stat wwvbi_svc (UNIT *uptr)
{
return SCPE_OK;
}

/* Terminal input reset */

t_stat wwvbi_reset (DEVICE *dptr)
{
wwvbi_unit.buf = 0;
wwvbi_csr = 0;
CLR_INT (WWVBI);
return SCPE_OK;
}

/* Terminal output address routines */

t_stat wwvbo_rd (int32 *data, int32 PA, int32 access)
{
switch ((PA >> 1) & 01) {                               /* decode PA<1> */

    case 00:                                            /* wwvbo csr */
        *data = wwvbo_csr & WWVBOCSR_IMP;
        return SCPE_OK;

    case 01:                                            /* wwvbo buf */
        *data = wwvbo_unit.buf;
        return SCPE_OK;
        }                                               /* end switch PA */

return SCPE_NXM;
}

t_stat wwvbo_wr (int32 data, int32 PA, int32 access)
{
switch ((PA >> 1) & 01) {                               /* decode PA<1> */

    case 00:                                            /* wwvbo csr */
        if (PA & 1)
            return SCPE_OK;
        if ((data & CSR_IE) == 0)
            CLR_INT (WWVBO);
        else if ((wwvbo_csr & (CSR_DONE + CSR_IE)) == CSR_DONE)
            SET_INT (WWVBO);
        wwvbo_csr = (wwvbo_csr & ~WWVBOCSR_RW) | (data & WWVBOCSR_RW);
        return SCPE_OK;

    case 01:                                            /* wwvbo buf */
        if ((PA & 1) == 0)
            wwvbo_unit.buf = data & 0377;
	if ((data & 0377) == 'T') {
	    time_t now = time((time_t*)0);
	    struct tm tm;
	    gmtime_r(&now, &tm);
	    strftime(time_string, sizeof time_string,
		     "\r\n   %j %T  TZ=00\r\n", &tm);
	    time_ptr = time_string;
	    wwvbi_csr = wwvbi_csr | CSR_DONE;
	    if (wwvbi_csr & CSR_IE)
		SET_INT (WWVBI);
	}
        wwvbo_csr = wwvbo_csr & ~CSR_DONE;
        CLR_INT (WWVBO);
	wwvbo_csr = wwvbo_csr | CSR_DONE;
	if (wwvbo_csr & CSR_IE)
	    SET_INT (WWVBO);
	++wwvbo_unit.pos;
        return SCPE_OK;
        }                                               /* end switch PA */

return SCPE_NXM;
}

/* Terminal output service */

t_stat wwvbo_svc (UNIT *uptr)
{
return SCPE_OK;
}

/* Terminal output reset */

t_stat wwvbo_reset (DEVICE *dptr)
{
time_ptr = time_string;
*time_ptr = '\0';
*(time_ptr+1) = '\0';
wwvbo_unit.buf = 0;
wwvbo_csr = CSR_DONE;
CLR_INT (WWVBO);
sim_cancel (&wwvbo_unit);                                 /* deactivate unit */
return SCPE_OK;
}
