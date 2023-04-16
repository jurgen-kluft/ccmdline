#include "cbase/c_target.h"
#include "cbase/c_allocator.h"
#include "cbase/c_runes.h"

#include "ccmdline/c_cmdline.h"

namespace ncore
{
    namespace cli
    {
        argv_t argv_t::nil(nullptr, nullptr, nullptr, Optional, va_r_t());
        argl_t argl_t::nil(nullptr, nullptr);

        struct paramstr_t
        {
            inline paramstr_t() : m_str(nullptr), m_end(nullptr) {}
            inline paramstr_t(const char* str) : m_str(str), m_end(nullptr)
            {
                m_end = m_str;
                while (*m_end != '\0')
                    m_end++;
            }
            inline paramstr_t(const char* str, const char* end) : m_str(str), m_end(end) {}

            bool empty() const { return m_str == m_end; }

            bool startsWith(char c) const;
            bool endsWith(char c) const;

            void clear()
            {
                m_str = nullptr;
                m_end = nullptr;
            }
            void trim(char c);

            s32 compare(const char* str) const;
            s32 compare(paramstr_t const& other) const;

            bool to_value(va_r_t& out) const;

        private:
            const char* m_str;
            const char* m_end;
        };

        bool paramstr_t::startsWith(char c) const
        {
            if (empty())
                return false;
            return m_str[0] == c;
        }

        bool paramstr_t::endsWith(char c) const
        {
            if (empty())
                return false;
            return m_end[-1] == c;
        }

        void paramstr_t::trim(char c)
        {
            if (empty() == false)
            {
                if (startsWith(c))
                {
                    m_str += 1;
                }
                if (endsWith(c))
                {
                    m_end -= 1;
                }
            }
        }

        s32 paramstr_t::compare(const char* str) const { return ncore::compare(crunes_t(m_str, m_end), crunes_t(str), false); }
        s32 paramstr_t::compare(paramstr_t const& other) const { return ncore::compare(crunes_t(m_str, m_end), crunes_t(other.m_str, other.m_end), false); }

        bool paramstr_t::to_value(va_r_t& out) const
        {
            crunes_t str(m_str, m_end);
            out = va_t(str);
            return true;
        }

        struct param_t
        {
        public:
            inline param_t() {}
            inline param_t(paramstr_t const& key, paramstr_t const& value) : m_key(key), m_val(value) {}

        private:
            paramstr_t m_key;
            paramstr_t m_val;
        };

        // Two versions of command-line handling
        //  1. the whole command-line is in one string.
        //  2. the command-line is seperated into parts.
        // This class abstracts that difference.
        class arguments_t
        {
        public:
            inline arguments_t() : m_cmdline(nullptr), m_argc(0), m_argv(nullptr) {}
            inline arguments_t(const char* cmdline) : m_cmdline(cmdline), m_argc(0), m_argv(nullptr), m_len(-1) {}
            inline arguments_t(s32 argc, const char** argv) : m_cmdline(nullptr), m_argc(argc), m_argv(argv), m_len(-1) {}

            void init()
            {
                s64 l = 0;
                if (m_cmdline != nullptr)
                {
                    l = ascii::strlen(m_cmdline, nullptr);
                }
                else if (m_argc > 0)
                {
                    s32 i = 0;
                    while (i < m_argc)
                    {
                        l += ascii::strlen(m_argv[i], nullptr);
                        i += 1;
                    }
                }
                m_len = (s32)l;
            }

            s32 len() const { return m_len; }

            // --switch "value"
            enum etype
            {
                PARAM_ERR = 0,
                PARAM_KEY = 1,
                PARAM_VAL = 2,
            };
            etype get(paramstr_t& p);

            const char* get_str(s32 pos) const
            {
                if (m_cmdline != nullptr && pos < m_len)
                {
                    return &m_cmdline[pos];
                }
                else
                {
                    s32 i = 0;
                    s32 l = 0;
                    while (i < m_argc)
                    {
                        l += (s32)ascii::strlen(m_argv[i], nullptr);
                        if (pos < l)
                        {
                            return &m_argv[i][l - pos];
                        }
                        i += 1;
                    }
                }

                return " ";
            }

            char get_char(s32 pos) const
            {
                if (m_cmdline != nullptr && pos < m_len)
                {
                    return m_cmdline[pos];
                }
                else
                {
                    s32 i = 0;
                    s32 l = 0;
                    while (i < m_argc)
                    {
                        l += (s32)ascii::strlen(m_argv[i], nullptr);
                        if (pos < l)
                        {
                            return m_argv[i][l - pos];
                        }
                        i += 1;
                    }
                }

                return ' ';
            }

