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
#ifndef O3_C_JS1_POSIX_H
#define O3_C_JS1_POSIX_H

// #include <node/deps/v8/include/v8.h>
#ifdef O3_V8
#include <v8/v8.h>
#endif

using namespace v8;

namespace o3 {

o3_iid(iScrObj, 0xf3afb7c0, 
	0x2bf3, 
	0x4b77, 
	0x8e, 0x63, 0xd, 0xec, 0x25, 0xc8, 0x9e, 0x96);

struct iScrObj : iUnk {
	virtual Handle<Object> unwrap() = 0;
};


struct cJs : cJsBase {
    struct cScrObj : cUnk, iScr, iScrObj {
        cJs* m_pthis;
        Persistent<Object> m_object;

        cScrObj(cJs* pthis, Handle<Object> object) : m_pthis(pthis),
            m_object(Persistent<Object>::New(object))
        {
        }

        ~cScrObj()
        {
            o3_trace_hostglue("~cScrObj");
            m_object.Dispose();
        }

        o3_begin_class(cUnk)
            o3_add_iface(iScr)
			o3_add_iface(iScrObj)
        o3_end_class()        

        virtual int enumerate(iCtx* ctx, int index)
        {
            o3_trace_hostglue("enumerate");
            return -1;
        }

        virtual Str name(iCtx* ctx, int index)
        {
            o3_trace_hostglue("name");
            return Str();
        }

        virtual int resolve(iCtx* ctx, const char* name, bool set)
        {
            o3_trace_hostglue("resolve");
            if (strEquals(name, "__self__"))
                return 0;
            return -1;
        }

        virtual siEx invoke(iCtx* ctx, Access access, int index, int argc,
                            const Var* argv, Var* rval)
        {
            o3_trace_hostglue("invoke");
            if (index == 0) {
                Local<Object> object = Local<Object>::New(m_object);
                Local<Function> function = Local<Function>::Cast(object);
                tVec<Handle<Value> > args;

                for (int i = 0; i < argc; ++i)
                    args.push(m_pthis->toValue(argv[i]));
                function->Call(object, argc, args);
            }
            return 0;
        }

		virtual Str className(){
			o3_trace_scrfun("className");
			return "ScrObj";
		}

		virtual Handle<Object> unwrap(){
			o3_trace_scrfun("unwrap");
			HandleScope handle_scope;
			return handle_scope.Close(m_object);
		}
    };

    static void* cast(Local<Value> value)
    {
        o3_trace_hostglue("cast");
        return Local<External>::Cast(value)->Value();
    }

#ifdef O3_V8_GLUE	
	static Handle<Value> indexedGetter(uint32_t index,
		const AccessorInfo& info)
	{
		o3_trace_hostglue("indexedGetter");
		HandleScope scope;
		cJs* pthis = (cJs*) cast(info.Data());
		cScr* scr = (cScr*)(iScr*) cast(info.Holder()->GetInternalField(1));
		siEx ex;

		Var rval = scr->__getter__(pthis, index, &ex);
		if (ex)
			ThrowException(String::New(ex->message()));

		return scope.Close(pthis->toValue(rval));
	}

	static Handle<Value> indexedSetter(uint32_t index, Local<Value> value,
		const AccessorInfo& info)
	{
		o3_trace_hostglue("indexedSetter");
		HandleScope scope;
		cJs* pthis = (cJs*) cast(info.Data());
		cScr* scr = (cScr*)(iScr*) cast(info.Holder()->GetInternalField(1));
		siEx ex;

		Var arg = pthis->toVar(value);
		Var rval = scr->__setter__(index, arg, &ex);
		if (ex)
			ThrowException(String::New(ex->message()));

		return scope.Close(pthis->toValue(rval));
	}

	static Handle<Integer> indexedQuery(uint32_t index,
		const AccessorInfo& info)
	{
		o3_trace_hostglue("indexedQuery");
		HandleScope scope;
		cJs* pthis = (cJs*) cast(info.Data());
		cScr* scr = (cScr*)(iScr*) cast(info.Holder()->GetInternalField(1));

		bool rval;
		rval = scr->__query__(pthis, index);

		return rval ? v8::Integer::New(v8::None)
			: v8::Handle<v8::Integer>();
	}

