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
#ifndef O3_C_GLWINDOW_H
#define O3_C_GLWINDOW_H

#include <lib_glee.h>

#ifdef WIN32
#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glu32.lib")
#endif
#include <gl/glu.h>

#include "canvas/canvas.h"

namespace o3 
{
	o3_iid(iShaderProgram, 0xdaf5e662, 0x66c, 0x483f, 0x89, 0x20, 0x9a, 0xf8, 0x49, 0x5c, 0x83, 0x2c);
	o3_iid(iVertexArray, 0xd15010ef, 0xc2c6, 0x4b3c, 0xad, 0xf1, 0x3e, 0xb7, 0x42, 0xde, 0xa4, 0x75);
	o3_iid(iTexture, 0x1d657e65, 0xf75a, 0x48f0, 0x8e, 0x67, 0xab, 0xee, 0x5, 0xc2, 0x8b, 0x43);

	o3_iid(iVector4, 0x71d38725, 0xf793, 0x41f3, 0x9e, 0x67, 0x59, 0x5c, 0xe6, 0x8f, 0x35, 0x8e);
	o3_iid(iVector3, 0x14be7bf0, 0x6ea7, 0x46dd, 0xbb, 0x60, 0x1b, 0xad, 0x36, 0x70, 0xb9, 0x4);
	o3_iid(iVector2, 0x3a8efd5c, 0xe360, 0x4cc1, 0xb9, 0xa6, 0xa3, 0x9, 0x85, 0x43, 0x9c, 0xe);
	o3_iid(iMat44, 0x6e24a8f1, 0x2441, 0x4056, 0xba, 0x17, 0x4a, 0xa2, 0x93, 0x50, 0xaa, 0x8e);

	struct iShaderProgram: iUnk{virtual void *getThis() = 0;};
	struct iVertexArray: iUnk{virtual void *getThis() = 0;};
	struct iTexture: iUnk {virtual void *getThis() = 0;};

	struct iVector4: iUnk {float x,y,z,w;};
	struct iVector3: iUnk {float x,y,z;};
	struct iVector2: iUnk {float x,y;};
	struct iMat44: iUnk {float v[16];};

	struct cVector4: cScr, iVector4
	{
		o3_begin_class(cScr);
		o3_add_iface(iVector4);
		o3_end_class();

		o3_glue_gen();

		cVector4(double xx,double yy, double zz, double ww)
		{
			x = (float)xx;
			y = (float)yy;
			z = (float)zz;
			w = (float)ww;
		}

		static o3_ext("cO3") o3_fun siVector4 vec4(double xx,double yy, double zz, double ww)
		{              
			cVector4* ret = o3_new(cVector4)(xx,yy,zz,ww);
			return ret;		
		}
	};

	struct cVector3: cScr, iVector3
	{
		o3_begin_class(cScr);
		o3_add_iface(iVector3);
		o3_end_class();

		o3_glue_gen();

		cVector3(double xx,double yy, double zz)
		{
			x = (float)xx;
			y = (float)yy;
			z = (float)zz;
		}

		static o3_ext("cO3") o3_fun siVector3 vec3(double xx,double yy, double zz)
		{              
			cVector3* ret = o3_new(cVector3)(xx,yy,zz);
			return ret;		
		}
	};

	struct cVector2: cScr, iVector2
	{
		o3_begin_class(cScr);
		o3_add_iface(iVector2);
		o3_end_class();

		o3_glue_gen();

		cVector2(double xx,double yy)
		{
			x = (float)xx;
			y = (float)yy;			
		}

		static o3_ext("cO3") o3_fun siVector2 vec2(double xx,double yy)
		{              
			cVector2* ret = o3_new(cVector2)(xx,yy);
			return ret;		
		}
	};

	struct cMat44: cScr, iMat44
	{
		o3_begin_class(cScr);
		o3_add_iface(iMat44);
		o3_end_class();

		o3_glue_gen();

		cMat44()
		{
			
		}

