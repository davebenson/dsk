/* --- these tests code from the IBM portion of 
       Extensible Markup Language (XML) Conformance Test Suites
       http://www.w3.org/XML/Test/
   --- */
{
  "ibm01v01.xml",
  565,
  "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"yes\" ?>\r\n<!-- Above is XMLDecl -->\r\n<!DOCTYPE animal [\r\n<!ELEMENT animal (cat|tiger|leopard)+>\r\n<!ELEMENT cat EMPTY>\r\n<!ELEMENT tiger (#PCDATA)>\r\n<!ELEMENT leopard ANY>\r\n<!ELEMENT small EMPTY>\r\n<!ELEMENT big EMPTY>\r\n<!ATTLIST tiger color CDATA #REQUIRED>\r\n]>\r\n<!-- Above is DTD -->\r\n<?music \"Here is a PI\" ?> \r\n<animal>\r\n   <cat/>\r\n   <tiger color=\"white\">This is a white tiger in Mirage!!</tiger>\r\n   <cat/>\r\n   <leopard>\r\n      <small/>\r\n      <big/>\r\n   </leopard>\r\n</animal>\r\n<!-- Above is element animal -->\r\n\r\n",
  238,
  "<?music \"Here is a PI\" ?><animal>&#10;   <cat></cat>&#10;   <tiger color=\"white\">This is a white tiger in Mirage!!</tiger>&#10;   <cat></cat>&#10;   <leopard>&#10;      <small></small>&#10;      <big></big>&#10;   </leopard>&#10;</animal>"
},
{
  "ibm02v01.xml",
  345,
  "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\r\n<!DOCTYPE book [\r\n<!ELEMENT book ANY>\r\n<!-- This test case covers     legal character ranges plus\r\n        discrete legal characters for production 02. -->\r\n<?NAME_09-\t_0A-\r\n_0D-\r\n_20- _D7FF-\355\237\277\0666c0f-\346\260\217\137\060000-\356\200\200\137\106\106\106\104\55\357\277\275\137\145\146\146\145\55\356\277\276\060010000-\360\220\200\200\06110FFFF-\364\217\277\277\06008ffff-\362\217\277\277 This is a PI target ?>\r\n]>\r\n<book/>\r\n",
  145,
  "<?NAME_09- _0A-\n\n_0D-\n_20- _D7FF-\355\237\277\0666c0f-\346\260\217\137\060000-\356\200\200\137\106\106\106\104\55\357\277\275\137\145\146\146\145\55\356\277\276\060010000-\360\220\200\200\06110FFFF-\364\217\277\277\06008ffff-\362\217\277\277 This is a PI target ?><book></book>\n"
},
{
  "ibm03v01.xml",
  269,
  "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\r\r\n<!DOCTYPE book [\r\r\n<!ELEMENT book ANY>\r\r\n<!-- This test case covers 0 legal character ranges plus\r\r\n     4 discrete legal characters for production 03. -->\r\r\n<?NAME_20- _09-\t_0D-\r_0A-\r\r\n This is a PI target ?>\r\r\n]>\r\r\n<book/>\r\r\n",
  63,
  "<?NAME_20- _09-\t_0D-\n_0A-\n\n This is a PI target ?><book></book>"
},
{
  "ibm09v01.xml",
  178,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)> \r\n \t<!ENTITY FullName \"\">\r\n]>\r\n\r\n<student>My Name is &FullName;. </student>\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n ",
  32,
  "<student>My Name is . </student>"
},
{
  "ibm09v02.xml",
  156,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)> \r\n \t<!ENTITY FullName \"SnowMan\">\r\n]>\r\n\r\n<student>My Name is &FullName;. </student>",
  39,
  "<student>My Name is SnowMan. </student>"
},
{
  "ibm09v03.xml",
  117,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student  SYSTEM \"ibm09v03.dtd\">\r\n<student>I am a new student with &Name;</student>\r\n",
  55,
  "<student>I am a new student with first , last</student>"
},
{
  "ibm09v04.xml",
  237,
  "<?xml version=\"1.0\"?> \r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)>\r\n<!-- testing entity value with Reference -->\r\n\t<!ENTITY RealName \"SnowMan\"> \r\n \t<!ENTITY FullName \"&RealName;\">\r\n]>\r\n\r\n<student>My Name is &FullName;. </student>",
  39,
  "<student>My Name is SnowMan. </student>"
},
{
  "ibm09v05.xml",
  313,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student SYSTEM \"student.dtd\"[\r\n\t<!ELEMENT student (#PCDATA)> \r\n\t<!ENTITY Age \"21\">\r\n\t<!ENTITY Status \"freshman\">\r\n \t<!ENTITY % FullName \"first , last , middle\">\r\n]>\r\n\r\n<!-- testing entity value with combination reference -->\r\n<student>This is a test of &combine;</student>\r\n\r\n\r\n\r\n",
  257,
  "<student>This is a test of My Name is first , last , middle and my age is 21 Again first , last , middle first , last , middle and my status is &#10;&#9;&#9;freshman freshman and first , last , middle 21 first , last , middle freshman That is all.</student>"
},
{
  "ibm10v01.xml",
  393,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)>\r\n\t<!ATTLIST student\r\n\t\tfirst CDATA #REQUIRED\r\n\t\tmiddle CDATA #IMPLIED\r\n\t\tlast CDATA #REQUIRED > \r\n\t<!ENTITY myfirst \"Snow\">\r\n\t<!ENTITY mymiddle \"Y\">\r\n\t<!ENTITY mylast \"\">\r\n]>\r\n<!-- testing AttValue with empty char inside double quote -->\r\n<student first=\"\" last=\"\">My Name is Snow &mylast; Man. </student>\r\n\r\n\r\n\r\n\r\n\r\n\r\n",
  58,
  "<student first=\"\" last=\"\">My Name is Snow  Man. </student>"
},
{
  "ibm10v02.xml",
  383,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)>\r\n\t<!ATTLIST student\r\n\t\tfirst CDATA #REQUIRED\r\n\t\tmiddle CDATA #IMPLIED\r\n\t\tlast CDATA #REQUIRED > \r\n\t<!ENTITY myfirst \"Snow\">\r\n\t<!ENTITY mymiddle \"Y\">\r\n\t<!ENTITY mylast ''>\r\n]>\r\n<!-- testing AttValue with empty char inside single quote -->\r\n<student first='' last=''>My Name is Snow &mylast; Man. </student>\r\n\r\n",
  58,
  "<student first=\"\" last=\"\">My Name is Snow  Man. </student>"
},
{
  "ibm10v03.xml",
  390,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)>\r\n\t<!ATTLIST student\r\n\t\tfirst CDATA #REQUIRED\r\n\t\tmiddle CDATA #IMPLIED\r\n\t\tlast CDATA #REQUIRED > \r\n\t<!ENTITY myfirst 'Snow'>\r\n\t<!ENTITY mymiddle 'I'>\r\n\t<!ENTITY mylast \"Man'\">\r\n]>\r\n<!-- testing AttValue string with a single quote inside -->\r\n<student first=\"Snow'\" last=\"Man\">My Name is &myfirst; &mylast;. </student>",
  66,
  "<student first=\"Snow'\" last=\"Man\">My Name is Snow Man'. </student>"
},
{
  "ibm10v04.xml",
  394,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)>\r\n\t<!ATTLIST student\r\n\t\tfirst CDATA #REQUIRED\r\n\t\tmiddle CDATA #IMPLIED\r\n\t\tlast CDATA #REQUIRED > \r\n\t<!ENTITY myfirst 'Snow'>\r\n\t<!ENTITY mymiddle 'I'>\r\n\t<!ENTITY mylast 'Man\"'>\r\n]>\r\n<!-- testing AttValue string with a double quote inside -->\r\n<student first='Snow\"' last='Man'>My Name is &myfirst; &mylast;. </student>\r\n\r\n",
  76,
  "<student first=\"Snow&quot;\" last=\"Man\">My Name is Snow Man&quot;. </student>"
},
{
  "ibm10v05.xml",
  400,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)>\r\n\t<!ATTLIST student\r\n\t\tfirst CDATA #REQUIRED\r\n\t\tmiddle CDATA #IMPLIED\r\n\t\tlast CDATA #REQUIRED > \r\n\t<!ENTITY myfirst \"Snow\">\r\n\t<!ENTITY mymiddle \"Y\">\r\n\t<!ENTITY mylast \"&myfirst; Man\">\r\n]>\r\n<!-- testing AttValue with a reference in double quote -->\r\n<student first=\"&myfirst;\" last=\"mylast;\">My Name is &mylast;. </student>\r\n\r\n",
  68,
  "<student first=\"Snow\" last=\"mylast;\">My Name is Snow Man. </student>"
},
{
  "ibm10v06.xml",
  401,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)>\r\n\t<!ATTLIST student\r\n\t\tfirst CDATA #REQUIRED\r\n\t\tmiddle CDATA #IMPLIED\r\n\t\tlast CDATA #REQUIRED > \r\n\t<!ENTITY myfirst \"Snow\">\r\n\t<!ENTITY mymiddle \"Y\">\r\n\t<!ENTITY mylast '&myfirst; Man'>\r\n]>\r\n<!-- testing AttValue with a reference in single quote -->\r\n<student first='&myfirst;' last='&mylast;'>My Name is &mylast;. </student>\r\n\r\n",
  69,
  "<student first=\"Snow\" last=\"Snow Man\">My Name is Snow Man. </student>"
},
{
  "ibm10v07.xml",
  519,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)>\r\n\t<!ATTLIST student\r\n\t\tfirst CDATA #REQUIRED\r\n\t\tmiddle CDATA #IMPLIED\r\n\t\tlast CDATA #REQUIRED > \r\n\t<!ENTITY myfirst \"Snow\">\r\n\t<!ENTITY mymiddle \"Y\">\r\n\t<!ENTITY mylast \"Man &myfirst; and &myfirst; mymiddle;.\">\r\n]>\r\n<!-- testing AttValue with references combination in double quotes -->\r\n<student first=\"Full Name &myfirst; &#x31; and &mylast; &mylast; &#x63;\" last=\"&mylast;\" >My first Name is &myfirst; and my last name is &mylast;. </student>\r\n",
  216,
  "<student first=\"Full Name Snow 1 and Man Snow and Snow mymiddle;. Man Snow and Snow mymiddle;. c\" last=\"Man Snow and Snow mymiddle;.\">My first Name is Snow and my last name is Man Snow and Snow mymiddle;.. </student>"
},
{
  "ibm10v08.xml",
  517,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)>\r\n\t<!ATTLIST student\r\n\t\tfirst CDATA #REQUIRED\r\n\t\tmiddle CDATA #IMPLIED\r\n\t\tlast CDATA #REQUIRED > \r\n\t<!ENTITY myfirst \"Snow\">\r\n\t<!ENTITY mymiddle \"I\">\r\n\t<!ENTITY mylast 'Man &myfirst; and &myfirst; mymiddle;.'>\r\n]>\r\n<!-- testing AttValue with references combination in single quote -->\r\n<student first='Full Name &myfirst; and &#x22;&mylast;&#x22; &mylast;' last='&mylast;'>My first Name is &myfirst; and my last name is &mylast;. </student>\r\n\r\n",
  224,
  "<student first=\"Full Name Snow and &quot;Man Snow and Snow mymiddle;.&quot; Man Snow and Snow mymiddle;.\" last=\"Man Snow and Snow mymiddle;.\">My first Name is Snow and my last name is Man Snow and Snow mymiddle;.. </student>"
},
{
  "ibm11v01.xml",
  242,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n   <!ELEMENT student (#PCDATA)>\r\n   <!ENTITY unref SYSTEM \"\">\r\n]>\r\n\r\n<!-- testing systemliteral with nothing between the double quotes -->\r\n<student>My Name is SnowMan. </student>\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n ",
  39,
  "<student>My Name is SnowMan. </student>"
},
{
  "ibm11v02.xml",
  225,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n   <!ELEMENT student (#PCDATA)>\r\n   <!ENTITY unref SYSTEM ''>\r\n]>\r\n\r\n<!-- testing systemliteral with nothing between the single quotes -->\r\n<student>My Name is SnowMan. </student>\r\n",
  39,
  "<student>My Name is SnowMan. </student>"
},
{
  "ibm11v03.xml",
  164,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student SYSTEM 'student.dtd'[\r\n]>\r\n<!-- testing systemliteral with a string with \"'\" -->\r\n<student>My Name is SnowMan. </student>\r\n",
  39,
  "<student>My Name is SnowMan. </student>"
},
{
  "ibm11v04.xml",
  169,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student SYSTEM \"student.dtd\" [\r\n]>\r\n\r\n<!-- testing systemliteral with a string with '\"' -->\r\n<student>My Name is SnowMan. </student>\r\n\r\n",
  39,
  "<student>My Name is SnowMan. </student>"
},
{
  "ibm12v01.xml",
  199,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student PUBLIC \"\" \"student.dtd\"[\r\n]>\r\n\r\n<!-- testing Pubid Literal with nothing between the double quote -->\r\n<student>My Name is SnowMan. </student>\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n ",
  39,
  "<student>My Name is SnowMan. </student>"
},
{
  "ibm12v02.xml",
  185,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student PUBLIC '' 'student.dtd'[\r\n]>\r\n\r\n<!-- testing Pubid Literal with nothing between the single quotes -->\r\n<student>My Name is SnowMan. </student>\r\n",
  39,
  "<student>My Name is SnowMan. </student>"
},
{
  "ibm12v03.xml",
  191,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student PUBLIC \"The big ' in it\" \"student.dtd\"[\r\n]>\r\n\r\n<!-- testing Pubid Literal with a string with \"'\" inside -->\r\n<student>My Name is SnowMan. </student>\r\n",
  39,
  "<student>My Name is SnowMan. </student>"
},
{
  "ibm12v04.xml",
  198,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student PUBLIC 'The latest version' 'student.dtd'[\r\n]>\r\n\r\n<!-- testing Pubid Literal with a string without  \"'\" inside -->\r\n<student>My Name is SnowMan. </student>\r\n",
  39,
  "<student>My Name is SnowMan. </student>"
},
{
  "ibm13v01.xml",
  283,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student PUBLIC \"#x20 #xD #xA abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ -'()+,./:=?;!*#@$_% \" \"student.dtd\"[\r\n]>\r\n\r\n<!-- testing Pubid char with all legal pubidchar in a string -->\r\n<student>My Name is SnowMan. </student>\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n ",
  39,
  "<student>My Name is SnowMan. </student>"
},
{
  "ibm14v01.xml",
  234,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)>\r\n\t<!ATTLIST student first CDATA #REQUIRED\r\n\t\t\t  last  CDATA #IMPLIED>\r\n]>\r\n\r\n<!-- testing chardata with empty string -->\r\n<student first=\"Snow\"></student>\r\n\r\n\r\n",
  32,
  "<student first=\"Snow\"></student>"
},
{
  "ibm14v02.xml",
  260,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)>\r\n\t<!ATTLIST student first CDATA #REQUIRED\r\n\t\t\t  last  CDATA #IMPLIED>\r\n]>\r\n\r\n<!-- testing chardata with white space -->\r\n<student first=\"Eric\"> &#x0A; &#x09; &#x0D;&#x20;</student>\r\n\r\n\r\n",
  50,
  "<student first=\"Eric\"> &#10; &#9; &#13; </student>"
},
{
  "ibm14v03.xml",
  324,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)>\r\n\t<!ATTLIST student first CDATA #REQUIRED\r\n\t\t\t  last  CDATA #IMPLIED>\r\n]>\r\n\r\n<!-- testing chardata with a string of sample legal char except '<' and '&' nor does it contain sequence \"]]>\" -->\r\n<student first=\"Snow\" last=\"Man\">This is a test</student>",
  57,
  "<student first=\"Snow\" last=\"Man\">This is a test</student>"
},
{
  "ibm15v01.xml",
  166,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)> \r\n]>\r\n<!--* Tests empty comment *-->\r\n<!---->\r\n<student>My Name is SnowMan. </student>\r\n\r\n\r\n",
  39,
  "<student>My Name is SnowMan. </student>"
},
{
  "ibm15v02.xml",
  146,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)> \r\n]>\r\n\r\n<!-- Student's name -->\r\n<student>My Name is SnowMan. </student>",
  39,
  "<student>My Name is SnowMan. </student>"
},
{
  "ibm15v03.xml",
  146,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)> \r\n]>\r\n\r\n<!-- student file-1 -->\r\n<student>My Name is SnowMan. </student>",
  39,
  "<student>My Name is SnowMan. </student>"
},
{
  "ibm15v04.xml",
  172,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)> \r\n]>\r\n\r\n<!--student phone number 408-398 (387)-4758 -->\r\n<student>My Name is SnowMan. </student>\r\n",
  39,
  "<student>My Name is SnowMan. </student>"
},
{
  "ibm16v01.xml",
  143,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)> \r\n]>\r\n\r\n<?MyInstruct?>\r\n<student>My Name is SnowMan. </student>\r\n\r\n\r\n",
  54,
  "<?MyInstruct ?><student>My Name is SnowMan. </student>"
},
{
  "ibm16v02.xml",
  138,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)> \r\n]>\r\n\r\n<?MyInstruct ?>\r\n<student>My Name is SnowMan. </student>",
  54,
  "<?MyInstruct ?><student>My Name is SnowMan. </student>"
},
{
  "ibm16v03.xml",
  161,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)> \r\n]>\r\n\r\n<?MyInstruct AVOID ? BEFORE > IN PI ?>\r\n<student>My Name is SnowMan. </student>",
  77,
  "<?MyInstruct AVOID ? BEFORE > IN PI ?><student>My Name is SnowMan. </student>"
},
{
  "ibm17v01.xml",
  159,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)> \r\n]>\r\n\r\n<?MyInstruct This is a test ?>\r\n<student>My Name is SnowMan. </student>\r\n\r\n\r\n",
  69,
  "<?MyInstruct This is a test ?><student>My Name is SnowMan. </student>"
},
{
  "ibm18v01.xml",
  211,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)> \r\n]>\r\n\r\n<!-- testing CDSect with CDStart CData CDEnd -->\r\n\r\n<student>My Name is SnowMan. <![CDATA[This is <normal> text]]> </student>\r\n\r\n",
  67,
  "<student>My Name is SnowMan. This is &lt;normal&gt; text </student>"
},
{
  "ibm19v01.xml",
  180,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)> \r\n]>\r\n\r\n<!-- testing CDStart -->\r\n<student>My Name is SnowMan. <![CDATA[This is a test]]> </student>\r\n\r\n\r\n",
  54,
  "<student>My Name is SnowMan. This is a test </student>"
},
{
  "ibm20v01.xml",
  183,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)> \r\n]>\r\n\r\n<!-- testing CData with empty string -->\r\n\r\n<student>My Name is SnowMan. <![CDATA[]]></student>\r\n\r\n\r\n",
  39,
  "<student>My Name is SnowMan. </student>"
},
{
  "ibm20v02.xml",
  209,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)> \r\n]>\r\n\r\n<!-- testing CData with legal chars -->\r\n\r\n<student>My Name is SnowMan. <![CDATA[<testing>This is a test</testing>]]></student>",
  84,
  "<student>My Name is SnowMan. &lt;testing&gt;This is a test&lt;/testing&gt;</student>"
},
{
  "ibm21v01.xml",
  181,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)> \r\n]>\r\n\r\n<!-- testing CDEnd --> \r\n\r\n<student>My Name is SnowMan. <![CDATA[This is a test]]> </student>\r\n\r\n\r\n",
  54,
  "<student>My Name is SnowMan. This is a test </student>"
},
{
  "ibm22v01.xml",
  92,
  "<?xml version=\"1.0\" encoding=\"utf-8\" ?>\r\n<!DOCTYPE doc [\r\n<!ELEMENT doc EMPTY>\r\n]>\r\n<doc/>\r\n",
  11,
  "<doc></doc>"
},
{
  "ibm22v02.xml",
  51,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc EMPTY>\r\n]>\r\n<doc/>\r\n",
  11,
  "<doc></doc>"
},
{
  "ibm22v03.xml",
  76,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc EMPTY>\r\n]>\r\n<!-- This is a Misc -->\r\n<doc/>\r\n",
  11,
  "<doc></doc>"
},
{
  "ibm22v04.xml",
  76,
  "<!-- This is a Misc -->\r\n<!DOCTYPE doc [\r\n<!ELEMENT doc EMPTY>\r\n]>\r\n<doc/>\r\n",
  11,
  "<doc></doc>"
},
{
  "ibm22v05.xml",
  117,
  "<?xml version=\"1.0\" encoding=\"utf-8\" ?>\r\n<!-- This is a Misc -->\r\n<!DOCTYPE doc [\r\n<!ELEMENT doc EMPTY>\r\n]>\r\n<doc/>\r\n",
  11,
  "<doc></doc>"
},
{
  "ibm22v06.xml",
  117,
  "<?xml version=\"1.0\" encoding=\"utf-8\" ?>\r\n<!DOCTYPE doc [\r\n<!ELEMENT doc EMPTY>\r\n]>\r\n<!-- This is a Misc -->\r\n<doc/>\r\n",
  11,
  "<doc></doc>"
},
{
  "ibm22v07.xml",
  142,
  "<?xml version=\"1.0\" encoding=\"utf-8\" ?>\r\n<!-- This is a Misc -->\r\n<!DOCTYPE doc [\r\n<!ELEMENT doc EMPTY>\r\n]>\r\n<!-- This is a Misc -->\r\n<doc/>\r\n",
  11,
  "<doc></doc>"
},
{
  "ibm23v01.xml",
  74,
  "<?xml version='1.0'?>\r\n<!DOCTYPE doc [\r\n<!ELEMENT doc EMPTY>\r\n]>\r\n<doc/>\r\n",
  11,
  "<doc></doc>"
},
{
  "ibm23v02.xml",
  92,
  "<?xml version='1.0' encoding='UTF-8' ?>\r\n<!DOCTYPE doc [\r\n<!ELEMENT doc EMPTY>\r\n]>\r\n<doc/>\r\n",
  11,
  "<doc></doc>"
},
{
  "ibm23v03.xml",
  92,
  "<?xml version='1.0' standalone='yes' ?>\r\n<!DOCTYPE doc [\r\n<!ELEMENT doc EMPTY>\r\n]>\r\n<doc/>\r\n",
  11,
  "<doc></doc>"
},
{
  "ibm23v04.xml",
  75,
  "<?xml version='1.0' ?>\r\n<!DOCTYPE doc [\r\n<!ELEMENT doc EMPTY>\r\n]>\r\n<doc/>\r\n",
  11,
  "<doc></doc>"
},
{
  "ibm23v05.xml",
  108,
  "<?xml version='1.0' encoding='UTF-8' standalone='yes'?>\r\n<!DOCTYPE doc [\r\n<!ELEMENT doc EMPTY>\r\n]>\r\n<doc/>\r\n",
  11,
  "<doc></doc>"
},
{
  "ibm23v06.xml",
  109,
  "<?xml version='1.0' encoding='UTF-8' standalone='yes' ?>\r\n<!DOCTYPE doc [\r\n<!ELEMENT doc EMPTY>\r\n]>\r\n<doc/>\r\n",
  11,
  "<doc></doc>"
},
{
  "ibm24v01.xml",
  74,
  "<?xml version='1.0'?>\r\n<!DOCTYPE doc [\r\n<!ELEMENT doc EMPTY>\r\n]>\r\n<doc/>\r\n",
  11,
  "<doc></doc>"
},
{
  "ibm24v02.xml",
  74,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE doc [\r\n<!ELEMENT doc EMPTY>\r\n]>\r\n<doc/>\r\n",
  11,
  "<doc></doc>"
},
{
  "ibm25v01.xml",
  74,
  "<?xml version='1.0'?>\r\n<!DOCTYPE doc [\r\n<!ELEMENT doc EMPTY>\r\n]>\r\n<doc/>\r\n",
  11,
  "<doc></doc>"
},
{
  "ibm25v02.xml",
  75,
  "<?xml version ='1.0'?>\r\n<!DOCTYPE doc [\r\n<!ELEMENT doc EMPTY>\r\n]>\r\n<doc/>\r\n",
  11,
  "<doc></doc>"
},
{
  "ibm25v03.xml",
  75,
  "<?xml version= '1.0'?>\r\n<!DOCTYPE doc [\r\n<!ELEMENT doc EMPTY>\r\n]>\r\n<doc/>\r\n",
  11,
  "<doc></doc>"
},
{
  "ibm25v04.xml",
  76,
  "<?xml version = '1.0'?>\r\n<!DOCTYPE doc [\r\n<!ELEMENT doc EMPTY>\r\n]>\r\n<doc/>\r\n",
  11,
  "<doc></doc>"
},
{
  "ibm26v01.xml",
  78,
  "<?xml version='1.0' ?>\r\n<!DOCTYPE doc [\r\n   <!ELEMENT doc EMPTY>\r\n]>\r\n<doc/>\r\n",
  11,
  "<doc></doc>"
},
{
  "ibm27v01.xml",
  112,
  "<?xml version='1.0' ?>\r\n<!DOCTYPE doc [\r\n   <!ELEMENT doc EMPTY>\r\n]>\r\n<doc/>\r\n<!-- This is a comment in Misc -->",
  11,
  "<doc></doc>"
},
{
  "ibm27v02.xml",
  110,
  "<?xml version='1.0' ?>\r\n<!DOCTYPE doc [\r\n   <!ELEMENT doc EMPTY>\r\n]>\r\n<doc/>\r\n<?sound \"This is a PI in Misc ?>",
  43,
  "<doc></doc><?sound \"This is a PI in Misc ?>"
},
{
  "ibm27v03.xml",
  107,
  "<?xml version='1.0' ?>\r\n<!DOCTYPE doc [\r\n   <!ELEMENT doc ANY>\r\n]>\r\n<doc>S is in the following Misc</doc>\r\n",
  37,
  "<doc>S is in the following Misc</doc>"
},
{
  "ibm28v01.xml",
  157,
  "<?xml version=\"1.0\" encoding='UTF-8'?>\r\n<!DOCTYPE animal [\r\n   <!ELEMENT animal EMPTY>\r\n]>\r\n<!-- This a valid test file for production [28] --> \r\n<animal/>\r\n",
  17,
  "<animal></animal>"
},
{
  "ibm28v02.xml",
  805,
  "<?xml version=\"1.0\" encoding=\"utf-8\" ?>\r\n<!DOCTYPE animal SYSTEM \"ibm28v02.dtd\" [\r\n   <!NOTATION animal_class SYSTEM \"ibm28v02.txt\">\r\n   <!ENTITY forcat \"This is a small cat\">\r\n   <!ELEMENT tiger (#PCDATA)>\r\n   <!ENTITY % make_small \"<!ELEMENT small EMPTY>\">\r\n   <!ENTITY % make_leopard_element \"<!ELEMENT leopard ANY>\">\r\n   <!ENTITY % make_attlist \"<!ATTLIST tiger color CDATA #REQUIRED>\">\r\n   %make_leopard_element; \r\n   <!ELEMENT cat ANY>\r\n   %make_small;\r\n   <!ENTITY % make_big \"<!ELEMENT big EMPTY>\">\r\n   %make_big;\r\n   %make_attlist;\r\n   <?sound \"This is a PI\" ?>\r\n   <!-- This is a valid test file for p28 -->\r\n]>\r\n<animal>\r\n   <cat>&forcat;</cat>\r\n   <tiger color=\"white\">This is a white tiger in Mirage!!</tiger>\r\n   <cat/>\r\n   <leopard>\r\n      <small/>\r\n      <big/>\r\n   </leopard>\r\n</animal>\r\n",
  326,
  "<?sound \"This is a PI\" ?><!DOCTYPE animal [\n<!NOTATION animal_class SYSTEM 'ibm28v02.txt'>\n]>\n<animal>&#10;   <cat>This is a small cat</cat>&#10;   <tiger color=\"white\">This is a white tiger in Mirage!!</tiger>&#10;   <cat></cat>&#10;   <leopard>&#10;      <small></small>&#10;      <big></big>&#10;   </leopard>&#10;</animal>"
},
{
  "ibm29v01.xml",
  627,
  "<?xml version=\"1.0\" encoding=\"utf-8\" ?>\r\n<!DOCTYPE animal [\r\n   <!ELEMENT animal (cat|tiger|leopard)+>\r\n   <!NOTATION animal_class SYSTEM \"ibm29v01.txt\">\r\n   <!ELEMENT cat ANY>\r\n   <!ENTITY forcat \"This is a small cat\">\r\n   <!ELEMENT tiger (#PCDATA)>\r\n   <!ELEMENT leopard ANY>\r\n   <!ELEMENT small EMPTY>\r\n   <!ELEMENT big EMPTY>\r\n   <!ATTLIST tiger color CDATA #REQUIRED>\r\n   <?sound \"This is a PI\" ?>\r\n   <!-- This is a comment -->\r\n    \r\n]>\r\n<animal>\r\n   <cat>&forcat;</cat>\r\n   <tiger color=\"white\">This is a white tiger in Mirage!!</tiger>\r\n   <cat/>\r\n   <leopard>\r\n      <small/>\r\n      <big/>\r\n   </leopard>\r\n</animal>\r\n",
  326,
  "<?sound \"This is a PI\" ?><!DOCTYPE animal [\n<!NOTATION animal_class SYSTEM 'ibm29v01.txt'>\n]>\n<animal>&#10;   <cat>This is a small cat</cat>&#10;   <tiger color=\"white\">This is a white tiger in Mirage!!</tiger>&#10;   <cat></cat>&#10;   <leopard>&#10;      <small></small>&#10;      <big></big>&#10;   </leopard>&#10;</animal>"
},
{
  "ibm29v02.xml",
  690,
  "<?xml version=\"1.0\" encoding=\"utf-8\" ?>\r\n<!DOCTYPE animal [\r\n   <!ELEMENT animal (cat|tiger|leopard)+>\r\n   <!NOTATION animal_class SYSTEM \"ibm29v01.txt\">\r\n   <!ELEMENT cat ANY>\r\n   <!ENTITY forcat \"This is a small cat\">\r\n   <!ELEMENT tiger (#PCDATA)>\r\n   <!ENTITY % make_leopard_element \"<!ELEMENT leopard ANY>\">\r\n   %make_leopard_element; \r\n   <!ELEMENT small EMPTY>\r\n   <!ELEMENT big EMPTY>\r\n   <!ATTLIST tiger color CDATA #REQUIRED>\r\n   <?sound \"This is a PI\" ?>\r\n   <!-- This is a comment -->\r\n    \r\n]>\r\n<animal>\r\n   <cat>&forcat;</cat>\r\n   <tiger color=\"white\">This is a white tiger in Mirage!!</tiger>\r\n   <cat/>\r\n   <leopard>\r\n      <small/>\r\n      <big/>\r\n   </leopard>\r\n</animal>\r\n",
  326,
  "<?sound \"This is a PI\" ?><!DOCTYPE animal [\n<!NOTATION animal_class SYSTEM 'ibm29v01.txt'>\n]>\n<animal>&#10;   <cat>This is a small cat</cat>&#10;   <tiger color=\"white\">This is a white tiger in Mirage!!</tiger>&#10;   <cat></cat>&#10;   <leopard>&#10;      <small></small>&#10;      <big></big>&#10;   </leopard>&#10;</animal>"
},
{
  "ibm30v01.xml",
  118,
  "<!DOCTYPE animal SYSTEM \"ibm30v01.dtd\">\r\n<animal/>\r\n<!-- tests extSubset with extSubsetDecl only in the dtd file -->\r\n",
  17,
  "<animal></animal>"
},
{
  "ibm30v02.xml",
  126,
  "<!DOCTYPE animal SYSTEM \"ibm30v02.dtd\">\r\n<animal/>\r\n<!-- tests extSubset with TextDecl and extSubsetDecl in the dtd file -->\r\n",
  17,
  "<animal></animal>"
},
{
  "ibm31v01.xml",
  187,
  "<!DOCTYPE animal SYSTEM \"ibm31v01.dtd\">\r\n<animal>\r\n   <tiger/>\r\n</animal>\r\n<!-- tests extSubsetDecl with combinations of markupdecls, conditionalSects, PEReferences and white spaces -->\r\n",
  45,
  "<animal>&#10;   <tiger></tiger>&#10;</animal>"
},
{
  "ibm32v01.xml",
  210,
  "<?xml version=\"1.0\" standalone=\"no\" ?>\r\n<!DOCTYPE animal SYSTEM \"ibm32v01.dtd\">\r\n<animal/>\r\n<!-- Tests VC: Standalone Document Declaration with absent attribute that has default value and standalone is no -->\r\n",
  32,
  "<animal color=\"yellow\"></animal>"
},
{
  "ibm32v02.xml",
  221,
  "<?xml version=\"1.0\" standalone=\"no\" ?>\r\n<!DOCTYPE animal SYSTEM \"ibm32v02.dtd\">\r\n<animal>&animal_content;</animal>\r\n<!-- Tests VC: Standalone Document Declaration with external enitity reference and standalone is no -->\r\n",
  39,
  "<animal>This is a yellow tiger</animal>"
},
{
  "ibm32v03.xml",
  214,
  "<?xml version=\"1.0\" standalone=\"no\" ?>\r\n<!DOCTYPE animal SYSTEM \"ibm32v03.dtd\">\r\n<animal/>\r\n<!-- Tests VC: Standalone Document Declaration with attribute values that need to be normalized and standalone is no -->\r\n",
  27,
  "<animal class=\")\"></animal>"
},
{
  "ibm32v04.xml",
  225,
  "<?xml version=\"1.0\" standalone=\"no\" ?>\r\n<!DOCTYPE animal SYSTEM \"ibm32v04.dtd\">\r\n<animal>This is a \r\n      <a/>  \r\n\r\nyellow tiger</animal>\r\n<!-- Tests VC: Standalone Document Declaration with whitespace in mixed content -->\r\n",
  90,
  "<animal xml:space=\"preserve\">This is a &#10;      <a></a>  &#10;&#10;yellow tiger</animal>"
},
{
  "ibm33v01.xml",
  196,
  "<!DOCTYPE book [\r\n   <!ELEMENT book ANY>\r\n   <!ATTLIST book xml:lang CDATA #REQUIRED>\r\n]>\r\n<book xml:lang=\"en-US\">It is written in English</book>\r\n<!-- Tests LanguageID with Langcode - Subcode -->",
  54,
  "<book xml:lang=\"en-US\">It is written in English</book>"
},
{
  "ibm34v01.xml",
  147,
  "<!DOCTYPE book [\r\n   <!ELEMENT book ANY>\r\n   <!ATTLIST book xml:lang CDATA #REQUIRED>\r\n]>\r\n<book xml:lang=\"en-US\">It is written in English</book>\r\n",
  54,
  "<book xml:lang=\"en-US\">It is written in English</book>"
},
{
  "ibm35v01.xml",
  144,
  "<!DOCTYPE book [\r\n   <!ELEMENT book ANY>\r\n   <!ATTLIST book xml:lang CDATA #REQUIRED>\r\n]>\r\n<book xml:lang=\"en\">It is written in English</book>\r\n",
  51,
  "<book xml:lang=\"en\">It is written in English</book>"
},
{
  "ibm36v01.xml",
  151,
  "<!DOCTYPE book [\r\n   <!ELEMENT book ANY>\r\n   <!ATTLIST book xml:lang CDATA #REQUIRED>\r\n]>\r\n<book xml:lang=\"i-BS-ABCD\">It is written in English</book>\r\n",
  58,
  "<book xml:lang=\"i-BS-ABCD\">It is written in English</book>"
},
{
  "ibm37v01.xml",
  150,
  "<!DOCTYPE book [\r\n   <!ELEMENT book ANY>\r\n   <!ATTLIST book xml:lang CDATA #REQUIRED>\r\n]>\r\n<book xml:lang=\"x-uk-eng\">It is written in English</book>\r\n",
  57,
  "<book xml:lang=\"x-uk-eng\">It is written in English</book>"
},
{
  "ibm38v01.xml",
  148,
  "<!DOCTYPE book [\r\n   <!ELEMENT book ANY>\r\n   <!ATTLIST book xml:lang CDATA #REQUIRED>\r\n]>\r\n<book xml:lang=\"en-USa\">It is written in English</book>\r\n",
  55,
  "<book xml:lang=\"en-USa\">It is written in English</book>"
},
{
  "ibm39v01.xml",
  399,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n  <!ELEMENT root (a,b)>\r\n  <!ELEMENT a EMPTY>\r\n  <!ELEMENT b (#PCDATA|c)* >\r\n  <!ELEMENT c ANY>\r\n  <!ELEMENT d ((e,e)|f)+ >\r\n  <!ELEMENT e ANY>\r\n  <!ELEMENT f EMPTY>\r\n]>\r\n<root><a/><b>\r\n   <c></c> \r\n   content of b element\r\n   <c>\r\n      <d><e>no more children</e><e><f/></e><f/></d>\r\n   </c>\r\n</b></root>\r\n<!--* test P39's syntax and Element Valid VC *-->\r\n",
  161,
  "<root><a></a><b>&#10;   <c></c> &#10;   content of b element&#10;   <c>&#10;      <d><e>no more children</e><e><f></f></e><f></f></d>&#10;   </c>&#10;</b></root>"
},
{
  "ibm40v01.xml",
  428,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n  <!ELEMENT root (#PCDATA|b)* >\r\n  <!ELEMENT b (#PCDATA) >\r\n  <!ATTLIST b attr1 CDATA #IMPLIED>\r\n  <!ATTLIST b attr2 CDATA #IMPLIED>\r\n  <!ATTLIST b attr3 CDATA #IMPLIED>\r\n]>\r\n<root>\r\n  <b>without white space</b>\r\n  <b > with a white space</b>\r\n  <b attr1=\"value1\">one attribute</b>\r\n  <b attr1=\"value1\" attr2=\"value2\" attr3 = \"value3\">one attribute</b>\r\n</root>\r\n<!--* testing P40 *-->\r\n",
  198,
  "<root>&#10;  <b>without white space</b>&#10;  <b> with a white space</b>&#10;  <b attr1=\"value1\">one attribute</b>&#10;  <b attr1=\"value1\" attr2=\"value2\" attr3=\"value3\">one attribute</b>&#10;</root>"
},
{
  "ibm41v01.xml",
  333,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n  <!ELEMENT root (#PCDATA|b)* >\r\n  <!ELEMENT b (#PCDATA) >\r\n  <!ATTLIST b attr1 CDATA #REQUIRED>\r\n  <!ATTLIST b attr2 (abc|def) \"abc\">\r\n  <!ATTLIST b attr3 CDATA #FIXED \"fixed\">\r\n]>\r\n<root>\r\n  <b attr1=\"value1\" attr2=\"def\" attr3=\"fixed\">Name eq AttValue</b>\r\n</root>\r\n<!--* testing P41 *-->\r\n",
  89,
  "<root>&#10;  <b attr1=\"value1\" attr2=\"def\" attr3=\"fixed\">Name eq AttValue</b>&#10;</root>"
},
{
  "ibm42v01.xml",
  261,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n  <!ELEMENT root (a,b)>\r\n  <!ELEMENT a EMPTY>\r\n  <!ELEMENT b (#PCDATA|c)* >\r\n  <!ELEMENT c ANY>\r\n]>\r\n<root><a/><b>\r\n   <c></c > : End tag with a space inside\r\n   content of b element\r\n</b></root>\r\n<!--* test P42 *-->\r\n\r\n",
  105,
  "<root><a></a><b>&#10;   <c></c> : End tag with a space inside&#10;   content of b element&#10;</b></root>"
},
{
  "ibm43v01.xml",
  746,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n  <!ELEMENT root (a,b)>\r\n  <!ELEMENT a EMPTY>\r\n  <!ELEMENT b (#PCDATA|c)* >\r\n  <!ELEMENT c ANY>\r\n  <!ENTITY inContent \"<b>General entity reference in element content</b>\">\r\n]>\r\n<!--* content: element|CharData|Reference|CDSect|PI|CDSect|PI|Comment *-->\r\n<root><a/><b>\r\n<!-- there is an empty element in the above line -->\r\n   <c></c> \r\n   CharData: content of b element\r\n   %paaa; : PE reference should not be recognized in element content \r\n   <c>\r\n<?PIcontent anyProcessingInstruction?>\r\n<!-- Comment content -->\r\n    &inContent;\r\n    Charater reference: &#x41;\r\n    CDSect in content: <![CDATA[ <html>markups<head>HEAD</head><body>nothing</body></html> ]]>\r\n   </c>\r\n</b>\r\n</root>\r\n<!--* test P43 *-->\r\n",
  453,
  "<root><a></a><b>&#10;&#10;   <c></c> &#10;   CharData: content of b element&#10;   %paaa; : PE reference should not be recognized in element content &#10;   <c>&#10;<?PIcontent anyProcessingInstruction?>&#10;&#10;    <b>General entity reference in element content</b>&#10;    Charater reference: A&#10;    CDSect in content:  &lt;html&gt;markups&lt;head&gt;HEAD&lt;/head&gt;&lt;body&gt;nothing&lt;/body&gt;&lt;/html&gt; &#10;   </c>&#10;</b>&#10;</root>"
},
{
  "ibm44v01.xml",
  393,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n  <!ELEMENT root (#PCDATA|b)* >\r\n  <!ELEMENT b EMPTY >\r\n  <!ATTLIST b attr1 CDATA #IMPLIED>\r\n  <!ATTLIST b attr2 CDATA #IMPLIED>\r\n  <!ATTLIST b attr3 CDATA #IMPLIED>\r\n]>\r\n<root>\r\n  <b/>without white space\r\n  <b /> with a white space\r\n  <b attr1=\"value1\" />\r\n  <b attr1=\"value1\" attr2=\"value2\" attr3 = \"value3\"/>\r\n</root>\r\n<!--* testing P44 *-->\r\n\r\n\r\n\r\n",
  172,
  "<root>&#10;  <b></b>without white space&#10;  <b></b> with a white space&#10;  <b attr1=\"value1\"></b>&#10;  <b attr1=\"value1\" attr2=\"value2\" attr3=\"value3\"></b>&#10;</root>"
},
{
  "ibm45v01.xml",
  558,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n  <!ELEMENT root (#PCDATA|b)* >\r\n  <!--* P45 no space before the end bracket *-->\r\n  <!ELEMENT b EMPTY>\r\n  <!ELEMENT unique ANY>\r\n  <!ELEMENT unique- ANY>\r\n  <!ELEMENT unique_ ANY>\r\n  <!ELEMENT unique. ANY>\r\n  <!ATTLIST b attr1 CDATA #IMPLIED>\r\n  <!ATTLIST b attr2 CDATA #IMPLIED>\r\n  <!ATTLIST b attr3 CDATA #IMPLIED>\r\n]>\r\n<root>\r\n  <b/>without white space\r\n  <b /> with a white space\r\n  <b attr1=\"value1\" />\r\n  <b attr1=\"value1\" attr2=\"value2\" attr3 = \"value3\"/>\r\n</root>\r\n<!--* !!! testing both P45 and p46 *-->\r\n\r\n",
  172,
  "<root>&#10;  <b></b>without white space&#10;  <b></b> with a white space&#10;  <b attr1=\"value1\"></b>&#10;  <b attr1=\"value1\" attr2=\"value2\" attr3=\"value3\"></b>&#10;</root>"
},
{
  "ibm47v01.xml",
  827,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n  <!ELEMENT root (a,b)>\r\n  <!ELEMENT a EMPTY>\r\n  <!ELEMENT b (#PCDATA|c)* >\r\n  <!ELEMENT c ANY>\r\n  <!ELEMENT d ANY>\r\n  <!ELEMENT e ANY>\r\n  <!ELEMENT f ANY>\r\n  <!--* test all possible children,cp,choice,seq patterns in P47,P48,P49,P50 *-->\r\n  <!ELEMENT child0 (a)>\r\n  <!ELEMENT child1 (a|b|c)>\r\n  <!ELEMENT child2 (a ,b,b?,a*,c,c,a,a,b+,c ) >\r\n  <!ELEMENT child3 (a+|b)? >\r\n  <!ELEMENT child4 (a, (b|c)+, (a|d)?, (e|f)* )?>\r\n  <!ELEMENT child5 ( (a,b) | c? | ((d|e),b,c) )* >\r\n  <!ELEMENT child5_1 ( (a,b)* | (c,b)? | (d,a)+ | ((e|f),b,c) )* >\r\n  <!ELEMENT child6 (a,b,c)*>\r\n  <!ELEMENT child7 ((a,b)|c*|((d|e),b,c) )+ >\r\n  <!ELEMENT child8 ( a, (b|c), (a|b), b)+>  \r\n]>\r\n<root><a/><b>\r\n   <c></c >\r\n   content of b element\r\n</b></root>\r\n<!--* a valid test: tests P47,P48,P49,P50*-->\r\n\r\n",
  75,
  "<root><a></a><b>&#10;   <c></c>&#10;   content of b element&#10;</b></root>"
},
{
  "ibm49v01.xml",
  228,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root SYSTEM \"ibm49v01.dtd\"[\r\n  <!ELEMENT root (a,b)>\r\n]>\r\n<root><a/><b>\r\n   <c></c>\r\n   content of b element\r\n</b></root>\r\n<!--* a valid test: tests VC:Proper Group/PE Nesting in P49 *-->\r\n\r\n\r\n\r\n",
  75,
  "<root><a></a><b>&#10;   <c></c>&#10;   content of b element&#10;</b></root>"
},
{
  "ibm50v01.xml",
  307,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root SYSTEM \"ibm50v01.dtd\" [\r\n  <!ELEMENT root (a,b)>\r\n]>\r\n<root><a/><b>\r\n   <c><child1><a/><b></b><c></c></child1></c >\r\n   <c><child2><a/><b></b><c></c></child2></c >\r\n   content of b element\r\n</b></root>\r\n<!--* a valid test: tests VC:Proper Group/PE Nesting in P50 *-->\r\n",
  166,
  "<root><a></a><b>&#10;   <c><child1><a></a><b></b><c></c></child1></c>&#10;   <c><child2><a></a><b></b><c></c></child2></c>&#10;   content of b element&#10;</b></root>"
},
{
  "ibm51v01.xml",
  604,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n  <!ELEMENT root ANY>\r\n  <!--* test all possible Mixed content type decl *-->\r\n  <!ELEMENT a (#PCDATA)* >\r\n  <!ELEMENT b (#PCDATA) >\r\n  <!ELEMENT c ( #PCDATA)*>\r\n  <!ELEMENT d (#PCDATA|c)*>\r\n  <!ELEMENT e (#PCDATA|c| b|a)* >\r\n  <!ELEMENT f (#PCDATA| c)* >\r\n  <!ELEMENT g ( #PCDATA) >\r\n  <!ELEMENT h (#PCDATA )>\r\n  <!ELEMENT i ( #PCDATA ) >\r\n]>\r\n<root>\r\n  <a> Element type a </a>\r\n  <b> Element type b </b>\r\n  <c> Element type c </c>\r\n  <d> Element type d <c></c> </d>\r\n  <e> Element type e <a></a> <b></b> <c></c> </e>\r\n</root>\r\n<!--* a valid test: tests P51 *-->",
  200,
  "<root>&#10;  <a> Element type a </a>&#10;  <b> Element type b </b>&#10;  <c> Element type c </c>&#10;  <d> Element type d <c></c> </d>&#10;  <e> Element type e <a></a> <b></b> <c></c> </e>&#10;</root>"
},
{
  "ibm51v02.xml",
  336,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root SYSTEM \"ibm51v02.dtd\" [\r\n  <!ELEMENT root ANY>\r\n]>\r\n<root>\r\n  <a> Element type a </a>\r\n  <b> Element type b </b>\r\n  <c> Element type c </c>\r\n  <d> Element type d <c></c> </d>\r\n  <e> Element type e <a></a> <b></b> <c></c> </e>\r\n</root>\r\n<!--* a valid test: tests P51 VC: Proper Group/PE Nesting *-->",
  200,
  "<root>&#10;  <a> Element type a </a>&#10;  <b> Element type b </b>&#10;  <c> Element type c </c>&#10;  <d> Element type d <c></c> </d>&#10;  <e> Element type e <a></a> <b></b> <c></c> </e>&#10;</root>"
},
{
  "ibm52v01.xml",
  532,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n  <!ELEMENT root ANY>\r\n  <!ELEMENT a (#PCDATA)* >\r\n  <!ELEMENT b (#PCDATA) >\r\n  <!--* All AttlistDecl Patterns in P52 and P53 *-->\r\n  <!ATTLIST a>\r\n  <!ATTLIST a >\r\n  <!ATTLIST b battr1 CDATA #REQUIRED >\r\n  <!ATTLIST b battr2 CDATA #IMPLIED \r\n              battr3 CDATA #FIXED \"fixedvalue\" battr4 (abc|def) \"abc\" >\r\n]>\r\n<root>\r\n  <a> Element type a </a>\r\n  <b battr1 = \"anyvalue\" battr3=\"fixedvalue\" battr4 =\"def\"> test P52 and P53 </b>\r\n</root>\r\n<!--* a valid test: tests P52 and P53 *-->\r\n",
  131,
  "<root>&#10;  <a> Element type a </a>&#10;  <b battr1=\"anyvalue\" battr3=\"fixedvalue\" battr4=\"def\"> test P52 and P53 </b>&#10;</root>"
},
{
  "ibm54v01.xml",
  1857,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n  <!ELEMENT root ANY>\r\n  <!ELEMENT a (#PCDATA) >\r\n  <!ELEMENT b (#PCDATA) >\r\n  <!ELEMENT c (#PCDATA) >\r\n  <!ELEMENT d (#PCDATA) >\r\n  <!ELEMENT e (#PCDATA) >\r\n  <!ELEMENT f (#PCDATA) >\r\n  <!ELEMENT g (#PCDATA) >\r\n  <!ELEMENT h (#PCDATA) >\r\n  <!ELEMENT i (#PCDATA) >\r\n  <!ELEMENT j (#PCDATA) >\r\n  <!ELEMENT k (#PCDATA) >\r\n  <!--* Tests all Attbuite types in P55,P56,P57,P58,P59 *-->\r\n  <!ATTLIST a aattr1 ID #REQUIRED>\r\n  <!ATTLIST b battr1 CDATA #REQUIRED \r\n              battr2 (good|bad) #REQUIRED \r\n              battr3 ID #REQUIRED>\r\n  <!ATTLIST c c_reference IDREF #REQUIRED>\r\n  <!ATTLIST d d_reference IDREFS #REQUIRED>\r\n  <!ENTITY xmltech SYSTEM \"xmltech.gif\" NDATA gif>\r\n  <!NOTATION gif SYSTEM \"gif\">\r\n  <!ATTLIST e eattr1 ENTITY #REQUIRED>\r\n  <!ENTITY IBMlogo SYSTEM \"IBMlogo.gif\" NDATA gif>\r\n  <!ATTLIST f fattr1 ENTITIES #REQUIRED>\r\n  <!ATTLIST g gattr1 NMTOKEN #REQUIRED>\r\n  <!ATTLIST h hattr1 NMTOKENS #REQUIRED>\r\n  <!NOTATION UTF-8 SYSTEM \"UTF-8\">\r\n  <!ATTLIST i iattr1 NOTATION (UTF-8) #REQUIRED>\r\n  <!--* Tests all DefaultDecl in P60 -->\r\n  <!ATTLIST j jattr1 CDATA #REQUIRED \r\n              jattr2 CDATA \"good\" \r\n              jattr3 CDATA #FIXED \"fixed\"\r\n\t      jattr4 CDATA #IMPLIED >\r\n\r\n]>\r\n<root>\r\n  <a aattr1 = \"a1\"> Element type a </a>\r\n  <b battr1 = \"anyvalue\" battr2=\"good\" battr3 =\"b1\"> Element type b </b>\r\n  <c c_reference = \"b1\"> Element type c </c>\r\n  <d d_reference = \"a1 b1\"> Element type d </d>\r\n  <e eattr1 = \"xmltech\"> Element type e </e>\r\n  <f fattr1 = \"xmltech IBMlogo\"> Element type f </f>\r\n  <g gattr1 = \"xml4j3_0_0_EA3\"> Element type g </g>\r\n  <h hattr1 = \"xml4j3_0_0_EA3 Xerces-J_1_0_1\"> Element type h </h>\r\n  <i iattr1 = \"UTF-8\"> Element type i </i>\r\n  <j jattr1 = \"anyvalue\" jattr2=\"good\" jattr3 =\"fixed\"> Element type j </j>\r\n</root>\r\n<!--* a valid test: tests P54 *-->\r\n",
  660,
  "<!DOCTYPE root [\n<!NOTATION UTF-8 SYSTEM 'UTF-8'>\n<!NOTATION gif SYSTEM 'gif'>\n]>\n<root>&#10;  <a aattr1=\"a1\"> Element type a </a>&#10;  <b battr1=\"anyvalue\" battr2=\"good\" battr3=\"b1\"> Element type b </b>&#10;  <c c_reference=\"b1\"> Element type c </c>&#10;  <d d_reference=\"a1 b1\"> Element type d </d>&#10;  <e eattr1=\"xmltech\"> Element type e </e>&#10;  <f fattr1=\"xmltech IBMlogo\"> Element type f </f>&#10;  <g gattr1=\"xml4j3_0_0_EA3\"> Element type g </g>&#10;  <h hattr1=\"xml4j3_0_0_EA3 Xerces-J_1_0_1\"> Element type h </h>&#10;  <i iattr1=\"UTF-8\"> Element type i </i>&#10;  <j jattr1=\"anyvalue\" jattr2=\"good\" jattr3=\"fixed\"> Element type j </j>&#10;</root>"
},
{
  "ibm54v02.xml",
  368,
  "<?xml  version=\"1.0\"?>\r\n<!-- test for Production 54-->\r\n<!DOCTYPE root\r\n [\r\n <!ELEMENT root (x|y|z)*>\r\n <!ELEMENT x (#PCDATA)>\r\n <!ELEMENT y ANY>\r\n <!ELEMENT z EMPTY>\r\n <!ATTLIST x attr CDATA #IMPLIED>\r\n <!ATTLIST y attr NMTOKENS #IMPLIED>\r\n <!ATTLIST z attr (x|y) #IMPLIED>\r\n ]>\r\n<root>\r\n<x attr= 'Madhu'></x>\r\n<y attr= '1.a.name.token.but.not.a.name'></y>\r\n</root>\r\n",
  92,
  "<root>&#10;<x attr=\"Madhu\"></x>&#10;<y attr=\"1.a.name.token.but.not.a.name\"></y>&#10;</root>"
},
{
  "ibm54v03.xml",
  220,
  "<?xml  version=\"1.0\"?>\r\n<!-- test for Production 54-->\r\n<!DOCTYPE AttrType\r\n[\r\n<!ELEMENT AttrType ANY>\r\n<!ELEMENT a (#PCDATA)>\r\n<!ATTLIST a att CDATA #IMPLIED>\r\n]>\r\n<AttrType>\r\n<a att= 'hello world'>\r\n</a>\r\n</AttrType>\r\n",
  61,
  "<AttrType>&#10;<a att=\"hello world\">&#10;</a>&#10;</AttrType>"
},
{
  "ibm55v01.xml",
  254,
  "<?xml  version=\"1.0\"?>\r\n<!-- test valid syntax for Production 55-->\r\n<!DOCTYPE StType\r\n[\r\n<!ELEMENT StType ANY>\r\n<!ELEMENT a EMPTY>\r\n<!ATTLIST a att CDATA #IMPLIED> \r\n]>\r\n<StType>\r\n<a att='Hello'/>\r\nTesting with a valid stringType attribute \r\n</StType>\r\n",
  93,
  "<StType>&#10;<a att=\"Hello\"></a>&#10;Testing with a valid stringType attribute &#10;</StType>"
},
{
  "ibm56v01.xml",
  530,
  "<?xml  version=\"1.0\"?>\r\n<!-- valid test for Production 56-->\r\n<!DOCTYPE root\r\n [\r\n <!ELEMENT root (a|b|c|d|e|f|g)*>\r\n <!ELEMENT a EMPTY>\r\n <!ELEMENT b EMPTY>\r\n <!ELEMENT c EMPTY>\r\n <!ELEMENT d EMPTY>\r\n <!ELEMENT e EMPTY>\r\n <!ELEMENT f EMPTY>\r\n <!ELEMENT g EMPTY>\r\n <!ATTLIST a attr ID #IMPLIED>\r\n <!ATTLIST b attr IDREF #IMPLIED>\r\n <!ATTLIST c attr IDREFS #IMPLIED>\r\n <!ATTLIST d attr ENTITY #IMPLIED>\r\n <!ATTLIST e attr ENTITIES #IMPLIED>\r\n <!ATTLIST f attr NMTOKEN #IMPLIED>\r\n <!ATTLIST g attr NMTOKENS #IMPLIED>\r\n ]>\r\n<root/>\r\n",
  13,
  "<root></root>"
},
{
  "ibm56v02.xml",
  374,
  "<?xml  version=\"1.0\"?>\r\n<!-- validity constraint check for Production 56(Positive Test)-->\r\n<!DOCTYPE tokenizer\r\n [\r\n <!ELEMENT tokenizer ANY>\r\n <!ATTLIST tokenizer UniqueName ID #REQUIRED>\r\n ]>\r\n<tokenizer UniqueName = \"AC1999\">\r\nThis is a positive test for validity constraints\r\nGiving a unique name to the attribute ID an ID Attribute default as #required\r\n</tokenizer>\r\n",
  183,
  "<tokenizer UniqueName=\"AC1999\">&#10;This is a positive test for validity constraints&#10;Giving a unique name to the attribute ID an ID Attribute default as #required&#10;</tokenizer>"
},
{
  "ibm56v03.xml",
  333,
  "<?xml  version=\"1.0\"?>\r\n<!-- validity constraint check for Production 56(Positive Test)-->\r\n<!DOCTYPE tokenizer\r\n [\r\n <!ELEMENT tokenizer ANY>\r\n <!ATTLIST tokenizer UniqueName ID #IMPLIED>\r\n ]>\r\n<tokenizer UniqueName = \"AC1999\">\r\nThis is a positive test for validity constraints\r\nGiving ID attribute default as #IMPLIED\r\n</tokenizer>",
  145,
  "<tokenizer UniqueName=\"AC1999\">&#10;This is a positive test for validity constraints&#10;Giving ID attribute default as #IMPLIED&#10;</tokenizer>"
},
{
  "ibm56v04.xml",
  467,
  "<?xml  version=\"1.0\"?>\r\n<!-- validity constraint check for Production 56(positive Test)-->\r\n<!DOCTYPE tokenizer\r\n [\r\n <!ELEMENT tokenizer ANY>\r\n <!ELEMENT b EMPTY>\r\n <!ATTLIST b attr ID #REQUIRED>\r\n <!ATTLIST tokenizer UniqueName ID #REQUIRED>\r\n  ]>\r\n<tokenizer UniqueName = \"Ac999\">\r\n<b attr = \"BC999\"></b>\r\nThis is a positive test for validity constraints\r\nthe value of the attribute with a type ID does not appear more than once in the XML document\r\n</tokenizer>\r\n",
  222,
  "<tokenizer UniqueName=\"Ac999\">&#10;<b attr=\"BC999\"></b>&#10;This is a positive test for validity constraints&#10;the value of the attribute with a type ID does not appear more than once in the XML document&#10;</tokenizer>"
},
{
  "ibm56v05.xml",
  449,
  "<?xml  version=\"1.0\"?>\r\n<!-- validity constraint check for Production 56(positive Test)-->\r\n<!DOCTYPE tokenizer\r\n [\r\n <!ELEMENT tokenizer ANY>\r\n <!ELEMENT a EMPTY>\r\n <!ELEMENT b EMPTY>\r\n <!ATTLIST a first ID #REQUIRED>\r\n <!ATTLIST b second ID #REQUIRED>\r\n ]>\r\n<tokenizer>\r\n<a first = \"AC1999\"></a>\r\n<b second = \"CD345\"></b>\r\nThis is a positive validity test for ID.\r\nany element type has no more than one attribute of type ID specified\r\n</tokenizer>",
  200,
  "<tokenizer>&#10;<a first=\"AC1999\"></a>&#10;<b second=\"CD345\"></b>&#10;This is a positive validity test for ID.&#10;any element type has no more than one attribute of type ID specified&#10;</tokenizer>"
},
{
  "ibm56v06.xml",
  572,
  "<?xml  version=\"1.0\"?>\r\n<!-- validity constraint check for Production 56(Positive Test)-->\r\n<!DOCTYPE test\r\n [\r\n <!ELEMENT test ANY>\r\n <!ELEMENT id EMPTY>\r\n <!ELEMENT idref EMPTY>\r\n <!ATTLIST id UniqueName ID #REQUIRED>\r\n <!ATTLIST idref reference IDREF #IMPLIED>\r\n ]>\r\n<test>\r\n<id UniqueName = \"AC456\"></id>\r\n<idref reference = \"AC456\"></idref>\r\nPositive test for validity constraint of IDREF.\r\nIn an attribute decl, values of type IDREF match tha name production\r\nand the IDREF value matches the value assigned to an ID attribute somewhere\r\nin the XML document.\r\n</test>",
  319,
  "<test>&#10;<id UniqueName=\"AC456\"></id>&#10;<idref reference=\"AC456\"></idref>&#10;Positive test for validity constraint of IDREF.&#10;In an attribute decl, values of type IDREF match tha name production&#10;and the IDREF value matches the value assigned to an ID attribute somewhere&#10;in the XML document.&#10;</test>"
},
{
  "ibm56v07.xml",
  673,
  "<?xml  version=\"1.0\"?>\r\n<!-- validity constraint check for Production 56(Positive Test)-->\r\n<!DOCTYPE test\r\n [\r\n <!ELEMENT test ANY>\r\n <!ELEMENT id1 EMPTY>\r\n <!ELEMENT id2 EMPTY>\r\n <!ELEMENT idref EMPTY>\r\n <!ATTLIST id1 UniqueName ID #REQUIRED>\r\n <!ATTLIST id2 UName ID #IMPLIED>\r\n <!ATTLIST idref reference IDREFS #IMPLIED>\r\n ]>\r\n<test>\r\n<id1 UniqueName = \"AC456\"></id1>\r\n<id2 UName = \"Q123\"></id2>\r\n<idref reference = \"AC456 Q123\"></idref>\r\nPositive test for validity constraint of IDREFS.\r\nIn an attribute decl, values of type IDREFS match tha name production\r\nand the IDREFS value matches the values assigned to an ID attributes somewhere\r\nin the XML document.\r\n</test>",
  360,
  "<test>&#10;<id1 UniqueName=\"AC456\"></id1>&#10;<id2 UName=\"Q123\"></id2>&#10;<idref reference=\"AC456 Q123\"></idref>&#10;Positive test for validity constraint of IDREFS.&#10;In an attribute decl, values of type IDREFS match tha name production&#10;and the IDREFS value matches the values assigned to an ID attributes somewhere&#10;in the XML document.&#10;</test>"
},
{
  "ibm56v08.xml",
  503,
  "<?xml  version=\"1.0\"?>\r\n<!-- validity constraint check for Production 56(Positive Test)it is a DTD-->\r\n<!DOCTYPE test\r\n [\r\n <!ELEMENT test ANY>\r\n <!ELEMENT landscape EMPTY>\r\n <!NOTATION gif PUBLIC \"gif\">\r\n <!ENTITY image SYSTEM \"testspec/images/sunset.gif\" NDATA gif>\r\n <!ATTLIST landscape sun ENTITY #IMPLIED>\r\n]>\r\n<test>\r\n<landscape sun = \"image\"></landscape>\r\nvalues of type ENTITY match the Name production and the ENTITY value\r\nmatches the name of an unparsed entity declared in the DTD.\r\n</test>\r\n",
  244,
  "<!DOCTYPE test [\n<!NOTATION gif PUBLIC 'gif'>\n]>\n<test>&#10;<landscape sun=\"image\"></landscape>&#10;values of type ENTITY match the Name production and the ENTITY value&#10;matches the name of an unparsed entity declared in the DTD.&#10;</test>"
},
{
  "ibm56v09.xml",
  361,
  "<?xml  version=\"1.0\"?>\r\n<!-- validity constraint check for Production 56(Positive Test)-->\r\n<!DOCTYPE test\r\n [\r\n <!ELEMENT test ANY>\r\n <!ELEMENT nametoken EMPTY>\r\n <!ATTLIST nametoken thistoken NMTOKEN #IMPLIED>\r\n]>\r\n<test>\r\n<nametoken thistoken = \"x:image\"></nametoken>\r\nIn an attribute declaration, values of type NMTOKEN match the Nmtoken production\r\n</test>",
  151,
  "<test>&#10;<nametoken thistoken=\"x:image\"></nametoken>&#10;In an attribute declaration, values of type NMTOKEN match the Nmtoken production&#10;</test>"
},
{
  "ibm56v10.xml",
  377,
  "<?xml  version=\"1.0\"?>\r\n<!-- validity constraint check for Production 56(Positive Test)-->\r\n<!DOCTYPE test\r\n [\r\n <!ELEMENT test ANY>\r\n <!ELEMENT nametokens EMPTY>\r\n <!ATTLIST nametokens thistoken NMTOKENS #IMPLIED>\r\n]>\r\n<test>\r\n<nametokens thistoken = \"x:lang y:country\"></nametokens>\r\nIn an attribute declaration, values of type NMTOKENS match the Nmtokens production\r\n</test>",
  164,
  "<test>&#10;<nametokens thistoken=\"x:lang y:country\"></nametokens>&#10;In an attribute declaration, values of type NMTOKENS match the Nmtokens production&#10;</test>"
},
{
  "ibm57v01.xml",
  370,
  "<?xml  version=\"1.0\"?>\r\n<!-- valid test for Production 57-->\r\n<!DOCTYPE root\r\n [\r\n <!ELEMENT root (#PCDATA|a|b)*>\r\n <!ELEMENT a ANY>\r\n <!ELEMENT b ANY>\r\n <!NOTATION a SYSTEM \"a\">\r\n <!NOTATION b SYSTEM \"b\"> \r\n <!ATTLIST a att (a|b) #IMPLIED>\r\n <!ATTLIST b att NOTATION (a|b) #IMPLIED>\r\n ]>\r\n <root>\r\nThis test case tests the kinds of enumerated types\r\n<a/><b/>\r\n</root>\r\n",
  162,
  "<!DOCTYPE root [\n<!NOTATION a SYSTEM 'a'>\n<!NOTATION b SYSTEM 'b'>\n]>\n<root>&#10;This test case tests the kinds of enumerated types&#10;<a></a><b></b>&#10;</root>"
},
{
  "ibm58v01.xml",
  618,
  "<?xml  version=\"1.0\"?>\r\n<!-- valid test for Production 58-->\r\n<!DOCTYPE test\r\n [\r\n <!ELEMENT test ANY>\r\n <!ELEMENT one ANY>\r\n <!ELEMENT two ANY>\r\n <!ELEMENT three ANY>\r\n <!ELEMENT four ANY>\r\n <!ELEMENT five ANY>\r\n <!NOTATION this SYSTEM \"alpha\">\r\n <!NOTATION that SYSTEM \"beta\">\r\n <!ATTLIST one attr NOTATION (this) #IMPLIED>\r\n <!ATTLIST two attr NOTATION ( this) #IMPLIED>\r\n <!ATTLIST three attr NOTATION (this|that) #IMPLIED>\r\n <!ATTLIST four attr NOTATION (that |this) #IMPLIED>\r\n <!ATTLIST five attr NOTATION ( that ) #IMPLIED>\r\n ]>\r\n <test>\r\nThis is a positive test with different patterns for NOTATION\r\n</test>\r\n",
  166,
  "<!DOCTYPE test [\n<!NOTATION that SYSTEM 'beta'>\n<!NOTATION this SYSTEM 'alpha'>\n]>\n<test>&#10;This is a positive test with different patterns for NOTATION&#10;</test>"
},
{
  "ibm58v02.xml",
  548,
  "<?xml  version=\"1.0\"?>\r\n<!-- validity test for Production 58-->\r\n<!DOCTYPE test\r\n [\r\n <!ELEMENT test ANY>\r\n <!ELEMENT blob (#PCDATA)>\r\n <!NOTATION base64 SYSTEM \"mimecode\">\r\n <!NOTATION uuencode SYSTEM \"uudecode\">\r\n <!NOTATION raw SYSTEM \"bin/cat\">\r\n <!ATTLIST blob content-encoding NOTATION (base64|uuencode|raw) #REQUIRED>\r\n ]>\r\n <test>\r\n<blob content-encoding=\"base64\"></blob>\r\nThe attribute values of type NOTATION matches one of the notation names included in the declaration;\r\nall notation names in the declaration have been declared\r\n</test>",
  357,
  "<!DOCTYPE test [\n<!NOTATION base64 SYSTEM 'mimecode'>\n<!NOTATION raw SYSTEM 'bin/cat'>\n<!NOTATION uuencode SYSTEM 'uudecode'>\n]>\n<test>&#10;<blob content-encoding=\"base64\"></blob>&#10;The attribute values of type NOTATION matches one of the notation names included in the declaration;&#10;all notation names in the declaration have been declared&#10;</test>"
},
{
  "ibm59v01.xml",
  476,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 59-->\r\n<!DOCTYPE test\r\n [\r\n <!ELEMENT test ANY>\r\n <!ELEMENT one EMPTY>\r\n <!ELEMENT two EMPTY>\r\n <!ELEMENT enum (#PCDATA)>\r\n <!ATTLIST one attr (one) #IMPLIED>\r\n <!ATTLIST two attr ( enum) #IMPLIED>\r\n <!ATTLIST two attr (one|two) #IMPLIED>\r\n <!ATTLIST two attr (one| two) #IMPLIED>\r\n <!ATTLIST two attr (enum ) #IMPLIED>\r\n <!ATTLIST two attr ( one | two | enum) #IMPLIED>\r\n ]>\r\n <test>\r\nThis is a Positive test\r\n</test>\r\n",
  46,
  "<test>&#10;This is a Positive test&#10;</test>"
},
{
  "ibm59v02.xml",
  385,
  "<?xml  version=\"1.0\"?>\r\n<!-- validity test for Production 59-->\r\n<!DOCTYPE test\r\n [\r\n <!ELEMENT test ANY>\r\n <!ELEMENT one EMPTY>\r\n <!ELEMENT two EMPTY>\r\n <!ELEMENT num EMPTY>\r\n <!ATTLIST num value (one|two) #IMPLIED>\r\n ]>\r\n <test>\r\n<num value = \"one\"></num>\r\nThis is a Positive test\r\nThe attribute values of type Enumeration match one of the Nmtoken tokens in the declaration.\r\n</test>",
  171,
  "<test>&#10;<num value=\"one\"></num>&#10;This is a Positive test&#10;The attribute values of type Enumeration match one of the Nmtoken tokens in the declaration.&#10;</test>"
},
{
  "ibm60v01.xml",
  567,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 60-->\r\n<!DOCTYPE Java \r\n [\r\n <!ELEMENT Java (#PCDATA|one|two|three|four)*>\r\n <!ELEMENT one EMPTY>\r\n <!ELEMENT two EMPTY>\r\n <!ELEMENT three EMPTY>\r\n <!ELEMENT four EMPTY>\r\n <!ATTLIST one chapter CDATA #IMPLIED>\r\n <!ATTLIST two chapter CDATA #REQUIRED>\r\n <!ATTLIST three chapter CDATA #FIXED \"JavaBeans\">\r\n <!ATTLIST four chapter CDATA 'defualt'>\r\n ]>\r\n<Java><one chapter=\"Introduction\"/>\r\n <three chapter=\"JavaBeans\"/>\r\n Positive test\r\n DefaultDecl attributes values IMPLIED, REQUIRED, FIXED and default\r\n</Java>",
  184,
  "<Java><one chapter=\"Introduction\"></one>&#10; <three chapter=\"JavaBeans\"></three>&#10; Positive test&#10; DefaultDecl attributes values IMPLIED, REQUIRED, FIXED and default&#10;</Java>"
},
{
  "ibm60v02.xml",
  477,
  "<?xml  version=\"1.0\"?>\r\n<!-- validity test for Production 60-->\r\n<!DOCTYPE Java \r\n [\r\n <!ELEMENT Java ANY>\r\n <!ELEMENT one EMPTY>\r\n <!ELEMENT two EMPTY>\r\n <!ATTLIST one chapter CDATA #REQUIRED>\r\n <!ATTLIST two chapter CDATA #REQUIRED>\r\n ]>\r\n<Java>\r\n<one chapter=\"Introduction\"></one>\r\n<two chapter=\"JavaApplets\"></two>\r\nPositive test. Required attribute. Every occurrence of an element with a \r\n#REQUIRED attribute default declaration gives the value of that attribute\r\n</Java>",
  251,
  "<Java>&#10;<one chapter=\"Introduction\"></one>&#10;<two chapter=\"JavaApplets\"></two>&#10;Positive test. Required attribute. Every occurrence of an element with a &#10;#REQUIRED attribute default declaration gives the value of that attribute&#10;</Java>"
},
{
  "ibm60v03.xml",
  435,
  "<?xml  version=\"1.0\"?>\r\n<!-- validity test for Production 60-->\r\n<!DOCTYPE Java \r\n [\r\n <!ELEMENT Java ANY>\r\n <!ELEMENT one EMPTY>\r\n <!ATTLIST one chapter CDATA #FIXED \"Introduction\">\r\n  ]>\r\n<Java>\r\n<one chapter=\"Introduction\"></one>\r\nAn attribute has a default value declared with the #FIXED keyword, \r\nand an instances of that attribute is given a value which is exactly \r\nthe same as the default value in the declaration. \r\n</Java>\r\n",
  258,
  "<Java>&#10;<one chapter=\"Introduction\"></one>&#10;An attribute has a default value declared with the #FIXED keyword, &#10;and an instances of that attribute is given a value which is exactly &#10;the same as the default value in the declaration. &#10;</Java>"
},
{
  "ibm60v04.xml",
  421,
  "<?xml  version=\"1.0\"?>\r\n<!-- validity test for Production 60-->\r\n<!DOCTYPE test\r\n [\r\n <!ELEMENT test ANY>\r\n <!ELEMENT a EMPTY>\r\n <!ELEMENT b EMPTY>\r\n <!ELEMENT attr EMPTY>\r\n <!ELEMENT nametoken EMPTY>\r\n <!ATTLIST attr value (a|b) \"a\"> \r\n <!ATTLIST nametoken namevalue NMTOKEN \"hello\">\r\n  ]>\r\n<test>\r\nThe default value specified for an attribute meets the \r\nlexical constraints of the declared attribute type.\r\n</test>\r\n\r\n",
  134,
  "<test>&#10;The default value specified for an attribute meets the &#10;lexical constraints of the declared attribute type.&#10;</test>"
},
{
  "ibm61v01.xml",
  137,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 61-->\r\n<!DOCTYPE animal SYSTEM \"ibm61v01.dtd\">\r\n<animal>\r\n <tiger/>\r\n</animal>\r\n ",
  43,
  "<animal>&#10; <tiger></tiger>&#10;</animal>"
},
{
  "ibm61v02.xml",
  151,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 61-->\r\n<!DOCTYPE animal SYSTEM \"ibm61v02.dtd\"\r\n[\r\n<!ELEMENT animal EMPTY>\r\n]>\r\n<animal/>\r\n\r\n \r\n",
  17,
  "<animal></animal>"
},
{
  "ibm62v01.xml",
  205,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 62-->\r\n<!DOCTYPE animal SYSTEM \"ibm62v01.dtd\">\r\n<animal>\r\n <tiger/>\r\nPositive test. Test includeSect with pattern1 of p62.\r\nNormal Pattern\r\n</animal>",
  120,
  "<animal>&#10; <tiger></tiger>&#10;Positive test. Test includeSect with pattern1 of p62.&#10;Normal Pattern&#10;</animal>"
},
{
  "ibm62v02.xml",
  222,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 62-->\r\n<!DOCTYPE animal SYSTEM \"ibm62v02.dtd\">\r\n<animal>\r\n <tiger/>\r\nPositive test. Test includeSect with pattern2 of p62.\r\nspace included before INCLUDE\r\n</animal>\r\n",
  135,
  "<animal>&#10; <tiger></tiger>&#10;Positive test. Test includeSect with pattern2 of p62.&#10;space included before INCLUDE&#10;</animal>"
},
{
  "ibm62v03.xml",
  221,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 62-->\r\n<!DOCTYPE animal SYSTEM \"ibm62v03.dtd\">\r\n<animal>\r\n <tiger/>\r\nPositive test. Test includeSect with pattern3 of p62.\r\nspace included after INCLUDE\r\n</animal>\r\n",
  134,
  "<animal>&#10; <tiger></tiger>&#10;Positive test. Test includeSect with pattern3 of p62.&#10;space included after INCLUDE&#10;</animal>"
},
{
  "ibm62v04.xml",
  232,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 62-->\r\n<!DOCTYPE animal SYSTEM \"ibm62v04.dtd\">\r\n<animal>\r\n <tiger/>\r\nPositive test. Test includeSect with pattern4 of p62.\r\nspace included before and after INCLUDE\r\n</animal>\r\n",
  145,
  "<animal>&#10; <tiger></tiger>&#10;Positive test. Test includeSect with pattern4 of p62.&#10;space included before and after INCLUDE&#10;</animal>"
},
{
  "ibm62v05.xml",
  242,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 62-->\r\n<!DOCTYPE animal SYSTEM \"ibm62v05.dtd\"\r\n[\r\n<!ELEMENT animal ANY>\r\n<!ELEMENT tiger EMPTY>\r\n]>\r\n\r\n<animal>\r\n <tiger/>\r\nPositive test. Missing external subset declaration.\r\n</animal>",
  99,
  "<animal>&#10; <tiger></tiger>&#10;Positive test. Missing external subset declaration.&#10;</animal>"
},
{
  "ibm63v01.xml",
  280,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 63-->\r\n<!DOCTYPE animal SYSTEM \"ibm63v01.dtd\"\r\n[\r\n<!ELEMENT animal ANY>\r\n<!ELEMENT tiger (#PCDATA)>\r\n<!ATTLIST animal a (tiger) #REQUIRED> \r\n]>\r\n<animal a = \"tiger\">\r\nPositive test. Test for IGNORE with pattern 1.\r\n</animal>",
  83,
  "<animal a=\"tiger\">&#10;Positive test. Test for IGNORE with pattern 1.&#10;</animal>"
},
{
  "ibm63v02.xml",
  280,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 63-->\r\n<!DOCTYPE animal SYSTEM \"ibm63v02.dtd\"\r\n[\r\n<!ELEMENT animal ANY>\r\n<!ELEMENT tiger (#PCDATA)>\r\n<!ATTLIST animal a (tiger) #REQUIRED> \r\n]>\r\n<animal a = \"tiger\">\r\nPositive test. Test for IGNORE with pattern 2.\r\n</animal>",
  83,
  "<animal a=\"tiger\">&#10;Positive test. Test for IGNORE with pattern 2.&#10;</animal>"
},
{
  "ibm63v03.xml",
  280,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 63-->\r\n<!DOCTYPE animal SYSTEM \"ibm63v03.dtd\"\r\n[\r\n<!ELEMENT animal ANY>\r\n<!ELEMENT tiger (#PCDATA)>\r\n<!ATTLIST animal a (tiger) #REQUIRED> \r\n]>\r\n<animal a = \"tiger\">\r\nPositive test. Test for IGNORE with pattern 3.\r\n</animal>",
  83,
  "<animal a=\"tiger\">&#10;Positive test. Test for IGNORE with pattern 3.&#10;</animal>"
},
{
  "ibm63v04.xml",
  280,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 63-->\r\n<!DOCTYPE animal SYSTEM \"ibm63v04.dtd\"\r\n[\r\n<!ELEMENT animal ANY>\r\n<!ELEMENT tiger (#PCDATA)>\r\n<!ATTLIST animal a (tiger) #REQUIRED> \r\n]>\r\n<animal a = \"tiger\">\r\nPositive test. Test for IGNORE with pattern 4.\r\n</animal>",
  83,
  "<animal a=\"tiger\">&#10;Positive test. Test for IGNORE with pattern 4.&#10;</animal>"
},
{
  "ibm63v05.xml",
  280,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 63-->\r\n<!DOCTYPE animal SYSTEM \"ibm63v05.dtd\"\r\n[\r\n<!ELEMENT animal ANY>\r\n<!ELEMENT tiger (#PCDATA)>\r\n<!ATTLIST animal a (tiger) #REQUIRED> \r\n]>\r\n<animal a = \"tiger\">\r\nPositive test. Test for IGNORE with pattern 5.\r\n</animal>",
  83,
  "<animal a=\"tiger\">&#10;Positive test. Test for IGNORE with pattern 5.&#10;</animal>"
},
{
  "ibm64v01.xml",
  177,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 64-->\r\n<!DOCTYPE animal SYSTEM \"ibm64v01.dtd\"\r\n[\r\n<!ELEMENT animal ANY>\r\n]>\r\n<animal>\r\nPositive Test. Pattern1\r\n</animal>",
  50,
  "<animal>&#10;Positive Test. Pattern1&#10;</animal>"
},
{
  "ibm64v02.xml",
  177,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 64-->\r\n<!DOCTYPE animal SYSTEM \"ibm64v02.dtd\"\r\n[\r\n<!ELEMENT animal ANY>\r\n]>\r\n<animal>\r\nPositive Test. Pattern2\r\n</animal>",
  50,
  "<animal>&#10;Positive Test. Pattern2&#10;</animal>"
},
{
  "ibm64v03.xml",
  177,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 64-->\r\n<!DOCTYPE animal SYSTEM \"ibm64v03.dtd\"\r\n[\r\n<!ELEMENT animal ANY>\r\n]>\r\n<animal>\r\nPositive Test. Pattern3\r\n</animal>",
  50,
  "<animal>&#10;Positive Test. Pattern3&#10;</animal>"
},
{
  "ibm65v01.xml",
  192,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 65-->\r\n<!DOCTYPE animal SYSTEM \"ibm65v01.dtd\"\r\n[\r\n<!ELEMENT animal ANY>\r\n]>\r\n<animal>\r\nPositive Test. Pattern1. Empty string.\r\n</animal>",
  65,
  "<animal>&#10;Positive Test. Pattern1. Empty string.&#10;</animal>"
},
{
  "ibm65v02.xml",
  178,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 65-->\r\n<!DOCTYPE animal SYSTEM \"ibm65v02.dtd\"\r\n[\r\n<!ELEMENT animal ANY>\r\n]>\r\n<animal>\r\nPositive Test. Pattern2.\r\n</animal>",
  51,
  "<animal>&#10;Positive Test. Pattern2.&#10;</animal>"
},
{
  "ibm66v01.xml",
  363,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n  <!ELEMENT root (#PCDATA)>\r\n]>\r\n<root>\r\nTest all valid Charater references for P66:\r\n&#9;&#09;&#0000000009;\r\n&#xA;&#xa;&#x0A;&#x00000000A;\r\n&#x0d;\r\n&#xAB; &#xab; &#xCD; &#xcD; &#xEf; &#xef;\r\n&#67; &#x43; &#x5f;\r\n&#x20; &#xD7A3; &#xAC00;\r\n&#xF900; &#xFFFD;\r\n&#x10000; &#x10FFFF;\r\n</root>\r\n<!--* a valid test for P66 *-->\r\n",
  190,
  "<root>&#10;Test all valid Charater references for P66:&#10;&#9;&#9;&#9;&#10;&#10;&#10;&#10;&#10;&#10;&#13;&#10;\302\253 \302\253 \303\215 \303\215 \303\257 \303\257\46\06110;C C _&#10;  \355\236\243 \352\260\200\46\06110;\357\244\200 \357\277\275\46\06110;\360\220\200\200 \364\217\277\277\46\06110;</root>"
},
{
  "ibm67v01.xml",
  218,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n  <!ELEMENT root (#PCDATA)>\r\n  <!ATTLIST root attr CDATA #REQUIRED>\r\n  <!ENTITY ge1 \"xyz\">\r\n]>\r\n<root attr=\"&ge1;&#65;\">\r\n&ge1; &#66;\r\n</root>\r\n<!--* a valid test for P67 *-->\r\n",
  40,
  "<root attr=\"xyzA\">&#10;xyz B&#10;</root>"
},
{
  "ibm68v01.xml",
  207,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root SYSTEM \"ibm68v01.dtd\" [\r\n  <!ELEMENT root (#PCDATA|a)* >\r\n]>\r\n<root>\r\n  pcdata content\r\n  <a attr1=\"xyz\"/>\r\n</root>\r\n<!--* a valid test for P68 VC:Entity Declared *-->\r\n",
  65,
  "<root>&#10;  pcdata content&#10;  <a attr1=\"xyz\"></a>&#10;</root>"
},
{
  "ibm68v02.xml",
  229,
  "<?xml version=\"1.0\" standalone='no'?>\r\n<!DOCTYPE root [\r\n  <!ELEMENT root (#PCDATA)* >\r\n  <!ENTITY % pe1 SYSTEM \"ibm68v02.ent\">\r\n  %pe1;\r\n]>\r\n<root>\r\n  pcdata content\r\n</root>\r\n<!--* a valid test for P68 VC:Entity Declared *-->\r\n",
  39,
  "<root>&#10;  pcdata content&#10;</root>"
},
{
  "ibm69v01.xml",
  277,
  "<?xml version=\"1.0\" standalone=\"no\" ?>\r\n<!DOCTYPE root SYSTEM \"ibm69v01.dtd\" [\r\n  <!ELEMENT root (#PCDATA|a)* >\r\n  <!ENTITY % pe1 \"<!-- comment in PE -->\">\r\n  %pe1;\r\n]>\r\n<root>\r\n  pcdata content\r\n  <a attr1=\"xyz\"/>\r\n</root>\r\n<!--* a valid test for P69 VC:Entity Declared *-->\r\n",
  65,
  "<root>&#10;  pcdata content&#10;  <a attr1=\"xyz\"></a>&#10;</root>"
},
{
  "ibm69v02.xml",
  229,
  "<?xml version=\"1.0\" standalone='no'?>\r\n<!DOCTYPE root [\r\n  <!ELEMENT root (#PCDATA)* >\r\n  <!ENTITY % pe1 SYSTEM \"ibm69v02.ent\">\r\n  %pe1;\r\n]>\r\n<root>\r\n  pcdata content\r\n</root>\r\n<!--* a valid test for P69 VC:Entity Declared *-->\r\n",
  39,
  "<root>&#10;  pcdata content&#10;</root>"
},
{
  "ibm70v01.xml",
  478,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n\r\n<!--* Test all legal patterns derived from P70-76 *-->\r\n<!ENTITY % pe1 '<!ATTLIST root att2 CDATA \"&ge1;\">'>\r\n<!ENTITY ge1 \"attdefaultvalue\" >\r\n%pe1;\r\n<!NOTATION JPGformat SYSTEM \"JPGFormat\">\r\n<!ENTITY ge2  SYSTEM \"image.jpg\" NDATA JPGformat>\r\n<!ENTITY % pe2 PUBLIC \"-//w3c//any\" \"ibm70v01.ent\" >\r\n%pe2;\r\n]>\r\n<root att2=\"any\" />\r\n<!-- a valid test case: test P70-P76 -->",
  85,
  "<!DOCTYPE root [\n<!NOTATION JPGformat SYSTEM 'JPGFormat'>\n]>\n<root att2=\"any\"></root>"
},
{
  "ibm78v01.xml",
  365,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n\r\n<!--* Test all legal patterns derived from P78 *-->\r\n<!ENTITY epe1 SYSTEM \"ibm78v01.ent\" >\r\n<!ENTITY epe2 SYSTEM \"ibm78v02.ent\" >\r\n<!ENTITY epe3 SYSTEM \"ibm78v03.ent\" >\r\n\r\n]>\r\n<root>&epe1;&epe2;&epe3;</root>\r\n<!-- a valid test case: test P78, P77, P80 -->\r\n",
  206,
  "<root>&#10;anything legal as PCDATA....&#10;N.B. also tested P77 and P80&#10;anything legal as PCDATA....&#10;N.B. also tested P77anything legal as PCDATA....&#10;e.g. 12345678E-33, &quot;hello&quot;</root>"
},
{
  "ibm79v01.xml",
  250,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE book \r\n[\r\n<!ELEMENT book ANY>\r\n<!ATTLIST notebook att CDATA #IMPLIED>\r\n<!ENTITY % epe SYSTEM \"ibm79v01.ent\" >\r\n%epe;\r\n]>\r\n<book><name>XML Handbook</name> This is a book</book>\r\n\r\n<!-- a valid test case: test P79 -->\r\n",
  53,
  "<book><name>XML Handbook</name> This is a book</book>"
},
{
  "ibm82v01.xml",
  397,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!ATTLIST root entatt1 ENTITY #REQUIRED >\r\n\r\n<!--* Test PublicID in P82 *-->\r\n<!NOTATION JPGformat PUBLIC \"-//image//notreal\" >\r\n<!ENTITY unparsed1  SYSTEM \"image.jpg\" NDATA JPGformat>\r\n]>\r\n<root entatt1=\"unparsed1\">test PublicID in P82</root>\r\n<!-- a valid test case: test P82 and P83 -->\r\n",
  122,
  "<!DOCTYPE root [\n<!NOTATION JPGformat PUBLIC '-//image//notreal'>\n]>\n<root entatt1=\"unparsed1\">test PublicID in P82</root>"
},
{
  "ibm85v01.xml",
  4486,
  "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\r\n<!DOCTYPE book [\r\n<!ELEMENT book ANY>\r\n<!-- This test case covers 149 legal character ranges plus\r\n     51 discrete legal characters for production 85. -->\r\n<?NAME_41-A_5A-Z_4d-M_61-a_7A-z_6d-m_0C0-\303\200\0600D6-\303\226\0600cb-\303\213\0600D8-\303\230\0600F6-\303\266\0600e7-\303\247\0600F8-\303\270\0600FF-\303\277\0600fb-\303\273\061100-\304\200\061131-\304\261\061118-\304\230\061134-\304\264\06113E-\304\276\061139-\304\271\061141-\305\201\061148-\305\210\061144-\305\204\06114A-\305\212\06117E-\305\276\061164-\305\244\061180-\306\200\0611C3-\307\203\0611a1-\306\241\0611CD-\307\215\0611F0-\307\260\0611de-\307\236\0611F4-\307\264\0611F5-\307\265\0611f4-\307\264\0611FA-\307\272\062217-\310\227\062208-\310\210\062250-\311\220\0622A8-\312\250\06227c-\311\274\0622BB-\312\273\0622C1-\313\201\0622be-\312\276\063386-\316\206\063388-\316\210\06338A-\316\212\063389-\316\211\06338C-\316\214\06338E-\316\216\0633A1-\316\241\063397-\316\227\0633A3-\316\243\0633CE-\317\216\0633b8-\316\270\0633D0-\317\220\0633D6-\317\226\0633d3-\317\223\0633DA-\317\232\0633DC-\317\234\0633DE-\317\236\0633E0-\317\240\0633E2-\317\242\0633F3-\317\263\0633ea-\317\252\064401-\320\201\06440C-\320\214\064406-\320\206\06440E-\320\216\06444F-\321\217\06442e-\320\256\064451-\321\221\06445C-\321\234\064456-\321\226\06445E-\321\236\064481-\322\201\06446f-\321\257\064490-\322\220\0644C4-\323\204\0644aa-\322\252\0644C7-\323\207\0644C8-\323\210\0644c7-\323\207\0644CB-\323\213\0644CC-\323\214\0644cb-\323\213\0644D0-\323\220\0644EB-\323\253\0644dd-\323\235\0644EE-\323\256\0644F5-\323\265\0644f1-\323\261\0644F8-\323\270\0644F9-\323\271\0644f8-\323\270\065531-\324\261\065556-\325\226\065543-\325\203\065559-\325\231\065561-\325\241\065586-\326\206\065573-\325\263\0655D0-\327\220\0655EA-\327\252\0655dd-\327\235\0655F0-\327\260\0655F2-\327\262\0655f1-\327\261\066621-\330\241\06663A-\330\272\06662d-\330\255\066641-\331\201\06664A-\331\212\066645-\331\205\066671-\331\261\0666B7-\332\267\066694-\332\224\0666BA-\332\272\0666BE-\332\276\0666bc-\332\274\0666C0-\333\200\0666CE-\333\216\0666c7-\333\207\0666D0-\333\220\0666D3-\333\223\0666d1-\333\221\0666D5-\333\225\0666E5-\333\245\0666E6-\333\246\0666e5-\333\245\0600905-\340\244\205\0600939-\340\244\271\060091f-\340\244\237\060093D-\340\244\275\0600958-\340\245\230\0600961-\340\245\241\060095c-\340\245\234\0600985-\340\246\205\060098C-\340\246\214\0600988-\340\246\210\060098F-\340\246\217\0600990-\340\246\220\060098f-\340\246\217\0600993-\340\246\223\06009A8-\340\246\250\060099d-\340\246\235\06009AA-\340\246\252\06009B0-\340\246\260\06009ad-\340\246\255\06009B2-\340\246\262\06009B6-\340\246\266\06009B9-\340\246\271\06009b7-\340\246\267\06009DC-\340\247\234\06009DD-\340\247\235\06009dc-\340\247\234\06009DF-\340\247\237\06009E1-\340\247\241\06009e0-\340\247\240\06009F0-\340\247\260\06009F1-\340\247\261\06009f0-\340\247\260\0600A05-\340\250\205\0600A0A-\340\250\212\0600a07-\340\250\207\0600A0F-\340\250\217\0600A10-\340\250\220\0600a0f-\340\250\217\0600A13-\340\250\223\0600A28-\340\250\250\0600a1d-\340\250\235\0600A2A-\340\250\252\0600A30-\340\250\260\0600a2d-\340\250\255\0600A32-\340\250\262\0600A33-\340\250\263\0600a32-\340\250\262\0600A35-\340\250\265\0600A36-\340\250\266\0600a35-\340\250\265\0600A38-\340\250\270\0600A39-\340\250\271\0600a38-\340\250\270\0600A59-\340\251\231\0600A5C-\340\251\234\0600a5a-\340\251\232\0600A5E-\340\251\236\0600A72-\340\251\262\0600A74-\340\251\264\0600a73-\340\251\263\0600A85-\340\252\205\0600A8B-\340\252\213\0600a88-\340\252\210\0600A8D-\340\252\215\0600A8F-\340\252\217\0600A91-\340\252\221\0600a90-\340\252\220\0600A93-\340\252\223\0600AA8-\340\252\250\0600a9d-\340\252\235\0600AAA-\340\252\252\0600AB0-\340\252\260\0600aad-\340\252\255\0600AB2-\340\252\262\0600AB3-\340\252\263\0600ab2-\340\252\262\0600AB5-\340\252\265\0600AB9-\340\252\271\0600ab7-\340\252\267\0600ABD-\340\252\275\0600AE0-\340\253\240\0600B05-\340\254\205\0600B0C-\340\254\214\0600b08-\340\254\210\0600B0F-\340\254\217\0600B10-\340\254\220\0600b0f-\340\254\217\0600B13-\340\254\223\0600B28-\340\254\250\0600b1d-\340\254\235\0600B2A-\340\254\252\0600B30-\340\254\260\0600b2d-\340\254\255\0600B32-\340\254\262\0600B33-\340\254\263\0600b32-\340\254\262\0600B36-\340\254\266\0600B39-\340\254\271\0600b37-\340\254\267\0600B3D-\340\254\275\0600B5C-\340\255\234\0600B5D-\340\255\235\0600b5c-\340\255\234\0600B5F-\340\255\237\0600B61-\340\255\241\0600b60-\340\255\240\0600B85-\340\256\205\0600B8A-\340\256\212\0600b87-\340\256\207\0600B8E-\340\256\216\0600B90-\340\256\220\0600b8f-\340\256\217\0600B92-\340\256\222\0600B95-\340\256\225\0600b93-\340\256\223\0600B99-\340\256\231\0600B9A-\340\256\232\0600b99-\340\256\231\0600B9C-\340\256\234\0600B9E-\340\256\236\0600B9F-\340\256\237\0600b9e-\340\256\236\0600BA3-\340\256\243\0600BA4-\340\256\244\0600ba3-\340\256\243\0600BA8-\340\256\250\0600BAA-\340\256\252\0600ba9-\340\256\251\0600BAE-\340\256\256\0600BB5-\340\256\265\0600bb1-\340\256\261\0600BB7-\340\256\267\0600BB9-\340\256\271\0600bb8-\340\256\270\0600C05-\340\260\205\0600C0C-\340\260\214\0600c08-\340\260\210\0600C0E-\340\260\216\0600C10-\340\260\220\0600c0f-\340\260\217\0600C12-\340\260\222\0600C28-\340\260\250\0600c1d-\340\260\235\0600C2A-\340\260\252\0600C33-\340\260\263\0600c2e-\340\260\256\0600C35-\340\260\265\0600C39-\340\260\271\0600c37-\340\260\267\0600C60-\340\261\240\0600C61-\340\261\241\0600c60-\340\261\240\0600C85-\340\262\205\0600C8C-\340\262\214\0600c88-\340\262\210\0600C8E-\340\262\216\0600C90-\340\262\220\0600c8f-\340\262\217\0600C92-\340\262\222\0600CA8-\340\262\250\0600c9d-\340\262\235\0600CAA-\340\262\252\0600CB3-\340\262\263\0600cae-\340\262\256\0600CB5-\340\262\265\0600CB9-\340\262\271\0600cb7-\340\262\267\0600CDE-\340\263\236\0600CE0-\340\263\240\0600CE1-\340\263\241\0600ce0-\340\263\240\0600D05-\340\264\205\0600D0C-\340\264\214\0600d08-\340\264\210\0600D0E-\340\264\216\0600D10-\340\264\220\0600d0f-\340\264\217\0600D12-\340\264\222\0600D28-\340\264\250\0600d1d-\340\264\235\0600D2A-\340\264\252\0600D39-\340\264\271\0600d31-\340\264\261\0600D60-\340\265\240\0600D61-\340\265\241\0600d60-\340\265\240\0600E01-\340\270\201\0600E2E-\340\270\256\0600e17-\340\270\227\0600E30-\340\270\260\0600E32-\340\270\262\0600E33-\340\270\263\0600e32-\340\270\262\0600E40-\340\271\200\0600E45-\340\271\205\0600e42-\340\271\202\0600E81-\340\272\201\0600E82-\340\272\202\0600e81-\340\272\201\0600E84-\340\272\204\0600E87-\340\272\207\0600E88-\340\272\210\0600e87-\340\272\207\0600E8A-\340\272\212\0600E8D-\340\272\215\0600E94-\340\272\224\0600E97-\340\272\227\0600e95-\340\272\225\0600E99-\340\272\231\0600E9F-\340\272\237\0600e9c-\340\272\234\0600EA1-\340\272\241\0600EA3-\340\272\243\0600ea2-\340\272\242\0600EA5-\340\272\245\0600EA7-\340\272\247\0600EAA-\340\272\252\0600EAB-\340\272\253\0600eaa-\340\272\252\0600EAD-\340\272\255\0600EAE-\340\272\256\0600ead-\340\272\255\0600EB0-\340\272\260\0600EB2-\340\272\262\0600EB3-\340\272\263\0600eb2-\340\272\262\0600EBD-\340\272\275\0600EC0-\340\273\200\0600EC4-\340\273\204\0600ec2-\340\273\202\0600F40-\340\275\200\0600F47-\340\275\207\0600f43-\340\275\203\0600F49-\340\275\211\0600F69-\340\275\251\0600f59-\340\275\231\06110A0-\341\202\240\06110C5-\341\203\205\06110b2-\341\202\262\06110D0-\341\203\220\06110F6-\341\203\266\06110e3-\341\203\243\0611100-\341\204\200\0611102-\341\204\202\0611103-\341\204\203\0611102-\341\204\202\0611105-\341\204\205\0611107-\341\204\207\0611106-\341\204\206\0611109-\341\204\211\061110B-\341\204\213\061110C-\341\204\214\061110b-\341\204\213\061110E-\341\204\216\0611112-\341\204\222\0611110-\341\204\220\061113C-\341\204\274\061113E-\341\204\276\0611140-\341\205\200\061114C-\341\205\214\061114E-\341\205\216\0611150-\341\205\220\0611154-\341\205\224\0611155-\341\205\225\0611154-\341\205\224\0611159-\341\205\231\061115F-\341\205\237\0611161-\341\205\241\0611160-\341\205\240\0611163-\341\205\243\0611165-\341\205\245\0611167-\341\205\247\0611169-\341\205\251\061116D-\341\205\255\061116E-\341\205\256\061116d-\341\205\255\0611172-\341\205\262\0611173-\341\205\263\0611172-\341\205\262\0611175-\341\205\265\061119E-\341\206\236\06111A8-\341\206\250\06111AB-\341\206\253\06111AE-\341\206\256\06111AF-\341\206\257\06111ae-\341\206\256\06111B7-\341\206\267\06111B8-\341\206\270\06111b7-\341\206\267\06111BA-\341\206\272\06111BC-\341\206\274\06111C2-\341\207\202\06111bf-\341\206\277\06111EB-\341\207\253\06111F0-\341\207\260\06111F9-\341\207\271\0611E00-\341\270\200\0611E9B-\341\272\233\0611e4d-\341\271\215\0611EA0-\341\272\240\0611EF9-\341\273\271\0611ecc-\341\273\214\0611F00-\341\274\200\0611F15-\341\274\225\0611f0a-\341\274\212\0611F18-\341\274\230\0611F1D-\341\274\235\0611f1a-\341\274\232\0611F20-\341\274\240\0611F45-\341\275\205\0611f32-\341\274\262\0611F48-\341\275\210\0611F4D-\341\275\215\0611f4a-\341\275\212\0611F50-\341\275\220\0611F57-\341\275\227\0611f53-\341\275\223\0611F59-\341\275\231\0611F5B-\341\275\233\0611F5D-\341\275\235\0611F5F-\341\275\237\0611F7D-\341\275\275\0611f6e-\341\275\256\0611F80-\341\276\200\0611FB4-\341\276\264\0611f9a-\341\276\232\0611FB6-\341\276\266\0611FBC-\341\276\274\0611fb9-\341\276\271\0611FBE-\341\276\276\0611FC2-\341\277\202\0611FC4-\341\277\204\0611fc3-\341\277\203\0611FC6-\341\277\206\0611FCC-\341\277\214\0611fc9-\341\277\211\0611FD0-\341\277\220\0611FD3-\341\277\223\0611fd1-\341\277\221\0611FD6-\341\277\226\0611FDB-\341\277\233\0611fd8-\341\277\230\0611FE0-\341\277\240\0611FEC-\341\277\254\0611fe6-\341\277\246\0611FF2-\341\277\262\0611FF4-\341\277\264\0611ff3-\341\277\263\0611FF6-\341\277\266\0611FFC-\341\277\274\0611ff9-\341\277\271\0622126-\342\204\246\062212A-\342\204\252\062212B-\342\204\253\062212a-\342\204\252\062212E-\342\204\256\0622180-\342\206\200\0622182-\342\206\202\0622181-\342\206\201\0633041-\343\201\201\0633094-\343\202\224\063306a-\343\201\252\06330A1-\343\202\241\06330FA-\343\203\272\06330cd-\343\203\215\0633105-\343\204\205\063312C-\343\204\254\0633118-\343\204\230\137\101\06000-\352\260\200\137\0677A3-\355\236\243\137\0611d1-\354\207\221 This is a PI target ?>\r\n]>\r\n<book/>\r\n",
  4286,
  "<?NAME_41-A_5A-Z_4d-M_61-a_7A-z_6d-m_0C0-\303\200\0600D6-\303\226\0600cb-\303\213\0600D8-\303\230\0600F6-\303\266\0600e7-\303\247\0600F8-\303\270\0600FF-\303\277\0600fb-\303\273\061100-\304\200\061131-\304\261\061118-\304\230\061134-\304\264\06113E-\304\276\061139-\304\271\061141-\305\201\061148-\305\210\061144-\305\204\06114A-\305\212\06117E-\305\276\061164-\305\244\061180-\306\200\0611C3-\307\203\0611a1-\306\241\0611CD-\307\215\0611F0-\307\260\0611de-\307\236\0611F4-\307\264\0611F5-\307\265\0611f4-\307\264\0611FA-\307\272\062217-\310\227\062208-\310\210\062250-\311\220\0622A8-\312\250\06227c-\311\274\0622BB-\312\273\0622C1-\313\201\0622be-\312\276\063386-\316\206\063388-\316\210\06338A-\316\212\063389-\316\211\06338C-\316\214\06338E-\316\216\0633A1-\316\241\063397-\316\227\0633A3-\316\243\0633CE-\317\216\0633b8-\316\270\0633D0-\317\220\0633D6-\317\226\0633d3-\317\223\0633DA-\317\232\0633DC-\317\234\0633DE-\317\236\0633E0-\317\240\0633E2-\317\242\0633F3-\317\263\0633ea-\317\252\064401-\320\201\06440C-\320\214\064406-\320\206\06440E-\320\216\06444F-\321\217\06442e-\320\256\064451-\321\221\06445C-\321\234\064456-\321\226\06445E-\321\236\064481-\322\201\06446f-\321\257\064490-\322\220\0644C4-\323\204\0644aa-\322\252\0644C7-\323\207\0644C8-\323\210\0644c7-\323\207\0644CB-\323\213\0644CC-\323\214\0644cb-\323\213\0644D0-\323\220\0644EB-\323\253\0644dd-\323\235\0644EE-\323\256\0644F5-\323\265\0644f1-\323\261\0644F8-\323\270\0644F9-\323\271\0644f8-\323\270\065531-\324\261\065556-\325\226\065543-\325\203\065559-\325\231\065561-\325\241\065586-\326\206\065573-\325\263\0655D0-\327\220\0655EA-\327\252\0655dd-\327\235\0655F0-\327\260\0655F2-\327\262\0655f1-\327\261\066621-\330\241\06663A-\330\272\06662d-\330\255\066641-\331\201\06664A-\331\212\066645-\331\205\066671-\331\261\0666B7-\332\267\066694-\332\224\0666BA-\332\272\0666BE-\332\276\0666bc-\332\274\0666C0-\333\200\0666CE-\333\216\0666c7-\333\207\0666D0-\333\220\0666D3-\333\223\0666d1-\333\221\0666D5-\333\225\0666E5-\333\245\0666E6-\333\246\0666e5-\333\245\0600905-\340\244\205\0600939-\340\244\271\060091f-\340\244\237\060093D-\340\244\275\0600958-\340\245\230\0600961-\340\245\241\060095c-\340\245\234\0600985-\340\246\205\060098C-\340\246\214\0600988-\340\246\210\060098F-\340\246\217\0600990-\340\246\220\060098f-\340\246\217\0600993-\340\246\223\06009A8-\340\246\250\060099d-\340\246\235\06009AA-\340\246\252\06009B0-\340\246\260\06009ad-\340\246\255\06009B2-\340\246\262\06009B6-\340\246\266\06009B9-\340\246\271\06009b7-\340\246\267\06009DC-\340\247\234\06009DD-\340\247\235\06009dc-\340\247\234\06009DF-\340\247\237\06009E1-\340\247\241\06009e0-\340\247\240\06009F0-\340\247\260\06009F1-\340\247\261\06009f0-\340\247\260\0600A05-\340\250\205\0600A0A-\340\250\212\0600a07-\340\250\207\0600A0F-\340\250\217\0600A10-\340\250\220\0600a0f-\340\250\217\0600A13-\340\250\223\0600A28-\340\250\250\0600a1d-\340\250\235\0600A2A-\340\250\252\0600A30-\340\250\260\0600a2d-\340\250\255\0600A32-\340\250\262\0600A33-\340\250\263\0600a32-\340\250\262\0600A35-\340\250\265\0600A36-\340\250\266\0600a35-\340\250\265\0600A38-\340\250\270\0600A39-\340\250\271\0600a38-\340\250\270\0600A59-\340\251\231\0600A5C-\340\251\234\0600a5a-\340\251\232\0600A5E-\340\251\236\0600A72-\340\251\262\0600A74-\340\251\264\0600a73-\340\251\263\0600A85-\340\252\205\0600A8B-\340\252\213\0600a88-\340\252\210\0600A8D-\340\252\215\0600A8F-\340\252\217\0600A91-\340\252\221\0600a90-\340\252\220\0600A93-\340\252\223\0600AA8-\340\252\250\0600a9d-\340\252\235\0600AAA-\340\252\252\0600AB0-\340\252\260\0600aad-\340\252\255\0600AB2-\340\252\262\0600AB3-\340\252\263\0600ab2-\340\252\262\0600AB5-\340\252\265\0600AB9-\340\252\271\0600ab7-\340\252\267\0600ABD-\340\252\275\0600AE0-\340\253\240\0600B05-\340\254\205\0600B0C-\340\254\214\0600b08-\340\254\210\0600B0F-\340\254\217\0600B10-\340\254\220\0600b0f-\340\254\217\0600B13-\340\254\223\0600B28-\340\254\250\0600b1d-\340\254\235\0600B2A-\340\254\252\0600B30-\340\254\260\0600b2d-\340\254\255\0600B32-\340\254\262\0600B33-\340\254\263\0600b32-\340\254\262\0600B36-\340\254\266\0600B39-\340\254\271\0600b37-\340\254\267\0600B3D-\340\254\275\0600B5C-\340\255\234\0600B5D-\340\255\235\0600b5c-\340\255\234\0600B5F-\340\255\237\0600B61-\340\255\241\0600b60-\340\255\240\0600B85-\340\256\205\0600B8A-\340\256\212\0600b87-\340\256\207\0600B8E-\340\256\216\0600B90-\340\256\220\0600b8f-\340\256\217\0600B92-\340\256\222\0600B95-\340\256\225\0600b93-\340\256\223\0600B99-\340\256\231\0600B9A-\340\256\232\0600b99-\340\256\231\0600B9C-\340\256\234\0600B9E-\340\256\236\0600B9F-\340\256\237\0600b9e-\340\256\236\0600BA3-\340\256\243\0600BA4-\340\256\244\0600ba3-\340\256\243\0600BA8-\340\256\250\0600BAA-\340\256\252\0600ba9-\340\256\251\0600BAE-\340\256\256\0600BB5-\340\256\265\0600bb1-\340\256\261\0600BB7-\340\256\267\0600BB9-\340\256\271\0600bb8-\340\256\270\0600C05-\340\260\205\0600C0C-\340\260\214\0600c08-\340\260\210\0600C0E-\340\260\216\0600C10-\340\260\220\0600c0f-\340\260\217\0600C12-\340\260\222\0600C28-\340\260\250\0600c1d-\340\260\235\0600C2A-\340\260\252\0600C33-\340\260\263\0600c2e-\340\260\256\0600C35-\340\260\265\0600C39-\340\260\271\0600c37-\340\260\267\0600C60-\340\261\240\0600C61-\340\261\241\0600c60-\340\261\240\0600C85-\340\262\205\0600C8C-\340\262\214\0600c88-\340\262\210\0600C8E-\340\262\216\0600C90-\340\262\220\0600c8f-\340\262\217\0600C92-\340\262\222\0600CA8-\340\262\250\0600c9d-\340\262\235\0600CAA-\340\262\252\0600CB3-\340\262\263\0600cae-\340\262\256\0600CB5-\340\262\265\0600CB9-\340\262\271\0600cb7-\340\262\267\0600CDE-\340\263\236\0600CE0-\340\263\240\0600CE1-\340\263\241\0600ce0-\340\263\240\0600D05-\340\264\205\0600D0C-\340\264\214\0600d08-\340\264\210\0600D0E-\340\264\216\0600D10-\340\264\220\0600d0f-\340\264\217\0600D12-\340\264\222\0600D28-\340\264\250\0600d1d-\340\264\235\0600D2A-\340\264\252\0600D39-\340\264\271\0600d31-\340\264\261\0600D60-\340\265\240\0600D61-\340\265\241\0600d60-\340\265\240\0600E01-\340\270\201\0600E2E-\340\270\256\0600e17-\340\270\227\0600E30-\340\270\260\0600E32-\340\270\262\0600E33-\340\270\263\0600e32-\340\270\262\0600E40-\340\271\200\0600E45-\340\271\205\0600e42-\340\271\202\0600E81-\340\272\201\0600E82-\340\272\202\0600e81-\340\272\201\0600E84-\340\272\204\0600E87-\340\272\207\0600E88-\340\272\210\0600e87-\340\272\207\0600E8A-\340\272\212\0600E8D-\340\272\215\0600E94-\340\272\224\0600E97-\340\272\227\0600e95-\340\272\225\0600E99-\340\272\231\0600E9F-\340\272\237\0600e9c-\340\272\234\0600EA1-\340\272\241\0600EA3-\340\272\243\0600ea2-\340\272\242\0600EA5-\340\272\245\0600EA7-\340\272\247\0600EAA-\340\272\252\0600EAB-\340\272\253\0600eaa-\340\272\252\0600EAD-\340\272\255\0600EAE-\340\272\256\0600ead-\340\272\255\0600EB0-\340\272\260\0600EB2-\340\272\262\0600EB3-\340\272\263\0600eb2-\340\272\262\0600EBD-\340\272\275\0600EC0-\340\273\200\0600EC4-\340\273\204\0600ec2-\340\273\202\0600F40-\340\275\200\0600F47-\340\275\207\0600f43-\340\275\203\0600F49-\340\275\211\0600F69-\340\275\251\0600f59-\340\275\231\06110A0-\341\202\240\06110C5-\341\203\205\06110b2-\341\202\262\06110D0-\341\203\220\06110F6-\341\203\266\06110e3-\341\203\243\0611100-\341\204\200\0611102-\341\204\202\0611103-\341\204\203\0611102-\341\204\202\0611105-\341\204\205\0611107-\341\204\207\0611106-\341\204\206\0611109-\341\204\211\061110B-\341\204\213\061110C-\341\204\214\061110b-\341\204\213\061110E-\341\204\216\0611112-\341\204\222\0611110-\341\204\220\061113C-\341\204\274\061113E-\341\204\276\0611140-\341\205\200\061114C-\341\205\214\061114E-\341\205\216\0611150-\341\205\220\0611154-\341\205\224\0611155-\341\205\225\0611154-\341\205\224\0611159-\341\205\231\061115F-\341\205\237\0611161-\341\205\241\0611160-\341\205\240\0611163-\341\205\243\0611165-\341\205\245\0611167-\341\205\247\0611169-\341\205\251\061116D-\341\205\255\061116E-\341\205\256\061116d-\341\205\255\0611172-\341\205\262\0611173-\341\205\263\0611172-\341\205\262\0611175-\341\205\265\061119E-\341\206\236\06111A8-\341\206\250\06111AB-\341\206\253\06111AE-\341\206\256\06111AF-\341\206\257\06111ae-\341\206\256\06111B7-\341\206\267\06111B8-\341\206\270\06111b7-\341\206\267\06111BA-\341\206\272\06111BC-\341\206\274\06111C2-\341\207\202\06111bf-\341\206\277\06111EB-\341\207\253\06111F0-\341\207\260\06111F9-\341\207\271\0611E00-\341\270\200\0611E9B-\341\272\233\0611e4d-\341\271\215\0611EA0-\341\272\240\0611EF9-\341\273\271\0611ecc-\341\273\214\0611F00-\341\274\200\0611F15-\341\274\225\0611f0a-\341\274\212\0611F18-\341\274\230\0611F1D-\341\274\235\0611f1a-\341\274\232\0611F20-\341\274\240\0611F45-\341\275\205\0611f32-\341\274\262\0611F48-\341\275\210\0611F4D-\341\275\215\0611f4a-\341\275\212\0611F50-\341\275\220\0611F57-\341\275\227\0611f53-\341\275\223\0611F59-\341\275\231\0611F5B-\341\275\233\0611F5D-\341\275\235\0611F5F-\341\275\237\0611F7D-\341\275\275\0611f6e-\341\275\256\0611F80-\341\276\200\0611FB4-\341\276\264\0611f9a-\341\276\232\0611FB6-\341\276\266\0611FBC-\341\276\274\0611fb9-\341\276\271\0611FBE-\341\276\276\0611FC2-\341\277\202\0611FC4-\341\277\204\0611fc3-\341\277\203\0611FC6-\341\277\206\0611FCC-\341\277\214\0611fc9-\341\277\211\0611FD0-\341\277\220\0611FD3-\341\277\223\0611fd1-\341\277\221\0611FD6-\341\277\226\0611FDB-\341\277\233\0611fd8-\341\277\230\0611FE0-\341\277\240\0611FEC-\341\277\254\0611fe6-\341\277\246\0611FF2-\341\277\262\0611FF4-\341\277\264\0611ff3-\341\277\263\0611FF6-\341\277\266\0611FFC-\341\277\274\0611ff9-\341\277\271\0622126-\342\204\246\062212A-\342\204\252\062212B-\342\204\253\062212a-\342\204\252\062212E-\342\204\256\0622180-\342\206\200\0622182-\342\206\202\0622181-\342\206\201\0633041-\343\201\201\0633094-\343\202\224\063306a-\343\201\252\06330A1-\343\202\241\06330FA-\343\203\272\06330cd-\343\203\215\0633105-\343\204\205\063312C-\343\204\254\0633118-\343\204\230\137\101\06000-\352\260\200\137\0677A3-\355\236\243\137\0611d1-\354\207\221 This is a PI target ?><book></book>"
},
{
  "ibm86v01.xml",
  302,
  "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\r\n<!DOCTYPE book [\r\n<!ELEMENT book ANY>\r\n<!-- This test case covers 2 legal character ranges plus\r\n     1 discrete legal characters for production 86. -->\r\n<?NAME_4E00-\344\270\200\0719FA5-\351\276\245\06776d2-\347\233\222\0633007-\343\200\207\0633021-\343\200\241\0633029-\343\200\251\0633025-\343\200\245 This is a PI target ?>\r\n]>\r\n<book/>\r\n",
  105,
  "<?NAME_4E00-\344\270\200\0719FA5-\351\276\245\06776d2-\347\233\222\0633007-\343\200\207\0633021-\343\200\241\0633029-\343\200\251\0633025-\343\200\245 This is a PI target ?><book></book>"
},
{
  "ibm87v01.xml",
  2200,
  "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\r\n<!DOCTYPE book [\r\n<!ELEMENT book ANY>\r\n<!-- This test case covers 65 legal character ranges plus\r\n     30 discrete legal characters for production 87. -->\r\n<?NAME_300-\314\200\063345-\315\205\063322-\314\242\063360-\315\240\063361-\315\241\063360-\315\240\064483-\322\203\064486-\322\206\064484-\322\204\065591-\326\221\0655A1-\326\241\065599-\326\231\0655A3-\326\243\0655B9-\326\271\0655ae-\326\256\0655BB-\326\273\0655BD-\326\275\0655bc-\326\274\0655BF-\326\277\0655C1-\327\201\0655C2-\327\202\0655c1-\327\201\0655C4-\327\204\06664B-\331\213\066652-\331\222\06664e-\331\216\066670-\331\260\0666D6-\333\226\0666DC-\333\234\0666d9-\333\231\0666DD-\333\235\0666DF-\333\237\0666de-\333\236\0666E0-\333\240\0666E4-\333\244\0666e2-\333\242\0666E7-\333\247\0666E8-\333\250\0666e7-\333\247\0666EA-\333\252\0666ED-\333\255\0666eb-\333\253\0600901-\340\244\201\0600903-\340\244\203\0600902-\340\244\202\060093C-\340\244\274\060093E-\340\244\276\060094C-\340\245\214\0600945-\340\245\205\060094D-\340\245\215\0600951-\340\245\221\0600954-\340\245\224\0600952-\340\245\222\0600962-\340\245\242\0600963-\340\245\243\0600962-\340\245\242\0600981-\340\246\201\0600983-\340\246\203\0600982-\340\246\202\06009BC-\340\246\274\06009BE-\340\246\276\06009BF-\340\246\277\06009C0-\340\247\200\06009C4-\340\247\204\06009c2-\340\247\202\06009C7-\340\247\207\06009C8-\340\247\210\06009c7-\340\247\207\06009CB-\340\247\213\06009CD-\340\247\215\06009cc-\340\247\214\06009D7-\340\247\227\06009E2-\340\247\242\06009E3-\340\247\243\06009e2-\340\247\242\0600A02-\340\250\202\0600A3C-\340\250\274\0600A3E-\340\250\276\0600A3F-\340\250\277\0600A40-\340\251\200\0600A42-\340\251\202\0600a41-\340\251\201\0600A47-\340\251\207\0600A48-\340\251\210\0600a47-\340\251\207\0600A4B-\340\251\213\0600A4D-\340\251\215\0600a4c-\340\251\214\0600A70-\340\251\260\0600A71-\340\251\261\0600a70-\340\251\260\0600A81-\340\252\201\0600A83-\340\252\203\0600a82-\340\252\202\0600ABC-\340\252\274\0600ABE-\340\252\276\0600AC5-\340\253\205\0600ac1-\340\253\201\0600AC7-\340\253\207\0600AC9-\340\253\211\0600ac8-\340\253\210\0600ACB-\340\253\213\0600ACD-\340\253\215\0600acc-\340\253\214\0600B01-\340\254\201\0600B03-\340\254\203\0600b02-\340\254\202\0600B3C-\340\254\274\0600B3E-\340\254\276\0600B43-\340\255\203\0600b40-\340\255\200\0600B47-\340\255\207\0600B48-\340\255\210\0600b47-\340\255\207\0600B4B-\340\255\213\0600B4D-\340\255\215\0600b4c-\340\255\214\0600B56-\340\255\226\0600B57-\340\255\227\0600b56-\340\255\226\0600B82-\340\256\202\0600B83-\340\256\203\0600b82-\340\256\202\0600BBE-\340\256\276\0600BC2-\340\257\202\0600bc0-\340\257\200\0600BC6-\340\257\206\0600BC8-\340\257\210\0600bc7-\340\257\207\0600BCA-\340\257\212\0600BCD-\340\257\215\0600bcb-\340\257\213\0600BD7-\340\257\227\0600C01-\340\260\201\0600C03-\340\260\203\0600c02-\340\260\202\0600C3E-\340\260\276\0600C44-\340\261\204\0600c41-\340\261\201\0600C46-\340\261\206\0600C48-\340\261\210\0600c47-\340\261\207\0600C4A-\340\261\212\0600C4D-\340\261\215\0600c4b-\340\261\213\0600C55-\340\261\225\0600C56-\340\261\226\0600c55-\340\261\225\0600C82-\340\262\202\0600C83-\340\262\203\0600c82-\340\262\202\0600CBE-\340\262\276\0600CC4-\340\263\204\0600cc1-\340\263\201\0600CC6-\340\263\206\0600CC8-\340\263\210\0600cc7-\340\263\207\0600CCA-\340\263\212\0600CCD-\340\263\215\0600ccb-\340\263\213\0600CD5-\340\263\225\0600CD6-\340\263\226\0600cd5-\340\263\225\0600D02-\340\264\202\0600D03-\340\264\203\0600d02-\340\264\202\0600D3E-\340\264\276\0600D43-\340\265\203\0600d40-\340\265\200\0600D46-\340\265\206\0600D48-\340\265\210\0600d47-\340\265\207\0600D4A-\340\265\212\0600D4D-\340\265\215\0600d4b-\340\265\213\0600D57-\340\265\227\0600E31-\340\270\261\0600E34-\340\270\264\0600E3A-\340\270\272\0600e37-\340\270\267\0600E47-\340\271\207\0600E4E-\340\271\216\0600e4a-\340\271\212\0600EB1-\340\272\261\0600EB4-\340\272\264\0600EB9-\340\272\271\0600eb6-\340\272\266\0600EBB-\340\272\273\0600EBC-\340\272\274\0600ebb-\340\272\273\0600EC8-\340\273\210\0600ECD-\340\273\215\0600eca-\340\273\212\0600F18-\340\274\230\0600F19-\340\274\231\0600f18-\340\274\230\0600F35-\340\274\265\0600F37-\340\274\267\0600F39-\340\274\271\0600F3E-\340\274\276\0600F3F-\340\274\277\0600F71-\340\275\261\0600F84-\340\276\204\0600f7a-\340\275\272\0600F86-\340\276\206\0600F8B-\340\276\213\0600f88-\340\276\210\0600F90-\340\276\220\0600F95-\340\276\225\0600f92-\340\276\222\0600F97-\340\276\227\0600F99-\340\276\231\0600FAD-\340\276\255\0600fa3-\340\276\243\0600FB1-\340\276\261\0600FB7-\340\276\267\0600fb4-\340\276\264\0600FB9-\340\276\271\06220D0-\342\203\220\06220DC-\342\203\234\06220d6-\342\203\226\06220E1-\342\203\241\063302A-\343\200\252\063302F-\343\200\257\063302c-\343\200\254\0633099-\343\202\231\063309A-\343\202\232 This is a PI target ?>\r\n]>\r\n<book/>\r\n",
  2001,
  "<?NAME_300-\314\200\063345-\315\205\063322-\314\242\063360-\315\240\063361-\315\241\063360-\315\240\064483-\322\203\064486-\322\206\064484-\322\204\065591-\326\221\0655A1-\326\241\065599-\326\231\0655A3-\326\243\0655B9-\326\271\0655ae-\326\256\0655BB-\326\273\0655BD-\326\275\0655bc-\326\274\0655BF-\326\277\0655C1-\327\201\0655C2-\327\202\0655c1-\327\201\0655C4-\327\204\06664B-\331\213\066652-\331\222\06664e-\331\216\066670-\331\260\0666D6-\333\226\0666DC-\333\234\0666d9-\333\231\0666DD-\333\235\0666DF-\333\237\0666de-\333\236\0666E0-\333\240\0666E4-\333\244\0666e2-\333\242\0666E7-\333\247\0666E8-\333\250\0666e7-\333\247\0666EA-\333\252\0666ED-\333\255\0666eb-\333\253\0600901-\340\244\201\0600903-\340\244\203\0600902-\340\244\202\060093C-\340\244\274\060093E-\340\244\276\060094C-\340\245\214\0600945-\340\245\205\060094D-\340\245\215\0600951-\340\245\221\0600954-\340\245\224\0600952-\340\245\222\0600962-\340\245\242\0600963-\340\245\243\0600962-\340\245\242\0600981-\340\246\201\0600983-\340\246\203\0600982-\340\246\202\06009BC-\340\246\274\06009BE-\340\246\276\06009BF-\340\246\277\06009C0-\340\247\200\06009C4-\340\247\204\06009c2-\340\247\202\06009C7-\340\247\207\06009C8-\340\247\210\06009c7-\340\247\207\06009CB-\340\247\213\06009CD-\340\247\215\06009cc-\340\247\214\06009D7-\340\247\227\06009E2-\340\247\242\06009E3-\340\247\243\06009e2-\340\247\242\0600A02-\340\250\202\0600A3C-\340\250\274\0600A3E-\340\250\276\0600A3F-\340\250\277\0600A40-\340\251\200\0600A42-\340\251\202\0600a41-\340\251\201\0600A47-\340\251\207\0600A48-\340\251\210\0600a47-\340\251\207\0600A4B-\340\251\213\0600A4D-\340\251\215\0600a4c-\340\251\214\0600A70-\340\251\260\0600A71-\340\251\261\0600a70-\340\251\260\0600A81-\340\252\201\0600A83-\340\252\203\0600a82-\340\252\202\0600ABC-\340\252\274\0600ABE-\340\252\276\0600AC5-\340\253\205\0600ac1-\340\253\201\0600AC7-\340\253\207\0600AC9-\340\253\211\0600ac8-\340\253\210\0600ACB-\340\253\213\0600ACD-\340\253\215\0600acc-\340\253\214\0600B01-\340\254\201\0600B03-\340\254\203\0600b02-\340\254\202\0600B3C-\340\254\274\0600B3E-\340\254\276\0600B43-\340\255\203\0600b40-\340\255\200\0600B47-\340\255\207\0600B48-\340\255\210\0600b47-\340\255\207\0600B4B-\340\255\213\0600B4D-\340\255\215\0600b4c-\340\255\214\0600B56-\340\255\226\0600B57-\340\255\227\0600b56-\340\255\226\0600B82-\340\256\202\0600B83-\340\256\203\0600b82-\340\256\202\0600BBE-\340\256\276\0600BC2-\340\257\202\0600bc0-\340\257\200\0600BC6-\340\257\206\0600BC8-\340\257\210\0600bc7-\340\257\207\0600BCA-\340\257\212\0600BCD-\340\257\215\0600bcb-\340\257\213\0600BD7-\340\257\227\0600C01-\340\260\201\0600C03-\340\260\203\0600c02-\340\260\202\0600C3E-\340\260\276\0600C44-\340\261\204\0600c41-\340\261\201\0600C46-\340\261\206\0600C48-\340\261\210\0600c47-\340\261\207\0600C4A-\340\261\212\0600C4D-\340\261\215\0600c4b-\340\261\213\0600C55-\340\261\225\0600C56-\340\261\226\0600c55-\340\261\225\0600C82-\340\262\202\0600C83-\340\262\203\0600c82-\340\262\202\0600CBE-\340\262\276\0600CC4-\340\263\204\0600cc1-\340\263\201\0600CC6-\340\263\206\0600CC8-\340\263\210\0600cc7-\340\263\207\0600CCA-\340\263\212\0600CCD-\340\263\215\0600ccb-\340\263\213\0600CD5-\340\263\225\0600CD6-\340\263\226\0600cd5-\340\263\225\0600D02-\340\264\202\0600D03-\340\264\203\0600d02-\340\264\202\0600D3E-\340\264\276\0600D43-\340\265\203\0600d40-\340\265\200\0600D46-\340\265\206\0600D48-\340\265\210\0600d47-\340\265\207\0600D4A-\340\265\212\0600D4D-\340\265\215\0600d4b-\340\265\213\0600D57-\340\265\227\0600E31-\340\270\261\0600E34-\340\270\264\0600E3A-\340\270\272\0600e37-\340\270\267\0600E47-\340\271\207\0600E4E-\340\271\216\0600e4a-\340\271\212\0600EB1-\340\272\261\0600EB4-\340\272\264\0600EB9-\340\272\271\0600eb6-\340\272\266\0600EBB-\340\272\273\0600EBC-\340\272\274\0600ebb-\340\272\273\0600EC8-\340\273\210\0600ECD-\340\273\215\0600eca-\340\273\212\0600F18-\340\274\230\0600F19-\340\274\231\0600f18-\340\274\230\0600F35-\340\274\265\0600F37-\340\274\267\0600F39-\340\274\271\0600F3E-\340\274\276\0600F3F-\340\274\277\0600F71-\340\275\261\0600F84-\340\276\204\0600f7a-\340\275\272\0600F86-\340\276\206\0600F8B-\340\276\213\0600f88-\340\276\210\0600F90-\340\276\220\0600F95-\340\276\225\0600f92-\340\276\222\0600F97-\340\276\227\0600F99-\340\276\231\0600FAD-\340\276\255\0600fa3-\340\276\243\0600FB1-\340\276\261\0600FB7-\340\276\267\0600fb4-\340\276\264\0600FB9-\340\276\271\06220D0-\342\203\220\06220DC-\342\203\234\06220d6-\342\203\226\06220E1-\342\203\241\063302A-\343\200\252\063302F-\343\200\257\063302c-\343\200\254\0633099-\343\202\231\063309A-\343\202\232 This is a PI target ?><book></book>"
},
{
  "ibm88v01.xml",
  621,
  "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\r\n<!DOCTYPE book [\r\n<!ELEMENT book ANY>\r\n<!-- This test case covers 15 legal character ranges plus\r\n     0 discrete legal characters for production 88. -->\r\n<?NAME_30-0_39-9_34-4_660-\331\240\066669-\331\251\066664-\331\244\0666F0-\333\260\0666F9-\333\271\0666f4-\333\264\0600966-\340\245\246\060096F-\340\245\257\060096a-\340\245\252\06009E6-\340\247\246\06009EF-\340\247\257\06009ea-\340\247\252\0600A66-\340\251\246\0600A6F-\340\251\257\0600a6a-\340\251\252\0600AE6-\340\253\246\0600AEF-\340\253\257\0600aea-\340\253\252\0600B66-\340\255\246\0600B6F-\340\255\257\0600b6a-\340\255\252\0600BE7-\340\257\247\0600BEF-\340\257\257\0600beb-\340\257\253\0600C66-\340\261\246\0600C6F-\340\261\257\0600c6a-\340\261\252\0600CE6-\340\263\246\0600CEF-\340\263\257\0600cea-\340\263\252\0600D66-\340\265\246\0600D6F-\340\265\257\0600d6a-\340\265\252\0600E50-\340\271\220\0600E59-\340\271\231\0600e54-\340\271\224\0600ED0-\340\273\220\0600ED9-\340\273\231\0600ed4-\340\273\224\0600F20-\340\274\240\0600F29-\340\274\251\0600f24-\340\274\244 This is a PI target ?>\r\n]>\r\n<book/>\r\n",
  423,
  "<?NAME_30-0_39-9_34-4_660-\331\240\066669-\331\251\066664-\331\244\0666F0-\333\260\0666F9-\333\271\0666f4-\333\264\0600966-\340\245\246\060096F-\340\245\257\060096a-\340\245\252\06009E6-\340\247\246\06009EF-\340\247\257\06009ea-\340\247\252\0600A66-\340\251\246\0600A6F-\340\251\257\0600a6a-\340\251\252\0600AE6-\340\253\246\0600AEF-\340\253\257\0600aea-\340\253\252\0600B66-\340\255\246\0600B6F-\340\255\257\0600b6a-\340\255\252\0600BE7-\340\257\247\0600BEF-\340\257\257\0600beb-\340\257\253\0600C66-\340\261\246\0600C6F-\340\261\257\0600c6a-\340\261\252\0600CE6-\340\263\246\0600CEF-\340\263\257\0600cea-\340\263\252\0600D66-\340\265\246\0600D6F-\340\265\257\0600d6a-\340\265\252\0600E50-\340\271\220\0600E59-\340\271\231\0600e54-\340\271\224\0600ED0-\340\273\220\0600ED9-\340\273\231\0600ed4-\340\273\224\0600F20-\340\274\240\0600F29-\340\274\251\0600f24-\340\274\244 This is a PI target ?><book></book>"
},
{
  "ibm89v01.xml",
  382,
  "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\r\n<!DOCTYPE book [\r\n<!ELEMENT book ANY>\r\n<!-- This test case covers 3 legal character ranges plus\r\n     8 discrete legal characters for production 89. -->\r\n<?NAME_0B7-\302\267\0622D0-\313\220\0622D1-\313\221\063387-\316\207\066640-\331\200\0600E46-\340\271\206\0600EC6-\340\273\206\0633005-\343\200\205\0633031-\343\200\261\0633035-\343\200\265\0633033-\343\200\263\063309D-\343\202\235\063309E-\343\202\236\063309d-\343\202\235\06330FC-\343\203\274\06330FE-\343\203\276\06330fd-\343\203\275 This is a PI target ?>\r\n]>\r\n<book/>\r\n",
  185,
  "<?NAME_0B7-\302\267\0622D0-\313\220\0622D1-\313\221\063387-\316\207\066640-\331\200\0600E46-\340\271\206\0600EC6-\340\273\206\0633005-\343\200\205\0633031-\343\200\261\0633035-\343\200\265\0633033-\343\200\263\063309D-\343\202\235\063309E-\343\202\236\063309d-\343\202\235\06330FC-\343\203\274\06330FE-\343\203\276\06330fd-\343\203\275 This is a PI target ?><book></book>"
},
{
  "ibm28i01.xml",
  231,
  "<?xml version=\"1.0\" encoding='UTF-8'?>\r\n<!DOCTYPE tiger [\r\n   <!ELEMENT tiger EMPTY>\r\n]>\r\n<!-- This against VC of P28. The Name in the document type declaration\r\n does not match the element type of the root element. --> \r\n<animal/>",
  0, NULL
},
{
  "ibm32i01.xml",
  434,
  "<?xml version=\"1.0\" standalone=\"yes\" ?>\r\n<!DOCTYPE animal SYSTEM \"ibm32i01.dtd\" [\r\n   <!ELEMENT animal EMPTY>\r\n]>\r\n<!-- This is against VC: Standalone Document Declaration in P32\r\n The standalone document declaration has the value \"yes\", there is an external \r\n markup declaration of attributes with default values, and the associated \r\n element appears in the document with specified values for those attributes.   \r\n-->\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm39i01.xml",
  283,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n  <!ELEMENT root (a,b)>\r\n  <!ELEMENT a EMPTY>\r\n  <!ELEMENT b (#PCDATA|c)* >\r\n  <!ELEMENT c ANY>\r\n\r\n]>\r\n<!--* EMPTY element a has content *-->\r\n<root><a>should not have content here</a><b>\r\n   <c></c> \r\n   content of b element\r\n</b></root>\r\n\r\n",
  0, NULL
},
{
  "ibm39i02.xml",
  291,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n  <!ELEMENT root (a,b)>\r\n  <!ELEMENT a EMPTY>\r\n  <!ELEMENT b (#PCDATA|c)* >\r\n  <!ELEMENT c ANY>\r\n\r\n]>\r\n<!--* root element have text content *-->\r\n<root>\r\n root can't have text content\r\n<a></a><b>\r\n   <c></c> \r\n   content of b element\r\n</b></root>\r\n\r\n",
  0, NULL
},
{
  "ibm39i03.xml",
  306,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n  <!ELEMENT root (a,b)>\r\n  <!ELEMENT a EMPTY>\r\n  <!ELEMENT b (#PCDATA|c)* >\r\n  <!ELEMENT c ANY>\r\n]>\r\n<!--* illgal element in b's Mixed content *-->\r\n<root><a/><b>\r\n   <c></c> \r\n   content of b element\r\n   <a/>\r\n   could not have 'a' as 'b's content\r\n</b></root>\r\n\r\n",
  0, NULL
},
{
  "ibm39i04.xml",
  351,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n  <!ELEMENT root (a,b)>\r\n  <!ELEMENT a EMPTY>\r\n  <!ELEMENT b (#PCDATA|c)* >\r\n  <!ELEMENT c ANY>\r\n  <!ELEMENT f EMPTY>\r\n]>\r\n<!--* element c has undeclared element as its ANY content *-->\r\n<root><a/><b>\r\n   <c><f/></c> \r\n   content of b element\r\n   <c>\r\n      <d>not declared in dtd</d>\r\n   </c>\r\n</b></root>\r\n\r\n",
  0, NULL
},
{
  "ibm41i01.xml",
  318,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n  <!ELEMENT root (#PCDATA|b)* >\r\n  <!ELEMENT b (#PCDATA) >\r\n  <!ATTLIST b attr2 (abc|def) \"abc\">\r\n  <!ATTLIST b attr3 CDATA #FIXED \"fixed\">\r\n]>\r\n<root>\r\n  <b attr1=\"value1\" attr2=\"def\" attr3=\"fixed\">attr1 not declared</b>\r\n</root>\r\n<!--* testing VC:Attribute Value Type  *-->\r\n",
  0, NULL
},
{
  "ibm41i02.xml",
  365,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n  <!ELEMENT root (PCDATA|b)* >\r\n  <!ELEMENT b (#PCDATA) >\r\n  <!ATTLIST b attr1 CDATA #REQUIRED>\r\n  <!ATTLIST b attr2 (abc|def) \"abc\">\r\n  <!ATTLIST b attr3 CDATA #FIXED \"fixed\">\r\n]>\r\n<root>\r\n  <b attr1=\"value1\" attr2=\"abc\" attr3=\"shoudbefixed\">attr3 value not fixed</b>\r\n</root>\r\n<!--* testing P41 VC: AtributeValueType*-->\r\n",
  0, NULL
},
{
  "ibm45i01.xml",
  549,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n  <!ELEMENT root (#PCDATA|b)* >\r\n  <!ELEMENT b EMPTY>\r\n  <!ELEMENT not_unique ANY>\r\n  <!ELEMENT not_unique EMPTY>\r\n  <!ELEMENT not_unique (b,b) >\r\n  <!ELEMENT unique. ANY>\r\n  <!ATTLIST b attr1 CDATA #IMPLIED>\r\n  <!ATTLIST b attr2 CDATA #IMPLIED>\r\n  <!ATTLIST b attr3 CDATA #IMPLIED>\r\n]>\r\n<root>\r\n  <b/>without white space\r\n  <b /> with a white space\r\n  <b attr1=\"value1\" />\r\n  <b attr1=\"value1\" attr2=\"value2\" attr3 = \"value3\"/>\r\n</root>\r\n<!--* a invalid test: testing P45 VC unique element type decl  *-->\r\n",
  0, NULL
},
{
  "ibm49i01.xml",
  226,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root SYSTEM \"ibm49i01.dtd\" [\r\n  <!ELEMENT root (a,b)>\r\n]>\r\n<root><a/><b>\r\n   <c></c >\r\n   content of b element\r\n</b></root>\r\n<!--* a invalid test: tests VC:Proper Group/PE Nesting in P49 *-->\r\n",
  0, NULL
},
{
  "ibm49i02.xml",
  226,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root SYSTEM \"ibm49i02.dtd\" [\r\n  <!ELEMENT root (a,b)>\r\n]>\r\n<root><a/><b>\r\n   <c></c >\r\n   content of b element\r\n</b></root>\r\n<!--* a invalid test: tests VC:Proper Group/PE Nesting in P49 *-->\r\n",
  0, NULL
},
{
  "ibm50i01.xml",
  226,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root SYSTEM \"ibm50i01.dtd\" [\r\n  <!ELEMENT root (a,b)>\r\n]>\r\n<root><a/><b>\r\n   <c></c >\r\n   content of b element\r\n</b></root>\r\n<!--* a invalid test: tests VC:Proper Group/PE Nesting in P50 *-->\r\n",
  0, NULL
},
{
  "ibm51i01.xml",
  225,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root SYSTEM \"ibm51i01.dtd\" [\r\n  <!ELEMENT root ANY>\r\n]>\r\n<root>\r\n  <a> Element type a </a>\r\n  <b> Element type b </b>\r\n</root>\r\n<!--* a invalid test: tests P51 VC: Proper Group/PE Nesting *-->",
  0, NULL
},
{
  "ibm56i01.xml",
  370,
  "<?xml  version=\"1.0\"?>\r\n<!-- validity constraint check for Production 56(negative Test)-->\r\n<!DOCTYPE tokenizer\r\n [\r\n <!ELEMENT tokenizer ANY>\r\n <!ATTLIST tokenizer UniqueName ID #REQUIRED>\r\n ]>\r\n<tokenizer UniqueName = \"@c999\">\r\nThis is a negative test for validity constraints\r\nthe value of the attribute with a type ID does not match the Name production\r\n</tokenizer>",
  0, NULL
},
{
  "ibm56i02.xml",
  459,
  "<?xml  version=\"1.0\"?>\r\n<!-- validity constraint check for Production 56(negative Test)-->\r\n<!DOCTYPE tokenizer\r\n [\r\n <!ELEMENT tokenizer ANY>\r\n <!ELEMENT b EMPTY>\r\n <!ATTLIST b attr ID #REQUIRED>\r\n <!ATTLIST tokenizer UniqueName ID #REQUIRED>\r\n  ]>\r\n<tokenizer UniqueName = \"Ac999\">\r\n<b attr = \"Ac999\"></b>\r\nThis is a negative test for validity constraints\r\nthe value of the attribute with a type ID appears more than once in the XML document\r\n</tokenizer>\r\n",
  0, NULL
},
{
  "ibm56i03.xml",
  329,
  "<?xml  version=\"1.0\"?>\r\n<!-- validity constraint check for Production 56(Negative Test)-->\r\n<!DOCTYPE tokenizer\r\n [\r\n <!ELEMENT tokenizer ANY>\r\n <!ATTLIST tokenizer UniqueName ID #FIXED \"AC1999\">\r\n ]>\r\n<tokenizer>\r\nThis is a Negative validity test for ID Attribute Default.\r\nGiving the attribute default as #FIXED\r\n</tokenizer>\r\n",
  0, NULL
},
{
  "ibm58i01.xml",
  560,
  "<?xml  version=\"1.0\"?>\r\n<!-- validity test for Production 58(Negative Test-->\r\n<!DOCTYPE test\r\n [\r\n <!ELEMENT test ANY>\r\n <!ELEMENT blob (#PCDATA)>\r\n <!NOTATION base64 SYSTEM \"mimecode\">\r\n <!NOTATION uuencode SYSTEM \"uudecode\">\r\n <!NOTATION raw SYSTEM \"raw\">\r\n <!ATTLIST blob content-encoding NOTATION (base64|uuencode) #REQUIRED>\r\n ]>\r\n <test>\r\n<blob content-encoding = \"raw\"></blob>\r\nThe attribute values of type NOTATION does not match any of the notation names included in the\r\ndeclaration.All notation names in the declaration have been declared.\r\n</test>",
  0, NULL
},
{
  "ibm58i02.xml",
  547,
  "<?xml  version=\"1.0\"?>\r\n<!-- validity test for Production 58(Negative Test-->\r\n<!DOCTYPE test\r\n [\r\n <!ELEMENT test ANY>\r\n <!ELEMENT blob (#PCDATA)>\r\n <!NOTATION base64 SYSTEM \"mimecode\">\r\n <!NOTATION uuencode SYSTEM \"uudecode\">\r\n <!ATTLIST blob content-encoding NOTATION (base64|uuencode|raw|ascii) #REQUIRED>\r\n ]>\r\n <test>\r\n<blob content-encoding = \"raw\"></blob>\r\nThe attribute values of type NOTATION does match any of the notation names included in the\r\ndeclaration, but some of notation names in the declaration have not been declared\r\n</test>",
  0, NULL
},
{
  "ibm59i01.xml",
  394,
  "<?xml  version=\"1.0\"?>\r\n<!-- validity test for Production 59-->\r\n<!DOCTYPE test\r\n [\r\n <!ELEMENT test ANY>\r\n <!ELEMENT one EMPTY>\r\n <!ELEMENT two EMPTY>\r\n <!ELEMENT num EMPTY>\r\n <!ATTLIST num value (one|two) #IMPLIED>\r\n ]>\r\n <test>\r\n<num value = \"ONE\"></num>\r\nThis is a Negative test\r\nThe attribute values of type Enumeration does not match any of the Nmtoken tokens in the declaration.\r\n</test>",
  0, NULL
},
{
  "ibm60i01.xml",
  472,
  "<?xml  version=\"1.0\"?>\r\n<!-- validity test for Production 60-->\r\n<!DOCTYPE Java \r\n [\r\n <!ELEMENT Java ANY>\r\n <!ELEMENT one EMPTY>\r\n <!ELEMENT two EMPTY>\r\n <!ATTLIST one chapter CDATA #REQUIRED>\r\n <!ATTLIST two chapter CDATA #REQUIRED>\r\n ]>\r\n<Java>\r\n<one chapter=\"Introduction\"></one>\r\n<two></two>\r\nNegative test for Required Attribute. Some occurrence of an element with \r\nan attribute of #REQUIRED default declaration does not give the value of \r\nthose attribute\r\n</Java>",
  0, NULL
},
{
  "ibm60i02.xml",
  443,
  "<?xml  version=\"1.0\"?>\r\n<!-- validity test for Production 60-->\r\n<!DOCTYPE Java \r\n [\r\n <!ELEMENT Java ANY>\r\n <!ELEMENT one EMPTY>\r\n <!ATTLIST one chapter CDATA #FIXED \"Introduction\">\r\n  ]>\r\n<Java>\r\n<one chapter=\"JavaBeans\"></one>\r\nNegative Test\r\nAn attribute has a default value declared with the #FIXED keyword, \r\nand an instances of that attribute is given a value which is not \r\nthe same as the default value in the declaration. \r\n</Java>\r\n",
  0, NULL
},
{
  "ibm60i03.xml",
  363,
  "<?xml  version=\"1.0\"?>\r\n<!-- validity test for Production 60-->\r\n<!DOCTYPE test\r\n [\r\n <!ELEMENT test ANY>\r\n <!ELEMENT a EMPTY>\r\n <!ELEMENT b EMPTY>\r\n <!ELEMENT attr EMPTY>\r\n <!ATTLIST attr value (a|b) \"c\"> \r\n  ]>\r\n<test>\r\nThe default value specified for an attribute does not meet the \r\nlexical constraints of the declared attribute type.\r\n</test>\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n",
  0, NULL
},
{
  "ibm60i04.xml",
  346,
  "<?xml  version=\"1.0\"?>\r\n<!-- validity test for Production 60-->\r\n<!DOCTYPE test\r\n [\r\n <!ELEMENT test ANY>\r\n <!ELEMENT a EMPTY>\r\n <!ELEMENT nametoken EMPTY>\r\n <!ATTLIST nametoken namevalue NMTOKEN \"@#$\"> \r\n  ]>\r\n<test>\r\nThe default value specified for an attribute does not meet the \r\nlexical constraints of the declared attribute type.\r\n</test>\r\n",
  0, NULL
},
{
  "ibm68i01.xml",
  211,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root SYSTEM \"ibm68i01.dtd\" [\r\n  <!ELEMENT root (#PCDATA|a)* >\r\n]>\r\n<root>\r\n  pcdata content\r\n  <a attr1=\"xyz\"/>\r\n</root>\r\n<!--* a invalid test for P68 VC:Entity Declared *-->\r\n\r\n",
  0, NULL
},
{
  "ibm68i02.xml",
  211,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root SYSTEM \"ibm68i02.dtd\" [\r\n  <!ELEMENT root (#PCDATA|a)* >\r\n]>\r\n<root>\r\n  pcdata content\r\n  <a attr1=\"xyz\"/>\r\n</root>\r\n<!--* a invalid test for P68 VC:Entity Declared *-->\r\n\r\n",
  0, NULL
},
{
  "ibm68i03.xml",
  231,
  "<?xml version=\"1.0\" standalone='no'?>\r\n<!DOCTYPE root [\r\n  <!ELEMENT root (#PCDATA)* >\r\n  <!ENTITY % pe1 SYSTEM \"ibm68i03.ent\">\r\n  %pe1;\r\n]>\r\n<root>\r\n  pcdata content\r\n</root>\r\n<!--* a invalid test for P68 VC:Entity Declared *-->\r\n",
  0, NULL
},
{
  "ibm68i04.xml",
  231,
  "<?xml version=\"1.0\" standalone='no'?>\r\n<!DOCTYPE root [\r\n  <!ELEMENT root (#PCDATA)* >\r\n  <!ENTITY % pe1 SYSTEM \"ibm68i04.ent\">\r\n  %pe1;\r\n]>\r\n<root>\r\n  pcdata content\r\n</root>\r\n<!--* a invalid test for P68 VC:Entity Declared *-->\r\n",
  0, NULL
},
{
  "ibm69i01.xml",
  211,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root SYSTEM \"ibm69i01.dtd\" [\r\n  <!ELEMENT root (#PCDATA|a)* >\r\n]>\r\n<root>\r\n  pcdata content\r\n  <a attr1=\"xyz\"/>\r\n</root>\r\n<!--* a invalid test for P69 VC:Entity Declared *-->\r\n\r\n",
  0, NULL
},
{
  "ibm69i02.xml",
  211,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root SYSTEM \"ibm69i02.dtd\" [\r\n  <!ELEMENT root (#PCDATA|a)* >\r\n]>\r\n<root>\r\n  pcdata content\r\n  <a attr1=\"xyz\"/>\r\n</root>\r\n<!--* a invalid test for P69 VC:Entity Declared *-->\r\n\r\n",
  0, NULL
},
{
  "ibm69i03.xml",
  231,
  "<?xml version=\"1.0\" standalone='no'?>\r\n<!DOCTYPE root [\r\n  <!ELEMENT root (#PCDATA)* >\r\n  <!ENTITY % pe1 SYSTEM \"ibm69i03.ent\">\r\n  %pe1;\r\n]>\r\n<root>\r\n  pcdata content\r\n</root>\r\n<!--* a invalid test for P69 VC:Entity Declared *-->\r\n",
  0, NULL
},
{
  "ibm69i04.xml",
  232,
  "<?xml version=\"1.0\" standalone='no'?>\r\n<!DOCTYPE root  [\r\n  <!ELEMENT root (#PCDATA)* >\r\n  <!ENTITY % pe1 SYSTEM \"ibm69i04.ent\">\r\n  %pe1;\r\n]>\r\n<root>\r\n  pcdata content\r\n</root>\r\n<!--* a invalid test for P69 VC:Entity Declared *-->\r\n",
  0, NULL
},
{
  "ibm76i01.xml",
  387,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n\r\n<!ENTITY % pe1 '<!ATTLIST root att2 CDATA \"&ge1;\">'>\r\n<!ENTITY ge1 \"attdefaultvalue\" >\r\n%pe1;\r\n\r\n<!--* notation JPGformat not declared *-->\r\n<!ENTITY ge2  SYSTEM \"image.jpg\" NDATA JPGformat>\r\n\r\n]>\r\n<root att2=\"any\" />\r\n<!-- a invalid test case: test P76 VC: Notation Declared -->",
  0, NULL
},
{
  "ibm01n01.xml",
  96,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE book [\r\n   <!ELEMENT book ANY>\r\n]>\r\n<!-- element is missing -->",
  0, NULL
},
{
  "ibm01n02.xml",
  120,
  "<doc>Wrong ordering between prolog and element!</doc>\r\n<?xml version=\"1.0\"?>\r\n<!DOCTYPE doc [\r\n   <!ELEMENT doc ANY>\r\n]>",
  0, NULL
},
{
  "ibm01n03.xml",
  217,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE doc [\r\n   <!ELEMENT doc ANY>\r\n   <!ELEMENT title ANY>\r\n]>\r\n<doc>Wrong combination!</doc>\r\n<?PI after document element?>\r\n<title>Wrong combination!</title>\r\n<?PI after title element?>\r\n",
  0, NULL
},
{
  "ibm02n01.xml",
  91,
  "<!DOCTYPE book [\r\n<!ELEMENT book ANY>\r\n]>\r\n<!-- IllegalChar #x00\r\n in p02: \0 -->\r\n<book/>\r\n",
  0, NULL
},
{
  "ibm02n02.xml",
  91,
  "<!DOCTYPE book [\r\n<!ELEMENT book ANY>\r\n]>\r\n<!-- IllegalChar #x01\r\n in p02: \1 -->\r\n<book/>\r\n",
  0, NULL
},
{
  "ibm02n03.xml",
  91,
  "<!DOCTYPE book [\r\n<!ELEMENT book ANY>\r\n]>\r\n<!-- IllegalChar #x02\r\n in p02: \2 -->\r\n<book/>\r\n",
  0, NULL
},
{
  "ibm02n04.xml",
  91,
  "<!DOCTYPE book [\r\n<!ELEMENT book ANY>\r\n]>\r\n<!-- IllegalChar #x03\r\n in p02: \3 -->\r\n<book/>\r\n",
  0, NULL
},
{
  "ibm02n05.xml",
  91,
  "<!DOCTYPE book [\r\n<!ELEMENT book ANY>\r\n]>\r\n<!-- IllegalChar #x04\r\n in p02: \4 -->\r\n<book/>\r\n",
  0, NULL
},
{
  "ibm02n06.xml",
  91,
  "<!DOCTYPE book [\r\n<!ELEMENT book ANY>\r\n]>\r\n<!-- IllegalChar #x05\r\n in p02: \5 -->\r\n<book/>\r\n",
  0, NULL
},
{
  "ibm02n07.xml",
  91,
  "<!DOCTYPE book [\r\n<!ELEMENT book ANY>\r\n]>\r\n<!-- IllegalChar #x06\r\n in p02: \6 -->\r\n<book/>\r\n",
  0, NULL
},
{
  "ibm02n08.xml",
  91,
  "<!DOCTYPE book [\r\n<!ELEMENT book ANY>\r\n]>\r\n<!-- IllegalChar #x07\r\n in p02: \a -->\r\n<book/>\r\n",
  0, NULL
},
{
  "ibm02n09.xml",
  91,
  "<!DOCTYPE book [\r\n<!ELEMENT book ANY>\r\n]>\r\n<!-- IllegalChar #x08\r\n in p02: \b -->\r\n<book/>\r\n",
  0, NULL
},
{
  "ibm02n10.xml",
  91,
  "<!DOCTYPE book [\r\n<!ELEMENT book ANY>\r\n]>\r\n<!-- IllegalChar #x0b\r\n in p02: \v -->\r\n<book/>\r\n",
  0, NULL
},
{
  "ibm02n11.xml",
  91,
  "<!DOCTYPE book [\r\n<!ELEMENT book ANY>\r\n]>\r\n<!-- IllegalChar #x0c\r\n in p02: \f -->\r\n<book/>\r\n",
  0, NULL
},
{
  "ibm02n12.xml",
  91,
  "<!DOCTYPE book [\r\n<!ELEMENT book ANY>\r\n]>\r\n<!-- IllegalChar #x0e\r\n in p02: \16 -->\r\n<book/>\r\n",
  0, NULL
},
{
  "ibm02n13.xml",
  91,
  "<!DOCTYPE book [\r\n<!ELEMENT book ANY>\r\n]>\r\n<!-- IllegalChar #x0f\r\n in p02: \17 -->\r\n<book/>\r\n",
  0, NULL
},
{
  "ibm02n14.xml",
  91,
  "<!DOCTYPE book [\r\n<!ELEMENT book ANY>\r\n]>\r\n<!-- IllegalChar #x10\r\n in p02: \20 -->\r\n<book/>\r\n",
  0, NULL
},
{
  "ibm02n15.xml",
  91,
  "<!DOCTYPE book [\r\n<!ELEMENT book ANY>\r\n]>\r\n<!-- IllegalChar #x11\r\n in p02: \21 -->\r\n<book/>\r\n",
  0, NULL
},
{
  "ibm02n16.xml",
  91,
  "<!DOCTYPE book [\r\n<!ELEMENT book ANY>\r\n]>\r\n<!-- IllegalChar #x12\r\n in p02: \22 -->\r\n<book/>\r\n",
  0, NULL
},
{
  "ibm02n17.xml",
  91,
  "<!DOCTYPE book [\r\n<!ELEMENT book ANY>\r\n]>\r\n<!-- IllegalChar #x13\r\n in p02: \23 -->\r\n<book/>\r\n",
  0, NULL
},
{
  "ibm02n18.xml",
  91,
  "<!DOCTYPE book [\r\n<!ELEMENT book ANY>\r\n]>\r\n<!-- IllegalChar #x14\r\n in p02: \24 -->\r\n<book/>\r\n",
  0, NULL
},
{
  "ibm02n19.xml",
  91,
  "<!DOCTYPE book [\r\n<!ELEMENT book ANY>\r\n]>\r\n<!-- IllegalChar #x15\r\n in p02: \25 -->\r\n<book/>\r\n",
  0, NULL
},
{
  "ibm02n20.xml",
  91,
  "<!DOCTYPE book [\r\n<!ELEMENT book ANY>\r\n]>\r\n<!-- IllegalChar #x16\r\n in p02: \26 -->\r\n<book/>\r\n",
  0, NULL
},
{
  "ibm02n21.xml",
  91,
  "<!DOCTYPE book [\r\n<!ELEMENT book ANY>\r\n]>\r\n<!-- IllegalChar #x17\r\n in p02: \27 -->\r\n<book/>\r\n",
  0, NULL
},
{
  "ibm02n22.xml",
  91,
  "<!DOCTYPE book [\r\n<!ELEMENT book ANY>\r\n]>\r\n<!-- IllegalChar #x18\r\n in p02: \30 -->\r\n<book/>\r\n",
  0, NULL
},
{
  "ibm02n23.xml",
  91,
  "<!DOCTYPE book [\r\n<!ELEMENT book ANY>\r\n]>\r\n<!-- IllegalChar #x19\r\n in p02: \31 -->\r\n<book/>\r\n",
  0, NULL
},
{
  "ibm02n24.xml",
  91,
  "<!DOCTYPE book [\r\n<!ELEMENT book ANY>\r\n]>\r\n<!-- IllegalChar #x1a\r\n in p02: \32 -->\r\n<book/>\r\n",
  0, NULL
},
{
  "ibm02n25.xml",
  91,
  "<!DOCTYPE book [\r\n<!ELEMENT book ANY>\r\n]>\r\n<!-- IllegalChar #x1b\r\n in p02: \33 -->\r\n<book/>\r\n",
  0, NULL
},
{
  "ibm02n26.xml",
  91,
  "<!DOCTYPE book [\r\n<!ELEMENT book ANY>\r\n]>\r\n<!-- IllegalChar #x1c\r\n in p02: \34 -->\r\n<book/>\r\n",
  0, NULL
},
{
  "ibm02n27.xml",
  91,
  "<!DOCTYPE book [\r\n<!ELEMENT book ANY>\r\n]>\r\n<!-- IllegalChar #x1d\r\n in p02: \35 -->\r\n<book/>\r\n",
  0, NULL
},
{
  "ibm02n28.xml",
  91,
  "<!DOCTYPE book [\r\n<!ELEMENT book ANY>\r\n]>\r\n<!-- IllegalChar #x1e\r\n in p02: \36 -->\r\n<book/>\r\n",
  0, NULL
},
{
  "ibm02n29.xml",
  91,
  "<!DOCTYPE book [\r\n<!ELEMENT book ANY>\r\n]>\r\n<!-- IllegalChar #x1f\r\n in p02: \37 -->\r\n<book/>\r\n",
  0, NULL
},
{
  "ibm02n30.xml",
  95,
  "<!DOCTYPE book [\r\n<!ELEMENT book ANY>\r\n]>\r\n<!-- IllegalChar #xd800\r\n in p02: \355\240\200 -->\r\n<book/>\r\n",
  0, NULL
},
{
  "ibm02n31.xml",
  95,
  "<!DOCTYPE book [\r\n<!ELEMENT book ANY>\r\n]>\r\n<!-- IllegalChar #xdfff\r\n in p02: \355\277\277 -->\r\n<book/>\r\n",
  0, NULL
},
{
  "ibm02n32.xml",
  95,
  "<!DOCTYPE book [\r\n<!ELEMENT book ANY>\r\n]>\r\n<!-- IllegalChar #xfffe\r\n in p02: \357\277\276 -->\r\n<book/>\r\n",
  0, NULL
},
{
  "ibm02n33.xml",
  95,
  "<!DOCTYPE book [\r\n<!ELEMENT book ANY>\r\n]>\r\n<!-- IllegalChar #xffff\r\n in p02: \357\277\277 -->\r\n<book/>\r\n",
  0, NULL
},
{
  "ibm03n01.xml",
  138,
  "<!DOCTYPE book [\r\n<!ELEMENT book ANY>\r\n]>\r\n<!-- IllegalChar #x3000\r\n in p03: \343\200\200 -->\r\n<book>Illegal space 3000 in the end tag</book \343\200\200\r\n",
  0, NULL
},
{
  "ibm04n01.xml",
  95,
  "<!DOCTYPE IllegalNameChar! [\r\n   <!ELEMENT IllegalNameChar! EMPTY>\r\n]>\r\n<IllegalNameChar!/>\r\n\r\n",
  0, NULL
},
{
  "ibm04n02.xml",
  95,
  "<!DOCTYPE IllegalNameChar( [\r\n   <!ELEMENT IllegalNameChar( EMPTY>\r\n]>\r\n<IllegalNameChar(/>\r\n\r\n",
  0, NULL
},
{
  "ibm04n03.xml",
  95,
  "<!DOCTYPE IllegalNameChar) [\r\n   <!ELEMENT IllegalNameChar) EMPTY>\r\n]>\r\n<IllegalNameChar)/>\r\n\r\n",
  0, NULL
},
{
  "ibm04n04.xml",
  95,
  "<!DOCTYPE IllegalNameChar+ [\r\n   <!ELEMENT IllegalNameChar+ EMPTY>\r\n]>\r\n<IllegalNameChar+/>\r\n\r\n",
  0, NULL
},
{
  "ibm04n05.xml",
  95,
  "<!DOCTYPE IllegalNameChar, [\r\n   <!ELEMENT IllegalNameChar, EMPTY>\r\n]>\r\n<IllegalNameChar,/>\r\n\r\n",
  0, NULL
},
{
  "ibm04n06.xml",
  95,
  "<!DOCTYPE IllegalNameChar/ [\r\n   <!ELEMENT IllegalNameChar/ EMPTY>\r\n]>\r\n<IllegalNameChar//>\r\n\r\n",
  0, NULL
},
{
  "ibm04n07.xml",
  95,
  "<!DOCTYPE IllegalNameChar; [\r\n   <!ELEMENT IllegalNameChar; EMPTY>\r\n]>\r\n<IllegalNameChar;/>\r\n\r\n",
  0, NULL
},
{
  "ibm04n08.xml",
  95,
  "<!DOCTYPE IllegalNameChar< [\r\n   <!ELEMENT IllegalNameChar< EMPTY>\r\n]>\r\n<IllegalNameChar</>\r\n\r\n",
  0, NULL
},
{
  "ibm04n09.xml",
  95,
  "<!DOCTYPE IllegalNameChar= [\r\n   <!ELEMENT IllegalNameChar= EMPTY>\r\n]>\r\n<IllegalNameChar=/>\r\n\r\n",
  0, NULL
},
{
  "ibm04n10.xml",
  95,
  "<!DOCTYPE IllegalNameChar? [\r\n   <!ELEMENT IllegalNameChar? EMPTY>\r\n]>\r\n<IllegalNameChar?/>\r\n\r\n",
  0, NULL
},
{
  "ibm04n11.xml",
  95,
  "<!DOCTYPE IllegalNameChar[ [\r\n   <!ELEMENT IllegalNameChar[ EMPTY>\r\n]>\r\n<IllegalNameChar[/>\r\n\r\n",
  0, NULL
},
{
  "ibm04n12.xml",
  95,
  "<!DOCTYPE IllegalNameChar\\ [\r\n   <!ELEMENT IllegalNameChar\\ EMPTY>\r\n]>\r\n<IllegalNameChar\\/>\r\n\r\n",
  0, NULL
},
{
  "ibm04n13.xml",
  95,
  "<!DOCTYPE IllegalNameChar] [\r\n   <!ELEMENT IllegalNameChar] EMPTY>\r\n]>\r\n<IllegalNameChar]/>\r\n\r\n",
  0, NULL
},
{
  "ibm04n14.xml",
  95,
  "<!DOCTYPE IllegalNameChar^ [\r\n   <!ELEMENT IllegalNameChar^ EMPTY>\r\n]>\r\n<IllegalNameChar^/>\r\n\r\n",
  0, NULL
},
{
  "ibm04n15.xml",
  95,
  "<!DOCTYPE IllegalNameChar` [\r\n   <!ELEMENT IllegalNameChar` EMPTY>\r\n]>\r\n<IllegalNameChar`/>\r\n\r\n",
  0, NULL
},
{
  "ibm04n16.xml",
  95,
  "<!DOCTYPE IllegalNameChar{ [\r\n   <!ELEMENT IllegalNameChar{ EMPTY>\r\n]>\r\n<IllegalNameChar{/>\r\n\r\n",
  0, NULL
},
{
  "ibm04n17.xml",
  95,
  "<!DOCTYPE IllegalNameChar| [\r\n   <!ELEMENT IllegalNameChar| EMPTY>\r\n]>\r\n<IllegalNameChar|/>\r\n\r\n",
  0, NULL
},
{
  "ibm04n18.xml",
  95,
  "<!DOCTYPE IllegalNameChar} [\r\n   <!ELEMENT IllegalNameChar} EMPTY>\r\n]>\r\n<IllegalNameChar}/>\r\n\r\n",
  0, NULL
},
{
  "ibm05n01.xml",
  107,
  "<!DOCTYPE .A_name_starts_with. [\r\n   <!ELEMENT .A_name_starts_with. EMPTY>\r\n]>\r\n<.A_name_starts_with./>  \r\n",
  0, NULL
},
{
  "ibm05n02.xml",
  107,
  "<!DOCTYPE -A_name_starts_With- [\r\n   <!ELEMENT -A_name_starts_With- EMPTY>\r\n]>\r\n<-A_name_starts_With-/>  \r\n",
  0, NULL
},
{
  "ibm05n03.xml",
  122,
  "<!DOCTYPE 5A_name_starts_with_digit [\r\n   <!ELEMENT 5A_name_starts_with_digit EMPTY>\r\n]>\r\n<5A_name_starts_with_digit/>  \r\n",
  0, NULL
},
{
  "ibm05n04.xml",
  128,
  "<!DOCTYPE illegalChar0f7\303\267\111\156\124\150\145\115\151\144\144\154\145 [\r\n   <!ELEMENT illegalChar0f7\303\267\111\156\124\150\145\115\151\144\144\154\145 EMPTY>\r\n]>\r\n<illegalChar0f7\303\267\111\156\124\150\145\115\151\144\144\154\145\57\r\n\r\n",
  0, NULL
},
{
  "ibm05n05.xml",
  98,
  "<!DOCTYPE illegalNameChar\303\267 [\r\n   <!ELEMENT illegalNameChar\303\267 EMPTY>\r\n]>\r\n<illegalNameChar\303\267\57\r\n\r\n",
  0, NULL
},
{
  "ibm09n01.xml",
  223,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)> \r\n \t<!ENTITY FullName \"Snow%Man\">\r\n]>\r\n\r\n<!-- testing invalid entity value -->\r\n<student>My Name is &FullName;. </student>\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n ",
  0, NULL
},
{
  "ibm09n02.xml",
  196,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)> \r\n \t<!ENTITY FullName \"Snow&Man\">\r\n]>\r\n\r\n<!-- testing invalid entity value -->\r\n<student>My Name is &FullName;. </student>",
  0, NULL
},
{
  "ibm09n03.xml",
  196,
  "<?xml version=\"1.0\"?> \r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)>\r\n\t<!ENTITY FullName \"Snow\"Man\"> \r\n]>\r\n\r\n<!-- testing invalid entity value -->\r\n<student>My Name is &FullName;. </student>",
  0, NULL
},
{
  "ibm09n04.xml",
  196,
  "<?xml version=\"1.0\"?> \r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)>\r\n\t<!ENTITY FullName \"SnowMan> \r\n]>\r\n\r\n<!-- testing invalid entity value -->\r\n<student>My Name is &FullName;. </student>\r\n",
  0, NULL
},
{
  "ibm10n01.xml",
  357,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)>\r\n\t<!ATTLIST student\r\n\t\tfirst CDATA #REQUIRED\r\n\t\tmiddle CDATA #IMPLIED\r\n\t\tlast CDATA #IMPLIED > \r\n\t<!ENTITY myfirst \"Snow\">\r\n\t<!ENTITY mymiddle \"I\">\r\n\t<!ENTITY mylast \"Man\">\r\n]>\r\n\r\n<!-- testing invalid attvalue -->\r\n<student first=\"Snow<Man\">My Name is SnowMan. </student>\r\n\r\n\r\n\r\n\r\n\r\n",
  0, NULL
},
{
  "ibm10n02.xml",
  347,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)>\r\n\t<!ATTLIST student\r\n\t\tfirst CDATA #REQUIRED\r\n\t\tmiddle CDATA #IMPLIED\r\n\t\tlast CDATA #IMPLIED > \r\n\t<!ENTITY myfirst \"Snow\">\r\n\t<!ENTITY mymiddle \"I\">\r\n\t<!ENTITY mylast \"Man\">\r\n]>\r\n\r\n<!-- testing invalid attvalue -->\r\n<student first=\"Snow&Man\">My Name is SnowMan. </student>\r\n",
  0, NULL
},
{
  "ibm10n03.xml",
  347,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)>\r\n\t<!ATTLIST student\r\n\t\tfirst CDATA #REQUIRED\r\n\t\tmiddle CDATA #IMPLIED\r\n\t\tlast CDATA #IMPLIED > \r\n\t<!ENTITY myfirst \"Snow\">\r\n\t<!ENTITY mymiddle \"I\">\r\n\t<!ENTITY mylast \"Man\">\r\n]>\r\n\r\n<!-- testing invalid attvalue -->\r\n<student first=\"Snow\"Man\">My Name is SnowMan. </student>\r\n",
  0, NULL
},
{
  "ibm10n04.xml",
  367,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)>\r\n\t<!ATTLIST student\r\n\t\tfirst CDATA #REQUIRED\r\n\t\tmiddle CDATA #IMPLIED\r\n\t\tlast CDATA #IMPLIED > \r\n\t<!ENTITY myfirst \"Snow\">\r\n\t<!ENTITY mymiddle \"I\">\r\n\t<!ENTITY mylast \"Man\">\r\n]>\r\n\r\n<!-- testing invalid attvalue with no closing bracket -->\r\n<student first=\"Snow >My Name is SnowMan. </student>\r\n",
  0, NULL
},
{
  "ibm10n05.xml",
  347,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)>\r\n\t<!ATTLIST student\r\n\t\tfirst CDATA #REQUIRED\r\n\t\tmiddle CDATA #IMPLIED\r\n\t\tlast CDATA #IMPLIED > \r\n\t<!ENTITY myfirst \"SNow\">\r\n\t<!ENTITY mymiddle \"I\">\r\n\t<!ENTITY mylast \"Man\">\r\n]>\r\n\r\n<!-- testing invalid attvalue -->\r\n<student first='Snow<Man'>My Name is SnowMan. </student>\r\n",
  0, NULL
},
{
  "ibm10n06.xml",
  347,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)>\r\n\t<!ATTLIST student\r\n\t\tfirst CDATA #REQUIRED\r\n\t\tmiddle CDATA #IMPLIED\r\n\t\tlast CDATA #IMPLIED > \r\n\t<!ENTITY myfirst \"Snow\">\r\n\t<!ENTITY mymiddle \"I\">\r\n\t<!ENTITY mylast \"Man\">\r\n]>\r\n\r\n<!-- testing invalid attvalue -->\r\n<student first='Snow&Man'>My Name is SnowMan. </student>\r\n",
  0, NULL
},
{
  "ibm10n07.xml",
  347,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)>\r\n\t<!ATTLIST student\r\n\t\tfirst CDATA #REQUIRED\r\n\t\tmiddle CDATA #IMPLIED\r\n\t\tlast CDATA #IMPLIED > \r\n\t<!ENTITY myfirst \"Snow\">\r\n\t<!ENTITY mymiddle \"I\">\r\n\t<!ENTITY mylast \"Man\">\r\n]>\r\n\r\n<!-- testing invalid attvalue -->\r\n<student first=\"Snow\"Man\">My Name is SnowMan. </student>\r\n",
  0, NULL
},
{
  "ibm10n08.xml",
  372,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)>\r\n\t<!ATTLIST student\r\n\t\tfirst CDATA #REQUIRED\r\n\t\tmiddle CDATA #IMPLIED\r\n\t\tlast CDATA #IMPLIED > \r\n\t<!ENTITY myfirst \"Snow\">\r\n\t<!ENTITY mymiddle \"I\">\r\n\t<!ENTITY mylast \"Man\">\r\n]>\r\n\r\n<!-- testing invalid attvalue with no closing single quote -->\r\n<student first='Snow >My Name is SnowMan. </student>\r\n",
  0, NULL
},
{
  "ibm11n01.xml",
  207,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student SYSTEM \"student\".dtd\"[\r\n\t<!ELEMENT student (#PCDATA)> \r\n]>\r\n\r\n<!-- testing invalid system literal  -->\r\n<student>My Name is SnowMan. </student>\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n ",
  0, NULL
},
{
  "ibm11n02.xml",
  186,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student SYSTEM 'student'.dtd'[\r\n\t<!ELEMENT student (#PCDATA)> \r\n]>\r\n\r\n<!-- testing invalid system literal  -->\r\n<student>My Name is SnowMan. </student>\r\n",
  0, NULL
},
{
  "ibm11n03.xml",
  209,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student SYSTEM \"student.DTD [\r\n\t<!ELEMENT student (#PCDATA)> \r\n]>\r\n\r\n<!-- testing invalid system literal with no closing bracket  -->\r\n<student>My Name is SnowMan. </student>\r\n",
  0, NULL
},
{
  "ibm11n04.xml",
  209,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student SYSTEM 'student.DTD [\r\n\t<!ELEMENT student (#PCDATA)> \r\n]>\r\n\r\n<!-- testing invalid system literal with no closing bracket  -->\r\n<student>My Name is SnowMan. </student>\r\n",
  0, NULL
},
{
  "ibm12n01.xml",
  243,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)> \r\n\t<!ENTITY info PUBLIC \"..\\info.dtd>\r\n]>\r\n\r\n<!-- testing invalid pubid literal with no closing bracket  -->\r\n<student>My Name is &info;. </student>\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n ",
  0, NULL
},
{
  "ibm12n02.xml",
  201,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)> \r\n\t<!ENTITY info PUBLIC '..\\info'.dtd'>\r\n]>\r\n\r\n<!-- testing invalid pubid literal -->\r\n<student>My Name is &info;. </student>\r\n",
  0, NULL
},
{
  "ibm12n03.xml",
  222,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)> \r\n\t<!ENTITY info PUBLIC '..\\info.dtd>\r\n]>\r\n\r\n<!-- testing invalid pubid literal with no closing bracket  -->\r\n<student>My Name is &info;. </student>",
  0, NULL
},
{
  "ibm13n01.xml",
  202,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ENTITY info PUBLIC \"This is a {test} \" \"student.dtd\">\r\n]>\r\n\r\n<!-- testing invalid pubid char with {  -->\r\n<student>My Name is &info;. </student>\r\n\r\n\r\n\r\n\r\n ",
  0, NULL
},
{
  "ibm13n02.xml",
  191,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ENTITY info PUBLIC \"This is a test~. \" \"student.dtd\">\r\n]>\r\n\r\n<!-- testing invalid pubid char with ~  -->\r\n<student>My Name is &info;. </student>",
  0, NULL
},
{
  "ibm13n03.xml",
  206,
  "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n<!DOCTYPE student [\n\t<!ENTITY info PUBLIC \"This is a test \303\241 \" \"student.dtd\">\n]>\n\n<!-- testing invalid pubid char with \303\241  -->\n<student>My Name is &info;. </student>\n\n",
  0, NULL
},
{
  "ibm14n01.xml",
  255,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)>\r\n\t<!ATTLIST student first CDATA #REQUIRED\r\n\t\t\t  last  CDATA #IMPLIED>\r\n]>\r\n\r\n<!-- testing invalid chardata string  -->\r\n<student first=\"Snow\">My name is Snow ]]> Man</student>\r\n\r\n\r\n",
  0, NULL
},
{
  "ibm14n02.xml",
  247,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)>\r\n\t<!ATTLIST student first CDATA #REQUIRED\r\n\t\t\t  last  CDATA #IMPLIED>\r\n]>\r\n\r\n<!-- testing invalid chardata string  -->\r\n<student first=\"Snow\">My name is Snow <Man </student>",
  0, NULL
},
{
  "ibm14n03.xml",
  248,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)>\r\n\t<!ATTLIST student first CDATA #REQUIRED\r\n\t\t\t  last  CDATA #IMPLIED>\r\n]>\r\n\r\n<!-- testing invalid chardata string  -->\r\n<student first=\"Snow\">My name is Snow&Man </student>\r\n",
  0, NULL
},
{
  "ibm15n01.xml",
  182,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)> \r\n]>\r\n\r\n<!-- testing invalid comment  -->\r\n<!------>\r\n<student>My Name is SnowMan. </student>\r\n\r\n\r\n\r\n\r\n\r\n\r\n ",
  0, NULL
},
{
  "ibm15n02.xml",
  180,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)> \r\n]>\r\n\r\n<!-- testing invalid comment  -->\r\n<!-- Student's name ->\r\n<student>My Name is SnowMan. </student>",
  0, NULL
},
{
  "ibm15n03.xml",
  180,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)> \r\n]>\r\n\r\n<!-- testing invalid comment  -->\r\n<!- student file-1 -->\r\n<student>My Name is SnowMan. </student>",
  0, NULL
},
{
  "ibm15n04.xml",
  196,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)> \r\n]>\r\n\r\n<!-- testing invalid comment  -->\r\n<!--student phone number 408-777-8888 \r\n<student>My Name is SnowMan. </student>",
  0, NULL
},
{
  "ibm16n01.xml",
  214,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)> \r\n]>\r\n\r\n<!-- testing invalid PI with illegal sequence  -->\r\n<?MyInstruct This is ?> a test ?>\r\n<student>My Name is SnowMan. </student>\r\n\r\n\r\n",
  0, NULL
},
{
  "ibm16n02.xml",
  181,
  /* Next line hand-editted to breakup trigraphs */
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)> \r\n]>\r\n\r\n\r\n<!-- testing invalid PI with missing PITarget  -->\r\n<?" "?>\r\n<student>My Name is SnowMan. </student>",
  0, NULL
},
{
  "ibm16n03.xml",
  197,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)> \r\n]>\r\n\r\n\r\n<!-- testing invalid PI with wrong closing sequence  -->\r\n<?MyInstruct >\r\n<student>My Name is SnowMan. </student>",
  0, NULL
},
{
  "ibm16n04.xml",
  190,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)> \r\n]>\r\n\r\n\r\n<!-- testing invalid PI with missing sequence  -->\r\n<?MyInstruct \r\n<student>My Name is SnowMan. </student>",
  0, NULL
},
{
  "ibm17n01.xml",
  190,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)> \r\n]>\r\n\r\n\r\n<!-- testing invalid PITarget  -->\r\n<?XML This is a test ?>\r\n<student>My Name is SnowMan. </student>\r\n\r\n\r\n",
  0, NULL
},
{
  "ibm17n02.xml",
  182,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)> \r\n]>\r\n\r\n<!-- testing invalid PITarget  -->\r\n<?xML This is a test ?>\r\n<student>My Name is SnowMan. </student>",
  0, NULL
},
{
  "ibm17n03.xml",
  182,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)> \r\n]>\r\n\r\n<!-- testing invalid PITarget  -->\r\n<?xml This is a test ?>\r\n<student>My Name is SnowMan. </student>",
  0, NULL
},
{
  "ibm17n04.xml",
  182,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)> \r\n]>\r\n\r\n<!-- testing invalid PITarget  -->\r\n<?xmL This is a test ?>\r\n<student>My Name is SnowMan. </student>",
  0, NULL
},
{
  "ibm18n01.xml",
  207,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)> \r\n]>\r\n\r\n<!-- testing invalid CDSect with missing CDStart   -->\r\n<student>My Name is SnowMan. This is <normal> text]]></student>\r\n\r\n\r\n",
  0, NULL
},
{
  "ibm18n02.xml",
  205,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)> \r\n]>\r\n\r\n<!-- testing invalid CDSect with missing CDEnd  -->\r\n<student>My Name is SnowMan. <![CDATA[This is <normal> text </student>",
  0, NULL
},
{
  "ibm19n01.xml",
  183,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)> \r\n]>\r\n\r\n<!-- testing invalid CDStart -->\r\n<![cdata[This is a test]]>\r\n<student>My Name is SnowMan. </student>",
  0, NULL
},
{
  "ibm19n02.xml",
  190,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)> \r\n]>\r\n\r\n<!-- testing invalid CDStart -->\r\n<![[CDATA[This is a test]]>\r\n<student>My Name is SnowMan. </student>\r\n\r\n\r\n",
  0, NULL
},
{
  "ibm19n03.xml",
  183,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)> \r\n]>\r\n\r\n<!-- testing invalid CDStart -->\r\n<?[CDATA[This is a test]]>\r\n<student>My Name is SnowMan. </student>",
  0, NULL
},
{
  "ibm20n01.xml",
  226,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)> \r\n]>\r\n\r\n<!-- testing invalid CData with illegal sequence -->\r\n<![CDATA[<testing>This is ]]> a test</testing>]]>\r\n<student>My Name is SnowMan. </student>",
  0, NULL
},
{
  "ibm21n01.xml",
  187,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)> \r\n]>\r\n\r\n<!-- testing invalid CDEnd -->\r\n<![[CDATA[This is a test]>\r\n<student>My Name is SnowMan. </student>\r\n\r\n\r\n",
  0, NULL
},
{
  "ibm21n02.xml",
  182,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)> \r\n]>\r\n\r\n<!-- testing invalid CDEnd -->\r\n<![cdata[This is a test]]]>\r\n<student>My Name is SnowMan. </student>",
  0, NULL
},
{
  "ibm21n03.xml",
  181,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)> \r\n]>\r\n\r\n<!-- testing invalid CDEnd -->\r\n<![CDATA[This is a test])>\r\n<student>My Name is SnowMan. </student>",
  0, NULL
},
{
  "ibm22n01.xml",
  139,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc EMPTY>\r\n]>\r\n<?xml version=\"1.0\" encoding=\"ASCII\" ?>\r\n<doc/>\r\n<!-- Wrong ordering between DTD and XMLDecl -->",
  0, NULL
},
{
  "ibm22n02.xml",
  139,
  "<!-- Wrong ordering Misc, DTD and XMLDecl -->\r\n<?xml version=\"1.0\" encoding=\"ASCII\" ?>\r\n<!DOCTYPE doc [\r\n<!ELEMENT doc EMPTY>\r\n]>\r\n<doc/>\r\n",
  0, NULL
},
{
  "ibm22n03.xml",
  180,
  "<!-- Wrong ordering patter 4 -->\r\n<!DOCTYPE doc [\r\n<!ELEMENT doc EMPTY>\r\n]>\r\n<!-- Wrong ordering Misc, DTD, Misc, and XMLDecl -->\r\n<?xml version=\"1.0\" encoding=\"ASCII\" ?>\r\n<doc/>\r\n",
  0, NULL
},
{
  "ibm23n01.xml",
  135,
  "<?xml encoding=\"ASCII\" ?>\r\n<!DOCTYPE doc [\r\n   <!ELEMENT doc EMPTY>\r\n]>\r\n<doc/>\r\n<!-- Missing required field VersionInfo in XMLDecl -->",
  0, NULL
},
{
  "ibm23n02.xml",
  165,
  "<?xml encoding='ASCII' version='1.0'?>\r\n<!DOCTYPE doc [\r\n   <!ELEMENT doc EMPTY>\r\n]>\r\n<doc/>\r\n<!-- Wrong ordering between VersionInfo and EncodingDecl in XMLDecl -->",
  0, NULL
},
{
  "ibm23n03.xml",
  183,
  "<?xml encoding='ASCII' standalone='yes' version='1.0'?>\r\n<!DOCTYPE doc [\r\n   <!ELEMENT doc EMPTY>\r\n]>\r\n<doc/>\r\n<!-- Wrong ordering EncodingDecl, SDDecl, and VersionInfo in XMLDecl -->",
  0, NULL
},
{
  "ibm23n04.xml",
  110,
  "<?XML version='1.0'?>\r\n<!DOCTYPE doc [\r\n   <!ELEMENT doc EMPTY>\r\n]>\r\n<doc/>\r\n<!-- Wrong keyword in XMLDecl -->",
  0, NULL
},
{
  "ibm23n05.xml",
  153,
  "<?xml version='1.0' encoding='ASCII' standalone='yes' >\r\n<!DOCTYPE doc [\r\n   <!ELEMENT doc EMPTY>\r\n]>\r\n<doc/>\r\n<!-- Wrong closing sequence in XMLDecl -->",
  0, NULL
},
{
  "ibm23n06.xml",
  154,
  "<!xml version='1.0' encoding='ASCII' standalone='yes' ?>\r\n<!DOCTYPE doc [\r\n   <!ELEMENT doc EMPTY>\r\n]>\r\n<doc/>\r\n<!-- Wrong opening sequence in XMLDecl -->",
  0, NULL
},
{
  "ibm24n01.xml",
  117,
  "<?xml version= ?>\r\n<!DOCTYPE doc [\r\n   <!ELEMENT doc EMPTY>\r\n]>\r\n<doc/>\r\n<!-- VersioNum is missing in VersionInfo -->",
  0, NULL
},
{
  "ibm24n02.xml",
  113,
  "<?xmlversion='1.0' ?>\r\n<!DOCTYPE doc [\r\n   <!ELEMENT doc EMPTY>\r\n]>\r\n<doc/>\r\n<!-- S is missing in VersionInfo -->",
  0, NULL
},
{
  "ibm24n03.xml",
  115,
  "<?xml version'1.0' ?>\r\n<!DOCTYPE doc [\r\n   <!ELEMENT doc EMPTY>\r\n]> \r\n<doc/>\r\n<!-- Eq is missing in VersionInfo -->",
  0, NULL
},
{
  "ibm24n04.xml",
  125,
  "<?xml '1.0'=version ?>\r\n<!DOCTYPE doc [\r\n   <!ELEMENT doc EMPTY>\r\n]>\r\n<doc/>\r\n<!-- Wrong ordering VersionNum Eq 'version' -->",
  0, NULL
},
{
  "ibm24n05.xml",
  123,
  "<?xml version'1.0'= ?>\r\n<!DOCTYPE doc [\r\n   <!ELEMENT doc EMPTY>\r\n]>\r\n<doc/>\r\n<!-- Wrong ordering version VersionNum Eq -->",
  0, NULL
},
{
  "ibm24n06.xml",
  111,
  "<?xml Version='1.0' ?>\r\n<!DOCTYPE doc [\r\n   <!ELEMENT doc EMPTY>\r\n]>\r\n<doc/>\r\n<!-- Wrong key word 'Version' -->",
  0, NULL
},
{
  "ibm24n07.xml",
  111,
  "<?xml versioN='1.0' ?>\r\n<!DOCTYPE doc [\r\n   <!ELEMENT doc EMPTY>\r\n]>\r\n<doc/>\r\n<!-- Wrong key word 'versioN' -->",
  0, NULL
},
{
  "ibm24n08.xml",
  118,
  "<?xml version='1.0\" ?>\r\n<!DOCTYPE doc [\r\n   <!ELEMENT doc EMPTY>\r\n]>\r\n<doc/>\r\n<!-- Mismatched qotes in VersionInfo -->",
  0, NULL
},
{
  "ibm24n09.xml",
  117,
  "<?xml version='1.0 ?>\r\n<!DOCTYPE doc [\r\n   <!ELEMENT doc EMPTY>\r\n]>\r\n<doc/>\r\n<!-- Mismatched qotes in VersionInfo -->",
  0, NULL
},
{
  "ibm25n01.xml",
  113,
  "<?xml version=='1.0' ?>\r\n<!DOCTYPE doc [\r\n   <!ELEMENT doc EMPTY>\r\n]>\r\n<doc/>\r\n<!-- Wrong key word \"==\" in Eq -->",
  0, NULL
},
{
  "ibm25n02.xml",
  115,
  "<?xml version eq '1.0' ?>\r\n<!DOCTYPE doc [\r\n   <!ELEMENT doc EMPTY>\r\n]>\r\n<doc/>\r\n<!-- Wrong key word \"eq\" in Eq -->",
  0, NULL
},
{
  "ibm26n01.xml",
  124,
  "<?xml version='_#1.0' ?>\r\n<!DOCTYPE doc [\r\n   <!ELEMENT doc EMPTY>\r\n]>\r\n<doc/>\r\n<!-- illegal character '#' in VersionNum -->",
  0, NULL
},
{
  "ibm27n01.xml",
  155,
  "<?xml version=\"1.0\" ?>\r\n<!DOCTYPE animal [\r\n   <!ELEMENT animal ANY>\r\n]>\r\n<animal>Wrong type of Misc following this element!</animal>\r\n<!ELEMENT cat EMPTY>",
  0, NULL
},
{
  "ibm28n01.xml",
  127,
  "<?xml version=\"1.0\" encoding='UTF-8'?>\r\n<!DOCTYPE SYSTEM \"ibm28n01.dtd\">\r\n<!-- Name is missing in doctypedecl --> \r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm28n02.xml",
  150,
  "<?xml version=\"1.0\" encoding='UTF-8'?>\r\n<!DOCTYPE [\r\n<!ELEMENT animal EMPTY>\r\n] animal>\r\n<!-- Wrong ordering [ ] Name in doctypedecl --> \r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm28n03.xml",
  217,
  "<?xml version=\"1.0\" encoding='UTF-8'?>\r\n<!DOCTYPE SYSTEM \"ibm28n01.dtd\" animal [\r\n   <!ATTLIST animal color CDATA #REQUIRED>\r\n]>\r\n<!-- Wrong ordering ExternalID Name [ ] in doctypedecl --> \r\n<animal color=\"yellow\"/>\r\n",
  0, NULL
},
{
  "ibm28n04.xml",
  380,
  "<?xml version=\"1.0\" encoding='UTF-8'?>\r\n<!DOCTYPE aniaml [\r\n   <!ELEMENT animal ANY>\r\n   <!ENTITY generalE \"leopard\">\r\n   &generalE;\r\n   <!ENTITY % parameterE \"<!ELEMENT leopard EMPTY>\">\r\n   %parameterE;\r\n] animal>\r\n<!-- Wrong componet general entity reference occurs inside the DTD -->\r\n<!-- General entity sould be used in the document content --> \r\n<animal>&generalE</animal>\r\n",
  0, NULL
},
{
  "ibm28n05.xml",
  151,
  "<?xml version=\"1.0\" encoding='UTF-8'?>\r\n<!DOCtYPE animal [\r\n   <!ELEMENT animal EMPTY>\r\n]>\r\n<!-- Wrong keyword DOCTYPE in doctypedecl --> \r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm28n06.xml",
  152,
  "<?xml version=\"1.0\" encoding='UTF-8'?>\r\n<!DOCTYPE animal [\r\n   <!ELEMENT animal EMPTY>\r\n>\r\n<!-- Bracket mismatch in [ ] in doctypedecl --> \r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm28n07.xml",
  150,
  "<?xml version=\"1.0\" encoding='UTF-8'?>\r\n<!DOCTYPE animal {\r\n   <!ELEMENT animal EMPTY>\r\n]>\r\n<!-- Wrong bracket in [ ] in doctypedecl --> \r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm28n08.xml",
  152,
  "<?xml version=\"1.0\" encoding='UTF-8'?>\r\n<?DOCTYPE animal [\r\n   <!ELEMENT animal EMPTY>\r\n]>\r\n<!-- Wrong opening sequence in doctypedecl --> \r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm29n01.xml",
  498,
  "<!DOCTYPE animal [\r\n   <?xml version=\"1.0\" encoding=\"ASCII\" ?>\r\n   <!-- Illegal markupdecl in DTD --> \r\n   <!ELEMENT animal (cat|tiger|leopard)+>\r\n   <!ELEMENT cat EMPTY>\r\n   <!ELEMENT tiger (#PCDATA)>\r\n   <!ELEMENT leopard ANY>\r\n   <!ELEMENT small EMPTY>\r\n   <!ELEMENT big EMPTY>\r\n   <!ATTLIST tiger color CDATA #REQUIRED>\r\n]>\r\n<animal>\r\n   <cat/>\r\n   <tiger color=\"white\">This is a white tiger in Mirage!!</tiger>\r\n   <cat/>\r\n   <leopard>\r\n      <small/>\r\n      <big/>\r\n   </leopard>\r\n</animal>\r\n",
  0, NULL
},
{
  "ibm29n02.xml",
  255,
  "<?xml version=\"1.0\" encoding='UTF-8'?>\r\n<!DOCTYPE animal [\r\n   <!ELEMENT animal ANY>\r\n   <!ENTITY % parameterE \"leopard EMPTY>\">\r\n   <!ELEMENT %parameterE;\r\n]>\r\n<!-- Parameter reference appears inside elementdecl in DTD -->\r\n<animal>Any content</animal>\r\n",
  0, NULL
},
{
  "ibm29n03.xml",
  269,
  "<?xml version=\"1.0\" encoding='UTF-8'?>\r\n<!DOCTYPE animal [\r\n   <!ELEMENT animal ANY>\r\n   <!ENTITY % parameterE \"color\">\r\n   <!ATTLIST animal %parameterE; CDATA #IMPLIED>\r\n]>\r\n<!-- Parameter reference appears inside AttlistDecl in DTD -->\r\n<animal>Any content</animal>\r\n",
  0, NULL
},
{
  "ibm29n04.xml",
  268,
  "<?xml version=\"1.0\" encoding='UTF-8'?>\r\n<!DOCTYPE animal [\r\n   <!ELEMENT animal ANY>\r\n   <!ENTITY % parameterE \"A leopard\">\r\n   <!ENTITY content \"%parameterE;\">\r\n]>\r\n<!-- Parameter reference appears inside an entity declaration in DTD -->\r\n<animal>&content;</animal>\r\n",
  0, NULL
},
{
  "ibm29n05.xml",
  247,
  "<?xml version=\"1.0\" encoding='UTF-8'?>\r\n<!DOCTYPE animal [\r\n   <!ELEMENT animal ANY>\r\n   <!ENTITY % parameterE \"A music file ?>\">\r\n   <?music %parameterE;\r\n]>\r\n<!-- Parameter reference appears inside a PI in DTD -->\r\n<animal>Any content</animal>\r\n",
  0, NULL
},
{
  "ibm29n06.xml",
  258,
  "<?xml version=\"1.0\" encoding='UTF-8'?>\r\n<!DOCTYPE animal [\r\n   <!ELEMENT animal ANY>\r\n   <!ENTITY % parameterE \"A music file -->\">\r\n<!-- Parameter reference appears inside a comment in DTD -->\r\n   <!-- This is %parameterE;\r\n]>\r\n<animal>Any content</animal>\r\n",
  0, NULL
},
{
  "ibm29n07.xml",
  266,
  "<?xml version=\"1.0\" encoding='UTF-8'?>\r\n<!DOCTYPE animal [\r\n   <!ELEMENT animal ANY>\r\n   <!ENTITY % parameterE \"cat SYSTEM\">\r\n   <!NOTATION %parameterE; \"cat.txt\">\r\n]>\r\n<!-- Parameter reference appears inside a NotationDecl in DTD -->\r\n<animal>Any content</animal>\r\n",
  0, NULL
},
{
  "ibm30n01.xml",
  118,
  "<!DOCTYPE animal SYSTEM \"ibm30n01.dtd\">\r\n<animal/>\r\n<!-- Wrong ordering extSubsetDecl TextDecl in the external DTD -->",
  0, NULL
},
{
  "ibm31n01.xml",
  102,
  "<!DOCTYPE animal SYSTEM \"ibm31n01.dtd\">\r\n<animal/>\r\n<!-- Illegal extSubsetDecl in the external DTD -->",
  0, NULL
},
{
  "ibm32n01.xml",
  135,
  "<?xml version=\"1.0\"standalone=\"yes\" ?>\r\n<!DOCTYPE animal [\r\n   <!ELEMENT animal EMPTY>\r\n]>\r\n<!-- Missing a S in SDDecl -->\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm32n02.xml",
  134,
  "<?xml version=\"1.0\" standalone\"yes\" ?>\r\n<!DOCTYPE animal [\r\n   <!ELEMENT animal EMPTY>\r\n]>\r\n<!-- Missing Eq in SDDecl -->\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm32n03.xml",
  138,
  "<?xml version=\"1.0\" Standalone=\"yes\" ?>\r\n<!DOCTYPE animal [\r\n   <!ELEMENT animal EMPTY>\r\n]>\r\n<!-- Wrong keyword in SDDecl -->\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm32n04.xml",
  138,
  "<?xml version=\"1.0\" standalone=\"Yes\" ?>\r\n<!DOCTYPE animal [\r\n   <!ELEMENT animal EMPTY>\r\n]>\r\n<!-- Wrong keyword in SDDecl -->\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm32n05.xml",
  138,
  "<?xml version=\"1.0\" standalone=\"YES\" ?>\r\n<!DOCTYPE animal [\r\n   <!ELEMENT animal EMPTY>\r\n]>\r\n<!-- Wrong keyword in SDDecl -->\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm32n06.xml",
  126,
  "<?xml version=\"1.0\" standalone=\"No\" ?>\r\n<!DOCTYPE animal SYSTEM \"ibm32n06.dtd\">\r\n<!-- Wrong keyword in SDDecl -->\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm32n07.xml",
  126,
  "<?xml version=\"1.0\" standalone=\"NO\" ?>\r\n<!DOCTYPE animal SYSTEM \"ibm32n06.dtd\">\r\n<!-- Wrong keyword in SDDecl -->\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm32n08.xml",
  139,
  "<?xml version=\"1.0\" standalone\"Yes\"= ?>\r\n<!DOCTYPE animal [\r\n   <!ELEMENT animal EMPTY>\r\n]>\r\n<!-- Wrong ordering in SDDecl -->\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm32n09.xml",
  423,
  "<?xml version=\"1.0\" standalone=\"yes\" ?>\r\n<!DOCTYPE animal SYSTEM \"ibm32n09.dtd\" [\r\n   <!ELEMENT animal (#PCDATA)>\r\n]>\r\n<!-- This is test violates WFC: Entity Declared in P68\r\n The standalone document declaration has the value \"yes\", there is an \r\n external markup declaration of an entity (other than amp, lt, gt, apos, quot), and references to this entity appear in the document. \r\n-->\r\n<animal>&animal_content;</animal>\r\n",
  0, NULL
},
{
  "ibm39n01.xml",
  97,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n]>\r\n<root>missing end tag\r\n\r\n",
  0, NULL
},
{
  "ibm39n02.xml",
  98,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n]>\r\nmissing start tag</root>\r\n",
  0, NULL
},
{
  "ibm39n03.xml",
  122,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n]>\r\n<root>\r\n<!--* Missing end tag and no content -->\r\n",
  0, NULL
},
{
  "ibm39n04.xml",
  129,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n]>\r\n</root>\r\n<!--* Missing start tag and no content -->\r\n\r\n\r\n",
  0, NULL
},
{
  "ibm39n05.xml",
  114,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n]>\r\n</root>switched start and end tags<root>\r\n",
  0, NULL
},
{
  "ibm39n06.xml",
  108,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n]>\r\n<root></root>content after end tag\r\n",
  0, NULL
},
{
  "ibm40n01.xml",
  195,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root ANY>\r\n<!ATTLIST root attr1 CDATA #IMPLIED>\r\n<!ATTLIST root attr2 CDATA #IMPLIED>\r\n]>\r\n<attr1=\"any\">missing name in start tag</root>\r\n\r\n\r\n\r\n",
  0, NULL
},
{
  "ibm40n02.xml",
  200,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root ANY>\r\n<!ATTLIST root attr1 CDATA #IMPLIED>\r\n<!ATTLIST root attr2 CDATA #IMPLIED>\r\n]>\r\n<rootattr1=\"any\">missing white space in start tag</root>\r\n",
  0, NULL
},
{
  "ibm40n03.xml",
  196,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root ANY>\r\n<!ATTLIST root attr1 CDATA #IMPLIED>\r\n<!ATTLIST root attr2 CDATA #IMPLIED>\r\n]>\r\n<attr1=\"any\" root>Wrong ordering in start tag</root>\r\n",
  0, NULL
},
{
  "ibm40n04.xml",
  206,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root ANY>\r\n<!ATTLIST root attr1 CDATA #IMPLIED>\r\n<!ATTLIST root attr2 CDATA #IMPLIED>\r\n]>\r\n<!root attr1=\"any\">wrong begining sequence in start tag</root>\r\n",
  0, NULL
},
{
  "ibm40n05.xml",
  220,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root ANY>\r\n<!ATTLIST root attr1 CDATA #IMPLIED>\r\n<!ATTLIST root attr2 CDATA #IMPLIED>\r\n]>\r\n<root attr1=\"any1\" attr1=\"any2\">duplicate attr names in start tag</root>\r\n\r\n\r\n",
  0, NULL
},
{
  "ibm41n01.xml",
  189,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root ANY>\r\n<!ATTLIST root attr1 CDATA #IMPLIED>\r\n<!ATTLIST root attr2 CDATA #IMPLIED>\r\n]>\r\n<root =\"any\">missing name in Attribute</root>\r\n",
  0, NULL
},
{
  "ibm41n02.xml",
  191,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root ANY>\r\n<!ATTLIST root attr1 CDATA #IMPLIED>\r\n<!ATTLIST root attr2 CDATA #IMPLIED>\r\n]>\r\n<root attr1\"any\">missing Eq in Attribute</root>\r\n",
  0, NULL
},
{
  "ibm41n03.xml",
  194,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root ANY>\r\n<!ATTLIST root attr1 CDATA #IMPLIED>\r\n<!ATTLIST root attr2 CDATA #IMPLIED>\r\n]>\r\n<root attr1= >missing AttValue in Attribute</root>\r\n",
  0, NULL
},
{
  "ibm41n04.xml",
  195,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root ANY>\r\n<!ATTLIST root attr1 CDATA #IMPLIED>\r\n<!ATTLIST root attr2 CDATA #IMPLIED>\r\n]>\r\n<root \"any\">missing name and Eq in Attribute</root>\r\n",
  0, NULL
},
{
  "ibm41n05.xml",
  199,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root ANY>\r\n<!ATTLIST root attr1 CDATA #IMPLIED>\r\n<!ATTLIST root attr2 CDATA #IMPLIED>\r\n]>\r\n<root attr1>missing Eq and AttValue in Attribute</root>\r\n",
  0, NULL
},
{
  "ibm41n06.xml",
  198,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root ANY>\r\n<!ATTLIST root attr1 CDATA #IMPLIED>\r\n<!ATTLIST root attr2 CDATA #IMPLIED>\r\n]>\r\n<root = >missing Name and AttValue in Attribute</root>\r\n",
  0, NULL
},
{
  "ibm41n07.xml",
  196,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root ANY>\r\n<!ATTLIST root attr1 CDATA #IMPLIED>\r\n<!ATTLIST root attr2 CDATA #IMPLIED>\r\n]>\r\n<root attr1\"any\"=>wrong ordering in Attribute</root>\r\n",
  0, NULL
},
{
  "ibm41n08.xml",
  196,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root ANY>\r\n<!ATTLIST root attr1 CDATA #IMPLIED>\r\n<!ATTLIST root attr2 CDATA #IMPLIED>\r\n]>\r\n<root \"any\"=attr1>wrong ordering in Attribute</root>\r\n",
  0, NULL
},
{
  "ibm41n09.xml",
  196,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root ANY>\r\n<!ATTLIST root attr1 CDATA #IMPLIED>\r\n<!ATTLIST root attr2 CDATA #IMPLIED>\r\n]>\r\n<root =attr1\"any\">wrong ordering in Attribute</root>\r\n",
  0, NULL
},
{
  "ibm41n10.xml",
  270,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root ANY>\r\n<!ATTLIST root attr1 CDATA #IMPLIED>\r\n<!ATTLIST root attr2 CDATA #IMPLIED>\r\n<!ENTITY aExternal SYSTEM \"ibm41n10.ent\">\r\n]>\r\n<root attr1=\"&aExternal;\">direct reference to external entinity in Attribute</root>\r\n",
  0, NULL
},
{
  "ibm41n11.xml",
  307,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root ANY>\r\n<!ATTLIST root attr1 CDATA #IMPLIED>\r\n<!ATTLIST root attr2 CDATA #IMPLIED>\r\n<!ENTITY aExternal SYSTEM \"ibm41n11.ent\">\r\n<!ENTITY aIndirect \"&aExternal;\">\r\n]>\r\n<root attr1=\"&aIndirect;\">indirect reference to external entinity in Attribute</root>\r\n",
  0, NULL
},
{
  "ibm41n12.xml",
  330,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root ANY>\r\n<!ATTLIST root attr1 CDATA #IMPLIED>\r\n<!ATTLIST root attr2 CDATA #IMPLIED>\r\n<!NOTATION JPGformat SYSTEM \"JPGFormat\">\r\n<!ENTITY aImage SYSTEM \"image.jpg\" NDATA JPGformat>\r\n]>\r\n<root attr1=\"&aImage;\">direct reference to external unparsed entinity in Attribute</root>\r\n\r\n",
  0, NULL
},
{
  "ibm41n13.xml",
  301,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root ANY>\r\n<!ATTLIST root attr1 CDATA #IMPLIED>\r\n<!ATTLIST root attr2 CDATA #IMPLIED>\r\n<!ENTITY withlt \"have <lessthan> inside\">\r\n]>\r\n<root attr1=\"&withlt;\">Direct reference to an entity with &lt; as part of its replacement text in Attribute</root>\r\n",
  0, NULL
},
{
  "ibm41n14.xml",
  338,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root ANY>\r\n<!ATTLIST root attr1 CDATA #IMPLIED>\r\n<!ATTLIST root attr2 CDATA #IMPLIED>\r\n<!ENTITY withlt \"have <lessthan> inside\">\r\n<!ENTITY aIndirect \"&withlt;\">\r\n]>\r\n<root attr1=\"&aIndirect;\">indirect reference to an entity with &lt; as part of its replacement text in Attribute</root>\r\n",
  0, NULL
},
{
  "ibm42n01.xml",
  147,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root ANY>\r\n<!ATTLIST root attr1 CDATA #IMPLIED>\r\n]>\r\n<root attr1=\"any\">missing Name in ETag</>\r\n",
  0, NULL
},
{
  "ibm42n02.xml",
  163,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root ANY>\r\n<!ATTLIST root attr1 CDATA #IMPLIED>\r\n]>\r\n<root attr1=\"any\">Wrong begining sequence in ETag <\\root>\r\n",
  0, NULL
},
{
  "ibm42n03.xml",
  162,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root ANY>\r\n<!ATTLIST root attr1 CDATA #IMPLIED>\r\n]>\r\n<root attr1=\"any\">Wrong begining sequence in ETag <root>\r\n",
  0, NULL
},
{
  "ibm42n04.xml",
  170,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root ANY>\r\n<!ATTLIST root attr1 CDATA #IMPLIED>\r\n]>\r\n<root attr1=\"any\">Extra white space before Name in ETag </ root>\r\n",
  0, NULL
},
{
  "ibm42n05.xml",
  150,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root ANY>\r\n<!ATTLIST root attr1 CDATA #IMPLIED>\r\n]>\r\n<root> Attribute in ETag </root attr1=\"any\">\r\n",
  0, NULL
},
{
  "ibm43n01.xml",
  204,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!ENTITY % paaa \"a string\">\r\n]>\r\n<root>\r\n<!NOTATION nota1 SYSTEM \"bogus.not\">\r\n</root>\r\n<!--* NotationDecl in content -->",
  0, NULL
},
{
  "ibm43n02.xml",
  188,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!ENTITY % paaa \"a string\">\r\n]>\r\n<root>\r\n<!ELEMENT ele1 ANY>\r\n</root>\r\n<!--* ElementDecl in content -->\r\n",
  0, NULL
},
{
  "ibm44n01.xml",
  202,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root EMPTY>\r\n<!ATTLIST root attr1 CDATA #IMPLIED>\r\n<!ATTLIST root attr2 CDATA #IMPLIED>\r\n]>\r\n< />\r\n<!--* Missing Name and Attribute EmptyElemTag *-->\r\n",
  0, NULL
},
{
  "ibm44n02.xml",
  231,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root EMPTY>\r\n<!ATTLIST root attr1 CDATA #IMPLIED>\r\n<!ATTLIST root attr2 CDATA #IMPLIED>\r\n]>\r\n<attr1=\"any\" root/>\r\n<!--* Swithech positions of Name and Attribute EmptyElemTag *-->\r\n",
  0, NULL
},
{
  "ibm44n03.xml",
  224,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root EMPTY>\r\n<!ATTLIST root attr1 CDATA #IMPLIED>\r\n<!ATTLIST root attr2 CDATA #IMPLIED>\r\n]>\r\n<root attr1=\"any\"\\>\r\n<!--* Wrong closing sequence in EmptyElemTag *-->\r\n\r\n\r\n\r\n\r\n",
  0, NULL
},
{
  "ibm44n04.xml",
  232,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root EMPTY>\r\n<!ATTLIST root attr1 CDATA #IMPLIED>\r\n<!ATTLIST root attr2 CDATA #IMPLIED>\r\n]>\r\n<root attr1=\"any1\" attr1=\"any2\"/>\r\n<!--* Duplicate Attribute Name in EmptyElemTag *-->\r\n",
  0, NULL
},
{
  "ibm45n01.xml",
  163,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!--* Mising Name in elementdecl *-->\r\n<!ELEMENT (#PCDATA)>\r\n]>\r\n<root>Any content</root>\r\n\r\n\r\n",
  0, NULL
},
{
  "ibm45n02.xml",
  170,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!--* Mising white space in elementdecl *-->\r\n<!ELEMENT aEle(#PCDATA)>\r\n]>\r\n<root>Any content</root>\r\n",
  0, NULL
},
{
  "ibm45n03.xml",
  162,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!--* Mising contentspec in elementdecl *-->\r\n<!ELEMENT root >\r\n]>\r\n<root>Any content</root>\r\n",
  0, NULL
},
{
  "ibm45n04.xml",
  177,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!--* Mising contentspec and white space in elementdecl *-->\r\n<!ELEMENT root>\r\n]>\r\n<root>Any content</root>\r\n",
  0, NULL
},
{
  "ibm45n05.xml",
  164,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!--* Mising Name S contentspec in elementdecl *-->\r\n<!ELEMENT >\r\n]>\r\n<root>Any content</root>\r\n",
  0, NULL
},
{
  "ibm45n06.xml",
  172,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!--* Wrong ordering in elementdecl *-->\r\n<!ELEMENT (#PCDATA) aElement >\r\n]>\r\n<root>Any content</root>\r\n",
  0, NULL
},
{
  "ibm45n07.xml",
  179,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!--* Wrong begining sequence in elementdecl *-->\r\n<ELEMENT aElement (#PCDATA)>\r\n]>\r\n<root>Any content</root>\r\n",
  0, NULL
},
{
  "ibm45n08.xml",
  179,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!--* Wrong Keyword: Element in elementdecl *-->\r\n<!Element aElement (#PCDATA)>\r\n]>\r\n<root>Any content</root>\r\n",
  0, NULL
},
{
  "ibm45n09.xml",
  179,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!--* Wrong keyword: element in elementdecl *-->\r\n<!element aElement (#PCDATA)>\r\n]>\r\n<root>Any content</root>\r\n",
  0, NULL
},
{
  "ibm46n01.xml",
  175,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!--* Wrong keyword: empty in contentspec *-->\r\n<!ELEMENT aElement empty>\r\n]>\r\n<root>Any content</root>\r\n\r\n",
  0, NULL
},
{
  "ibm46n02.xml",
  174,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!--* Wrong keyword: Empty in contentspec *-->\r\n<!ELEMENT aElement Empty >\r\n]>\r\n<root>Any content</root>\r\n",
  0, NULL
},
{
  "ibm46n03.xml",
  169,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!--* Wrong keyword: Any in contentspec *-->\r\n<!ELEMENT aElement Any>\r\n]>\r\n<root>Any content</root>\r\n",
  0, NULL
},
{
  "ibm46n04.xml",
  170,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!--* Wrong keyword: any in contentspec *-->\r\n<!ELEMENT aElement any >\r\n]>\r\n<root>Any content</root>\r\n",
  0, NULL
},
{
  "ibm46n05.xml",
  180,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!--* Bogus content type: #CDATA in contentspec *-->\r\n<!ELEMENT aElement #CDATA>\r\n]>\r\n<root>Any content</root>\r\n",
  0, NULL
},
{
  "ibm47n01.xml",
  165,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!--* Missing choice|seq in children *-->\r\n<!ELEMENT aElement + >\r\n]>\r\n<root>Any content</root>\r\n",
  0, NULL
},
{
  "ibm47n02.xml",
  165,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!--* Missing choice|seq in children *-->\r\n<!ELEMENT aElement * >\r\n]>\r\n<root>Any content</root>\r\n",
  0, NULL
},
{
  "ibm47n03.xml",
  165,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!--* Missing choice|seq in children *-->\r\n<!ELEMENT aElement ? >\r\n]>\r\n<root>Any content</root>\r\n",
  0, NULL
},
{
  "ibm47n04.xml",
  188,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!ELEMENT a ANY>\r\n<!--* Wrong ordering in children *-->\r\n<!ELEMENT aElement *(a,a) >\r\n]>\r\n<root>Any content</root>\r\n\r\n\r\n",
  0, NULL
},
{
  "ibm47n05.xml",
  202,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!ELEMENT a ANY>\r\n<!ELEMENT b ANY>\r\n<!--* Wrong ordering in children *-->\r\n<!ELEMENT aElement +(a|a) >\r\n]>\r\n<root>Any content</root>\r\n",
  0, NULL
},
{
  "ibm47n06.xml",
  187,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!ELEMENT a ANY>\r\n<!--* Wrong keyword: ^  in children *-->\r\n<!ELEMENT aElement (a,a)^ >\r\n]>\r\n<root>Any content</root>\r\n",
  0, NULL
},
{
  "ibm48n01.xml",
  194,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!ELEMENT a ANY>\r\n<!--* Missing seq|choice|Name in cp *-->\r\n<!ELEMENT aElement ((a,a)|+) >\r\n]>\r\n<root>Any content</root>\r\n\r\n\r\n",
  0, NULL
},
{
  "ibm48n02.xml",
  184,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!ELEMENT a ANY>\r\n<!--* Missing seq|choice|Name in cp *-->\r\n<!ELEMENT aElement (*) >\r\n]>\r\n<root>Any content</root>\r\n",
  0, NULL
},
{
  "ibm48n03.xml",
  192,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!ELEMENT a ANY>\r\n<!--* Missing seq|choice|Name in cp *-->\r\n<!ELEMENT aElement (?|(a,a)|a) >\r\n]>\r\n<root>Any content</root>\r\n",
  0, NULL
},
{
  "ibm48n04.xml",
  182,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!ELEMENT a ANY>\r\n<!--* wrong ordering in cp *-->\r\n<!ELEMENT aElement (+(a,a)|a) >\r\n]>\r\n<root>Any content</root>\r\n",
  0, NULL
},
{
  "ibm48n05.xml",
  200,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!ELEMENT a ANY>\r\n<!ELEMENT b ANY>\r\n<!--* wrong ordering in cp *-->\r\n<!ELEMENT aElement (*(a|b),a) >\r\n]>\r\n<root>Any content</root>\r\n",
  0, NULL
},
{
  "ibm48n06.xml",
  181,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!ELEMENT a ANY>\r\n<!--* wrong ordering in cp *-->\r\n<!ELEMENT aElement (a, ?a) >\r\n]>\r\n<root>Any content</root>\r\n\r\n",
  0, NULL
},
{
  "ibm48n07.xml",
  184,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!ELEMENT a ANY>\r\n<!--* wrong keyword: ^ in cp *-->\r\n<!ELEMENT aElement ( a^ | a ) >\r\n]>\r\n<root>Any content</root>\r\n",
  0, NULL
},
{
  "ibm49n01.xml",
  176,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!ELEMENT a ANY>\r\n<!--* missing cp in choice *-->\r\n<!ELEMENT aElement (|)* >\r\n]>\r\n<root>Any content</root>\r\n",
  0, NULL
},
{
  "ibm49n02.xml",
  198,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!ELEMENT a ANY>\r\n<!ELEMENT b ANY>\r\n<!--* missing cp in choice *-->\r\n<!ELEMENT aElement (a |b|)* >\r\n]>\r\n<root>Any content</root>\r\n",
  0, NULL
},
{
  "ibm49n03.xml",
  205,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!ELEMENT a ANY>\r\n<!ELEMENT b ANY>\r\n<!--* wrong separator: !  in choice *-->\r\n<!ELEMENT aElement (a!b)+ >\r\n]>\r\n<root>Any content</root>\r\n",
  0, NULL
},
{
  "ibm49n04.xml",
  202,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!ELEMENT a ANY>\r\n<!ELEMENT b ANY>\r\n<!--* missing separator in choice *-->\r\n<!ELEMENT aElement (a b) >\r\n]>\r\n<root>Any content</root>\r\n",
  0, NULL
},
{
  "ibm49n05.xml",
  203,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!ELEMENT a ANY>\r\n<!ELEMENT b ANY>\r\n<!--* extra separator in choice *-->\r\n<!ELEMENT aElement (a ||b)* >\r\n]>\r\n<root>Any content</root>\r\n",
  0, NULL
},
{
  "ibm49n06.xml",
  212,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!ELEMENT a ANY>\r\n<!ELEMENT b ANY>\r\n<!--* missing closing bracket in choice *-->\r\n<!ELEMENT aElement (a |b * >\r\n]>\r\n<root>Any content</root>\r\n\r\n",
  0, NULL
},
{
  "ibm50n01.xml",
  190,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!ELEMENT a ANY>\r\n<!ELEMENT b ANY>\r\n<!--* missing cp in seq *-->\r\n<!ELEMENT aElement (,) >\r\n]>\r\n<root>Any content</root>\r\n",
  0, NULL
},
{
  "ibm50n02.xml",
  194,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!ELEMENT a ANY>\r\n<!ELEMENT b ANY>\r\n<!--* missing cp in seq *-->\r\n<!ELEMENT aElement (a,a,)+ >\r\n]>\r\n<root>Any content</root>\r\n",
  0, NULL
},
{
  "ibm50n03.xml",
  200,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!ELEMENT a ANY>\r\n<!ELEMENT b ANY>\r\n<!--* wrong separators in seq *-->\r\n<!ELEMENT aElement (a,a|b) >\r\n]>\r\n<root>Any content</root>\r\n",
  0, NULL
},
{
  "ibm50n04.xml",
  200,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!ELEMENT a ANY>\r\n<!ELEMENT b ANY>\r\n<!--* wrong separator in seq *-->\r\n<!ELEMENT aElement (a . b)* >\r\n]>\r\n<root>Any content</root>\r\n",
  0, NULL
},
{
  "ibm50n05.xml",
  203,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!ELEMENT a ANY>\r\n<!ELEMENT b ANY>\r\n<!--* extra separator in seq *-->\r\n<!ELEMENT aElement ((a|b),,a)? >\r\n]>\r\n<root>Any content</root>\r\n",
  0, NULL
},
{
  "ibm50n06.xml",
  207,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!ELEMENT a ANY>\r\n<!ELEMENT b ANY>\r\n<!--* missing separator in seq *-->\r\n<!ELEMENT aElement ((a|b) (b|a)) >\r\n]>\r\n<root>Any content</root>\r\n",
  0, NULL
},
{
  "ibm50n07.xml",
  205,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!ELEMENT a ANY>\r\n<!ELEMENT b ANY>\r\n<!--* wrong closing bracket in seq *-->\r\n<!ELEMENT aElement (a, b]* >\r\n]>\r\n<root>Any content</root>\r\n",
  0, NULL
},
{
  "ibm51n01.xml",
  213,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!ELEMENT a ANY>\r\n<!ELEMENT b ANY>\r\n<!--* wrong keyword : #pcdata in Mixed  *-->\r\n<!ELEMENT aElement (#pcdata)* >\r\n]>\r\n<root>Any content</root>\r\n",
  0, NULL
},
{
  "ibm51n02.xml",
  222,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!ELEMENT a ANY>\r\n<!ELEMENT b ANY>\r\n<!--* #PCDATA must be the first in Mixed  *-->\r\n<!ELEMENT aElement ( a | b|#PCDATA)* >\r\n]>\r\n<root>Any content</root>\r\n",
  0, NULL
},
{
  "ibm51n03.xml",
  205,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!ELEMENT a ANY>\r\n<!ELEMENT b ANY>\r\n<!--* Missing '|' in Mixed  *-->\r\n<!ELEMENT aElement ( #PCDATA a )* >\r\n]>\r\n<root>Any content</root>\r\n",
  0, NULL
},
{
  "ibm51n04.xml",
  209,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!ELEMENT a ANY>\r\n<!ELEMENT b ANY>\r\n<!--* wrong keyword: #CDATA in Mixed  *-->\r\n<!ELEMENT aElement (#CDATA) >\r\n]>\r\n<root>Any content</root>\r\n",
  0, NULL
},
{
  "ibm51n05.xml",
  213,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!ELEMENT a ANY>\r\n<!ELEMENT b ANY>\r\n<!--* Missing '* after ')' in Mixed  *-->\r\n<!ELEMENT aElement ( #PCDATA | a ) >\r\n]>\r\n<root>Any content</root>",
  0, NULL
},
{
  "ibm51n06.xml",
  217,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!ELEMENT a ANY>\r\n<!ELEMENT b ANY>\r\n<!--* Wrong closing bracket in Mixed  *-->\r\n<!ELEMENT aElement ( #PCDATA | a ]* >\r\n]>\r\n<root>Any content</root>\r\n",
  0, NULL
},
{
  "ibm51n07.xml",
  213,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!ELEMENT a ANY>\r\n<!ELEMENT b ANY>\r\n<!--* Missing closing bracket in Mixed  *-->\r\n<!ELEMENT aElement ( #PCDATA *>\r\n]>\r\n<root>Any content</root>\r\n",
  0, NULL
},
{
  "ibm52n01.xml",
  189,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!ELEMENT a ANY>\r\n<!--* Missing Name in AttlistDecl *-->\r\n<!ATTLIST attr1 CDATA #IMPLIED>\r\n]>\r\n<root>Any content</root>\r\n",
  0, NULL
},
{
  "ibm52n02.xml",
  197,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!ELEMENT a ANY>\r\n<!--* Missing white space after 'ATTLIST' *-->\r\n<!ATTLISTa attr1 ID #REQUIRED >\r\n]>\r\n<root>Any content</root>\r\n",
  0, NULL
},
{
  "ibm52n03.xml",
  223,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!ELEMENT a ANY>\r\n<!--* Wrong ordering in AttlistDecl *-->\r\n<!ATTLIST attr1 CDATA \"defaultAttValue\" a attr2 CDATA #IMPLIED>\r\n]>\r\n<root>Any content</root>\r\n",
  0, NULL
},
{
  "ibm52n04.xml",
  203,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!ELEMENT a ANY>\r\n<!--* Wrong keyword: Attlist in AttlistDecl *-->\r\n<!Attlist a attr1 CDATA #REQUIRED >\r\n]>\r\n<root>Any content</root>\r\n",
  0, NULL
},
{
  "ibm52n05.xml",
  200,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!ELEMENT a ANY>\r\n<!--* Missing closing bracket in AttlistDecl *-->\r\n<!ATTLIST a \r\n<!--* random *-->\r\n]>\r\n<root>Any content</root>\r\n",
  0, NULL
},
{
  "ibm52n06.xml",
  202,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!ELEMENT a ANY>\r\n<!--* Wrong begining sequence in AttlistDecl *-->\r\n<ATTLIST a attr1 CDATA \"default\">\r\n]>\r\n<root>Any content</root>\r\n",
  0, NULL
},
{
  "ibm53n01.xml",
  189,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!ELEMENT a ANY>\r\n<!--* Missing DefaultDecl in AttDef *-->\r\n<!ATTLIST a attr1 CDATA >\r\n]>\r\n<root>Any content</root>\r\n\r\n\r\n",
  0, NULL
},
{
  "ibm53n02.xml",
  225,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!ELEMENT a ANY>\r\n<!--* Missing white space between AttType and DefaultDecl in AttDef *-->\r\n<!ATTLIST a attr1 (abc|def)\"def\">\r\n]>\r\n<root>Any content</root>\r\n",
  0, NULL
},
{
  "ibm53n03.xml",
  183,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!ELEMENT a ANY>\r\n<!--* Missing AttType in AttDef *-->\r\n<!ATTLIST a attr1 #IMPLIED>\r\n]>\r\n<root>Any content</root>\r\n",
  0, NULL
},
{
  "ibm53n04.xml",
  219,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!ELEMENT a ANY>\r\n<!--* Missing white space between Name and AttType in AttDef *-->\r\n<!ATTLIST a attr1(abc|def) \"abc\" >\r\n]>\r\n<root>Any content</root>\r\n",
  0, NULL
},
{
  "ibm53n05.xml",
  182,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!ELEMENT a ANY>\r\n<!--* Missing Name in AttDef *-->\r\n<!ATTLIST a (abc|def) \"def\" >\r\n]>\r\n<root>Any content</root>\r\n",
  0, NULL
},
{
  "ibm53n06.xml",
  224,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!ELEMENT a ANY>\r\n<!--* Missing white space before Name in AttDef *-->\r\n<!ATTLIST a attr1 CDATA \"default\"attr2 ID #required>\r\n]>\r\n<root>Any content</root>\r\n",
  0, NULL
},
{
  "ibm53n07.xml",
  189,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!ELEMENT a ANY>\r\n<!--* Wrong ordering in AttDef *-->\r\n<!ATTLIST a (abc|def) attr1 \"abc\">\r\n]>\r\n<root>Any content</root>\r\n",
  0, NULL
},
{
  "ibm53n08.xml",
  190,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!ELEMENT a ANY>\r\n<!--* wrong ordering in AttDef *-->\r\n<!ATTLIST a \"default\" CDATA attr1 >\r\n]>\r\n<root>Any content</root>\r\n",
  0, NULL
},
{
  "ibm54n01.xml",
  217,
  "<?xml  version=\"1.0\"?>\r\n<!-- test for Production 54-->\r\n<!DOCTYPE AttrType\r\n[\r\n<!ELEMENT AttrType ANY>\r\n<!ELEMENT a EMPTY>\r\n<!ATTLIST a att BOGUSATTR #IMPLIED> \r\n]>\r\n<AttrType>\r\nGiving a Bogus attribute. \r\n</AttrType>",
  0, NULL
},
{
  "ibm54n02.xml",
  249,
  "<?xml  version=\"1.0\"?>\r\n<!-- test for Production 54-->\r\n<!DOCTYPE AttrType\r\n[\r\n<!ELEMENT AttrType ANY>\r\n<!ELEMENT a EMPTY>\r\n<!ATTLIST a att PCDATA #IMPLIED> \r\n]>\r\n<AttrType>\r\nGiving a wrong AttType for the attribute. \r\n<a att=\"23\" ></a>\r\n</AttrType>",
  0, NULL
},
{
  "ibm55n01.xml",
  248,
  "<?xml  version=\"1.0\"?>\r\n<!-- test syntax invalid for Production 55-->\r\n<!DOCTYPE AttrType\r\n[\r\n<!ELEMENT AttrType (#PCDATA)>\r\n<!ELEMENT a EMPTY>\r\n<!ATTLIST a att cdata #IMPLIED>  \r\n]>\r\n<AttrType>\r\nGiving a lowercase for CDATA attribute.\r\n</AttrType>",
  0, NULL
},
{
  "ibm55n02.xml",
  235,
  "<?xml  version=\"1.0\"?>\r\n<!-- test invalid syntax for Production 55-->\r\n<!DOCTYPE AttrType\r\n[\r\n<!ELEMENT AttrType (#PCDATA)>\r\n<!ELEMENT a EMPTY>\r\n<!ATTLIST a att #CDATA #IMPLIED> \r\n]>\r\n<AttrType>\r\nGiving a wrong character. \r\n</AttrType>",
  0, NULL
},
{
  "ibm55n03.xml",
  251,
  "<?xml  version=\"1.0\"?>\r\n<!-- test invalid syntax for Production 55-->\r\n<!DOCTYPE AttrType\r\n[\r\n<!ELEMENT AttrType (#PCDATA)>\r\n<!ELEMENT a EMPTY>\r\n<!ATTLIST a att CData #IMPLIED> \r\n]>\r\n<AttrType>\r\n Giving a wrong key word of the StringType.\r\n</AttrType>",
  0, NULL
},
{
  "ibm56n01.xml",
  204,
  "<?xml  version=\"1.0\"?>\r\n<!-- test for Production 56-->\r\n<!DOCTYPE root\r\n[\r\n<!ELEMENT root ANY>\r\n<!ELEMENT a EMPTY>\r\n<!ATTLIST a attr id #REQUIRED>\r\n]>\r\n<root>\r\nInvalid TokenizedType id(lowercase)\r\n</root>",
  0, NULL
},
{
  "ibm56n02.xml",
  215,
  "<?xml  version=\"1.0\"?>\r\n<!-- test for Production 56-->\r\n<!DOCTYPE root\r\n[\r\n<!ELEMENT root ANY>\r\n<!ELEMENT a EMPTY>\r\n<!ATTLIST a attr Idref #REQUIRED>\r\n]>\r\n<root>\r\nInvalid TokenizedType Idref(case sensitive)\r\n</root>",
  0, NULL
},
{
  "ibm56n03.xml",
  217,
  "<?xml  version=\"1.0\"?>\r\n<!-- test for Production 56-->\r\n<!DOCTYPE root\r\n[\r\n<!ELEMENT root ANY>\r\n<!ELEMENT a EMPTY>\r\n<!ATTLIST a attr IdRefs #REQUIRED>\r\n]>\r\n<root>\r\nInvalid TokenizedType IdRefs(case sensitive)\r\n</root>",
  0, NULL
},
{
  "ibm56n04.xml",
  217,
  "<?xml  version=\"1.0\"?>\r\n<!-- test for Production 56-->\r\n<!DOCTYPE root\r\n[\r\n<!ELEMENT root ANY>\r\n<!ELEMENT a EMPTY>\r\n<!ATTLIST a attr EntitY #REQUIRED>\r\n]>\r\n<root>\r\nInvalid TokenizedType EntitY(case sensitive)\r\n</root>",
  0, NULL
},
{
  "ibm56n05.xml",
  219,
  "<?xml  version=\"1.0\"?>\r\n<!-- test for Production 56-->\r\n<!DOCTYPE root\r\n[\r\n<!ELEMENT root ANY>\r\n<!ELEMENT a EMPTY>\r\n<!ATTLIST a attr nmTOKEN #REQUIRED>\r\n]>\r\n<root>\r\nInvalid TokenizedType nmTOKEN(case sensitive)\r\n</root>",
  0, NULL
},
{
  "ibm56n06.xml",
  221,
  "<?xml  version=\"1.0\"?>\r\n<!-- test for Production 56-->\r\n<!DOCTYPE root\r\n[\r\n<!ELEMENT root ANY>\r\n<!ELEMENT a EMPTY>\r\n<!ATTLIST a attr NMtokens #REQUIRED>\r\n]>\r\n<root>\r\nInvalid TokenizedType NMtokens(case sensitive)\r\n</root>",
  0, NULL
},
{
  "ibm56n07.xml",
  212,
  "<?xml  version=\"1.0\"?>\r\n<!-- test for Production 56-->\r\n<!DOCTYPE root\r\n[\r\n<!ELEMENT root ANY>\r\n<!ELEMENT a EMPTY>\r\n<!ATTLIST a attr #ID #REQUIRED>\r\n]>\r\n<root>\r\nInvalid TokenizedType #ID(Wrong Character)\r\n</root>",
  0, NULL
},
{
  "ibm57n01.xml",
  224,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 57-->\r\n<!DOCTYPE root\r\n [\r\n <!ELEMENT root EMPTY>\r\n <!ATTLIST root att NMTOKEN (a|b) #IMPLIED>\r\n ]>\r\n <root>\r\nThis test case tests the illegal enumerated types\r\n</root>",
  0, NULL
},
{
  "ibm58n01.xml",
  300,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 58-->\r\n<!DOCTYPE test\r\n [\r\n <!ELEMENT test ANY>\r\n <!ELEMENT one EMPTY>\r\n <!NOTATION this SYSTEM \"alpha\">\r\n <!ATTLIST one attr notation (this) #IMPLIED>\r\n ]>\r\n <test>\r\nThis is a Negative test with notation (name) \r\nIt is case sensitive.\r\n</test>",
  0, NULL
},
{
  "ibm58n02.xml",
  298,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 58-->\r\n<!DOCTYPE test\r\n [\r\n <!ELEMENT test ANY>\r\n <!ELEMENT one EMPTY>\r\n <!NOTATION this SYSTEM \"alpha\">\r\n <!ATTLIST one attr NOTATION this) #IMPLIED>\r\n ]>\r\n <test>\r\nThis is a Negative test with  (name) \r\nMissing the open parenthesis\r\n</test>",
  0, NULL
},
{
  "ibm58n03.xml",
  298,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 58-->\r\n<!DOCTYPE test\r\n [\r\n <!ELEMENT test ANY>\r\n <!ELEMENT one EMPTY>\r\n <!NOTATION this SYSTEM \"alpha\">\r\n <!ATTLIST one attr NOTATION () #IMPLIED>\r\n ]>\r\n <test>\r\nThis is a Negative test with  NOTATION () \r\nMissing the required field\r\n</test>",
  0, NULL
},
{
  "ibm58n04.xml",
  306,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 58-->\r\n<!DOCTYPE test\r\n [\r\n <!ELEMENT test ANY>\r\n <!ELEMENT one EMPTY>\r\n <!NOTATION this SYSTEM \"alpha\">\r\n <!ATTLIST one attr NOTATION (this #IMPLIED>\r\n ]>\r\n <test>\r\nThis is a Negative test with  NOTATION (Name \r\nMissing the closing brackets\r\n</test>",
  0, NULL
},
{
  "ibm58n05.xml",
  294,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 58-->\r\n<!DOCTYPE test\r\n [\r\n <!ELEMENT test ANY>\r\n <!ELEMENT one EMPTY>\r\n <!NOTATION this SYSTEM \"alpha\">\r\n <!ATTLIST one attr (this) NOTATION #IMPLIED>\r\n ]>\r\n <test>\r\nThis is a Negative test with (Name) NOTATION  \r\nWrong Ordering\r\n</test>",
  0, NULL
},
{
  "ibm58n06.xml",
  392,
  "<?xml  version=\"1.0\"?>\r\n<!-- Syntax test for Production 58-->\r\n<!DOCTYPE test\r\n [\r\n <!ELEMENT test ANY>\r\n <!ELEMENT one EMPTY>\r\n <!ELEMENT two EMPTY>\r\n <!NOTATION this SYSTEM \"alpha\">\r\n <!NOTATION that SYSTEM \"beta\">\r\n <!ATTLIST three attr NOTATION (this,that) #IMPLIED>\r\n ]>\r\n<test>\r\nNegative Test.\r\nThis test tests the presence of a correct seperator. There is a wrong seperator(,)\r\n</test>",
  0, NULL
},
{
  "ibm58n07.xml",
  299,
  "<?xml  version=\"1.0\"?>\r\n<!-- Syntax test for Production 58-->\r\n<!DOCTYPE test\r\n [\r\n <!ELEMENT test ANY>\r\n <!ELEMENT one EMPTY>\r\n <!ELEMENT two EMPTY>\r\n <!NOTATION this SYSTEM \"alpha\">\r\n <!ATTLIST three attr NOTATION(this) #IMPLIED>\r\n ]>\r\n<test>\r\nNegative Test.\r\nMissing space after NOTATION\r\n</test>",
  0, NULL
},
{
  "ibm58n08.xml",
  309,
  "<?xml  version=\"1.0\"?>\r\n<!-- Syntax test for Production 58-->\r\n<!DOCTYPE test\r\n [\r\n <!ELEMENT test ANY>\r\n <!ELEMENT one EMPTY>\r\n <!ELEMENT two EMPTY>\r\n <!NOTATION this SYSTEM \"alpha\">\r\n <!ATTLIST three attr NOTATION (\"this\") #IMPLIED>\r\n ]>\r\n<test>\r\nNegative Test.\r\nPresence of quotes around the value\r\n</test>",
  0, NULL
},
{
  "ibm59n01.xml",
  264,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 59-->\r\n<!DOCTYPE test\r\n [\r\n <!ELEMENT test ANY>\r\n <!ELEMENT one EMPTY>\r\n <!ELEMENT enum (#PCDATA)>\r\n <!ATTLIST one attr () #IMPLIED>\r\n ]>\r\n <test>\r\nThis is a Negative test\r\nMissing the required field\r\n</test>",
  0, NULL
},
{
  "ibm59n02.xml",
  269,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 59-->\r\n<!DOCTYPE test\r\n [\r\n <!ELEMENT test ANY>\r\n <!ELEMENT one EMPTY>\r\n <!ELEMENT enum (#PCDATA)>\r\n <!ATTLIST one attr (enum #IMPLIED>\r\n ]>\r\n <test>\r\nThis is a Negative test\r\nMissing the closing brackets\r\n</test>",
  0, NULL
},
{
  "ibm59n03.xml",
  300,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 59-->\r\n<!DOCTYPE test\r\n [\r\n <!ELEMENT test ANY>\r\n <!ELEMENT one EMPTY>\r\n <!ELEMENT two EMPTY>\r\n <!ELEMENT enum (#PCDATA)>\r\n <!ATTLIST one attr (enum,two) #IMPLIED>\r\n ]>\r\n <test>\r\nThis is a Negative test\r\nWrong Separator(, instead of |)\r\n</test>",
  0, NULL
},
{
  "ibm59n04.xml",
  287,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 59-->\r\n<!DOCTYPE test\r\n [\r\n <!ELEMENT test ANY>\r\n <!ELEMENT one EMPTY>\r\n <!ELEMENT enum (#PCDATA)>\r\n <!ATTLIST one attr (\"enum\") #IMPLIED>\r\n ]>\r\n <test>\r\nThis is a Negative test\r\nIllegal presence of quotes around the value\r\n</test>",
  0, NULL
},
{
  "ibm59n05.xml",
  271,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 59-->\r\n<!DOCTYPE test\r\n [\r\n <!ELEMENT test ANY>\r\n <!ELEMENT one EMPTY>\r\n <!ELEMENT enum (#PCDATA)>\r\n <!ATTLIST one attr enum) #IMPLIED>\r\n ]>\r\n <test>\r\nThis is a Negative test\r\nMissing the begining bracket  \r\n</test>",
  0, NULL
},
{
  "ibm59n06.xml",
  269,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 59-->\r\n<!DOCTYPE test\r\n [\r\n <!ELEMENT test ANY>\r\n <!ELEMENT one EMPTY>\r\n <!ELEMENT enum (#PCDATA)>\r\n <!ATTLIST one attr enum) #IMPLIED>\r\n ]>\r\n <test>\r\nThis is a Negative test\r\nMissing the Opening brackets\r\n</test>",
  0, NULL
},
{
  "ibm60n01.xml",
  258,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 60-->\r\n<!DOCTYPE Java \r\n [\r\n <!ELEMENT Java ANY>\r\n <!ELEMENT one EMPTY>\r\n <!ATTLIST one chapter CDATA #required>\r\n ]>\r\n<Java>\r\n<one chapter=\"Introduction\"></one>\r\nNegative Test. Case sensitive.\r\n</Java>",
  0, NULL
},
{
  "ibm60n02.xml",
  256,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 60-->\r\n<!DOCTYPE Java \r\n [\r\n <!ELEMENT Java ANY>\r\n <!ELEMENT one EMPTY>\r\n <!ATTLIST one chapter CDATA #Implied>\r\n ]>\r\n<Java>\r\n<one chapter=\"Introduction\"></one>\r\nNegative test. Case Sensitive\r\n</Java>",
  0, NULL
},
{
  "ibm60n03.xml",
  258,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 60-->\r\n<!DOCTYPE Java \r\n [\r\n <!ELEMENT Java ANY>\r\n <!ELEMENT one EMPTY>\r\n <!ATTLIST one chapter CDATA !IMPLIED>\r\n ]>\r\n<Java>\r\n<one chapter=\"Introduction\"></one>\r\nNegative Test. Wrong Character.\r\n</Java>",
  0, NULL
},
{
  "ibm60n04.xml",
  291,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 60-->\r\n<!DOCTYPE Java \r\n [\r\n <!ELEMENT Java ANY>\r\n <!ELEMENT one EMPTY>\r\n <!ATTLIST one chapter CDATA #FIXED >\r\n ]>\r\n<Java>\r\n<one chapter=\"Introduction\"></one>\r\nNegative test. Missing required field(#FIXED should have a value)\r\n</Java>",
  0, NULL
},
{
  "ibm60n05.xml",
  317,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 60-->\r\n<!DOCTYPE Java \r\n [\r\n <!ELEMENT Java ANY>\r\n <!ELEMENT one EMPTY>\r\n <!ATTLIST one chapter CDATA #FIXED\"Introduction\">\r\n ]>\r\n<Java>\r\n<one chapter=\"Introduction\"></one>\r\nNegative test. Missing required field(#FIXED should have a space before value)\r\n</Java>",
  0, NULL
},
{
  "ibm60n06.xml",
  269,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 60-->\r\n<!DOCTYPE Java \r\n [\r\n <!ELEMENT Java ANY>\r\n <!ELEMENT one EMPTY>\r\n <!ATTLIST one chapter CDATA \"Introduction\" #FIXED>\r\n ]>\r\n<Java>\r\n<one chapter=\"Introduction\"></one>\r\nNegative test. Wrong Ordering\r\n</Java>",
  0, NULL
},
{
  "ibm60n07.xml",
  397,
  "<?xml  version=\"1.0\"?>\r\n<!-- WFC test for Production 60-->\r\n<!DOCTYPE Java \r\n [\r\n <!ELEMENT Java ANY>\r\n <!ELEMENT one EMPTY>\r\n <!ENTITY avalue \"<Introduction\">\r\n <!ATTLIST one chapter CDATA #REQUIRED>\r\n ]>\r\n<Java>\r\n<one chapter=\"&avalue;\"></one>\r\nNegative test. \r\nThe replacement text of any entity referred to directly or indirectly \r\nin an attribute value contains a less than character\r\n</Java>",
  0, NULL
},
{
  "ibm60n08.xml",
  294,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 60-->\r\n<!DOCTYPE Java \r\n [\r\n <!ELEMENT Java ANY>\r\n <!ELEMENT one EMPTY>\r\n <!ATTLIST one chapter CDATA #REQUIRED #IMPLIED>\r\n ]>\r\n<Java>\r\n<one chapter=\"Introduction\"></one>\r\nNegative Test. More than one Default type declarations.\r\n</Java>\r\n",
  0, NULL
},
{
  "ibm61n01.xml",
  136,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 61-->\r\n<!DOCTYPE animal SYSTEM \"ibm61n01.dtd\">\r\n<animal>\r\n <tiger/>\r\n</animal>\r\n",
  0, NULL
},
{
  "ibm62n01.xml",
  198,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 62-->\r\n<!DOCTYPE animal SYSTEM \"ibm62n01.dtd\">\r\n<animal>\r\n <tiger/>\r\nNegative test. Test includeSect with include(Case sensitive)\r\n</animal>\r\n",
  0, NULL
},
{
  "ibm62n02.xml",
  175,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 62-->\r\n<!DOCTYPE animal SYSTEM \"ibm62n02.dtd\">\r\n<animal>\r\n <tiger/>\r\nNegative test.  An extra '[' is used.\r\n</animal>\r\n",
  0, NULL
},
{
  "ibm62n03.xml",
  184,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 62-->\r\n<!DOCTYPE animal SYSTEM \"ibm62n03.dtd\">\r\n<animal>\r\n <tiger/>\r\nNegative test.  Wrong character is used is used.\r\n</animal>",
  0, NULL
},
{
  "ibm62n04.xml",
  187,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 62-->\r\n<!DOCTYPE animal SYSTEM \"ibm62n04.dtd\">\r\n<animal>\r\n <tiger/>\r\nNegative test.  Missing the required field INCLUDE.\r\n</animal>",
  0, NULL
},
{
  "ibm62n05.xml",
  197,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 62-->\r\n<!DOCTYPE animal SYSTEM \"ibm62n05.dtd\">\r\n<animal>\r\n <tiger/>\r\nNegative test.  Missing the required field '[' after INCLUDE.\r\n</animal>",
  0, NULL
},
{
  "ibm62n06.xml",
  224,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 62-->\r\n<!DOCTYPE animal SYSTEM \"ibm62n06.dtd\">\r\n<animal>\r\n <tiger/>\r\nNegative test.  Wrong Ordering. External subset declaration prior to the keyword INCLUDE\r\n</animal>",
  0, NULL
},
{
  "ibm62n07.xml",
  176,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 62-->\r\n<!DOCTYPE animal SYSTEM \"ibm62n07.dtd\">\r\n<animal>\r\n <tiger/>\r\nNegative test. Missing closing sequence.\r\n</animal>",
  0, NULL
},
{
  "ibm62n08.xml",
  187,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 62-->\r\n<!DOCTYPE animal SYSTEM \"ibm62n08.dtd\">\r\n<animal>\r\n <tiger/>\r\nNegative test. Missing external subset declaration.\r\n</animal>",
  0, NULL
},
{
  "ibm63n01.xml",
  299,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 63-->\r\n<!DOCTYPE animal SYSTEM \"ibm63n01.dtd\"\r\n[\r\n<!ELEMENT animal ANY>\r\n<!ELEMENT tiger (#PCDATA)>\r\n<!ATTLIST animal a (tiger) #REQUIRED> \r\n]>\r\n<animal a = \"TIGER1\">\r\nNegative test. Case sensitive(ignore is used instead of IGNORE).\r\n</animal>",
  0, NULL
},
{
  "ibm63n02.xml",
  229,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 63-->\r\n<!DOCTYPE animal SYSTEM \"ibm63n02.dtd\"\r\n[\r\n<!ATTLIST attr a (tiger) #REQUIRED> \r\n]>\r\n<animal a = \"TIGER1\">\r\nNegative test. Extra '[' is used before IGNORE.\r\n</animal>",
  0, NULL
},
{
  "ibm63n03.xml",
  267,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 63-->\r\n<!DOCTYPE animal SYSTEM \"ibm63n03.dtd\"\r\n[\r\n<!ELEMENT animal ANY>\r\n<!ELEMENT tiger (#PCDATA)>\r\n<!ATTLIST animal a (tiger) #REQUIRED> \r\n]>\r\n<animal a = \"TIGER1\">\r\nNegative test.  Wrong character.\r\n</animal>",
  0, NULL
},
{
  "ibm63n04.xml",
  305,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 63-->\r\n<!DOCTYPE animal SYSTEM \"ibm63n04.dtd\"\r\n[\r\n<!ELEMENT animal ANY>\r\n<!ELEMENT tiger (#PCDATA)>\r\n<!ATTLIST animal a (tiger) #REQUIRED> \r\n]>\r\n<animal a = \"TIGER1\">\r\nNegative test.  Missing required field(The keyword IGNORE is missing).\r\n</animal>",
  0, NULL
},
{
  "ibm63n05.xml",
  305,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 63-->\r\n<!DOCTYPE animal SYSTEM \"ibm63n05.dtd\"\r\n[\r\n<!ELEMENT animal ANY>\r\n<!ELEMENT tiger (#PCDATA)>\r\n<!ATTLIST animal a (tiger) #REQUIRED> \r\n]>\r\n<animal a = \"TIGER1\">\r\nNegative test.  Missing required field( '[' is missing after IGNORE ).\r\n</animal>",
  0, NULL
},
{
  "ibm63n06.xml",
  252,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 63-->\r\n<!DOCTYPE animal SYSTEM \"ibm63n06.dtd\"\r\n[\r\n<!ATTLIST attr a (tiger) #REQUIRED> \r\n]>\r\n<animal a = \"TIGER1\">\r\nNegative test.  Wrong Ordering. Ignore sect contents preceding IGNORE.\r\n</animal>",
  0, NULL
},
{
  "ibm63n07.xml",
  276,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 63-->\r\n<!DOCTYPE animal SYSTEM \"ibm63n07.dtd\"\r\n[\r\n<!ELEMENT animal ANY>\r\n<!ELEMENT tiger (#PCDATA)>\r\n<!ATTLIST animal a (tiger) #REQUIRED> \r\n]>\r\n<animal a = \"TIGER1\">\r\nNegative test.  Missing closing sequence.\r\n</animal>",
  0, NULL
},
{
  "ibm64n01.xml",
  195,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 64-->\r\n<!DOCTYPE animal SYSTEM \"ibm64n01.dtd\"\r\n[\r\n<!ELEMENT animal ANY>\r\n]>\r\n<animal>\r\nNegative Test. Pattern2. Wrong character.\r\n</animal>",
  0, NULL
},
{
  "ibm64n02.xml",
  204,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 64-->\r\n<!DOCTYPE animal SYSTEM \"ibm64n02.dtd\"\r\n[\r\n<!ELEMENT animal ANY>\r\n]>\r\n<animal>\r\nNegative Test. Pattern3. Missing closing sequence.\r\n</animal>",
  0, NULL
},
{
  "ibm64n03.xml",
  204,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 64-->\r\n<!DOCTYPE animal SYSTEM \"ibm64n03.dtd\"\r\n[\r\n<!ELEMENT animal ANY>\r\n]>\r\n<animal>\r\nNegative Test. Pattern4. Missing opening sequence.\r\n</animal>",
  0, NULL
},
{
  "ibm65n01.xml",
  202,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 65-->\r\n<!DOCTYPE animal SYSTEM \"ibm65n01.dtd\"\r\n[\r\n<!ELEMENT animal ANY>\r\n]>\r\n<animal>\r\nNegative Test. Pattern1.Illegal sequence of ']]'\r\n</animal>",
  0, NULL
},
{
  "ibm65n02.xml",
  179,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 65-->\r\n<!DOCTYPE animal SYSTEM \"ibm65n02.dtd\"\r\n[\r\n<!ELEMENT animal ANY>\r\n]>\r\n<animal>\r\nNegative Test. Pattern2. \r\n</animal>",
  0, NULL
},
{
  "ibm66n01.xml",
  143,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root EMPTY>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!ENTITY aaa \"wrong charater reference: &#002f;\">\r\n]>\r\n<root/>\r\n",
  0, NULL
},
{
  "ibm66n02.xml",
  135,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root EMPTY>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n\r\n]>\r\n<root att=\"wrong character reference: &#x003a\"/>\r\n",
  0, NULL
},
{
  "ibm66n03.xml",
  140,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root EMPTY>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!ENTITY aaa \"wrong charater reference: &49;\">\r\n]>\r\n<root/>\r\n",
  0, NULL
},
{
  "ibm66n04.xml",
  132,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root EMPTY>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n\r\n]>\r\n<root att=\"wrong charater reference:&#5~0;\"/>\r\n",
  0, NULL
},
{
  "ibm66n05.xml",
  144,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root EMPTY>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!ENTITY aaa \"wrong charater reference: &#x002g;\">\r\n]>\r\n<root/>\r\n",
  0, NULL
},
{
  "ibm66n06.xml",
  134,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root EMPTY>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n\r\n]>\r\n<root att=\"wrong charater reference:&#x006G;\"/>\r\n",
  0, NULL
},
{
  "ibm66n07.xml",
  144,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root EMPTY>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!ENTITY aaa \"wrong charater reference: &#x0=2f;\">\r\n]>\r\n<root/>\r\n",
  0, NULL
},
{
  "ibm66n08.xml",
  133,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root EMPTY>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n\r\n]>\r\n<root att=\"wrong charater reference:&#56.0;\"/>\r\n",
  0, NULL
},
{
  "ibm66n09.xml",
  145,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root EMPTY>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!ENTITY aaa \"wrong charater reference: &#x00/2f;\">\r\n]>\r\n<root/>\r\n",
  0, NULL
},
{
  "ibm66n10.xml",
  133,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root EMPTY>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n\r\n]>\r\n<root att=\"wrong charater reference: &#51);\"/>\r\n",
  0, NULL
},
{
  "ibm66n11.xml",
  145,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root EMPTY>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!ENTITY aaa \"wrong charater reference: &#x00 2f;\">\r\n]>\r\n<root/>\r\n",
  0, NULL
},
{
  "ibm66n12.xml",
  148,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root EMPTY>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n\r\n]>\r\n<root att=\"wrong replacement charater reference: &#x0000;\" />\r\n",
  0, NULL
},
{
  "ibm66n13.xml",
  138,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root EMPTY>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n\r\n]>\r\n<root att=\"wrong replacement charater: &#x001f;\" />\r\n",
  0, NULL
},
{
  "ibm66n14.xml",
  139,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root EMPTY>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n\r\n]>\r\n<root att=\"wrong replacement charater : &#xfffe;\" />\r\n",
  0, NULL
},
{
  "ibm66n15.xml",
  138,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root EMPTY>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n\r\n]>\r\n<root att=\"wrong replacement charater: &#xffff;\" />\r\n",
  0, NULL
},
{
  "ibm68n01.xml",
  149,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!ENTITY aaa \"aString\">\r\n]>\r\n<root>missing entity name &;</root>\r\n",
  0, NULL
},
{
  "ibm68n02.xml",
  156,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!ENTITY aaa \"aString\">\r\n]>\r\n<root att=\"&aaa\">missing semi-colon</root>\r\n",
  0, NULL
},
{
  "ibm68n03.xml",
  163,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!ENTITY aaa \"aString\">\r\n]>\r\n<root>extra space after ampsand & aaa;</root>\r\n\r\n\r\n",
  0, NULL
},
{
  "ibm68n04.xml",
  173,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!ENTITY aaa \"aString\">\r\n]>\r\n<root att=\"&aAa;\">reference doesn't match delaration</root>\r\n",
  0, NULL
},
{
  "ibm68n05.xml",
  126,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n]>\r\n<root>undefined entitiy &aaa; </root>\r\n",
  0, NULL
},
{
  "ibm68n06.xml",
  225,
  "<?xml version=\"1.0\" standalone=\"yes\"?>\r\n<!DOCTYPE root SYSTEM \"ibm68n06.dtd\"\r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n\r\n]>\r\n<root att=\"&aaa;\">entity declared externally but standalone is yes</root>\r\n",
  0, NULL
},
{
  "ibm68n07.xml",
  203,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!--* Entity referenced before declared *-->\r\n<!ATTLIST root att1 CDATA \"&aaa;\">\r\n<!ENTITY aaa \"aString\">\r\n]>\r\n<root/>\r\n",
  0, NULL
},
{
  "ibm68n08.xml",
  275,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!ENTITY aaa \"aString\">\r\n<!NOTATION JPGformat SYSTEM \"JPGFormat\">\r\n<!ENTITY aImage SYSTEM \"image.jpg\" NDATA JPGformat>\r\n]>\r\n<root>unparsed entity reference in the wrong place &aImage;</root>\r\n",
  0, NULL
},
{
  "ibm68n09.xml",
  194,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!--* recursive entity reference *-->\r\n<!ENTITY aaa \"&bbb;\">\r\n<!ENTITY bbb \"&aaa;\">\r\n]>\r\n<root>&aaa;</root>\r\n\r\n",
  0, NULL
},
{
  "ibm68n10.xml",
  274,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!--* indirect recursive entity reference *-->\r\n<!ENTITY aaa \"&bbb;\">\r\n<!ENTITY bbb \"&ccc;\">\r\n<!ENTITY ccc \"&ddd;\">\r\n<!ENTITY ddd \"&eee;\">\r\n<!ENTITY eee \"&aaa;\">\r\n]>\r\n<root>&aaa;</root>\r\n\r\n\r\n",
  0, NULL
},
{
  "ibm69n01.xml",
  179,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ENTITY % paaa \"<!ATTLIST root att CDATA #IMPLIED>\">\r\n<!--* incorrect PE reference *-->\r\n%;\r\n<!ENTITY aaa \"aString\">\r\n]>\r\n<root/>\r\n",
  0, NULL
},
{
  "ibm69n02.xml",
  200,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ENTITY % paaa \"<!ATTLIST root att CDATA #IMPLIED>\">\r\n<!--* incorrect PE reference without semicolon *-->\r\n%paaa\r\n<!ENTITY aaa \"aString\">\r\n]>\r\n<root/>\r\n",
  0, NULL
},
{
  "ibm69n03.xml",
  224,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ENTITY % paaa \"<!ATTLIST root att CDATA #IMPLIED>\">\r\n<!--* incorrect PE reference with a extra white space charater *-->\r\n%paaa ;\r\n<!ENTITY aaa \"aString\">\r\n]>\r\n<root/>\r\n\r\n\r\n\r\n",
  0, NULL
},
{
  "ibm69n04.xml",
  214,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ENTITY % paaa \"<!ATTLIST root att CDATA #IMPLIED>\">\r\n<!--* incorrect PE reference with a extra white space char *-->\r\n% paaa;\r\n<!ENTITY aaa \"aString\">\r\n]>\r\n<root/>\r\n",
  0, NULL
},
{
  "ibm69n05.xml",
  259,
  "<?xml version=\"1.0\" standalone=\"yes\"?>\r\n<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!--* PE referenced before declared, against WFC: entity declared -->\r\n%paaa;\r\n<!ENTITY % paaa \"<!ATTLIST root att CDATA #IMPLIED>\">\r\n<!ENTITY aaa \"aString\">\r\n]>\r\n<root/>\r\n",
  0, NULL
},
{
  "ibm69n06.xml",
  144,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!--* recursive PE reference -->\r\n<!ENTITY % paaa \"&bbb;\">\r\n<!ENTITY bbb \"%paaa;\">\r\n]>\r\n<root/>\r\n",
  0, NULL
},
{
  "ibm69n07.xml",
  224,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!--* indirect recursive PE reference -->\r\n<!ENTITY % paaa \"&bbb;\">\r\n<!ENTITY bbb \"&ccc;\">\r\n<!ENTITY ccc \"&ddd;\">\r\n<!ENTITY ddd \"&eee;\">\r\n<!ENTITY eee \"%paaa;\">\r\n]>\r\n<root/>\r\n\r\n",
  0, NULL
},
{
  "ibm71n01.xml",
  162,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!--* missing space  *-->\r\n<!ENTITYaaa \"aString\">\r\n\r\n]>\r\n<root>&aaa;</root>\r\n\r\n",
  0, NULL
},
{
  "ibm71n02.xml",
  160,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!--* missing space  *-->\r\n<!ENTITY aaa\"aString\">\r\n\r\n]>\r\n<root>&aaa;</root>\r\n",
  0, NULL
},
{
  "ibm71n03.xml",
  155,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!--* missing EntityDef  *-->\r\n<!ENTITY aaa>\r\n\r\n]>\r\n<root>&aaa;</root>\r\n",
  0, NULL
},
{
  "ibm71n04.xml",
  156,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!--* missing name  *-->\r\n<!ENTITY \"aString\">\r\n\r\n]>\r\n<root>&aaa;</root>\r\n",
  0, NULL
},
{
  "ibm71n05.xml",
  160,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!--* wrong ordering  *-->\r\n<!ENTITY \"aString\" aaa>\r\n]>\r\n<root>&aaa;</root>\r\n",
  0, NULL
},
{
  "ibm71n06.xml",
  166,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!--* ENTITY in lower case  *-->\r\n<!entity aaa \"aString\">\r\n]>\r\n<root>&aaa;</root>\r\n",
  0, NULL
},
{
  "ibm71n07.xml",
  170,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!--* missing closing bracket  *-->\r\n<!ENTITY aaa \"aString\"\r\n\r\n]>\r\n<root>&aaa;</root>\r\n",
  0, NULL
},
{
  "ibm71n08.xml",
  171,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!--* exclamation mark missing  *-->\r\n<ENTITY aaa \"aString\">\r\n\r\n]>\r\n<root>&aaa;</root>\r\n",
  0, NULL
},
{
  "ibm72n01.xml",
  178,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!--* missing space  *-->\r\n<!ENTITY% paaa \"<!-- comments -->\">\r\n%paaa;\r\n]>\r\n<root/>\r\n\r\n\r\n\r\n\r\n\r\n",
  0, NULL
},
{
  "ibm72n02.xml",
  163,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!--* missing name  *-->\r\n<!ENTITY % \"<!-- comments -->\">\r\n%paaa;\r\n]>\r\n<root/>\r\n",
  0, NULL
},
{
  "ibm72n03.xml",
  168,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!--* missing space  *-->\r\n<!ENTITY % paaa\"<!-- comments -->\">\r\n%paaa;\r\n]>\r\n<root/>\r\n",
  0, NULL
},
{
  "ibm72n04.xml",
  149,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!--* missing PEDef  *-->\r\n<!ENTITY % paaa>\r\n%paaa;\r\n]>\r\n<root/>\r\n",
  0, NULL
},
{
  "ibm72n05.xml",
  167,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!--* wrong order  *-->\r\n<!ENTITY % \"<!-- comments -->\" paaa>\r\n%paaa;\r\n]>\r\n<root/>\r\n",
  0, NULL
},
{
  "ibm72n06.xml",
  168,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!--* wrong order  *-->\r\n<!ENTITY \"<!-- comments -->\" % paaa >\r\n%paaa;\r\n]>\r\n<root/>\r\n",
  0, NULL
},
{
  "ibm72n07.xml",
  169,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!--* wrong keyword  *-->\r\n<!entity % paaa \"<!-- comments -->\">\r\n%paaa;\r\n]>\r\n<root/>\r\n",
  0, NULL
},
{
  "ibm72n08.xml",
  178,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!--* missing closing bracket  *-->\r\n<!ENTITY % paaa \"<!-- comments -->\"\r\n%paaa;\r\n]>\r\n<root/>\r\n",
  0, NULL
},
{
  "ibm72n09.xml",
  202,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!--* wrong closing sequence: extra exclamation mark *-->\r\n<!ENTITY% paaa \"<!-- comments -->\" !>\r\n%paaa;\r\n]>\r\n<root/>\r\n",
  0, NULL
},
{
  "ibm73n01.xml",
  239,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!NOTATION JPGformat SYSTEM \"JPGFormat\">\r\n<!--* wrong order: NDataDecl ExternalID  *-->\r\n<!ENTITY aImage NDATA JPGformat SYSTEM \"image.jpg\" >\r\n]>\r\n<root/>\r\n",
  0, NULL
},
{
  "ibm74n01.xml",
  240,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!NOTATION JPGformat SYSTEM \"JPGFormat\">\r\n<!--* wrong PEDef: NDataDecl ExternalID  *-->\r\n<!ENTITY % pImage SYSTEM \"image.jpg\" NDATA JPGformat>\r\n]>\r\n<root/>\r\n",
  0, NULL
},
{
  "ibm75n01.xml",
  168,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!--* wrong keyword: system  *-->\r\n<!ENTITY pImage system \"image.jpg\">\r\n]>\r\n<root/>\r\n",
  0, NULL
},
{
  "ibm75n02.xml",
  196,
  "<?xml version=\"1.0\"?>\r\n<!--* wrong keyword: public  *-->\r\n<!DOCTYPE root \r\n    public \"-//W3C//DTD//EN\" \"empty.dtd\"\r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n]>\r\n<root/>\r\n",
  0, NULL
},
{
  "ibm75n03.xml",
  196,
  "<?xml version=\"1.0\"?>\r\n<!--* wrong keyword: Public  *-->\r\n<!DOCTYPE root \r\n    Public \"-//W3C//DTD//EN\" \"empty.dtd\"\r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n]>\r\n<root/>\r\n",
  0, NULL
},
{
  "ibm75n04.xml",
  185,
  "<?xml version=\"1.0\"?>\r\n<!--* wrong order *-->\r\n<!DOCTYPE root \r\n    \"-//W3C//DTD//EN\" \"empty.dtd\" PUBLIC\r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n]>\r\n<root/>\r\n",
  0, NULL
},
{
  "ibm75n05.xml",
  158,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!--* missing space *-->\r\n<!ENTITY pImage SYSTEM\"image.jpg\">\r\n]>\r\n<root/>\r\n",
  0, NULL
},
{
  "ibm75n06.xml",
  157,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!--* missing SystemLiterral *-->\r\n<!ENTITY pImage SYSTEM >\r\n]>\r\n<root/>\r\n",
  0, NULL
},
{
  "ibm75n07.xml",
  187,
  "<?xml version=\"1.0\"?>\r\n<!--* missing space  *-->\r\n<!DOCTYPE root \r\n    PUBLIC \"-//W3C//DTD//EN\"\"empty.dtd\"\r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n]>\r\n<root/>\r\n",
  0, NULL
},
{
  "ibm75n08.xml",
  191,
  "<?xml version=\"1.0\"?>\r\n<!--* missing keyword: PUBLIC  *-->\r\n<!DOCTYPE root \r\n    \"-//W3C//DTD//EN\" \"empty.dtd\"\r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n]>\r\n<root/>\r\n",
  0, NULL
},
{
  "ibm75n09.xml",
  187,
  "<?xml version=\"1.0\"?>\r\n<!--* missing space  *-->\r\n<!DOCTYPE root \r\n    PUBLIC\"-//W3C//DTD//EN\" \"empty.dtd\"\r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n]>\r\n<root/>\r\n",
  0, NULL
},
{
  "ibm75n10.xml",
  179,
  "<?xml version=\"1.0\"?>\r\n<!--* missing PubidLiteral  *-->\r\n<!DOCTYPE root \r\n    PUBLIC \".\\empty.dtd\"\r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n]>\r\n<root/>\r\n",
  0, NULL
},
{
  "ibm75n11.xml",
  184,
  "<?xml version=\"1.0\"?>\r\n<!--* missing System Literal *-->\r\n<!DOCTYPE root \r\n    public \"-//W3C//DTD//EN\"\r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n]>\r\n<root/>\r\n",
  0, NULL
},
{
  "ibm75n12.xml",
  157,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!--* wrong order *-->\r\n<!ENTITY pImage \"image.jpg\" SYSTEM>\r\n]>\r\n<root/>\r\n",
  0, NULL
},
{
  "ibm75n13.xml",
  185,
  "<?xml version=\"1.0\"?>\r\n<!--* wrong order *-->\r\n<!DOCTYPE root \r\n    \"-//W3C//DTD//EN\" PUBLIC \"empty.dtd\"\r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n]>\r\n<root/>\r\n",
  0, NULL
},
{
  "ibm76n01.xml",
  260,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!NOTATION JPGformat SYSTEM \"JPGFormat\">\r\n<!--* wrong keyword in NdataDecl: ndata *-->\r\n<!ENTITY aImage SYSTEM \"image.jpg\" ndata JPGformat>\r\n]>\r\n<root/>\r\n",
  0, NULL
},
{
  "ibm76n02.xml",
  260,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!NOTATION JPGformat SYSTEM \"JPGFormat\">\r\n<!--* wrong keyword in NdataDecl: NData *-->\r\n<!ENTITY aImage SYSTEM \"image.jpg\" NData JPGformat>\r\n]>\r\n<root/>\r\n",
  0, NULL
},
{
  "ibm76n03.xml",
  252,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!NOTATION JPGformat SYSTEM \"JPGFormat\">\r\n<!--* missing space in NdataDecl *-->\r\n<!ENTITY aImage SYSTEM \"image.jpg\"NDATA JPGformat>\r\n]>\r\n<root/>\r\n",
  0, NULL
},
{
  "ibm76n04.xml",
  257,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!NOTATION JPGformat SYSTEM \"JPGFormat\">\r\n<!--* missing keyword in NdataDecl : NDATA *-->\r\n<!ENTITY aImage SYSTEM \"image.jpg\" JPGformat>\r\n]>\r\n<root/>\r\n",
  0, NULL
},
{
  "ibm76n05.xml",
  248,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!NOTATION JPGformat SYSTEM \"JPGFormat\">\r\n<!--* Missing Name field in NdataDecl *-->\r\n<!ENTITY aImage SYSTEM \"image.jpg\" NDATA>\r\n]>\r\n<root/>\r\n",
  0, NULL
},
{
  "ibm76n06.xml",
  252,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!NOTATION JPGformat SYSTEM \"JPGFormat\">\r\n<!--* missing space in NdataDecl *-->\r\n<!ENTITY aImage SYSTEM \"image.jpg\" NDATAJPGformat>\r\n]>\r\n<root/>\r\n",
  0, NULL
},
{
  "ibm76n07.xml",
  251,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!NOTATION JPGformat SYSTEM \"JPGFormat\">\r\n<!--* wrong order in NdataDecl *-->\r\n<!ENTITY aImage SYSTEM \"image.jpg\" JPGformat NDATA>\r\n]>\r\n<root/>\r\n",
  0, NULL
},
{
  "ibm77n01.xml",
  194,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!--* error in external entity  *-->\r\n<!ENTITY aExternal SYSTEM \"ibm77n01.ent\">\r\n]>\r\n<root>&aExternal;</root>\r\n",
  0, NULL
},
{
  "ibm77n02.xml",
  194,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!--* error in external entity  *-->\r\n<!ENTITY aExternal SYSTEM \"ibm77n02.ent\">\r\n]>\r\n<root>&aExternal;</root>\r\n",
  0, NULL
},
{
  "ibm77n03.xml",
  192,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!--* error in external entity  *-->\r\n<!ENTITY % pExternal SYSTEM \"ibm77n03.ent\">\r\n%pExternal;\r\n]>\r\n<root/>\r\n",
  0, NULL
},
{
  "ibm77n04.xml",
  192,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!--* error in external entity  *-->\r\n<!ENTITY % pExternal SYSTEM \"ibm77n04.ent\">\r\n%pExternal;\r\n]>\r\n<root/>\r\n",
  0, NULL
},
{
  "ibm78n01.xml",
  200,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!--* error in external entity  *-->\r\n<!ENTITY aExternal SYSTEM \"ibm78n01.ent\">\r\n]>\r\n<root>&aExternal;</root>\r\n\r\n\r\n\r\n",
  0, NULL
},
{
  "ibm78n02.xml",
  194,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!--* error in external entity  *-->\r\n<!ENTITY aExternal SYSTEM \"ibm78n02.ent\">\r\n]>\r\n<root>&aExternal;</root>\r\n",
  0, NULL
},
{
  "ibm79n01.xml",
  192,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!--* error in external entity  *-->\r\n<!ENTITY % pExternal SYSTEM \"ibm79n01.ent\">\r\n%pExternal;\r\n]>\r\n<root/>\r\n",
  0, NULL
},
{
  "ibm79n02.xml",
  192,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!--* error in external entity  *-->\r\n<!ENTITY % pExternal SYSTEM \"ibm79n02.ent\">\r\n%pExternal;\r\n]>\r\n<root/>\r\n",
  0, NULL
},
{
  "ibm80n01.xml",
  189,
  "<?xml version=\"1.0\"encoding=\"UTF-8\"?>\r\n<!--* missing white space in above EncodingDecl *-->\r\n<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n]>\r\n<root/>\r\n",
  0, NULL
},
{
  "ibm80n02.xml",
  181,
  "<?xml version=\"1.0\" encoding \"UTF-8\"?>\r\n<!--* missing Eq in above EncodingDecl *-->\r\n<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n]>\r\n<root/>\r\n",
  0, NULL
},
{
  "ibm80n03.xml",
  180,
  "<?xml version=\"1.0\" encoding= ?>\r\n<!--* missing EncName in above EncodingDecl *-->\r\n<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n]>\r\n<root/>\r\n",
  0, NULL
},
{
  "ibm80n04.xml",
  185,
  "<?xml version=\"1.0\" \"UTF-8\"encoding=?>\r\n<!--* wrong ordering in above EncodingDecl *-->\r\n<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n]>\r\n<root/>\r\n",
  0, NULL
},
{
  "ibm80n05.xml",
  185,
  "<?xml version=\"1.0\" \"UTF-8\"=encoding?>\r\n<!--* wrong ordering in above EncodingDecl *-->\r\n<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n]>\r\n<root/>\r\n",
  0, NULL
},
{
  "ibm80n06.xml",
  193,
  "<?xml version=\"1.0\" Encoding=\"UTF-8\"?>\r\n<!--* Wrong keyword Encoding in above EncodingDecl *-->\r\n<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n]>\r\n<root/>\r\n",
  0, NULL
},
{
  "ibm81n01.xml",
  192,
  "<?xml version=\"1.0\" encoding=\"_UTF-8\"?>\r\n<!--* Illegal inital Charater in above EncName *-->\r\n<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n]>\r\n<root/>\r\n\r\n",
  0, NULL
},
{
  "ibm81n02.xml",
  190,
  "<?xml version=\"1.0\" encoding=\"-UTF-8\"?>\r\n<!--* Illegal inital Charater in above EncName *-->\r\n<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n]>\r\n<root/>\r\n",
  0, NULL
},
{
  "ibm81n03.xml",
  190,
  "<?xml version=\"1.0\" encoding=\".UTF-8\"?>\r\n<!--* Illegal inital Charater in above EncName *-->\r\n<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n]>\r\n<root/>\r\n",
  0, NULL
},
{
  "ibm81n04.xml",
  190,
  "<?xml version=\"1.0\" encoding=\"8-UTF\"?>\r\n<!--* Illegal initial Charater in above EncName *-->\r\n<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n]>\r\n<root/>\r\n",
  0, NULL
},
{
  "ibm81n05.xml",
  182,
  "<?xml version=\"1.0\" encoding=\"UTF~8\"?>\r\n<!--* Illegal Charater in above EncName *-->\r\n<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n]>\r\n<root/>\r\n",
  0, NULL
},
{
  "ibm81n06.xml",
  182,
  "<?xml version=\"1.0\" encoding=\"UTF#8\"?>\r\n<!--* Illegal Charater in above EncName *-->\r\n<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n]>\r\n<root/>\r\n",
  0, NULL
},
{
  "ibm81n07.xml",
  181,
  "<?xml version=\"1.0\" encoding=\"UTF:8\"?>\r\n<!--* IllegalCharater in above EncName *-->\r\n<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n]>\r\n<root/>\r\n",
  0, NULL
},
{
  "ibm81n08.xml",
  182,
  "<?xml version=\"1.0\" encoding=\"UTF/8\"?>\r\n<!--* Illegal Charater in above EncName *-->\r\n<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n]>\r\n<root/>\r\n",
  0, NULL
},
{
  "ibm81n09.xml",
  182,
  "<?xml version=\"1.0\" encoding=\"UTF;8\"?>\r\n<!--* Illegal Charater in above EncName *-->\r\n<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n]>\r\n<root/>\r\n",
  0, NULL
},
{
  "ibm82n01.xml",
  260,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!--* Missing whitespace in NotationDecl *-->\r\n<!NOTATIONJPGformat SYSTEM \"JPGFormat\">\r\n<!ENTITY aImage SYSTEM \"image.jpg\" NDATA JPGformat>\r\n]>\r\n<root/>\r\n",
  0, NULL
},
{
  "ibm82n02.xml",
  258,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!--* Missing Name in NotationDecl *-->\r\n<!NOTATION PUBLIC \"-//JPG//DTD//JPGFormat\">\r\n<!ENTITY aImage SYSTEM \"image.jpg\" NDATA JPGformat>\r\n]>\r\n<root/>\r\n",
  0, NULL
},
{
  "ibm82n03.xml",
  255,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!--* Missing ExternalID or PublicID in NotationDecl *-->\r\n<!NOTATION JPGformat >\r\n<!ENTITY aImage SYSTEM \"image.jpg\" NDATA JPGformat>\r\n]>\r\n<root/>\r\n",
  0, NULL
},
{
  "ibm82n04.xml",
  258,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!--* Wrong ordering in NotationDecl *-->\r\n<!NOTATION SYSTEM \"JPGFormat\" JPGformat >\r\n<!ENTITY aImage SYSTEM \"image.jpg\" NDATA JPGformat>\r\n]>\r\n<root/>\r\n",
  0, NULL
},
{
  "ibm82n05.xml",
  266,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!--* Wrong keyword: notation in NotationDecl *-->\r\n<!notation JPGformat SYSTEM \"JPGFormat\">\r\n<!ENTITY aImage SYSTEM \"image.jpg\" NDATA JPGformat>\r\n]>\r\n<root/>\r\n",
  0, NULL
},
{
  "ibm82n06.xml",
  278,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!--* Missing closing bracket in NotationDecl *-->\r\n<!NOTATION JPGformat PUBLIC \"-//JPG//DTD//JPGFormat\"\r\n<!ENTITY aImage SYSTEM \"image.jpg\" NDATA JPGformat>\r\n]>\r\n<root/>\r\n",
  0, NULL
},
{
  "ibm82n07.xml",
  295,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!--* Wrong beginning sequence in NotationDecl *-->\r\n<NOTATION JPGformat PUBLIC \"-//JPG//DTD//JPGFormat\">\r\n<!ENTITY aImage SYSTEM \"image.jpg\" NDATA JPGformat>\r\n]>\r\n<root/>\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n",
  0, NULL
},
{
  "ibm82n08.xml",
  266,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!--* Wrong Closing sequence in NotationDecl *-->\r\n<!NOTATION JPGformat SYSTEM \"JPGFormat\"!>\r\n<!ENTITY aImage SYSTEM \"image.jpg\" NDATA JPGformat>\r\n]>\r\n<root/>\r\n",
  0, NULL
},
{
  "ibm83n01.xml",
  267,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!--* Wrong keyword in PublicID *-->\r\n<!NOTATION JPGformat public \"-//JPG//DTD//JPGFormat\">\r\n<!ENTITY aImage SYSTEM \"image.jpg\" NDATA JPGformat>\r\n]>\r\n<root/>\r\n\r\n",
  0, NULL
},
{
  "ibm83n02.xml",
  275,
  "r<?xml version=\"1.0\"?>\r\n<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!--* Wrong keyword : Public in PublicID *-->\r\n<!NOTATION JPGformat Public \"-//JPG//DTD//JPGFormat\">\r\n<!ENTITY aImage SYSTEM \"image.jpg\" NDATA JPGformat>\r\n]>\r\n<root/>\r\n",
  0, NULL
},
{
  "ibm83n03.xml",
  260,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!--* Missing keyword in PublicID *-->\r\n<!NOTATION JPGformat \"-//JPG//DTD//JPGFormat\">\r\n<!ENTITY aImage SYSTEM \"image.jpg\" NDATA JPGformat>\r\n]>\r\n<root/>\r\n",
  0, NULL
},
{
  "ibm83n04.xml",
  270,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!--* Missing White space in PublicID *-->\r\n<!NOTATION JPGformat PUBLIC\"-//JPG//DTD//JPGFormat\">\r\n<!ENTITY aImage SYSTEM \"image.jpg\" NDATA JPGformat>\r\n]>\r\n<root/>\r\n",
  0, NULL
},
{
  "ibm83n05.xml",
  248,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!--* Missing PubidLiteral in PublicID *-->\r\n<!NOTATION JPGformat PUBLIC >\r\n<!ENTITY aImage SYSTEM \"image.jpg\" NDATA JPGformat>\r\n]>\r\n<root/>\r\n",
  0, NULL
},
{
  "ibm83n06.xml",
  266,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!--* Wrong ordering in PublicID *-->\r\n<!NOTATION JPGformat \"-//JPG//DTD//JPGFormat\" PUBLIC>\r\n<!ENTITY aImage SYSTEM \"image.jpg\" NDATA JPGformat>\r\n]>\r\n<root/>\r\n",
  0, NULL
},
{
  "ibm85n01.xml",
  103,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\303\227 an illegal char #x0d7\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n02.xml",
  103,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\303\267 an illegal char #x0f7\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n03.xml",
  103,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\304\262 an illegal char #x132\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n04.xml",
  103,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\304\263 an illegal char #x133\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n05.xml",
  103,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\304\277 an illegal char #x13f\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n06.xml",
  103,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\305\200 an illegal char #x140\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n07.xml",
  103,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\305\211 an illegal char #x149\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n08.xml",
  103,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\305\277 an illegal char #x17f\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n09.xml",
  103,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\307\204 an illegal char #x1c4\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n10.xml",
  103,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\307\214 an illegal char #x1cc\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n11.xml",
  103,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\307\261 an illegal char #x1f1\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n12.xml",
  103,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\307\263 an illegal char #x1f3\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n13.xml",
  103,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\307\266 an illegal char #x1f6\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n14.xml",
  103,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\307\271 an illegal char #x1f9\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n15.xml",
  103,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\307\271 an illegal char #x1f9\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n16.xml",
  103,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\310\260 an illegal char #x230\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n17.xml",
  103,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\312\257 an illegal char #x2af\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n18.xml",
  103,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\313\217 an illegal char #x2cf\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n19.xml",
  103,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\316\207 an illegal char #x387\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n20.xml",
  103,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\316\213 an illegal char #x38b\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n21.xml",
  103,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\316\242 an illegal char #x3a2\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n22.xml",
  103,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\317\217 an illegal char #x3cf\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n23.xml",
  103,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\317\227 an illegal char #x3d7\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n24.xml",
  103,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\317\235 an illegal char #x3dd\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n25.xml",
  103,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\317\241 an illegal char #x3e1\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n26.xml",
  103,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\317\264 an illegal char #x3f4\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n27.xml",
  103,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\320\215 an illegal char #x40d\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n28.xml",
  103,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\321\220 an illegal char #x450\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n29.xml",
  103,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\321\235 an illegal char #x45d\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n30.xml",
  103,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\322\202 an illegal char #x482\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n31.xml",
  103,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\323\205 an illegal char #x4c5\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n32.xml",
  103,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\323\206 an illegal char #x4c6\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n33.xml",
  103,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\323\211 an illegal char #x4c9\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n34.xml",
  103,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\323\254 an illegal char #x4ec\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n35.xml",
  103,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\323\255 an illegal char #x4ed\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n36.xml",
  103,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\323\266 an illegal char #x4f6\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n37.xml",
  103,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\323\272 an illegal char #x4fa\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n38.xml",
  103,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\325\227 an illegal char #x557\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n39.xml",
  103,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\325\230 an illegal char #x558\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n40.xml",
  103,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\326\207 an illegal char #x587\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n41.xml",
  103,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\327\253 an illegal char #x5eb\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n42.xml",
  103,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\327\263 an illegal char #x5f3\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n43.xml",
  103,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\330\240 an illegal char #x620\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n44.xml",
  103,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\330\273 an illegal char #x63b\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n45.xml",
  103,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\331\213 an illegal char #x64b\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n46.xml",
  103,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\332\270 an illegal char #x6b8\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n47.xml",
  103,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\332\277 an illegal char #x6bf\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n48.xml",
  103,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\333\217 an illegal char #x6cf\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n49.xml",
  103,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\333\224 an illegal char #x6d4\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n50.xml",
  103,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\333\226 an illegal char #x6d6\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n51.xml",
  103,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\333\247 an illegal char #x6e7\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n52.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\244\272 an illegal char #x093a\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n53.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\244\276 an illegal char #x093e\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n54.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\245\242 an illegal char #x0962\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n55.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\246\215 an illegal char #x098d\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n56.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\246\221 an illegal char #x0991\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n57.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\246\222 an illegal char #x0992\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n58.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\246\251 an illegal char #x09a9\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n59.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\246\261 an illegal char #x09b1\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n60.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\246\265 an illegal char #x09b5\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n61.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\246\272 an illegal char #x09ba\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n62.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\247\236 an illegal char #x09de\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n63.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\247\242 an illegal char #x09e2\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n64.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\247\262 an illegal char #x09f2\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n65.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\250\213 an illegal char #x0a0b\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n66.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\250\221 an illegal char #x0a11\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n67.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\250\251 an illegal char #x0a29\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n68.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\250\261 an illegal char #x0a31\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n69.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\250\264 an illegal char #x0a34\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n70.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\250\267 an illegal char #x0a37\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n71.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\250\272 an illegal char #x0a3a\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n72.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\251\235 an illegal char #x0a5d\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n73.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\251\260 an illegal char #x0a70\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n74.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\251\265 an illegal char #x0a75\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n75.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\252\204 an illegal char #x0a84\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n76.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\252\274 an illegal char #x0abc\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n77.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\252\222 an illegal char #x0a92\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n78.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\252\251 an illegal char #x0aa9\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n79.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\252\261 an illegal char #x0ab1\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n80.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\252\264 an illegal char #x0ab4\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n81.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\252\272 an illegal char #x0aba\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n82.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\254\204 an illegal char #x0b04\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n83.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\254\215 an illegal char #x0b0d\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n84.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\254\221 an illegal char #x0b11\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n85.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\254\251 an illegal char #x0b29\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n86.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\254\261 an illegal char #x0b31\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n87.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\254\264 an illegal char #x0b34\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n88.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\254\272 an illegal char #x0b3a\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n89.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\254\276 an illegal char #x0b3e\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n90.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\255\236 an illegal char #x0b5e\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n91.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\255\242 an illegal char #x0b62\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n92.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\256\213 an illegal char #x0b8b\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n93.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\256\221 an illegal char #x0b91\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n94.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\256\230 an illegal char #x0b98\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n95.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\256\233 an illegal char #x0b9b\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n96.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\256\235 an illegal char #x0b9d\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n97.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\256\240 an illegal char #x0ba0\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n98.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\256\247 an illegal char #x0ba7\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n99.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\256\253 an illegal char #x0bab\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n100.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\256\266 an illegal char #x0bb6\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n101.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\256\272 an illegal char #x0bba\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n102.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\260\215 an illegal char #x0c0d\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n103.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\260\221 an illegal char #x0c11\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n104.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\260\251 an illegal char #x0c29\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n105.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\260\264 an illegal char #x0c34\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n106.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\261\237 an illegal char #x0c5f\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n107.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\261\242 an illegal char #x0c62\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n108.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\262\215 an illegal char #x0c8d\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n109.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\262\221 an illegal char #x0c91\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n110.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\262\251 an illegal char #x0ca9\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n111.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\262\264 an illegal char #x0cb4\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n112.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\262\272 an illegal char #x0cba\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n113.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\263\237 an illegal char #x0cdf\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n114.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\263\242 an illegal char #x0ce2\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n115.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\264\215 an illegal char #x0d0d\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n116.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\264\221 an illegal char #x0d11\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n117.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\264\251 an illegal char #x0d29\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n118.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\264\272 an illegal char #x0d3a\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n119.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\265\242 an illegal char #x0d62\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n120.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\270\257 an illegal char #x0e2f\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n121.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\270\261 an illegal char #x0e31\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n122.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\270\264 an illegal char #x0e34\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n123.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\271\206 an illegal char #x0e46\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n124.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\272\203 an illegal char #x0e83\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n125.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\272\205 an illegal char #x0e85\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n126.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\272\211 an illegal char #x0e89\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n127.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\272\213 an illegal char #x0e8b\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n128.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\272\216 an illegal char #x0e8e\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n129.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\272\230 an illegal char #x0e98\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n130.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\272\240 an illegal char #x0ea0\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n131.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\272\244 an illegal char #x0ea4\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n132.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\272\246 an illegal char #x0ea6\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n133.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\272\250 an illegal char #x0ea8\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n134.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\272\254 an illegal char #x0eac\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n135.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\272\257 an illegal char #x0eaf\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n136.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\272\261 an illegal char #x0eb1\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n137.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\272\264 an illegal char #x0eb4\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n138.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\272\276 an illegal char #x0ebe\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n139.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\273\205 an illegal char #x0ec5\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n140.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\275\210 an illegal char #x0f48\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n141.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\275\252 an illegal char #x0f6a\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n142.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\341\203\206 an illegal char #x10c6\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n143.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\341\203\267 an illegal char #x10f7\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n144.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\341\204\201 an illegal char #x1101\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n145.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\341\204\204 an illegal char #x1104\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n146.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\341\204\210 an illegal char #x1108\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n147.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\341\204\212 an illegal char #x110a\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n148.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\341\204\215 an illegal char #x110d\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n149.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\341\204\273 an illegal char #x113b\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n150.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\341\204\277 an illegal char #x113f\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n151.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\341\205\201 an illegal char #x1141\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n152.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\341\205\215 an illegal char #x114d\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n153.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\341\205\217 an illegal char #x114f\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n154.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\341\205\221 an illegal char #x1151\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n155.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\341\205\226 an illegal char #x1156\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n156.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\341\205\232 an illegal char #x115a\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n157.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\341\205\242 an illegal char #x1162\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n158.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\341\205\244 an illegal char #x1164\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n159.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\341\205\246 an illegal char #x1166\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n160.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\341\205\253 an illegal char #x116b\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n161.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\341\205\257 an illegal char #x116f\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n162.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\341\205\264 an illegal char #x1174\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n163.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\341\206\237 an illegal char #x119f\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n164.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\341\206\254 an illegal char #x11ac\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n165.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\341\206\266 an illegal char #x11b6\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n166.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\341\206\271 an illegal char #x11b9\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n167.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\341\206\273 an illegal char #x11bb\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n168.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\341\207\203 an illegal char #x11c3\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n169.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\341\207\261 an illegal char #x11f1\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n170.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\341\207\272 an illegal char #x11fa\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n171.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\341\272\234 an illegal char #x1e9c\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n172.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\341\273\272 an illegal char #x1efa\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n173.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\341\274\226 an illegal char #x1f16\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n174.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\341\274\236 an illegal char #x1f1e\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n175.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\341\275\206 an illegal char #x1f46\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n176.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\341\275\217 an illegal char #x1f4f\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n177.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\341\275\230 an illegal char #x1f58\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n178.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\341\275\232 an illegal char #x1f5a\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n179.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\341\275\234 an illegal char #x1f5c\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n180.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\341\275\236 an illegal char #x1f5e\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n181.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\341\275\276 an illegal char #x1f7e\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n182.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\341\276\265 an illegal char #x1fb5\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n183.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\341\276\275 an illegal char #x1fbd\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n184.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\341\276\277 an illegal char #x1fbf\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n185.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\341\277\205 an illegal char #x1fc5\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n186.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\341\277\215 an illegal char #x1fcd\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n187.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\341\277\225 an illegal char #x1fd5\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n188.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\341\277\234 an illegal char #x1fdc\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n189.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\341\277\255 an illegal char #x1fed\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n190.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\341\277\265 an illegal char #x1ff5\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n191.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\341\277\275 an illegal char #x1ffd\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n192.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\342\204\247 an illegal char #x2127\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n193.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\342\204\257 an illegal char #x212f\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n194.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\342\206\203 an illegal char #x2183\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n195.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\343\202\225 an illegal char #x3095\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n196.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\343\203\273 an illegal char #x30fb\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n197.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\343\204\255 an illegal char #x312d\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm85n198.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\355\236\244 an illegal char #xd7a4\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm86n01.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\344\263\277 an illegal char #x4cff\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm86n02.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\351\276\246 an illegal char #x9fa6\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm86n03.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\343\200\210 an illegal char #x3008\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm86n04.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\343\200\252 an illegal char #x302a\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm87n01.xml",
  104,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\313\277 an illegal char #x2ff\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm87n02.xml",
  104,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\315\206 an illegal char #x346\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm87n03.xml",
  104,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\315\242 an illegal char #x362\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm87n04.xml",
  104,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\322\207 an illegal char #x487\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm87n05.xml",
  104,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\326\242 an illegal char #x5a2\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm87n06.xml",
  104,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\326\272 an illegal char #x5ba\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm87n07.xml",
  104,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\326\276 an illegal char #x5be\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm87n08.xml",
  104,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\327\200 an illegal char #x5c0\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm87n09.xml",
  104,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\327\203 an illegal char #x5c3\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm87n10.xml",
  104,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\331\223 an illegal char #x653\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm87n11.xml",
  104,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\332\270 an illegal char #x6b8\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm87n12.xml",
  104,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\332\271 an illegal char #x6b9\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm87n13.xml",
  104,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\333\251 an illegal char #x6e9\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm87n14.xml",
  104,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\333\256 an illegal char #x6ee\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm87n15.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\244\204 an illegal char #x0904\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm87n16.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\244\273 an illegal char #x093b\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm87n17.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\245\216 an illegal char #x094e\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm87n18.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\245\225 an illegal char #x0955\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm87n19.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\245\244 an illegal char #x0964\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm87n20.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\246\204 an illegal char #x0984\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm87n21.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\247\205 an illegal char #x09c5\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm87n22.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\247\211 an illegal char #x09c9\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm87n23.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\247\216 an illegal char #x09ce\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm87n24.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\247\230 an illegal char #x09d8\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm87n25.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\247\244 an illegal char #x09e4\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm87n26.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\250\203 an illegal char #x0a03\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm87n27.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\250\275 an illegal char #x0a3d\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm87n28.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\251\206 an illegal char #x0a46\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm87n29.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\251\211 an illegal char #x0a49\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm87n30.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\251\216 an illegal char #x0a4e\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm87n31.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\252\200 an illegal char #x0a80\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm87n32.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\252\204 an illegal char #x0a84\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm87n33.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\252\273 an illegal char #x0abb\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm87n34.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\253\206 an illegal char #x0ac6\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm87n35.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\253\212 an illegal char #x0aca\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm87n36.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\253\216 an illegal char #x0ace\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm87n37.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\254\204 an illegal char #x0b04\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm87n38.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\254\273 an illegal char #x0b3b\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm87n39.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\255\204 an illegal char #x0b44\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm87n40.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\255\212 an illegal char #x0b4a\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm87n41.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\255\216 an illegal char #x0b4e\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm87n42.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\255\230 an illegal char #x0b58\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm87n43.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\256\204 an illegal char #x0b84\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm87n44.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\257\203 an illegal char #x0bc3\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm87n45.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\257\211 an illegal char #x0bc9\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm87n46.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\257\226 an illegal char #x0bd6\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm87n47.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\260\215 an illegal char #x0c0d\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm87n48.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\261\205 an illegal char #x0c45\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm87n49.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\261\211 an illegal char #x0c49\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm87n50.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\261\224 an illegal char #x0c54\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm87n51.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\262\201 an illegal char #x0c81\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm87n52.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\262\204 an illegal char #x0c84\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm87n53.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\263\205 an illegal char #x0cc5\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm87n54.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\263\211 an illegal char #x0cc9\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm87n55.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\263\224 an illegal char #x0cd4\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm87n56.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\263\227 an illegal char #x0cd7\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm87n57.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\264\204 an illegal char #x0d04\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm87n58.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\265\205 an illegal char #x0d45\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm87n59.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\265\211 an illegal char #x0d49\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm87n60.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\265\216 an illegal char #x0d4e\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm87n61.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\265\230 an illegal char #x0d58\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm87n62.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\270\277 an illegal char #x0e3f\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm87n63.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\270\273 an illegal char #x0e3b\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm87n64.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\271\217 an illegal char #x0e4f\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm88n01.xml",
  100,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_) an illegal char #x29 in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm88n02.xml",
  100,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_; an illegal char #x3b in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm88n03.xml",
  104,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\331\252 an illegal char #x66a\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm88n04.xml",
  104,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\333\272 an illegal char #x6fa\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm88n05.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\245\260 an illegal char #x0970\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm88n06.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\247\262 an illegal char #x09f2\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm89n01.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\302\266 an illegal extender #x0b6 in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm89n02.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\302\270 an illegal extender #x0b8 in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm89n03.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\313\222 an illegal extender #x2d2 in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm89n04.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\317\276 an illegal extender #x3fe in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm89n05.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\331\237 an illegal extender #x65f in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0, NULL
},
{
  "ibm89n06.xml",
  131,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\273\207 an illegal extender #x0ec7 in PITarget ?>\r\n]>\r\n<animal/>\r\n<IllegalExtender\340\273\207\57\r\n",
  0, NULL
},
{
  "ibm89n07.xml",
  131,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\343\200\206 an illegal extender #x3006 in PITarget ?>\r\n]>\r\n<animal/>\r\n<IllegalExtender\343\200\206\57\r\n",
  0, NULL
},
{
  "ibm89n08.xml",
  131,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\343\200\260 an illegal extender #x3030 in PITarget ?>\r\n]>\r\n<animal/>\r\n<IllegalExtender\343\200\260\57\r\n",
  0, NULL
},
{
  "ibm89n09.xml",
  131,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\343\200\266 an illegal extender #x3036 in PITarget ?>\r\n]>\r\n<animal/>\r\n<IllegalExtender\343\200\266\57\r\n",
  0, NULL
},
{
  "ibm89n10.xml",
  131,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\343\202\234 an illegal extender #x309c in PITarget ?>\r\n]>\r\n<animal/>\r\n<IllegalExtender\343\202\234\57\r\n",
  0, NULL
},
{
  "ibm89n11.xml",
  131,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\343\202\237 an illegal extender #x309f in PITarget ?>\r\n]>\r\n<animal/>\r\n<IllegalExtender\343\202\237\57\r\n",
  0, NULL
},
{
  "ibm89n12.xml",
  131,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\343\203\277 an illegal extender #x30ff in PITarget ?>\r\n]>\r\n<animal/>\r\n<IllegalExtender\343\203\277\57\r\n",
  0, NULL
},
