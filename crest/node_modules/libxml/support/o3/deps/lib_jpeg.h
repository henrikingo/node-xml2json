#ifndef O3_LIBJPG
#define O3_LIBJPG
namespace o3
{
	namespace jpg
	{
#pragma warning( push)
#pragma warning( disable : 4244)
#pragma warning( disable : 4100)
#pragma warning( disable : 4005)
//#include "jpeg-8b/ansi2knr.c"
//#include "jpeg-8b/cdjpeg.c"
//#include "jpeg-8b/cjpeg.c"
//#include "jpeg-8b/ckconfig.c"
//#include "jpeg-8b/djpeg.c"
//#include "jpeg-8b/example.c"
#define JPEG_INTERNALS
#include "jpeg-8b/jinclude.h"
#include "jpeg-8b/jpeglib.h"
#ifdef O3_COMPILE_LIBRARIES
#include "jpeg-8b/jaricom.c"
#include "jpeg-8b/jcapimin.c"
#include "jpeg-8b/jcapistd.c"
#include "jpeg-8b/jcarith.c"
#include "jpeg-8b/jccoefct.c"
#include "jpeg-8b/jccolor.c"
#include "jpeg-8b/jcdctmgr.c"
#include "jpeg-8b/jchuff.c"
#include "jpeg-8b/jcinit.c"
#include "jpeg-8b/jcmainct.c"
#include "jpeg-8b/jcmarker.c"
#include "jpeg-8b/jcmaster.c"
#include "jpeg-8b/jcomapi.c"
#include "jpeg-8b/jcparam.c"
#include "jpeg-8b/jcprepct.c"
#include "jpeg-8b/jcsample.c"
#include "jpeg-8b/jctrans.c"
#include "jpeg-8b/jdapimin.c"
#include "jpeg-8b/jdapistd.c"
#include "jpeg-8b/jdarith.c"
#include "jpeg-8b/jdatadst.c"
#include "jpeg-8b/jdatasrc.c"
#include "jpeg-8b/jdcoefct.c"
#include "jpeg-8b/jdcolor.c"
#include "jpeg-8b/jddctmgr.c"
#include "jpeg-8b/jdhuff.c"
#include "jpeg-8b/jdinput.c"
#include "jpeg-8b/jdmainct.c"
#include "jpeg-8b/jdmarker.c"
#include "jpeg-8b/jdmaster.c"
#include "jpeg-8b/jdmerge.c"
#include "jpeg-8b/jdpostct.c"
#include "jpeg-8b/jdsample.c"
#include "jpeg-8b/jdtrans.c"
#include "jpeg-8b/jerror.c"
#include "jpeg-8b/jfdctflt.c"
#include "jpeg-8b/jfdctfst.c"
#include "jpeg-8b/jfdctint.c"
#include "jpeg-8b/jidctflt.c"
#include "jpeg-8b/jidctfst.c"
#include "jpeg-8b/jidctint.c"
#include "jpeg-8b/jmemansi.c"
//#include "jpeg-8b/jmemdos.c"
//#include "jpeg-8b/jmemmac.c"
#include "jpeg-8b/jmemmgr.c"
//#include "jpeg-8b/jmemname.c"
//#include "jpeg-8b/jmemnobs.c"
//#include "jpeg-8b/jpegtran.c"
#include "jpeg-8b/jquant1.c"
#include "jpeg-8b/jquant2.c"
#include "jpeg-8b/jutils.c"
//#include "jpeg-8b/rdbmp.c"
//#include "jpeg-8b/rdcolmap.c"
//#include "jpeg-8b/rdgif.c"
//#include "jpeg-8b/rdjpgcom.c"
//#include "jpeg-8b/rdppm.c"
//#include "jpeg-8b/rdrle.c"
//#include "jpeg-8b/rdswitch.c"
//#include "jpeg-8b/rdtarga.c"
//#include "jpeg-8b/transupp.c"
//#include "jpeg-8b/wrbmp.c"
//#include "jpeg-8b/wrgif.c"
//#include "jpeg-8b/wrjpgcom.c"
//#include "jpeg-8b/wrppm.c"
//#include "jpeg-8b/wrrle.c"
//#include "jpeg-8b/wrtarga.c"

#endif

#pragma warning( pop)
	}
}
#endif