	static Handle<Boolean> indexedDeleter(uint32_t index,
		const AccessorInfo& info)
	{
		o3_trace_hostglue("indexedDeleter");
		HandleScope scope;
		cJs* pthis = (cJs*) cast(info.Data());
		cScr* scr = (cScr*)(iScr*) cast(info.Holder()->GetInternalField(1));
		siEx ex;
		bool rval = scr->__deleter__(index, &ex);	
		
		if (ex) {
			ThrowException(String::New(ex->message()));
			return Handle<Boolean>();
		}
		return Boolean::New(rval);
	}

	static Handle<Array> indexedEnumerator(const AccessorInfo& info)
	{
		o3_trace_hostglue("indexedEnumerator");
		cJs* pthis = (cJs*) cast(info.Data());
		iScr* scr = (iScr*) cast(info.Holder()->GetInternalField(0));
		siCtx ctx(pthis);
		Var arg;
		Var rval;
		Handle<Array> names_out;
		size_t length;
		int key;
		int next_id = -1;
		tVec<Str> names;


		length = names.size();
		names_out = Array::New(length);
		for (size_t i=0; i<length; i++) {
			names_out->Set(Number::New(i),
				String::New(names[i]));
		}

		return names_out;
	}

#else
    static Handle<Value> invocation(const Arguments& args)
    {
        o3_trace_hostglue("invocation");
        cJs* pthis = (cJs*) cast(args.Data());
        iScr* scr = (iScr*) cast(args.This()->GetInternalField(0));
        int self = scr->resolve(pthis, "__self__");
        int argc = args.Length();
        tVec<Var> argv(argc);
        Var rval((iAlloc*) pthis);

        if (self < 0)
            return Handle<Value>();
        for (int i = 0; i < argc; ++i)
            argv.push(pthis->toVar(args[i]));
        if (siEx ex = scr->invoke(pthis, ACCESS_CALL, self, argc, argv, &rval))
            return ThrowException(String::New(ex->message()));
        return pthis->toValue(rval);
    }

    static Handle<Value> namedGetter(Local<String> property,
                                     const AccessorInfo& info)
    {
        o3_trace_hostglue("namedGetter");
        cJs* pthis = (cJs*) cast(info.Data());
        iScr* scr = (iScr*) cast(info.Holder()->GetInternalField(0));
        Str name = *String::Utf8Value(property);
        int index = scr->resolve(pthis, name);
        Var rval((iAlloc*) pthis);

		if (index < 0) {
			Handle<Object> proto = pthis->prototype(scr);
			if (strEquals(name.ptr(), "prototype"))
				return proto;						
			if (proto.IsEmpty())			
				return Handle<Value>();
			return proto->Get(property);
		}
        if (siEx ex = scr->invoke(pthis, ACCESS_GET, index, 0, 0, &rval))
            return ThrowException(String::New(ex->message()));
        return pthis->toValue(rval);
    }

    static Handle<Value> namedSetter(Local<String> property,
                                     Local<Value> value,
                                     const AccessorInfo& info)
    {
        o3_trace_hostglue("namedSetter");
        cJs* pthis = (cJs*) cast(info.Data());
        iScr* scr = (iScr*) cast(info.Holder()->GetInternalField(0));
        Str name = *String::Utf8Value(property);
        int index = scr->resolve(pthis, name);
        Var arg(pthis->toVar(value));
        Var rval((iAlloc*) pthis);

		if (index < 0) {
			if (strEquals(name.ptr(), "prototype")) {
				pthis->setValue(scr->className(), arg);
				return value;
			}
		}
		index = scr->resolve(pthis, name, true);
        if (siEx ex = scr->invoke(pthis, ACCESS_SET, index, 1, &arg, &rval))
            return ThrowException(String::New(ex->message()));
        return pthis->toValue(rval);
    }

