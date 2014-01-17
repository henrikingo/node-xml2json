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
#ifndef O3_C_KINECT_H
#define O3_C_KINECT_H

#include <libfreenect.h>

#include "canvas/canvas.h"
#include <vector>

namespace o3 
{

	freenect_context *gFreenectContext= NULL;
	int gFreenectContextCount = 0;
	void FreenectLog(freenect_context *dev, freenect_loglevel level, const char *msg, ...)
	{
		// nothing logged yet...
	};

	void gDepthCallback(freenect_device *dev, freenect_depth *depth, uint32_t timestamp);
	void gRGBCallback(freenect_device *dev, freenect_pixel *rgb, uint32_t timestamp);

	struct cDepthBuffer: cScr
	{
		o3_begin_class(cScr)
		o3_end_class()
		
		cDepthBuffer(unsigned int W, unsigned int H)
		{
			mData = NULL;
			mWidth = W;
			mHeight = H;

			if (W*H == 0) return;
			mData = new unsigned short[W*H];
		};

		virtual ~cDepthBuffer()
		{
			if (mData) delete [] mData;
		};
		unsigned short *getRowPtr(size_t y)
		{
			if (y<mHeight) return mData + (mWidth * y);
			return NULL;
		};
		size_t mWidth, mHeight;
		unsigned short *mData;

		o3_glue_gen()
	};

	struct cKinect : cScr
	{
		o3_begin_class(cScr)
		o3_end_class()

		o3_glue_gen()

		freenect_device *mDevice;

		static o3_ext("cO3") o3_fun siScr kinect(size_t index= 0)
		{              
			cKinect* ret = o3_new(cKinect)(index);
			return ret;		
		}

		int mRGBFrameCount;
		int mDepthFrameCount;
		int mRGBFrame;
		int mDepthFrame;
		unsigned char mRGBFrameStore[640*480*3];

		cDepthBuffer mDepthFrameStore;

		cKinect(size_t kinectnumber = 0): mDepthFrameStore(640,480)
		{
			mRGBFrameCount = 0;
			mRGBFrame = 0;
			mDepthFrameCount = 0;
			mDepthFrame = 0;
			mDevice = NULL;
			if (gFreenectContextCount == 0)
			{
				freenect_init(&gFreenectContext, NULL);
				freenect_set_log_level(gFreenectContext, FREENECT_LOG_FLOOD);
				freenect_set_log_callback(gFreenectContext, (freenect_log_cb) FreenectLog);
			}

			gFreenectContextCount++;


			unsigned int DeviceCount = freenect_num_devices(gFreenectContext);
			if (DeviceCount > 0)
			{
				if (kinectnumber>=DeviceCount) kinectnumber = 0;
				freenect_open_device(gFreenectContext, &mDevice, kinectnumber);
				if (mDevice)
				{
					freenect_set_user(mDevice, (void*)this);
					freenect_set_depth_callback(mDevice, gDepthCallback);
					freenect_set_rgb_callback(mDevice, gRGBCallback);
					freenect_start_depth(mDevice);
					freenect_start_rgb(mDevice);	
				};
			};
		};

		o3_fun void processEvents()
		{
			if (gFreenectContext) freenect_process_events(gFreenectContext);
		};

		void DepthCallback(unsigned short *depth)
		{
			mDepthFrameCount++;
			memcpy(mDepthFrameStore.mData, depth, sizeof(unsigned short)*640*480);
		};

		void RGBCallback(unsigned char *rgb)
		{
			mRGBFrameCount++;
			memcpy(mRGBFrameStore, rgb, 3*640*480);
		};

		o3_fun bool newDepthAvailable()
		{
			if (mDepthFrameCount > mDepthFrame)
			{
				mDepthFrame = mDepthFrameCount;
				return true;
			};
			return false;
		};

		o3_fun bool newRGBAvailable()
		{
			if (mRGBFrameCount > mRGBFrame)
			{
				mRGBFrame = mRGBFrameCount;
				return true;
			};
			return false;
		};		

		o3_fun double depthToCM(size_t Depth)
		{

			double div= (Depth * -0.0030711016 + 3.3309495161);
			if (div != 0)
			{
				return 100.0 / div;	
			}
			else
			{
				return 0;
			}
		};

