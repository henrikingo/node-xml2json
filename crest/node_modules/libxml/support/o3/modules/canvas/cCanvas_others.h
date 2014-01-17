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
#include <map>

namespace o3 
{

	
	o3_iid(iCanvasGradient, 0x763992e8, 0x17cc, 0x4dd5, 0xa7, 0xb1, 0x30, 0x9c, 0x19, 0xdf, 0xa9, 0xe2);


	struct iCanvasGradient: iUnk 
	{
		virtual void *GetActualGradientPointer() = 0;
	};


	struct cImage_CanvasGradientData
	{
		int m_type;
		V2<double> m_CP1;
		double m_Radius1;
		V2<double> m_CP2;
		double m_Radius2;
		tMap<double, unsigned int> m_colorstops;
		
		void FillGradientArray(agg::Agg2D::GradientArray *inArray, double globalAlpha)
		{
			o3_trace_scrfun("FillGradientArray");
			int Count = m_colorstops.size();
			if (Count == 0)
			{

				return;
			}
			tVec<agg::Agg2D::Color> Colors(Count);
			tVec<int> Offsets(Count);

			
			for (tMap<double, unsigned int>::ConstIter i = ((const tMap<double, unsigned int>*)&m_colorstops)->begin();i!= ((const tMap<double, unsigned int>*)&m_colorstops)->end();i++)				
			{
				unsigned int color =  (*i).val;
				unsigned char *cc = (unsigned char *)&color;

				agg::Agg2D::Color C(cc[2], cc[1], cc[0], (unsigned int)(cc[3] * globalAlpha));

				Colors.push(C);;
				Offsets.push((unsigned int)floor((*i).key * 255.0));
			};
			
			
			
			if (Offsets[0] > 0)
			{
				Offsets.insert(0,0);
				Colors.insert(0,agg::Agg2D::Color(0,0,0,(unsigned int)(255* globalAlpha)));
				Count++;
			};

			if (Count == 1)
			{
				for (unsigned int i = 0;i<256;i++)
				{
					(*inArray)[i] = Colors[0];
				};
				return;
			}
			int colorindex = 1;
			agg::Agg2D::Color *First = &Colors[0];
			agg::Agg2D::Color *Second = &Colors[1];
			double length = Offsets[colorindex]-Offsets[colorindex-1];
			for (int i = 0;i<256;i++)
			{
				if (Offsets[colorindex] <= i)
				{
					colorindex++;
					First = Second;
					if (colorindex < Count)
					{						
						Second = &Colors[colorindex];
						length = Offsets[colorindex]-Offsets[colorindex-1];
					}
					else
					{
						for (;i<256;i++)
						{
							(*inArray)[i] = *Second;
						};
						return;
					}
				}
				double fade = (double)(i-Offsets[colorindex-1])/length ;
				(*inArray)[i] = (*First).gradient(*Second, fade);
			};

			
		};
		
	};

	struct cImage_CanvasGradient: cScr , iCanvasGradient
	{
		o3_begin_class(cScr)
			o3_add_iface(iCanvasGradient)
		o3_end_class()
		o3_glue_gen();

		virtual void *GetActualGradientPointer() { o3_trace_scrfun("GetActualGradientPointer"); return this;};

		enum Types
		{
			GRADIENT_LIN,
			GRADIENT_RAD,
			__Type_Count
		};

		o3_fun void addColorStop(double offset, const Str &colorstring)
		{
			o3_trace_scrfun("addColorStop");
			if (offset < 0.0 || offset > 1.0) return;
			unsigned int color = 0;
			decodeColor(colorstring, &color);
			while(mData.m_colorstops.find(offset) != mData.m_colorstops.end()) offset+=0.00001;
			mData.m_colorstops[offset] = color;			
		};
		
		cImage_CanvasGradientData mData;		
	};

	struct cImage_CanvasPattern: cScr 
	{
		o3_begin_class(cScr)
		o3_end_class()

		o3_glue_gen();
	};

	struct cImage_TextMetrics: cScr 
	{
		o3_begin_class(cScr)
		o3_end_class()
		
		o3_glue_gen();

		o3_get double width(){o3_trace_scrfun("width");return mWidth;};

		double mWidth;
	};

	struct cImage_CanvasPixelArray: cScr 
	{
		o3_begin_class(cScr)
		o3_end_class()
		
		o3_glue_gen();

		tVec<unsigned char> mData;
       
		o3_fun bool __query__(int idx) 
		{       
			o3_trace_scrfun("__query__");       
			if (idx<0 || ((size_t)idx)>=mData.size())
				return false;
			return true;
        } 
        
        o3_fun Var __getter__(iCtx* ctx, int idx, siEx* ex = 0) 
		{
            o3_trace_scrfun("__getter__");
            o3_unused(ctx);
            ex;
            return Var(item(idx));
        }

        unsigned char item(int index) 
		{
            o3_trace_scrfun("item");
			if (index<0 || ((size_t)index)>=mData.size())
				return 0;
			return mData[index];			
        }
        
        virtual o3_get int length() 
		{
            o3_trace_scrfun("length");
            return mData.size();           
        }
	};

	struct cImage_ImageData: cScr
	{
		o3_begin_class(cScr)
		o3_end_class()
	
