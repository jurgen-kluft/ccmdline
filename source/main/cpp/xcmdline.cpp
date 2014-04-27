#include "xbase\x_string.h"
#include "xbase\x_string_std.h"
#include "xbase\x_types.h"
#include "xbase\x_allocator.h"

#include "xcmdline\xcmdline.h"


namespace xcore
{
	namespace xcmdline
	{
		struct xparamstr
		{
			xparamstr()
				: mStr(NULL)
				, mLen(0)
			{

			}
			xparamstr(char* str, s32 len)
				: mStr(str)
				, mLen(len)
			{
			}

			bool	empty() const					{ return mStr == NULL || mLen == 0; }
			s32		len() const						{ return mLen; }

			void	clear()
			{
				mStr = NULL;
				mLen = 0;
			}

			bool	startsWith(char c) const
			{
				if (empty())
					return false;
				return mStr[0] == c;
			}

			bool	endsWith(char c) const
			{
				if (empty())
					return false;
				return mStr[mLen - 1] == c;
			}

			void	trim(char c)
			{
				if (startsWith(c))
				{
					mStr++;
					--mLen;
				}
				if (endsWith(c))
				{
					--mLen;
				}
			}

			void	set(xparamstr const& other)
			{
				mStr = other.mStr;
				mLen = other.mLen;
			}

			s32		compare(xparamstr const& other) const
			{
				return x_strCompareNoCase(mStr, mLen, other.mStr, other.mLen);
			}

		private:
			char*	mStr;
			s32		mLen;
		};

		struct xparam
		{
		public:
			inline			xparam() : mNext(NULL), mPrev(NULL)	{mValueType = TYPE_UNSUPPORTED;}
			inline			xparam(xparamstr const& key, xparamstr const& value) 
				: mKey(key)
				, mValue(value)
				, mNext(NULL)
				, mPrev(NULL)
			{
				mValueType = TYPE_UNSUPPORTED;
			}

			void			insertAfter(xparam* new_node)
			{
				new_node->mNext = this->mNext;
				if (this->mNext != NULL)
				{
					new_node->mPrev = this->mNext->mPrev;
					this->mNext->mPrev = new_node;
				}
				this->mNext = new_node;
			}

			void			insertBefore(xparam* new_node)
			{
				new_node->mPrev = NULL;
				new_node->mNext = this;
				this->mPrev = new_node;
			}

			xparam*			next()													{ return mNext; }

			void			overrideValue(xparamstr const& value)					{ mValue = value; }

			void*			operator new(xcore::xsize_t num_bytes, void* mem)		{ return mem; }
			void			operator delete(void* pMem, void* )						{ }

			s32				compareKey(const xparamstr& otherkey) const				{ return mKey.compare(otherkey); }

		private:
			xparamstr		mKey;
			xparamstr		mValue;
			enum EValueType
			{
				TYPE_BOOL,                      
				TYPE_INT,
				TYPE_FLOAT,
				TYPE_STRING,
				TYPE_CHAR,
				TYPE_UNSUPPORTED
			};
			EValueType		mValueType;
			xparam*			mNext;
			xparam*			mPrev;
		};

		struct xparams
		{
			xparams(x_iallocator* allocator) 
				: mParametersString(NULL)
				, mAllocator(allocator)
				, mParams(NULL)
				, mCaseSensitive(true)
			{

			}

			XCORE_CLASS_PLACEMENT_NEW_DELETE
			
			char*			mParametersString;
			x_iallocator*	mAllocator;
			xparam*			mParams;
			xparamstr		mCmd;
			xbool			mCaseSensitive;

			void			clear()
			{
				mCmd.clear();
				mCaseSensitive = true;

				// deallocate parameter list
				xparam* i = mParams;
				while (i!=NULL)
				{
					xparam* d = i;
					i = i->next();
					
					d->~xparam();
					mAllocator->deallocate(d);
				}
				mParams = NULL;
			}