    static Handle<Integer> namedQuery(Local<String> property,
                                      const AccessorInfo& info)
    {
        o3_trace_hostglue("namedQuery");
        cJs* pthis = (cJs*) cast(info.Data());
        iScr* scr = (iScr*) cast(info.Holder()->GetInternalField(0));
        Str name = *String::Utf8Value(property);
        int index = scr->resolve(pthis, name);

		if (index < 0) {
			Handle<Object> proto = pthis->prototype(scr);
			if (strEquals(name.ptr(), "prototype"))
				return proto.IsEmpty() ? v8::Handle<v8::Integer>() : v8::Integer::New(v8::None);						
			if (proto.IsEmpty())			
				return v8::Handle<v8::Integer>();
			return proto->Has(property) ? v8::Integer::New(v8::None)
				: v8::Handle<v8::Integer>();
		}

		return v8::Integer::New(v8::None);		
	}

    static Handle<Boolean> namedDeleter(Local<String> property,
                                        const AccessorInfo& info)
    {
        o3_trace_hostglue("namedDeleter");
        cJs* pthis = (cJs*) cast(info.Data());
        iScr* scr = (iScr*) cast(info.Holder()->GetInternalField(0));
        Str name = *String::Utf8Value(property);
        int index = scr->resolve(pthis, name);
        Var rval((iAlloc*) pthis);

        if (index < 0)
            return False();
        if (siEx ex = scr->invoke(pthis, ACCESS_DEL, index, 0, 0, &rval)) {
            ThrowException(String::New(ex->message()));
	    return Handle<Boolean>();
	}
        return Boolean::New(rval.toBool());
    }

    static Handle<Array> namedEnumerator(const AccessorInfo& info)
    {
        o3_trace_hostglue("namedEnumerator");
        cJs* pthis = (cJs*) cast(info.Data());
        iScr* scr = (iScr*) cast(info.Holder()->GetInternalField(0));
        Handle<Array> names;
        size_t length;
        int key;

        length = 0;
        for (int index = scr->enumerate(pthis, -1); index >= 0;
             index = scr->enumerate(pthis, index))
            ++length;
        names = Array::New(length);
        key = 0;
        for (int index = scr->enumerate(pthis, -1); index >= 0;
             index = scr->enumerate(pthis, index))
            names->Set(Number::New(key++),
                       String::New(scr->name(pthis, index)));
        return names;
    }

    static Handle<Value> indexedGetter(uint32_t index,
                                       const AccessorInfo& info)
    {
        o3_trace_hostglue("indexedGetter");
        cJs* pthis = (cJs*) cast(info.Data());
        iScr* scr = (iScr*) cast(info.Holder()->GetInternalField(0));
        int getter = scr->resolve(pthis, "__getter__");
        Var arg((int) index, pthis);
        Var rval((iAlloc*) pthis);

        if (getter < 0)
            return Handle<Value>();
        if (siEx ex = scr->invoke(pthis, ACCESS_CALL, getter, 1, &arg, &rval))
            return ThrowException(String::New(ex->message()));
        return pthis->toValue(rval);
    }

    static Handle<Value> indexedSetter(uint32_t index, Local<Value> value,
                                       const AccessorInfo& info)
    {
        o3_trace_hostglue("indexedSetter");
        cJs* pthis = (cJs*) cast(info.Data());
        iScr* scr = (iScr*) cast(info.Holder()->GetInternalField(0));
        int setter = scr->resolve(pthis, "__setter__");
        tVec<Var> argv(2);
        Var rval((iAlloc*) pthis);

        if (setter < 0)
            return Handle<Value>();
        argv.push(Var((int) index, pthis));
        argv.push(pthis->toVar(value));
        if (siEx ex = scr->invoke(pthis, ACCESS_CALL, setter, 2, argv, &rval))
            return ThrowException(String::New(ex->message()));
        return pthis->toValue(rval);
    }

    static Handle<Integer> indexedQuery(uint32_t index,
                                        const AccessorInfo& info)
    {
        o3_trace_hostglue("indexedQuery");
        cJs* pthis = (cJs*) cast(info.Data());
        iScr* scr = (iScr*) cast(info.Holder()->GetInternalField(0));
        int query = scr->resolve(pthis, "__query__");
        Var arg((int) index, (iAlloc*) pthis);
        Var rval((iAlloc*) pthis);

        if (query < 0)
            return Local<Integer>();
        if (siEx ex = scr->invoke(pthis, ACCESS_CALL, query, 1, &arg, &rval)) {
            ThrowException(String::New(ex->message()));
            return Handle<Integer>();
        }

		return rval.toBool() ? v8::Integer::New(v8::None)
			: v8::Handle<v8::Integer>();
	}

