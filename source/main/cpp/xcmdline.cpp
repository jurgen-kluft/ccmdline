#include "xbase/x_target.h"
#include "xbase/x_allocator.h"
#include "xbase/x_runes.h"

#include "xcmdline/xcmdline.h"


namespace xcore
{
	namespace cli
	{
		argV	argV::nil(NULL, NULL, NULL, eOPT_OPTIONAL, x_va_r());
		argL	argL::nil(NULL, NULL);

		struct paramstr
		{
			inline		paramstr() : mStr(NULL), mEnd(NULL) { }
			inline		paramstr(const char* str) : mStr(str), mEnd(NULL) { mEnd = ascii::endof(mStr, NULL); }
			inline		paramstr(const char* str, const char* end) : mStr(str), mEnd(end) { }

			bool		empty() const { return mStr == mEnd; }

			bool		startsWith(char c) const;
			bool		endsWith(char c) const;

			void		clear() { mStr = NULL; mEnd = NULL; }
			void		trim(char c);

			s32			compare(const char* str) const;
			s32			compare(paramstr const& other) const;

			bool		to_value(x_va_r& out) const;

		private:
			const char*	mStr;
			const char*	mEnd;
		};

		bool	paramstr::startsWith(char c) const
		{
			if (empty())
				return false;
			return mStr[0] == c;
		}

		bool	paramstr::endsWith(char c) const
		{
			if (empty())
				return false;
			return mEnd[-1] == c;
		}

		void	paramstr::trim(char c)
		{
			if (empty() == false)
			{
				if (startsWith(c))
				{
					mStr += 1;
				}
				if (endsWith(c))
				{
					mEnd -= 1;
				}
			}
		}

		s32		paramstr::compare(const char* str) const
		{
			return ascii::compare(ascii::crunes(mStr, mEnd), ascii::crunes(str), false);
		}

		s32		paramstr::compare(paramstr const& other) const
		{
			return ascii::compare(ascii::crunes(mStr, mEnd), ascii::crunes(other.mStr, other.mEnd), false);
		}

		bool		paramstr::to_value(x_va_r& out) const
		{
			ascii::crunes str(mStr, mEnd);
			out = x_va(str);
			return true;
		}


		struct param
		{
		public:
			inline			param() { }
			inline			param(paramstr const& key, paramstr const& value)
				: mKey(key)
				, mValue(value)
			{
			}

		private:
			paramstr		mKey;
			paramstr		mValue;

		};

		// Two versions of command-line handling
		//  1. the whole command-line is in one string.
		//  2. the command-line is seperated into parts.
		// This class abstracts that difference.
		class arguments
		{
		public:
			inline			arguments() : mCmdline(NULL), mArgc(0), mArgv(NULL) {}
			inline			arguments(const char* cmdline) : mCmdline(cmdline), mArgc(0), mArgv(NULL), mLen(-1) {}
			inline			arguments(s32 argc, const char** argv) : mCmdline(NULL), mArgc(argc), mArgv(argv), mLen(-1) {}

			void			init()
			{
				s64 l = 0;
				if (mCmdline != NULL)
				{
					l = ascii::size(mCmdline);
				}
				else if (mArgc > 0)
				{
					s32 i = 0;
					while (i < mArgc)
					{
						l += ascii::size(mArgv[i]);
						i += 1;
					}
				}
				mLen = (s32)l;
			}

			s32				len() const { return mLen; }

			// --switch "value"
			enum etype 
			{
				PARAM_ERR = 0,
				PARAM_KEY = 1,
				PARAM_VAL = 2,
			};
			etype			get(paramstr& p);

			const char*		get_str(s32 pos) const
			{
				if (mCmdline != NULL && pos < mLen)
				{
					return &mCmdline[pos];
				}
				else
				{
					s32 i = 0;
					s32 l = 0;
					while (i < mArgc)
					{
						l += (s32)ascii::size(mArgv[i]);
						if (pos < l)
						{
							return &mArgv[i][l - pos];
						}
						i += 1;
					}
				}

				return " ";
			}