		o3_glue_gen();

		cImage_CanvasPixelArray mStorage;

		o3_get size_t width()
		{
			o3_trace_scrfun("width");
			return 0;
		};

		o3_get size_t height()
		{
			o3_trace_scrfun("height");
			return 0;
		};

//		o3_get tSi<cImage_CanvasPixelArray> data();

		o3_get siScr data()
		{
			o3_trace_scrfun("data");
			return siScr(&mStorage);
		}
	};




	struct cImage_CanvasRenderingContext2D
	{
		// interface taken from http://www.whatwg.org/specs/web-apps/current-work/multipage/the-canvas-element.html

		void save(); // push state on state stack
		void restore(); // pop state stack and restore state

		// transformations (default transform is the identity matrix)
		void scale(double x, double y);
		void rotate(double angle);
		void translate(double x, double y);
		void transform(double a, double b, double c, double d, double e, double f);
		void setTransform(double a, double b, double c, double d, double e, double f);

		// compositing
		//attribute 
		double globalAlpha; // (default 1.0)
		//attribute 
		Str globalCompositeOperation; // (default source-over)

		// colors and styles
		//attribute 
		Str strokeStyle; // (default black)
		//attribute 
		Str fillStyle; // (default black)
		
		cImage_CanvasGradient createLinearGradient(double x0, double y0, double x1, double y1);
		cImage_CanvasGradient createRadialGradient(double x0, double y0, double r0, double x1, double y1, double r1);
		
		// these calls need communication to a dom -> 
		//CanvasPattern createPattern(HTMLImageElement image, const Str & repetition);
		//CanvasPattern createPattern(HTMLCanvasElement image, const Str & repetition);
		//CanvasPattern createPattern(HTMLVideoElement image, const Str &repetition);

		// line caps/joins
		//attribute 
		double lineWidth; // (default 1)
		//attribute 
		Str lineCap; // "butt", "round", "square" (default "butt")
		//attribute 
		Str lineJoin; // "round", "bevel", "miter" (default "miter")
		//attribute 
		double miterLimit; // (default 10)

		// shadows
		//attribute 
		double shadowOffsetX; // (default 0)
		//attribute 
		double shadowOffsetY; // (default 0)
		//attribute 
		double shadowBlur; // (default 0)
		//attribute 
		Str shadowColor; // (default transparent black)

		// rects
		void clearRect(double x, double y, double w, double h);
		void fillRect(double x, double y, double w, double h);
		void strokeRect(double x, double y, double w, double h);

		// path API
		void beginPath();
		void closePath();
		void moveTo(double x, double y);
		void lineTo(double x, double y);
		void quadraticCurveTo(double cpx, double cpy, double x, double y);
		void bezierCurveTo(double cp1x, double cp1y, double cp2x, double cp2y, double x, double y);
		void arcTo(double x1, double y1, double x2, double y2, double radius);
		void rect(double x, double y, double w, double h);
		void arc(double x, double y, double radius, double startAngle, double endAngle);
		void arc(double x, double y, double radius, double startAngle, double endAngle, bool anticlockwise);

		void fill();
		void stroke();
		void clip();
		bool isPointInPath(double x, double y);

		// focus management
		// needs communication with the dom
		// boolean drawFocusRing(Element element, double xCaret, double yCaret, in optional boolean canDrawCustom);

		// text
		//attribute 
		Str font; // (default 10px sans-serif)
		//attribute 
		Str textAlign; // "start", "end", "left", "right", "center" (default: "start")
		//attribute 
		Str textBaseline; // "top", "hanging", "middle", "alphabetic", "ideographic", "bottom" (default: "alphabetic")
		
		void fillText(const Str & text, double x, double y);
		void fillText(const Str & text, double x, double y, double maxWidth);
		void strokeText(const Str & text, double x, double y);
		void strokeText(const Str & text, double x, double y, double maxWidth);
		cImage_TextMetrics measureText(const Str & text);

		// drawing images
		// these calls need communication to a dom -> 
		//void drawImage(in HTMLImageElement image, double dx, double dy, in optional double dw, double dh);
		//void drawImage(in HTMLImageElement image, double sx, double sy, double sw, double sh, double dx, double dy, double dw, double dh);
		//void drawImage(in HTMLCanvasElement image, double dx, double dy, in optional double dw, double dh);
		//void drawImage(in HTMLCanvasElement image, double sx, double sy, double sw, double sh, double dx, double dy, double dw, double dh);
		//void drawImage(in HTMLVideoElement image, double dx, double dy, in optional double dw, double dh);
		//void drawImage(in HTMLVideoElement image, double sx, double sy, double sw, double sh, double dx, double dy, double dw, double dh);

		// pixel manipulation
		cImage_ImageData createImageData(double sw, double sh);
		cImage_ImageData createImageData(cImage_ImageData *imagedata);
		cImage_ImageData getImageData(double sx, double sy, double sw, double sh);
		
		void putImageData(cImage_ImageData *imagedata, double dx, double dy);
		void putImageData(cImage_ImageData *imagedata, double dx, double dy, double dirtyX, double dirtyY, double dirtyWidth, double dirtyHeight);
	};
};
