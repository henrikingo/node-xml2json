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
#ifndef O3_C_FS1_POSIX_H
#define O3_C_FS1_POSIX_H

#include <sys/stat.h>
#include <dirent.h>
#include <utime.h>

namespace o3 {

struct cFs : cFsBase {
    bool m_valid;
    Str m_root_path;
    Str m_rel_path;
    int64_t m_time;
    siTimer m_timer;

    cFs(const char* root_path = "/", const char* rel_path = "/")
        : m_root_path(root_path), m_rel_path(rel_path)
    {
        o3_trace_scrfun("cFs");
        m_valid = parsePath();
    }

    o3_begin_class(cFsBase)
    o3_end_class()

    o3_glue_gen();

    static o3_ext("cO3") o3_get siScr fs(iCtx* ctx)
    {
        o3_trace_scrfun("fs");
        Var fs = ctx->value("fs");

        if (fs.type() == Var::TYPE_VOID)
            fs = ctx->setValue("fs", o3_new(cFs)());
        return fs.toScr();
    }

    static o3_ext("cO3") o3_get siScr cwd(iCtx* ctx)
    {
        o3_trace_scrfun("cwd");
        Var cwd = ctx->value("cwd");

        if (cwd.type() == Var::TYPE_VOID) {
            char* buf = getcwd(0, 0);

            cwd = ctx->setValue("cwd", o3_new(cFs)("/", buf));
            free(buf);
        }
        return cwd.toScr();
    }
	
	static siScr cwd()
    {
        o3_trace_scrfun("cwd");
		char* buf = getcwd(0, 0);

		siFs ret = o3_new(cFs)("/", buf);
		free(buf);
		return ret;
    }

	static o3_ext("cO3") o3_get siFs fsSafe(iCtx* ctx) 
	{     
		o3_trace_scrfun("fsSafe");     
		siFs ret = o3_new(cFs(ctx->mgr()->root(), "/"));

		return ret;
	}

	static siUnk rootDir(iCtx*) 
	{	
		o3_trace_scrfun("rootDir");	
		return o3_new(cFs)(O3_PLUGIN_TMP);
	}

	static siUnk settingsDir(iCtx*) 
	{	
        o3_trace_scrfun("settingsDir");	
        return o3_new(cFs)(O3_PLUGIN_TMP);
	}

    static siUnk installerDir(iCtx*)
    {
        o3_trace_scrfun("installerDir");
        return o3_new(cFs)(O3_PLUGIN_TMP);
    }

    static siUnk pluginDir(iCtx*)
    {
        o3_trace_scrfun("pluginDir");
        return o3_new(cFs)(O3_PLUGIN_DIR);
    }

    bool valid()
    {
        o3_trace_scrfun("valid");

        return m_valid;
    }

    bool exists()
    {
        o3_trace_scrfun("exists");
        struct stat buf;

        return stat(localPath(), &buf) == 0;
    }

    Type type()
    {
        o3_trace_scrfun("type");
        struct stat buf;

        if (stat(localPath(), &buf) >= 0) {
            if (buf.st_mode & S_IFDIR)
                return TYPE_DIR;
            else if (buf.st_mode & S_IFREG)
                return TYPE_FILE;
            else if (buf.st_mode & S_IFLNK)
                return TYPE_LINK;
        }
        return TYPE_INVALID;
    }

    int64_t accessedTime()
    {
        o3_trace_scrfun("accessedTime");
        struct stat buf;

        if (stat(localPath(), &buf) < 0) 
            return 0;
#ifdef O3_APPLE
        return buf.st_atimespec.tv_sec;
#endif // O3_APPLE
#ifdef O3_LINUX
        return buf.st_atime;
#endif // O3_LINUX
    }

    int64_t modifiedTime()
    {
        o3_trace3 trace;
        struct stat buf;

        if (stat(localPath(), &buf) < 0) 
            return 0;
#ifdef O3_APPLE
        return buf.st_mtimespec.tv_sec;
#endif // O3_APPLE
#ifdef O3_LINUX
        return buf.st_mtime;
#endif // O3_LINUX
    }

    int64_t createdTime()
    {
		// no such thing on UNIX as created time
        o3_trace_scrfun("createdTime");
		// no such thing on UNIX as created time
        return 0;
    }

