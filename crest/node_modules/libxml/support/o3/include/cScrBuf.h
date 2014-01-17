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
#ifndef O3_C_SCR_BUF_H
#define O3_C_SCR_BUF_H

namespace o3 {

o3_cls(cScrBuf);

struct cScrBuf : cScr, iBuf {
    Buf m_buf;

    cScrBuf(const Buf& buf = Buf()) : m_buf(buf)
    {
        o3_trace_scrfun("cScrBuf");
    }

    o3_begin_class(cScr)
        o3_add_iface(iBuf)
    o3_end_class()

	o3_glue_gen()

	o3_fun Str toString(const char* encoding="utf8", int start=0, int end=-1)
	{
		o3_trace3 trace;

		if (end == -1 && start == 0) {							
			if (strEquals(encoding, "base64"))
				return toBase64();
			if (strEquals(encoding, "ascii"))
				return toHex();

			return m_buf;
		}			

		if (end==-1)
			end = m_buf.size();

		if (start > end)
			return Str();

		if (end == -1 && start == 0) {							
			if (strEquals(encoding, "base64")){
				Str ret(toBase64());
				if (start > ret.size())
					return Str();
				return Str(ret.ptr()+start, end-start);
			}
			if (strEquals(encoding, "ascii")) {
				WStr ret = Str(m_buf);
				if(start > (int)ret.size())
					return Str();
				ret = WStr(ret.ptr()+start, end-start);
				return Str(ret);
			}

			return m_buf;
		}	
	}

    o3_get int length()
    {
        o3_trace_scrfun("length");

        return (int) m_buf.size();
    }

	o3_fun size_t byteLength(const char* string, const char* encoding="utf8")
	{
		o3_trace_scrfun("length");

		return strFromStr(0, string, strLen(string));
	}

    //o3_set size_t setLength(size_t size)
    //{
    //    o3_trace3 trace;

    //    m_buf.resize(size);
    //    return size;
    //}

	//o3_fun void append(const Buf& other)
	//{
	//	m_buf.append(other.ptr(), other.size());
	//}

    o3_set size_t setLength(size_t size)
    {
        o3_trace_scrfun("setLength");

        m_buf.resize(size);
        return size;
    }

	o3_fun void append(const Buf& other)
	{
		o3_trace_scrfun("append");
		m_buf.append(other.ptr(), other.size());
	}


	//o3_fun Buf slice(size_t start, size_t end)
	//{
	//	size_t size = m_buf.size();
	//	if(start<0 || end<0 || start>size || end>size || start>end)
	//		return Buf();


	//	return Buf(((int8_t*)m_buf.ptr())+start, end-start);
	//}


    o3_fun bool __query__(size_t index)
    {
        o3_trace_scrfun("__query__");

        return (size_t) index < m_buf.size();
    }

    o3_fun int  __getter__(size_t index)
    {
        o3_trace_scrfun("__getter__");

        if ((size_t) index < m_buf.size())
            return ((uint8_t*) m_buf.ptr())[index];
        return 0; 
    }

    o3_fun int __setter__(size_t index, int b)
    {
        o3_trace_scrfun("__setter__");

        if ((size_t) index < m_buf.size())
            return (int) (((uint8_t*) m_buf.ptr())[index] = (uint8_t)b);
        return 0; 
    }

    Buf& unwrap()
    {
        o3_trace_scrfun("unwrap");

        return m_buf;
    }

	static o3_ext("cO3") o3_fun Buf BufFromString(const Str& str)
	{
		o3_trace3 trace;

		return Buf(str);
	}

	static o3_ext("cO3") o3_fun Buf BufFromHex(const Str& str)
	{
		o3_trace3 trace;

		return Buf::fromHex(str.ptr(), str.alloc());
	}                         

	static o3_ext("cO3") o3_fun Buf BufFromBase64(const Str& str)
	{
		o3_trace3 trace;

		return Buf::fromBase64(str.ptr(), str.alloc());
	}

	o3_fun Str toHex()
	{
		o3_trace3 trace;

		return Str::fromHex(m_buf.ptr(), m_buf.size());
	}

	o3_fun Str toBase64()
	{
		o3_trace3 trace;

		return Str::fromBase64(m_buf.ptr(), m_buf.size());
	}

	static o3_ext("cO3") o3_fun siScr ScrBuf(size_t size) 
	{
		cScrBuf* ret = o3_new(cScrBuf)(Buf(size));
		memSet(ret->m_buf.ptr(), 0, size);
		ret->m_buf.resize(size);
		return siScr(ret);
	}

	o3_fun void write(const char* string, int offset=0, const char* encoding="utf8")
	{
		size_t l = min(strLen(string), m_buf.size() - offset);
		memCopy((uint8_t*)m_buf.ptr()+offset,string,l);
	}

	o3_fun void copy(iBuf* target, int targetStart=0, int sourceStart=0, int sourceEnd=-1)
	{
		targetStart = max(targetStart, 0);
		sourceStart = max(sourceEnd, 0);
		sourceEnd = (sourceEnd<0 || sourceEnd>target->unwrap().size()) 
			? target->unwrap().size() : sourceEnd;

		int tl = max((int)target->unwrap().size()-targetStart, 0);
		int sl = max(min(sourceEnd-sourceStart, (int)m_buf.size()-sourceStart),0);
		int l = min(tl,sl);

		memCopy((uint8_t*)m_buf.ptr() + sourceStart, 
			(uint8_t*)target->unwrap().ptr() + targetStart, l);
	}

	o3_fun siScr slice(int start, int end)
	{
		start = max(0,start);
		end = min(end, (int)m_buf.size());
		return o3_new(cScrBuf)(Buf((uint8_t*)m_buf.ptr()+start, end-start));
	}

	o3_fun size_t find(const char* text, int from = 0)
	{
		return m_buf.find((size_t) max(0,from),text, strLen(text));
	}
};

}

#endif // O3_C_SCR_BUF_H