			char			get_char(s32 pos) const 
			{
				if (mCmdline != NULL && pos < mLen)
				{
					return mCmdline[pos];
				}
				else
				{
					s32 i = 0;
					s32 l = 0;
					while (i < mArgc)
					{
						l += (s32)ascii::size(mArgv[i]);
						if (pos < l)
						{
							return mArgv[i][l - pos];
						}
						i += 1;
					}
				}

				return ' ';
			}

		private:
			s32				mLen;

			const char*		mCmdline;

			s32				mArgc;
			const char**	mArgv;
		};


		struct context
		{
			context()
				: mCmdLine(NULL)
				, mCaseSensitive(true)
			{
			}

			const char*		mCmdLine;
			xbool			mCaseSensitive;

			paramstr		mCmd;
			cmds			mCmds;

			void			clear()
			{
				mCmd.clear();
				mCaseSensitive = true;
			}
		};

		static xbool	is_argv_nil(argV* argv)
		{
			return argv->mShort == argV::nil.mShort && argv->mLong == argV::nil.mLong;
		}

		static xbool	is_argl_nil(argL* argl)
		{
			return argl->mName == argL::nil.mName && argl->mArgV == argL::nil.mArgV;
		}

		static argV*	find_argv(argL* argl, paramstr& argv)
		{
			argV* argvs = argl->mArgV;
			while (!is_argv_nil(argvs))
			{
				if (argv.compare(argvs->mLong) == 0 || argv.compare(argvs->mShort) == 0)
				{
					return argvs;
				}

				argvs++;
			}
			return NULL;
		}

		static xbool	set_argv_value(argV* argv, paramstr& value_str)
		{
			return value_str.to_value(argv->mValue);
		}

		static argL*	find_argl(cmds& cmd, paramstr& argcmd)
		{
			argL* argls = cmd.mArgL;
			while (!is_argl_nil(argls))
			{
				if (ascii::compare("", argls->mName, false) == 0)
				{
					return argls;
				}
				else if (ascii::compare("default", argls->mName, false) == 0)
				{
					return argls;
				}
				else if (argcmd.compare(argls->mName) == 0)
				{
					return argls;
				}

				argls++;
			}
			return NULL;
		}


		class parser
		{
		public:
			inline			parser(cmds& c)
				: mCmds(c)
				, mArgL(NULL)
			{
			}

			xbool			parse(const char* cmdline);
			xbool			parse(s32 argc, const char** argv);

		private:
			xbool			parse();

			xbool			matchParameters(s32 pos, s32 offset) const;
			xbool			matchParameter(s32 pos, s32& ioOffset) const;
			xbool			matchParameterStruct(s32 pos, s32& ioOffset, paramstr& outName, paramstr& outValue) const;
			xbool			matchParameterName(s32 pos, s32& ioOffset) const;
			xbool			matchParameterSeparator(s32 pos) const;
			xbool			matchParameterValue(s32 pos, s32& end_pos, s32& ioOffset) const;

			xbool			matchBoolean(const char* string, s32 length) const;
			xbool			matchInteger(const char* string, s32 stringLen) const;
			xbool			matchFloatNumber(const char* string, s32 stringLen) const;

			typedef			xbool(parser::*matchDelegate)(s32) const;

			s32				advanceWhile(s32 pos, matchDelegate matcher) const;
			s32				advanceWhileNot(s32 pos, matchDelegate matcher) const;

			xbool			matchAllNotOf(s32 pos, char* cc) const;
			xbool			matchParameterNameChar(s32 pos) const;
			xbool			matchParameterValueChar(s32 pos) const;
			xbool			matchParameterValueFirstChar(s32 pos) const;
			xbool			match(s32 pos, char c) const;
			xbool			matchSlash(s32 pos) const;
			xbool			matchMinus(s32 pos) const;
			xbool			matchQuestion(s32 pos) const;
			xbool			matchColon(s32 pos) const;
			xbool			matchSpace(s32 pos) const;
			xbool			matchQuote(s32 pos) const;
			xbool			matchDoubleQuote(s32 pos) const;
			xbool			matchTerminator(s32 pos)  const;

			cmds&			mCmds;
			argL*			mArgL;
			arguments		mArgs;
		};

		xbool	parser::parse(const char * cmdline)
		{
			if (cmdline == NULL)
				return false;

			mArgs = arguments(cmdline);
			mArgs.init();
			
			return parse();
		}