	int64_t setAccessedTime(int64_t time)
	{
		o3_trace_scrfun("setAccessedTime");
		struct utimbuf b;
		b.actime = time;
		b.modtime = modifiedTime();
		utime(localPath(), &b);
		return time;
	}

	int64_t setModifiedTime(int64_t time)
	{
		struct utimbuf b;
		b.actime = accessedTime();
		b.modtime = time;
		utime(localPath(), &b);
		return time;
	}

	int64_t setCreatedTime(int64_t time)
	{
		o3_trace_scrfun("setCreatedTime");
		return time;
	}

    size_t size()
    {
        o3_trace_scrfun("size");
        struct stat buf;

        if (stat(localPath(), &buf) < 0) 
            return 0;
        return buf.st_size;
    }

    Str path()
    {
        o3_trace_scrfun("path");

        return m_rel_path;
    }

    siFs get(const char* path)
    {
        o3_trace_scrfun("get");

        return o3_new(cFs)(m_root_path,
                            *path == '/' ? Str(path)
							: (m_rel_path.size() > 1 ? m_rel_path : Str()) + "/" + path);
    }

    bool hasChildren()
    {
        o3_trace_scrfun("hasChildren");
        DIR* dir = opendir(localPath());

        if (!dir)
            return false;
        closedir(dir);
        return true;
    }

    tVec<Str> scandir(const char* path = 0)
    {
        o3_trace_scrfun("scandir");
        tVec<Str> names;
        DIR* dir;

        if (*path)
            return ((cFs*) get(path).ptr())->scandir();
        dir = opendir(localPath());
        if (!dir)
            return names;
        while (struct dirent* ent = readdir(dir))
            if (!strEquals(ent->d_name, ".") &&
                !strEquals(ent->d_name, ".."))
                names.push(ent->d_name);
        closedir(dir);
        return names;
    }

    tVec<siFs> children()
    {
        o3_trace_scrfun("children");
        tVec<siFs> names;
        DIR* dir = opendir(localPath());     

        if (!dir)
            return names;
		while (struct dirent* ent = readdir(dir))
            if (!strEquals(ent->d_name, ".") &&
				!strEquals(ent->d_name, "..")) 
                names.push(get(ent->d_name));				
		
        closedir(dir);
        return names;
    }

    bool createDir()
    {
        o3_trace_scrfun("createDir");
		
        if (!exists()) {
            siFs dir = parent();

            if (dir->valid())
                if (!dir->createDir())
				    return false;
            mkdir(localPath(), 0777);
        }
        return isDir();
    }

    bool createFile()
    {
        o3_trace_scrfun("createFile");

        if (!exists()) 
            if ( ! ((cFs*) parent().ptr())->createDir())
				return false;
        ::fclose(::fopen(localPath(), "a"));
        return isFile();
    }

    bool createLink(iFs* to)
    {
        o3_trace_scrfun("createLink");

        if (!exists()) 
            if ( ! ((cFs*) parent().ptr())->createDir())
				return false;
        link(localPath(), ((cFs*) to)->localPath());
        return isLink();
    }

    bool remove(bool deep)
    {
        o3_trace_scrfun("remove");

        if (deep) {
            tVec<siFs> nodes = children();

            for (size_t i = 0; i < nodes.size(); ++i) 
                ((cFs*) nodes[i].ptr())->remove(deep);
        }
        if (exists()) {
            if (isDir())
                rmdir(localPath());
            else
                unlink(localPath());
        }
        return !exists();
    }

    siStream open(const char* mode, siEx* ex)
    {
        o3_trace_scrfun("open");
        FILE* stream;
        createFile();
        stream = ::fopen(localPath(), mode);
        if (!stream)
            return siStream();
        return o3_new(cStream)(stream);
    }

    void startListening()
    {
        o3_trace_scrfun("startListening");
        m_time = exists() ? modifiedTime() : 0;
        m_timer = siCtx(m_ctx)->loop()->createTimer(10,
                Delegate(this, &cFs::listen));
    }

    void stopListening()
    {
        o3_trace_scrfun("stopListening");
        m_timer = 0;
    }

    void listen(iUnk*)
    {
        o3_trace_scrfun("listen");
        int64_t time = modifiedTime();

        if (exists() && m_time != time) {
            m_time = time;
            m_onchange_delegate(this);
        }
        m_timer->restart(10);
    }

