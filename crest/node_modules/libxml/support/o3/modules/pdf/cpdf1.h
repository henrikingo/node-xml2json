#ifndef O3_C_PDF1_H
#define O3_C_PDF1_H

//#include <core/Buf.h>
#include <math.h>
#include <lib_zlib.h>

namespace o3 
{
	namespace pdf
	{
		enum
		{
			type_int,
			type_bool,
			type_float,
			type_name,
			type_string,
			type_dictionary,
			type_array,
			type_null,
			type_stream,
			type_undefined,
			__type_count,

		};
	};

	struct cPdf1_Object
	{
		tVec<unsigned char> mData;		

		unsigned long mType;
		int mID;
		unsigned long mOffset;
		
		cPdf1_Object()
		{
			o3_trace_scrfun("cPdf1_Object");
			mID = -1;
			mType = pdf::type_undefined;
		};
		
		virtual ~cPdf1_Object()
		{
		}

		void SetType(unsigned long newtype)
		{
			o3_trace_scrfun("SetType");
			newtype;
			// mm?
		};

		unsigned long GetType()
		{
			o3_trace_scrfun("GetType");
			return mType;
		};

		unsigned long GetLength()
		{
			o3_trace_scrfun("GetLength");
			return mData.size();
		};
		
		virtual void PrepareData(){o3_trace_scrfun("PrepareData");;};
		virtual int GetSize(){o3_trace_scrfun("GetSize");return 0;};
		
		unsigned char *GetData()
		{
			o3_trace_scrfun("GetData");
			if (mData.size() == 0) return NULL;
			return &mData[0];
		};

		
		void WriteBytes(unsigned char *bytes, unsigned int len)
		{
			o3_trace_scrfun("WriteBytes");
			for (unsigned int i = 0;i<len;i++) mData.push(bytes[i]);
		};
		
		void WriteString(Str data)
		{
			o3_trace_scrfun("WriteString");
			WriteBytes((unsigned char*)data.ptr(), data.size());
			WriteBytes((unsigned char*)"\n",1);
		}	

		Str Escape(Str in)
		{
			// todo.. do proper escaping as the PDF-spec says
			o3_trace_scrfun("Escape");
			// todo.. do proper escaping as the PDF-spec says
			return in;
		};

	};	

	struct cPdf1_Dictionary: public cPdf1_Object
	{
	public:

		virtual ~cPdf1_Dictionary()
		{
			o3_trace_scrfun("~cPdf1_Dictionary");
			mValues.clear();
			mValueOrder.clear();
		};

		virtual void PrepareData()
		{
			o3_trace_scrfun("PrepareData");
			mData.clear();
			WriteString("<<");
			
			for (unsigned int i = 0;i<mValueOrder.size();i++)
			{				
				Str ss;
				ss += "/";
				ss += mValueOrder[i];
				ss += " ";
				ss += mValues[mValueOrder[i]];
				if (ss.size()>0)
				{
					WriteBytes((unsigned char *)ss.ptr(), ss.size());
					if (ss[ss.size()-1] != '\n')
					{
						WriteString("");
					}
				}
			};
			
			WriteString(">>");
		}

		void AddKey_Obj(Str A, cPdf1_Object *O)
		{
			o3_trace_scrfun("AddKey_Obj");
			mValueOrder.push(A);
			O->PrepareData();
			if (O->GetLength()>0)
			{
				Str R;
				R.resize(O->GetLength());
				char *D = (char*)O->GetData();
				for (unsigned int i =0 ;i<O->GetLength();i++)
				{
					R[i] = D[i];
				};
//				R[O->GetLength()] = 0;
				mValues[A] = R;
			};
		};

		void AddKey_Name(Str A, Str B)
		{
			o3_trace_scrfun("AddKey_Name");
			mValueOrder.push(A);
			mValues[A] = "/"+ B;
		}

		void AddKey_String(Str A, Str B)
		{
			o3_trace_scrfun("AddKey_String");
			mValueOrder.push(A);
			mValues[A] = "("+ Escape(B) + ")";
		}

		void AddKey_LiteralString(Str A, Str B)
		{
			o3_trace_scrfun("AddKey_LiteralString");
			mValueOrder.push(A);
			mValues[A] = B;
		}

		void AddKey_Int(Str A, int B)
		{
			o3_trace_scrfun("AddKey_Int");
			mValueOrder.push(A);
			Str ss = Str::fromInt32(B);
			mValues[A] = ss;
		}

