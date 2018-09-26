/* pdp11_umc.c: Simulation of dummy UMC-Z80 devices
*/

#include "pdp11_defs.h"
#include "sim_tmxr.h"
#include <time.h>

#define UMCCSR_IMP      (CSR_DONE + CSR_IE)           /* terminal input */
#define UMCCSR_RW       (CSR_IE)

extern int32 int_req[IPL_HLVL];
extern uint32 cpu_type;

int32 umc_csr = 0;                                    /* control/status */

t_stat umc_rd (int32 *data, int32 PA, int32 access);
t_stat umc_wr (int32 data, int32 PA, int32 access);
t_stat umc_svc (UNIT *uptr);
t_stat umc_reset (DEVICE *dptr);

/* UMC data structures

   umc_dev      UMC device descriptor
   umc_unit     UMC unit descriptor
   umc_reg      UMC register list
*/

#define IOLN_UMC      0400

DIB umc_dib = {
    IOBA_AUTO, IOLN_UMC, &umc_rd, &umc_wr,
    0, 0, 0, { NULL }, IOLN_UMC,
    };

UNIT umc_unit = { UDATA (NULL, 0, 0) };

REG umc_reg[] = {
    { HRDATAD (CSR,            umc_csr,         16, "control/status register") },
    { NULL }
    };

MTAB umc_mod[] = {
    { MTAB_XTD|MTAB_VDV, 0100, "ADDRESS", "ADDRESS",
      &set_addr, &show_addr, NULL, "Bus address" },
    { 0 }
    };

DEVICE umc_dev = {
    "UMC", &umc_unit, umc_reg, umc_mod,
    1, 8, 16, 2, 8, 16,
    NULL, NULL, &umc_reset,
    NULL, NULL, NULL,
    &umc_dib, DEV_DISABLE | DEV_UBUS | DEV_QBUS
    };

t_stat umc_rd (int32 *data, int32 PA, int32 access)
{
        return SCPE_OK;
}

t_stat umc_wr (int32 data, int32 PA, int32 access)
{
        return SCPE_OK;
}

t_stat umc_reset (DEVICE *dptr)
{
return SCPE_OK;
}
