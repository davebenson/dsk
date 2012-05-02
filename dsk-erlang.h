
typedef enum
{
  DSK_ERLANG_TYPE_ATOM,
  DSK_ERLANG_TYPE_BINARY,
  DSK_ERLANG_TYPE_REF,		/* just a stub */
  DSK_ERLANG_TYPE_FUN,		/* just a stub */
  DSK_ERLANG_TYPE_PORT,		/* just a stub */
  DSK_ERLANG_TYPE_PID,		/* just a stub */
  DSK_ERLANG_TYPE_TUPLE,
  DSK_ERLANG_TYPE_LIST,
  DSK_ERLANG_TYPE_INTEGER,
  DSK_ERLANG_TYPE_FLOAT
} DskErlangType;

union _DskErlangTerm
{
  DskErlangType type;
  DskErlangValueAtom atom;
  DskErlangValueTuple tuple
  DskErlangValueList li;
};