		void AddKey_ObjRef(Str A, int B)
		{
			o3_trace_scrfun("AddKey_ObjRef");
			mValueOrder.push(A);
			Str ss;
			ss= Str::fromInt32(B+1) + " 0 R";
			mValues[A] = ss;
		}

		tMap<Str, Str> mValues;
		tVec<Str> mValueOrder;
	};

	class cPdf1_Array: public cPdf1_Object
	{
	public:
		void AddObj(cPdf1_Object *O)
		{
			o3_trace_scrfun("AddObj");
			mObjects.push(O);
		};

		virtual void PrepareData()
		{
			o3_trace_scrfun("PrepareData");
			mData.clear();
			WriteBytes((unsigned char*)"[", 1);
			for (unsigned int i =0 ;i<mObjects.size();i++)
			{
				mObjects[i]->PrepareData();
				unsigned long L = mObjects[i]->GetLength();
				unsigned char *D = mObjects[i]->GetData();
				WriteBytes(D, L);
				//if (i<mObjects.size() -1 )
					WriteBytes((unsigned char*)" ", 1);
			};
			WriteBytes((unsigned char*)"]", 1);
		};
		tVec<cPdf1_Object *> mObjects;
	};

	class cPdf1_ObjectReference: public cPdf1_Object
	{
	public:
		cPdf1_ObjectReference(int ID = -1, int Gen = 0)
		{
			o3_trace_scrfun("cPdf1_ObjectReference");
			mRefID = ID;
			mGenID = Gen;
		};

		virtual void PrepareData()
		{
			o3_trace_scrfun("PrepareData");
			mData.clear();
			Str ss;
			ss = Str::fromInt32(mRefID+1) + " " + Str::fromInt32(mGenID) + " R";			
			WriteBytes((unsigned char*)ss.ptr(), ss.size());
		};
		int mRefID;
		int mGenID;
	};
	
	class cPdf1_Name: public cPdf1_Object
	{
	public:

		cPdf1_Name(Str name="")
		{
			o3_trace_scrfun("cPdf1_Name");
			mName = name;
		};

		virtual void PrepareData()
		{
			o3_trace_scrfun("PrepareData");
			mData.clear();
			WriteBytes((unsigned char*)"/", 1);
			WriteBytes((unsigned char*)mName.ptr(), mName.size());
		};
		Str mName;
	};

	class cPdf1_Stream: public cPdf1_Object
	{
	public:
		cPdf1_Stream(bool compress = true)
		{
			o3_trace_scrfun("cPdf1_Stream");
			mCompress = compress;
		};

		bool mCompress;
		
		virtual void PrepareData()
		{			
		//	return;

			o3_trace_scrfun("PrepareData");			
		//	return;

			if (mCompress)
			{

				mStreamInfo.AddKey_Int("Length", mStreamData.size());
				mStreamInfo.AddKey_Name("Filter", "FlateDecode");
				mStreamInfo.PrepareData();
				WriteBytes(mStreamInfo.GetData(), mStreamInfo.GetLength());
				WriteString("stream");
				if (mStreamData.size()>0)
				{
					int ret, flush;
					unsigned have;
					ZLib::z_stream strm;
					
					/* allocate deflate state */
					strm.zalloc = 0;
					strm.zfree = 0;
					strm.opaque = 0;
					ret = ZLib::deflateInit_(&strm, ZLib::Z_DEFAULT_COMPRESSION, ZLIB_VERSION, sizeof(ZLib::z_stream));
					if (ret == ZLib::Z_OK)
					{

						strm.avail_in = mStreamData.size();
						flush = ZLib::Z_FINISH;
						strm.next_in = mStreamData.ptr();
						unsigned char *streamout = new unsigned char[mStreamData.size()];

						strm.avail_out = mStreamData.size();
						strm.next_out = streamout;

						ret = deflate(&strm, flush);    /* no bad return value */
            
						have = mStreamData.size() - strm.avail_out;
						
						deflateEnd(&strm);
						WriteBytes(streamout,have);				
						if (streamout[have-1] != '\n')
						{
							WriteString("");
						}
						delete streamout;
					}
				}
			}
			else
			{
				mStreamInfo.AddKey_Int("Length", mStreamData.size());
				mStreamInfo.PrepareData();
				WriteBytes(mStreamInfo.GetData(), mStreamInfo.GetLength());
				WriteString("stream");
				if (mStreamData.size()>0)
				{
					WriteBytes(&mStreamData[0],mStreamData.size());				
					if (mStreamData[mStreamData.size()-1] != '\n')
					{
						WriteString("");
					}
				}
			};
			
			
			WriteString("endstream");
		};
		
