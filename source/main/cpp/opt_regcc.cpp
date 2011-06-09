#include <cstring>
#include "xcmdline\private\opt.h"

using namespace std;

namespace xcore
{
	namespace xcmdline
	{
		s32 OptRegister(char *v, char c, char *n, char *B)
		{
			return optrega(v,OPT_CHAR,c,n,B);
		}
		s32 OptRegister(char *v, char *n, char *B)
		{
			return optrega(v,OPT_CHAR,'\0',n,B);
		}
		s32 OptRegister(char *v, opt_TYPE o, char c, char *n, char *B)
		{
			return optrega(v,o,c,n,B);
		}
		s32 OptRegister(char *v, opt_TYPE o, char *n, char *B)
		{
			return optrega(v,o,'\0',n,B);
		}
		s32 OptRegister(char *v, opt_MODE m, char c, char *n, char *B)
		{
			s32 i = optrega(v,OPT_CHAR,c,n,B);
			optmode_n(i, m);
			return i;
		}
		s32 OptRegister(char *v, opt_MODE m, char *n, char *B)
		{
			s32 i = optrega(v,OPT_CHAR,'\0',n,B);
			optmode_n(i, m);
			return i;
		}
		s32 OptRegister(char *v, opt_TYPE o, opt_MODE m, char c, char *n, char *B)
		{
			s32 i = optrega(v,o,c,n,B);
			optmode_n(i, m);
			return i;
		}
		s32 OptRegister(char *v, opt_TYPE o, opt_MODE m, char *n, char *B)
		{
			s32 i = optrega(v,o,'\0',n,B);
			optmode_n(i, m);
			return i;
		}
		s32 OptRegister(s16 *v, char c, char *n, char *B)
		{
			return optrega(v,OPT_SHORT,c,n,B);
		}
		s32 OptRegister(s16 *v, char *n, char *B)
		{
			return optrega(v,OPT_SHORT,'\0',n,B);
		}
		s32 OptRegister(s16 *v, opt_TYPE o, char c, char *n, char *B)
		{
			return optrega(v,o,c,n,B);
		}
		s32 OptRegister(s16 *v, opt_TYPE o, char *n, char *B)
		{
			return optrega(v,o,'\0',n,B);
		}
		s32 OptRegister(s16 *v, opt_MODE m, char c, char *n, char *B)
		{
			s32 i = optrega(v,OPT_SHORT,c,n,B);
			optmode_n(i, m);
			return i;
		}
		s32 OptRegister(s16 *v, opt_MODE m, char *n, char *B)
		{
			s32 i = optrega(v,OPT_SHORT,'\0',n,B);
			optmode_n(i, m);
			return i;
		}
		s32 OptRegister(s16 *v, opt_TYPE o, opt_MODE m, char c, char *n, char *B)
		{
			s32 i = optrega(v,o,c,n,B);
			optmode_n(i, m);
			return i;
		}
		s32 OptRegister(s16 *v, opt_TYPE o, opt_MODE m, char *n, char *B)
		{
			s32 i = optrega(v,o,'\0',n,B);
			optmode_n(i, m);
			return i;
		}
		s32 OptRegister(u16 *v, char c, char *n, char *B)
		{
			return optrega(v,OPT_USHORT,c,n,B);
		}
		s32 OptRegister(u16 *v, char *n, char *B)
		{
			return optrega(v,OPT_USHORT,'\0',n,B);
		}
		s32 OptRegister(u16 *v, opt_TYPE o, char c, char *n, char *B)
		{
			return optrega(v,o,c,n,B);
		}
		s32 OptRegister(u16 *v, opt_TYPE o, char *n, char *B)
		{
			return optrega(v,o,'\0',n,B);
		}
		s32 OptRegister(u16 *v, opt_MODE m, char c, char *n, char *B)
		{
			s32 i = optrega(v,OPT_USHORT,c,n,B);
			optmode_n(i, m);
			return i;
		}
		s32 OptRegister(u16 *v, opt_MODE m, char *n, char *B)
		{
			s32 i = optrega(v,OPT_USHORT,'\0',n,B);
			optmode_n(i, m);
			return i;
		}
		s32 OptRegister(u16 *v, opt_TYPE o, opt_MODE m, char c, char *n, char *B)
		{
			s32 i = optrega(v,o,c,n,B);
			optmode_n(i, m);
			return i;
		}
		s32 OptRegister(u16 *v, opt_TYPE o, opt_MODE m, char *n, char *B)
		{
			s32 i = optrega(v,o,'\0',n,B);
			optmode_n(i, m);
			return i;
		}
		s32 OptRegister(s32 *v, char c, char *n, char *B)
		{
			return optrega(v,OPT_INT,c,n,B);
		}
		s32 OptRegister(s32 *v, char *n, char *B)
		{
			return optrega(v,OPT_INT,'\0',n,B);
		}
		s32 OptRegister(s32 *v, opt_TYPE o, char c, char *n, char *B)
		{
			return optrega(v,o,c,n,B);
		}
		s32 OptRegister(s32 *v, opt_TYPE o, char *n, char *B)
		{
			return optrega(v,o,'\0',n,B);
		}
		s32 OptRegister(s32 *v, opt_MODE m, char c, char *n, char *B)
		{
			s32 i = optrega(v,OPT_INT,c,n,B);
			optmode_n(i, m);
			return i;
		}
		s32 OptRegister(s32 *v, opt_MODE m, char *n, char *B)
		{
			s32 i = optrega(v,OPT_INT,'\0',n,B);
			optmode_n(i, m);
			return i;
		}
		s32 OptRegister(s32 *v, opt_TYPE o, opt_MODE m, char c, char *n, char *B)
		{
			s32 i = optrega(v,o,c,n,B);
			optmode_n(i, m);
			return i;
		}
		s32 OptRegister(s32 *v, opt_TYPE o, opt_MODE m, char *n, char *B)
		{
			s32 i = optrega(v,o,'\0',n,B);
			optmode_n(i, m);
			return i;
		}
		s32 OptRegister(u64 *v, char c, char *n, char *B)
		{
			return optrega(v,OPT_ULONG,c,n,B);
		}
		s32 OptRegister(u64 *v, char *n, char *B)
		{
			return optrega(v,OPT_ULONG,'\0',n,B);
		}
		s32 OptRegister(u64 *v, opt_TYPE o, char c, char *n, char *B)
		{
			return optrega(v,o,c,n,B);
		}
		s32 OptRegister(u64 *v, opt_TYPE o, char *n, char *B)
		{
			return optrega(v,o,'\0',n,B);
		}
		s32 OptRegister(u64 *v, opt_MODE m, char c, char *n, char *B)
		{
			s32 i = optrega(v,OPT_ULONG,c,n,B);
			optmode_n(i, m);
			return i;
		}
		s32 OptRegister(u64 *v, opt_MODE m, char *n, char *B)
		{
			s32 i = optrega(v,OPT_ULONG,'\0',n,B);
			optmode_n(i, m);
			return i;
		}
		s32 OptRegister(u64 *v, opt_TYPE o, opt_MODE m, char c, char *n, char *B)
		{
			s32 i = optrega(v,o,c,n,B);
			optmode_n(i, m);
			return i;
		}
		s32 OptRegister(u64 *v, opt_TYPE o, opt_MODE m, char *n, char *B)
		{
			s32 i = optrega(v,o,'\0',n,B);
			optmode_n(i, m);
			return i;
		}
		s32 OptRegister(f64 *v, char c, char *n, char *B)
		{
			return optrega(v,OPT_DOUBLE,c,n,B);
		}
		s32 OptRegister(f64 *v, char *n, char *B)
		{
			return optrega(v,OPT_DOUBLE,'\0',n,B);
		}
		s32 OptRegister(f64 *v, opt_TYPE o, char c, char *n, char *B)
		{
			return optrega(v,o,c,n,B);
		}
		s32 OptRegister(f64 *v, opt_TYPE o, char *n, char *B)
		{
			return optrega(v,o,'\0',n,B);
		}
		s32 OptRegister(f64 *v, opt_MODE m, char c, char *n, char *B)
		{
			s32 i = optrega(v,OPT_DOUBLE,c,n,B);
			optmode_n(i, m);
			return i;
		}
		s32 OptRegister(f64 *v, opt_MODE m, char *n, char *B)
		{
			s32 i = optrega(v,OPT_DOUBLE,'\0',n,B);
			optmode_n(i, m);
			return i;
		}
		s32 OptRegister(f64 *v, opt_TYPE o, opt_MODE m, char c, char *n, char *B)
		{
			s32 i = optrega(v,o,c,n,B);
			optmode_n(i, m);
			return i;
		}
		s32 OptRegister(f64 *v, opt_TYPE o, opt_MODE m, char *n, char *B)
		{
			s32 i = optrega(v,o,'\0',n,B);
			optmode_n(i, m);
			return i;
		}
		s32 OptRegister(s64 *v, char c, char *n, char *B)
		{
			return optrega(v,OPT_LONG,c,n,B);
		}
		s32 OptRegister(s64 *v, char *n, char *B)
		{
			return optrega(v,OPT_LONG,'\0',n,B);
		}
		s32 OptRegister(s64 *v, opt_TYPE o, char c, char *n, char *B)
		{
			return optrega(v,o,c,n,B);
		}
		s32 OptRegister(s64 *v, opt_TYPE o, char *n, char *B)
		{
			return optrega(v,o,'\0',n,B);
		}
		s32 OptRegister(s64 *v, opt_MODE m, char c, char *n, char *B)
		{
			s32 i = optrega(v,OPT_LONG,c,n,B);
			optmode_n(i, m);
			return i;
		}
		s32 OptRegister(s64 *v, opt_MODE m, char *n, char *B)
		{
			s32 i = optrega(v,OPT_LONG,'\0',n,B);
			optmode_n(i, m);
			return i;
		}
		s32 OptRegister(s64 *v, opt_TYPE o, opt_MODE m, char c, char *n, char *B)
		{
			s32 i = optrega(v,o,c,n,B);
			optmode_n(i, m);
			return i;
		}
		s32 OptRegister(s64 *v, opt_TYPE o, opt_MODE m, char *n, char *B)
		{
			s32 i = optrega(v,o,'\0',n,B);
			optmode_n(i, m);
			return i;
		}
		s32 OptRegister(u32 *v, char c, char *n, char *B)
		{
			return optrega(v,OPT_UINT,c,n,B);
		}
		s32 OptRegister(u32 *v, char *n, char *B)
		{
			return optrega(v,OPT_UINT,'\0',n,B);
		}
		s32 OptRegister(u32 *v, opt_TYPE o, char c, char *n, char *B)
		{
			return optrega(v,o,c,n,B);
		}
		s32 OptRegister(u32 *v, opt_TYPE o, char *n, char *B)
		{
			return optrega(v,o,'\0',n,B);
		}
		s32 OptRegister(u32 *v, opt_MODE m, char c, char *n, char *B)
		{
			s32 i = optrega(v,OPT_UINT,c,n,B);
			optmode_n(i, m);
			return i;
		}
		s32 OptRegister(u32 *v, opt_MODE m, char *n, char *B)
		{
			s32 i = optrega(v,OPT_UINT,'\0',n,B);
			optmode_n(i, m);
			return i;
		}
		s32 OptRegister(u32 *v, opt_TYPE o, opt_MODE m, char c, char *n, char *B)
		{
			s32 i = optrega(v,o,c,n,B);
			optmode_n(i, m);
			return i;
		}
		s32 OptRegister(u32 *v, opt_TYPE o, opt_MODE m, char *n, char *B)
		{
			s32 i = optrega(v,o,'\0',n,B);
			optmode_n(i, m);
			return i;
		}
		s32 OptRegister(char **v, char c, char *n, char *B)
		{
			return optrega(v,OPT_STRING,c,n,B);
		}
		s32 OptRegister(char **v, char *n, char *B)
		{
			return optrega(v,OPT_STRING,'\0',n,B);
		}
		s32 OptRegister(char **v, opt_TYPE o, char c, char *n, char *B)
		{
			return optrega(v,o,c,n,B);
		}
		s32 OptRegister(char **v, opt_TYPE o, char *n, char *B)
		{
			return optrega(v,o,'\0',n,B);
		}
		s32 OptRegister(char **v, opt_MODE m, char c, char *n, char *B)
		{
			s32 i = optrega(v,OPT_STRING,c,n,B);
			optmode_n(i, m);
			return i;
		}
		s32 OptRegister(char **v, opt_MODE m, char *n, char *B)
		{
			s32 i = optrega(v,OPT_STRING,'\0',n,B);
			optmode_n(i, m);
			return i;
		}
		s32 OptRegister(char **v, opt_TYPE o, opt_MODE m, char c, char *n, char *B)
		{
			s32 i = optrega(v,o,c,n,B);
			optmode_n(i, m);
			return i;
		}
		s32 OptRegister(char **v, opt_TYPE o, opt_MODE m, char *n, char *B)
		{
			s32 i = optrega(v,o,'\0',n,B);
			optmode_n(i, m);
			return i;
		}
		s32 OptRegister(f32 *v, char c, char *n, char *B)
		{
			return optrega(v,OPT_FLOAT,c,n,B);
		}
		s32 OptRegister(f32 *v, char *n, char *B)
		{
			return optrega(v,OPT_FLOAT,'\0',n,B);
		}
		s32 OptRegister(f32 *v, opt_TYPE o, char c, char *n, char *B)
		{
			return optrega(v,o,c,n,B);
		}
		s32 OptRegister(f32 *v, opt_TYPE o, char *n, char *B)
		{
			return optrega(v,o,'\0',n,B);
		}
		s32 OptRegister(f32 *v, opt_MODE m, char c, char *n, char *B)
		{
			s32 i = optrega(v,OPT_FLOAT,c,n,B);
			optmode_n(i, m);
			return i;
		}
		s32 OptRegister(f32 *v, opt_MODE m, char *n, char *B)
		{
			s32 i = optrega(v,OPT_FLOAT,'\0',n,B);
			optmode_n(i, m);
			return i;
		}
		s32 OptRegister(f32 *v, opt_TYPE o, opt_MODE m, char c, char *n, char *B)
		{
			s32 i = optrega(v,o,c,n,B);
			optmode_n(i, m);
			return i;
		}
		s32 OptRegister(f32 *v, opt_TYPE o, opt_MODE m, char *n, char *B)
		{
			s32 i = optrega(v,o,'\0',n,B);
			optmode_n(i, m);
			return i;
		}
		s32 OptRegister(unsigned char *v, char c, char *n, char *B)
		{
			return optrega(v,OPT_UCHAR,c,n,B);
		}
		s32 OptRegister(unsigned char *v, char *n, char *B)
		{
			return optrega(v,OPT_UCHAR,'\0',n,B);
		}
		s32 OptRegister(unsigned char *v, opt_TYPE o, char c, char *n, char *B)
		{
			return optrega(v,o,c,n,B);
		}
		s32 OptRegister(unsigned char *v, opt_TYPE o, char *n, char *B)
		{
			return optrega(v,o,'\0',n,B);
		}
		s32 OptRegister(unsigned char *v, opt_MODE m, char c, char *n, char *B)
		{
			s32 i = optrega(v,OPT_UCHAR,c,n,B);
			optmode_n(i, m);
			return i;
		}
		s32 OptRegister(unsigned char *v, opt_MODE m, char *n, char *B)
		{
			s32 i = optrega(v,OPT_UCHAR,'\0',n,B);
			optmode_n(i, m);
			return i;
		}
		s32 OptRegister(unsigned char *v, opt_TYPE o, opt_MODE m, char c, char *n, char *B)
		{
			s32 i = optrega(v,o,c,n,B);
			optmode_n(i, m);
			return i;
		}
		s32 OptRegister(unsigned char *v, opt_TYPE o, opt_MODE m, char *n, char *B)
		{
			s32 i = optrega(v,o,'\0',n,B);
			optmode_n(i, m);
			return i;
		}
	}
}