    static Handle<Boolean> indexedDeleter(uint32_t index,
                                          const AccessorInfo& info)
    {
        o3_trace_hostglue("indexedDeleter");
        cJs* pthis = (cJs*) cast(info.Data());
        iScr* scr = (iScr*) cast(info.Holder()->GetInternalField(0));
        int deleter = scr->resolve(pthis, "__deleter__");
        Var arg((int) index, (iAlloc*) pthis);
        Var rval((iAlloc*) pthis);

        if (deleter < 0)
            return Local<Boolean>();
        if (siEx ex = scr->invoke(pthis, ACCESS_CALL, deleter, 1, &arg,
                                  &rval)) {
            ThrowException(String::New(ex->message()));
            return Handle<Boolean>();
        }
        return Boolean::New(rval.toBool());
    }

    static Handle<Array> indexedEnumerator(const AccessorInfo& info)
    {
        o3_trace_hostglue("indexedEnumerator");
        cJs* pthis = (cJs*) cast(info.Data());
        iScr* scr = (iScr*) cast(info.Holder()->GetInternalField(0));
        siCtx ctx(pthis);
		Var arg((iAlloc*) pthis);
        Var rval((iAlloc*) pthis);
        Handle<Array> names_out;
        size_t length;
        int key;
		int next_id = -1;
		tVec<Str> names;

		while (true) {
			next_id = scr->enumerate(ctx,next_id);
			if (next_id == -1)
				break;			
			names.push(scr->name(ctx,next_id));
		}

		length = names.size();
		names_out = Array::New(length);
		for (size_t i=0; i<length; i++) {
			names_out->Set(Number::New(i),
				String::New(names[i]));
		}
		
        return names_out;
    }

#endif // O3_V8_GLUE

    static void finalize(Persistent<Value> value, void* parameter)
    {
		o3_trace_hostglue("finalize");
		HandleScope handle_scope;

		if (!value.IsNearDeath())
			return;

        cJs* pthis = (cJs*) parameter;
        Local<Object> object = value->ToObject();
#ifdef O3_V8_GLUE
        iScr* scr = (iScr*) cast(object->GetInternalField(1));
#else
		iScr* scr = (iScr*) cast(object->GetInternalField(0));
#endif
        if (scr) {
            scr->release();
			pthis->m_wrappers[scr].Clear();
			pthis->m_wrappers.remove(scr);
            //pthis->m_objects.remove(*object);        
		}
		value.Dispose();
		value.Clear();
    }

	static void cleanup(Persistent<Value> value, void *parameter)
	{
		o3_trace_hostglue("cleanup");
		cJs* pthis = (cJs*) parameter;
		pthis->release();
	}
#ifndef O3_NODE
	Persistent<Context> m_context;
#endif
    Persistent<ObjectTemplate> m_template;
    tMap<Object*, Object*> m_objects;
    tMap<iScr*, Handle<Object> > m_wrappers;
	siMgr m_mgr;
    siMessageLoop m_loop;
    tMap<Str, Var> m_values;

    Handle<Object> createObject(iScr* scr)
    { 
        o3_trace_hostglue("createObject"); 
		HandleScope handle_scope;

		tMap<iScr*, Handle<Object> >::Iter it = 
			m_wrappers.find(scr);

		if (it != m_wrappers.end() && !it->val.IsEmpty()) {			
			return it->val;
		}	
        scr->addRef();
        Local<Object> object = m_template->NewInstance();
        Persistent<Object> handle = Persistent<Object>::New(object);
        handle.MakeWeak(this, finalize);
#ifdef O3_V8_GLUE
		object->SetInternalField(0, External::New(siCtx(this).ptr()));
		object->SetInternalField(1, External::New(scr));
		scr->init(object);
#else
		object->SetInternalField(0, External::New(scr));
#endif
		m_wrappers[scr] = handle;
		return handle_scope.Close(object);
    }