		xbool	parser::parse(s32 argc, const char** argv)
		{
			if (argc == 0 || argv == NULL)
				return false;

			mArgs = arguments(argc, argv);
			mArgs.init();

			return parse();
		}

		xbool	parser::parse()
		{
			s32 pos = 0;
			s32 offset = 0;
			xbool _success = false;

			if (mArgs.len() > 0)
			{
				if (matchParameterValueFirstChar(offset))
				{
					pos = advanceWhile(offset, &parser::matchParameterValueChar);
					paramstr cmd(mArgs.get_str(offset), mArgs.get_str(pos));
					mArgL = find_argl(mCmds, cmd);
				}
				else
				{
					paramstr cmd("");
					mArgL = find_argl(mCmds, cmd);
				}

				if (mArgL == NULL)
				{
					return false;
				}

				pos = advanceWhile(pos, &parser::matchSpace);
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

		xbool	parser::matchParameters(s32 pos, s32 offset) const
		{
			/*overload matchParameter function*/
			while (pos < mArgs.len() && matchParameter(pos, offset))
			{
				pos = offset;
				pos = advanceWhile(pos, &parser::matchSpace);
			}

			if (pos == mArgs.len())
			{
				return true;
			}

			return false;
		}

		xbool	parser::matchParameter(s32 pos, s32& ioOffset) const
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

			paramstr arg_name, arg_value;
			if (matchParameterStruct(pos, ioOffset, arg_name, arg_value))
			{
				// @TODO: Find ArgV in @ArgL and set the value
				xbool result = true;
				argV* argv = find_argv(mArgL, arg_name);
				if (argv != NULL)
				{
					result = set_argv_value(argv, arg_value);
				}
				else
				{
					result = false;
				}
				
				pos = ioOffset;

				// Error? (pos < (mParametersString.Length-1))
				if (pos < mArgs.len() && !matchSpace(pos))
				{
					result = false;
				}
				return result;
			}
			return false;
		}

		xbool	parser::matchBoolean(const char* string, s32 length) const
		{
			const char* boolean_strings[] = { "false", "no", "off", "0", "true", "yes", "on", "1", NULL };
			const bool boolean_values[] = { false,false,false,false, true,true,true,true, NULL };

			s32 i = 0;
			while (boolean_strings[i] != NULL)
			{
				const char* bool_str = boolean_strings[i];
				bool const result = ascii::crunes(string, string + length) == ascii::crunes(bool_str);
				if (result)
					return boolean_values[i];
			};
			return false;
		}

		xbool parser::matchFloatNumber(const char* string, s32	stringLen) const
		{
			s32		dotNum = 0;
			for (s32 i = 0; i < stringLen; i++)
			{
				/*all the float number character should be 0~9 or '.', and the number of '.' should be only one*/
				if (string[i] < 48 || string[i]	>	57)
				{
					if (string[i] == '.' &&	dotNum == 0)
					{
						dotNum++;
					}
				/*the last character can be 'f' or 'F'*/
					else if ((i == (stringLen - 1)) && (string[i] == 'f' || string[i] == 'F'))
					{
						return	true;
					}
					else
					{
						return	false;
					}
				}
			}
			if (dotNum == 1)
				return	true;
			return false;
		}

		xbool parser::matchInteger(const char* string, s32 stringLen) const
		{
			for (s32 i = 0; i < stringLen; i++)
			{
				if (string[i] < 48 || string[i]	>	57)
					return false;
			}
			return true;
		}

		xbool	parser::matchParameterStruct(s32 pos, s32& ioOffset, paramstr& outName, paramstr& outValue) const
		{
			if (matchParameterName(pos, ioOffset))
			{
				outName = paramstr(mArgs.get_str(pos), mArgs.get_str(ioOffset));
				outValue = paramstr("true");

				pos = ioOffset;

				if (pos < mArgs.len())
				{
					if (matchParameterSeparator(pos))
					{
						pos++;
						while (matchSpace(pos))
							pos++;

						s32 end_pos;
						if (matchParameterValue(pos, end_pos, ioOffset))
						{
							outValue = paramstr(mArgs.get_str(pos), mArgs.get_str(end_pos));

							if (outValue.empty())
							{
								outValue = paramstr("true");
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

		xbool	parser::matchParameterName(s32 pos, s32& ioOffset) const
		{
			s32 pos2 = pos;
			pos = advanceWhile(pos, &parser::matchParameterNameChar);

			if (pos > pos2)
			{
				ioOffset = pos;
				return true;
			}
			return false;
		}

		xbool	parser::matchParameterSeparator(s32 pos) const
		{
			char c = mArgs.get_char(pos);
			if (c == ':' || c == '=' || c == ' ')
				return true;
			return false;
		}

		xbool	parser::matchParameterValue(s32 pos, s32& end_pos, s32& ioOffset) const
		{
			if (matchQuote(pos))
			{
				pos = advanceWhileNot(pos + 1, &parser::matchQuote);
				if (matchQuote(pos))
				{
					end_pos = pos;
					ioOffset = pos + 1;
					return true;
				}
			}
			else if (matchDoubleQuote(pos))
			{
				pos = advanceWhileNot(pos + 1, &parser::matchDoubleQuote);
				if (matchDoubleQuote(pos))
				{
					end_pos = pos;
					ioOffset = pos + 1;
					return true;
				}
			}
			else if (matchParameterValueFirstChar(pos))
			{
				pos = advanceWhile(pos + 1, &parser::matchParameterValueChar);
				end_pos = pos;
				ioOffset = pos;
				return true;
			}
			return false;
		}

		s32	parser::advanceWhile(s32 pos, matchDelegate matcher) const
		{
			while (pos < mArgs.len() && (this->*matcher)(pos))
				pos++;
			return pos;
		}

		s32	parser::advanceWhileNot(s32 pos, matchDelegate matcher) const
		{
			while (pos < mArgs.len() && !(this->*matcher)(pos))
				pos++;
			return pos;
		}

		xbool parser::matchAllNotOf(s32 pos, char* cc) const
		{
			while (*cc != '\0')
			{
				char c = *cc++;
				if (mArgs.get_char(pos) == c)
					return false;
			}
			return true;
		}

		xbool parser::matchParameterNameChar(s32 pos) const
		{
			char cc[] = { ' ', ':', '=', '\0' };
			return matchAllNotOf(pos, cc);
		}

		xbool parser::matchParameterValueChar(s32 pos) const
		{
			char cc[] = { ' ', '\0' };
			return matchAllNotOf(pos, cc);
		}

		xbool parser::matchParameterValueFirstChar(s32 pos) const
		{
			char cc[] = { ' ', '/', ':', '-', '=','?', '\0' };
			return matchAllNotOf(pos, cc);
		}

		xbool parser::match(s32 pos, char c) const
		{
			return (mArgs.get_char(pos) == c);
		}

		xbool parser::matchSlash(s32 pos) const
		{
			return match(pos, '/');
		}

		xbool parser::matchMinus(s32 pos) const
		{
			return match(pos, '-');
		}

		xbool	parser::matchQuestion(s32 pos) const
		{
			return match(pos, '?');
		}

		xbool parser::matchColon(s32 pos) const
		{
			return match(pos, ':');
		}

		xbool parser::matchSpace(s32 pos) const
		{
			return match(pos, ' ');
		}

		xbool parser::matchQuote(s32 pos) const
		{
			return match(pos, '\'');
		}

		xbool parser::matchDoubleQuote(s32 pos) const
		{
			return match(pos, '\"');
		}

		xbool parser::matchTerminator(s32 pos) const
		{
			return match(pos, '\0');
		}

		xbool			instance::parse(argV * arg, const char* cmdline)
		{
			argL argl("", arg);
			cmds c(&argl);
			parser parser(c);
			xbool res = parser.parse(cmdline);
			return res;
		}

		xbool			instance::parse(argV * arg, s32 argc, const char** argv)
		{
			argL argl("", arg);
			cmds c(&argl);
			parser p(c);
			xbool res = p.parse(argc, argv);
			return res;
		}

		xbool			instance::parse(cmds & c, const char* cmdline)
		{
			parser p(c);
			xbool res = p.parse(cmdline);
			return res;
		}

		xbool			instance::parse(cmds & c, s32 argc, const char** argv)
		{
			parser p(c);
			xbool res = p.parse(argc, argv);
			return res;
		}

	};
}