        private:
            s32          m_len;
            const char*  m_cmdline;
            s32          m_argc;
            const char** m_argv;
        };

        struct context_t
        {
            context_t() : m_cmdline(nullptr), m_casesensitive(true) {}

            const char* m_cmdline;
            bool       m_casesensitive;
            paramstr_t  m_cmd;
            cmds_t      m_cmds;

            void clear()
            {
                m_cmd.clear();
                m_casesensitive = true;
            }
        };

        static bool is_argv_nil(argv_t* argv) { return argv->m_short == argv_t::nil.m_short && argv->m_long == argv_t::nil.m_long; }
        static bool is_argl_nil(argl_t* argl) { return argl->m_name == argl_t::nil.m_name && argl->m_argv == argl_t::nil.m_argv; }

        static argv_t* find_argv(argl_t* argl, paramstr_t& argv)
        {
            argv_t* argvs = argl->m_argv;
            while (!is_argv_nil(argvs))
            {
                if (argv.compare(argvs->m_long) == 0 || argv.compare(argvs->m_short) == 0)
                {
                    return argvs;
                }

                argvs++;
            }
            return nullptr;
        }

        static bool set_argv_value(argv_t* argv, paramstr_t& value_str) { return value_str.to_value(argv->m_value); }

        static argl_t* find_argl(cmds_t& cmd, paramstr_t& argcmd)
        {
			cmd.m_index = 0;
            argl_t* argls = cmd.m_argl;
            while (!is_argl_nil(argls))
            {
                if (compare("", argls->m_name, false) == 0)
                {
                    return argls;
                }
                else if (compare("default", argls->m_name, false) == 0)
                {
                    return argls;
                }
                else if (argcmd.compare(argls->m_name) == 0)
                {
                    return argls;
                }

				cmd.m_index += 1;
                argls++;
            }
            return nullptr;
        }

        class parser_t
        {
        public:
            inline parser_t(cmds_t& c) : m_cmds(c), m_argl(nullptr) {}

            bool parse(const char* cmdline);
            bool parse(s32 argc, const char** argv);

        private:
            bool parse();

            bool matchParameters(s32 pos, s32 offset) const;
            bool matchParameter(s32 pos, s32& ioOffset) const;
            bool matchParameterStruct(s32 pos, s32& ioOffset, paramstr_t& outName, paramstr_t& outValue) const;
            bool matchParameterName(s32 pos, s32& ioOffset) const;
            bool matchParameterSeparator(s32 pos) const;
            bool matchParameterValue(s32 pos, s32& end_pos, s32& ioOffset) const;

            bool matchBoolean(const char* string, s32 length) const;
            bool matchInteger(const char* string, s32 stringLen) const;
            bool matchFloatNumber(const char* string, s32 stringLen) const;

            typedef bool (parser_t::*matchDelegate)(s32) const;

            s32 advanceWhile(s32 pos, matchDelegate matcher) const;
            s32 advanceWhileNot(s32 pos, matchDelegate matcher) const;

            bool matchAllNotOf(s32 pos, char* cc) const;
            bool matchParameterNameChar(s32 pos) const;
            bool matchParameterValueChar(s32 pos) const;
            bool matchParameterValueFirstChar(s32 pos) const;
            bool match(s32 pos, char c) const;
            bool matchSlash(s32 pos) const;
            bool matchMinus(s32 pos) const;
            bool matchQuestion(s32 pos) const;
            bool matchColon(s32 pos) const;
            bool matchSpace(s32 pos) const;
            bool matchQuote(s32 pos) const;
            bool matchDoubleQuote(s32 pos) const;
            bool matchTerminator(s32 pos) const;

            cmds_t&     m_cmds;
            argl_t*     m_argl;
            arguments_t m_args;
        };

        bool parser_t::parse(const char* cmdline)
        {
            if (cmdline == nullptr)
                return false;

            m_args = arguments_t(cmdline);
            m_args.init();

            return parse();
        }

        bool parser_t::parse(s32 argc, const char** argv)
        {
            if (argc == 0 || argv == nullptr)
                return false;

            m_args = arguments_t(argc, argv);
            m_args.init();

            return parse();
        }

