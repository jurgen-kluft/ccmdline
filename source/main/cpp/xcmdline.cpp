#include "xbase\x_string.h"
#include "xbase\x_string_std.h"

#include "xcmdline\xcmdline.h"

#include "xcmdline\private\opt.h"
#include "xcmdline\private\ag.h"
#include "xcmdline\private\opt_p.h"

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

			const char*		mStr;
			s32				mLen;

			void	clear()
			{
				mStr = NULL;
				mLen = 0;
			}
		};

		struct xparameters
		{
			xparameters() 
				: mParams(NULL)
				, mCaseSensitive(true)
			{

			}

			struct xparam
			{
							xparam() : mNext(NULL), mPrev(NULL)	{ }

				xparamstr		mKey;
				xparamstr		mValue;
				enum EValueType
				{
					TYPE_BOOL,
					TYPE_INT,
					TYPE_FLOAT,
					TYPE_STRING,
				};
				EValueType		mValueType;
				xparam*			mNext;
				xparam*			mPrev;

				void*			operator new(u32 num_bytes, void* mem)		{ return mem; }
				void			operator delete(void* pMem, void* )			{ }

			};
			
			const char*		mParametersString;
			xparam*			mParams;
			xparamstr		mCmd;
			bool			mCaseSensitive;

			void			clear()
			{
				mCmd.clear();
				mCaseSensitive = true;
				// deallocate every node
				xparam* i = mParams;
				while (i!=NULL)
				{
					xparam* d = i;
					i = i->mNext;
					
					d->~xparam();
					get_opt_allocator()->deallocate(d);
				}
				mParams = NULL;
			}

			void			add(xparamstr const& _key, xparamstr const& _value)
			{
				xparam* p = NULL;
				xparam* i = mParams;
				while (i!=NULL)
				{
					s32 e = x_strCompareNoCase(_key.mStr, _key.mLen, i->mKey.mStr, i->mKey.mLen);
					if (e == 0)
						return;
					else if (e == -1)
						break;
					p = i;
					i = i->mNext;
				}

				// Key doesn't exist yet
				void* new_node_mem = get_opt_allocator()->allocate(sizeof(xparam), 4);
				xparam* new_node = new (new_node_mem) xparam();
				new_node->mKey = _key;
				new_node->mValue = _value;

				if (p!=NULL)
				{
					// Insert after p
					new_node->mNext = p->mNext;
					if (p->mNext != NULL)
					{
						new_node->mPrev = p->mNext->mPrev;
						p->mNext->mPrev = new_node;
					}
					p->mNext = new_node;
				}
				else if (mParams==NULL)
				{
					mParams = new_node;
				}
				else if (i!=NULL)
				{
					// Insert before head
					new_node->mNext = i;
					i->mPrev = new_node;
					mParams = new_node;
				}
			}
		};


		class xparser
		{
		public:
			bool		parse(const char* parameters_string, xparameters& outParams);

		private:
			bool		matchParameters(s32 pos, s32 offset, xparameters& params) const;
			bool		matchParameter(s32 pos, s32& ioOffset, xparameters& params) const;
			bool		matchParameterStruct(s32 pos, s32& ioOffset, xparamstr& outName, xparamstr& outValue) const;
			bool		matchParameterName(s32 pos, s32& ioOffset) const;
			bool		matchParameterSeparator(s32 pos) const;
			bool		matchParameterValue(s32 pos, s32& ioOffset) const;

			typedef		bool (xparser::*matchDelegate)(s32) const;

			s32			advanceWhile(s32 pos, matchDelegate matcher) const;
			s32			advanceWhileNot(s32 pos, matchDelegate matcher) const;

			bool		matchAllNotOf(s32 pos, char* cc) const;
			bool		matchParameterNameChar(s32 pos) const;
			bool		matchParameterValueChar(s32 pos) const;
			bool		matchParameterValueFirstChar(s32 pos) const;
			bool		match(s32 pos, char c) const;
			bool		matchSlash(s32 pos) const;
			bool		matchMinus(s32 pos) const;
			bool		matchColon(s32 pos) const;
			bool		matchSpace(s32 pos) const;
			bool		matchQuote(s32 pos) const;
			bool		matchDoubleQuote(s32 pos) const;

			s32			mParametersStringLen;
			const char*	mParametersString;
		};
		

		bool	xparser::parse(const char* parameters_string, xparameters& outParams)
		{
			if (parameters_string==NULL)
				return false;

			mParametersString = parameters_string;
			mParametersStringLen = x_strlen(parameters_string);

			s32 pos = 0;
			s32 offset = 0;
			bool _success = false;

			if (mParametersStringLen>0)
			{
				if (matchParameterValueFirstChar(offset))
				{
					s32 pos = advanceWhile(offset, &xparser::matchParameterValueChar);
					
					outParams.mCmd.mStr = mParametersString + offset;
					outParams.mCmd.mLen = pos - offset;
					
					offset = pos;
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

		bool	xparser::matchParameters(s32 pos, s32 offset, xparameters& params) const
		{
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

		bool	xparser::matchParameter(s32 pos, s32& ioOffset, xparameters& params) const
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
			else
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

		bool	xparser::matchParameterStruct(s32 pos, s32& ioOffset, xparamstr& outName, xparamstr& outValue) const
		{
			if (matchParameterName(pos, ioOffset))
			{
				outName.mStr = mParametersString + pos;
				outName.mLen = ioOffset - pos;

				outValue.mStr = "true";
				outValue.mLen = x_strlen(outValue.mStr);

				pos = ioOffset;

				if (pos < mParametersStringLen)
				{
					if (matchParameterSeparator(pos))
					{
						while (matchSpace(pos + 1)) 
							pos++;

						if (matchParameterValue(pos+1, ioOffset))
						{
							pos++;

							outValue.mStr = mParametersString + pos;
							outValue.mLen = ioOffset - pos;

							if (outValue.mLen == 0)
							{
								outValue.mStr = "true";
								outValue.mLen = x_strlen(outValue.mStr);
							}
							else if (outValue.mStr[0] == '\'' || outValue.mStr[0] == '"')
							{
								outValue.mStr += 1;
								outValue.mLen -= 1;
								if (outValue.mStr[outValue.mLen] == '\'' || outValue.mStr[outValue.mLen] == '"')
									outValue.mLen -= 1;
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

		bool	xparser::matchParameterName(s32 pos, s32& ioOffset) const
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

		bool	xparser::matchParameterSeparator(s32 pos) const
		{
			if (mParametersString[pos] == ':' || mParametersString[pos] == '=' || mParametersString[pos] == ' ')
				return true;
			return false;
		}

		bool	xparser::matchParameterValue(s32 pos, s32& ioOffset) const
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

		bool xparser::matchAllNotOf(s32 pos, char* cc) const
		{
			while (*cc != '\0')
			{
				char c = *cc++;
				if (mParametersString[pos] == c)
					return false;
			}
			return true;
		}

		bool xparser::matchParameterNameChar(s32 pos) const
		{
			char cc[] = { ' ', ':', '=', '\0' };
			return matchAllNotOf(pos, cc);
		}

		bool xparser::matchParameterValueChar(s32 pos) const
		{
			char cc[] = { ' ', '\0' };
			return matchAllNotOf(pos, cc);
		}

		bool xparser::matchParameterValueFirstChar(s32 pos) const
		{
			char cc[] = { ' ', '/', ':', '-', '=', '\0' };
			return matchAllNotOf(pos, cc);
		}

		bool xparser::match(s32 pos, char c) const
		{
			return (mParametersString[pos] == c);
		}

		bool xparser::matchSlash(s32 pos) const
		{
			return match(pos, '/');
		}

		bool xparser::matchMinus(s32 pos) const
		{
			return match(pos, '-');
		}

		bool xparser::matchColon(s32 pos) const
		{
			return match(pos, ':');
		}

		bool xparser::matchSpace(s32 pos) const
		{
			return match(pos, ' ');
		}

		bool xparser::matchQuote(s32 pos) const
		{
			return match(pos, '\'');
		}

		bool xparser::matchDoubleQuote(s32 pos) const
		{
			return match(pos, '"');
		}
	}


	bool			x_cmdline::parse(const char* cmdline)
	{
		xcmdline::xparser parser;
		xcmdline::xparameters params;
		bool res = parser.parse(cmdline, params);

		params.clear();
		return res;
	}
}