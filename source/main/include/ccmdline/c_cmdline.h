#ifndef __CCORE_CMDLINE_H__
#define __CCORE_CMDLINE_H__
#include "ccore/c_target.h"
#ifdef USE_PRAGMA_ONCE
#    pragma once
#endif

#include "ccore/c_target.h"
#include "cbase/c_va_list.h"

namespace ncore
{
    // Forward declares
    class alloc_t;

    namespace cli
    {
        struct params;

        enum EOption
        {
            Optional,
            Required
        };

        struct argv
        {
            inline argv(const char* sn, const char* ln, const char* de, EOption o, va_r_t v) : m_short(sn), m_long(ln), m_description(de), m_option(o), m_value(v) {}

            const char* m_short;       // e.g. f
            const char* m_long;        // e.g. force
            const char* m_description; // "Force to add files"
            EOption     m_option;      // OPTIONAL
            va_r_t      m_value;       // false -> boolean
            static argv nil;
        };

        struct argl
        {
            inline argl(const char* name, argv* argv) : m_name(name), m_argv(argv) {}

            const char* m_name; // "add"
            argv*       m_argv;
            static argl nil;
        };

        struct cmds
        {
            inline cmds() : m_index(-1), m_argl(nullptr) {}
            inline cmds(argl* argl) : m_index(-1), m_argl(argl) {}

            s32   m_index;
            argl* m_argl;
        };

        class cmdline
        {
        public:
            bool parse(argv* arg, const char* cmdline);
            bool parse(argv* arg, s32 argc, const char** argv);

            bool parse(cmds& c, const char* cmdline);
            bool parse(cmds& c, s32 argc, const char** argv);

        private:
            bool parse(cmds& c);
        };
    }; // namespace cli
};     // namespace ncore

#endif /// __CCORE_CMDLINE_H__
