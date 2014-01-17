#ifndef O3_LIBPNG
#define O3_LIBPNG
#define PNG_NO_SPRINTF
#define PNG_NO_PEDANTIC_WARNINGS
namespace o3
{
	namespace png
	{
		using namespace ZLib;

		typedef void *voidpf;
		typedef void *voidp;
		typedef unsigned int uInt ;
		typedef unsigned char  Byte;  /* 8 bits */
		typedef Byte Bytef;

#ifdef O3_WIN32
		#define png_snprintf _snprintf   /* Added to v 1.2.19 */
		#define png_snprintf2 _snprintf
		#define png_snprintf6 _snprintf
#endif

		#include "libpng/png.h"

#ifdef O3_COMPILE_LIBRARIES
		#include "libpng/pngpriv.h"



		#include "libpng/png.c"
		#include "libpng/pngerror.c"
		#include "libpng/pngget.c"
		#include "libpng/pngmem.c"
		#include "libpng/pngpread.c"
		#include "libpng/pngread.c"
		#include "libpng/pngrio.c"
		#include "libpng/pngrtran.c"
		#include "libpng/pngrutil.c"
		#include "libpng/pngset.c"
		#include "libpng/pngtest.c"
		#include "libpng/pngtrans.c"
		#include "libpng/pngwio.c"
		#include "libpng/pngwrite.c"
		#include "libpng/pngwtran.c"
		#include "libpng/pngwutil.c"
#endif

	}
}
#endif
