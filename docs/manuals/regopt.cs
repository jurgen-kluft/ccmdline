Good commandline analyzer that can act as a auto register layer for getopt().
By analyzing the parameter types we can register them at getopt.

    public class CommandLine
    {
        #region Fields

        private String _ParametersString = null;
        private bool _CaseSensitive = false;
        private bool _Parsed = false;
        private bool _Success = false;
        private int _Offset = -1;
        private String _CurrentName = null;
        private String _CurrentValue = null;

        private string _Command = null;
        private Dictionary<String, String> _Parameters = null;

        #endregion


        #region Properties

        /// <summary>
        /// Gets or sets the parameters string.
        /// </summary>
        /// <value>The parameters string.</value>
        public String ParametersString
        {
            get
            {
                return _ParametersString;
            }
            set
            {
                _ParametersString = value;

                Parse();
            }
        }

        /// <summary>
        /// Gets or sets a value indicating whether parameters name are case sensitive.
        /// </summary>
        /// <value><c>true</c> if parameters name are case sensitive; otherwise, <c>false</c>.</value>
        public bool CaseSensitive
        {
            get
            {
                return _CaseSensitive;
            }
        }

        /// <summary>
        /// Gets a value indicating whether this <see cref="CommandLine"/> has been parsed.
        /// </summary>
        /// <value><c>true</c> if parsed; otherwise, <c>false</c>.</value>
        public bool Parsed
        {
            get
            {
                return _Parsed;
            }
        }

        /// <summary>
        /// Gets a value indicating whether this <see cref="CommandLine"/> has been parsed successfully.
        /// </summary>
        /// <value><c>true</c> if parsed successfully; otherwise, <c>false</c>.</value>
        public bool Success
        {
            get
            {
                return _Success;
            }
        }

        /// <summary>
        /// Gets the offset where the parsing blocked.
        /// </summary>
        /// <value>The error offset.</value>
        public int ErrorOffset
        {
            get
            {
                return _Offset;
            }
        }

        /// <summary>
        /// Gets the parameters dictionary.
        /// </summary>
        /// <value>The parameters dictionary.</value>
        public Dictionary<String, String> Parameters
        {
            get
            {
                if (!_Parsed)
                {
                    Parse();
                }

                return _Parameters;
            }
        }

        /// <summary>
        /// Gets the count of parameters currently parsed.
        /// </summary>
        /// <value>The count.</value>
        public int Count
        {
            get
            {
                return Parameters.Count;
            }
        }

        /// <summary>
        /// Gets the <see cref="System.String"/> parameter value with the specified parameter name.
        /// </summary>
        /// <value></value>
        public String Command
        {
            get
            {
                if (String.IsNullOrEmpty(_Command))
                    return string.Empty;

                return _Command;
            }
        }

        /// <summary>
        /// Gets the <see cref="System.String"/> parameter value with the specified parameter name.
        /// </summary>
        /// <value></value>
        public String this[String parameterName]
        {
            get
            {
                if (!_CaseSensitive)
                {
                    parameterName = parameterName.ToLower();
                }

                Dictionary<String, String> parms = Parameters;

                if (parms.ContainsKey(parameterName))
                {
                    return parms[parameterName];
                }

                return null;
            }
        }

        #endregion


        #region Construction

        public CommandLine()
            : this(false)
        { }

        public CommandLine(bool caseSensitive)
            : this(String.Empty, caseSensitive)
        { }

        public CommandLine(String commandLine)
            : this(commandLine, false)
        { }

        public CommandLine(String[] parameters)
            : this(parameters, false)
        { }

        public CommandLine(String[] parameters, bool caseSensitive)
            : this(String.Join(" ", parameters), caseSensitive)
        { }

        public CommandLine(String commandLine, bool caseSensitive)
        {
            _CaseSensitive = caseSensitive;
            _Parameters = new Dictionary<string, string>();
            ParametersString = commandLine;
        }

        #endregion


        #region Methods

        /// <summary>
        /// Clears the parameters dictionary.
        /// </summary>
        public void Clear()
        {
            _Parameters.Clear();
        }

        /// <summary>
        /// Parses the ParametersString and fill the parameters dictionary.
        /// </summary>
        /// <returns>true if parsing was successful</returns>
        /// <remarks>
        /// 
        /// Grammar
        /// 
        /// parameters : parameter*
        ///            ;
        /// 
        /// parameter : '/' parameter-struct
        ///           | '--' parameter-struct
        ///           | '-' parameter-struct
        ///           ;
        /// 
        /// parameter-struct : parameter-name (parameter-separator parameter-value)? ;
        /// 
        /// parameter-name : parameter-name-char+ ;
        /// 
        /// parameter-separator : ':'
        ///                     | '='
        ///                     | ' '
        ///                     ;
        /// 
        /// parameter-value : '\'' (ANY LESS '\'')* '\''
        ///                 | '"' (ANY LESS '"')* '"'
        ///                 | (parameter-value-first-char parameter-value-char*)?
        ///                 ;
        /// 
        /// parameter-name-char : ANY LESS ' ', ':', '=' ;
        /// 
        /// parameter-value-first-char : ANY LESS ' ', '/', ':', '=' ;
        /// 
        /// parameter-value-char : ANY LESS ' ' ;
        /// 
        /// 
        /// Matches:
        /// 
        /// Application.exe status /new /parm: "A parameter can be '/parm: '/value:''"   /parm2:  value2   /parm3: "value3: 'the value'"
        /// 
        /// </remarks>
        public bool Parse()
        {
            try
            {
                Clear();

                _Offset = 0;
                _Parsed = true;
                _Success = false;

                if (!String.IsNullOrEmpty(_ParametersString))
                {
                    if (MatchParameterValueFirstChar(_Offset))
                    {
                        int pos = AdvanceWhile(_Offset, MatchParameterValueChar);
                        _Command = _ParametersString.Substring(_Offset, pos - _Offset);
                        _Offset = pos;
                    }

                    _Offset = AdvanceWhile(_Offset, MatchSpace);

                    if (MatchParameters(_Offset))
                    {
                        _Success = true;
                    }
                }
                else
                {
                    _Success = true;
                }
            }
            catch (ArgumentOutOfRangeException)
            {
                _Success = false;
            }
            catch (IndexOutOfRangeException)
            {
                _Success = false;
            }

            return _Success;
        }

        private bool MatchParameters(int pos)
        {
            while (pos < _ParametersString.Length && MatchParameter(pos))
            {
                pos = _Offset;
                pos = AdvanceWhile(pos, MatchSpace);
            }

            if (pos == _ParametersString.Length)
            {
                return true;
            }

            return false;
        }

        private bool MatchParameter(int pos)
        {
            if (MatchSlash(pos))
            {
                pos++;
            }
            else if (MatchMinus(pos))
            {
                pos++;

                if (MatchMinus(pos))
                {
                    pos++;
                }
            }
            else
            {
                return false;
            }

            if (MatchParameterStruct(pos))
            {
                if (!_CaseSensitive)
                {
                    _CurrentName = _CurrentName.ToLower();
                }

                if (!_Parameters.ContainsKey(_CurrentName))
                    _Parameters[_CurrentName] = _CurrentValue;

                pos = _Offset;

                // Error? (pos < (_ParametersString.Length-1))
                if (pos < _ParametersString.Length && !MatchSpace(pos))
                {
                    return false;
                }

                return true;
            }

            return false;
        }

        private bool MatchParameterStruct(int pos)
        {
            if (MatchParameterName(pos))
            {
                _CurrentName = _ParametersString.Substring(pos, _Offset - pos);
                _CurrentValue = "true";

                pos = _Offset;

                if (pos < _ParametersString.Length)
                {
                    if (MatchParameterSeparator(pos))
                    {
                        while (MatchSpace(pos + 1)) pos++;

                        if (MatchParameterValue(pos+1))
                        {
                            pos++;

                            _CurrentValue = _ParametersString.Substring(pos, _Offset - pos);

                            if (String.IsNullOrEmpty(_CurrentValue))
                            {
                                _CurrentValue = "true";
                            }
                            else if (_CurrentValue[0] == '\'' || _CurrentValue[0] == '"')
                            {
                                _CurrentValue = _CurrentValue.Substring(1, _CurrentValue.Length - 2);
                            }

                            pos = _Offset;
                        }
                    }
                }

                _Offset = pos;

                return true;
            }

            return false;
        }

        private bool MatchParameterName(int pos)
        {
            int pos2 = pos;
            pos = AdvanceWhile(pos, MatchParameterNameChar);

            if (pos > pos2)
            {
                _Offset = pos;

                return true;
            }

            return false;
        }

        private bool MatchParameterSeparator(int pos)
        {
            if (this._ParametersString[pos] == ':' ||
                this._ParametersString[pos] == '=' ||
                this._ParametersString[pos] == ' ')
            {
                return true;
            }

            return false;
        }

        private bool MatchParameterValue(int pos)
        {
            if (MatchQuote(pos))
            {
                pos = AdvanceWhileNot(pos+1, MatchQuote);
                if (MatchQuote(pos))
                {
                    pos++;
                    _Offset = pos;
                    return true;
                }
            }
            else if (MatchDoubleQuote(pos))
            {
                pos = AdvanceWhileNot(pos+1, MatchDoubleQuote);
                if (MatchDoubleQuote(pos))
                {
                    pos++;
                    _Offset = pos;
                    return true;
                }
            }
            else if (MatchParameterValueFirstChar(pos))
            {
                pos = AdvanceWhile(pos+1, MatchParameterValueChar);
                _Offset = pos;
                return true;
            }
            return false;
        }

        public delegate bool MatchDelegate(int pos);

        private int AdvanceWhile(int pos, MatchDelegate matcher)
        {
            while (pos < _ParametersString.Length && matcher(pos))
                pos++;
            return pos;
        }

        private int AdvanceWhileNot(int pos, MatchDelegate matcher)
        {
            while (pos < _ParametersString.Length && !matcher(pos))
                pos++;
            return pos;
        }

        private bool MatchAllNotOf(int pos, params char[] cc)
        {
            foreach (char c in cc)
                if (_ParametersString[pos] == c)
                    return false;

            return true;
        }

        private bool MatchParameterNameChar(int pos)
        {
            return MatchAllNotOf(pos, ' ', ':', '=');
        }

        private bool MatchParameterValueChar(int pos)
        {
            return MatchAllNotOf(pos, ' ');
        }

        private bool MatchParameterValueFirstChar(int pos)
        {
            return MatchAllNotOf(pos, ' ', '/', ':', '-', '=');
        }

        private bool Match(int pos, char c)
        {
            return (this._ParametersString[pos] == c);
        }

        private bool MatchSlash(int pos)
        {
            return Match(pos, '/');
        }

        private bool MatchMinus(int pos)
        {
            return Match(pos, '-');
        }

        private bool MatchColon(int pos)
        {
            return Match(pos, ':');
        }

        private bool MatchSpace(int pos)
        {
            return Match(pos, ' ');
        }

        private bool MatchQuote(int pos)
        {
            return Match(pos, '\'');
        }

        private bool MatchDoubleQuote(int pos)
        {
            return Match(pos, '"');
        }

        #endregion
    }