		void StreamBytes(unsigned char *bytes, unsigned int len)
		{
			o3_trace_scrfun("StreamBytes");
			for (unsigned int i = 0;i<len;i++) mStreamData.push(bytes[i]);
		};

		void StreamString(Str data)
		{
			o3_trace_scrfun("StreamString");
			StreamBytes((unsigned char*)data.ptr(), data.size());
			StreamBytes((unsigned char*)"\n",1);
		};

		tVec<unsigned char> mStreamData;
		cPdf1_Dictionary mStreamInfo;
	};


	
	struct cPdf1_Page : cScr, cPdf1_Object
	{
		o3_begin_class(cScr)
		o3_end_class()
		
		o3_glue_gen()
		double mWidth, mHeight;
		cPdf1_Page(double width, double height)
		{
			o3_trace_scrfun("cPdf1_Page");
			mWidth  = width;
			mHeight = height;
			mPageContents = new cPdf1_Stream();
			mPageDict = new cPdf1_Dictionary();			
		}
	
		cPdf1_ObjectReference *OR;
		cPdf1_Stream *mPageContents;
		cPdf1_Dictionary *mPageDict;
		cPdf1_ObjectReference mPageRef;
	};

	struct cPdf1 : cScr
	{
		o3_begin_class(cScr)
		o3_end_class()
		
		o3_glue_gen()

		tVec<o3::siImage> mImageLibrary;
		tVec<int> mImageRefs;

		int mCurrentPage;

		cPdf1()
		{
			o3_trace_scrfun("cPdf1");
			mCurrentPage = -1;

			mRoot = new cPdf1_Dictionary();
			mResources = new cPdf1_Dictionary();
			cPdf1_Array ProcSetArray;
			tVec<cPdf1_Name> Names;
			Names.push(cPdf1_Name("PDF"));
			Names.push(cPdf1_Name("Text"));
			Names.push(cPdf1_Name("ImageB"));
			Names.push(cPdf1_Name("ImageC"));
			Names.push(cPdf1_Name("ImageI"));

			ProcSetArray.AddObj(&Names[0]);
			ProcSetArray.AddObj(&Names[1]);
			ProcSetArray.AddObj(&Names[2]);
			ProcSetArray.AddObj(&Names[3]);
			ProcSetArray.AddObj(&Names[4]);
			
			mResources->AddKey_Obj("ProcSet", &ProcSetArray);
//			[/PDF /Text /ImageB /ImageC /ImageI]

			mPageRoot = new cPdf1_Dictionary();
			mMetadata = new cPdf1_Stream();

			//AddObject(mInfo);
			AddObject(mRoot);
			AddObject(mResources);
			AddObject(mPageRoot);
			mMetadata->mStreamInfo.AddKey_Name("Type", "Metadata");
			mMetadata->mStreamInfo.AddKey_Name("Subtype", "XML");

			AddObject(mMetadata);
		};
		
		

		void AddObject(cPdf1_Object *O)
		{
			o3_trace_scrfun("AddObject");
			if (O == NULL) return;
			O->mID = mObjects.size();
			mObjects.push(O);		
			mCollectableObjects.push(O);
		};

		~cPdf1()
		{
			o3_trace_scrfun("~cPdf1");
			for (unsigned int i =0 ;i<mCollectableObjects.size();i++)
			{
				delete mCollectableObjects[i];
			};
			mCollectableObjects.clear();
		};

		static o3_ext("cO3") o3_fun siScr pdf()
		{
			o3_trace_scrfun("pdf");
			return o3_new(cPdf1)();
		};

