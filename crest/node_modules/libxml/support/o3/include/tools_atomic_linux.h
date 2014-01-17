/*
 * Copyright (C) 2010 Ajax.org BV
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option) any later
 * version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this library; if not, write to the Free Software Foundation, Inc., 51
 * Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
#ifndef O3_TOOLS_ATOMIC_LINUX_H
#define O3_TOOLS_ATOMIC_LINUX_H

namespace o3 {

inline int atomicTas(volatile int& x)
{
    o3_trace_tools("atomicTas");
#ifdef O3_SOLARIS
	int r = x; x = 1;
	return r;
#else
    return __sync_lock_test_and_set(&x, 1);
#endif
}

inline int atomicInc(volatile int& x)
{
    o3_trace_tools("atomicInc");
#ifdef O3_SOLARIS
	return ++x;
#else
    return __sync_add_and_fetch(&x, 1);
#endif
}

inline int atomicDec(volatile int& x)
{
    o3_trace_tools("atomicDec");
#ifdef O3_SOLARIS
	return --x;
#else	
    return __sync_sub_and_fetch(&x, 1);
#endif
}

}

#endif // O3_TOOLS_ATOMIC_LINUX_H