		static o3_ext("cO3") o3_fun siMat44 mat44()
		{              
			cMat44 *ret = o3_new(cMat44)();
			return ret;		
		}
	};

	struct cGLVertex
	{
		cGLVertex(){};
		cGLVertex(float _x, float _y, float _z, float _nx, float _ny, float _nz, float _u, float _v)
		{
			x = _x;y = _y;z=_z;
			nx = _nx;ny = _ny;nz = _nz;
			u=_u;v=_v;
		};
		float x,y,z;	// position
		float nx,ny,nz; // normal
		float u,v;      // texcoord0
	};
	
	struct cGLVertexArray: cScr, iVertexArray
	{
		o3_begin_class(cScr);
		o3_add_iface(iVertexArray);
		o3_end_class();

		o3_glue_gen();
		
		virtual void *getThis(){return this;};
		
		tVec<cGLVertex> mVertices;
		bool mHasVBO;
		unsigned int mBufferID;
		bool mUploadNeeded;
		
		cGLVertexArray(size_t initialsize)
		{
			GLeeInit();
			if (_GLEE_ARB_vertex_buffer_object)
			{
				mHasVBO = true;
				mUploadNeeded = true;
				glGenBuffers(1, &mBufferID);
				glBindBufferARB(GL_ARRAY_BUFFER_ARB, mBufferID);         // for vertex coordinates

				while (initialsize--) mVertices.push(cGLVertex());
				glBufferDataARB(GL_ARRAY_BUFFER_ARB, mVertices.size()*sizeof(cGLVertex), mVertices.ptr(), GL_STREAM_DRAW_ARB);
			}
			else
			{
				// gl vertex array fallback!
				mUploadNeeded = false;
			};
		};

		~cGLVertexArray()
		{
			if (mHasVBO) 
			{
				glDeleteBuffers(1, &mBufferID);
			};
		};
		static o3_ext("cGLWindow") o3_fun siVertexArray vertexarray(size_t intitialsize)
		{
			cGLVertexArray* ret = o3_new(cGLVertexArray)(intitialsize);
			return ret;
		};