		o3_fun int AddImage(iScr *newImage)
		{
			o3_trace_scrfun("AddImage");
			o3::siImage image= o3::siImage(newImage);
			if (image)
			{
				mImageLibrary.push(image);
				// write image dictionary object
				cPdf1_Stream *ImageData = new cPdf1_Stream();
				
				ImageData->mStreamInfo.AddKey_Name("Type","XObject");
				ImageData->mStreamInfo.AddKey_Name("Subtype","Image");

				ImageData->mStreamInfo.AddKey_Int("Width", image->width());
				ImageData->mStreamInfo.AddKey_Int("Height", image->height());
				ImageData->mStreamInfo.AddKey_Name("ColorSpace", "DeviceRGB");
				ImageData->mStreamInfo.AddKey_Int("BitsPerComponent", 8);
				
				//ImageData->mStreamInfo.AddKey_Int("Length", image->width*image->height*image->bpp);
				switch(image->mode_int())
				{
					case Image::MODE_RGB:
						{
							for (unsigned int y =0;y<image->height();y++)
							{
								unsigned char *P = image->getrowptr(y);
								for (unsigned int x =0;x<image->width();x++)
								{
									unsigned char pixel[3];
									pixel[2] = *P++;
									pixel[1] = *P++;
									pixel[0] = *P++;;
									ImageData->StreamBytes(pixel,3);
								};	
							};
						};
						break;
					case Image::MODE_ARGB:
						{
							for (unsigned int y =0;y<image->height();y++)
							{
								unsigned char *P = image->getrowptr(y);
								for (unsigned int x =0;x<image->width();x++)
								{
									
									unsigned char pixel[3];
									pixel[2] = *P++;
									pixel[1] = *P++;
									pixel[0] = *P++;
									*P++;
									ImageData->StreamBytes(pixel,3);
								};	
							};
						};
						break;
				};

				AddObject(ImageData);
				
				//cPdf1_Dictionary *ImageDataRef = new cPdf1_Dictionary();

				//mCollectableObjects.push(ImageDataRef);
				mImageRefs.push(ImageData->mID);
				//ImageDataRef->AddKey_ObjRef("I"+Str::fromInt32(mImageLibrary.size()),ImageData->mID);
				
				//mResources->AddKey_Obj("XObject", ImageDataRef);

				return mImageLibrary.size()-1;
			}
			else
			{
				return -1;
			}
		};

		o3_fun void PlaceImage(int imageID, double X, double Y, double Width, double Height)
		{
			o3_trace_scrfun("PlaceImage");
			if (mCurrentPage == -1) return;
			
			if (!(imageID>=0 && imageID < (int)mImageLibrary.size())) return;
			mPages[mCurrentPage]->mPageContents->StreamString(
				"q "+ 
					Str::fromDouble( Width) + " 0 0 " + Str::fromDouble(Height) + " " + Str::fromDouble(X) + " " + Str::fromDouble(Y) + " cm " + 
					"/I"+Str::fromInt32(imageID+1)+" Do "+
					"Q");


		};
		
		o3_fun int AddPage(double width, double height)
		{
			o3_trace_scrfun("AddPage");
			cPdf1_Page *P = new cPdf1_Page(width, height);
			
			AddObject(P->mPageDict);
			AddObject(P->mPageContents);
			
			P->mPageDict->AddKey_Name("Type", "Page");
			P->mPageDict->AddKey_ObjRef("Parent", mPageRoot->mID);
			P->mPageDict->AddKey_ObjRef("Resources", mResources->mID);
			P->mPageDict->AddKey_ObjRef("Contents", P->mPageContents->mID);

			P->mPageDict->AddKey_LiteralString("MediaBox", "[0 0 "+Str::fromDouble(width)+" "+Str::fromDouble(height)+"]");


			P->mPageRef.mRefID = P->mPageDict->mID;
			mPages.push(P);

			mCurrentPage = mPages.size()-1;
			return mPages.size()-1;
		};

		o3_fun void SetCurrentPage(int newpage)
		{
			o3_trace_scrfun("SetCurrentPage");
			if (newpage >=0 && newpage < (int)mPages.size())
			{
				mCurrentPage = newpage;
			};
		};


		unsigned long mWrittenBytes;
		cPdf1_Dictionary *mPageRoot;
		cPdf1_Dictionary *mResources;
		cPdf1_Dictionary *mRoot;
		cPdf1_Stream *mMetadata;
		unsigned long mCrossReferenceOffset;

		tVec<cPdf1_Object *> mCollectableObjects;
		tVec<cPdf1_Object *> mObjects;
		tVec<cPdf1_Page *> mPages;

		siStream stream;

		o3_fun void SetMetadata(const Str &newdata)
		{
			o3_trace_scrfun("SetMetadata");
			mMetadata->mStreamData.clear();
			mMetadata->StreamString(newdata);
		};