			void			add(xparamstr const& _key, xparamstr const& _value)
			{
				xparam* p = NULL;
				xparam* i = mParams;
				while (i!=NULL)
				{
					s32 e = i->compareKey(_key);
					if (e == 0)
					{
						i->overrideValue(_value);
						return;
					}
					else if (e == 1)
					{
						break;
					}
					else
					{
						p = i;
						i = i->next();
					}
				}

				// Key doesn't exist yet
				void* new_node_mem = mAllocator->allocate(sizeof(xparam), 4);
				xparam* new_node = new (new_node_mem) xparam(_key, _value);

				if (mParams == NULL)
				{
					mParams = new_node;
				}
				else if (p!=NULL)
				{
					p->insertAfter(new_node);
				}
				else if (i!=NULL)
				{
					i->insertBefore(new_node);
					if (i==mParams)
						mParams = new_node;
				}
			}
		};


		class xparser
		{
		public:
			inline			xparser(x_iallocator* allocator)
				: mAllocator(allocator)
				, mParametersStringLen(0)
				, mParametersString(NULL)
			{
				x_strcpy(mValueBooleanTrue, sizeof(mValueBooleanTrue), "true");
			}

			xbool			parse(char* parameters_string, xparams& outParams);

		private:
			xbool			matchParameters(s32 pos, s32 offset, xparams& params) const;
			xbool			matchParameter(s32 pos, s32& ioOffset, xparams& params) const;
			xbool			matchParameterStruct(s32 pos, s32& ioOffset, xparamstr& outName, xparamstr& outValue) const;
			xbool			matchParameterName(s32 pos, s32& ioOffset) const;
			xbool			matchParameterSeparator(s32 pos) const;
			xbool			matchParameterValue(s32 pos, s32& ioOffset) const;

			xbool			matchBoolean(const char* string, s32 length) const;
			xbool			matchInteger(const char* string, s32 stringLen) const;
			xbool			matchFloatNumber(const char* string, s32 stringLen) const;

			typedef			xbool (xparser::*matchDelegate)(s32) const;

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

			x_iallocator*	mAllocator;
			s32				mParametersStringLen;
			char*			mParametersString;

			char			mValueBooleanTrue[5];
		};
		