    bool parsePath()
    {		
        o3_trace_scrfun("parsePath");		
        Str path = m_rel_path;
        const char* src = path.ptr();
        char* dst = path.ptr();

        while (*src) {
            if (*src == '/') {
                *dst++ = *src++; 
                if (*src == '.') {
                    *dst++ = *src++;
                    if (*src == '.') {
                        *dst++ = *src++;
                        if (!*src || *src == '/') {
                            dst -= 3;
                            if (dst-- == path.ptr())
                                return false;
                            while (dst != path.ptr() && *dst != '/')
                                --dst;
                        }
                    } else if (!*src || *src == '/')
                        dst -= 2;
                } else if (*src == '/')
                    return false;
            } else
                *dst++ = *src++;
        }
        path.resize(dst - path.ptr());
        m_rel_path = path;
		return true;

    }

    Str localPath()
    {
        o3_trace_scrfun("localPath");
        return (m_root_path.size() > 1 ? m_root_path : Str()) + m_rel_path;
    }

	Str fullPath()
	{
		o3_trace_scrfun("fullPath");
		return localPath();
	}
	
	void openDoc() 
	{

	}

#ifdef O3_PLUGIN
    static bool parseDescription(const char** in, void* ctx)
    {
        o3_trace_scrfun("parseDescription");
        while (**in && **in != ']')
            ++*in;
        return true;
    }
    
    static bool parseType(const char** in, void* ctx)
    {
        o3_trace_scrfun("parseType");
        NSMutableArray* types = (NSMutableArray*) ctx;
        const char* str;
        NSString* type;
        
        if (!strAcceptChr(in, '"'))
                return false;
        str = *in;
        while (**in && **in != '"')
            ++*in;
        if (!strAcceptChr(in, '"'))
            return false;
        type = [[NSString alloc] initWithCString:str length:*in - str - 1];
        if (!type)
            return false;
        [types addObject:type];
        [type release];
        return true;
    }
    
    static bool parseTypes(const char** in, void* ctx)
    {   
        o3_trace_scrfun("parseTypes");   
        if (!strAcceptChr(in, '['))
            return false;
        while (**in && **in != ']') {
            if (!parseType(in, ctx))
                return false;
            if (!strAcceptChr(in, ','))
                return false;
        }
        if (!strAcceptChr(in, '['))
            return false;
        return true;
    }
    
    static bool parseEntry(const char** in, void* ctx)
    {
        o3_trace_scrfun("parseEntry");
        if (!strAcceptChr(in, '['))
            return false;
        if (!parseTypes(in, ctx))
            return false;
        if (!parseDescription(in, ctx))
            return false;
        if (!strAcceptChr(in, ']'))
            return false;
        return true;
    }
#endif // O3_PLUGIN

    static o3_ext("cO3") o3_fun siFs openFileDialog(iCtx* ctx,
            const char* filter)
    {
o3_trace_scrfun("openFileDialog");
#ifdef O3_PLUGIN
        NSOpenPanel* panel;
        NSMutableArray* types;
        
        panel = [NSOpenPanel openPanel];
        types = [[NSMutableArray alloc] initWithCapacity:1];
        strParseList(&filter, parseEntry, types);
        if ([types count] > 0)
            [panel setAllowedFileTypes:types];
        if ([panel runModalForDirectory:NSHomeDirectory()
                                   file:nil
                                 types:types] == NSOKButton) {
            NSArray* paths;
            NSString *path;
            
            [types release];
            paths = [panel filenames];
            path = [paths objectAtIndex:0];
            return siFs(cFs::fs(ctx))->get([path UTF8String]);
        } else {
            [types release];
            return 0;
        }
#else
        return 0;
#endif // O3_PLUGIN
    }
    
    static o3_ext("cO3") o3_fun siFs saveFileDialog(iCtx* ctx)
    {
o3_trace_scrfun("saveFileDialog");
#ifdef O3_PLUGIN
        NSSavePanel* panel;
        
        panel = [NSSavePanel savePanel];
        if ([panel runModal] == NSOKButton) {
            NSString *path;
            
            path = [panel filename];
            return siFs(cFs::fs(ctx))->get([path UTF8String]);
        } else
            return 0;
#else
        return 0;
#endif // O3_PLUGIN
    }
};

}

#endif // O3_C_FS1_POSIX_H