		static o3_ext("cGLWindow") o3_fun siVertexArray cube(double s = 1.0)
		{
			float s2 = (float)(s/2.0);
			cGLVertexArray* ret = o3_new(cGLVertexArray)(18*2);
			ret->mVertices[0] = cGLVertex(-s2,-s2,-s2,  0,0,-1,0,0);
			ret->mVertices[1] = cGLVertex( s2,-s2,-s2,  0,0,-1,1,0);
			ret->mVertices[2] = cGLVertex( s2, s2,-s2,  0,0,-1,1,1);
			
			ret->mVertices[3] = cGLVertex(-s2,-s2,-s2,  0,0,-1,0,0);
			ret->mVertices[4] = cGLVertex( s2, s2,-s2,  0,0,-1,1,1);
			ret->mVertices[5] = cGLVertex(-s2, s2,-s2,  0,0,-1,0,1);

			ret->mVertices[6] = cGLVertex(-s2,-s2,-s2,  0,-1,0,0,0);
			ret->mVertices[7] = cGLVertex( s2,-s2,-s2,  0,-1,0,1,0);
			ret->mVertices[8] = cGLVertex( s2,-s2, s2,  0,-1,0,1,1);
											   
			ret->mVertices[9] = cGLVertex( -s2,-s2,-s2,  0,-1,0,0,0);
			ret->mVertices[10] = cGLVertex( s2,-s2, s2,  0,-1,0,1,1);
			ret->mVertices[11] = cGLVertex(-s2,-s2, s2,  0,-1,0,0,1);

			ret->mVertices[12] = cGLVertex(-s2,-s2,-s2,  -1,0,0,0,0);
			ret->mVertices[13] = cGLVertex(-s2, s2,-s2,  -1,0,0,1,0);
			ret->mVertices[14] = cGLVertex(-s2, s2, s2,  -1,0,0,1,1);
											 
			ret->mVertices[15] = cGLVertex(-s2,-s2,-s2,  -1,0,0,0,0);
			ret->mVertices[16] = cGLVertex(-s2, s2, s2,  -1,0,0,1,1);
			ret->mVertices[17] = cGLVertex(-s2,-s2, s2,  -1,0,0,0,1);

			ret->mVertices[18+0] = cGLVertex(-s2,-s2,s2,  0,0,1,0,0);
			ret->mVertices[18+1] = cGLVertex( s2,-s2,s2,  0,0,1,1,0);
			ret->mVertices[18+2] = cGLVertex( s2, s2,s2,  0,0,1,1,1);
			
			ret->mVertices[18+3] = cGLVertex(-s2,-s2,s2,  0,0,1,0,0);
			ret->mVertices[18+4] = cGLVertex( s2, s2,s2,  0,0,1,1,1);
			ret->mVertices[18+5] = cGLVertex(-s2, s2,s2,  0,0,1,0,1);

			ret->mVertices[18+6] = cGLVertex(-s2,s2,-s2,  0,1,0,0,0);
			ret->mVertices[18+7] = cGLVertex( s2,s2,-s2,  0,1,0,1,0);
			ret->mVertices[18+8] = cGLVertex( s2,s2, s2,  0,1,0,1,1);
											   
			ret->mVertices[18+9] = cGLVertex( -s2,s2,-s2,  0,1,0,0,0);
			ret->mVertices[18+10] = cGLVertex( s2,s2, s2,  0,1,0,1,1);
			ret->mVertices[18+11] = cGLVertex(-s2,s2, s2,  0,1,0,0,1);

			ret->mVertices[18+12] = cGLVertex(s2,-s2,-s2,  1,0,0,0,0);
			ret->mVertices[18+13] = cGLVertex(s2, s2,-s2,  1,0,0,1,0);
			ret->mVertices[18+14] = cGLVertex(s2, s2, s2,  1,0,0,1,1);
											 
			ret->mVertices[18+15] = cGLVertex(s2,-s2,-s2,  1,0,0,0,0);
			ret->mVertices[18+16] = cGLVertex(s2, s2, s2,  1,0,0,1,1);
			ret->mVertices[18+17] = cGLVertex(s2,-s2, s2,  1,0,0,0,1);
			ret->mUploadNeeded = true;

			return ret;
		};

		o3_fun void Render(size_t mode = GL_TRIANGLES, size_t startvertex = 0, size_t endvertex = 0xffffffff)
		{
			if (endvertex == 0xffffffff) 
			{
				endvertex = mVertices.size();
			}
			endvertex = __min(mVertices.size(), endvertex);
			if (endvertex <= startvertex) return;
			
			if (mHasVBO)
			{


				glBindBufferARB(GL_ARRAY_BUFFER_ARB, mBufferID);         // for vertex coordinates
				if (mUploadNeeded)
				{
					unsigned char *d = (unsigned char *)glMapBuffer(GL_ARRAY_BUFFER_ARB, GL_WRITE_ONLY_ARB);
					if (d)
					{
						int VertexBytes = sizeof(cGLVertex) * mVertices.size();
						cGLVertex *VertexData = mVertices.ptr();
						memcpy(d, VertexData, VertexBytes);
						glUnmapBuffer(GL_ARRAY_BUFFER_ARB);
						mUploadNeeded = false;
					};
					
				};
				glEnableClientState(GL_VERTEX_ARRAY);             
				glEnableClientState(GL_NORMAL_ARRAY);    
				glClientActiveTexture(GL_TEXTURE0);
				glEnableClientState(GL_TEXTURE_COORD_ARRAY);           
				glVertexPointer(3, GL_FLOAT, sizeof(cGLVertex), 0);              
				glNormalPointer(GL_FLOAT, sizeof(cGLVertex), (void*)(sizeof(float)*3));               
				glTexCoordPointer(2, GL_FLOAT, sizeof(cGLVertex), (void*)(sizeof(float)*6));          
				
				glDrawArrays(mode, startvertex, endvertex);

				glDisableClientState(GL_VERTEX_ARRAY);            
				glDisableClientState(GL_NORMAL_ARRAY);            
				glDisableClientState(GL_TEXTURE_COORD_ARRAY);     

				glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
			};		
		};

