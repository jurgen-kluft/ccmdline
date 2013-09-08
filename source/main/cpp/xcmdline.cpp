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

			const char*		mStr;
			s32				mLen;

			void	clear()
			{
				mStr = NULL;
				mLen = 0;
			}
		};

		struct xparam
		{
		public:
			xparam() : mNext(NULL), mPrev(NULL)	{mValueType = TYPE_OTHER;}

			xparamstr		mKey;
			xparamstr		mValue;
			enum EValueType
			{
				TYPE_BOOL,                      
				TYPE_INT,
				TYPE_FLOAT,
				TYPE_STRING,
				TYPE_CHAR,
				TYPE_OTHER						//like --version,?a,etc				
			};
			EValueType		mValueType;
			xparam*			mNext;
			xparam*			mPrev;

			void*		operator new(xcore::xsize_t num_bytes, void* mem)		{ return mem; }
			void			operator delete(void* pMem, void* )			{ }

		};

		struct xparams
		{
			xparams() 
				: mParams(NULL)
				, mCaseSensitive(true)
			{

			}
			
			const char*		mParametersString;
			x_iallocator*	mAllocator;
			xparam*			mParams;
			xparamstr		mCmd;
			xbool			mCaseSensitive;

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
					mAllocator->deallocate(d);
				}
				mParams = NULL;
			}

			void			add(xparam const& _param)
			{
				xparam* p = NULL;
				xparam* i = mParams;
				while (i!=NULL)
				{
					s32 e = x_strCompareNoCase(_param.mKey.mStr, _param.mKey.mLen, i->mKey.mStr, i->mKey.mLen);
					if (e == 0)
					{
						i->mValue.mStr = _param.mValue.mStr;
						i->mValue.mLen = _param.mValue.mLen;
						return;
					}
					else if (e == -1)
						break;
					p = i;
					i = i->mNext;
				}

				// Key doesn't exist yet
				void* new_node_mem = mAllocator->allocate(sizeof(xparam), 4);
				xparam* new_node = new (new_node_mem) xparam();
				new_node->mKey = _param.mKey;
				new_node->mValue = _param.mValue;
				new_node->mValueType = _param.mValueType;

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

			void			add(xparamstr const& _key, xparamstr const& _value)
			{
				xparam* p = NULL;
				xparam* i = mParams;
				while (i!=NULL)
				{
					s32 e = x_strCompareNoCase(_key.mStr, _key.mLen, i->mKey.mStr, i->mKey.mLen);
					if (e == 0)
					{
						i->mValue.mStr = _value.mStr;
						i->mValue.mLen = _value.mLen;
						return;
					}
					else if (e == -1)
						break;
					p = i;
					i = i->mNext;
				}

				// Key doesn't exist yet
				void* new_node_mem = mAllocator->allocate(sizeof(xparam), 4);
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
			xbool			parse(const char* parameters_string, xparams& outParams);

		private:
			xbool			matchParameters(s32 pos, s32 offset, xparams& params) const;
			xbool			matchParameter(s32 pos, s32& ioOffset, xparams& params) const;
			xbool			matchParameter(s32 pos, s32&	ioOffset, xparams& params, int overload) const;
			xbool			matchParameterStruct(s32 pos, s32& ioOffset, xparamstr& outName, xparamstr& outValue) const;
			xbool			matchParameterStruct(s32 pos, s32& ioOffset, xparam& params) const;
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

			x_iallocator*	mAllocator;
			s32				mParametersStringLen;
			const char*		mParametersString;
		};
		

		xbool	xparser::parse(const char* parameters_string, xparams& outParams)
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
					s32 pos = advanceWhile(offset, &xparser::matchParameterValueChar);

					outParams.mCmd.mStr = mParametersString + offset;
					outParams.mCmd.mLen = pos - offset;

					offset = pos;
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
			while (pos < mParametersStringLen && matchParameter(pos, offset, params,1))
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

		/*just overload the matchParameters to fit for the xparam argument in the matchParameterStruct function.*/
		xbool		xparser::matchParameter(s32 pos, s32&	ioOffset, xparams& params, int overload) const
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

			xparam parameters;
			if (matchParameterStruct(pos, ioOffset, parameters))
			{
				params.add(parameters);

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

		xbool	xparser::matchParameterStruct(s32 pos, s32& ioOffset, xparam& params) const
		{
			if (matchParameterName(pos, ioOffset))
			{
				params.mKey.mStr	=	mParametersString	+	pos;
				params.mKey.mLen	=	ioOffset	-	pos;

				params.mValue.mStr	=	"true";
				params.mValue.mLen	=	x_strlen(params.mValue.mStr);

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

							params.mValue.mStr = mParametersString + pos;
							params.mValue.mLen = ioOffset - pos;

							if (params.mValue.mLen == 0)
							{
								params.mValue.mStr = "true";
								params.mValue.mLen = x_strlen(params.mValue.mStr);
							}
							else if (params.mValue.mStr[0] == '\'' )
							{
								params.mValueType = xcmdline::xparam::TYPE_CHAR;
								params.mValue.mStr += 1;
								params.mValue.mLen -= 1;
								if (params.mValue.mStr[params.mValue.mLen-1] == '\'')
									params.mValue.mLen -= 1;
							}
							else if( params.mValue.mStr[0] == '\"')
							{
								params.mValueType = xcmdline::xparam::TYPE_STRING;
								params.mValue.mStr += 1;
								params.mValue.mLen -= 1;
								if (params.mValue.mStr[params.mValue.mLen-1] == '\"')
									params.mValue.mLen -= 1;
							}
							else if (matchBoolean(params.mValue.mStr,params.mValue.mLen))
							{
								params.mValueType = xcmdline::xparam::TYPE_BOOL;
							}
							else if (matchFloatNumber(params.mValue.mStr,params.mValue.mLen))
							{
								params.mValueType = xcmdline::xparam::TYPE_FLOAT;
								if(params.mValue.mStr[params.mValue.mLen-1] == 'f' ||
									params.mValue.mStr[params.mValue.mLen-1] == 'F')
									params.mValue.mLen -= 1;
							}
							else if (matchInteger(params.mValue.mStr,params.mValue.mLen))
							{
								params.mValueType = xcmdline::xparam::TYPE_INT;
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

		xbool	xparser::matchParameterStruct(s32 pos, s32& ioOffset, xparamstr& outName, xparamstr& outValue) const
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
							else if (outValue.mStr[0] == '\'' || outValue.mStr[0] == '\"')
							{
								outValue.mStr += 1;
								outValue.mLen -= 1;
								if (outValue.mStr[outValue.mLen-1] == '\'' || outValue.mStr[outValue.mLen-1] == '\"')
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
	}
	
	xbool			x_cmdline::JudgeTrueOrFalse(char* _string)
	{
		ASSERT(_string!=NULL);
		ASSERT(x_strcmp(_string,"true")==0 || x_strcmp(_string,"false")==0);

		if(x_strcmp("true",_string)==0)
			return true;
		else 	if(x_strcmp("false",_string)==0)
			return false;
		/*the function should never go to here*/
		else
			return false;
	}



	xbool			x_cmdline::parse(const char* cmdline)
	{
		xcmdline::xparser parser;
		xbool res = parser.parse(cmdline, *mParameter);


		/*now cmdline are correctly parsed and stored in the params
		* what we should do now is set the values according the identifier
		*/

		return res;
	}


	x_cmdline::x_cmdline():mAllocArgvBlockSize(10)
	{ 
		mParameter = (xcmdline::xparams*)mAllocator->allocate(sizeof(xcmdline::xparams),4);
		mParameter->mParams = NULL;
		mParameter->mParametersString = NULL;
		mRegList = NULL;
		mRegListNum = 0;
		mNewCmdline = NULL;
		mArgc = 0;
		mArgv = (char**)mAllocator->allocate(sizeof(char*),4);
		mStringListNum = 0;
		mStringList = NULL;
		mTotalAllocArgvNum = 1;
	}


	x_cmdline::~x_cmdline()
	{
		mAllocator->deallocate(mParameter);
		mAllocator->deallocate(mRegList);

		if (mNewCmdline != NULL)
		{
			mAllocator->deallocate(mNewCmdline);
		}
		mAllocator->deallocate(mArgv);

	}
}