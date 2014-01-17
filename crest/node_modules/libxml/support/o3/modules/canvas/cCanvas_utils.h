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

namespace o3 
{

    #ifndef __min
    #define __min(X,Y) (((X)<(Y))?(X):(Y))
    #endif

    #ifndef __max
    #define __max(X,Y) (((X)>(Y))?(X):(Y))
    #endif

	// custom io functions for the png lib:	
	void o3_read_data(png::png_structp png_ptr,
		png::png_bytep data, png::png_size_t length) 
	{
		o3_trace_scrfun("o3_read_data");
		siStream stream = (iStream*) png_ptr->io_ptr;
		if ( !stream || length != 
			stream->read((void*) data, (size_t) length )) 
		{				
			png::png_error(png_ptr, "PNG file read failed.");
		}
	}

	void o3_read_data_bufstream(png::png_structp png_ptr,
		png::png_bytep data, png::png_size_t length) 
	{
		o3_trace_scrfun("o3_read_data_bufstream");
		cBufStream *stream = (cBufStream *) png_ptr->io_ptr;
		if ( !stream || length != 
			stream->read((void*) data, (size_t) length )) 
		{				
			png::png_error(png_ptr, "PNG file read failed.");
		}
	}

	void o3_write_data(png::png_structp png_ptr,
		png::png_bytep data, png::png_size_t length)
	{
		o3_trace_scrfun("o3_write_data");
		siStream stream = (iStream*) png_ptr->io_ptr;
		if ( !stream || length != 
			stream->write((void*) data, (size_t) length )) 
		{				
			png::png_error(png_ptr, "PNG file write failed.");
		}
	}

	void o3_write_data_bufstream(png::png_structp png_ptr,
		png::png_bytep data, png::png_size_t length)
	{
		o3_trace_scrfun("o3_write_data_bufstream");
		Buf *buf= (Buf*) png_ptr->io_ptr;
		buf->append((void*) data, (size_t) length);
	}

	void o3_flush_data(png::png_structp png_ptr)
	{
		o3_trace_scrfun("o3_flush_data");
		siStream stream = (iStream*) png_ptr->io_ptr;
		if ( !stream ) 
		{				
			png::png_error(png_ptr, "PNG file flush failed.");
		} 
		else
		{
			stream->flush();
		}
	}

	void o3_flush_data_bufstream(png::png_structp png_ptr)
	{		
		o3_trace_scrfun("o3_flush_data_bufstream");		
		png_ptr;
	}

	bool decodeColor(const Str &style, unsigned int *color)
	{
		o3_trace_scrfun("decodeColor");
		unsigned int stylesize = style.size();
		if (stylesize>0)
		{
			if (style[0] == '#')
			{
				int HexDigitsFound = 0;
				int digits[6];
				char *d = (char * ) (style.ptr()) + 1;
				for (;*d;d++)
				{
					switch (*d)
					{
					case '0':
					case '1':
					case '2':
					case '3':
					case '4':
					case '5':
					case '6':
					case '7':
					case '8':
					case '9':
						digits[HexDigitsFound++] = *d-'0';
						break;
					case 'a':
					case 'b':
					case 'c':
					case 'd':
					case 'e':
					case 'f':
						digits[HexDigitsFound++] = (*d-'a')+10;
						break;
					case 'A':
					case 'B':
					case 'C':
					case 'D':
					case 'E':
					case 'F':
						digits[HexDigitsFound++] = (*d-'A')+10;
						break;
					};
					if (HexDigitsFound==6)
					{
						unsigned char Res[3] = 
						{
							(unsigned char)((digits[0]<<4)+ digits[1]),
							(unsigned char)((digits[2]<<4)+ digits[3]),
							(unsigned char)((digits[4]<<4)+ digits[5])
						};
						*color = 0xff000000 + (Res[0]<<16) + (Res[1]<<8) + Res[2];
						return true;
					};
				}

				if (HexDigitsFound > 2 )
				{
					unsigned char Res[3] = 
					{
						(unsigned char)((digits[0]<<4)+ digits[0]),
						(unsigned char)((digits[1]<<4)+ digits[1]),
						(unsigned char)((digits[2]<<4)+ digits[2])
					};
					*color = 0xff000000 + (Res[0]<<16) + (Res[1]<<8) + Res[2];
					return true;
				};

				return false;					

			}
			unsigned int internalcolor = 0;
			unsigned int index = 0;
			while (internalcolor < 0x0f000000)
			{

				if (index > stylesize) 
				{					
					return false;
				}
				else
				{
					int c;
					if (index == stylesize)	
					{
						c = 'q'-'a';
					}
					else
					{
						c = style[index];
						if (c>='a' && c<='z')
						{
							c-='a';
						}
						else
						{
							if (c>='A' && c<='Z')
							{
								c-='A';
							}
							else
							{
								c='q'-'a';
							}
						}
					};
					index++;
					internalcolor = css_lut[internalcolor][c];
				};
			}
			unsigned int masked = internalcolor&0xff000000;

			if (masked == 0xff000000)
			{
				*color = internalcolor;
				return true;
			};

			int totalchannels;

			double AlphaRes=0;

			if (masked == 0x2f000000)
			{
				totalchannels = 4;
			}
			else
			{
				totalchannels = 3;
				AlphaRes = 1;
			}

			int Res[3]={0,0,0};
			int current = 0;

			//				int val = 0;
			bool afterdot = false;
			int digitsafterdot = 0;
			char *d = (char * ) (style.ptr());
			for (;*d;d++)
			{
				if (*d>='0' && *d<='9') 
				{
					if (current<3)
					{
						Res[current] *= 10;
						Res[current] += (*d-'0');
					}
					else
					{
						AlphaRes *= 10;
						AlphaRes += (*d-'0');
						if (afterdot) digitsafterdot++;
					}
				}
				else
				{
					if (*d ==',')
					{
						current ++;
						if (current == totalchannels)break;

					}
					if (current == 3 && *d == '.')
					{
						afterdot = true;
					}
				}
			}
			while (digitsafterdot > 0) 
			{ 
				AlphaRes /= 10;digitsafterdot--;
			};
			*color = (Res[0]<<16) + (Res[1]<<8) + Res[2] + ((int)(AlphaRes*255.0f)<<24);

			return true;

		}
		else
		{
			return false;
		};
	};
};