		o3_fun size_t getSize()
		{
			return mVertices.size();
		};

		o3_fun void setVertex(size_t index,
							  double x,double y, double z, 
							  double nx, double ny , double nz,
							  double u, double v)
		{
			if (index < mVertices.size())
			{
				cGLVertex &V = mVertices[index];
				V.x = (float)x;
				V.y = (float)y;
				V.z = (float)z;
				V.nx = (float)nx;
				V.ny = (float)ny;
				V.nz = (float)nz;
				V.u = (float)u;
				V.v = (float)v;
				mUploadNeeded = true;
			};
		}

		
	};
	
	struct cGLShaderProgram:cScr, iShaderProgram
	{
		o3_begin_class(cScr);
		o3_add_iface(iShaderProgram);
		o3_end_class();

		o3_glue_gen();

		virtual void *getThis(){return this;};
		
		unsigned int mProgram;

		static o3_ext("cGLWindow") o3_fun siShaderProgram shaderprogram(const Str &vertexsource, const Str &fragmentsource)
		{
			cGLShaderProgram* ret = o3_new(cGLShaderProgram)(vertexsource, fragmentsource);
			return ret;
		};


		void CheckError(unsigned int obj)
		{
			int infologLength = 0;
			int maxLength = 0;
			
			if(glIsShader(obj))
			{
				glGetShaderiv(obj,GL_INFO_LOG_LENGTH,&maxLength);
			}
			else
			{
				glGetProgramiv(obj,GL_INFO_LOG_LENGTH,&maxLength);
			};
			if (maxLength<=0) return;
			char *infoLog = new char[maxLength];
 
			if (glIsShader(obj))
			{
				glGetShaderInfoLog(obj, maxLength, &infologLength, infoLog);
			}
			else
			{
				glGetProgramInfoLog(obj, maxLength, &infologLength, infoLog);
			};
 
			if (infologLength > 0)
			{
				printf("%s\n",infoLog);
			};
			delete [] infoLog;
		};

		cGLShaderProgram(const Str &VertexSource, const Str &FragmentSource)
		{
		
			GLenum vshader;

			vshader = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
			const char *v = VertexSource.ptr();
			glShaderSourceARB(vshader, 1, &v, NULL);

			GLenum fshader;
			fshader = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
			const char *f = FragmentSource.ptr();
			glShaderSourceARB(fshader, 1,&f, NULL);

			mProgram = glCreateProgramObjectARB();

			glCompileShaderARB(fshader);

			glCompileShaderARB(vshader);

			glAttachObjectARB(mProgram, fshader);

			glAttachObjectARB(mProgram, vshader);

			glLinkProgramARB(mProgram);
			CheckError(vshader);
			CheckError(fshader);
			CheckError(mProgram);
		};

		~cGLShaderProgram()
		{
			glDeleteProgram(mProgram);
		}

		o3_fun void bind()
		{
			glUseProgramObjectARB(mProgram);
		};

		o3_fun void unbind()
		{
			glUseProgramObjectARB(0);
		};
	};
	
	struct cGLTexture: cScr, iTexture
	{
		o3_begin_class(cScr);
		o3_add_iface(iTexture);
		o3_end_class();

		o3_glue_gen();


		virtual void *getThis(){return this;};

		cGLTexture(int w, int h)
		{
			if (w<1 || h<1) return;
			GLeeInit();
			glGenTextures(1, &mTextureID);
			mW = w;
			mH = h;
			mActualW = 1;
			mActualH = 1;
			while (mActualW < mW)  mActualW <<= 1;
			while (mActualH < mH)  mActualH <<= 1;
			bind();
			glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
			glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR);
			glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
			glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
			glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
			mLocalData = new unsigned char[mActualW*mActualH*4];