		o3_fun size_t CMToDepth(double CM)
		{
			if (CM == 0) return 0;
			return ( (100.0 / CM) - 3.3309495161) / -0.0030711016;
		};
		struct BlobCoordinate
		{
			unsigned int x, y;
			void * data;
		};
		struct Blob
		{
			//unsigned int blobId;
			BlobCoordinate minc, maxc;
			double centerx, centery;
			double z;
			int size;
		};


		std::vector<Blob> mBlobs;

		o3_fun double blobX(size_t ID){if (ID<blobCount()) return mBlobs[ID].centerx;return 0;};
		o3_fun double blobY(size_t ID){if (ID<blobCount()) return mBlobs[ID].centery;return 0;};;
		o3_fun double blobZ(size_t ID){if (ID<blobCount()) return mBlobs[ID].z;return 0;};
		o3_fun double blobSize(size_t ID){if (ID<blobCount()) return mBlobs[ID].size;return 0;};

		o3_fun size_t blobCount()		
		{
			return mBlobs.size();
		};



		struct BlobLineBlob
		{
			unsigned int minc, maxc;
			unsigned int blobId;

			bool attached;
		};


		struct BlobFrame
		{
			int width, height;
			unsigned short *imageData;
		};

		void DetectBlobs(BlobFrame *frame, std::vector<Blob> *OutBlobs, unsigned short minv, unsigned short maxv)
		{

			int blobCounter = 0;
			std::map<unsigned int, Blob> blobs;

			float threshold = 235;


			std::vector< std::vector<BlobLineBlob> > imgData(frame->width);

			for(int row = 0; row < frame->height; ++row)
			{

				for(int column = 0; column < frame->width; ++column)
				{

					//unsigned char byte = (unsigned char) imgStream.get();
					float val = frame->imageData[(row*frame->width)+ column];
//#define inRange(x) (x>=minv && x<maxv)
#define inRange(x) (x>0)
					if(inRange(val))
					{
						int start = column;

						for(;inRange(val); val = frame->imageData[(row*frame->width)+ column], ++column);

						int stop = column-1;
						BlobLineBlob lineBlobData = {start, stop, blobCounter, false};

						imgData[row].push_back(lineBlobData);
						blobCounter++;
					}
				}
			}

			/* Check lineBlobs for a touching lineblob on the next row */

			for(unsigned int row = 0; row < imgData.size(); ++row)
			{

				for(unsigned int entryLine1 = 0; entryLine1 < imgData[row].size(); ++entryLine1)
				{

					for(unsigned int entryLine2 = 0; entryLine2 < imgData[row+1].size(); ++entryLine2)
					{

						if(!((imgData[row][entryLine1].maxc < imgData[row+1][entryLine2].minc) || (imgData[row][entryLine1].minc > imgData[row+1][entryLine2].maxc)))
						{

							if(imgData[row+1][entryLine2].attached == false)
							{

								imgData[row+1][entryLine2].blobId = imgData[row][entryLine1].blobId;

								imgData[row+1][entryLine2].attached = true;
							}
							else

							{
								imgData[row][entryLine1].blobId = imgData[row+1][entryLine2].blobId;

								imgData[row][entryLine1].attached = true;
							}
						}
					}
				}
			}

			// Sort and group blobs

			for(int row = 0; row < (int)imgData.size(); ++row)
			{

				for(int entry = 0; entry < (int)imgData[row].size(); ++entry)
				{

					if(blobs.find(imgData[row][entry].blobId) == blobs.end()) // Blob does not exist yet

					{
						Blob blobData = {{imgData[row][entry].minc, row}, {imgData[row][entry].maxc, row}, 0,0,0,0};

						blobs[imgData[row][entry].blobId] = blobData;
					}
					else

					{
						if(imgData[row][entry].minc < blobs[imgData[row][entry].blobId].minc.x)

							blobs[imgData[row][entry].blobId].minc.x = imgData[row][entry].minc;

						else if(imgData[row][entry].maxc > blobs[imgData[row][entry].blobId].maxc.x)

							blobs[imgData[row][entry].blobId].maxc.x = imgData[row][entry].maxc;

						if(row < (int)blobs[imgData[row][entry].blobId].minc.y)

							blobs[imgData[row][entry].blobId].minc.y = row;

						else if(row > (int)blobs[imgData[row][entry].blobId].maxc.y)

							blobs[imgData[row][entry].blobId].maxc.y = row;
					}
				}
			}

			// Calculate center
			for(std::map<unsigned int, Blob>::iterator i = blobs.begin(); i != blobs.end(); ++i)
			{
				(*i).second.centerx = (*i).second.minc.x + ((*i).second.maxc.x - (*i).second.minc.x) / 2;
				(*i).second.centery = (*i).second.minc.y + ((*i).second.maxc.y - (*i).second.minc.y) / 2;

				(*i).second.size = ((*i).second.maxc.x - (*i).second.minc.x) * ((*i).second.maxc.y - (*i).second.minc.y);					
				OutBlobs->push_back(i->second);
			}
		}