        bool parser_t::parse()
        {
            s32   pos      = 0;
            s32   offset   = 0;
            bool _success = false;

            if (m_args.len() > 0)
            {
                if (matchParameterValueFirstChar(offset))
                {
                    pos = advanceWhile(offset, &parser_t::matchParameterValueChar);
                    paramstr_t cmd(m_args.get_str(offset), m_args.get_str(pos));
                    m_argl = find_argl(m_cmds, cmd);
                }
                else
                {
                    paramstr_t cmd("");
                    m_argl = find_argl(m_cmds, cmd);
                }

                if (m_argl == nullptr)
                {
                    return false;
                }

                pos    = advanceWhile(pos, &parser_t::matchSpace);
                offset = pos;

                if (matchParameters(pos, offset))
                {
                    _success = true;
                }
            }
            else
            {
                _success = true;
            }

            return _success;
        }

        bool parser_t::matchParameters(s32 pos, s32 offset) const
        {
            /*overload matchParameter function*/
            while (pos < m_args.len() && matchParameter(pos, offset))
            {
                pos = offset;
                pos = advanceWhile(pos, &parser_t::matchSpace);
            }

            if (pos == m_args.len())
            {
                return true;
            }

            return false;
        }

        bool parser_t::matchParameter(s32 pos, s32& ioOffset) const
        {
            if (matchSlash(pos))
            {
                pos++;
            }
            else if (matchMinus(pos))
            {
                pos++;

                if (matchMinus(pos))
                {
                    pos++;
                }
            }
            else if (!matchQuestion(pos))
            {
                return false;
            }

            paramstr_t arg_name, arg_value;
            if (matchParameterStruct(pos, ioOffset, arg_name, arg_value))
            {
                // @TODO: Find ArgV in @ArgL and set the value
                bool   result = true;
                argv_t* argv   = find_argv(m_argl, arg_name);
                if (argv != nullptr)
                {
                    result = set_argv_value(argv, arg_value);
                }
                else
                {
                    result = false;
                }

                pos = ioOffset;

                // Error? (pos < (mParametersString.Length-1))
                if (pos < m_args.len() && !matchSpace(pos))
                {
                    result = false;
                }
                return result;
            }
            return false;
        }

        bool parser_t::matchBoolean(const char* string, s32 length) const
        {
            const char* boolean_strings[] = {"false", "no", "off", "0", "true", "yes", "on", "1", nullptr};
            const bool  boolean_values[]  = {false, false, false, false, true, true, true, true, false};

            s32 i = 0;
            while (boolean_strings[i] != nullptr)
            {
                const char* bool_str = boolean_strings[i];
                bool const  result   = crunes_t(string, string + length) == crunes_t(bool_str);
                if (result)
                    return boolean_values[i];
            };
            return false;
        }

        bool parser_t::matchFloatNumber(const char* string, s32 stringLen) const
        {
            s32 dotNum = 0;
            for (s32 i = 0; i < stringLen; i++)
            {
                /*all the float number character should be 0~9 or '.', and the number of '.' should be only one*/
                if (string[i] < 48 || string[i] > 57)
                {
                    if (string[i] == '.' && dotNum == 0)
                    {
                        dotNum++;
                    }
                    /*the last character can be 'f' or 'F'*/
                    else if ((i == (stringLen - 1)) && (string[i] == 'f' || string[i] == 'F'))
                    {
                        return true;
                    }
                    else
                    {
                        return false;
                    }
                }
            }
            if (dotNum == 1)
                return true;
            return false;
        }

        bool parser_t::matchInteger(const char* string, s32 stringLen) const
        {
            for (s32 i = 0; i < stringLen; i++)
            {
                if (string[i] < 48 || string[i] > 57)
                    return false;
            }
            return true;
        }

        bool parser_t::matchParameterStruct(s32 pos, s32& ioOffset, paramstr_t& outName, paramstr_t& outValue) const
        {
            if (matchParameterName(pos, ioOffset))
            {
                outName  = paramstr_t(m_args.get_str(pos), m_args.get_str(ioOffset));
                outValue = paramstr_t("true");

                pos = ioOffset;

                if (pos < m_args.len())
                {
                    if (matchParameterSeparator(pos))
                    {
                        pos++;
                        while (matchSpace(pos))
                            pos++;

                        s32 end_pos;
                        if (matchParameterValue(pos, end_pos, ioOffset))
                        {
                            outValue = paramstr_t(m_args.get_str(pos), m_args.get_str(end_pos));

                            if (outValue.empty())
                            {
                                outValue = paramstr_t("true");
                            }
                            else if (outValue.startsWith('\''))
                            {
                                outValue.trim('\'');
                            }
                            else if (outValue.startsWith('\"'))
                            {
                                outValue.trim('\"');
                            }

                            pos = ioOffset;
                        }
                    }
                }

                ioOffset = pos;
                return true;
            }

            return false;
        }