			unsigned char *blankimage = mLocalData;
			for (int i= 0;i<mActualW*mActualH;i++)
			{
				int x = (i/32);
				int y = (x/32)/32;
				x%= (1024/32);
				unsigned char check = ((x+y)%2 ==1)?0xff:0;
				blankimage[i*4+0] = check;
				blankimage[i*4+1] = check;
				blankimage[i*4+2] = check;
				blankimage[i*4+3] = 0xff;
			};

			glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA, mActualW,mActualH, 0,GL_RGBA, GL_UNSIGNED_BYTE, blankimage);
		//	delete [] blankimage;
		};
		unsigned char *mLocalData ;

		~cGLTexture()
		{
			if (mLocalData) delete [] mLocalData;
			glDeleteTextures(1, &mTextureID);
		};
		
		static o3_ext("cGLWindow") o3_fun siTexture texture(size_t width, size_t height)
		{
			cGLTexture* ret = o3_new(cGLTexture)(width, height);
			return ret;
		};

		int mW, mH, mActualW, mActualH;


		unsigned int mColorMode;
		unsigned int mTextureID;
		
		virtual o3_fun void bind()
		{
			glBindTexture(GL_TEXTURE_2D, mTextureID);
		};

		virtual o3_fun void unbind()
		{
			glBindTexture(GL_TEXTURE_2D, 0);
		};

		virtual o3_fun void upload(iScr* source, size_t width = 0, size_t height =0)
		{

			tSi<iImage> Image(source);
			
			if (Image)
			{
				if (width == 0) width = Image->width();
				if (height == 0) height = Image->height();
				switch(Image->mode_int())
				{
				case Image::MODE_ARGB:
					{
						for (size_t y =0;y<(size_t)__min(__min((unsigned int)mActualH, height), Image->height());y++)
						{
							unsigned char *P = Image->getrowptr(y);
							unsigned char *localp = mLocalData + (y*(unsigned int)mActualW)*4;
							for (size_t x = 0;x<(size_t)__min(__min((unsigned int)mActualW, width),Image->width());x++)
							{
								localp[0] = P[2];
								localp[1] =  P[1];
								localp[2] =  P[0];
								localp[3] =  P[3];

								P+=4;
								localp+=4;
							};
						};
					};
					break;
				case Image::MODE_RGB:
					{
						for (size_t y =0;y<(size_t)__min(__min((unsigned int)mActualH, height), Image->height());y++)
						{
							unsigned char *P = Image->getrowptr(y);
							unsigned char *localp = mLocalData + (y*(unsigned int)mActualW)*4;
							for (size_t x = 0;x<(size_t)__min(__min((unsigned int)mActualW, width),Image->width());x++)
							{
								localp[0] = P[2];
								localp[1] =  P[1];
								localp[2] =  P[0];
								localp[3] =  255;

								P+=4;
								localp+=4;
							};
						};
					};
					break;
				}

			};
			bind();
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0,0,mActualW,__min((int)height, mActualH), GL_RGBA, GL_UNSIGNED_BYTE, mLocalData);
		};
	};

	struct cGLWindow : cWindow
	{
		enum ButtonType {
			TYPE_PUSH,
			TYPE_RADIO 
		};

		cGLWindow()
		{
			m_def_proc = NULL;
			mGLContext = NULL;
			mCurrentDC = NULL;
		};

		virtual ~cGLWindow()
		{

		}

		o3_begin_class(cWindow)
			o3_add_iface(iWindow)
			o3_add_iface(iWindowProc)
		o3_end_class()

		o3_glue_gen()

		WNDPROC m_def_proc;
		HGLRC mGLContext ;
		HDC mCurrentDC;

		o3_enum("GLCONSTANTS", 
				
			COLOR_BUFFER_BIT  = GL_COLOR_BUFFER_BIT,
			DEPTH_BUFFER_BIT  = GL_DEPTH_BUFFER_BIT,
			TEXTURE_2D = GL_TEXTURE_2D,
			BLEND  = GL_BLEND,
			ALPHA_TEST  = GL_ALPHA_TEST,
			RGB  = GL_RGB,
			RGBA =GL_RGBA,
			MODELVIEW =GL_MODELVIEW,
			PROJECTION = GL_PROJECTION,
			TEXTURE = GL_TEXTURE,
			POINTS = GL_POINTS,
			LINES = GL_LINES,
			QUADS = GL_QUADS,
			QUAD_STRIP = GL_QUAD_STRIP,
			TRIANGLES = GL_TRIANGLES,
			TRIANGLE_STRIP = GL_TRIANGLE_STRIP,
			
		    
			LIGHTING = GL_LIGHTING,
			LIGHT0 = GL_LIGHT0 ,
			LIGHT1 = GL_LIGHT1 ,
			LIGHT2 = GL_LIGHT2 ,
			LIGHT3 = GL_LIGHT3 ,
			LIGHT4 = GL_LIGHT4 ,
			LIGHT5 = GL_LIGHT5 ,
			LIGHT6 = GL_LIGHT6 ,
			LIGHT7 = GL_LIGHT7 ,
			AMBIENT = GL_AMBIENT,
			DIFFUSE = GL_DIFFUSE,
			EMISSION = GL_EMISSION,
			SHININESS = GL_SHININESS,
			SPECULAR = GL_SPECULAR,
			POSITION = GL_POSITION,
			DEPTH_TEST = GL_DEPTH_TEST,
			ALPHA_TEST = GL_ALPHA_TEST,
			CULL_FACE = GL_CULL_FACE,

			FRONT = GL_FRONT, 
			BACK = GL_BACK, 
			FRONT_AND_BACK = GL_FRONT_AND_BACK,

			CCW = GL_CCW,
			CW = GL_CW,
			NORMALIZE = GL_NORMALIZE
			);

		o3_fun void BeginFrame()
		{
			if (mCurrentDC == NULL)
			{
				mCurrentDC  = GetDC(m_hwnd);
				wglMakeCurrent ( mCurrentDC , mGLContext );
			};
		};

		o3_fun void EndFrame()
		{
			if (mCurrentDC != NULL)
			{
				ReleaseDC(m_hwnd, mCurrentDC );
				mCurrentDC  = NULL;
			};
			invalidate();
		};

		o3_fun void invalidate()
		{
			InvalidateRect(m_hwnd,NULL, false);
		};

		o3_fun void SetUniform(iScr* shader, const Str &name, const Var &Value, size_t textureslot = -1)
		{
			siShaderProgram SP(shader);
			if (SP)
			{
				cGLShaderProgram *shad = (cGLShaderProgram *)SP->getThis();
				
				int uniform = glGetUniformLocation(shad->mProgram, name.ptr());
				if (uniform > -1)
				{
					switch (Value.type())
					{
					case Var::TYPE_DOUBLE:
							glUniform1fARB(uniform, (float)Value.toDouble());
							break;
					case Var::TYPE_INT32:
							glUniform1iARB(uniform, Value.toInt32());
							break;
					case Var::TYPE_SCR:
						{
							siScr S(Value.toScr());
							siTexture Tex(S);
							if (Tex)
							{
								cGLTexture *T = (cGLTexture *)Tex->getThis();
								if (textureslot == -1 ) textureslot = uniform;
								glActiveTexture(GL_TEXTURE0 + textureslot);
								glBindTexture(GL_TEXTURE_2D, T->mTextureID);
								glUniform1iARB(uniform, textureslot);
								glActiveTexture(GL_TEXTURE0);
								break;
							}
							siMat44 Mat44(S);
							if (Mat44)
							{
								glUniformMatrix4fv(uniform, 16,false, Mat44->v);
								break;
							}
							siVector4 Vec4(S);
							if (Vec4)
							{
								float V[4] = {Vec4->x, Vec4->y, Vec4->z, Vec4->w};
								::glUniform4f(uniform, V[0],V[1],V[2],V[3]);
								break;
							}
							siVector3 Vec3(S);
							if (Vec3)
							{
								float V[3] = {Vec3->x, Vec3->y, Vec3->z};
								::glUniform3f(uniform, V[0],V[1],V[2]);
								break;
							}
							siVector2 Vec2(S);
							if (Vec2)
							{
								float V[2] = {Vec2->x, Vec2->y};
								::glUniform2f(uniform, V[0],V[1]);
								break;
							};
						};
						break;
					};
				};
			};
		};

		o3_fun void ClearColor(double r,double g, double b, double a)
		{
			glClearColor((float)r,(float)g,(float)b,(float)a);
		};
		
		o3_fun void Material(size_t target, size_t attribute, double r, double g, double b, double a)
		{
			float params[4] = {(float)r, (float)g, (float)b, (float)a};
			glMaterialfv(target, attribute, params);
		};

		o3_fun void Light(size_t target, size_t attribute, double r, double g, double b, double a)
		{
			float params[4] = {(float)r, (float)g, (float)b, (float)a};
			glLightfv(target, attribute, params);
		};

		o3_fun void Clear(size_t bits)
		{
			glClear(bits);
		};

		o3_fun void CullFace(size_t mode)
		{
			glCullFace(mode);
		};

		o3_fun void Enable(size_t bits)
		{
			glEnable(bits);
		};

		o3_fun void Disable(size_t bits)
		{
			glDisable(bits);
		};

		o3_fun void PushMatrix()
		{
			glPushMatrix();
		};

		o3_fun void LoadIdentity()
		{
			glLoadIdentity();
		};

		o3_fun void PopMatrix()
		{
			glPopMatrix();
		};

		o3_fun void Rotate(double angle, double x, double y, double z)
		{
			glRotated(angle, x,y,z);
		};

		o3_fun void PointSize(double ps)
		{
			glPointSize((float)ps);
		};

		o3_fun void LineWidth(double lw)
		{
			glLineWidth((float)lw);
		};

		o3_fun void Vertex3(double x, double y, double z)
		{
			glVertex3d(x,y,z);
		};

		o3_fun void TexCoord2(double x, double y)
		{
			glTexCoord2d(x,y);
		};

		o3_fun void Begin(size_t mode)
		{
			glBegin(mode);
		};

		o3_fun void End()
		{
			glEnd();
		};

		o3_fun void Vertex2(double x, double y)
		{
			glVertex2d(x,y);
		};

		o3_fun void Color3(double r, double g, double b)
		{
			glColor3d(r,g,b);
		};

		o3_fun void Color4(double r, double g, double b, double a)
		{
			glColor4d(r,g,b,a);
		}
		
		o3_fun void Scale(double x, double y, double z)
		{
			glScaled(x,y,z);
		};
		
		o3_fun void Translate(double x, double y, double z)
		{
			glTranslated(x,y,z);
		};

		o3_fun void MatrixMode(size_t Mode)
		{
			glMatrixMode(Mode);
		};
		o3_fun void UseProgram(iScr *inprogram)
		{
			siShaderProgram SP(inprogram);
			if (SP)
			{
				cGLShaderProgram *shad = (cGLShaderProgram *)SP->getThis();
				shad->bind();
			}
			else
			{
				glUseProgramObjectARB(0);
			}
		};
		o3_fun void Render(iScr *vbo, size_t mode = GL_TRIANGLES, size_t startvertex = 0, size_t endvertex = 0xffffffff)
		{
			siVertexArray VA(vbo);
			if (VA)
			{
				cGLVertexArray *arr = (cGLVertexArray *)VA->getThis();
				arr->Render(mode, startvertex, endvertex);
			}
			
		};

		o3_fun void Perspective(double fov, double aspect, double nearplane, double farplane)
		{
			gluPerspective(fov, aspect,nearplane, farplane);
		};

		o3_fun void Ortho2D(double left, double right, double bottom, double top)
		{
			gluOrtho2D(left, right, bottom, top);
		};

		o3_fun void LookAt(double xeye, double yeye, double zeye, double xtarget, double ytarget, double ztarget, double xup, double yup, double zup)
		{
			gluLookAt(xeye,yeye, zeye, xtarget, ytarget, ztarget, xup, yup, zup);
		};

		o3_fun void Viewport(size_t x, size_t y, size_t w, size_t h)
		{
			glViewport(x,y,w,h);
		};

		o3_fun size_t Time()
		{
			return GetTickCount();
		};
		o3_fun void ColorMask(bool r, bool g, bool b, bool a)
		{
			glColorMask(r,g,b,a);
		};

		static o3_ext("cO3") o3_fun siWindow createGLWindow(o3_tgt iScr* target, const char* text, 
			int x, int y, int w, int h)
		{              
			o3_trace_scrfun("createGLWindow");              
			target = 0;
			return create(0,text,x,y,w,h);
		}

		static siWindow create(HWND parent, const char* caption, int x, int y, 
			int width, int height, int style = 0)
		{
			o3_trace_scrfun("create");
			// create the component
			cGLWindow* ret = o3_new(cGLWindow)();

			WNDCLASSW wnd_class;        
			regWndClass(wnd_class);

			// convert o3 style flags to native flags
			DWORD flags = getFlags(style);
			if (parent)
				flags |= WS_CHILD;
			flags |= WS_THICKFRAME ;
			// create the object and the window
			ret->m_hwnd = CreateWindowExW(0,o3_wnd_class_name, 
				WStr(caption).ptr(), flags, x, y, width, height, 
				parent, 0, GetModuleHandle(0), (LPVOID)(iWindowProc*)ret);  

			SetWindowLongPtr( ret->m_hwnd, GWL_USERDATA, (LONG_PTR)(iWindowProc*)ret );

			PIXELFORMATDESCRIPTOR pfd;  
			pfd.cColorBits = pfd.cDepthBits = 32; 
			pfd.dwFlags    = PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;	


			HDC hDC = GetDC (  ret->m_hwnd);     
			int PixelFormat = ChoosePixelFormat ( hDC, &pfd) ;
			SetPixelFormat ( hDC, PixelFormat , &pfd );
			ret->mGLContext = wglCreateContext(hDC);
			wglMakeCurrent ( hDC, ret->mGLContext );
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//			glColorMaterial(GL_BOTH, GL_AMBIENT_AND_DIFFUSE);
			glShadeModel(GL_SMOOTH);
			glDisable(GL_CULL_FACE);
			glEnable(GL_COLOR_MATERIAL);
			glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);
	        glEnable(GL_DEPTH_TEST);
			glEnable(GL_NORMALIZE);

			GLeeInit();
			ReleaseDC(ret->m_hwnd, hDC);

			return ret;
		}

		virtual LRESULT CALLBACK wndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
		{
			o3_trace_scrfun("wndProc");
			siCtx ctx(m_ctx);
			switch(message)
			{
			case WM_ERASEBKGND: return FALSE;
			case WM_PAINT:
				{
					PAINTSTRUCT ps;
					HDC dc = BeginPaint(hwnd, &ps);
					wglMakeCurrent ( dc, mGLContext );
					SwapBuffers(dc);
					EndPaint(hwnd, &ps);
				};
				break;
			default:
				if (m_def_proc)	return m_def_proc(hwnd, message, wparam, lparam);
				return cWindow::wndProc(hwnd, message, wparam, lparam);
				//return(DefWindowProc(hwnd, message, wparam, lparam));
			}
			return 0;
		}

	};

}

#endif 