
#ifndef O3_FREETYPE
#define O3_FREETYPE


// standard includes have been pulled out of namespace to prevent collission of globals!
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdarg.h>
#include <string.h>
#include <limits.h>


namespace o3
{
	namespace freetype
	{
#pragma warning( push)
#pragma warning( disable : 4700 4701 4100 4512 4244)


#include "freetype-2.4.3/include/ft2build.h"
#include "freetype-2.4.3/include/freetype/fterrors.h"
#include "freetype-2.4.3/include/freetype/freetype.h"
#include "freetype-2.4.3/include/freetype/ftmoderr.h"

#include "freetype-2.4.3/include/ft2build.h"
#include "freetype-2.4.3/src/base/ftbase.c"
#include "freetype-2.4.3/src/base/ftsystem.c"
#include "freetype-2.4.3/src/base/fttype1.c"
#include "freetype-2.4.3/src/base/ftbitmap.c"
#include "freetype-2.4.3/src/base/ftsynth.c"
#include "freetype-2.4.3/src/base/ftbbox.c"
#include "freetype-2.4.3/src/base/ftpfr.c"

#include "freetype-2.4.3/src/base/ftgxval.c"
#include "freetype-2.4.3/src/base/ftlcdfil.c"
#include "freetype-2.4.3/src/base/ftmm.c"
#include "freetype-2.4.3/src/base/ftotval.c"
#include "freetype-2.4.3/src/base/ftpatent.c"
#include "freetype-2.4.3/src/base/ftwinfnt.c"
#include "freetype-2.4.3/src/base/ftxf86.c"
#include "freetype-2.4.3/src/pcf/pcf.c"
#include "freetype-2.4.3/src/pfr/pfr.c"
#include "freetype-2.4.3/src/psaux/psaux.c"
typedef void* voidpf;
typedef unsigned int uInt;
#define local 
#include "freetype-2.4.3/src/gzip/ftgzip.c"
#undef local
#include "freetype-2.4.3/src/lzw/ftlzw.c"

#include "freetype-2.4.3/src/base/ftinit.c"

#include "freetype-2.4.3/src/pshinter/pshinter.c"
#include "freetype-2.4.3/src/psnames/psmodule.c"
#include "freetype-2.4.3/src/sfnt/sfnt.c"
#include "freetype-2.4.3/src/raster/raster.c"


#include "freetype-2.4.3/src/smooth/smooth.c"
#include "freetype-2.4.3/src/bdf/bdf.c"

#include "freetype-2.4.3/src/winfonts/winfnt.c"

#include "freetype-2.4.3/src/truetype/truetype.c"
#include "freetype-2.4.3/src/type1/type1.c"
#include "freetype-2.4.3/src/type42/type42.c"


#include "freetype-2.4.3/src/base/ftglyph.c"
#include "freetype-2.4.3/src/cache/ftcache.c"


#include "freetype-2.4.3/src/cid/type1cid.c"



#include "freetype-2.4.3/src/cff/cff.c"

/*




*/
#include "freetype-2.4.3/src/autofit/autofit.c"


#pragma warning( pop)
	}
}

#endif