    Var toVar(Handle<Value> value)
    {
        o3_trace_hostglue("toVar");
        if (value->IsUndefined())
            return Var((iAlloc*) this);
        else if (value->IsNull())
            return Var((iScr*) 0, this);
        else if (value->IsBoolean())
            return Var(value->ToBoolean()->Value(), this);
        else if (value->IsInt32())
            return Var(value->ToInt32()->Value(), this);
        else if (value->IsNumber())
            return Var(value->ToNumber()->Value(), this);
        else if (value->IsObject()) {
            Handle<Object> object = value->ToObject();
#ifdef O3_V8_GLUE
			if (object->InternalFieldCount() > 1) 
				return Var((iScr*) cast(object->GetInternalField(1)),this);
			else
				return Var();
#else
            if (object->InternalFieldCount() > 0) 
                return (iScr*) cast(object->GetInternalField(0));
            else
                return o3_new(cScrObj)(this, object);
#endif
		} else if (value->IsString())
            return Var(*String::Utf8Value(value->ToString()), this);
        return Var((iAlloc*) this);
    }
 
    Handle<Value> toValue(const Var& val)
    {
        o3_trace_hostglue("toValue");
        switch (val.type()) {
        case Var::TYPE_VOID:
            return Undefined();
        case Var::TYPE_NULL:
            return Null();
        case Var::TYPE_BOOL:
            return Boolean::New(val.toBool());
        case Var::TYPE_INT32:
            return Int32::New(val.toInt32());
        case Var::TYPE_INT64:
        case Var::TYPE_DOUBLE:
            return Number::New(val.toDouble());
		case Var::TYPE_SCR: {
			siScr obj = val.toScr();
			siScrObj wrapped(obj);
			if (wrapped) {
				return wrapped->unwrap();
			}
            return createObject(obj);
		}
        case Var::TYPE_STR:
            return String::New(val.toStr());
        case Var::TYPE_WSTR:
            return String::New((::uint16_t*) val.toWStr().ptr());
        default:
            return Undefined();
        }
    }

public:
#ifndef O3_NODE
    cJs(iMgr* mgr, int argc, char** argv, char** envp)
    :   m_context(Context::New())
    {
        o3_trace_hostglue("cJs");
        Context::Scope context_scope(m_context);
        HandleScope handle_scope;
        Local<ObjectTemplate> handle;
        Local<External> data;
        Handle<Object> object;

        handle = ObjectTemplate::New();
        m_template = Persistent<ObjectTemplate>::New(handle);
        m_mgr = mgr;
        m_loop = g_sys->createMessageLoop();
        m_template->SetInternalFieldCount(2);
        data = External::New(this);
#ifndef O3_V8_GLUE 
		m_template->SetCallAsFunctionHandler(invocation, data);
        m_template->SetNamedPropertyHandler(namedGetter, namedSetter,
                                            namedQuery, namedDeleter,
                                            namedEnumerator, data);
#endif
        m_template->SetIndexedPropertyHandler(indexedGetter, indexedSetter,
                                              indexedQuery, indexedDeleter,
                                              indexedEnumerator, data);
        object = createObject(o3_new(cO3)(this, argc, argv, envp));
        m_context->Global()->Set(String::New("o3"), object);
    }
#endif

    cJs(Handle<Object> target, iMgr* mgr, int argc, char** argv, char** envp)
    {
        o3_trace_hostglue("cJs");
        HandleScope handle_scope;
        Local<ObjectTemplate> handle;
        Local<External> data;       
		Handle<Object> object;
		Persistent<Object> object2;

        handle = ObjectTemplate::New();
        m_template = Persistent<ObjectTemplate>::New(handle);
        m_mgr = mgr;
        m_loop = g_sys->createMessageLoop();
        m_template->SetInternalFieldCount(2);

        data = External::New(this);
#ifndef O3_V8_GLUE 
        m_template->SetCallAsFunctionHandler(invocation, data);
        m_template->SetNamedPropertyHandler(namedGetter, namedSetter,
                                            namedQuery, namedDeleter,
                                            namedEnumerator, data);
#endif
        m_template->SetIndexedPropertyHandler(indexedGetter, indexedSetter,
                                              indexedQuery, indexedDeleter,
                                              indexedEnumerator, data);

		object = createObject(o3_new(cO3)(this, argc, argv, envp));		
		object2 = Persistent<Object>::New(object);
#ifdef O3_NODE
		object2.MakeWeak(this, cleanup);
#endif
		target->Set(String::New("root"), object2);
	}

