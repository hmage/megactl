/*
 *	Little string-trimming function.
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

#include	<sys/types.h>
#include	<string.h>
#include	<ctype.h>

#include	"ntrim.h"

void ntrim (char *s)
{
    char		*t;
    size_t		len = strlen (s);

    for (t = s + len - 1; (t >= s) && isspace (*t); --t)
	*t = '\0';
    for (t = s; isspace (*t); ++t)
	;
    if (t > s)
    {
	for ( ; *t; ++t, ++s)
	    *s = *t;
	*s = *t;
    }
}

