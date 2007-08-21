#ifndef	_ADAPTER_H
#define	_ADAPTER_H
/*
 *	Definitions for high-level adapter interface.
 *
 *	Copyright (c) 2007 by Jefferson Ogata
 */

/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; see the file COPYING.  If not, write to the Free Software
 * Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */


#include	"mega.h"

#include	<stdio.h>

struct log_page_list *getDriveLogPage (struct physical_drive_info *d, uint8_t page);
struct physical_drive_info *getPhysicalDriveInfo (struct adapter_config *cf, uint16_t target, int fetch);
struct adapter_config *getAdapterConfig (int fd, uint8_t adapno, int sas);

#endif
