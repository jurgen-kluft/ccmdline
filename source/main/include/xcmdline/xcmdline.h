#ifndef __CCORE_CMDLINE_H__
#define __CCORE_CMDLINE_H__
#include "cbase/c_target.h"
#ifdef USE_PRAGMA_ONCE
#pragma once
#endif

#include "cbase/c_target.h"
#include "cbase/c_va_list.h"

namespace ncore
{
	// Forward declares
	class alloc_t;

	namespace cli
	{
		struct params_t;

		enum eoption
		{
			eOPT_OPTIONAL,
			eOPT_REQUIRED
		};

		struct argv_t
		{
			inline				argv_t(const char* sn, const char* ln, const char* de, eoption o, va_r_t v)
				: m_short(sn)
				, m_long(ln)
				, m_description(de)
				, m_option(o)
				, m_value(v) 
			{
			}

			const char*			m_short;						// e.g. f
			const char*			m_long;						// e.g. force
			const char*			m_description;				// "Force to add files"
			eoption				m_option;					// OPTIONAL
			va_r_t				m_value;						// false -> boolean
			static argv_t		nil;
		};

		struct argl_t
		{
			inline				argl_t(const char* name, argv_t* argv) 
				: m_name(name)
				, m_argv(argv) 
			{
			}

			const char*			m_name;						// "add"
			argv_t*				m_argv;
			static argl_t		nil;
		};

		struct cmds_t
		{
			inline				cmds_t() : m_index(-1), m_argl(nullptr) {}
			inline				cmds_t(argl_t* argl) : m_index(-1), m_argl(argl) {}

			s32					m_index;
			argl_t*				m_argl;
		};

		class cmdline_t
		{
		public:
			bool				parse(argv_t * arg, const char* cmdline);
			bool				parse(argv_t * arg, s32 argc, const char** argv);

			bool				parse(cmds_t & c, const char* cmdline);
			bool				parse(cmds_t & c, s32 argc, const char** argv);

		private:
			bool				parse(cmds_t & c);
		};
	};
};


#endif    /// __CCORE_CMDLINE_H__
