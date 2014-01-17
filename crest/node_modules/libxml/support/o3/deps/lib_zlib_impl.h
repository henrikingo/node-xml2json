#ifndef J_ZLIB_IMP_H
#define J_ZLIB_IMP_H

namespace o3 
{
	namespace ZLib 
	{

		const char * const z_errmsg[10] = {
			"need dictionary",     /* Z_NEED_DICT       2  */
			"stream end",          /* Z_STREAM_END      1  */
			"",                    /* Z_OK              0  */
			"file error",          /* Z_ERRNO         (-1) */
			"stream error",        /* Z_STREAM_ERROR  (-2) */
			"data error",          /* Z_DATA_ERROR    (-3) */
			"insufficient memory", /* Z_MEM_ERROR     (-4) */
			"buffer error",        /* Z_BUF_ERROR     (-5) */
			"incompatible version",/* Z_VERSION_ERROR (-6) */
			""};

			const char *  zlibVersion()
			{
				return ZLIB_VERSION;
			}

			void  zcfree (void * opaque, void * ptr)
			{
				free(ptr);
				if (opaque) return; /* make compiler happy */
			}

			void * zcalloc (void * opaque, unsigned items, unsigned size)
			{
				if (opaque) items += size - size; /* make compiler happy */
				return sizeof(unsigned int) > 2 ? (void *)malloc(items * size) :
					(void *)calloc(items, size);
			}

			unsigned long  zlibCompileFlags()
			{
				unsigned long flags;

				flags = 0;
				switch (sizeof(unsigned int)) {
			case 2:     break;
			case 4:     flags += 1;     break;
			case 8:     flags += 2;     break;
			default:    flags += 3;
				}
				switch (sizeof(unsigned long)) {
			case 2:     break;
			case 4:     flags += 1 << 2;        break;
			case 8:     flags += 2 << 2;        break;
			default:    flags += 3 << 2;
				}
				switch (sizeof(void *)) {
			case 2:     break;
			case 4:     flags += 1 << 4;        break;
			case 8:     flags += 2 << 4;        break;
			default:    flags += 3 << 4;
				}
				switch (sizeof(z_off_t)) {
			case 2:     break;
			case 4:     flags += 1 << 6;        break;
			case 8:     flags += 2 << 6;        break;
			default:    flags += 3 << 6;
				}
#ifdef DEBUG
				flags += 1 << 8;
#endif
#if defined(ASMV) || defined(ASMINF)
				flags += 1 << 9;
#endif
#ifdef ZLIB_WINAPI
				flags += 1 << 10;
#endif
#ifdef BUILDFIXED
				flags += 1 << 12;
#endif
#ifdef DYNAMIC_CRC_TABLE
				flags += 1 << 13;
#endif
#ifdef NO_GZCOMPRESS
				flags += 1L << 16;
#endif
#ifdef NO_GZIP
				flags += 1L << 17;
#endif
#ifdef PKZIP_BUG_WORKAROUND
				flags += 1L << 20;
#endif
#ifdef FASTEST
				flags += 1L << 21;
#endif
#ifdef STDC
#  ifdef NO_vsnprintf
				flags += 1L << 25;
#    ifdef HAS_vsprintf_void
				flags += 1L << 26;
#    endif
#  else
#    ifdef HAS_vsnprintf_void
				flags += 1L << 26;
#    endif
#  endif
#else
				flags += 1L << 24;
#  ifdef NO_snprintf
				flags += 1L << 25;
#    ifdef HAS_sprintf_void
				flags += 1L << 26;
#    endif
#  else
#    ifdef HAS_snprintf_void
				flags += 1L << 26;
#    endif
#  endif
#endif
				return flags;
			}

			const unsigned char _length_code[MAX_MATCH - MIN_MATCH + 1]= {
				0,  1,  2,  3,  4,  5,  6,  7,  8,  8,  9,  9, 10, 10, 11, 11, 12, 12, 12, 12,
				13, 13, 13, 13, 14, 14, 14, 14, 15, 15, 15, 15, 16, 16, 16, 16, 16, 16, 16, 16,
				17, 17, 17, 17, 17, 17, 17, 17, 18, 18, 18, 18, 18, 18, 18, 18, 19, 19, 19, 19,
				19, 19, 19, 19, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
				21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 22, 22, 22, 22,
				22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 23, 23, 23, 23, 23, 23, 23, 23,
				23, 23, 23, 23, 23, 23, 23, 23, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
				24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
				25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
				25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 26, 26, 26, 26, 26, 26, 26, 26,
				26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26,
				26, 26, 26, 26, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27,
				27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 28
			};

			const unsigned char _dist_code[512] = { // 512 = DIST_CODE_LEN const!
				0,  1,  2,  3,  4,  4,  5,  5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  8,
				8,  8,  8,  8,  9,  9,  9,  9,  9,  9,  9,  9, 10, 10, 10, 10, 10, 10, 10, 10,
				10, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
				11, 11, 11, 11, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
				12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 13, 13, 13, 13,
				13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
				13, 13, 13, 13, 13, 13, 13, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
				14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
				14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
				14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 15, 15, 15, 15, 15, 15, 15, 15,
				15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
				15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
				15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,  0,  0, 16, 17,
				18, 18, 19, 19, 20, 20, 20, 20, 21, 21, 21, 21, 22, 22, 22, 22, 22, 22, 22, 22,
				23, 23, 23, 23, 23, 23, 23, 23, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
				24, 24, 24, 24, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
				26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26,
				26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 27, 27, 27, 27, 27, 27, 27, 27,
				27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27,
				27, 27, 27, 27, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
				28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
				28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
				28, 28, 28, 28, 28, 28, 28, 28, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29,
				29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29,
				29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29,
				29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29
			};
	}
}

#pragma warning( default : 4244)    // conversion without explicit cast
#pragma warning( default : 4127)    // conditional expression is constant

#endif /* J_ZLIB_H */