        bool parser_t::matchParameterName(s32 pos, s32& ioOffset) const
        {
            s32 pos2 = pos;
            pos      = advanceWhile(pos, &parser_t::matchParameterNameChar);

            if (pos > pos2)
            {
                ioOffset = pos;
                return true;
            }
            return false;
        }

        bool parser_t::matchParameterSeparator(s32 pos) const
        {
            char c = m_args.get_char(pos);
            if (c == ':' || c == '=' || c == ' ')
                return true;
            return false;
        }

        bool parser_t::matchParameterValue(s32 pos, s32& end_pos, s32& ioOffset) const
        {
            if (matchQuote(pos))
            {
                pos = advanceWhileNot(pos + 1, &parser_t::matchQuote);
                if (matchQuote(pos))
                {
                    end_pos  = pos;
                    ioOffset = pos + 1;
                    return true;
                }
            }
            else if (matchDoubleQuote(pos))
            {
                pos = advanceWhileNot(pos + 1, &parser_t::matchDoubleQuote);
                if (matchDoubleQuote(pos))
                {
                    end_pos  = pos;
                    ioOffset = pos + 1;
                    return true;
                }
            }
            else if (matchParameterValueFirstChar(pos))
            {
                pos      = advanceWhile(pos + 1, &parser_t::matchParameterValueChar);
                end_pos  = pos;
                ioOffset = pos;
                return true;
            }
            return false;
        }

        s32 parser_t::advanceWhile(s32 pos, matchDelegate matcher) const
        {
            while (pos < m_args.len() && (this->*matcher)(pos))
                pos++;
            return pos;
        }

        s32 parser_t::advanceWhileNot(s32 pos, matchDelegate matcher) const
        {
            while (pos < m_args.len() && !(this->*matcher)(pos))
                pos++;
            return pos;
        }

        bool parser_t::matchAllNotOf(s32 pos, char* cc) const
        {
            while (*cc != '\0')
            {
                char c = *cc++;
                if (m_args.get_char(pos) == c)
                    return false;
            }
            return true;
        }

        bool parser_t::matchParameterNameChar(s32 pos) const
        {
            char cc[] = {' ', ':', '=', '\0'};
            return matchAllNotOf(pos, cc);
        }

        bool parser_t::matchParameterValueChar(s32 pos) const
        {
            char cc[] = {' ', '\0'};
            return matchAllNotOf(pos, cc);
        }

        bool parser_t::matchParameterValueFirstChar(s32 pos) const
        {
            char cc[] = {' ', '/', ':', '-', '=', '?', '\0'};
            return matchAllNotOf(pos, cc);
        }

        bool parser_t::match(s32 pos, char c) const { return (m_args.get_char(pos) == c); }
        bool parser_t::matchSlash(s32 pos) const { return match(pos, '/'); }
        bool parser_t::matchMinus(s32 pos) const { return match(pos, '-'); }
        bool parser_t::matchQuestion(s32 pos) const { return match(pos, '?'); }
        bool parser_t::matchColon(s32 pos) const { return match(pos, ':'); }
        bool parser_t::matchSpace(s32 pos) const { return match(pos, ' '); }
        bool parser_t::matchQuote(s32 pos) const { return match(pos, '\''); }
        bool parser_t::matchDoubleQuote(s32 pos) const { return match(pos, '\"'); }
        bool parser_t::matchTerminator(s32 pos) const { return match(pos, '\0'); }

        bool cmdline_t::parse(argv_t* arg, const char* cmdline)
        {
            argl_t argl("", arg);
            cmds_t c(&argl);
            parser_t p(c);
            bool  res = p.parse(cmdline);
            return res;
        }

        bool cmdline_t::parse(argv_t* arg, s32 argc, const char** argv)
        {
            argl_t argl("", arg);
            cmds_t c(&argl);
            parser_t p(c);
            bool  res = p.parse(argc, argv);
            return res;
        }

        bool cmdline_t::parse(cmds_t& c, const char* cmdline)
        {
            parser_t p(c);
            bool  res = p.parse(cmdline);
            return res;
        }

        bool cmdline_t::parse(cmds_t& c, s32 argc, const char** argv)
        {
            parser_t p(c);
            bool  res = p.parse(argc, argv);
            return res;
        }

    }; // namespace cli
} // namespace ncore
