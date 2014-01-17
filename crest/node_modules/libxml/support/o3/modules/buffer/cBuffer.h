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
#ifndef O3_C_BUFFER_H
#define O3_C_BUFFER_H

namespace o3 {

struct cBuffer : cScr {
    o3_begin_class(cScr)
    o3_end_class()

    o3_glue_gen()

    static o3_ext("cO3") o3_get siScr buffer(iCtx* ctx)
    {
        o3_trace_scrfun("buffer");
        Var blob = ctx->value("buffer");

        if (blob.type() == Var::TYPE_VOID)
            blob = ctx->setValue("buffer", o3_new(cBuffer)());
        return blob.toScr();
    }

    o3_fun Buf __self__(iCtx* ctx)
    {
        o3_trace_scrfun("__self__");

        return Buf(ctx);
    }

    o3_fun Buf __self__(iCtx* ctx, size_t n)
    {
        o3_trace_scrfun("__self__");
        Buf buf(n, ctx);

        buf.appendPattern((uint8_t) 0, n);
        return buf;
    }

    o3_fun Buf __self__(const Str& str)
    {
        o3_trace_scrfun("__self__");

        return Buf(str);
    }

    o3_fun Buf fromString(const Str& str)
    {
        o3_trace_scrfun("fromString");

        return Buf(str);
    }

    o3_fun Buf fromHex(const Str& str)
    {
        o3_trace_scrfun("fromHex");

        return Buf::fromHex(str.ptr(), str.alloc());
    }

    o3_fun Buf fromBase64(const Str& str)
    {
        o3_trace_scrfun("fromBase64");

        return Buf::fromBase64(str.ptr(), str.alloc());
    }

    o3_fun Str toString()
    {
        o3_trace_scrfun("toString");
        return Str(m_buf);
    }

    o3_fun Str toHex(o3_tgt iScr* tgt)
    {
        o3_trace_scrfun("toHex");
        return Str::fromHex(m_buf.ptr(), m_buf.size());
    }

    o3_fun Str toBase64(o3_tgt iScr* tgt)
    {
        o3_trace_scrfun("toBase64");
        return Str::fromBase64(m_buf.ptr(), m_buf.size());
    }
    
    o3_fun void replace(iBuf* orig, iBuf* rep) 
    {
        o3_trace_scrfun("replace");
        Buf& orig_buf = orig->unwrap();
        Buf& replace_buf = rep->unwrap();

        if (!orig && !rep)
            return;

        m_buf.findAndReplaceAll(orig_buf.ptr(), orig_buf.size(),
            replace_buf.ptr(), replace_buf.size());
    }

    o3_fun void replace(const char* orig, const char* rep)
    {    
        o3_trace_scrfun("replace");    

        m_buf.findAndReplaceAll(orig, strLen(orig),
            rep, strLen(rep));
    }

    o3_fun void replaceUtf16(const wchar_t* orig, const wchar_t* rep) 
    {
        o3_trace_scrfun("replaceUtf16");

        m_buf.findAndReplaceAll(orig, strLen(orig)*sizeof(wchar_t),
            rep, strLen(rep)*sizeof(wchar_t));
    }
};    
}

#endif // O3_C_BUFFER_H
