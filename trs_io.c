/*
 * Copyright (C) 1992 Clarendon Hill Software.
 *
 * Permission is granted to any individual or institution to use, copy,
 * or redistribute this software, provided this copyright notice is retained. 
 *
 * This software is provided "as is" without any expressed or implied
 * warranty.  If this software brings on any sort of damage -- physical,
 * monetary, emotional, or brain -- too bad.  You've got no one to blame
 * but yourself. 
 *
 * The software may be modified for your own purposes, but modified versions
 * must retain this notice.
 */

/*
   Modified by Timothy Mann, 1996
   Last modified on Tue Sep 23 14:53:42 PDT 1997 by mann
*/

#include "z80.h"
#include "trs.h"
#include "trs_imp_exp.h"
#include "trs_disk.h"

static int modesel = 0;

/*ARGSUSED*/
void z80_out(port, value)
    int port, value;
{
    if (trs_model == 1) {
	switch (port) {
	  case IMPEXP_CMD: /* 0xD0 */
	    trs_impexp_cmd_write(value);
	    break;
	  case IMPEXP_DATA: /* 0xD1 */
	    trs_impexp_data_write(value);
	    break;
	  case 0xFF:
	    /* screen mode select is on D3 line */
	    modesel = (value >> 3) & 1;
	    trs_screen_mode_select(modesel ? EXPANDED : NORMAL);
	    /* do cassette emulation */
	    trs_cassette_out(value & 0x7);
	    break;
	  default:
	    break;
	}
    } else {
	switch (port) {
	  case IMPEXP_CMD: /* 0xD0 */
	    trs_impexp_cmd_write(value);
	    break;
	  case IMPEXP_DATA: /* 0xD1 */
	    trs_impexp_data_write(value);
	    break;
	  case 0xE0:
	    trs_interrupt_mask_write(value);
	    break;
	  case TRSDISK3_INTERRUPT: /* 0xE4 */
	  case 0xE5:
	  case 0xE6:
	  case 0xE7:
	    trs_nmi_mask_write(value);
	    break;
	  case 0xEC:
	  case 0xED:
	  case 0xEE:
	  case 0xEF:
	    /* screen mode select is on D2 line */
	    modesel = (value >> 2) & 1;
	    trs_screen_mode_select(modesel ? EXPANDED : NORMAL);
	    /*!! more to do*/
	    break;
	  case TRSDISK3_COMMAND: /* 0xF0 */
	    trs_disk_command_write(value);
	    break;
	  case TRSDISK3_TRACK: /* 0xF1 */
	    trs_disk_track_write(value);
	    break;
	  case TRSDISK3_SECTOR: /* 0xF2 */
	    trs_disk_sector_write(value);
	    break;
	  case TRSDISK3_DATA: /* 0xF3 */
	    trs_disk_data_write(value);
	    break;
	  case TRSDISK3_SELECT: /* 0xF4 */
	  case 0xF5:
	  case 0xF6:
	  case 0xF7:
	    trs_disk_select_write(value);
	    break;
	  case 0xF8:
	  case 0xF9:
	  case 0xFA:
	  case 0xFB:
	    trs_printer_write(value);
	    break;
	  case 0xFC:
	  case 0xFD:
	  case 0xFE:
	  case 0xFF:
	    /* do cassette emulation (future) */
	    /*!! trs_cassette_out(value);*/
	    break;
	  default:
	    break;
	}
    }
    return;
}

/*ARGSUSED*/
int z80_in(port)
    int port;
{
    if (trs_model == 1) {
	switch (port) {
	  case IMPEXP_STATUS: /* 0xD0 */
	    return trs_impexp_status_read();
	  case IMPEXP_DATA: /* 0xD1 */
	    return trs_impexp_data_read();
	  case 0xFF:
	    return trs_cassette_in(modesel);
	  default:
	    break;
	}
    } else {
	switch (port) {
	  case IMPEXP_STATUS: /* 0xD0 */
	    return trs_impexp_status_read();
	  case IMPEXP_DATA: /* 0xD1 */
	    return trs_impexp_data_read();
	  case 0xE0:
	    return trs_interrupt_latch_read();
	  case 0xEC:
	    trs_timer_interrupt(0); /* acknowledge */
	    return 0xFF;
	  case TRSDISK3_INTERRUPT: /* 0xE4 */
	    return trs_nmi_latch_read();
	  case TRSDISK3_STATUS: /* 0xF0 */
	    return trs_disk_status_read();
	  case TRSDISK3_TRACK: /* 0xF1 */
	    return trs_disk_track_read();
	  case TRSDISK3_SECTOR: /* 0xF2 */
	    return trs_disk_sector_read();
	  case TRSDISK3_DATA: /* 0xF3 */
	    return trs_disk_data_read();
	  case 0xF8:
	    return trs_printer_read();
	  case 0xFF:
	    /* !!Add cassette bits, etc., later */
	    return modesel << 2;
	  default:
	    break;
	}
    }
    /* other ports -- unmapped */
    return 0xFF;
}

