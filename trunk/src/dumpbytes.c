/*
 *	Hexadecimal dump for displaying ioctl data structures.
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


#include	<stdio.h>
#include	<string.h>
#include	<ctype.h>
#include	<sys/types.h>


void dumpbytes (FILE *f, void *z, size_t len, void *addr, char *prefix)
{
    off_t		k;
    int			j = 15;
    char		abuf[20];
    unsigned char	*buf = z;

    for (k = 0; k < len; ++k)
    {
	unsigned char	c = buf[k];

	j = k % 16;
	if (j == 0)
	{
	    if (prefix)
		fprintf (f, "    %s+%04lx:\t", prefix, k);
	    else
		fprintf (f, "    %08lx:\t", ((unsigned long) addr) + k);
	    memset (abuf, ' ', sizeof abuf - 1);
	    abuf[sizeof abuf - 1] = '\0';
	}
	else
	{
	    if (j % 4 == 0)
		putc (' ', f);
	    if (j % 8 == 0)
		putc (' ', f);
	}
	fprintf (f, "%02x", c);
	abuf[j + j / 4] = isprint (c) ? c : '.';
	if (j == 15)
	{
	    fprintf (f, "    %s\n", abuf);
	}
    }
    if (j != 15)
    {
	for ( ; j < 15; ++j)
	{
	    if (j % 4 == 0)
		putc (' ', f);
	    if (j % 8 == 0)
		putc (' ', f);
	    putc (' ', f);
	    putc (' ', f);
	}
	fprintf (f, "    %s\n", abuf);
    }
}