		o3_fun int Write(iFs *outFile, siEx* ex = 0)
		{
			o3_trace_scrfun("Write");
			if (mPages.size() == 0) return 0;
			
			stream = outFile->open("w", ex);
			if (!stream) return 0;
			mWrittenBytes = 0;
			mCrossReferenceOffset = 0;

			//mInfo->AddKey_String("Producer","O3");
			mRoot->AddKey_Name("Type", "Catalog"); 
			mRoot->AddKey_ObjRef("Pages", mPageRoot->mID); 
			mPageRoot->AddKey_LiteralString("MediaBox", "[0 0 "+Str::fromDouble(mPages[0]->mWidth)+" "+Str::fromDouble(mPages[0]->mHeight)+"]");


		    mRoot->AddKey_ObjRef("Metadata", mMetadata->mID); 
		    //$this->_out('/OpenAction [3 0 R /FitH null]'); 
			mRoot->AddKey_Name("PageLayout","OneColumn"); 
			mPageRoot->AddKey_Name("Type", "Pages");
			cPdf1_Array PageArray;
			cPdf1_Dictionary *ImageDictionary = new cPdf1_Dictionary();
			mCollectableObjects.push(ImageDictionary);

			for (unsigned int i =0;i<mImageRefs.size();i++)
			{
				ImageDictionary->AddKey_ObjRef("I"+Str::fromInt32(i+1), mImageRefs[i]);
			}
			mResources->AddKey_Obj("XObject", ImageDictionary);


			for (unsigned int i =0;i<mPages.size();i++)
			{
				PageArray.AddObj(&mPages[i]->mPageRef);
			};

			mPageRoot->AddKey_Obj("Kids", &PageArray);
			mPageRoot->AddKey_Int("Count", mPages.size());

			WriteHeader();
			WriteBody();
			WriteCrossReferenceTable();
			WriteTrailer();
			stream->close();
			return 0;
		};

		void WriteBytes(unsigned char *data, unsigned long len)
		{
			o3_trace_scrfun("WriteBytes");
			if (!stream) return;
			stream->write((void*)data, (size_t)len);
			mWrittenBytes += len;
		};

		void WriteString(Str data)
		{
			o3_trace_scrfun("WriteString");
			WriteBytes((unsigned char*)data.ptr(), data.size());
			WriteBytes((unsigned char*)"\n", 1);
		};

		void WriteHeader()
		{
			o3_trace_scrfun("WriteHeader");
			WriteString("%PDF-1.3");
			unsigned char force8bit[] = {0x25,0xb5,0xb5,0xb5,0xb5, 0x0a};			
			WriteBytes(force8bit, 6);
		};

		void WriteBody()
		{
			o3_trace_scrfun("WriteBody");
			for (unsigned int i =0;i<mObjects.size();i++)
			{
				mObjects[i]->mOffset = mWrittenBytes;
				WriteString(Str::fromInt32(i+1) + " 0 obj");
				mObjects[i]->PrepareData();
				unsigned long L = mObjects[i]->GetLength();
				WriteBytes(mObjects[i]->GetData(), L);
				WriteString("endobj");
			};
		};

		void WriteCrossReferenceTable()
		{
			o3_trace_scrfun("WriteCrossReferenceTable");
			mCrossReferenceOffset = mWrittenBytes;
			WriteString("xref");
			char txt[255];
			sprintf_s<255>(txt, "0 %d", mObjects.size()+1);
			WriteString(txt);
			WriteString("0000000000 65535 f ");
			for (unsigned int i = 0;i<mObjects.size();i++)
			{
				sprintf_s<255>(txt, "%010d %05d n \n   ", mObjects[i]->mOffset, 0); // generation number always 0.. in use always N
				WriteBytes((unsigned char*)txt,20);
			};
		};

		void WriteTrailer()
		{
			o3_trace_scrfun("WriteTrailer");
			WriteString("trailer");

			cPdf1_Dictionary PageDictionary;
			PageDictionary.AddKey_Int("Size", mObjects.size());			
			PageDictionary.AddKey_ObjRef("Root", mRoot->mID);
			//PageDictionary.AddKey_ObjRef("Info", mInfo->mID);

			PageDictionary.PrepareData();

			WriteBytes(PageDictionary.GetData(),PageDictionary.GetLength());

			WriteString("startxref");
			Str Offset = Str::fromInt32(mCrossReferenceOffset);
			WriteString(Offset );
			WriteString("%%EOF");
		};
	};
}
#endif