		xbool	xparser::parse(char* parameters_string, xparams& outParams)
		{
			if (parameters_string==NULL)
				return false;

			mParametersString = parameters_string;
			mParametersStringLen = x_strlen(parameters_string);

			s32 pos = 0;
			s32 offset = 0;
			xbool _success = false;

			if (mParametersStringLen>0)
			{
				if (matchParameterValueFirstChar(offset))
				{
					//local definition: pos
					s32 tmp_pos = advanceWhile(offset, &xparser::matchParameterValueChar);

					xparamstr param(mParametersString + offset, tmp_pos - offset);
					outParams.mCmd.set(param);

					offset = tmp_pos;
					//pos released
				}

				offset = advanceWhile(offset, &xparser::matchSpace);

				if (matchParameters(pos, offset, outParams))
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

		xbool	xparser::matchParameters(s32 pos, s32 offset, xparams& params) const
		{
			/*overload matchParameter function*/
			while (pos < mParametersStringLen && matchParameter(pos, offset, params))
			{
				pos = offset;
				pos = advanceWhile(pos, &xparser::matchSpace);
			}

			if (pos == mParametersStringLen)
			{
				return true;
			}

			return false;
		}

		xbool	xparser::matchParameter(s32 pos, s32& ioOffset, xparams& params) const
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
			else if(!matchQuestion(pos))
			{
				return false;
			}

			xparamstr _name, _value;
			if (matchParameterStruct(pos, ioOffset, _name, _value))
			{
				params.add(_name, _value);

				pos = ioOffset;

				// Error? (pos < (mParametersString.Length-1))
				if (pos < mParametersStringLen && !matchSpace(pos))
				{
					return false;
				}
				return true;
			}
			return false;
		}

		xbool	xparser::matchBoolean(const char* string, s32 length) const
		{
			xbool result = false;
			char* tempBoolStr = (char*)mAllocator->allocate(length*sizeof(char)+1,4);
			x_strcpy(tempBoolStr,length+1,string);
			if(x_strcmp("true",tempBoolStr)==0	||	x_strcmp("false",tempBoolStr)==0)
				result = true;
			mAllocator->deallocate(tempBoolStr);
			return result;
		}

		xbool xparser::matchFloatNumber(const char* string,	s32	stringLen) const
		{
			s32		dotNum			=	0;
			for (s32 i=0; i<stringLen; i++)
			{
				/*all the float number character should be 0~9 or '.', and the number of '.' should be only one*/
				if(string[i]	<	48	||	string[i]	>	57)
					if (string[i]	==	'.'	&&	dotNum	==	0)
						dotNum++;
					/*the last character can be 'f' or 'F'*/
					else if(i	==	stringLen-1	&&	(string[i]	==	'f'	||	string[i]	==	'F'))
						return	true;
					else
						return	false;
			}
			if(dotNum==1)
				return	true;
			return false;
		}

		xbool xparser::matchInteger(const char* string, s32 stringLen) const
		{
			for (s32 i=0; i<stringLen; i++)
			{
				if (string[i]	<	48	||	string[i]	>	57)
				return false;
			}
			return true;
		}

		xbool	xparser::matchParameterStruct(s32 pos, s32& ioOffset, xparamstr& outName, xparamstr& outValue) const
		{
			if (matchParameterName(pos, ioOffset))
			{
				outName = xparamstr(mParametersString + pos, ioOffset - pos);
				outValue = xparamstr((char*)&mValueBooleanTrue[0], x_strlen(mValueBooleanTrue));

				pos = ioOffset;

				if (pos < mParametersStringLen)
				{
					if (matchParameterSeparator(pos))
					{
						pos++;
						while (matchSpace(pos)) 
							pos++;

						if (matchParameterValue(pos, ioOffset))
						{
							outValue = xparamstr(mParametersString + pos, ioOffset - pos);

							if (outValue.len() == 0)
							{
								outValue = xparamstr((char*)&mValueBooleanTrue[0], x_strlen(mValueBooleanTrue));
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

		xbool	xparser::matchParameterName(s32 pos, s32& ioOffset) const
		{
			s32 pos2 = pos;
			pos = advanceWhile(pos, &xparser::matchParameterNameChar);

			if (pos > pos2)
			{
				ioOffset = pos;
				return true;
			}
			return false;
		}

		xbool	xparser::matchParameterSeparator(s32 pos) const
		{
			if (mParametersString[pos] == ':' || mParametersString[pos] == '=' || mParametersString[pos] == ' ')
				return true;
			return false;
		}

		xbool	xparser::matchParameterValue(s32 pos, s32& ioOffset) const
		{
			if (matchQuote(pos))
			{
				pos = advanceWhileNot(pos+1, &xparser::matchQuote);
				if (matchQuote(pos))
				{
					pos++;
					ioOffset = pos;
					return true;
				}
			}
			else if (matchDoubleQuote(pos))
			{
				pos = advanceWhileNot(pos+1, &xparser::matchDoubleQuote);
				if (matchDoubleQuote(pos))
				{
					pos++;
					ioOffset = pos;
					return true;
				}
			}
			else if (matchParameterValueFirstChar(pos))
			{
				pos = advanceWhile(pos+1, &xparser::matchParameterValueChar);
				ioOffset = pos;
				return true;
			}
			return false;
		}

		s32	xparser::advanceWhile(s32 pos, matchDelegate matcher) const
		{
			while (pos < mParametersStringLen && (this->*matcher)(pos))
				pos++;
			return pos;
		}

		s32	xparser::advanceWhileNot(s32 pos, matchDelegate matcher) const
		{
			while (pos < mParametersStringLen && !(this->*matcher)(pos))
				pos++;
			return pos;
		}

		xbool xparser::matchAllNotOf(s32 pos, char* cc) const
		{
			while (*cc != '\0')
			{
				char c = *cc++;
				if (mParametersString[pos] == c)
					return false;
			}
			return true;
		}

		xbool xparser::matchParameterNameChar(s32 pos) const
		{
			char cc[] = { ' ', ':', '=', '\0' };
			return matchAllNotOf(pos, cc);
		}

		xbool xparser::matchParameterValueChar(s32 pos) const
		{
			char cc[] = { ' ', '\0' };
			return matchAllNotOf(pos, cc);
		}

		xbool xparser::matchParameterValueFirstChar(s32 pos) const
		{
			char cc[] = { ' ', '/', ':', '-', '=','?', '\0' };
			return matchAllNotOf(pos, cc);
		}

		xbool xparser::match(s32 pos, char c) const
		{
			return (mParametersString[pos] == c);
		}

		xbool xparser::matchSlash(s32 pos) const
		{
			return match(pos, '/');
		}

		xbool xparser::matchMinus(s32 pos) const
		{
			return match(pos, '-');
		}

		xbool	xparser::matchQuestion(s32 pos) const
		{
			return match(pos, '?');
		}

		xbool xparser::matchColon(s32 pos) const
		{
			return match(pos, ':');
		}

		xbool xparser::matchSpace(s32 pos) const
		{
			return match(pos, ' ');
		}

		xbool xparser::matchQuote(s32 pos) const
		{
			return match(pos, '\'');
		}

		xbool xparser::matchDoubleQuote(s32 pos) const
		{
			return match(pos, '\"');
		}

		xbool xparser::matchTerminator(s32 pos) const
		{
			return match(pos, '\0');
		}

	}

	xbool			x_cmdline::parse()
	{
		xcmdline::xparser parser(mAllocator);
		xbool res = parser.parse(mCmdline, *mParameter);

		// now cmdline is correctly parsed and stored in the params
		// what we should do now is set the values according the identifier

		return res;
	}

	xbool			x_cmdline::parse(const char* cmdline)
	{
		s32 const cmdline_len = x_strlen(cmdline);
		mCmdline = (char*)mAllocator->allocate(cmdline_len + 1,4);
		x_strcpy(mCmdline, cmdline_len+1, cmdline);
		return parse();
	}

	xbool			x_cmdline::parse(s32 argc, const char** argv)
	{
		s32 cmdline_len = 0;
		for (s32 i=0; i<argc; ++i)
		{
			const char* arg = argv[i];
			s32 const arg_len = x_strlen(arg);
			cmdline_len += arg_len + 1; 
		}

		s32 const cmdline_len_alloc = cmdline_len;
		mCmdline = (char*)mAllocator->allocate(cmdline_len_alloc+ 1,4);
		mCmdline[cmdline_len_alloc] = '\0';

		cmdline_len = 0;
		for (s32 i=0; i<argc; ++i)
		{
			const char* arg = argv[i];
			x_strcpy(&mCmdline[cmdline_len], cmdline_len_alloc - cmdline_len, arg);
			s32 const arg_len = x_strlen(arg);
			mCmdline[cmdline_len + arg_len] = ' ';
			cmdline_len += arg_len + 1; 
		}

		return parse();
	}

	x_cmdline::x_cmdline(x_iallocator* allocator)
		: mAllocator(allocator)
		, mCmdline(NULL)
	{ 
		void* params_mem = mAllocator->allocate(sizeof(xcmdline::xparams),4);
		mParameter = new (params_mem) xcmdline::xparams(mAllocator);
	}


	x_cmdline::~x_cmdline()
	{
		if (mParameter!=NULL)
		{
			mParameter->clear();
			mAllocator->deallocate(mParameter);
		}
		if (mCmdline != NULL)
		{
			mAllocator->deallocate(mCmdline);
		}
	}
}