		o3_fun size_t findBlobs(size_t lowerbound, size_t upperbound, size_t subdiv = 10, size_t dilate_iterations=2)
		{
			if (subdiv < 1) subdiv = 1;
			mBlobs.clear();
			int div = subdiv;
			BlobFrame BF;
			BF.width = 640/div;
			BF.height = 480/div;
			BF.imageData = new unsigned short [BF.width*BF.height];
			unsigned short *dst = BF.imageData;
			
			for (unsigned int y = 0;y < 480;y += div)
			{
				unsigned short *src = mDepthFrameStore.getRowPtr(y);
				for (unsigned int x = 0;x < 640;x += div)
				{
					unsigned short val = *src;
					if (val>lowerbound && val <=upperbound) *dst++ = 255;else *dst++= 0;
					src += div;					
				}
			}

			if (dilate_iterations)
			{
				unsigned short *dilatetarget = new unsigned short [BF.width*BF.height];
				
				unsigned short *dst = dilatetarget;
				unsigned short *src = BF.imageData;

				for (size_t i = 0; i < dilate_iterations; i++)
				{
					unsigned short *toprow = dst;
					unsigned short *bottomrow = dst + (BF.height-1)*BF.width;

					for (unsigned int x = 0;x<(unsigned int)BF.width;x++)
					{
						*toprow++ = 0;
						*bottomrow++ = 0;
					};
						
					for (unsigned int y = 1;y<(unsigned int)BF.height-1;y++)
					{
						
						unsigned short *dst1 = dst+(y*BF.width);
						dst1[0] = 0;dst1[BF.width-1] = 0;
						unsigned short *src1 = src+(y-1)*BF.width;
						
						for (unsigned int x = 1;x<(unsigned int)BF.width-1;x++)
						{
							if (src1[x-1]           || src1[x]           || src1[x+1]   ||
								src1[x-1+BF.width]  || src1[x+1+BF.width]|| 
								src1[x-1+BF.width*2]|| src1[x+BF.width*2]|| src1[x+1+BF.width*2])
							{
								*dst1++ = 255;
							}
							else 
							{
								*dst1++ = 0;
							};

						};
					};

					unsigned short *T = dst;
					dst = src;
					src = T;
										
				};
				
				if (dst != BF.imageData)
				{
					memcpy(BF.imageData, dst, BF.width*BF.height*sizeof(unsigned short));
				};

				delete [] dilatetarget;			
			};

			DetectBlobs(&BF, &mBlobs, lowerbound, upperbound);
			delete [] BF.imageData ;
			return mBlobs.size();
		};

