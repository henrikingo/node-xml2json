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
#ifndef O3_DEBUG_H
#define O3_DEBUG_H

#ifdef O3_TRACE
#define o3_trace struct
#else
#define o3_trace struct
#endif // O3_TRACE

#if (O3_TRACE <= 0)
#define o3_trace0 o3_trace
#else
#define o3_trace0 struct
#endif

#if (O3_TRACE <= 1)
#define o3_trace1 o3_trace
#else
#define o3_trace1 struct
#endif

#if (O3_TRACE <= 2)
#define o3_trace2 o3_trace
#else
#define o3_trace2 struct
#endif

#if (O3_TRACE <= 3)
#define o3_trace3 o3_trace
#else
#define o3_trace3 struct
#endif

#define o3_trace_scrfun(name) 
#define o3_trace_tools(name)
#define o3_trace_combase(name)
#define o3_trace_comglue(name)
#define o3_trace_containers(name)
#define o3_trace_hostglue(name)
#define o3_trace_sys(name)

#define o3_trace_no_trace

//#define O3_LOGFILE
//#define o3_trace_scrfun(name) O3Trace trace(name, __FILE__, __LINE__)
//#define o3_trace_tools(name) O3Trace trace(name, __FILE__, __LINE__)
//#define o3_trace_combase(name) O3Trace trace(name, __FILE__, __LINE__)
//#define o3_trace_comglue(name) O3Trace trace(name, __FILE__, __LINE__)
//#define o3_trace_containers(name) O3Trace trace(name, __FILE__, __LINE__)
//#define o3_trace_hostglue(name) O3Trace trace(name, __FILE__, __LINE__)
//#define o3_trace_sys(name) O3Trace trace(name, __FILE__, __LINE__)


#ifdef O3_ASSERT
#define o3_assert(pred) ((void) ((pred) ? 0 \
                                        : (o3::o3assert(#pred, \
                                                      __FILE__, \
                                                      __LINE__), \
                                          0)))
#else
#define o3_assert(pred) ((void) 0)
#endif // O3_ASSERT

#ifdef O3_LOG
#define o3_log(...) o3::log(__VA_ARGS__)
#else
#define o3_log(...) ((void) 0)
#endif // O3_LOG

#ifdef O3_TRACE

namespace o3 {

inline void traceEnter(const char* fun, const char* file, int line);

inline void traceLeave();

inline void o3assert(const char* pred, const char* file, int line);

inline void log(const char* format, ...);

struct O3Trace {
	O3Trace(const char* fun, const char* file, int line) 
	{
		traceEnter(fun,file,line);
	}

	~O3Trace() 
	{
		traceLeave();
	}
};
}
#endif

#endif // O3_DEBUG_H
