#include "xbase\x_string.h"
#include "xbase\x_string_std.h"
#include "xbase\x_types.h"

#include "xcmdline\xcmdline.h"

#include "xcmdline\private\opt.h"
#include "xcmdline\private\ag.h"
#include "xcmdline\private\opt_p.h"


#ifdef	OUTDEBUG
#include <iostream>
using namespace std;
#endif

#include <stdio.h>


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

		struct xparameters
		{
			xparameters() 
				: mParams(NULL)
				, mCaseSensitive(true)
			{

			}
			
			const char*		mParametersString;
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
					Opt_Allocator::get_opt_allocator()->deallocate(d);
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
				void* new_node_mem = Opt_Allocator::get_opt_allocator()->allocate(sizeof(xparam), 4);
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
				void* new_node_mem = Opt_Allocator::get_opt_allocator()->allocate(sizeof(xparam), 4);
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
			xbool		parse(const char* parameters_string, xparameters& outParams);

		private:
			xbool		matchParameters(s32 pos, s32 offset, xparameters& params) const;
			xbool		matchParameter(s32 pos, s32& ioOffset, xparameters& params) const;
			xbool		matchParameter(s32 pos, s32&	ioOffset, xparameters& params, int overload) const;
			xbool		matchParameterStruct(s32 pos, s32& ioOffset, xparamstr& outName, xparamstr& outValue) const;
			xbool		matchParameterStruct(s32 pos, s32& ioOffset, xparam& params) const;
			xbool		matchParameterName(s32 pos, s32& ioOffset) const;
			xbool		matchParameterSeparator(s32 pos) const;
			xbool		matchParameterValue(s32 pos, s32& ioOffset) const;

			typedef		xbool (xparser::*matchDelegate)(s32) const;

			s32			advanceWhile(s32 pos, matchDelegate matcher) const;
			s32			advanceWhileNot(s32 pos, matchDelegate matcher) const;

			xbool		matchAllNotOf(s32 pos, char* cc) const;
			xbool		matchParameterNameChar(s32 pos) const;
			xbool		matchParameterValueChar(s32 pos) const;
			xbool		matchParameterValueFirstChar(s32 pos) const;
			xbool		match(s32 pos, char c) const;
			xbool		matchSlash(s32 pos) const;
			xbool		matchMinus(s32 pos) const;
			xbool		matchQuestion(s32 pos) const;
			xbool		matchColon(s32 pos) const;
			xbool		matchSpace(s32 pos) const;
			xbool		matchQuote(s32 pos) const;
			xbool		matchDoubleQuote(s32 pos) const;

			s32			mParametersStringLen;
			const char*	mParametersString;
		};
		

		xbool	xparser::parse(const char* parameters_string, xparameters& outParams)
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

		xbool	xparser::matchParameters(s32 pos, s32 offset, xparameters& params) const
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
		xbool		xparser::matchParameter(s32 pos, s32&	ioOffset, xparameters& params, int overload) const
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

		xbool	xparser::matchParameter(s32 pos, s32& ioOffset, xparameters& params) const
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

		xbool	matchBoolean(const char* string, s32 length)
		{
			xbool result = false;
			char* tempBoolStr = (char*)xcmdline::Opt_Allocator::get_opt_allocator()->allocate(length*sizeof(char)+1,4);
			x_strcpy(tempBoolStr,length+1,string);
			if(x_strcmp("true",tempBoolStr)==0	||	x_strcmp("false",tempBoolStr)==0)
				result = true;
			xcmdline::Opt_Allocator::get_opt_allocator()->deallocate(tempBoolStr);
			return result;
		}

		xbool matchFloatNumber(const char* string,	s32	stringLen)
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

		xbool matchInteger(const char* string, s32 stringLen)
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
							else if (xcmdline::matchBoolean(params.mValue.mStr,params.mValue.mLen))
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


	char* cutString(const char* original, s32 len)
	{
		ASSERT(original!=NULL);
		ASSERT(len>0);

		char* back = NULL;
		back = (char*)xcmdline::Opt_Allocator::get_opt_allocator()->allocate(len*sizeof(char)+1,4);

#ifdef	OUTDEBUG
		cout << "original string : " << original << "   length: "<< len << endl;
#endif

		x_strcpy(back,len+1,original);

#ifdef	OUTDEBUG
		cout << "cut string : " << back << endl;
#endif
		return back;
	}

	void			x_cmdline::setRegList(s32 regNum)
	{
		if(mRegList	==	NULL)
		{
			mRegList = (s32*)xcmdline::Opt_Allocator::get_opt_allocator()->allocate((mRegListNum+1)*sizeof(s32),4);
		}
		else 
		{
			mRegList = (s32*)xcmdline::Opt_Allocator::get_opt_allocator()->reallocate(mRegList,(1+mRegListNum)*sizeof(s32),4);
		}
		mRegList[mRegListNum] = regNum;
		mRegListNum++;
	}

	void			x_cmdline::argvClear()
	{
		for (s32 i = 1; i < mArgc; i++)
		{
			xcmdline::Opt_Allocator::get_opt_allocator()->deallocate(*(mArgv + i));
		}

		mArgc = 0;
	}

	void			x_cmdline::changeParamToArgv()
	{
		ASSERT(mParameter);
		ASSERT(mParameter->mParams);


		/*The program name, the same with argv[0] in main()*/
		mArgv[0]	=	"xcmdline";
		mArgc++;

		xcmdline::xparam* tempParam	=	mParameter->mParams;
 		while(tempParam)
		{
			char* argvString	=	cutString(tempParam->mKey.mStr,tempParam->mKey.mLen);
			char* argvValue	=	cutString(tempParam->mValue.mStr,tempParam->mValue.mLen);

			switch (tempParam->mValueType)
			{
			case	xcmdline::xparam::TYPE_INT:
			case	xcmdline::xparam::TYPE_FLOAT:
			case	xcmdline::xparam::TYPE_STRING:
			case	xcmdline::xparam::TYPE_CHAR:

				if (mArgc >= mTotalAllocArgvNum)
				{
					mTotalAllocArgvNum	+=	mAllocArgvBlockSize;
					mArgv	=	(char**)xcmdline::Opt_Allocator::get_opt_allocator()->reallocate(mArgv,mTotalAllocArgvNum*sizeof(char*),4);
				}			
				if(tempParam->mKey.mLen == 1)
				{
					mArgv[mArgc]	=	(char*)xcmdline::Opt_Allocator::get_opt_allocator()->allocate(tempParam->mKey.mLen*sizeof(char)+2,4);
					mArgv[mArgc][0]	=	'-';
					x_strcpy(&mArgv[mArgc][1], tempParam->mKey.mLen + 1, argvString);
					mArgc++;
				}
				else
				{
					mArgv[mArgc]	=	(char*)xcmdline::Opt_Allocator::get_opt_allocator()->allocate(tempParam->mKey.mLen*sizeof(char)+3,4);
					mArgv[mArgc][0]	=	'-';
					mArgv[mArgc][1]	=	'-';
					x_strcpy(&mArgv[mArgc][2], tempParam->mKey.mLen + 2, argvString);
					mArgc++;
				}

				mArgv[mArgc]	=	(char*)xcmdline::Opt_Allocator::get_opt_allocator()->allocate(tempParam->mValue.mLen*sizeof(char)+1,4);
				x_strcpy(mArgv[mArgc], tempParam->mValue.mLen + 1, argvValue);
				mArgc++;

				break;
				
			case	xcmdline::xparam::TYPE_BOOL:

				mTotalAllocArgvNum	+=	1;
				mArgv	=	(char**)xcmdline::Opt_Allocator::get_opt_allocator()->reallocate(mArgv,mTotalAllocArgvNum*sizeof(char*),4);

				if(tempParam->mKey.mLen == 1)
				{
					mArgv[mArgc]	=	(char*)xcmdline::Opt_Allocator::get_opt_allocator()->allocate(
						                    (tempParam->mKey.mLen + tempParam->mValue.mLen)*sizeof(char)+2,4);
					mArgv[mArgc][0]	=	'-';
					x_strcpy(&mArgv[mArgc][1], tempParam->mKey.mLen + 1, argvString);

					if (JudgeTrueOrFalse(argvValue))
					{
						x_strcpy(&mArgv[mArgc][tempParam->mKey.mLen + 1], 2, "1");
					}
					else if (!JudgeTrueOrFalse(argvValue))
					{
						x_strcpy(&mArgv[mArgc][tempParam->mKey.mLen + 1], 2, "0");
					}
					mArgc++;
				}
				else
				{
					mArgv[mArgc]	=	(char*)xcmdline::Opt_Allocator::get_opt_allocator()->allocate(
											(tempParam->mKey.mLen + tempParam->mValue.mLen)*sizeof(char)+4,4);
					mArgv[mArgc][0]	=	'-';
					mArgv[mArgc][1]	=	'-';
					x_strcpy(&mArgv[mArgc][2], tempParam->mKey.mLen + 2, argvString);
					mArgv[mArgc][tempParam->mKey.mLen + 2] = '=';

					if (JudgeTrueOrFalse(argvValue))
					{
						x_strcpy(&mArgv[mArgc][tempParam->mKey.mLen + 3], 2, "1");
					}
					else if (!JudgeTrueOrFalse(argvValue))
					{
						x_strcpy(&mArgv[mArgc][tempParam->mKey.mLen + 3], 2, "0");
					}
					mArgc++;
				}

				break;

			case xcmdline::xparam::TYPE_OTHER:
				/*?X : get X's helpful information, X is a shortname of a registered variable*/
				if (tempParam->mKey.mStr[0]		==	'?')
				{
					mTotalAllocArgvNum	+=	1;
					mArgv	=	(char**)xcmdline::Opt_Allocator::get_opt_allocator()->reallocate(mArgv,mTotalAllocArgvNum*sizeof(char*),4);

					mArgv[mArgc]	=	(char*)xcmdline::Opt_Allocator::get_opt_allocator()->allocate(tempParam->mKey.mLen*sizeof(char)+1,4);
					x_strcpy(mArgv[mArgc], tempParam->mKey.mLen+1, argvString);
					mArgc++;
				}
				/*--version: show the version number of opt*/
				else if (x_strcmp("version",argvString)	==	0	||
							x_strcmp("help",argvString)	==	0	||
							x_strcmp("optVersion",argvString)	==	0)
				{
					mTotalAllocArgvNum	+=	1;
					mArgv	=	(char**)xcmdline::Opt_Allocator::get_opt_allocator()->reallocate(mArgv,mTotalAllocArgvNum*sizeof(char*),4);

					mArgv[mArgc]	=	(char*)xcmdline::Opt_Allocator::get_opt_allocator()->allocate(tempParam->mKey.mLen*sizeof(char)+3,4);
					mArgv[mArgc][0]	=	'-';
					mArgv[mArgc][1]	=	'-';
					x_strcpy(&mArgv[mArgc][2], tempParam->mKey.mLen + 2, argvString);
					mArgc++;
				}
				break;
			default:
				xcmdline::Opt_Util::opt_fatal("regAndSetValue: Undefined type");
				break;

			}

			tempParam	=	tempParam->mNext;
			xcmdline::Opt_Allocator::get_opt_allocator()->deallocate(argvString);
			xcmdline::Opt_Allocator::get_opt_allocator()->deallocate(argvValue);
		}
	}

	xbool			x_cmdline::regAndSetValue()

	{
		if (mParameter == NULL || mParameter->mParams == NULL)
		{
			return false;
		}

		xcmdline::xparam* tempParams = mParameter->mParams;

		while(tempParams)
		{
			char*	getKey	=	cutString(tempParams->mKey.mStr,tempParams->mKey.mLen);
			s32 num = -1;

			if (xcmdline::Opt_Reg::getNumReg() > 0)
			{
				num = findVariable(getKey);
			}

			/*If the var is not in optlist, reg it*/
			if (num == -1)
			{
				switch (tempParams->mValueType)
				{
				case xcmdline::xparam::TYPE_INT:
					s32* s32Var;
					s32Var = (s32*)xcmdline::Opt_Allocator::get_opt_allocator()->allocate(sizeof(s32),4);
					if (tempParams->mKey.mLen == 1)
					{
						char c = tempParams->mKey.mStr[0];
						s32 regNum = reg_opt(s32Var,c);
						setRegList(regNum);
					}
					else
					{
						s32 regNum = reg_opt(s32Var,getKey);
						setRegList(regNum);
					}

					break;

				case xcmdline::xparam::TYPE_FLOAT:
					f32* f32Var;
					f32Var = (f32*)xcmdline::Opt_Allocator::get_opt_allocator()->allocate(sizeof(f32),4);
					if (tempParams->mKey.mLen == 1)
					{
						char c = tempParams->mKey.mStr[0];
						s32 regNum = reg_opt(f32Var,c);
						setRegList(regNum);
					}
					else
					{
						s32 regNum = reg_opt(f32Var,getKey);
						setRegList(regNum);
					}

					break;

				case xcmdline::xparam::TYPE_BOOL:
					s32* s32BoolVar;
					s32BoolVar = (s32*)xcmdline::Opt_Allocator::get_opt_allocator()->allocate(sizeof(s32),4);
					if (tempParams->mKey.mLen == 1)
					{
						char c = tempParams->mKey.mStr[0];
						s32 regNum = reg_opt(s32BoolVar,OPT_BOOL,c);
						setRegList(regNum);
					}
					else
					{
						s32 regNum = reg_opt(s32BoolVar,OPT_BOOL,getKey);
						setRegList(regNum);
					}

					break;

				case xcmdline::xparam::TYPE_STRING:
					/*we use a string array to store the string we registered*/
					if(mStringListNum == 0)
					{
						mStringList = (char**)xcmdline::Opt_Allocator::get_opt_allocator()->allocate(sizeof(char*),4);
						mStringList[mStringListNum] = NULL;
						mStringListNum++;
					}
					else
					{
						/*here may be obscure
							the array is dynamic allocate size to adapt the increase number of registered string
							because the optlist.value stores the address of the string array element
							when the string array reallocate, the address of the array will be changed
							so we must adjust the optlist.value to be the same as the array
						*/
						mStringList = (char**)xcmdline::Opt_Allocator::get_opt_allocator()->reallocate(mStringList,(mStringListNum+1)*sizeof(char*),4);
						mStringList[mStringListNum] = NULL;
						mStringListNum++;
						int i = 0;
						for (s32 j=0;j<mRegListNum;j++)
						{
							if(xcmdline::Opt_Reg::optlist[mRegList[j]].type==OPT_STRING)
							{
								xcmdline::Opt_Reg::optlist[mRegList[j]].value = &mStringList[i];
								i++;							
							}
						}							
					}

					if (tempParams->mKey.mLen == 1)
					{
						char c = tempParams->mKey.mStr[0];
						s32 regNum = reg_opt(&mStringList[mStringListNum-1],c);
						setRegList(regNum);
					}
					else
					{
						s32 regNum = reg_opt(&mStringList[mStringListNum-1],getKey);
						setRegList(regNum);
					}
					break;

				case xcmdline::xparam::TYPE_CHAR:
					char* charVar;
					charVar = (char*)xcmdline::Opt_Allocator::get_opt_allocator()->allocate(sizeof(char),4);
					if (tempParams->mKey.mLen == 1)
					{
						char c = tempParams->mKey.mStr[0];
						s32 regNum = reg_opt(charVar,c);
						setRegList(regNum);
					}
					else
					{
						s32 regNum = reg_opt(charVar,getKey);
						setRegList(regNum);
					}

					break;

				case xcmdline::xparam::TYPE_OTHER:
					break;

				default:
					xcmdline::Opt_Util::opt_fatal("regAndSetValue: Undefined type");
					break;
				}
			}

			xcmdline::Opt_Allocator::get_opt_allocator()->deallocate(getKey);
			tempParams = tempParams->mNext;
		}

		s32 argc = mArgc;

		xcmdline::Opt_Proc::opt(&argc,&mArgv);


		mParameter->clear();
		argvClear();

		//printf("%s    %s    %s \n", mArgv[1],mArgv[2],mArgv[3]);

		return true;
	}

	xbool			x_cmdline::parse(const char* cmdline)
	{
		xcmdline::xparser parser;
		xbool res = parser.parse(cmdline, *mParameter);

		if (res)
		{
			changeParamToArgv();
		}

		/*now cmdline are correctly parsed and stored in the params
		* what we should do now is set the values according the identifier
		*/

		return res;
	}


	s32			x_cmdline::findVariable(char* variableName)
	{
		ASSERT(variableName);
		ASSERT(xcmdline::Opt_Reg::getNumReg()>0);

		int len = x_strlen(variableName);
		int num = xcmdline::Opt_Reg::getNumReg()-1;
		while(num	>=	0)
		{
			/*len==1, so it's only a character*/
			if(len	==	1)
			{
				/*here we first assume that the optlist.longname's length longer than 1*/
				if(xcmdline::Opt_Reg::optlist[num].name	!=	NULL	&&	variableName[0]	==	xcmdline::Opt_Reg::optlist[num].name)
				{
					return	num;
				}
				/*if names are incompatible, then maybe the longnames are the same and the length is 1*/
				else if(xcmdline::Opt_Reg::optlist[num].longname	!=	NULL	&&	x_strcmp(variableName,	xcmdline::Opt_Reg::optlist[num].longname)	==	0)
				{
					return num;
				}
			}//end if
			else
			{
				if(xcmdline::Opt_Reg::optlist[num].longname	!=	NULL	&&	x_strcmp(variableName, xcmdline::Opt_Reg::optlist[num].longname)	==	0)
					return num;
			}//end else
			num--;
		}//end while

		/*error return*/
		return	-1;
	}



	void			x_cmdline::setValue(s32		optNum,	char*	optValue)
	{
#ifdef	OUTDEBUG
		cout << "optValue   " << optValue << endl << "optNum   " << optNum << endl;
		cout << "oldValue  " << xcmdline::Opt_Proc::optstrval(optNum) << endl;
#endif
		if (xcmdline::Opt_Reg::optlist[optNum].type	==	OPT_BOOL)
		{
			xbool*	xptr;
			xptr	=	(xbool*)xcmdline::Opt_Reg::optlist[optNum].value;
			if(JudgeTrueOrFalse(optValue))
			{
				*xptr	=	(xbool)true;
			}
			else
			{
				*xptr	=	(xbool)false;
			}
			return;
		}

		if (xcmdline::Opt_Reg::optlist[optNum].value	&&	xcmdline::Opt_Reg::optlist[optNum].type	==	OPT_STRING)
		{
			xcmdline::Opt_Allocator::get_opt_allocator()->deallocate(*((char **)(xcmdline::Opt_Reg::optlist[optNum].value)));
		}

		xcmdline::Opt_Reg::opt_setvalue(xcmdline::Opt_Reg::optlist[optNum].value,xcmdline::Opt_Reg::optlist[optNum].type,optValue);

#ifdef	OUTDEBUG
		cout	<< "newValue  " << xcmdline::Opt_Proc::optstrval(optNum) << endl;
#endif
	}

	void			x_cmdline::setValueForOpt(xcmdline::xparameters* parameters)
	{
		ASSERT(parameters);
		ASSERT(parameters->mParams);

		xcmdline::xparam* tempParameters = parameters->mParams;

		while(tempParameters)
		{
			char*	getKey	=	cutString(tempParameters->mKey.mStr,tempParameters->mKey.mLen);
			char*	getValue	=	cutString(tempParameters->mValue.mStr,tempParameters->mValue.mLen);
			s32		optNum	=	findVariable(getKey);	

			if(optNum	>=	0)
				setValue(optNum,	getValue);

			xcmdline::Opt_Allocator::get_opt_allocator()->deallocate(getKey);
			xcmdline::Opt_Allocator::get_opt_allocator()->deallocate(getValue);

			tempParameters = tempParameters->mNext;
		}
	}

	xbool			x_cmdline::parse(s32 argc, const char** argv)
	{
		if (**argv == NULL)
		{
			return false;
		}
		s32 len = 0;

		for (s32 i = 0; i < argc; i++)
		{
			len += x_strlen(*(argv + i));
		}

		mNewCmdline = (char*) xcmdline::Opt_Allocator::get_opt_allocator()->allocate( len * sizeof(char) + argc, 4);

		len = 0;

		for (s32 i = 0;	i < argc; i++)
		{
			x_strcpy(mNewCmdline + len, x_strlen(*(argv + i)) + 1,*(argv + i));
			len += x_strlen(*(argv + i));
			mNewCmdline[len] = ' ';
			len += 1;
		}

		mNewCmdline[len] = '\0';

		parse(mNewCmdline);

		return true;
	}

	void*			x_cmdline::getValue(char* name)
	{
		s32 num = findVariable(name);
		ASSERT(num != -1);
		return xcmdline::Opt_Reg::optlist[num].value;
	}

	s32 x_cmdline::reg_opt(char *v, char c, char *n, char *B)
	{
		return xcmdline::Opt_Reg::optrega(v,xcmdline::OPT_CHAR,c,n,B);
	}

	s32 x_cmdline::reg_opt(char *v, char *n, char *B)
	{
		return xcmdline::Opt_Reg::optregsb(v,xcmdline::OPT_CHAR,n,B);
	}

	s32 x_cmdline::reg_opt(char *v, EOptType o, char c, char *n, char *B)
	{
		return xcmdline::Opt_Reg::optrega(v,(xcmdline::opt_TYPE)o,c,n,B);
	}

	s32 x_cmdline::reg_opt(char *v, EOptType o, char *n, char *B)
	{
		return xcmdline::Opt_Reg::optregsb(v,(xcmdline::opt_TYPE)o,n,B);
	}

	s32	x_cmdline::reg_opt(char *v, EOptMode m, char c, char *n, char *B)
	{
		s32 i = xcmdline::Opt_Reg::optrega(v,xcmdline::OPT_CHAR,c,n,B);
		xcmdline::Opt_Reg::optmode_n(i,(xcmdline::opt_MODE)m);
		return i ;
	}

	s32 x_cmdline::reg_opt(char *v, EOptMode m, char *n, char *B)
	{
		s32 i = xcmdline::Opt_Reg::optregsb(v,xcmdline::OPT_CHAR,n,B);
		xcmdline::Opt_Reg::optmode_n(i,(xcmdline::opt_MODE)m);
		return i ;
	}

	s32 x_cmdline::reg_opt(char *v, EOptType o, EOptMode m, char c, char *n, char *B)
	{
		s32 i = xcmdline::Opt_Reg::optrega(v,(xcmdline::opt_TYPE)o,c,n,B);
		xcmdline::Opt_Reg::optmode_n(i,(xcmdline::opt_MODE)m);
		return i ;
	}

	s32 x_cmdline::reg_opt(char *v, EOptType o, EOptMode m, char *n, char *B)
	{
		s32 i = xcmdline::Opt_Reg::optregsb(v,(xcmdline::opt_TYPE)o,n,B);
		xcmdline::Opt_Reg::optmode_n(i,(xcmdline::opt_MODE)m);
		return i ;
	}

	s32 x_cmdline::reg_opt(s16 *v, char c, char *n, char *B)
	{
		return xcmdline::Opt_Reg::optrega(v,xcmdline::OPT_SHORT,c,n,B);
	}

	s32	x_cmdline::reg_opt(s16 *v, char *n, char *B)
	{
		return xcmdline::Opt_Reg::optregsb(v,xcmdline::OPT_SHORT,n,B);
	}

	s32	x_cmdline::reg_opt(s16 *v, EOptType o, char c, char *n, char *B)
	{
		return xcmdline::Opt_Reg::optrega(v,(xcmdline::opt_TYPE)o,c,n,B);
	}

	s32	x_cmdline::reg_opt(s16 *v, EOptType o, char *n, char *B)
	{
		return xcmdline::Opt_Reg::optregsb(v,(xcmdline::opt_TYPE)o,n,B);
	}

	s32	x_cmdline::reg_opt(s16 *v, EOptMode m, char c, char *n, char *B)
	{
		s32 i = xcmdline::Opt_Reg::optrega(v,xcmdline::OPT_SHORT,c,n,B);
		xcmdline::Opt_Reg::optmode_n(i,(xcmdline::opt_MODE)m);
		return i ;
	}

	s32	x_cmdline::reg_opt(s16 *v, EOptMode m, char *n, char *B)
	{
		s32 i = xcmdline::Opt_Reg::optregsb(v,xcmdline::OPT_SHORT,n,B);
		xcmdline::Opt_Reg::optmode_n(i,(xcmdline::opt_MODE)m);
		return i ;
	}

	s32	x_cmdline::reg_opt(s16 *v, EOptType o, EOptMode m, char c, char *n, char *B)
	{
		s32 i = xcmdline::Opt_Reg::optrega(v,(xcmdline::opt_TYPE)o,c,n,B);
		xcmdline::Opt_Reg::optmode_n(i,(xcmdline::opt_MODE)m);
		return i ;
	}

	s32	x_cmdline::reg_opt(s16 *v, EOptType o, EOptMode m, char *n, char *B)
	{
		s32 i = xcmdline::Opt_Reg::optregsb(v,(xcmdline::opt_TYPE)o,n,B);
		xcmdline::Opt_Reg::optmode_n(i,(xcmdline::opt_MODE)m);
		return i ;
	}

	s32	x_cmdline::reg_opt(u16 *v, char c, char *n, char *B)
	{
		return xcmdline::Opt_Reg::optrega(v,xcmdline::OPT_USHORT,c,n,B);
	}

	s32	x_cmdline::reg_opt(u16 *v, char *n, char *B)
	{
		return xcmdline::Opt_Reg::optregsb(v,xcmdline::OPT_USHORT,n,B);
	}

	s32	x_cmdline::reg_opt(u16 *v, EOptType o, char c, char *n, char *B)
	{
		return xcmdline::Opt_Reg::optrega(v,(xcmdline::opt_TYPE)o,c,n,B);
	}

	s32 x_cmdline::reg_opt(u16 *v, EOptType o, char *n, char *B)
	{
		return xcmdline::Opt_Reg::optregsb(v,(xcmdline::opt_TYPE)o,n,B);
	}

	s32	x_cmdline::reg_opt(u16 *v, EOptMode m, char c, char *n, char *B)
	{
		s32 i = xcmdline::Opt_Reg::optrega(v,xcmdline::OPT_USHORT,c,n,B);
		xcmdline::Opt_Reg::optmode_n(i,(xcmdline::opt_MODE)m);
		return i ;
	}

	s32	x_cmdline::reg_opt(u16 *v, EOptMode m, char *n, char *B)
	{
		s32 i = xcmdline::Opt_Reg::optregsb(v,xcmdline::OPT_USHORT,n,B);
		xcmdline::Opt_Reg::optmode_n(i,(xcmdline::opt_MODE)m);
		return i ;
	}

	s32	x_cmdline::reg_opt(u16 *v, EOptType o, EOptMode m, char c, char *n, char *B)
	{
		s32 i = xcmdline::Opt_Reg::optrega(v,(xcmdline::opt_TYPE)o,c,n,B);
		xcmdline::Opt_Reg::optmode_n(i,(xcmdline::opt_MODE)m);
		return i ;
	}

	s32	x_cmdline::reg_opt(u16 *v, EOptType o, EOptMode m, char *n, char *B)
	{
		s32 i = xcmdline::Opt_Reg::optregsb(v,(xcmdline::opt_TYPE)o,n,B);
		xcmdline::Opt_Reg::optmode_n(i,(xcmdline::opt_MODE)m);
		return i ;
	}

	s32	x_cmdline::reg_opt(s32 *v, char c, char *n, char *B)
	{
		return xcmdline::Opt_Reg::optrega(v,xcmdline::OPT_INT,c,n,B);
	}

	s32	x_cmdline::reg_opt(s32 *v, char *n, char *B)
	{
		return xcmdline::Opt_Reg::optregsb(v,xcmdline::OPT_INT,n,B);
	}

	s32	x_cmdline::reg_opt(s32 *v, EOptType o, char c, char *n, char *B)
	{
		return xcmdline::Opt_Reg::optrega(v,(xcmdline::opt_TYPE)o,c,n,B);
	}

	s32	x_cmdline::reg_opt(s32 *v, EOptType o, char *n, char *B)
	{
		return xcmdline::Opt_Reg::optregsb(v,(xcmdline::opt_TYPE)o,n,B);
	}

	s32	x_cmdline::reg_opt(s32 *v, EOptMode m, char c, char *n, char *B)
	{
		s32 i = xcmdline::Opt_Reg::optrega(v,xcmdline::OPT_INT,c,n,B);
		xcmdline::Opt_Reg::optmode_n(i,(xcmdline::opt_MODE)m);
		return i ;
	}

	s32	x_cmdline::reg_opt(s32 *v, EOptMode m, char *n, char *B)
	{
		s32 i = xcmdline::Opt_Reg::optregsb(v,xcmdline::OPT_INT,n,B);
		xcmdline::Opt_Reg::optmode_n(i,(xcmdline::opt_MODE)m);
		return i ;
	}

	s32	x_cmdline::reg_opt(s32 *v, EOptType o, EOptMode m, char c, char *n, char *B)
	{
		s32 i = xcmdline::Opt_Reg::optrega(v,(xcmdline::opt_TYPE)o,c,n,B);
		xcmdline::Opt_Reg::optmode_n(i,(xcmdline::opt_MODE)m);
		return i ;
	}

	s32	x_cmdline::reg_opt(s32 *v, EOptType o, EOptMode m, char *n, char *B)
	{
		s32 i = xcmdline::Opt_Reg::optregsb(v,(xcmdline::opt_TYPE)o,n,B);
		xcmdline::Opt_Reg::optmode_n(i,(xcmdline::opt_MODE)m);
		return i ;
	}

	s32	x_cmdline::reg_opt(u32 *v, char c, char *n, char *B)
	{
		return xcmdline::Opt_Reg::optrega(v,xcmdline::OPT_UINT,c,n,B);
	}

	s32	x_cmdline::reg_opt(u32 *v, char *n, char *B)
	{
		return xcmdline::Opt_Reg::optregsb(v,xcmdline::OPT_UINT,n,B);
	}

	s32	x_cmdline::reg_opt(u32 *v, EOptType o, char c, char *n, char *B)
	{
		return xcmdline::Opt_Reg::optrega(v,(xcmdline::opt_TYPE)o,c,n,B);
	}

	s32 x_cmdline::reg_opt(u32 *v, EOptType o, char *n, char *B)
	{
		return xcmdline::Opt_Reg::optregsb(v,(xcmdline::opt_TYPE)o,n,B);
	}

	s32	x_cmdline::reg_opt(u32 *v, EOptMode m, char c, char *n, char *B)
	{
		s32 i = xcmdline::Opt_Reg::optrega(v,xcmdline::OPT_UINT,c,n,B);
		xcmdline::Opt_Reg::optmode_n(i,(xcmdline::opt_MODE)m);
		return i ;
	}

	s32	x_cmdline::reg_opt(u32 *v, EOptMode m, char *n, char *B)
	{
		s32 i = xcmdline::Opt_Reg::optregsb(v,xcmdline::OPT_UINT,n,B);
		xcmdline::Opt_Reg::optmode_n(i,(xcmdline::opt_MODE)m);
		return i ;
	}

	s32	x_cmdline::reg_opt(u32 *v, EOptType o, EOptMode m, char c, char *n, char *B)
	{
		s32 i = xcmdline::Opt_Reg::optrega(v,(xcmdline::opt_TYPE)o,c,n,B);
		xcmdline::Opt_Reg::optmode_n(i,(xcmdline::opt_MODE)m);
		return i ;
	}

	s32	x_cmdline::reg_opt(u32 *v, EOptType o, EOptMode m, char *n, char *B)
	{
		s32 i = xcmdline::Opt_Reg::optregsb(v,(xcmdline::opt_TYPE)o,n,B);
		xcmdline::Opt_Reg::optmode_n(i,(xcmdline::opt_MODE)m);
		return i ;
	}

	s32	x_cmdline::reg_opt(f64 *v, char c, char *n, char *B)
	{
		return xcmdline::Opt_Reg::optrega(v,xcmdline::OPT_DOUBLE,c,n,B);
	}

	s32	x_cmdline::reg_opt(f64 *v, char *n, char *B)
	{
		return xcmdline::Opt_Reg::optregsb(v,xcmdline::OPT_DOUBLE,n,B);
	}

	s32	x_cmdline::reg_opt(f64 *v, EOptType o, char c, char *n, char *B)
	{
		return xcmdline::Opt_Reg::optrega(v,(xcmdline::opt_TYPE)o,c,n,B);
	}

	s32	x_cmdline::reg_opt(f64 *v, EOptType o, char *n, char *B)
	{
		return xcmdline::Opt_Reg::optregsb(v,(xcmdline::opt_TYPE)o,n,B);
	}

	s32	x_cmdline::reg_opt(f64 *v, EOptMode m, char c, char *n, char *B)
	{
		s32 i = xcmdline::Opt_Reg::optrega(v,xcmdline::OPT_DOUBLE,c,n,B);
		xcmdline::Opt_Reg::optmode_n(i,(xcmdline::opt_MODE)m);
		return i ;
	}

	s32	x_cmdline::reg_opt(f64 *v, EOptMode m, char *n, char *B)
	{
		s32 i = xcmdline::Opt_Reg::optregsb(v,xcmdline::OPT_DOUBLE,n,B);
		xcmdline::Opt_Reg::optmode_n(i,(xcmdline::opt_MODE)m);
		return i ;
	}

	s32	x_cmdline::reg_opt(f64 *v, EOptType o, EOptMode m, char c, char *n, char *B)
	{
		s32 i = xcmdline::Opt_Reg::optrega(v,(xcmdline::opt_TYPE)o,c,n,B);
		xcmdline::Opt_Reg::optmode_n(i,(xcmdline::opt_MODE)m);
		return i ;
	}

	s32	x_cmdline::reg_opt(f64 *v, EOptType o, EOptMode m, char *n, char *B)
	{
		s32 i = xcmdline::Opt_Reg::optregsb(v,(xcmdline::opt_TYPE)o,n,B);
		xcmdline::Opt_Reg::optmode_n(i,(xcmdline::opt_MODE)m);
		return i ;
	}

	s32	x_cmdline::reg_opt(s64 *v, char c, char *n, char *B)
	{
		return xcmdline::Opt_Reg::optrega(v,xcmdline::OPT_LONG,c,n,B);
	}

	s32	x_cmdline::reg_opt(s64 *v, char *n, char *B)
	{
		return xcmdline::Opt_Reg::optregsb(v,xcmdline::OPT_LONG,n,B);
	}

	s32	x_cmdline::reg_opt(s64 *v, EOptType o, char c, char *n, char *B)
	{
		return xcmdline::Opt_Reg::optrega(v,(xcmdline::opt_TYPE)o,c,n,B);
	}

	s32	x_cmdline::reg_opt(s64 *v, EOptType o, char *n, char *B)
	{
		return xcmdline::Opt_Reg::optregsb(v,(xcmdline::opt_TYPE)o,n,B);
	}

	s32	x_cmdline::reg_opt(s64 *v, EOptMode m, char c, char *n, char *B)
	{
		s32 i = xcmdline::Opt_Reg::optrega(v,xcmdline::OPT_LONG,c,n,B);
		xcmdline::Opt_Reg::optmode_n(i,(xcmdline::opt_MODE)m);
		return i ;
	}

	s32	x_cmdline::reg_opt(s64 *v, EOptMode m, char *n, char *B)
	{
		s32 i = xcmdline::Opt_Reg::optregsb(v,xcmdline::OPT_LONG,n,B);
		xcmdline::Opt_Reg::optmode_n(i,(xcmdline::opt_MODE)m);
		return i ;
	}

	s32	x_cmdline::reg_opt(s64 *v, EOptType o, EOptMode m, char c, char *n, char *B)
	{
		s32 i = xcmdline::Opt_Reg::optrega(v,(xcmdline::opt_TYPE)o,c,n,B);
		xcmdline::Opt_Reg::optmode_n(i,(xcmdline::opt_MODE)m);
		return i ;
	}

	s32	x_cmdline::reg_opt(s64 *v, EOptType o, EOptMode m, char *n, char *B)
	{
		s32 i = xcmdline::Opt_Reg::optregsb(v,(xcmdline::opt_TYPE)o,n,B);
		xcmdline::Opt_Reg::optmode_n(i,(xcmdline::opt_MODE)m);
		return i ;
	}

	s32	x_cmdline::reg_opt(u64 *v, char c, char *n, char *B)
	{
		return xcmdline::Opt_Reg::optrega(v,xcmdline::OPT_ULONG,c,n,B);
	}

	s32	x_cmdline::reg_opt(u64 *v, char *n, char *B)
	{
		return xcmdline::Opt_Reg::optregsb(v,xcmdline::OPT_ULONG,n,B);
	}

	s32	x_cmdline::reg_opt(u64 *v, EOptType o, char c, char *n, char *B)
	{
		return xcmdline::Opt_Reg::optrega(v,(xcmdline::opt_TYPE)o,c,n,B);
	}

	s32	x_cmdline::reg_opt(u64 *v, EOptType o, char *n, char *B)
	{
		return xcmdline::Opt_Reg::optregsb(v,(xcmdline::opt_TYPE)o,n,B);
	}

	s32	x_cmdline::reg_opt(u64 *v, EOptMode m, char c, char *n, char *B)
	{
		s32 i = xcmdline::Opt_Reg::optrega(v,xcmdline::OPT_ULONG,c,n,B);
		xcmdline::Opt_Reg::optmode_n(i,(xcmdline::opt_MODE)m);
		return i ;
	}

	s32	x_cmdline::reg_opt(u64 *v, EOptMode m, char *n, char *B)
	{
		s32 i = xcmdline::Opt_Reg::optregsb(v,xcmdline::OPT_ULONG,n,B);
		xcmdline::Opt_Reg::optmode_n(i,(xcmdline::opt_MODE)m);
		return i ;
	}

	s32	x_cmdline::reg_opt(u64 *v, EOptType o, EOptMode m, char c, char *n, char *B)
	{
		s32 i = xcmdline::Opt_Reg::optrega(v,(xcmdline::opt_TYPE)o,c,n,B);
		xcmdline::Opt_Reg::optmode_n(i,(xcmdline::opt_MODE)m);
		return i ;
	}

	s32	x_cmdline::reg_opt(u64 *v, EOptType o, EOptMode m, char *n, char *B)
	{
		s32 i = xcmdline::Opt_Reg::optregsb(v,(xcmdline::opt_TYPE)o,n,B);
		xcmdline::Opt_Reg::optmode_n(i,(xcmdline::opt_MODE)m);
		return i ;
	}

	s32	x_cmdline::reg_opt(char **v, char c, char *n, char *B)
	{
		return xcmdline::Opt_Reg::optrega(v,xcmdline::OPT_STRING,c,n,B);
	}

	s32	x_cmdline::reg_opt(char **v, char *n, char *B)
	{
		return xcmdline::Opt_Reg::optregsb(v,xcmdline::OPT_STRING,n,B);
	}

	s32	x_cmdline::reg_opt(char **v, EOptType o, char c, char *n, char *B)
	{
		return xcmdline::Opt_Reg::optrega(v,(xcmdline::opt_TYPE)o,c,n,B);
	}

	s32	x_cmdline::reg_opt(char **v, EOptType o, char *n, char *B)
	{
		return xcmdline::Opt_Reg::optregsb(v,(xcmdline::opt_TYPE)o,n,B);
	}

	s32	x_cmdline::reg_opt(char **v, EOptMode m, char c, char *n, char *B)
	{
		s32 i = xcmdline::Opt_Reg::optrega(v,xcmdline::OPT_STRING,c,n,B);
		xcmdline::Opt_Reg::optmode_n(i,(xcmdline::opt_MODE)m);
		return i ;
	}

	s32	x_cmdline::reg_opt(char **v, EOptMode m, char *n, char *B)
	{
		s32 i = xcmdline::Opt_Reg::optregsb(v,xcmdline::OPT_STRING,n,B);
		xcmdline::Opt_Reg::optmode_n(i,(xcmdline::opt_MODE)m);
		return i ;
	}

	s32	x_cmdline::reg_opt(char **v, EOptType o, EOptMode m, char c, char *n, char *B)
	{
		s32 i = xcmdline::Opt_Reg::optrega(v,(xcmdline::opt_TYPE)o,c,n,B);
		xcmdline::Opt_Reg::optmode_n(i,(xcmdline::opt_MODE)m);
		return i ;
	}

	s32	x_cmdline::reg_opt(char **v, EOptType o, EOptMode m, char *n, char *B)
	{
		s32 i = xcmdline::Opt_Reg::optregsb(v,(xcmdline::opt_TYPE)o,n,B);
		xcmdline::Opt_Reg::optmode_n(i,(xcmdline::opt_MODE)m);
		return i ;
	}

	s32	x_cmdline::reg_opt(f32 *v, char c, char *n, char *B)
	{
		return xcmdline::Opt_Reg::optrega(v,xcmdline::OPT_FLOAT,c,n,B);
	}

	s32	x_cmdline::reg_opt(f32 *v, char *n, char *B)
	{
		return xcmdline::Opt_Reg::optregsb(v,xcmdline::OPT_FLOAT,n,B);
	}

	s32	x_cmdline::reg_opt(f32 *v, EOptType o, char c, char *n, char *B)
	{
		return xcmdline::Opt_Reg::optrega(v,(xcmdline::opt_TYPE)o,c,n,B);
	}

	s32	x_cmdline::reg_opt(f32 *v, EOptType o, char *n, char *B)
	{
		return xcmdline::Opt_Reg::optregsb(v,(xcmdline::opt_TYPE)o,n,B);
	}

	s32	x_cmdline::reg_opt(f32 *v, EOptMode m, char c, char *n, char *B)
	{
		s32 i = xcmdline::Opt_Reg::optrega(v,xcmdline::OPT_FLOAT,c,n,B);
		xcmdline::Opt_Reg::optmode_n(i,(xcmdline::opt_MODE)m);
		return i ;
	}

	s32	x_cmdline::reg_opt(f32 *v, EOptMode m, char *n, char *B)
	{
		s32 i = xcmdline::Opt_Reg::optregsb(v,xcmdline::OPT_FLOAT,n,B);
		xcmdline::Opt_Reg::optmode_n(i,(xcmdline::opt_MODE)m);
		return i ;
	}

	s32	x_cmdline::reg_opt(f32 *v, EOptType o, EOptMode m, char c, char *n, char *B)
	{
		s32 i = xcmdline::Opt_Reg::optrega(v,(xcmdline::opt_TYPE)o,c,n,B);
		xcmdline::Opt_Reg::optmode_n(i,(xcmdline::opt_MODE)m);
		return i ;
	}

	s32	x_cmdline::reg_opt(f32 *v, EOptType o, EOptMode m, char *n, char *B)
	{
		s32 i = xcmdline::Opt_Reg::optregsb(v,(xcmdline::opt_TYPE)o,n,B);
		xcmdline::Opt_Reg::optmode_n(i,(xcmdline::opt_MODE)m);
		return i ;
	}

	s32 x_cmdline::reg_opt(unsigned char *v, char c, char *n, char *B)
	{
		return xcmdline::Opt_Reg::optrega(v,xcmdline::OPT_UCHAR,c,n,B);
	}

	s32 x_cmdline::reg_opt(unsigned char *v, char *n, char *B)
	{
		return xcmdline::Opt_Reg::optregsb(v,xcmdline::OPT_UCHAR,n,B);
	}

	s32 x_cmdline::reg_opt(unsigned char *v, EOptType o, char c, char *n, char *B)
	{
		return xcmdline::Opt_Reg::optrega(v,(xcmdline::opt_TYPE)o,c,n,B);
	}

	s32 x_cmdline::reg_opt(unsigned char *v, EOptType o, char *n, char *B)
	{
		return xcmdline::Opt_Reg::optregsb(v,(xcmdline::opt_TYPE)o,n,B);
	}

	s32	x_cmdline::reg_opt(unsigned char *v, EOptMode m, char c, char *n, char *B)
	{
		s32 i = xcmdline::Opt_Reg::optrega(v,xcmdline::OPT_UCHAR,c,n,B);
		xcmdline::Opt_Reg::optmode_n(i,(xcmdline::opt_MODE)m);
		return i ;
	}

	s32 x_cmdline::reg_opt(unsigned char *v, EOptMode m, char *n, char *B)
	{
		s32 i = xcmdline::Opt_Reg::optregsb(v,xcmdline::OPT_UCHAR,n,B);
		xcmdline::Opt_Reg::optmode_n(i,(xcmdline::opt_MODE)m);
		return i ;
	}

	s32 x_cmdline::reg_opt(unsigned char *v, EOptType o, EOptMode m, char c, char *n, char *B)
	{
		s32 i = xcmdline::Opt_Reg::optrega(v,(xcmdline::opt_TYPE)o,c,n,B);
		xcmdline::Opt_Reg::optmode_n(i,(xcmdline::opt_MODE)m);
		return i ;
	}

	s32 x_cmdline::reg_opt(unsigned char *v, EOptType o, EOptMode m, char *n, char *B)
	{
		s32 i = xcmdline::Opt_Reg::optregsb(v,(xcmdline::opt_TYPE)o,n,B);
		xcmdline::Opt_Reg::optmode_n(i,(xcmdline::opt_MODE)m);
		return i ;
	}

	x_cmdline::x_cmdline():mAllocArgvBlockSize(10)
	{ 
		mParameter = (xcmdline::xparameters*)xcmdline::Opt_Allocator::get_opt_allocator()->allocate(sizeof(xcmdline::xparameters),4);
		mParameter->mParams = NULL;
		mParameter->mParametersString = NULL;
		mRegList = NULL;
		mRegListNum = 0;
		mNewCmdline = NULL;
		mArgc = 0;
		mArgv = (char**)xcmdline::Opt_Allocator::get_opt_allocator()->allocate(sizeof(char*),4);
		mStringListNum = 0;
		mStringList = NULL;
		mTotalAllocArgvNum = 1;
	}

	void x_cmdline::clearRegisteredOption()
	{
		for (s32 i=0;i<mRegListNum;i++)
		{
			if (xcmdline::Opt_Reg::optlist[mRegList[i]].value && xcmdline::Opt_Reg::optlist[mRegList[i]].type == OPT_STRING)
			{
				xcmdline::Opt_Allocator::get_opt_allocator()->deallocate(*((char **)(xcmdline::Opt_Reg::optlist[mRegList[i]].value)));
				xcmdline::Opt_Reg::optlist[mRegList[i]].value = NULL;
				continue;
			}
			if(xcmdline::Opt_Reg::optlist[mRegList[i]].value)
			{
				xcmdline::Opt_Allocator::get_opt_allocator()->deallocate(xcmdline::Opt_Reg::optlist[mRegList[i]].value);
				xcmdline::Opt_Reg::optlist[mRegList[i]].value = NULL;
			}
		}
	}

	x_cmdline::~x_cmdline()
	{
		if(mStringList != NULL)
			xcmdline::Opt_Allocator::get_opt_allocator()->deallocate(mStringList);
		xcmdline::Opt_Allocator::get_opt_allocator()->deallocate(mParameter);
		xcmdline::Opt_Allocator::get_opt_allocator()->deallocate(mRegList);
		if (mNewCmdline != NULL)
		{
			xcmdline::Opt_Allocator::get_opt_allocator()->deallocate(mNewCmdline);
		}
		xcmdline::Opt_Allocator::get_opt_allocator()->deallocate(mArgv);
	}
}