		o3_fun void depthThreshold(iScr *canvas, size_t lowerbound, size_t upperbound)
		{
			siImage img(canvas);
			if (img)
			{
				switch (img->mode_int())					
				{
				case Image::MODE_ARGB:
					{
						int w = __min(img->width(), 640);
						int h = __min(img->height(), 480);
						unsigned short S;
						for (int y =0;y<h;y++)
						{							
							unsigned char *dst = img->getrowptr(y);

							for (int x = 0;x<w;x++)
							{
								S = dst[2] + (dst[1]<<8);
								if (S>=lowerbound && S<=upperbound)
								{
									dst[0] = 255;
									dst[1] = 255;
									dst[2] = 255;
								}
								else
								{
									dst[0] = 0;
									dst[1] = 0;
									dst[2] = 0;
								}
								dst+=4;
							};
						}
					};
					break;
				case Image::MODE_RGB:
					{
						int w = __min(img->width(), 640);
						int h = __min(img->height(), 480);
						for (int y =0;y<h;y++)
						{
							unsigned char *src = mRGBFrameStore + 640*y;
							unsigned char *dst = img->getrowptr(y);
							for (int x = 0;x<w;x++)
							{
								dst[2]  = *src++;
								dst[1]  = *src++;
								dst[0]  = *src++;
								dst+=3;
							};
						}
					};
					break;
				};					
			};

		}


		o3_fun void RGBToCanvas(iScr *canvas)
		{
			siImage img(canvas);
			if (img)
			{
				switch (img->mode_int())					
				{
				case Image::MODE_ARGB:
					{
						int w = __min(img->width(), 640);
						int h = __min(img->height(), 480);
						for (int y =0;y<h;y++)
						{
							unsigned char *src = mRGBFrameStore + (640*y*3);
							unsigned char *dst = img->getrowptr(y);
							for (int x = 0;x<w;x++)
							{
								dst[2]  = *src++;
								dst[1]  = *src++;
								dst[0]  = *src++;
								dst[3] = 255;
								dst+=4;
							};
						}
					};
					break;
				case Image::MODE_RGB:
					{
						int w = __min(img->width(), 640);
						int h = __min(img->height(), 480);
						for (int y =0;y<h;y++)
						{
							unsigned char *src = mRGBFrameStore + 640*y;
							unsigned char *dst = img->getrowptr(y);
							for (int x = 0;x<w;x++)
							{
								dst[2]  = *src++;
								dst[1]  = *src++;
								dst[0]  = *src++;
								dst+=3;
							};
						}
					};
					break;
				};					
			};
		};

		o3_fun void DepthToCanvas(iScr *canvas)
		{
			siImage img(canvas);
			if (img)
			{
				switch (img->mode_int())					
				{
				case Image::MODE_ARGB:
					{
						int w = __min(img->width(), 640);
						int h = __min(img->height(), 480);
						for (int y =0;y<h;y++)
						{
							unsigned short *src = mDepthFrameStore.getRowPtr(y);
							unsigned char *dst = img->getrowptr(y);
							for (int x = 0;x<w;x++)
							{
								unsigned short const S = *src++;
								dst[2]  = S&0xff;
								dst[1]  = S>>8;
								dst[0]  = 0;
								dst[3] = 255;
								dst+=4;
							};
						}
					};
					break;
				case Image::MODE_RGB:
					{
						int w = __min(img->width(), 640);
						int h = __min(img->height(), 480);
						for (int y =0;y<h;y++)
						{
							unsigned short *src = mDepthFrameStore.getRowPtr(y);
							unsigned char *dst = img->getrowptr(y);
							for (int x = 0;x<w;x++)
							{
								unsigned short const S = *src++;
								dst[2]  = S&0xff;
								dst[1]  = S>>8;
								dst[0]  = 0;
								dst+=3;
							};
						}
					};
					break;
				};					
			};
		};

		o3_fun void DepthToVBO(iScr *vbo)
		{
		};

		virtual ~cKinect()
		{
			if (mDevice)
			{
				freenect_close_device(mDevice);
				mDevice = NULL;
			}
			gFreenectContextCount--;
			if (gFreenectContextCount == 0)
			{
				freenect_shutdown(gFreenectContext);
				gFreenectContext = NULL;
			};
		};
	};

	void gDepthCallback(freenect_device *dev, freenect_depth *depth, uint32_t timestamp)
	{
		cKinect *K = (cKinect *)freenect_get_user(dev);
		if (K)
		{
			K->DepthCallback(depth);
		};
	};

	void gRGBCallback(freenect_device *dev, freenect_pixel *rgb, uint32_t timestamp)
	{
		cKinect *K = (cKinect *)freenect_get_user(dev);
		if (K)
		{
			K->RGBCallback(rgb);
		};
	};
}

#endif 