	cJs(Handle<Object> target, iMgr* mgr) 
	{
		o3_trace_scrfun("cJs");
		HandleScope handle_scope;
		Local<ObjectTemplate> handle;
		Local<External> data;       
		Handle<Object> object;
		Persistent<Object> object2;
		m_mgr = mgr;
		handle = ObjectTemplate::New();
		data = External::New(this);
		m_template = Persistent<ObjectTemplate>::New(handle);
		m_template->SetInternalFieldCount(2);
		m_template->SetIndexedPropertyHandler(indexedGetter, indexedSetter,
			indexedQuery, indexedDeleter,
			indexedEnumerator, data);
addRef();
		siScr scr(o3_new(cO3)(this, 0, 0, 0));
		//siScr scr = o3_new(cGlueTest)();
		object = createObject(scr);	
		object2 = Persistent<Object>::New(object);
		object2.MakeWeak(this, cleanup);
		target->Set(String::New("root"), object2);
	}


    ~cJs()
    {
        o3_trace_hostglue("~cJs");
#ifndef O3_NODE		
        Context::Scope context_scope(m_context);
#endif		
        HandleScope handle_scope;

        for (tMap<Object*, Object*>::Iter i = m_objects.begin();
             i != m_objects.end(); ++i) {
            Local<Object> object = i->val;
            iScr* scr = (iScr*) cast(object->GetInternalField(0));

            scr->release();
            object->SetInternalField(0, Null());
        }

        m_template.Dispose();
#ifndef O3_NODE
        m_context.Dispose();
#endif    
	}

    o3_begin_class(cJsBase)
    o3_end_class()

    o3_glue_gen()

    static o3_ext("cO3") o3_get siScr js(iCtx* ctx)
    {
        o3_trace_hostglue("js");
#ifndef O3_NODE
		Var js = ctx->value("js");

        if (js.type() == Var::TYPE_VOID)
            js = ctx->setValue("js", (iScr*) o3_new(cJs)(ctx->mgr(), 0, 0, 0));
        return js.toScr();
#else
		return siScr();	
#endif
    }

    void* alloc(size_t size)
    {
        o3_trace_hostglue("alloc");

        return memAlloc(size);
    }

    void free(void* ptr)
    {
        o3_trace_hostglue("free");

        memFree(ptr);
    }

    siMgr mgr()
    {
        o3_trace_hostglue("mgr");

        return m_mgr;
    }

    siMessageLoop loop()
    {
        o3_trace_hostglue("loop");

        return m_loop;
    }

    Var value(const char* key)
    {
        o3_trace_hostglue("value");

        return m_values[key];
    }

    Var setValue(const char* key, const Var& val)
    {
        o3_trace_hostglue("setValue");

        return m_values[key] = val;
    }

    o3_fun Var eval(const char* str, siEx* ex)
    {
        o3_trace_hostglue("eval");
#ifndef O3_NODE		
        Context::Scope context_scope(m_context);
#endif	
        HandleScope handle_scope;
        TryCatch try_catch;
        v8::Handle<Script> script;
        v8::Handle<Value> result;

        if (*str == '#')
            while (*str)
                if (*str++ == '\n')
                    break;
        script = Script::New(String::New(str));
        if (script.IsEmpty())
            goto error;
        result = script->Run();
        if (result.IsEmpty())
            goto error;
        return toVar(result);
    error:
        Str msg = *String::Utf8Value(try_catch.Exception());
        Str line = Str::fromInt32(try_catch.Message()->GetLineNumber());

        if (ex)
            *ex = o3_new(cEx)(msg + " on line " + line);
        return Var((iAlloc*) this);
    }

	virtual void setAppWindow(void*)
	{
		
	}

	virtual void* appWindow()
	{
		o3_trace_scrfun("appWindow");
		return 0;
	}

	virtual bool isIE()
	{
		o3_trace_hostglue("isIE");
		return false;
	}

	Handle<Object> prototype(iScr* scr)
	{
		o3_trace_scrfun("prototype");
		Var proto_var = value(scr->className());
		if (proto_var.type() == Var::TYPE_SCR){
			siScrObj proto = proto_var.toScr();
			if (proto)
				return proto->unwrap();
		}
		return Handle<Object>();
	}
};

}

#endif // O3_C_JS1_POSIX_H
