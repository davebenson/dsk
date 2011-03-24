/* ==== from ibm ibm_oasis_invalid.xml ==== */
{
  "invalid/P28/ibm28i01.xml",
  231,
  "<?xml version=\"1.0\" encoding='UTF-8'?>\r\n<!DOCTYPE tiger [\r\n   <!ELEMENT tiger EMPTY>\r\n]>\r\n<!-- This against VC of P28. The Name in the document type declaration\r\n does not match the element type of the root element. --> \r\n<animal/>",
  0,
  NULL,
  "The test violates VC:Root Element Type in P28. The Name in the document type declaration does not match the element type of the root element."
},
{
  "invalid/P32/ibm32i01.xml",
  434,
  "<?xml version=\"1.0\" standalone=\"yes\" ?>\r\n<!DOCTYPE animal SYSTEM \"ibm32i01.dtd\" [\r\n   <!ELEMENT animal EMPTY>\r\n]>\r\n<!-- This is against VC: Standalone Document Declaration in P32\r\n The standalone document declaration has the value \"yes\", there is an external \r\n markup declaration of attributes with default values, and the associated \r\n element appears in the document with specified values for those attributes.   \r\n-->\r\n<animal/>\r\n",
  0,
  NULL,
  "This test violates VC: Standalone Document Declaration in P32. The standalone document declaration has the value yes, BUT there is an external markup declaration of attributes with default values, and the associated element appears in the document with specified values for those attributes."
},
{
  "invalid/P32/ibm32i03.xml",
  416,
  "<?xml version=\"1.0\" standalone=\"yes\" ?>\r\n<!DOCTYPE animal SYSTEM \"ibm32i03.dtd\" [\r\n   <!ELEMENT animal EMPTY>\r\n]>\r\n<!-- This is against VC: Standalone Document Declaration in P32\r\n The standalone document declaration has the value \"yes\", there is an external\r\n markup declaration of attributes with values containing character reference.\r\n-->\r\n<animal class=\"   NMTOKEN_with_leading_and_trailing_space\t \"/>\r\n\r\n\r\n\r\n\r\n",
  0,
  NULL,
  "This test violates VC: Standalone Document Declaration in P32. The standalone document declaration has the value yes, BUT there is an external markup declaration of attributes with values that will change if normalized."
},
{
  "invalid/P32/ibm32i04.xml",
  493,
  "<?xml version='1.0' standalone='yes' ?>\r\n<!DOCTYPE animal SYSTEM \"ibm32i04.dtd\" [\r\n  <!ATTLIST animal xml:space (default|preserve) 'preserve'>\r\n]>\r\n<!-- This is against VC: Standalone Document Declaration in P32\r\n The standalone document declaration has the value \"yes\", there is an \r\n external markup declaration of element with a element only content type, and \r\n white space occurs directly within the mixed content.\r\n-->\r\n<animal><a>This is a \r\n\r\nyellow tiger</a> <b/>\r\n<c/>\r\n\r\n</animal>\r\n",
  0,
  NULL,
  "This test violates VC: Standalone Document Declaration in P32. The standalone document declaration has the value yes, BUT there is an external markup declaration of element with element content, and white space occurs directly within the mixed content."
},
{
  "invalid/P39/ibm39i01.xml",
  283,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n  <!ELEMENT root (a,b)>\r\n  <!ELEMENT a EMPTY>\r\n  <!ELEMENT b (#PCDATA|c)* >\r\n  <!ELEMENT c ANY>\r\n\r\n]>\r\n<!--* EMPTY element a has content *-->\r\n<root><a>should not have content here</a><b>\r\n   <c></c> \r\n   content of b element\r\n</b></root>\r\n\r\n",
  0,
  NULL,
  "This test violates VC: Element Valid in P39. Element a is declared empty in DTD, but has content in the document."
},
{
  "invalid/P39/ibm39i02.xml",
  291,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n  <!ELEMENT root (a,b)>\r\n  <!ELEMENT a EMPTY>\r\n  <!ELEMENT b (#PCDATA|c)* >\r\n  <!ELEMENT c ANY>\r\n\r\n]>\r\n<!--* root element have text content *-->\r\n<root>\r\n root can't have text content\r\n<a></a><b>\r\n   <c></c> \r\n   content of b element\r\n</b></root>\r\n\r\n",
  0,
  NULL,
  "This test violates VC: Element Valid in P39. root is declared only having element children in DTD, but have text content in the document."
},
{
  "invalid/P39/ibm39i03.xml",
  306,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n  <!ELEMENT root (a,b)>\r\n  <!ELEMENT a EMPTY>\r\n  <!ELEMENT b (#PCDATA|c)* >\r\n  <!ELEMENT c ANY>\r\n]>\r\n<!--* illgal element in b's Mixed content *-->\r\n<root><a/><b>\r\n   <c></c> \r\n   content of b element\r\n   <a/>\r\n   could not have 'a' as 'b's content\r\n</b></root>\r\n\r\n",
  0,
  NULL,
  "This test violates VC: Element Valid in P39. Illegal elements are inserted in b's content of Mixed type."
},
{
  "invalid/P39/ibm39i04.xml",
  351,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n  <!ELEMENT root (a,b)>\r\n  <!ELEMENT a EMPTY>\r\n  <!ELEMENT b (#PCDATA|c)* >\r\n  <!ELEMENT c ANY>\r\n  <!ELEMENT f EMPTY>\r\n]>\r\n<!--* element c has undeclared element as its ANY content *-->\r\n<root><a/><b>\r\n   <c><f/></c> \r\n   content of b element\r\n   <c>\r\n      <d>not declared in dtd</d>\r\n   </c>\r\n</b></root>\r\n\r\n",
  0,
  NULL,
  "This test violates VC: Element Valid in P39. Element c has undeclared element as its content of ANY type"
},
{
  "invalid/P41/ibm41i01.xml",
  318,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n  <!ELEMENT root (#PCDATA|b)* >\r\n  <!ELEMENT b (#PCDATA) >\r\n  <!ATTLIST b attr2 (abc|def) \"abc\">\r\n  <!ATTLIST b attr3 CDATA #FIXED \"fixed\">\r\n]>\r\n<root>\r\n  <b attr1=\"value1\" attr2=\"def\" attr3=\"fixed\">attr1 not declared</b>\r\n</root>\r\n<!--* testing VC:Attribute Value Type  *-->\r\n",
  0,
  NULL,
  "This test violates VC: Attribute Value Type in P41. attr1 for Element b is not declared."
},
{
  "invalid/P41/ibm41i02.xml",
  365,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n  <!ELEMENT root (PCDATA|b)* >\r\n  <!ELEMENT b (#PCDATA) >\r\n  <!ATTLIST b attr1 CDATA #REQUIRED>\r\n  <!ATTLIST b attr2 (abc|def) \"abc\">\r\n  <!ATTLIST b attr3 CDATA #FIXED \"fixed\">\r\n]>\r\n<root>\r\n  <b attr1=\"value1\" attr2=\"abc\" attr3=\"shoudbefixed\">attr3 value not fixed</b>\r\n</root>\r\n<!--* testing P41 VC: AtributeValueType*-->\r\n",
  0,
  NULL,
  "This test violates VC: Attribute Value Type in P41. attr3 for Element b is given a value that does not match the declaration in the DTD."
},
{
  "invalid/P45/ibm45i01.xml",
  549,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n  <!ELEMENT root (#PCDATA|b)* >\r\n  <!ELEMENT b EMPTY>\r\n  <!ELEMENT not_unique ANY>\r\n  <!ELEMENT not_unique EMPTY>\r\n  <!ELEMENT not_unique (b,b) >\r\n  <!ELEMENT unique. ANY>\r\n  <!ATTLIST b attr1 CDATA #IMPLIED>\r\n  <!ATTLIST b attr2 CDATA #IMPLIED>\r\n  <!ATTLIST b attr3 CDATA #IMPLIED>\r\n]>\r\n<root>\r\n  <b/>without white space\r\n  <b /> with a white space\r\n  <b attr1=\"value1\" />\r\n  <b attr1=\"value1\" attr2=\"value2\" attr3 = \"value3\"/>\r\n</root>\r\n<!--* a invalid test: testing P45 VC unique element type decl  *-->\r\n",
  0,
  NULL,
  "This test violates VC: Unique Element Type Declaration. Element not_unique has been declared 3 time in the DTD."
},
{
  "invalid/P49/ibm49i01.xml",
  226,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root SYSTEM \"ibm49i01.dtd\" [\r\n  <!ELEMENT root (a,b)>\r\n]>\r\n<root><a/><b>\r\n   <c></c >\r\n   content of b element\r\n</b></root>\r\n<!--* a invalid test: tests VC:Proper Group/PE Nesting in P49 *-->\r\n",
  0,
  NULL,
  "Violates VC:Proper Group/PE Nesting in P49. Open and close parenthesis for a choice content model are in different PE replace Texts."
},
{
  "invalid/P50/ibm50i01.xml",
  226,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root SYSTEM \"ibm50i01.dtd\" [\r\n  <!ELEMENT root (a,b)>\r\n]>\r\n<root><a/><b>\r\n   <c></c >\r\n   content of b element\r\n</b></root>\r\n<!--* a invalid test: tests VC:Proper Group/PE Nesting in P50 *-->\r\n",
  0,
  NULL,
  "Violates VC:Proper Group/PE Nesting in P50. Open and close parenthesis for a seq content model are in different PE replace Texts."
},
{
  "invalid/P51/ibm51i01.xml",
  225,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root SYSTEM \"ibm51i01.dtd\" [\r\n  <!ELEMENT root ANY>\r\n]>\r\n<root>\r\n  <a> Element type a </a>\r\n  <b> Element type b </b>\r\n</root>\r\n<!--* a invalid test: tests P51 VC: Proper Group/PE Nesting *-->",
  0,
  NULL,
  "Violates VC:Proper Group/PE Nesting in P51. Open and close parenthesis for a Mixed content model are in different PE replace Texts."
},
{
  "invalid/P51/ibm51i03.xml",
  409,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n  <!ELEMENT root ANY>\r\n  <!ELEMENT a (#PCDATA)* >\r\n  <!ELEMENT b (#PCDATA) >\r\n  <!ELEMENT c ( #PCDATA)*>\r\n  <!ELEMENT d (#PCDATA|c)* >\r\n  <!--* Duplicate element types in Mixed content decl *-->\r\n  <!ELEMENT e (#PCDATA|a|a|b|c)* >\r\n]>\r\n<root>\r\n  <a> Element type a </a>\r\n  <b> Element type b </b>\r\n</root>\r\n<!--* a invalid test: tests P51 VC: No Duplicate Types *-->\r\n",
  0,
  NULL,
  "Violates VC:No Duplicate Types in P51. Element a appears twice in the Mixed content model of Element e."
},
{
  "invalid/P56/ibm56i01.xml",
  370,
  "<?xml  version=\"1.0\"?>\r\n<!-- validity constraint check for Production 56(negative Test)-->\r\n<!DOCTYPE tokenizer\r\n [\r\n <!ELEMENT tokenizer ANY>\r\n <!ATTLIST tokenizer UniqueName ID #REQUIRED>\r\n ]>\r\n<tokenizer UniqueName = \"@c999\">\r\nThis is a negative test for validity constraints\r\nthe value of the attribute with a type ID does not match the Name production\r\n</tokenizer>",
  0,
  NULL,
  "Tests invalid TokenizedType which is against P56 VC: ID. The value of the ID attribute \"UniqueName\" is \"@999\" which does not meet the Name production."
},
{
  "invalid/P56/ibm56i02.xml",
  459,
  "<?xml  version=\"1.0\"?>\r\n<!-- validity constraint check for Production 56(negative Test)-->\r\n<!DOCTYPE tokenizer\r\n [\r\n <!ELEMENT tokenizer ANY>\r\n <!ELEMENT b EMPTY>\r\n <!ATTLIST b attr ID #REQUIRED>\r\n <!ATTLIST tokenizer UniqueName ID #REQUIRED>\r\n  ]>\r\n<tokenizer UniqueName = \"Ac999\">\r\n<b attr = \"Ac999\"></b>\r\nThis is a negative test for validity constraints\r\nthe value of the attribute with a type ID appears more than once in the XML document\r\n</tokenizer>\r\n",
  0,
  NULL,
  "Tests invalid TokenizedType which is against P56 VC: ID. The two ID attributes \"attr\" and \"UniqueName\" have the same value \"Ac999\" for the element \"b\" and the element \"tokenizer\"."
},
{
  "invalid/P56/ibm56i03.xml",
  329,
  "<?xml  version=\"1.0\"?>\r\n<!-- validity constraint check for Production 56(Negative Test)-->\r\n<!DOCTYPE tokenizer\r\n [\r\n <!ELEMENT tokenizer ANY>\r\n <!ATTLIST tokenizer UniqueName ID #FIXED \"AC1999\">\r\n ]>\r\n<tokenizer>\r\nThis is a Negative validity test for ID Attribute Default.\r\nGiving the attribute default as #FIXED\r\n</tokenizer>\r\n",
  0,
  NULL,
  "Tests invalid TokenizedType which is against P56 VC: ID Attribute Default. The \"#FIXED\" occurs in the DefaultDecl for the ID attribute \"UniqueName\"."
},
{
  "invalid/P56/ibm56i05.xml",
  350,
  "<?xml  version=\"1.0\"?>\r\n<!-- validity constraint check for Production 56(Negative Test)-->\r\n<!DOCTYPE tokenizer\r\n [\r\n <!ELEMENT tokenizer ANY>\r\n <!ATTLIST tokenizer UniqueName ID \"BOGUS\">\r\n ]>\r\n<tokenizer UniqueName = \"AC1999\">\r\nThis is a Negative validity test for ID Attribute Default.\r\nGiving the attibute default as a const string\r\n</tokenizer>\r\n",
  0,
  NULL,
  "Tests invalid TokenizedType which is against P56 VC: ID Attribute Default. The constant string \"BOGUS\" occurs in the DefaultDecl for the ID attribute \"UniqueName\"."
},
{
  "invalid/P56/ibm56i06.xml",
  411,
  "<?xml  version=\"1.0\"?>\r\n<!-- validity constraint check for Production 56(Negative Test)-->\r\n<!DOCTYPE tokenizer\r\n [\r\n <!ELEMENT tokenizer ANY>\r\n <!ELEMENT a EMPTY>\r\n <!ATTLIST a first ID #REQUIRED>\r\n <!ATTLIST a second ID #REQUIRED>\r\n ]>\r\n<tokenizer>\r\n<a first = \"AC1999\" second=\"BC1999\"></a>\r\nThis is a Negative validity test for ID.\r\nThere is more than attribute of type ID for the element a\r\n\r\n</tokenizer>\r\n",
  0,
  NULL,
  "Tests invalid TokenizedType which is against P56 VC: One ID per Element Type. The element \"a\" has two ID attributes \"first\" and \"second\"."
},
{
  "invalid/P56/ibm56i07.xml",
  481,
  "<?xml  version=\"1.0\"?>\r\n<!-- validity constraint check for Production 56(Negative Test)-->\r\n<!DOCTYPE test\r\n [\r\n <!ELEMENT test ANY>\r\n <!ELEMENT id EMPTY>\r\n <!ELEMENT idref EMPTY>\r\n <!ATTLIST id UniqueName ID #REQUIRED>\r\n <!ATTLIST idref reference IDREF #IMPLIED>\r\n ]>\r\n<test>\r\n<id UniqueName = \"AC456\"></id>\r\n<idref reference = \"@456\"></idref>\r\nNegative test for validity constraint of IDREF.\r\nIn an attribute decl, values of type IDREF does not match the name production\r\n</test>",
  0,
  NULL,
  "Tests invalid TokenizedType which is against P56 VC: IDREF. The value of the IDREF attribute \"reference\" is \"@456\" which does not meet the Name production."
},
{
  "invalid/P56/ibm56i08.xml",
  576,
  "<?xml  version=\"1.0\"?>\r\n<!-- validity constraint check for Production 56(Negative Test)-->\r\n<!DOCTYPE test\r\n [\r\n <!ELEMENT test ANY>\r\n <!ELEMENT id EMPTY>\r\n <!ELEMENT idref EMPTY>\r\n <!ATTLIST id UniqueName ID #REQUIRED>\r\n <!ATTLIST idref reference IDREF #IMPLIED>\r\n ]>\r\n<test>\r\n<id UniqueName = \"AC456\"></id>\r\n<idref reference = \"BC456\"></idref>\r\nNegative test for validity constraint of IDREF.\r\nIn an attribute decl, values of type IDREF match the name production and\r\nIDREF value does not match the value assigned to any ID attribute somewhere\r\nin the XML document.\r\n</test>",
  0,
  NULL,
  "Tests invalid TokenizedType which is against P56 VC: IDREF. The value of the IDREF attribute \"reference\" is \"BC456\" which does not match the value assigned to any ID attributes."
},
{
  "invalid/P56/ibm56i09.xml",
  585,
  "<?xml  version=\"1.0\"?>\r\n<!-- validity constraint check for Production 56(Negative Test)-->\r\n<!DOCTYPE test\r\n [\r\n <!ELEMENT test ANY>\r\n <!ELEMENT id1 EMPTY>\r\n <!ELEMENT id2 EMPTY>\r\n <!ELEMENT idrefs EMPTY>\r\n <!ATTLIST id1 UniqueName ID #REQUIRED>\r\n <!ATTLIST id2 UName ID #IMPLIED>\r\n <!ATTLIST idrefs reference IDREFS #IMPLIED>\r\n ]>\r\n<test>\r\n<id1 UniqueName = \"AC456\"></id1>\r\n<id2 UName = \"BC567\"></id2>\r\n<idrefs reference = \"AC456 #567\"></idrefs>\r\nNegative test for validity constraint of IDREFS.\r\nIn an attribute decl, values of type IDREFS does not match the name production\r\n</test>",
  0,
  NULL,
  "Tests invalid TokenizedType which is against P56 VC: IDREFS. The value of the IDREFS attribute \"reference\" is \"AC456 #567\" which does not meet the Names production."
},
{
  "invalid/P56/ibm56i10.xml",
  688,
  "<?xml  version=\"1.0\"?>\r\n<!-- validity constraint check for Production 56(Negative Test)-->\r\n<!DOCTYPE test\r\n [\r\n <!ELEMENT test ANY>\r\n <!ELEMENT id1 EMPTY>\r\n <!ELEMENT id2 EMPTY>\r\n <!ELEMENT idrefs EMPTY>\r\n <!ATTLIST id1 UniqueName ID #REQUIRED>\r\n <!ATTLIST id2 UName ID #IMPLIED>\r\n <!ATTLIST idrefs reference IDREFS #IMPLIED>\r\n ]>\r\n<test>\r\n<id1 UniqueName = \"BC456\"></id1>\r\n<id2 UName = \"AC567\"></id2>\r\n<idrefs reference = \"EF456 DE355\"></idrefs>\r\nNegative test for validity constraint of IDREFS.\r\nIn an attribute decl, values of type IDREFS match the name production\r\nbut IDREFS value do not match the values assigned to one or more ID attributes\r\nsomewhere in the XML document\r\n</test>",
  0,
  NULL,
  "Tests invalid TokenizedType which is against P56 VC: IDREFS. The value of the IDREFS attribute \"reference\" is \"EF456 DE355\" which does not match the values assigned to two ID attributes."
},
{
  "invalid/P56/ibm56i11.xml",
  411,
  "<?xml  version=\"1.0\"?>\r\n<!-- validity constraint check for Production 56(Negative test)-->\r\n<!DOCTYPE test\r\n [\r\n <!ELEMENT test ANY>\r\n <!ELEMENT landscape EMPTY>\r\n <!ENTITY image SYSTEM \"d:\\testspec\\images\\sunset.gif\" NDATA gif>\r\n <!ATTLIST landscape sun ENTITY #IMPLIED>\r\n]>\r\n<test>\r\n<landscape sun = \"ima ge\"></landscape>\r\nIn the attribute decl, values of type ENTITY do not match the Name production\r\n</test>",
  0,
  NULL,
  "Tests invalid TokenizedType which is against P56 VC: Entity Name. The value of the ENTITY attribute \"sun\" is \"ima ge\" which does not meet the Name production."
},
{
  "invalid/P56/ibm56i12.xml",
  469,
  "<?xml  version=\"1.0\"?>\r\n<!-- validity constraint check for Production 56(Negative test)-->\r\n<!DOCTYPE test\r\n [\r\n <!ELEMENT test ANY>\r\n <!ELEMENT landscape EMPTY>\r\n <!ENTITY image SYSTEM \"d:\\testspec\\images\\sunset.gif\" NDATA gif>\r\n <!ATTLIST landscape sun ENTITY #IMPLIED>\r\n]>\r\n<test>\r\n<landscape sun = \"notimage\"></landscape>\r\nIn the attribute decl, values of type ENTITY match the Name production\r\nbut does not match the name of any entity declared in the DTD\r\n</test>",
  0,
  NULL,
  "Tests invalid TokenizedType which is against P56 VC: Entity Name. The value of the ENTITY attribute \"sun\" is \"notimage\" which does not match the name of any unparsed entity declared."
},
{
  "invalid/P56/ibm56i13.xml",
  478,
  "<?xml  version=\"1.0\"?>\r\n<!-- validity constraint check for Production 56(Negative test)-->\r\n<!DOCTYPE test\r\n [\r\n <!ELEMENT test ANY>\r\n <!ELEMENT landscape EMPTY>\r\n <!ENTITY parsedentity SYSTEM \"ibm56iv01.xml\" >\r\n <!ATTLIST landscape sun ENTITY #IMPLIED>\r\n]>\r\n<test>\r\n<landscape sun = \"parsedentity\"></landscape>\r\nIn an attribute declaration, values of type ENTITY match the Name production and the ENTITY value\r\nmatches the name of a parsed entity declared in the DTD. \r\n</test>",
  0,
  NULL,
  "Tests invalid TokenizedType which is against P56 VC: Entity Name. The value of the ENTITY attribute \"sun\" is \"parsedentity\" which matches the name of a parsed entity instead of an unparsed entity declared."
},
{
  "invalid/P56/ibm56i14.xml",
  502,
  "<?xml  version=\"1.0\"?>\r\n<!-- validity constraint check for Production 56(Negative test)-->\r\n<!DOCTYPE test\r\n [\r\n <!ELEMENT test ANY>\r\n <!ELEMENT landscape EMPTY>\r\n <!ENTITY image1 SYSTEM \"d:\\testspec\\images\\sunset.gif\" NDATA gif>\r\n <!ENTITY image2 SYSTEM \"d:\\testspec\\images\\frontpage.gif\" NDATA gif>\r\n <!ATTLIST landscape sun ENTITIES #IMPLIED>\r\n]>\r\n<test>\r\n<landscape sun = \"#image1 @image\"></landscape>\r\nIn an attribute declaration, values of type ENTITIES do not match the Name production.\r\n</test>",
  0,
  NULL,
  "Tests invalid TokenizedType which is against P56 VC: Entity Name. The value of the ENTITIES attribute \"sun\" is \"#image1 @image\" which does not meet the Names production."
},
{
  "invalid/P56/ibm56i15.xml",
  583,
  "<?xml  version=\"1.0\"?>\r\n<!-- validity constraint check for Production 56(Negative test)-->\r\n<!DOCTYPE test\r\n [\r\n <!ELEMENT test ANY>\r\n <!ELEMENT landscape EMPTY>\r\n <!ENTITY image1 SYSTEM \"d:\\testspec\\images\\sunset.gif\" NDATA gif>\r\n <!ENTITY image2 SYSTEM \"d:\\testspec\\images\\frontpag.gif\" NDATA gif>\r\n <!ATTLIST landscape sun ENTITIES #IMPLIED>\r\n]>\r\n<test>\r\n<landscape sun = \"image3 image4\"></landscape>\r\nIn an attribute declaration, values of type ENTITIES match the Name production and the ENTITIES value\r\ndoes not match one or more names of entities declared in the DTD. \r\n</test>",
  0,
  NULL,
  "Tests invalid TokenizedType which is against P56 VC: ENTITIES. The value of the ENTITIES attribute \"sun\" is \"image3 image4\" which does not match the names of two unparsed entities declared."
},
{
  "invalid/P56/ibm56i16.xml",
  558,
  "<?xml  version=\"1.0\"?>\r\n<!-- validity constraint check for Production 56(Negative test)-->\r\n<!DOCTYPE test\r\n [\r\n <!ELEMENT test ANY>\r\n <!ELEMENT landscape EMPTY>\r\n <!ENTITY parsedentity1 SYSTEM \"ibm56iv01.xml\">\r\n <!ENTITY parsedentity2 SYSTEM \"ibm56iv02.xml\">\r\n <!ATTLIST landscape sun ENTITIES #IMPLIED>\r\n]>\r\n<test>\r\n<landscape sun = \"parsedentity1 parsedentity2\"></landscape>\r\nIn an attribute declaration, values of type ENTITIES match the Name production and the ENTITIES value\r\nmatches one or more names of parsed entities declared in the DTD. .\r\n</test>",
  0,
  NULL,
  "Tests invalid TokenizedType which is against P56 VC: ENTITIES. The value of the ENTITIES attribute \"sun\" is \"parsedentity1 parsedentity2\" which matches the names of two parsed entities instead of two unparsed entities declared."
},
{
  "invalid/P56/ibm56i17.xml",
  372,
  "<?xml  version=\"1.0\"?>\r\n<!-- validity constraint check for Production 56(Negative Test)-->\r\n<!DOCTYPE test\r\n [\r\n <!ELEMENT test ANY>\r\n <!ELEMENT nametoken EMPTY>\r\n <!ATTLIST nametoken thistoken NMTOKEN #IMPLIED>\r\n]>\r\n<test>\r\n<nametoken thistoken = \"x : image\"></nametoken>\r\nIn an attribute declaration, values of type NMTOKEN does not match the Nmtoken production\r\n</test>",
  0,
  NULL,
  "Tests invalid TokenizedType which is against P56 VC: Name Token. The value of the NMTOKEN attribute \"thistoken\" is \"x : image\" which does not meet the Nmtoken production."
},
{
  "invalid/P56/ibm56i18.xml",
  387,
  "<?xml  version=\"1.0\"?>\r\n<!-- validity constraint check for Production 56(Negative Test)-->\r\n<!DOCTYPE test\r\n [\r\n <!ELEMENT test ANY>\r\n <!ELEMENT nametokens EMPTY>\r\n <!ATTLIST nametokens thistoken NMTOKENS #IMPLIED>\r\n]>\r\n<test>\r\n<nametokens thistoken = \"@lang y: #country\"></nametokens>\r\nIn an attribute declaration, values of type NMTOKENS does not match the Nmtokens production\r\n</test>",
  0,
  NULL,
  "Tests invalid TokenizedType which is against P56 VC: Name Token. The value of the NMTOKENS attribute \"thistoken\" is \"@lang y: #country\" which does not meet the Nmtokens production."
},
{
  "invalid/P58/ibm58i01.xml",
  560,
  "<?xml  version=\"1.0\"?>\r\n<!-- validity test for Production 58(Negative Test-->\r\n<!DOCTYPE test\r\n [\r\n <!ELEMENT test ANY>\r\n <!ELEMENT blob (#PCDATA)>\r\n <!NOTATION base64 SYSTEM \"mimecode\">\r\n <!NOTATION uuencode SYSTEM \"uudecode\">\r\n <!NOTATION raw SYSTEM \"raw\">\r\n <!ATTLIST blob content-encoding NOTATION (base64|uuencode) #REQUIRED>\r\n ]>\r\n <test>\r\n<blob content-encoding = \"raw\"></blob>\r\nThe attribute values of type NOTATION does not match any of the notation names included in the\r\ndeclaration.All notation names in the declaration have been declared.\r\n</test>",
  0,
  NULL,
  "Tests invalid NotationType which is against P58 VC: Notation Attributes. The attribute \"content-encoding\" with value \"raw\" is not a value from the list \"(base64|uuencode)\"."
},
{
  "invalid/P58/ibm58i02.xml",
  547,
  "<?xml  version=\"1.0\"?>\r\n<!-- validity test for Production 58(Negative Test-->\r\n<!DOCTYPE test\r\n [\r\n <!ELEMENT test ANY>\r\n <!ELEMENT blob (#PCDATA)>\r\n <!NOTATION base64 SYSTEM \"mimecode\">\r\n <!NOTATION uuencode SYSTEM \"uudecode\">\r\n <!ATTLIST blob content-encoding NOTATION (base64|uuencode|raw|ascii) #REQUIRED>\r\n ]>\r\n <test>\r\n<blob content-encoding = \"raw\"></blob>\r\nThe attribute values of type NOTATION does match any of the notation names included in the\r\ndeclaration, but some of notation names in the declaration have not been declared\r\n</test>",
  0,
  NULL,
  "Tests invalid NotationType which is against P58 VC: Notation Attributes. The attribute \"content-encoding\" with value \"raw\" is a value from the list \"(base64|uuencode|raw|ascii)\", but \"raw\" is not a declared notation."
},
{
  "invalid/P59/ibm59i01.xml",
  394,
  "<?xml  version=\"1.0\"?>\r\n<!-- validity test for Production 59-->\r\n<!DOCTYPE test\r\n [\r\n <!ELEMENT test ANY>\r\n <!ELEMENT one EMPTY>\r\n <!ELEMENT two EMPTY>\r\n <!ELEMENT num EMPTY>\r\n <!ATTLIST num value (one|two) #IMPLIED>\r\n ]>\r\n <test>\r\n<num value = \"ONE\"></num>\r\nThis is a Negative test\r\nThe attribute values of type Enumeration does not match any of the Nmtoken tokens in the declaration.\r\n</test>",
  0,
  NULL,
  "Tests invalid Enumeration which is against P59 VC: Enumeration. The value of the attribute is \"ONE\" which matches neither \"one\" nor \"two\" as declared in the Enumeration in the AttDef in the AttlistDecl."
},
{
  "invalid/P60/ibm60i01.xml",
  472,
  "<?xml  version=\"1.0\"?>\r\n<!-- validity test for Production 60-->\r\n<!DOCTYPE Java \r\n [\r\n <!ELEMENT Java ANY>\r\n <!ELEMENT one EMPTY>\r\n <!ELEMENT two EMPTY>\r\n <!ATTLIST one chapter CDATA #REQUIRED>\r\n <!ATTLIST two chapter CDATA #REQUIRED>\r\n ]>\r\n<Java>\r\n<one chapter=\"Introduction\"></one>\r\n<two></two>\r\nNegative test for Required Attribute. Some occurrence of an element with \r\nan attribute of #REQUIRED default declaration does not give the value of \r\nthose attribute\r\n</Java>",
  0,
  NULL,
  "Tests invalid DefaultDecl which is against P60 VC: Required Attribute. The attribute \"chapter\" for the element \"two\" is declared as #REQUIRED in the DefaultDecl in the AttlistDecl, but the value of this attribute is not given."
},
{
  "invalid/P60/ibm60i02.xml",
  443,
  "<?xml  version=\"1.0\"?>\r\n<!-- validity test for Production 60-->\r\n<!DOCTYPE Java \r\n [\r\n <!ELEMENT Java ANY>\r\n <!ELEMENT one EMPTY>\r\n <!ATTLIST one chapter CDATA #FIXED \"Introduction\">\r\n  ]>\r\n<Java>\r\n<one chapter=\"JavaBeans\"></one>\r\nNegative Test\r\nAn attribute has a default value declared with the #FIXED keyword, \r\nand an instances of that attribute is given a value which is not \r\nthe same as the default value in the declaration. \r\n</Java>\r\n",
  0,
  NULL,
  "Tests invalid DefaultDecl which is against P60 VC: Fixed Attribute Default.. The attribute \"chapter\" for the element \"one\" is declared as #FIXED with the given value \"Introduction\" in the DefaultDecl in the AttlistDecl, but the value of a instance of this attribute is assigned to \"JavaBeans\"."
},
{
  "invalid/P60/ibm60i03.xml",
  363,
  "<?xml  version=\"1.0\"?>\r\n<!-- validity test for Production 60-->\r\n<!DOCTYPE test\r\n [\r\n <!ELEMENT test ANY>\r\n <!ELEMENT a EMPTY>\r\n <!ELEMENT b EMPTY>\r\n <!ELEMENT attr EMPTY>\r\n <!ATTLIST attr value (a|b) \"c\"> \r\n  ]>\r\n<test>\r\nThe default value specified for an attribute does not meet the \r\nlexical constraints of the declared attribute type.\r\n</test>\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n",
  0,
  NULL,
  "Tests invalid DefaultDecl which is against P60 VC: Attribute Default Legal. The declared default value \"c\" is not legal for the type (a|b) in the AttDef in the AttlistDecl."
},
{
  "invalid/P60/ibm60i04.xml",
  346,
  "<?xml  version=\"1.0\"?>\r\n<!-- validity test for Production 60-->\r\n<!DOCTYPE test\r\n [\r\n <!ELEMENT test ANY>\r\n <!ELEMENT a EMPTY>\r\n <!ELEMENT nametoken EMPTY>\r\n <!ATTLIST nametoken namevalue NMTOKEN \"@#$\"> \r\n  ]>\r\n<test>\r\nThe default value specified for an attribute does not meet the \r\nlexical constraints of the declared attribute type.\r\n</test>\r\n",
  0,
  NULL,
  "Tests invalid DefaultDecl which is against P60 VC: Attribute Default Legal. The declared default value \"@#$\" is not legal for the type NMTOKEN the AttDef in the AttlistDecl."
},
{
  "invalid/P68/ibm68i01.xml",
  211,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root SYSTEM \"ibm68i01.dtd\" [\r\n  <!ELEMENT root (#PCDATA|a)* >\r\n]>\r\n<root>\r\n  pcdata content\r\n  <a attr1=\"xyz\"/>\r\n</root>\r\n<!--* a invalid test for P68 VC:Entity Declared *-->\r\n\r\n",
  0,
  NULL,
  "Tests invalid EntityRef which is against P68 VC: Entity Declared. The GE with the name \"ge2\" is referred in the file ibm68i01.dtd\", but not declared."
},
{
  "invalid/P68/ibm68i02.xml",
  211,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root SYSTEM \"ibm68i02.dtd\" [\r\n  <!ELEMENT root (#PCDATA|a)* >\r\n]>\r\n<root>\r\n  pcdata content\r\n  <a attr1=\"xyz\"/>\r\n</root>\r\n<!--* a invalid test for P68 VC:Entity Declared *-->\r\n\r\n",
  0,
  NULL,
  "Tests invalid EntityRef which is against P68 VC: Entity Declared. The GE with the name \"ge1\" is referred before declared in the file ibm68i01.dtd\"."
},
{
  "invalid/P68/ibm68i03.xml",
  231,
  "<?xml version=\"1.0\" standalone='no'?>\r\n<!DOCTYPE root [\r\n  <!ELEMENT root (#PCDATA)* >\r\n  <!ENTITY % pe1 SYSTEM \"ibm68i03.ent\">\r\n  %pe1;\r\n]>\r\n<root>\r\n  pcdata content\r\n</root>\r\n<!--* a invalid test for P68 VC:Entity Declared *-->\r\n",
  0,
  NULL,
  "Tests invalid EntityRef which is against P68 VC: Entity Declared. The GE with the name \"ge2\" is referred in the file ibm68i03.ent\", but not declared."
},
{
  "invalid/P68/ibm68i04.xml",
  231,
  "<?xml version=\"1.0\" standalone='no'?>\r\n<!DOCTYPE root [\r\n  <!ELEMENT root (#PCDATA)* >\r\n  <!ENTITY % pe1 SYSTEM \"ibm68i04.ent\">\r\n  %pe1;\r\n]>\r\n<root>\r\n  pcdata content\r\n</root>\r\n<!--* a invalid test for P68 VC:Entity Declared *-->\r\n",
  0,
  NULL,
  "Tests invalid EntityRef which is against P68 VC: Entity Declared. The GE with the name \"ge1\" is referred before declared in the file ibm68i04.ent\"."
},
{
  "invalid/P69/ibm69i01.xml",
  211,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root SYSTEM \"ibm69i01.dtd\" [\r\n  <!ELEMENT root (#PCDATA|a)* >\r\n]>\r\n<root>\r\n  pcdata content\r\n  <a attr1=\"xyz\"/>\r\n</root>\r\n<!--* a invalid test for P69 VC:Entity Declared *-->\r\n\r\n",
  0,
  NULL,
  "Tests invalid PEReference which is against P69 VC: Entity Declared. The Name \"pe2\" in the PEReference in the file ibm69i01.dtd does not match the Name of any declared PE."
},
{
  "invalid/P69/ibm69i02.xml",
  211,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root SYSTEM \"ibm69i02.dtd\" [\r\n  <!ELEMENT root (#PCDATA|a)* >\r\n]>\r\n<root>\r\n  pcdata content\r\n  <a attr1=\"xyz\"/>\r\n</root>\r\n<!--* a invalid test for P69 VC:Entity Declared *-->\r\n\r\n",
  0,
  NULL,
  "Tests invalid PEReference which is against P69 VC: Entity Declared. The PE with the name \"pe1\" is referred before declared in the file ibm69i02.dtd"
},
{
  "invalid/P69/ibm69i03.xml",
  231,
  "<?xml version=\"1.0\" standalone='no'?>\r\n<!DOCTYPE root [\r\n  <!ELEMENT root (#PCDATA)* >\r\n  <!ENTITY % pe1 SYSTEM \"ibm69i03.ent\">\r\n  %pe1;\r\n]>\r\n<root>\r\n  pcdata content\r\n</root>\r\n<!--* a invalid test for P69 VC:Entity Declared *-->\r\n",
  0,
  NULL,
  "Tests invalid PEReference which is against P69 VC: Entity Declared. The Name \"pe3\" in the PEReference in the file ibm69i03.ent does not match the Name of any declared PE."
},
{
  "invalid/P69/ibm69i04.xml",
  232,
  "<?xml version=\"1.0\" standalone='no'?>\r\n<!DOCTYPE root  [\r\n  <!ELEMENT root (#PCDATA)* >\r\n  <!ENTITY % pe1 SYSTEM \"ibm69i04.ent\">\r\n  %pe1;\r\n]>\r\n<root>\r\n  pcdata content\r\n</root>\r\n<!--* a invalid test for P69 VC:Entity Declared *-->\r\n",
  0,
  NULL,
  "Tests invalid PEReference which is against P69 VC: Entity Declared. The PE with the name \"pe2\" is referred before declared in the file ibm69i04.ent."
},
{
  "invalid/P76/ibm76i01.xml",
  387,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n\r\n<!ENTITY % pe1 '<!ATTLIST root att2 CDATA \"&ge1;\">'>\r\n<!ENTITY ge1 \"attdefaultvalue\" >\r\n%pe1;\r\n\r\n<!--* notation JPGformat not declared *-->\r\n<!ENTITY ge2  SYSTEM \"image.jpg\" NDATA JPGformat>\r\n\r\n]>\r\n<root att2=\"any\" />\r\n<!-- a invalid test case: test P76 VC: Notation Declared -->",
  0,
  NULL,
  "Tests invalid NDataDecl which is against P76 VC: Notation declared. The Name \"JPGformat\" in the NDataDecl in the EntityDecl for \"ge2\" does not match the Name of any declared notation."
},
/* ==== from ibm ibm_oasis_valid.xml ==== */
{
  "valid/P01/ibm01v01.xml",
  565,
  "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"yes\" ?>\r\n<!-- Above is XMLDecl -->\r\n<!DOCTYPE animal [\r\n<!ELEMENT animal (cat|tiger|leopard)+>\r\n<!ELEMENT cat EMPTY>\r\n<!ELEMENT tiger (#PCDATA)>\r\n<!ELEMENT leopard ANY>\r\n<!ELEMENT small EMPTY>\r\n<!ELEMENT big EMPTY>\r\n<!ATTLIST tiger color CDATA #REQUIRED>\r\n]>\r\n<!-- Above is DTD -->\r\n<?music \"Here is a PI\" ?> \r\n<animal>\r\n   <cat/>\r\n   <tiger color=\"white\">This is a white tiger in Mirage!!</tiger>\r\n   <cat/>\r\n   <leopard>\r\n      <small/>\r\n      <big/>\r\n   </leopard>\r\n</animal>\r\n<!-- Above is element animal -->\r\n\r\n",
  238,
  "<?music \"Here is a PI\" ?><animal>&#10;   <cat></cat>&#10;   <tiger color=\"white\">This is a white tiger in Mirage!!</tiger>&#10;   <cat></cat>&#10;   <leopard>&#10;      <small></small>&#10;      <big></big>&#10;   </leopard>&#10;</animal>",
  "Tests with a xml document consisting of prolog followed by element then Misc"
},
{
  "valid/P02/ibm02v01.xml",
  345,
  "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\r\n<!DOCTYPE book [\r\n<!ELEMENT book ANY>\r\n<!-- This test case covers     legal character ranges plus\r\n        discrete legal characters for production 02. -->\r\n<?NAME_09-\t_0A-\r\n_0D-\r\n_20- _D7FF-\355\237\277_6c0f-\346\260\217_E000-\356\200\200_FFFD-\357\277\275_effe-\356\277\276_010000-\360\220\200\200_10FFFF-\364\217\277\277_08ffff-\362\217\277\277 This is a PI target ?>\r\n]>\r\n<book/>\r\n\277",
  0,
  NULL,
  "This test case covers legal character ranges plus discrete legal characters for production 02."
},
{
  "valid/P03/ibm03v01.xml",
  269,
  "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\r\r\n<!DOCTYPE book [\r\r\n<!ELEMENT book ANY>\r\r\n<!-- This test case covers 0 legal character ranges plus\r\r\n     4 discrete legal characters for production 03. -->\r\r\n<?NAME_20- _09-\t_0D-\r_0A-\r\r\n This is a PI target ?>\r\r\n]>\r\r\n<book/>\r\r\n",
  0,
  NULL,
  "Tests all 4 legal white space characters - #x20 #x9 #xD #xA"
},
{
  "valid/P09/ibm09v01.xml",
  178,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)> \r\n \t<!ENTITY FullName \"\">\r\n]>\r\n\r\n<student>My Name is &FullName;. </student>\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n ",
  32,
  "<student>My Name is . </student>",
  "Empty EntityValue is legal"
},
{
  "valid/P09/ibm09v02.xml",
  156,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)> \r\n \t<!ENTITY FullName \"SnowMan\">\r\n]>\r\n\r\n<student>My Name is &FullName;. </student>",
  39,
  "<student>My Name is SnowMan. </student>",
  "Tests a normal EnitityValue"
},
{
  "valid/P09/ibm09v03.xml",
  117,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student  SYSTEM \"ibm09v03.dtd\">\r\n<student>I am a new student with &Name;</student>\r\n",
  55,
  "<student>I am a new student with first , last</student>",
  "Tests EnitityValue referencing a Parameter Entity"
},
{
  "valid/P09/ibm09v04.xml",
  237,
  "<?xml version=\"1.0\"?> \r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)>\r\n<!-- testing entity value with Reference -->\r\n\t<!ENTITY RealName \"SnowMan\"> \r\n \t<!ENTITY FullName \"&RealName;\">\r\n]>\r\n\r\n<student>My Name is &FullName;. </student>",
  39,
  "<student>My Name is SnowMan. </student>",
  "Tests EnitityValue referencing a General Entity"
},
{
  "valid/P09/ibm09v05.xml",
  313,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student SYSTEM \"student.dtd\"[\r\n\t<!ELEMENT student (#PCDATA)> \r\n\t<!ENTITY Age \"21\">\r\n\t<!ENTITY Status \"freshman\">\r\n \t<!ENTITY % FullName \"first , last , middle\">\r\n]>\r\n\r\n<!-- testing entity value with combination reference -->\r\n<student>This is a test of &combine;</student>\r\n\r\n\r\n\r\n",
  257,
  "<student>This is a test of My Name is first , last , middle and my age is 21 Again first , last , middle first , last , middle and my status is &#10;&#9;&#9;freshman freshman and first , last , middle 21 first , last , middle freshman That is all.</student>",
  "Tests EnitityValue with combination of GE, PE and text, the GE used is declared in the student.dtd"
},
{
  "valid/P10/ibm10v01.xml",
  393,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)>\r\n\t<!ATTLIST student\r\n\t\tfirst CDATA #REQUIRED\r\n\t\tmiddle CDATA #IMPLIED\r\n\t\tlast CDATA #REQUIRED > \r\n\t<!ENTITY myfirst \"Snow\">\r\n\t<!ENTITY mymiddle \"Y\">\r\n\t<!ENTITY mylast \"\">\r\n]>\r\n<!-- testing AttValue with empty char inside double quote -->\r\n<student first=\"\" last=\"\">My Name is Snow &mylast; Man. </student>\r\n\r\n\r\n\r\n\r\n\r\n\r\n",
  58,
  "<student first=\"\" last=\"\">My Name is Snow  Man. </student>",
  "Tests empty AttValue with double quotes as the delimiters"
},
{
  "valid/P10/ibm10v02.xml",
  383,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)>\r\n\t<!ATTLIST student\r\n\t\tfirst CDATA #REQUIRED\r\n\t\tmiddle CDATA #IMPLIED\r\n\t\tlast CDATA #REQUIRED > \r\n\t<!ENTITY myfirst \"Snow\">\r\n\t<!ENTITY mymiddle \"Y\">\r\n\t<!ENTITY mylast ''>\r\n]>\r\n<!-- testing AttValue with empty char inside single quote -->\r\n<student first='' last=''>My Name is Snow &mylast; Man. </student>\r\n\r\n",
  58,
  "<student first=\"\" last=\"\">My Name is Snow  Man. </student>",
  "Tests empty AttValue with single quotes as the delimiters"
},
{
  "valid/P10/ibm10v03.xml",
  390,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)>\r\n\t<!ATTLIST student\r\n\t\tfirst CDATA #REQUIRED\r\n\t\tmiddle CDATA #IMPLIED\r\n\t\tlast CDATA #REQUIRED > \r\n\t<!ENTITY myfirst 'Snow'>\r\n\t<!ENTITY mymiddle 'I'>\r\n\t<!ENTITY mylast \"Man'\">\r\n]>\r\n<!-- testing AttValue string with a single quote inside -->\r\n<student first=\"Snow'\" last=\"Man\">My Name is &myfirst; &mylast;. </student>",
  66,
  "<student first=\"Snow'\" last=\"Man\">My Name is Snow Man'. </student>",
  "Test AttValue with double quotes as the delimiters and single quote inside"
},
{
  "valid/P10/ibm10v04.xml",
  394,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)>\r\n\t<!ATTLIST student\r\n\t\tfirst CDATA #REQUIRED\r\n\t\tmiddle CDATA #IMPLIED\r\n\t\tlast CDATA #REQUIRED > \r\n\t<!ENTITY myfirst 'Snow'>\r\n\t<!ENTITY mymiddle 'I'>\r\n\t<!ENTITY mylast 'Man\"'>\r\n]>\r\n<!-- testing AttValue string with a double quote inside -->\r\n<student first='Snow\"' last='Man'>My Name is &myfirst; &mylast;. </student>\r\n\r\n",
  76,
  "<student first=\"Snow&quot;\" last=\"Man\">My Name is Snow Man&quot;. </student>",
  "Test AttValue with single quotes as the delimiters and double quote inside"
},
{
  "valid/P10/ibm10v05.xml",
  400,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)>\r\n\t<!ATTLIST student\r\n\t\tfirst CDATA #REQUIRED\r\n\t\tmiddle CDATA #IMPLIED\r\n\t\tlast CDATA #REQUIRED > \r\n\t<!ENTITY myfirst \"Snow\">\r\n\t<!ENTITY mymiddle \"Y\">\r\n\t<!ENTITY mylast \"&myfirst; Man\">\r\n]>\r\n<!-- testing AttValue with a reference in double quote -->\r\n<student first=\"&myfirst;\" last=\"mylast;\">My Name is &mylast;. </student>\r\n\r\n",
  68,
  "<student first=\"Snow\" last=\"mylast;\">My Name is Snow Man. </student>",
  "Test AttValue with a GE reference and double quotes as the delimiters"
},
{
  "valid/P10/ibm10v06.xml",
  401,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)>\r\n\t<!ATTLIST student\r\n\t\tfirst CDATA #REQUIRED\r\n\t\tmiddle CDATA #IMPLIED\r\n\t\tlast CDATA #REQUIRED > \r\n\t<!ENTITY myfirst \"Snow\">\r\n\t<!ENTITY mymiddle \"Y\">\r\n\t<!ENTITY mylast '&myfirst; Man'>\r\n]>\r\n<!-- testing AttValue with a reference in single quote -->\r\n<student first='&myfirst;' last='&mylast;'>My Name is &mylast;. </student>\r\n\r\n",
  69,
  "<student first=\"Snow\" last=\"Snow Man\">My Name is Snow Man. </student>",
  "Test AttValue with a GE reference and single quotes as the delimiters"
},
{
  "valid/P10/ibm10v07.xml",
  519,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)>\r\n\t<!ATTLIST student\r\n\t\tfirst CDATA #REQUIRED\r\n\t\tmiddle CDATA #IMPLIED\r\n\t\tlast CDATA #REQUIRED > \r\n\t<!ENTITY myfirst \"Snow\">\r\n\t<!ENTITY mymiddle \"Y\">\r\n\t<!ENTITY mylast \"Man &myfirst; and &myfirst; mymiddle;.\">\r\n]>\r\n<!-- testing AttValue with references combination in double quotes -->\r\n<student first=\"Full Name &myfirst; &#x31; and &mylast; &mylast; &#x63;\" last=\"&mylast;\" >My first Name is &myfirst; and my last name is &mylast;. </student>\r\n",
  216,
  "<student first=\"Full Name Snow 1 and Man Snow and Snow mymiddle;. Man Snow and Snow mymiddle;. c\" last=\"Man Snow and Snow mymiddle;.\">My first Name is Snow and my last name is Man Snow and Snow mymiddle;.. </student>",
  "testing AttValue with mixed references and text content in double quotes"
},
{
  "valid/P10/ibm10v08.xml",
  517,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)>\r\n\t<!ATTLIST student\r\n\t\tfirst CDATA #REQUIRED\r\n\t\tmiddle CDATA #IMPLIED\r\n\t\tlast CDATA #REQUIRED > \r\n\t<!ENTITY myfirst \"Snow\">\r\n\t<!ENTITY mymiddle \"I\">\r\n\t<!ENTITY mylast 'Man &myfirst; and &myfirst; mymiddle;.'>\r\n]>\r\n<!-- testing AttValue with references combination in single quote -->\r\n<student first='Full Name &myfirst; and &#x22;&mylast;&#x22; &mylast;' last='&mylast;'>My first Name is &myfirst; and my last name is &mylast;. </student>\r\n\r\n",
  224,
  "<student first=\"Full Name Snow and &quot;Man Snow and Snow mymiddle;.&quot; Man Snow and Snow mymiddle;.\" last=\"Man Snow and Snow mymiddle;.\">My first Name is Snow and my last name is Man Snow and Snow mymiddle;.. </student>",
  "testing AttValue with mixed references and text content in single quotes"
},
{
  "valid/P11/ibm11v01.xml",
  242,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n   <!ELEMENT student (#PCDATA)>\r\n   <!ENTITY unref SYSTEM \"\">\r\n]>\r\n\r\n<!-- testing systemliteral with nothing between the double quotes -->\r\n<student>My Name is SnowMan. </student>\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n ",
  39,
  "<student>My Name is SnowMan. </student>",
  "Tests empty systemliteral using the double quotes"
},
{
  "valid/P11/ibm11v02.xml",
  225,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n   <!ELEMENT student (#PCDATA)>\r\n   <!ENTITY unref SYSTEM ''>\r\n]>\r\n\r\n<!-- testing systemliteral with nothing between the single quotes -->\r\n<student>My Name is SnowMan. </student>\r\n",
  39,
  "<student>My Name is SnowMan. </student>",
  "Tests empty systemliteral using the single quotes"
},
{
  "valid/P11/ibm11v03.xml",
  164,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student SYSTEM 'student.dtd'[\r\n]>\r\n<!-- testing systemliteral with a string with \"'\" -->\r\n<student>My Name is SnowMan. </student>\r\n",
  39,
  "<student>My Name is SnowMan. </student>",
  "Tests regular systemliteral using the single quotes"
},
{
  "valid/P11/ibm11v04.xml",
  169,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student SYSTEM \"student.dtd\" [\r\n]>\r\n\r\n<!-- testing systemliteral with a string with '\"' -->\r\n<student>My Name is SnowMan. </student>\r\n\r\n",
  39,
  "<student>My Name is SnowMan. </student>",
  "Tests regular systemliteral using the double quotes"
},
{
  "valid/P12/ibm12v01.xml",
  199,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student PUBLIC \"\" \"student.dtd\"[\r\n]>\r\n\r\n<!-- testing Pubid Literal with nothing between the double quote -->\r\n<student>My Name is SnowMan. </student>\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n ",
  39,
  "<student>My Name is SnowMan. </student>",
  "Tests empty systemliteral using the double quotes"
},
{
  "valid/P12/ibm12v02.xml",
  185,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student PUBLIC '' 'student.dtd'[\r\n]>\r\n\r\n<!-- testing Pubid Literal with nothing between the single quotes -->\r\n<student>My Name is SnowMan. </student>\r\n",
  39,
  "<student>My Name is SnowMan. </student>",
  "Tests empty systemliteral using the single quotes"
},
{
  "valid/P12/ibm12v03.xml",
  191,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student PUBLIC \"The big ' in it\" \"student.dtd\"[\r\n]>\r\n\r\n<!-- testing Pubid Literal with a string with \"'\" inside -->\r\n<student>My Name is SnowMan. </student>\r\n",
  39,
  "<student>My Name is SnowMan. </student>",
  "Tests regular systemliteral using the double quotes"
},
{
  "valid/P12/ibm12v04.xml",
  198,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student PUBLIC 'The latest version' 'student.dtd'[\r\n]>\r\n\r\n<!-- testing Pubid Literal with a string without  \"'\" inside -->\r\n<student>My Name is SnowMan. </student>\r\n",
  39,
  "<student>My Name is SnowMan. </student>",
  "Tests regular systemliteral using the single quotes"
},
{
  "valid/P13/ibm13v01.xml",
  283,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student PUBLIC \"#x20 #xD #xA abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ -'()+,./:=?;!*#@$_% \" \"student.dtd\"[\r\n]>\r\n\r\n<!-- testing Pubid char with all legal pubidchar in a string -->\r\n<student>My Name is SnowMan. </student>\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n ",
  39,
  "<student>My Name is SnowMan. </student>",
  "Testing PubidChar with all legal PubidChar in a PubidLiteral"
},
{
  "valid/P14/ibm14v01.xml",
  234,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)>\r\n\t<!ATTLIST student first CDATA #REQUIRED\r\n\t\t\t  last  CDATA #IMPLIED>\r\n]>\r\n\r\n<!-- testing chardata with empty string -->\r\n<student first=\"Snow\"></student>\r\n\r\n\r\n",
  32,
  "<student first=\"Snow\"></student>",
  "Testing CharData with empty string"
},
{
  "valid/P14/ibm14v02.xml",
  260,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)>\r\n\t<!ATTLIST student first CDATA #REQUIRED\r\n\t\t\t  last  CDATA #IMPLIED>\r\n]>\r\n\r\n<!-- testing chardata with white space -->\r\n<student first=\"Eric\"> &#x0A; &#x09; &#x0D;&#x20;</student>\r\n\r\n\r\n",
  50,
  "<student first=\"Eric\"> &#10; &#9; &#13; </student>",
  "Testing CharData with white space character"
},
{
  "valid/P14/ibm14v03.xml",
  324,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)>\r\n\t<!ATTLIST student first CDATA #REQUIRED\r\n\t\t\t  last  CDATA #IMPLIED>\r\n]>\r\n\r\n<!-- testing chardata with a string of sample legal char except '<' and '&' nor does it contain sequence \"]]>\" -->\r\n<student first=\"Snow\" last=\"Man\">This is a test</student>",
  57,
  "<student first=\"Snow\" last=\"Man\">This is a test</student>",
  "Testing CharData with a general text string"
},
{
  "valid/P15/ibm15v01.xml",
  166,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)> \r\n]>\r\n<!--* Tests empty comment *-->\r\n<!---->\r\n<student>My Name is SnowMan. </student>\r\n\r\n\r\n",
  39,
  "<student>My Name is SnowMan. </student>",
  "Tests empty comment"
},
{
  "valid/P15/ibm15v02.xml",
  146,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)> \r\n]>\r\n\r\n<!-- Student's name -->\r\n<student>My Name is SnowMan. </student>",
  39,
  "<student>My Name is SnowMan. </student>",
  "Tests comment with regular text"
},
{
  "valid/P15/ibm15v03.xml",
  146,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)> \r\n]>\r\n\r\n<!-- student file-1 -->\r\n<student>My Name is SnowMan. </student>",
  39,
  "<student>My Name is SnowMan. </student>",
  "Tests comment with one dash inside"
},
{
  "valid/P15/ibm15v04.xml",
  172,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)> \r\n]>\r\n\r\n<!--student phone number 408-398 (387)-4758 -->\r\n<student>My Name is SnowMan. </student>\r\n",
  39,
  "<student>My Name is SnowMan. </student>",
  "Tests comment with more comprehensive content"
},
{
  "valid/P16/ibm16v01.xml",
  143,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)> \r\n]>\r\n\r\n<?MyInstruct?>\r\n<student>My Name is SnowMan. </student>\r\n\r\n\r\n",
  54,
  "<?MyInstruct ?><student>My Name is SnowMan. </student>",
  "Tests PI definition with only PItarget name and nothing else"
},
{
  "valid/P16/ibm16v02.xml",
  138,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)> \r\n]>\r\n\r\n<?MyInstruct ?>\r\n<student>My Name is SnowMan. </student>",
  54,
  "<?MyInstruct ?><student>My Name is SnowMan. </student>",
  "Tests PI definition with only PItarget name and a white space"
},
{
  "valid/P16/ibm16v03.xml",
  161,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)> \r\n]>\r\n\r\n<?MyInstruct AVOID ? BEFORE > IN PI ?>\r\n<student>My Name is SnowMan. </student>",
  77,
  "<?MyInstruct AVOID ? BEFORE > IN PI ?><student>My Name is SnowMan. </student>",
  "Tests PI definition with PItarget name and text that contains question mark and right angle"
},
{
  "valid/P17/ibm17v01.xml",
  159,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)> \r\n]>\r\n\r\n<?MyInstruct This is a test ?>\r\n<student>My Name is SnowMan. </student>\r\n\r\n\r\n",
  69,
  "<?MyInstruct This is a test ?><student>My Name is SnowMan. </student>",
  "Tests PITarget name"
},
{
  "valid/P18/ibm18v01.xml",
  211,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)> \r\n]>\r\n\r\n<!-- testing CDSect with CDStart CData CDEnd -->\r\n\r\n<student>My Name is SnowMan. <![CDATA[This is <normal> text]]> </student>\r\n\r\n",
  67,
  "<student>My Name is SnowMan. This is &lt;normal&gt; text </student>",
  "Tests CDSect with CDStart CData CDEnd"
},
{
  "valid/P19/ibm19v01.xml",
  180,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)> \r\n]>\r\n\r\n<!-- testing CDStart -->\r\n<student>My Name is SnowMan. <![CDATA[This is a test]]> </student>\r\n\r\n\r\n",
  54,
  "<student>My Name is SnowMan. This is a test </student>",
  "Tests CDStart"
},
{
  "valid/P20/ibm20v01.xml",
  183,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)> \r\n]>\r\n\r\n<!-- testing CData with empty string -->\r\n\r\n<student>My Name is SnowMan. <![CDATA[]]></student>\r\n\r\n\r\n",
  39,
  "<student>My Name is SnowMan. </student>",
  "Tests CDATA with empty string"
},
{
  "valid/P20/ibm20v02.xml",
  209,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)> \r\n]>\r\n\r\n<!-- testing CData with legal chars -->\r\n\r\n<student>My Name is SnowMan. <![CDATA[<testing>This is a test</testing>]]></student>",
  84,
  "<student>My Name is SnowMan. &lt;testing&gt;This is a test&lt;/testing&gt;</student>",
  "Tests CDATA with regular content"
},
{
  "valid/P21/ibm21v01.xml",
  181,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)> \r\n]>\r\n\r\n<!-- testing CDEnd --> \r\n\r\n<student>My Name is SnowMan. <![CDATA[This is a test]]> </student>\r\n\r\n\r\n",
  54,
  "<student>My Name is SnowMan. This is a test </student>",
  "Tests CDEnd"
},
{
  "valid/P22/ibm22v01.xml",
  92,
  "<?xml version=\"1.0\" encoding=\"utf-8\" ?>\r\n<!DOCTYPE doc [\r\n<!ELEMENT doc EMPTY>\r\n]>\r\n<doc/>\r\n",
  11,
  "<doc></doc>",
  "Tests prolog with XMLDecl and doctypedecl"
},
{
  "valid/P22/ibm22v02.xml",
  51,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc EMPTY>\r\n]>\r\n<doc/>\r\n",
  11,
  "<doc></doc>",
  "Tests prolog with doctypedecl"
},
{
  "valid/P22/ibm22v03.xml",
  76,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc EMPTY>\r\n]>\r\n<!-- This is a Misc -->\r\n<doc/>\r\n",
  11,
  "<doc></doc>",
  "Tests prolog with Misc doctypedecl"
},
{
  "valid/P22/ibm22v04.xml",
  76,
  "<!-- This is a Misc -->\r\n<!DOCTYPE doc [\r\n<!ELEMENT doc EMPTY>\r\n]>\r\n<doc/>\r\n",
  11,
  "<doc></doc>",
  "Tests prolog with doctypedecl Misc"
},
{
  "valid/P22/ibm22v05.xml",
  117,
  "<?xml version=\"1.0\" encoding=\"utf-8\" ?>\r\n<!-- This is a Misc -->\r\n<!DOCTYPE doc [\r\n<!ELEMENT doc EMPTY>\r\n]>\r\n<doc/>\r\n",
  11,
  "<doc></doc>",
  "Tests prolog with XMLDecl Misc doctypedecl"
},
{
  "valid/P22/ibm22v06.xml",
  117,
  "<?xml version=\"1.0\" encoding=\"utf-8\" ?>\r\n<!DOCTYPE doc [\r\n<!ELEMENT doc EMPTY>\r\n]>\r\n<!-- This is a Misc -->\r\n<doc/>\r\n",
  11,
  "<doc></doc>",
  "Tests prolog with XMLDecl doctypedecl Misc"
},
{
  "valid/P22/ibm22v07.xml",
  142,
  "<?xml version=\"1.0\" encoding=\"utf-8\" ?>\r\n<!-- This is a Misc -->\r\n<!DOCTYPE doc [\r\n<!ELEMENT doc EMPTY>\r\n]>\r\n<!-- This is a Misc -->\r\n<doc/>\r\n",
  11,
  "<doc></doc>",
  "Tests prolog with XMLDecl Misc doctypedecl Misc"
},
{
  "valid/P23/ibm23v01.xml",
  74,
  "<?xml version='1.0'?>\r\n<!DOCTYPE doc [\r\n<!ELEMENT doc EMPTY>\r\n]>\r\n<doc/>\r\n",
  11,
  "<doc></doc>",
  "Tests XMLDecl with VersionInfo only"
},
{
  "valid/P23/ibm23v02.xml",
  92,
  "<?xml version='1.0' encoding='UTF-8' ?>\r\n<!DOCTYPE doc [\r\n<!ELEMENT doc EMPTY>\r\n]>\r\n<doc/>\r\n",
  11,
  "<doc></doc>",
  "Tests XMLDecl with VersionInfo EncodingDecl"
},
{
  "valid/P23/ibm23v03.xml",
  92,
  "<?xml version='1.0' standalone='yes' ?>\r\n<!DOCTYPE doc [\r\n<!ELEMENT doc EMPTY>\r\n]>\r\n<doc/>\r\n",
  11,
  "<doc></doc>",
  "Tests XMLDecl with VersionInfo SDDecl"
},
{
  "valid/P23/ibm23v04.xml",
  75,
  "<?xml version='1.0' ?>\r\n<!DOCTYPE doc [\r\n<!ELEMENT doc EMPTY>\r\n]>\r\n<doc/>\r\n",
  11,
  "<doc></doc>",
  "Tests XMLDecl with VerstionInfo and a trailing whitespace char"
},
{
  "valid/P23/ibm23v05.xml",
  108,
  "<?xml version='1.0' encoding='UTF-8' standalone='yes'?>\r\n<!DOCTYPE doc [\r\n<!ELEMENT doc EMPTY>\r\n]>\r\n<doc/>\r\n",
  11,
  "<doc></doc>",
  "Tests XMLDecl with VersionInfo EncodingDecl SDDecl"
},
{
  "valid/P23/ibm23v06.xml",
  109,
  "<?xml version='1.0' encoding='UTF-8' standalone='yes' ?>\r\n<!DOCTYPE doc [\r\n<!ELEMENT doc EMPTY>\r\n]>\r\n<doc/>\r\n",
  11,
  "<doc></doc>",
  "Tests XMLDecl with VersionInfo EncodingDecl SDDecl and a trailing whitespace"
},
{
  "valid/P24/ibm24v01.xml",
  74,
  "<?xml version='1.0'?>\r\n<!DOCTYPE doc [\r\n<!ELEMENT doc EMPTY>\r\n]>\r\n<doc/>\r\n",
  11,
  "<doc></doc>",
  "Tests VersionInfo with single quote"
},
{
  "valid/P24/ibm24v02.xml",
  74,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE doc [\r\n<!ELEMENT doc EMPTY>\r\n]>\r\n<doc/>\r\n",
  11,
  "<doc></doc>",
  "Tests VersionInfo with double quote"
},
{
  "valid/P25/ibm25v01.xml",
  74,
  "<?xml version='1.0'?>\r\n<!DOCTYPE doc [\r\n<!ELEMENT doc EMPTY>\r\n]>\r\n<doc/>\r\n",
  11,
  "<doc></doc>",
  "Tests EQ with ="
},
{
  "valid/P25/ibm25v02.xml",
  75,
  "<?xml version ='1.0'?>\r\n<!DOCTYPE doc [\r\n<!ELEMENT doc EMPTY>\r\n]>\r\n<doc/>\r\n",
  11,
  "<doc></doc>",
  "Tests EQ with = and spaces on both sides"
},
{
  "valid/P25/ibm25v03.xml",
  75,
  "<?xml version= '1.0'?>\r\n<!DOCTYPE doc [\r\n<!ELEMENT doc EMPTY>\r\n]>\r\n<doc/>\r\n",
  11,
  "<doc></doc>",
  "Tests EQ with = and space in front of it"
},
{
  "valid/P25/ibm25v04.xml",
  76,
  "<?xml version = '1.0'?>\r\n<!DOCTYPE doc [\r\n<!ELEMENT doc EMPTY>\r\n]>\r\n<doc/>\r\n",
  11,
  "<doc></doc>",
  "Tests EQ with = and space after it"
},
{
  "valid/P26/ibm26v01.xml",
  78,
  "<?xml version='1.0' ?>\r\n<!DOCTYPE doc [\r\n   <!ELEMENT doc EMPTY>\r\n]>\r\n<doc/>\r\n",
  11,
  "<doc></doc>",
  "Tests VersionNum 1.0"
},
{
  "valid/P27/ibm27v01.xml",
  112,
  "<?xml version='1.0' ?>\r\n<!DOCTYPE doc [\r\n   <!ELEMENT doc EMPTY>\r\n]>\r\n<doc/>\r\n<!-- This is a comment in Misc -->",
  11,
  "<doc></doc>",
  "Tests Misc with comment"
},
{
  "valid/P27/ibm27v02.xml",
  110,
  "<?xml version='1.0' ?>\r\n<!DOCTYPE doc [\r\n   <!ELEMENT doc EMPTY>\r\n]>\r\n<doc/>\r\n<?sound \"This is a PI in Misc ?>",
  43,
  "<doc></doc><?sound \"This is a PI in Misc ?>",
  "Tests Misc with PI"
},
{
  "valid/P27/ibm27v03.xml",
  107,
  "<?xml version='1.0' ?>\r\n<!DOCTYPE doc [\r\n   <!ELEMENT doc ANY>\r\n]>\r\n<doc>S is in the following Misc</doc>\r\n",
  37,
  "<doc>S is in the following Misc</doc>",
  "Tests Misc with white spaces"
},
{
  "valid/P28/ibm28v01.xml",
  157,
  "<?xml version=\"1.0\" encoding='UTF-8'?>\r\n<!DOCTYPE animal [\r\n   <!ELEMENT animal EMPTY>\r\n]>\r\n<!-- This a valid test file for production [28] --> \r\n<animal/>\r\n",
  17,
  "<animal></animal>",
  "Tests doctypedecl with internal DTD only"
},
{
  "valid/P28/ibm28v02.xml",
  805,
  "<?xml version=\"1.0\" encoding=\"utf-8\" ?>\r\n<!DOCTYPE animal SYSTEM \"ibm28v02.dtd\" [\r\n   <!NOTATION animal_class SYSTEM \"ibm28v02.txt\">\r\n   <!ENTITY forcat \"This is a small cat\">\r\n   <!ELEMENT tiger (#PCDATA)>\r\n   <!ENTITY % make_small \"<!ELEMENT small EMPTY>\">\r\n   <!ENTITY % make_leopard_element \"<!ELEMENT leopard ANY>\">\r\n   <!ENTITY % make_attlist \"<!ATTLIST tiger color CDATA #REQUIRED>\">\r\n   %make_leopard_element; \r\n   <!ELEMENT cat ANY>\r\n   %make_small;\r\n   <!ENTITY % make_big \"<!ELEMENT big EMPTY>\">\r\n   %make_big;\r\n   %make_attlist;\r\n   <?sound \"This is a PI\" ?>\r\n   <!-- This is a valid test file for p28 -->\r\n]>\r\n<animal>\r\n   <cat>&forcat;</cat>\r\n   <tiger color=\"white\">This is a white tiger in Mirage!!</tiger>\r\n   <cat/>\r\n   <leopard>\r\n      <small/>\r\n      <big/>\r\n   </leopard>\r\n</animal>\r\n",
  326,
  "<?sound \"This is a PI\" ?><!DOCTYPE animal [\n<!NOTATION animal_class SYSTEM 'ibm28v02.txt'>\n]>\n<animal>&#10;   <cat>This is a small cat</cat>&#10;   <tiger color=\"white\">This is a white tiger in Mirage!!</tiger>&#10;   <cat></cat>&#10;   <leopard>&#10;      <small></small>&#10;      <big></big>&#10;   </leopard>&#10;</animal>",
  "Tests doctypedecl with external subset and combinations of different markup declarations and PEReferences"
},
{
  "valid/P29/ibm29v01.xml",
  627,
  "<?xml version=\"1.0\" encoding=\"utf-8\" ?>\r\n<!DOCTYPE animal [\r\n   <!ELEMENT animal (cat|tiger|leopard)+>\r\n   <!NOTATION animal_class SYSTEM \"ibm29v01.txt\">\r\n   <!ELEMENT cat ANY>\r\n   <!ENTITY forcat \"This is a small cat\">\r\n   <!ELEMENT tiger (#PCDATA)>\r\n   <!ELEMENT leopard ANY>\r\n   <!ELEMENT small EMPTY>\r\n   <!ELEMENT big EMPTY>\r\n   <!ATTLIST tiger color CDATA #REQUIRED>\r\n   <?sound \"This is a PI\" ?>\r\n   <!-- This is a comment -->\r\n    \r\n]>\r\n<animal>\r\n   <cat>&forcat;</cat>\r\n   <tiger color=\"white\">This is a white tiger in Mirage!!</tiger>\r\n   <cat/>\r\n   <leopard>\r\n      <small/>\r\n      <big/>\r\n   </leopard>\r\n</animal>\r\n",
  326,
  "<?sound \"This is a PI\" ?><!DOCTYPE animal [\n<!NOTATION animal_class SYSTEM 'ibm29v01.txt'>\n]>\n<animal>&#10;   <cat>This is a small cat</cat>&#10;   <tiger color=\"white\">This is a white tiger in Mirage!!</tiger>&#10;   <cat></cat>&#10;   <leopard>&#10;      <small></small>&#10;      <big></big>&#10;   </leopard>&#10;</animal>",
  "Tests markupdecl with combinations of elementdecl, AttlistDecl,EntityDecl, NotationDecl, PI and comment"
},
{
  "valid/P29/ibm29v02.xml",
  690,
  "<?xml version=\"1.0\" encoding=\"utf-8\" ?>\r\n<!DOCTYPE animal [\r\n   <!ELEMENT animal (cat|tiger|leopard)+>\r\n   <!NOTATION animal_class SYSTEM \"ibm29v01.txt\">\r\n   <!ELEMENT cat ANY>\r\n   <!ENTITY forcat \"This is a small cat\">\r\n   <!ELEMENT tiger (#PCDATA)>\r\n   <!ENTITY % make_leopard_element \"<!ELEMENT leopard ANY>\">\r\n   %make_leopard_element; \r\n   <!ELEMENT small EMPTY>\r\n   <!ELEMENT big EMPTY>\r\n   <!ATTLIST tiger color CDATA #REQUIRED>\r\n   <?sound \"This is a PI\" ?>\r\n   <!-- This is a comment -->\r\n    \r\n]>\r\n<animal>\r\n   <cat>&forcat;</cat>\r\n   <tiger color=\"white\">This is a white tiger in Mirage!!</tiger>\r\n   <cat/>\r\n   <leopard>\r\n      <small/>\r\n      <big/>\r\n   </leopard>\r\n</animal>\r\n",
  326,
  "<?sound \"This is a PI\" ?><!DOCTYPE animal [\n<!NOTATION animal_class SYSTEM 'ibm29v01.txt'>\n]>\n<animal>&#10;   <cat>This is a small cat</cat>&#10;   <tiger color=\"white\">This is a white tiger in Mirage!!</tiger>&#10;   <cat></cat>&#10;   <leopard>&#10;      <small></small>&#10;      <big></big>&#10;   </leopard>&#10;</animal>",
  "Tests WFC: PE in internal subset as a positive test"
},
{
  "valid/P30/ibm30v01.xml",
  118,
  "<!DOCTYPE animal SYSTEM \"ibm30v01.dtd\">\r\n<animal/>\r\n<!-- tests extSubset with extSubsetDecl only in the dtd file -->\r\n",
  17,
  "<animal></animal>",
  "Tests extSubset with extSubsetDecl only in the dtd file"
},
{
  "valid/P30/ibm30v02.xml",
  126,
  "<!DOCTYPE animal SYSTEM \"ibm30v02.dtd\">\r\n<animal/>\r\n<!-- tests extSubset with TextDecl and extSubsetDecl in the dtd file -->\r\n",
  17,
  "<animal></animal>",
  "Tests extSubset with TextDecl and extSubsetDecl in the dtd file"
},
{
  "valid/P31/ibm31v01.xml",
  187,
  "<!DOCTYPE animal SYSTEM \"ibm31v01.dtd\">\r\n<animal>\r\n   <tiger/>\r\n</animal>\r\n<!-- tests extSubsetDecl with combinations of markupdecls, conditionalSects, PEReferences and white spaces -->\r\n",
  45,
  "<animal>&#10;   <tiger></tiger>&#10;</animal>",
  "Tests extSubsetDecl with combinations of markupdecls, conditionalSects, PEReferences and white spaces"
},
{
  "valid/P32/ibm32v01.xml",
  210,
  "<?xml version=\"1.0\" standalone=\"no\" ?>\r\n<!DOCTYPE animal SYSTEM \"ibm32v01.dtd\">\r\n<animal/>\r\n<!-- Tests VC: Standalone Document Declaration with absent attribute that has default value and standalone is no -->\r\n",
  32,
  "<animal color=\"yellow\"></animal>",
  "Tests VC: Standalone Document Declaration with absent attribute that has default value and standalone is no"
},
{
  "valid/P32/ibm32v02.xml",
  221,
  "<?xml version=\"1.0\" standalone=\"no\" ?>\r\n<!DOCTYPE animal SYSTEM \"ibm32v02.dtd\">\r\n<animal>&animal_content;</animal>\r\n<!-- Tests VC: Standalone Document Declaration with external enitity reference and standalone is no -->\r\n",
  39,
  "<animal>This is a yellow tiger</animal>",
  "Tests VC: Standalone Document Declaration with external entity reference and standalone is no"
},
{
  "valid/P32/ibm32v03.xml",
  214,
  "<?xml version=\"1.0\" standalone=\"no\" ?>\r\n<!DOCTYPE animal SYSTEM \"ibm32v03.dtd\">\r\n<animal/>\r\n<!-- Tests VC: Standalone Document Declaration with attribute values that need to be normalized and standalone is no -->\r\n",
  27,
  "<animal class=\")\"></animal>",
  "Tests VC: Standalone Document Declaration with attribute values that need to be normalized and standalone is no"
},
{
  "valid/P32/ibm32v04.xml",
  225,
  "<?xml version=\"1.0\" standalone=\"no\" ?>\r\n<!DOCTYPE animal SYSTEM \"ibm32v04.dtd\">\r\n<animal>This is a \r\n      <a/>  \r\n\r\nyellow tiger</animal>\r\n<!-- Tests VC: Standalone Document Declaration with whitespace in mixed content -->\r\n",
  90,
  "<animal xml:space=\"preserve\">This is a &#10;      <a></a>  &#10;&#10;yellow tiger</animal>",
  "Tests VC: Standalone Document Declaration with whitespace in mixed content and standalone is no"
},
{
  "valid/P33/ibm33v01.xml",
  196,
  "<!DOCTYPE book [\r\n   <!ELEMENT book ANY>\r\n   <!ATTLIST book xml:lang CDATA #REQUIRED>\r\n]>\r\n<book xml:lang=\"en-US\">It is written in English</book>\r\n<!-- Tests LanguageID with Langcode - Subcode -->",
  54,
  "<book xml:lang=\"en-US\">It is written in English</book>",
  "Tests LanguageID with Langcode - Subcode"
},
{
  "valid/P34/ibm34v01.xml",
  147,
  "<!DOCTYPE book [\r\n   <!ELEMENT book ANY>\r\n   <!ATTLIST book xml:lang CDATA #REQUIRED>\r\n]>\r\n<book xml:lang=\"en-US\">It is written in English</book>\r\n",
  54,
  "<book xml:lang=\"en-US\">It is written in English</book>",
  "Duplicate Test as ibm33v01.xml"
},
{
  "valid/P35/ibm35v01.xml",
  144,
  "<!DOCTYPE book [\r\n   <!ELEMENT book ANY>\r\n   <!ATTLIST book xml:lang CDATA #REQUIRED>\r\n]>\r\n<book xml:lang=\"en\">It is written in English</book>\r\n",
  51,
  "<book xml:lang=\"en\">It is written in English</book>",
  "Tests ISO639Code"
},
{
  "valid/P36/ibm36v01.xml",
  151,
  "<!DOCTYPE book [\r\n   <!ELEMENT book ANY>\r\n   <!ATTLIST book xml:lang CDATA #REQUIRED>\r\n]>\r\n<book xml:lang=\"i-BS-ABCD\">It is written in English</book>\r\n",
  58,
  "<book xml:lang=\"i-BS-ABCD\">It is written in English</book>",
  "Tests IanaCode"
},
{
  "valid/P37/ibm37v01.xml",
  150,
  "<!DOCTYPE book [\r\n   <!ELEMENT book ANY>\r\n   <!ATTLIST book xml:lang CDATA #REQUIRED>\r\n]>\r\n<book xml:lang=\"x-uk-eng\">It is written in English</book>\r\n",
  57,
  "<book xml:lang=\"x-uk-eng\">It is written in English</book>",
  "Tests UserCode"
},
{
  "valid/P38/ibm38v01.xml",
  148,
  "<!DOCTYPE book [\r\n   <!ELEMENT book ANY>\r\n   <!ATTLIST book xml:lang CDATA #REQUIRED>\r\n]>\r\n<book xml:lang=\"en-USa\">It is written in English</book>\r\n",
  55,
  "<book xml:lang=\"en-USa\">It is written in English</book>",
  "Tests SubCode"
},
{
  "valid/P39/ibm39v01.xml",
  399,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n  <!ELEMENT root (a,b)>\r\n  <!ELEMENT a EMPTY>\r\n  <!ELEMENT b (#PCDATA|c)* >\r\n  <!ELEMENT c ANY>\r\n  <!ELEMENT d ((e,e)|f)+ >\r\n  <!ELEMENT e ANY>\r\n  <!ELEMENT f EMPTY>\r\n]>\r\n<root><a/><b>\r\n   <c></c> \r\n   content of b element\r\n   <c>\r\n      <d><e>no more children</e><e><f/></e><f/></d>\r\n   </c>\r\n</b></root>\r\n<!--* test P39's syntax and Element Valid VC *-->\r\n",
  161,
  "<root><a></a><b>&#10;   <c></c> &#10;   content of b element&#10;   <c>&#10;      <d><e>no more children</e><e><f></f></e><f></f></d>&#10;   </c>&#10;</b></root>",
  "Tests element with EmptyElemTag and STag content Etag, also tests the VC: Element Valid with elements that have children, Mixed and ANY contents"
},
{
  "valid/P40/ibm40v01.xml",
  428,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n  <!ELEMENT root (#PCDATA|b)* >\r\n  <!ELEMENT b (#PCDATA) >\r\n  <!ATTLIST b attr1 CDATA #IMPLIED>\r\n  <!ATTLIST b attr2 CDATA #IMPLIED>\r\n  <!ATTLIST b attr3 CDATA #IMPLIED>\r\n]>\r\n<root>\r\n  <b>without white space</b>\r\n  <b > with a white space</b>\r\n  <b attr1=\"value1\">one attribute</b>\r\n  <b attr1=\"value1\" attr2=\"value2\" attr3 = \"value3\">one attribute</b>\r\n</root>\r\n<!--* testing P40 *-->\r\n",
  198,
  "<root>&#10;  <b>without white space</b>&#10;  <b> with a white space</b>&#10;  <b attr1=\"value1\">one attribute</b>&#10;  <b attr1=\"value1\" attr2=\"value2\" attr3=\"value3\">one attribute</b>&#10;</root>",
  "Tests STag with possible combinations of its fields, also tests WFC: Unique Att Spec."
},
{
  "valid/P41/ibm41v01.xml",
  333,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n  <!ELEMENT root (#PCDATA|b)* >\r\n  <!ELEMENT b (#PCDATA) >\r\n  <!ATTLIST b attr1 CDATA #REQUIRED>\r\n  <!ATTLIST b attr2 (abc|def) \"abc\">\r\n  <!ATTLIST b attr3 CDATA #FIXED \"fixed\">\r\n]>\r\n<root>\r\n  <b attr1=\"value1\" attr2=\"def\" attr3=\"fixed\">Name eq AttValue</b>\r\n</root>\r\n<!--* testing P41 *-->\r\n",
  89,
  "<root>&#10;  <b attr1=\"value1\" attr2=\"def\" attr3=\"fixed\">Name eq AttValue</b>&#10;</root>",
  "Tests Attribute with Name Eq AttValue and VC: Attribute Value Type"
},
{
  "valid/P42/ibm42v01.xml",
  261,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n  <!ELEMENT root (a,b)>\r\n  <!ELEMENT a EMPTY>\r\n  <!ELEMENT b (#PCDATA|c)* >\r\n  <!ELEMENT c ANY>\r\n]>\r\n<root><a/><b>\r\n   <c></c > : End tag with a space inside\r\n   content of b element\r\n</b></root>\r\n<!--* test P42 *-->\r\n\r\n",
  105,
  "<root><a></a><b>&#10;   <c></c> : End tag with a space inside&#10;   content of b element&#10;</b></root>",
  "Tests ETag with possible combinations of its fields"
},
{
  "valid/P43/ibm43v01.xml",
  746,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n  <!ELEMENT root (a,b)>\r\n  <!ELEMENT a EMPTY>\r\n  <!ELEMENT b (#PCDATA|c)* >\r\n  <!ELEMENT c ANY>\r\n  <!ENTITY inContent \"<b>General entity reference in element content</b>\">\r\n]>\r\n<!--* content: element|CharData|Reference|CDSect|PI|CDSect|PI|Comment *-->\r\n<root><a/><b>\r\n<!-- there is an empty element in the above line -->\r\n   <c></c> \r\n   CharData: content of b element\r\n   %paaa; : PE reference should not be recognized in element content \r\n   <c>\r\n<?PIcontent anyProcessingInstruction?>\r\n<!-- Comment content -->\r\n    &inContent;\r\n    Charater reference: &#x41;\r\n    CDSect in content: <![CDATA[ <html>markups<head>HEAD</head><body>nothing</body></html> ]]>\r\n   </c>\r\n</b>\r\n</root>\r\n<!--* test P43 *-->\r\n",
  453,
  "<root><a></a><b>&#10;&#10;   <c></c> &#10;   CharData: content of b element&#10;   %paaa; : PE reference should not be recognized in element content &#10;   <c>&#10;<?PIcontent anyProcessingInstruction?>&#10;&#10;    <b>General entity reference in element content</b>&#10;    Charater reference: A&#10;    CDSect in content:  &lt;html&gt;markups&lt;head&gt;HEAD&lt;/head&gt;&lt;body&gt;nothing&lt;/body&gt;&lt;/html&gt; &#10;   </c>&#10;</b>&#10;</root>",
  "Tests content with all possible constructs: element, CharData, Reference, CDSect, Comment"
},
{
  "valid/P44/ibm44v01.xml",
  393,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n  <!ELEMENT root (#PCDATA|b)* >\r\n  <!ELEMENT b EMPTY >\r\n  <!ATTLIST b attr1 CDATA #IMPLIED>\r\n  <!ATTLIST b attr2 CDATA #IMPLIED>\r\n  <!ATTLIST b attr3 CDATA #IMPLIED>\r\n]>\r\n<root>\r\n  <b/>without white space\r\n  <b /> with a white space\r\n  <b attr1=\"value1\" />\r\n  <b attr1=\"value1\" attr2=\"value2\" attr3 = \"value3\"/>\r\n</root>\r\n<!--* testing P44 *-->\r\n\r\n\r\n\r\n",
  172,
  "<root>&#10;  <b></b>without white space&#10;  <b></b> with a white space&#10;  <b attr1=\"value1\"></b>&#10;  <b attr1=\"value1\" attr2=\"value2\" attr3=\"value3\"></b>&#10;</root>",
  "Tests EmptyElemTag with possible combinations of its fields"
},
{
  "valid/P45/ibm45v01.xml",
  558,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n  <!ELEMENT root (#PCDATA|b)* >\r\n  <!--* P45 no space before the end bracket *-->\r\n  <!ELEMENT b EMPTY>\r\n  <!ELEMENT unique ANY>\r\n  <!ELEMENT unique- ANY>\r\n  <!ELEMENT unique_ ANY>\r\n  <!ELEMENT unique. ANY>\r\n  <!ATTLIST b attr1 CDATA #IMPLIED>\r\n  <!ATTLIST b attr2 CDATA #IMPLIED>\r\n  <!ATTLIST b attr3 CDATA #IMPLIED>\r\n]>\r\n<root>\r\n  <b/>without white space\r\n  <b /> with a white space\r\n  <b attr1=\"value1\" />\r\n  <b attr1=\"value1\" attr2=\"value2\" attr3 = \"value3\"/>\r\n</root>\r\n<!--* !!! testing both P45 and p46 *-->\r\n\r\n",
  172,
  "<root>&#10;  <b></b>without white space&#10;  <b></b> with a white space&#10;  <b attr1=\"value1\"></b>&#10;  <b attr1=\"value1\" attr2=\"value2\" attr3=\"value3\"></b>&#10;</root>",
  "Tests both P45 elementDecl and P46 contentspec with possible combinations of their constructs"
},
{
  "valid/P47/ibm47v01.xml",
  827,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n  <!ELEMENT root (a,b)>\r\n  <!ELEMENT a EMPTY>\r\n  <!ELEMENT b (#PCDATA|c)* >\r\n  <!ELEMENT c ANY>\r\n  <!ELEMENT d ANY>\r\n  <!ELEMENT e ANY>\r\n  <!ELEMENT f ANY>\r\n  <!--* test all possible children,cp,choice,seq patterns in P47,P48,P49,P50 *-->\r\n  <!ELEMENT child0 (a)>\r\n  <!ELEMENT child1 (a|b|c)>\r\n  <!ELEMENT child2 (a ,b,b?,a*,c,c,a,a,b+,c ) >\r\n  <!ELEMENT child3 (a+|b)? >\r\n  <!ELEMENT child4 (a, (b|c)+, (a|d)?, (e|f)* )?>\r\n  <!ELEMENT child5 ( (a,b) | c? | ((d|e),b,c) )* >\r\n  <!ELEMENT child5_1 ( (a,b)* | (c,b)? | (d,a)+ | ((e|f),b,c) )* >\r\n  <!ELEMENT child6 (a,b,c)*>\r\n  <!ELEMENT child7 ((a,b)|c*|((d|e),b,c) )+ >\r\n  <!ELEMENT child8 ( a, (b|c), (a|b), b)+>  \r\n]>\r\n<root><a/><b>\r\n   <c></c >\r\n   content of b element\r\n</b></root>\r\n<!--* a valid test: tests P47,P48,P49,P50*-->\r\n\r\n",
  75,
  "<root><a></a><b>&#10;   <c></c>&#10;   content of b element&#10;</b></root>",
  "Tests all possible children,cp,choice,seq patterns in P47,P48,P49,P50"
},
{
  "valid/P49/ibm49v01.xml",
  228,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root SYSTEM \"ibm49v01.dtd\"[\r\n  <!ELEMENT root (a,b)>\r\n]>\r\n<root><a/><b>\r\n   <c></c>\r\n   content of b element\r\n</b></root>\r\n<!--* a valid test: tests VC:Proper Group/PE Nesting in P49 *-->\r\n\r\n\r\n\r\n",
  75,
  "<root><a></a><b>&#10;   <c></c>&#10;   content of b element&#10;</b></root>",
  "Tests VC:Proper Group/PE Nesting with PEs of choices that are properly nested with parenthesized groups in external subsets"
},
{
  "valid/P50/ibm50v01.xml",
  307,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root SYSTEM \"ibm50v01.dtd\" [\r\n  <!ELEMENT root (a,b)>\r\n]>\r\n<root><a/><b>\r\n   <c><child1><a/><b></b><c></c></child1></c >\r\n   <c><child2><a/><b></b><c></c></child2></c >\r\n   content of b element\r\n</b></root>\r\n<!--* a valid test: tests VC:Proper Group/PE Nesting in P50 *-->\r\n",
  166,
  "<root><a></a><b>&#10;   <c><child1><a></a><b></b><c></c></child1></c>&#10;   <c><child2><a></a><b></b><c></c></child2></c>&#10;   content of b element&#10;</b></root>",
  "Tests VC:Proper Group/PE Nesting with PEs of seq that are properly nested with parenthesized groups in external subsets"
},
{
  "valid/P51/ibm51v01.xml",
  604,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n  <!ELEMENT root ANY>\r\n  <!--* test all possible Mixed content type decl *-->\r\n  <!ELEMENT a (#PCDATA)* >\r\n  <!ELEMENT b (#PCDATA) >\r\n  <!ELEMENT c ( #PCDATA)*>\r\n  <!ELEMENT d (#PCDATA|c)*>\r\n  <!ELEMENT e (#PCDATA|c| b|a)* >\r\n  <!ELEMENT f (#PCDATA| c)* >\r\n  <!ELEMENT g ( #PCDATA) >\r\n  <!ELEMENT h (#PCDATA )>\r\n  <!ELEMENT i ( #PCDATA ) >\r\n]>\r\n<root>\r\n  <a> Element type a </a>\r\n  <b> Element type b </b>\r\n  <c> Element type c </c>\r\n  <d> Element type d <c></c> </d>\r\n  <e> Element type e <a></a> <b></b> <c></c> </e>\r\n</root>\r\n<!--* a valid test: tests P51 *-->",
  200,
  "<root>&#10;  <a> Element type a </a>&#10;  <b> Element type b </b>&#10;  <c> Element type c </c>&#10;  <d> Element type d <c></c> </d>&#10;  <e> Element type e <a></a> <b></b> <c></c> </e>&#10;</root>",
  "Tests Mixed with possible combinations of its fields amd VC: No Duplicate Types"
},
{
  "valid/P51/ibm51v02.xml",
  336,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root SYSTEM \"ibm51v02.dtd\" [\r\n  <!ELEMENT root ANY>\r\n]>\r\n<root>\r\n  <a> Element type a </a>\r\n  <b> Element type b </b>\r\n  <c> Element type c </c>\r\n  <d> Element type d <c></c> </d>\r\n  <e> Element type e <a></a> <b></b> <c></c> </e>\r\n</root>\r\n<!--* a valid test: tests P51 VC: Proper Group/PE Nesting *-->",
  200,
  "<root>&#10;  <a> Element type a </a>&#10;  <b> Element type b </b>&#10;  <c> Element type c </c>&#10;  <d> Element type d <c></c> </d>&#10;  <e> Element type e <a></a> <b></b> <c></c> </e>&#10;</root>",
  "Tests VC:Proper Group/PE Nesting with PEs of Mixed that are properly nested with parenthesized groups in external subsets"
},
{
  "valid/P52/ibm52v01.xml",
  532,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n  <!ELEMENT root ANY>\r\n  <!ELEMENT a (#PCDATA)* >\r\n  <!ELEMENT b (#PCDATA) >\r\n  <!--* All AttlistDecl Patterns in P52 and P53 *-->\r\n  <!ATTLIST a>\r\n  <!ATTLIST a >\r\n  <!ATTLIST b battr1 CDATA #REQUIRED >\r\n  <!ATTLIST b battr2 CDATA #IMPLIED \r\n              battr3 CDATA #FIXED \"fixedvalue\" battr4 (abc|def) \"abc\" >\r\n]>\r\n<root>\r\n  <a> Element type a </a>\r\n  <b battr1 = \"anyvalue\" battr3=\"fixedvalue\" battr4 =\"def\"> test P52 and P53 </b>\r\n</root>\r\n<!--* a valid test: tests P52 and P53 *-->\r\n",
  131,
  "<root>&#10;  <a> Element type a </a>&#10;  <b battr1=\"anyvalue\" battr3=\"fixedvalue\" battr4=\"def\"> test P52 and P53 </b>&#10;</root>",
  "Tests all AttlistDecl and AttDef Patterns in P52 and P53"
},
{
  "valid/P54/ibm54v01.xml",
  1857,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n  <!ELEMENT root ANY>\r\n  <!ELEMENT a (#PCDATA) >\r\n  <!ELEMENT b (#PCDATA) >\r\n  <!ELEMENT c (#PCDATA) >\r\n  <!ELEMENT d (#PCDATA) >\r\n  <!ELEMENT e (#PCDATA) >\r\n  <!ELEMENT f (#PCDATA) >\r\n  <!ELEMENT g (#PCDATA) >\r\n  <!ELEMENT h (#PCDATA) >\r\n  <!ELEMENT i (#PCDATA) >\r\n  <!ELEMENT j (#PCDATA) >\r\n  <!ELEMENT k (#PCDATA) >\r\n  <!--* Tests all Attbuite types in P55,P56,P57,P58,P59 *-->\r\n  <!ATTLIST a aattr1 ID #REQUIRED>\r\n  <!ATTLIST b battr1 CDATA #REQUIRED \r\n              battr2 (good|bad) #REQUIRED \r\n              battr3 ID #REQUIRED>\r\n  <!ATTLIST c c_reference IDREF #REQUIRED>\r\n  <!ATTLIST d d_reference IDREFS #REQUIRED>\r\n  <!ENTITY xmltech SYSTEM \"xmltech.gif\" NDATA gif>\r\n  <!NOTATION gif SYSTEM \"gif\">\r\n  <!ATTLIST e eattr1 ENTITY #REQUIRED>\r\n  <!ENTITY IBMlogo SYSTEM \"IBMlogo.gif\" NDATA gif>\r\n  <!ATTLIST f fattr1 ENTITIES #REQUIRED>\r\n  <!ATTLIST g gattr1 NMTOKEN #REQUIRED>\r\n  <!ATTLIST h hattr1 NMTOKENS #REQUIRED>\r\n  <!NOTATION UTF-8 SYSTEM \"UTF-8\">\r\n  <!ATTLIST i iattr1 NOTATION (UTF-8) #REQUIRED>\r\n  <!--* Tests all DefaultDecl in P60 -->\r\n  <!ATTLIST j jattr1 CDATA #REQUIRED \r\n              jattr2 CDATA \"good\" \r\n              jattr3 CDATA #FIXED \"fixed\"\r\n\t      jattr4 CDATA #IMPLIED >\r\n\r\n]>\r\n<root>\r\n  <a aattr1 = \"a1\"> Element type a </a>\r\n  <b battr1 = \"anyvalue\" battr2=\"good\" battr3 =\"b1\"> Element type b </b>\r\n  <c c_reference = \"b1\"> Element type c </c>\r\n  <d d_reference = \"a1 b1\"> Element type d </d>\r\n  <e eattr1 = \"xmltech\"> Element type e </e>\r\n  <f fattr1 = \"xmltech IBMlogo\"> Element type f </f>\r\n  <g gattr1 = \"xml4j3_0_0_EA3\"> Element type g </g>\r\n  <h hattr1 = \"xml4j3_0_0_EA3 Xerces-J_1_0_1\"> Element type h </h>\r\n  <i iattr1 = \"UTF-8\"> Element type i </i>\r\n  <j jattr1 = \"anyvalue\" jattr2=\"good\" jattr3 =\"fixed\"> Element type j </j>\r\n</root>\r\n<!--* a valid test: tests P54 *-->\r\n",
  0,
  NULL,
  "Tests all AttTypes : StringType, TokenizedTypes, EnumeratedTypes in P55,P56,P57,P58,P59. Also tests all DefaultDecls in P60."
},
{
  "valid/P54/ibm54v02.xml",
  368,
  "<?xml  version=\"1.0\"?>\r\n<!-- test for Production 54-->\r\n<!DOCTYPE root\r\n [\r\n <!ELEMENT root (x|y|z)*>\r\n <!ELEMENT x (#PCDATA)>\r\n <!ELEMENT y ANY>\r\n <!ELEMENT z EMPTY>\r\n <!ATTLIST x attr CDATA #IMPLIED>\r\n <!ATTLIST y attr NMTOKENS #IMPLIED>\r\n <!ATTLIST z attr (x|y) #IMPLIED>\r\n ]>\r\n<root>\r\n<x attr= 'Madhu'></x>\r\n<y attr= '1.a.name.token.but.not.a.name'></y>\r\n</root>\r\n",
  92,
  "<root>&#10;<x attr=\"Madhu\"></x>&#10;<y attr=\"1.a.name.token.but.not.a.name\"></y>&#10;</root>",
  "Tests all AttTypes : StringType, TokenizedType, EnumeratedTypes in P55,P56,P57."
},
{
  "valid/P54/ibm54v03.xml",
  220,
  "<?xml  version=\"1.0\"?>\r\n<!-- test for Production 54-->\r\n<!DOCTYPE AttrType\r\n[\r\n<!ELEMENT AttrType ANY>\r\n<!ELEMENT a (#PCDATA)>\r\n<!ATTLIST a att CDATA #IMPLIED>\r\n]>\r\n<AttrType>\r\n<a att= 'hello world'>\r\n</a>\r\n</AttrType>\r\n",
  61,
  "<AttrType>&#10;<a att=\"hello world\">&#10;</a>&#10;</AttrType>",
  "Tests AttTypes with StringType in P55."
},
{
  "valid/P55/ibm55v01.xml",
  254,
  "<?xml  version=\"1.0\"?>\r\n<!-- test valid syntax for Production 55-->\r\n<!DOCTYPE StType\r\n[\r\n<!ELEMENT StType ANY>\r\n<!ELEMENT a EMPTY>\r\n<!ATTLIST a att CDATA #IMPLIED> \r\n]>\r\n<StType>\r\n<a att='Hello'/>\r\nTesting with a valid stringType attribute \r\n</StType>\r\n",
  93,
  "<StType>&#10;<a att=\"Hello\"></a>&#10;Testing with a valid stringType attribute &#10;</StType>",
  "Tests StringType for P55. The \"CDATA\" occurs in the StringType for the attribute \"att\" for the element \"a\"."
},
{
  "valid/P56/ibm56v01.xml",
  530,
  "<?xml  version=\"1.0\"?>\r\n<!-- valid test for Production 56-->\r\n<!DOCTYPE root\r\n [\r\n <!ELEMENT root (a|b|c|d|e|f|g)*>\r\n <!ELEMENT a EMPTY>\r\n <!ELEMENT b EMPTY>\r\n <!ELEMENT c EMPTY>\r\n <!ELEMENT d EMPTY>\r\n <!ELEMENT e EMPTY>\r\n <!ELEMENT f EMPTY>\r\n <!ELEMENT g EMPTY>\r\n <!ATTLIST a attr ID #IMPLIED>\r\n <!ATTLIST b attr IDREF #IMPLIED>\r\n <!ATTLIST c attr IDREFS #IMPLIED>\r\n <!ATTLIST d attr ENTITY #IMPLIED>\r\n <!ATTLIST e attr ENTITIES #IMPLIED>\r\n <!ATTLIST f attr NMTOKEN #IMPLIED>\r\n <!ATTLIST g attr NMTOKENS #IMPLIED>\r\n ]>\r\n<root/>\r\n",
  13,
  "<root></root>",
  "Tests TokenizedType for P56. The \"ID\", \"IDREF\", \"IDREFS\", \"ENTITY\", \"ENTITIES\", \"NMTOKEN\", and \"NMTOKENS\" occur in the TokenizedType for the attribute \"attr\"."
},
{
  "valid/P56/ibm56v02.xml",
  374,
  "<?xml  version=\"1.0\"?>\r\n<!-- validity constraint check for Production 56(Positive Test)-->\r\n<!DOCTYPE tokenizer\r\n [\r\n <!ELEMENT tokenizer ANY>\r\n <!ATTLIST tokenizer UniqueName ID #REQUIRED>\r\n ]>\r\n<tokenizer UniqueName = \"AC1999\">\r\nThis is a positive test for validity constraints\r\nGiving a unique name to the attribute ID an ID Attribute default as #required\r\n</tokenizer>\r\n",
  183,
  "<tokenizer UniqueName=\"AC1999\">&#10;This is a positive test for validity constraints&#10;Giving a unique name to the attribute ID an ID Attribute default as #required&#10;</tokenizer>",
  "Tests TokenizedType for P56 VC: ID Attribute Default. The value \"AC1999\" is assigned to the ID attribute \"attr\" with \"#REQUIRED\" in the DeaultDecl."
},
{
  "valid/P56/ibm56v03.xml",
  333,
  "<?xml  version=\"1.0\"?>\r\n<!-- validity constraint check for Production 56(Positive Test)-->\r\n<!DOCTYPE tokenizer\r\n [\r\n <!ELEMENT tokenizer ANY>\r\n <!ATTLIST tokenizer UniqueName ID #IMPLIED>\r\n ]>\r\n<tokenizer UniqueName = \"AC1999\">\r\nThis is a positive test for validity constraints\r\nGiving ID attribute default as #IMPLIED\r\n</tokenizer>",
  145,
  "<tokenizer UniqueName=\"AC1999\">&#10;This is a positive test for validity constraints&#10;Giving ID attribute default as #IMPLIED&#10;</tokenizer>",
  "Tests TokenizedType for P56 VC: ID Attribute Default. The value \"AC1999\" is assigned to the ID attribute \"attr\" with \"#IMPLIED\" in the DeaultDecl."
},
{
  "valid/P56/ibm56v04.xml",
  467,
  "<?xml  version=\"1.0\"?>\r\n<!-- validity constraint check for Production 56(positive Test)-->\r\n<!DOCTYPE tokenizer\r\n [\r\n <!ELEMENT tokenizer ANY>\r\n <!ELEMENT b EMPTY>\r\n <!ATTLIST b attr ID #REQUIRED>\r\n <!ATTLIST tokenizer UniqueName ID #REQUIRED>\r\n  ]>\r\n<tokenizer UniqueName = \"Ac999\">\r\n<b attr = \"BC999\"></b>\r\nThis is a positive test for validity constraints\r\nthe value of the attribute with a type ID does not appear more than once in the XML document\r\n</tokenizer>\r\n",
  222,
  "<tokenizer UniqueName=\"Ac999\">&#10;<b attr=\"BC999\"></b>&#10;This is a positive test for validity constraints&#10;the value of the attribute with a type ID does not appear more than once in the XML document&#10;</tokenizer>",
  "Tests TokenizedType for P56 VC: ID. The ID attribute \"UniqueName\" appears only once in the document."
},
{
  "valid/P56/ibm56v05.xml",
  449,
  "<?xml  version=\"1.0\"?>\r\n<!-- validity constraint check for Production 56(positive Test)-->\r\n<!DOCTYPE tokenizer\r\n [\r\n <!ELEMENT tokenizer ANY>\r\n <!ELEMENT a EMPTY>\r\n <!ELEMENT b EMPTY>\r\n <!ATTLIST a first ID #REQUIRED>\r\n <!ATTLIST b second ID #REQUIRED>\r\n ]>\r\n<tokenizer>\r\n<a first = \"AC1999\"></a>\r\n<b second = \"CD345\"></b>\r\nThis is a positive validity test for ID.\r\nany element type has no more than one attribute of type ID specified\r\n</tokenizer>",
  200,
  "<tokenizer>&#10;<a first=\"AC1999\"></a>&#10;<b second=\"CD345\"></b>&#10;This is a positive validity test for ID.&#10;any element type has no more than one attribute of type ID specified&#10;</tokenizer>",
  "Tests TokenizedType for P56 VC: One ID per element type. The element \"a\" or \"b\" has only one ID attribute."
},
{
  "valid/P56/ibm56v06.xml",
  572,
  "<?xml  version=\"1.0\"?>\r\n<!-- validity constraint check for Production 56(Positive Test)-->\r\n<!DOCTYPE test\r\n [\r\n <!ELEMENT test ANY>\r\n <!ELEMENT id EMPTY>\r\n <!ELEMENT idref EMPTY>\r\n <!ATTLIST id UniqueName ID #REQUIRED>\r\n <!ATTLIST idref reference IDREF #IMPLIED>\r\n ]>\r\n<test>\r\n<id UniqueName = \"AC456\"></id>\r\n<idref reference = \"AC456\"></idref>\r\nPositive test for validity constraint of IDREF.\r\nIn an attribute decl, values of type IDREF match tha name production\r\nand the IDREF value matches the value assigned to an ID attribute somewhere\r\nin the XML document.\r\n</test>",
  319,
  "<test>&#10;<id UniqueName=\"AC456\"></id>&#10;<idref reference=\"AC456\"></idref>&#10;Positive test for validity constraint of IDREF.&#10;In an attribute decl, values of type IDREF match tha name production&#10;and the IDREF value matches the value assigned to an ID attribute somewhere&#10;in the XML document.&#10;</test>",
  "Tests TokenizedType for P56 VC: IDREF. The IDREF value \"AC456\" matches the value assigned to an ID attribute \"UniqueName\"."
},
{
  "valid/P56/ibm56v07.xml",
  673,
  "<?xml  version=\"1.0\"?>\r\n<!-- validity constraint check for Production 56(Positive Test)-->\r\n<!DOCTYPE test\r\n [\r\n <!ELEMENT test ANY>\r\n <!ELEMENT id1 EMPTY>\r\n <!ELEMENT id2 EMPTY>\r\n <!ELEMENT idref EMPTY>\r\n <!ATTLIST id1 UniqueName ID #REQUIRED>\r\n <!ATTLIST id2 UName ID #IMPLIED>\r\n <!ATTLIST idref reference IDREFS #IMPLIED>\r\n ]>\r\n<test>\r\n<id1 UniqueName = \"AC456\"></id1>\r\n<id2 UName = \"Q123\"></id2>\r\n<idref reference = \"AC456 Q123\"></idref>\r\nPositive test for validity constraint of IDREFS.\r\nIn an attribute decl, values of type IDREFS match tha name production\r\nand the IDREFS value matches the values assigned to an ID attributes somewhere\r\nin the XML document.\r\n</test>",
  360,
  "<test>&#10;<id1 UniqueName=\"AC456\"></id1>&#10;<id2 UName=\"Q123\"></id2>&#10;<idref reference=\"AC456 Q123\"></idref>&#10;Positive test for validity constraint of IDREFS.&#10;In an attribute decl, values of type IDREFS match tha name production&#10;and the IDREFS value matches the values assigned to an ID attributes somewhere&#10;in the XML document.&#10;</test>",
  "Tests TokenizedType for P56 VC: IDREF. The IDREFS value \"AC456 Q123\" matches the values assigned to the ID attribute \"UniqueName\" and \"Uname\"."
},
{
  "valid/P56/ibm56v08.xml",
  503,
  "<?xml  version=\"1.0\"?>\r\n<!-- validity constraint check for Production 56(Positive Test)it is a DTD-->\r\n<!DOCTYPE test\r\n [\r\n <!ELEMENT test ANY>\r\n <!ELEMENT landscape EMPTY>\r\n <!NOTATION gif PUBLIC \"gif\">\r\n <!ENTITY image SYSTEM \"testspec/images/sunset.gif\" NDATA gif>\r\n <!ATTLIST landscape sun ENTITY #IMPLIED>\r\n]>\r\n<test>\r\n<landscape sun = \"image\"></landscape>\r\nvalues of type ENTITY match the Name production and the ENTITY value\r\nmatches the name of an unparsed entity declared in the DTD.\r\n</test>\r\n",
  244,
  "<!DOCTYPE test [\n<!NOTATION gif PUBLIC 'gif'>\n]>\n<test>&#10;<landscape sun=\"image\"></landscape>&#10;values of type ENTITY match the Name production and the ENTITY value&#10;matches the name of an unparsed entity declared in the DTD.&#10;</test>",
  "Tests TokenizedType for P56 VC: Entity Name. The value \"image\" of the ENTITY attribute \"sun\" matches the name of an unparsed entity declared."
},
{
  "valid/P56/ibm56v09.xml",
  361,
  "<?xml  version=\"1.0\"?>\r\n<!-- validity constraint check for Production 56(Positive Test)-->\r\n<!DOCTYPE test\r\n [\r\n <!ELEMENT test ANY>\r\n <!ELEMENT nametoken EMPTY>\r\n <!ATTLIST nametoken thistoken NMTOKEN #IMPLIED>\r\n]>\r\n<test>\r\n<nametoken thistoken = \"x:image\"></nametoken>\r\nIn an attribute declaration, values of type NMTOKEN match the Nmtoken production\r\n</test>",
  151,
  "<test>&#10;<nametoken thistoken=\"x:image\"></nametoken>&#10;In an attribute declaration, values of type NMTOKEN match the Nmtoken production&#10;</test>",
  "Tests TokenizedType for P56 VC: Name Token. The value of the NMTOKEN attribute \"thistoken\" matches the Nmtoken production."
},
{
  "valid/P56/ibm56v10.xml",
  377,
  "<?xml  version=\"1.0\"?>\r\n<!-- validity constraint check for Production 56(Positive Test)-->\r\n<!DOCTYPE test\r\n [\r\n <!ELEMENT test ANY>\r\n <!ELEMENT nametokens EMPTY>\r\n <!ATTLIST nametokens thistoken NMTOKENS #IMPLIED>\r\n]>\r\n<test>\r\n<nametokens thistoken = \"x:lang y:country\"></nametokens>\r\nIn an attribute declaration, values of type NMTOKENS match the Nmtokens production\r\n</test>",
  164,
  "<test>&#10;<nametokens thistoken=\"x:lang y:country\"></nametokens>&#10;In an attribute declaration, values of type NMTOKENS match the Nmtokens production&#10;</test>",
  "Tests TokenizedType for P56 VC: Name Token. The value of the NMTOKENS attribute \"thistoken\" matches the Nmtoken production."
},
{
  "valid/P57/ibm57v01.xml",
  370,
  "<?xml  version=\"1.0\"?>\r\n<!-- valid test for Production 57-->\r\n<!DOCTYPE root\r\n [\r\n <!ELEMENT root (#PCDATA|a|b)*>\r\n <!ELEMENT a ANY>\r\n <!ELEMENT b ANY>\r\n <!NOTATION a SYSTEM \"a\">\r\n <!NOTATION b SYSTEM \"b\"> \r\n <!ATTLIST a att (a|b) #IMPLIED>\r\n <!ATTLIST b att NOTATION (a|b) #IMPLIED>\r\n ]>\r\n <root>\r\nThis test case tests the kinds of enumerated types\r\n<a/><b/>\r\n</root>\r\n",
  162,
  "<!DOCTYPE root [\n<!NOTATION a SYSTEM 'a'>\n<!NOTATION b SYSTEM 'b'>\n]>\n<root>&#10;This test case tests the kinds of enumerated types&#10;<a></a><b></b>&#10;</root>",
  "Tests EnumeratedType in the AttType. The attribute \"att\" has a type (a|b) with the element \"a\". the"
},
{
  "valid/P58/ibm58v01.xml",
  618,
  "<?xml  version=\"1.0\"?>\r\n<!-- valid test for Production 58-->\r\n<!DOCTYPE test\r\n [\r\n <!ELEMENT test ANY>\r\n <!ELEMENT one ANY>\r\n <!ELEMENT two ANY>\r\n <!ELEMENT three ANY>\r\n <!ELEMENT four ANY>\r\n <!ELEMENT five ANY>\r\n <!NOTATION this SYSTEM \"alpha\">\r\n <!NOTATION that SYSTEM \"beta\">\r\n <!ATTLIST one attr NOTATION (this) #IMPLIED>\r\n <!ATTLIST two attr NOTATION ( this) #IMPLIED>\r\n <!ATTLIST three attr NOTATION (this|that) #IMPLIED>\r\n <!ATTLIST four attr NOTATION (that |this) #IMPLIED>\r\n <!ATTLIST five attr NOTATION ( that ) #IMPLIED>\r\n ]>\r\n <test>\r\nThis is a positive test with different patterns for NOTATION\r\n</test>\r\n",
  166,
  "<!DOCTYPE test [\n<!NOTATION that SYSTEM 'beta'>\n<!NOTATION this SYSTEM 'alpha'>\n]>\n<test>&#10;This is a positive test with different patterns for NOTATION&#10;</test>",
  "Tests NotationType for P58. It shows different patterns fro the NOTATION attribute \"attr\"."
},
{
  "valid/P58/ibm58v02.xml",
  548,
  "<?xml  version=\"1.0\"?>\r\n<!-- validity test for Production 58-->\r\n<!DOCTYPE test\r\n [\r\n <!ELEMENT test ANY>\r\n <!ELEMENT blob (#PCDATA)>\r\n <!NOTATION base64 SYSTEM \"mimecode\">\r\n <!NOTATION uuencode SYSTEM \"uudecode\">\r\n <!NOTATION raw SYSTEM \"bin/cat\">\r\n <!ATTLIST blob content-encoding NOTATION (base64|uuencode|raw) #REQUIRED>\r\n ]>\r\n <test>\r\n<blob content-encoding=\"base64\"></blob>\r\nThe attribute values of type NOTATION matches one of the notation names included in the declaration;\r\nall notation names in the declaration have been declared\r\n</test>",
  357,
  "<!DOCTYPE test [\n<!NOTATION base64 SYSTEM 'mimecode'>\n<!NOTATION raw SYSTEM 'bin/cat'>\n<!NOTATION uuencode SYSTEM 'uudecode'>\n]>\n<test>&#10;<blob content-encoding=\"base64\"></blob>&#10;The attribute values of type NOTATION matches one of the notation names included in the declaration;&#10;all notation names in the declaration have been declared&#10;</test>",
  "Tests NotationType for P58: Notation Attributes. The value \"base64\" of the NOTATION attribute \"attr\" matches one of the notation names declared."
},
{
  "valid/P59/ibm59v01.xml",
  476,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 59-->\r\n<!DOCTYPE test\r\n [\r\n <!ELEMENT test ANY>\r\n <!ELEMENT one EMPTY>\r\n <!ELEMENT two EMPTY>\r\n <!ELEMENT enum (#PCDATA)>\r\n <!ATTLIST one attr (one) #IMPLIED>\r\n <!ATTLIST two attr ( enum) #IMPLIED>\r\n <!ATTLIST two attr (one|two) #IMPLIED>\r\n <!ATTLIST two attr (one| two) #IMPLIED>\r\n <!ATTLIST two attr (enum ) #IMPLIED>\r\n <!ATTLIST two attr ( one | two | enum) #IMPLIED>\r\n ]>\r\n <test>\r\nThis is a Positive test\r\n</test>\r\n",
  46,
  "<test>&#10;This is a Positive test&#10;</test>",
  "Tests Enumeration in the EnumeratedType for P59. It shows different patterns for the Enumeration attribute \"attr\"."
},
{
  "valid/P59/ibm59v02.xml",
  385,
  "<?xml  version=\"1.0\"?>\r\n<!-- validity test for Production 59-->\r\n<!DOCTYPE test\r\n [\r\n <!ELEMENT test ANY>\r\n <!ELEMENT one EMPTY>\r\n <!ELEMENT two EMPTY>\r\n <!ELEMENT num EMPTY>\r\n <!ATTLIST num value (one|two) #IMPLIED>\r\n ]>\r\n <test>\r\n<num value = \"one\"></num>\r\nThis is a Positive test\r\nThe attribute values of type Enumeration match one of the Nmtoken tokens in the declaration.\r\n</test>",
  171,
  "<test>&#10;<num value=\"one\"></num>&#10;This is a Positive test&#10;The attribute values of type Enumeration match one of the Nmtoken tokens in the declaration.&#10;</test>",
  "Tests Enumeration for P59 VC: Enumeration. The value \"one\" of the Enumeration attribute \"attr\" matches one of the element names declared."
},
{
  "valid/P60/ibm60v01.xml",
  567,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 60-->\r\n<!DOCTYPE Java \r\n [\r\n <!ELEMENT Java (#PCDATA|one|two|three|four)*>\r\n <!ELEMENT one EMPTY>\r\n <!ELEMENT two EMPTY>\r\n <!ELEMENT three EMPTY>\r\n <!ELEMENT four EMPTY>\r\n <!ATTLIST one chapter CDATA #IMPLIED>\r\n <!ATTLIST two chapter CDATA #REQUIRED>\r\n <!ATTLIST three chapter CDATA #FIXED \"JavaBeans\">\r\n <!ATTLIST four chapter CDATA 'defualt'>\r\n ]>\r\n<Java><one chapter=\"Introduction\"/>\r\n <three chapter=\"JavaBeans\"/>\r\n Positive test\r\n DefaultDecl attributes values IMPLIED, REQUIRED, FIXED and default\r\n</Java>",
  184,
  "<Java><one chapter=\"Introduction\"></one>&#10; <three chapter=\"JavaBeans\"></three>&#10; Positive test&#10; DefaultDecl attributes values IMPLIED, REQUIRED, FIXED and default&#10;</Java>",
  "Tests DefaultDecl for P60. It shows different options \"#REQUIRED\", \"#FIXED\", \"#IMPLIED\", and default for the attribute \"chapter\"."
},
{
  "valid/P60/ibm60v02.xml",
  477,
  "<?xml  version=\"1.0\"?>\r\n<!-- validity test for Production 60-->\r\n<!DOCTYPE Java \r\n [\r\n <!ELEMENT Java ANY>\r\n <!ELEMENT one EMPTY>\r\n <!ELEMENT two EMPTY>\r\n <!ATTLIST one chapter CDATA #REQUIRED>\r\n <!ATTLIST two chapter CDATA #REQUIRED>\r\n ]>\r\n<Java>\r\n<one chapter=\"Introduction\"></one>\r\n<two chapter=\"JavaApplets\"></two>\r\nPositive test. Required attribute. Every occurrence of an element with a \r\n#REQUIRED attribute default declaration gives the value of that attribute\r\n</Java>",
  251,
  "<Java>&#10;<one chapter=\"Introduction\"></one>&#10;<two chapter=\"JavaApplets\"></two>&#10;Positive test. Required attribute. Every occurrence of an element with a &#10;#REQUIRED attribute default declaration gives the value of that attribute&#10;</Java>",
  "Tests DefaultDecl for P60 VC: Required Attribute. In the element \"one\" and \"two\" the value of the #REQUIRED attribute \"chapter\" is given."
},
{
  "valid/P60/ibm60v03.xml",
  435,
  "<?xml  version=\"1.0\"?>\r\n<!-- validity test for Production 60-->\r\n<!DOCTYPE Java \r\n [\r\n <!ELEMENT Java ANY>\r\n <!ELEMENT one EMPTY>\r\n <!ATTLIST one chapter CDATA #FIXED \"Introduction\">\r\n  ]>\r\n<Java>\r\n<one chapter=\"Introduction\"></one>\r\nAn attribute has a default value declared with the #FIXED keyword, \r\nand an instances of that attribute is given a value which is exactly \r\nthe same as the default value in the declaration. \r\n</Java>\r\n",
  258,
  "<Java>&#10;<one chapter=\"Introduction\"></one>&#10;An attribute has a default value declared with the #FIXED keyword, &#10;and an instances of that attribute is given a value which is exactly &#10;the same as the default value in the declaration. &#10;</Java>",
  "Tests DefaultDecl for P60 VC: Fixed Attribute Default. The value of the #FIXED attribute \"chapter\" is exactly the same as the default value."
},
{
  "valid/P60/ibm60v04.xml",
  421,
  "<?xml  version=\"1.0\"?>\r\n<!-- validity test for Production 60-->\r\n<!DOCTYPE test\r\n [\r\n <!ELEMENT test ANY>\r\n <!ELEMENT a EMPTY>\r\n <!ELEMENT b EMPTY>\r\n <!ELEMENT attr EMPTY>\r\n <!ELEMENT nametoken EMPTY>\r\n <!ATTLIST attr value (a|b) \"a\"> \r\n <!ATTLIST nametoken namevalue NMTOKEN \"hello\">\r\n  ]>\r\n<test>\r\nThe default value specified for an attribute meets the \r\nlexical constraints of the declared attribute type.\r\n</test>\r\n\r\n",
  134,
  "<test>&#10;The default value specified for an attribute meets the &#10;lexical constraints of the declared attribute type.&#10;</test>",
  "Tests DefaultDecl for P60 VC: Attribute Default Legal. The default value specified for the attribute \"attr\" meets the lexical constraints of the declared attribute type."
},
{
  "valid/P61/ibm61v01.xml",
  137,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 61-->\r\n<!DOCTYPE animal SYSTEM \"ibm61v01.dtd\">\r\n<animal>\r\n <tiger/>\r\n</animal>\r\n ",
  43,
  "<animal>&#10; <tiger></tiger>&#10;</animal>",
  "Tests conditionalSect for P61. It takes the option \"invludeSect\" in the file ibm61v01.dtd."
},
{
  "valid/P61/ibm61v02.xml",
  151,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 61-->\r\n<!DOCTYPE animal SYSTEM \"ibm61v02.dtd\"\r\n[\r\n<!ELEMENT animal EMPTY>\r\n]>\r\n<animal/>\r\n\r\n \r\n",
  17,
  "<animal></animal>",
  "Tests conditionalSect for P61. It takes the option \"ignoreSect\" in the file ibm61v02.dtd."
},
{
  "valid/P62/ibm62v01.xml",
  205,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 62-->\r\n<!DOCTYPE animal SYSTEM \"ibm62v01.dtd\">\r\n<animal>\r\n <tiger/>\r\nPositive test. Test includeSect with pattern1 of p62.\r\nNormal Pattern\r\n</animal>",
  120,
  "<animal>&#10; <tiger></tiger>&#10;Positive test. Test includeSect with pattern1 of p62.&#10;Normal Pattern&#10;</animal>",
  "Tests includeSect for P62. The white space is not included before the key word \"INCLUDE\" in the beginning sequence."
},
{
  "valid/P62/ibm62v02.xml",
  222,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 62-->\r\n<!DOCTYPE animal SYSTEM \"ibm62v02.dtd\">\r\n<animal>\r\n <tiger/>\r\nPositive test. Test includeSect with pattern2 of p62.\r\nspace included before INCLUDE\r\n</animal>\r\n",
  135,
  "<animal>&#10; <tiger></tiger>&#10;Positive test. Test includeSect with pattern2 of p62.&#10;space included before INCLUDE&#10;</animal>",
  "Tests includeSect for P62. The white space is not included after the key word \"INCLUDE\" in the beginning sequence."
},
{
  "valid/P62/ibm62v03.xml",
  221,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 62-->\r\n<!DOCTYPE animal SYSTEM \"ibm62v03.dtd\">\r\n<animal>\r\n <tiger/>\r\nPositive test. Test includeSect with pattern3 of p62.\r\nspace included after INCLUDE\r\n</animal>\r\n",
  134,
  "<animal>&#10; <tiger></tiger>&#10;Positive test. Test includeSect with pattern3 of p62.&#10;space included after INCLUDE&#10;</animal>",
  "Tests includeSect for P62. The white space is included after the key word \"INCLUDE\" in the beginning sequence."
},
{
  "valid/P62/ibm62v04.xml",
  232,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 62-->\r\n<!DOCTYPE animal SYSTEM \"ibm62v04.dtd\">\r\n<animal>\r\n <tiger/>\r\nPositive test. Test includeSect with pattern4 of p62.\r\nspace included before and after INCLUDE\r\n</animal>\r\n",
  145,
  "<animal>&#10; <tiger></tiger>&#10;Positive test. Test includeSect with pattern4 of p62.&#10;space included before and after INCLUDE&#10;</animal>",
  "Tests includeSect for P62. The white space is included before the key word \"INCLUDE\" in the beginning sequence."
},
{
  "valid/P62/ibm62v05.xml",
  242,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 62-->\r\n<!DOCTYPE animal SYSTEM \"ibm62v05.dtd\"\r\n[\r\n<!ELEMENT animal ANY>\r\n<!ELEMENT tiger EMPTY>\r\n]>\r\n\r\n<animal>\r\n <tiger/>\r\nPositive test. Missing external subset declaration.\r\n</animal>",
  99,
  "<animal>&#10; <tiger></tiger>&#10;Positive test. Missing external subset declaration.&#10;</animal>",
  "Tests includeSect for P62. The extSubsetDecl is not included."
},
{
  "valid/P63/ibm63v01.xml",
  280,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 63-->\r\n<!DOCTYPE animal SYSTEM \"ibm63v01.dtd\"\r\n[\r\n<!ELEMENT animal ANY>\r\n<!ELEMENT tiger (#PCDATA)>\r\n<!ATTLIST animal a (tiger) #REQUIRED> \r\n]>\r\n<animal a = \"tiger\">\r\nPositive test. Test for IGNORE with pattern 1.\r\n</animal>",
  83,
  "<animal a=\"tiger\">&#10;Positive test. Test for IGNORE with pattern 1.&#10;</animal>",
  "Tests ignoreSect for P63. The white space is not included before the key word \"IGNORE\" in the beginning sequence."
},
{
  "valid/P63/ibm63v02.xml",
  280,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 63-->\r\n<!DOCTYPE animal SYSTEM \"ibm63v02.dtd\"\r\n[\r\n<!ELEMENT animal ANY>\r\n<!ELEMENT tiger (#PCDATA)>\r\n<!ATTLIST animal a (tiger) #REQUIRED> \r\n]>\r\n<animal a = \"tiger\">\r\nPositive test. Test for IGNORE with pattern 2.\r\n</animal>",
  83,
  "<animal a=\"tiger\">&#10;Positive test. Test for IGNORE with pattern 2.&#10;</animal>",
  "Tests ignoreSect for P63. The white space is not included after the key word \"IGNORE\" in the beginning sequence."
},
{
  "valid/P63/ibm63v03.xml",
  280,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 63-->\r\n<!DOCTYPE animal SYSTEM \"ibm63v03.dtd\"\r\n[\r\n<!ELEMENT animal ANY>\r\n<!ELEMENT tiger (#PCDATA)>\r\n<!ATTLIST animal a (tiger) #REQUIRED> \r\n]>\r\n<animal a = \"tiger\">\r\nPositive test. Test for IGNORE with pattern 3.\r\n</animal>",
  83,
  "<animal a=\"tiger\">&#10;Positive test. Test for IGNORE with pattern 3.&#10;</animal>",
  "Tests ignoreSect for P63. The white space is included after the key word \"IGNORE\" in the beginning sequence."
},
{
  "valid/P63/ibm63v04.xml",
  280,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 63-->\r\n<!DOCTYPE animal SYSTEM \"ibm63v04.dtd\"\r\n[\r\n<!ELEMENT animal ANY>\r\n<!ELEMENT tiger (#PCDATA)>\r\n<!ATTLIST animal a (tiger) #REQUIRED> \r\n]>\r\n<animal a = \"tiger\">\r\nPositive test. Test for IGNORE with pattern 4.\r\n</animal>",
  83,
  "<animal a=\"tiger\">&#10;Positive test. Test for IGNORE with pattern 4.&#10;</animal>",
  "Tests ignoreSect for P63. The ignireSectContents is included."
},
{
  "valid/P63/ibm63v05.xml",
  280,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 63-->\r\n<!DOCTYPE animal SYSTEM \"ibm63v05.dtd\"\r\n[\r\n<!ELEMENT animal ANY>\r\n<!ELEMENT tiger (#PCDATA)>\r\n<!ATTLIST animal a (tiger) #REQUIRED> \r\n]>\r\n<animal a = \"tiger\">\r\nPositive test. Test for IGNORE with pattern 5.\r\n</animal>",
  83,
  "<animal a=\"tiger\">&#10;Positive test. Test for IGNORE with pattern 5.&#10;</animal>",
  "Tests ignoreSect for P63. The white space is included before and after the key word \"IGNORE\" in the beginning sequence."
},
{
  "valid/P64/ibm64v01.xml",
  177,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 64-->\r\n<!DOCTYPE animal SYSTEM \"ibm64v01.dtd\"\r\n[\r\n<!ELEMENT animal ANY>\r\n]>\r\n<animal>\r\nPositive Test. Pattern1\r\n</animal>",
  50,
  "<animal>&#10;Positive Test. Pattern1&#10;</animal>",
  "Tests ignoreSectContents for P64. One \"ignore\" field is included."
},
{
  "valid/P64/ibm64v02.xml",
  177,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 64-->\r\n<!DOCTYPE animal SYSTEM \"ibm64v02.dtd\"\r\n[\r\n<!ELEMENT animal ANY>\r\n]>\r\n<animal>\r\nPositive Test. Pattern2\r\n</animal>",
  50,
  "<animal>&#10;Positive Test. Pattern2&#10;</animal>",
  "Tests ignoreSectContents for P64. Two \"ignore\" and one \"ignoreSectContents\" fields are included."
},
{
  "valid/P64/ibm64v03.xml",
  177,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 64-->\r\n<!DOCTYPE animal SYSTEM \"ibm64v03.dtd\"\r\n[\r\n<!ELEMENT animal ANY>\r\n]>\r\n<animal>\r\nPositive Test. Pattern3\r\n</animal>",
  50,
  "<animal>&#10;Positive Test. Pattern3&#10;</animal>",
  "Tests ignoreSectContents for P64. Four \"ignore\" and three \"ignoreSectContents\" fields are included."
},
{
  "valid/P65/ibm65v01.xml",
  192,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 65-->\r\n<!DOCTYPE animal SYSTEM \"ibm65v01.dtd\"\r\n[\r\n<!ELEMENT animal ANY>\r\n]>\r\n<animal>\r\nPositive Test. Pattern1. Empty string.\r\n</animal>",
  65,
  "<animal>&#10;Positive Test. Pattern1. Empty string.&#10;</animal>",
  "Tests Ignore for P65. An empty string occurs in the Ignore filed."
},
{
  "valid/P65/ibm65v02.xml",
  178,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 65-->\r\n<!DOCTYPE animal SYSTEM \"ibm65v02.dtd\"\r\n[\r\n<!ELEMENT animal ANY>\r\n]>\r\n<animal>\r\nPositive Test. Pattern2.\r\n</animal>",
  51,
  "<animal>&#10;Positive Test. Pattern2.&#10;</animal>",
  "Tests Ignore for P65. An string not including the brackets occurs in each of the Ignore filed."
},
{
  "valid/P66/ibm66v01.xml",
  363,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n  <!ELEMENT root (#PCDATA)>\r\n]>\r\n<root>\r\nTest all valid Charater references for P66:\r\n&#9;&#09;&#0000000009;\r\n&#xA;&#xa;&#x0A;&#x00000000A;\r\n&#x0d;\r\n&#xAB; &#xab; &#xCD; &#xcD; &#xEf; &#xef;\r\n&#67; &#x43; &#x5f;\r\n&#x20; &#xD7A3; &#xAC00;\r\n&#xF900; &#xFFFD;\r\n&#x10000; &#x10FFFF;\r\n</root>\r\n<!--* a valid test for P66 *-->\r\n",
  190,
  "<root>&#10;Test all valid Charater references for P66:&#10;&#9;&#9;&#9;&#10;&#10;&#10;&#10;&#10;&#10;&#13;&#10;\302\253 \302\253 \303\215 \303\215 \303\257 \303\257&#10;C C _&#10;  \355\236\243 \352\260\200&#10;\357\244\200 \357\277\275&#10;\360\220\200\200 \364\217\277\277&#10;</root>\277",
  "Tests all legal CharRef's."
},
{
  "valid/P67/ibm67v01.xml",
  218,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n  <!ELEMENT root (#PCDATA)>\r\n  <!ATTLIST root attr CDATA #REQUIRED>\r\n  <!ENTITY ge1 \"xyz\">\r\n]>\r\n<root attr=\"&ge1;&#65;\">\r\n&ge1; &#66;\r\n</root>\r\n<!--* a valid test for P67 *-->\r\n",
  40,
  "<root attr=\"xyzA\">&#10;xyz B&#10;</root>",
  "Tests Reference could be EntityRef or CharRef."
},
{
  "valid/P68/ibm68v01.xml",
  207,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root SYSTEM \"ibm68v01.dtd\" [\r\n  <!ELEMENT root (#PCDATA|a)* >\r\n]>\r\n<root>\r\n  pcdata content\r\n  <a attr1=\"xyz\"/>\r\n</root>\r\n<!--* a valid test for P68 VC:Entity Declared *-->\r\n",
  65,
  "<root>&#10;  pcdata content&#10;  <a attr1=\"xyz\"></a>&#10;</root>",
  "Tests P68 VC:Entity Declared with Entities in External Subset , standalone is no"
},
{
  "valid/P68/ibm68v02.xml",
  229,
  "<?xml version=\"1.0\" standalone='no'?>\r\n<!DOCTYPE root [\r\n  <!ELEMENT root (#PCDATA)* >\r\n  <!ENTITY % pe1 SYSTEM \"ibm68v02.ent\">\r\n  %pe1;\r\n]>\r\n<root>\r\n  pcdata content\r\n</root>\r\n<!--* a valid test for P68 VC:Entity Declared *-->\r\n",
  39,
  "<root>&#10;  pcdata content&#10;</root>",
  "Tests P68 VC:Entity Declared with Entities in External Parameter Entities , standalone is no"
},
{
  "valid/P69/ibm69v01.xml",
  277,
  "<?xml version=\"1.0\" standalone=\"no\" ?>\r\n<!DOCTYPE root SYSTEM \"ibm69v01.dtd\" [\r\n  <!ELEMENT root (#PCDATA|a)* >\r\n  <!ENTITY % pe1 \"<!-- comment in PE -->\">\r\n  %pe1;\r\n]>\r\n<root>\r\n  pcdata content\r\n  <a attr1=\"xyz\"/>\r\n</root>\r\n<!--* a valid test for P69 VC:Entity Declared *-->\r\n",
  65,
  "<root>&#10;  pcdata content&#10;  <a attr1=\"xyz\"></a>&#10;</root>",
  "Tests P68 VC:Entity Declared with Parameter Entities in External Subset , standalone is no"
},
{
  "valid/P69/ibm69v02.xml",
  229,
  "<?xml version=\"1.0\" standalone='no'?>\r\n<!DOCTYPE root [\r\n  <!ELEMENT root (#PCDATA)* >\r\n  <!ENTITY % pe1 SYSTEM \"ibm69v02.ent\">\r\n  %pe1;\r\n]>\r\n<root>\r\n  pcdata content\r\n</root>\r\n<!--* a valid test for P69 VC:Entity Declared *-->\r\n",
  39,
  "<root>&#10;  pcdata content&#10;</root>",
  "Tests P68 VC:Entity Declared with Parameter Entities in External Parameter Entities, standalone is no"
},
{
  "valid/P70/ibm70v01.xml",
  478,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n\r\n<!--* Test all legal patterns derived from P70-76 *-->\r\n<!ENTITY % pe1 '<!ATTLIST root att2 CDATA \"&ge1;\">'>\r\n<!ENTITY ge1 \"attdefaultvalue\" >\r\n%pe1;\r\n<!NOTATION JPGformat SYSTEM \"JPGFormat\">\r\n<!ENTITY ge2  SYSTEM \"image.jpg\" NDATA JPGformat>\r\n<!ENTITY % pe2 PUBLIC \"-//w3c//any\" \"ibm70v01.ent\" >\r\n%pe2;\r\n]>\r\n<root att2=\"any\" />\r\n<!-- a valid test case: test P70-P76 -->",
  85,
  "<!DOCTYPE root [\n<!NOTATION JPGformat SYSTEM 'JPGFormat'>\n]>\n<root att2=\"any\"></root>",
  "Tests all legal GEDecls and PEDecls constructs derived from P70-76"
},
{
  "valid/P78/ibm78v01.xml",
  365,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n\r\n<!--* Test all legal patterns derived from P78 *-->\r\n<!ENTITY epe1 SYSTEM \"ibm78v01.ent\" >\r\n<!ENTITY epe2 SYSTEM \"ibm78v02.ent\" >\r\n<!ENTITY epe3 SYSTEM \"ibm78v03.ent\" >\r\n\r\n]>\r\n<root>&epe1;&epe2;&epe3;</root>\r\n<!-- a valid test case: test P78, P77, P80 -->\r\n",
  206,
  "<root>&#10;anything legal as PCDATA....&#10;N.B. also tested P77 and P80&#10;anything legal as PCDATA....&#10;N.B. also tested P77anything legal as PCDATA....&#10;e.g. 12345678E-33, &quot;hello&quot;</root>",
  "Tests ExtParsedEnt, also TextDecl in P77 and EncodingDecl in P80"
},
{
  "valid/P79/ibm79v01.xml",
  250,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE book \r\n[\r\n<!ELEMENT book ANY>\r\n<!ATTLIST notebook att CDATA #IMPLIED>\r\n<!ENTITY % epe SYSTEM \"ibm79v01.ent\" >\r\n%epe;\r\n]>\r\n<book><name>XML Handbook</name> This is a book</book>\r\n\r\n<!-- a valid test case: test P79 -->\r\n",
  0,
  NULL,
  "Tests extPE"
},
{
  "valid/P82/ibm82v01.xml",
  397,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!ATTLIST root entatt1 ENTITY #REQUIRED >\r\n\r\n<!--* Test PublicID in P82 *-->\r\n<!NOTATION JPGformat PUBLIC \"-//image//notreal\" >\r\n<!ENTITY unparsed1  SYSTEM \"image.jpg\" NDATA JPGformat>\r\n]>\r\n<root entatt1=\"unparsed1\">test PublicID in P82</root>\r\n<!-- a valid test case: test P82 and P83 -->\r\n",
  122,
  "<!DOCTYPE root [\n<!NOTATION JPGformat PUBLIC '-//image//notreal'>\n]>\n<root entatt1=\"unparsed1\">test PublicID in P82</root>",
  "Tests NotationDecl in P82 and PublicID in P83"
},
{
  "valid/P85/ibm85v01.xml",
  4486,
  "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\r\n<!DOCTYPE book [\r\n<!ELEMENT book ANY>\r\n<!-- This test case covers 149 legal character ranges plus\r\n     51 discrete legal characters for production 85. -->\r\n<?NAME_41-A_5A-Z_4d-M_61-a_7A-z_6d-m_0C0-\303\200_0D6-\303\226_0cb-\303\213_0D8-\303\230_0F6-\303\266_0e7-\303\247_0F8-\303\270_0FF-\303\277_0fb-\303\273_100-\304\200_131-\304\261_118-\304\230_134-\304\264_13E-\304\276_139-\304\271_141-\305\201_148-\305\210_144-\305\204_14A-\305\212_17E-\305\276_164-\305\244_180-\306\200_1C3-\307\203_1a1-\306\241_1CD-\307\215_1F0-\307\260_1de-\307\236_1F4-\307\264_1F5-\307\265_1f4-\307\264_1FA-\307\272_217-\310\227_208-\310\210_250-\311\220_2A8-\312\250_27c-\311\274_2BB-\312\273_2C1-\313\201_2be-\312\276_386-\316\206_388-\316\210_38A-\316\212_389-\316\211_38C-\316\214_38E-\316\216_3A1-\316\241_397-\316\227_3A3-\316\243_3CE-\317\216_3b8-\316\270_3D0-\317\220_3D6-\317\226_3d3-\317\223_3DA-\317\232_3DC-\317\234_3DE-\317\236_3E0-\317\240_3E2-\317\242_3F3-\317\263_3ea-\317\252_401-\320\201_40C-\320\214_406-\320\206_40E-\320\216_44F-\321\217_42e-\320\256_451-\321\221_45C-\321\234_456-\321\226_45E-\321\236_481-\322\201_46f-\321\257_490-\322\220_4C4-\323\204_4aa-\322\252_4C7-\323\207_4C8-\323\210_4c7-\323\207_4CB-\323\213_4CC-\323\214_4cb-\323\213_4D0-\323\220_4EB-\323\253_4dd-\323\235_4EE-\323\256_4F5-\323\265_4f1-\323\261_4F8-\323\270_4F9-\323\271_4f8-\323\270_531-\324\261_556-\325\226_543-\325\203_559-\325\231_561-\325\241_586-\326\206_573-\325\263_5D0-\327\220_5EA-\327\252_5dd-\327\235_5F0-\327\260_5F2-\327\262_5f1-\327\261_621-\330\241_63A-\330\272_62d-\330\255_641-\331\201_64A-\331\212_645-\331\205_671-\331\261_6B7-\332\267_694-\332\224_6BA-\332\272_6BE-\332\276_6bc-\332\274_6C0-\333\200_6CE-\333\216_6c7-\333\207_6D0-\333\220_6D3-\333\223_6d1-\333\221_6D5-\333\225_6E5-\333\245_6E6-\333\246_6e5-\333\245_0905-\340\244\205_0939-\340\244\271_091f-\340\244\237_093D-\340\244\275_0958-\340\245\230_0961-\340\245\241_095c-\340\245\234_0985-\340\246\205_098C-\340\246\214_0988-\340\246\210_098F-\340\246\217_0990-\340\246\220_098f-\340\246\217_0993-\340\246\223_09A8-\340\246\250_099d-\340\246\235_09AA-\340\246\252_09B0-\340\246\260_09ad-\340\246\255_09B2-\340\246\262_09B6-\340\246\266_09B9-\340\246\271_09b7-\340\246\267_09DC-\340\247\234_09DD-\340\247\235_09dc-\340\247\234_09DF-\340\247\237_09E1-\340\247\241_09e0-\340\247\240_09F0-\340\247\260_09F1-\340\247\261_09f0-\340\247\260_0A05-\340\250\205_0A0A-\340\250\212_0a07-\340\250\207_0A0F-\340\250\217_0A10-\340\250\220_0a0f-\340\250\217_0A13-\340\250\223_0A28-\340\250\250_0a1d-\340\250\235_0A2A-\340\250\252_0A30-\340\250\260_0a2d-\340\250\255_0A32-\340\250\262_0A33-\340\250\263_0a32-\340\250\262_0A35-\340\250\265_0A36-\340\250\266_0a35-\340\250\265_0A38-\340\250\270_0A39-\340\250\271_0a38-\340\250\270_0A59-\340\251\231_0A5C-\340\251\234_0a5a-\340\251\232_0A5E-\340\251\236_0A72-\340\251\262_0A74-\340\251\264_0a73-\340\251\263_0A85-\340\252\205_0A8B-\340\252\213_0a88-\340\252\210_0A8D-\340\252\215_0A8F-\340\252\217_0A91-\340\252\221_0a90-\340\252\220_0A93-\340\252\223_0AA8-\340\252\250_0a9d-\340\252\235_0AAA-\340\252\252_0AB0-\340\252\260_0aad-\340\252\255_0AB2-\340\252\262_0AB3-\340\252\263_0ab2-\340\252\262_0AB5-\340\252\265_0AB9-\340\252\271_0ab7-\340\252\267_0ABD-\340\252\275_0AE0-\340\253\240_0B05-\340\254\205_0B0C-\340\254\214_0b08-\340\254\210_0B0F-\340\254\217_0B10-\340\254\220_0b0f-\340\254\217_0B13-\340\254\223_0B28-\340\254\250_0b1d-\340\254\235_0B2A-\340\254\252_0B30-\340\254\260_0b2d-\340\254\255_0B32-\340\254\262_0B33-\340\254\263_0b32-\340\254\262_0B36-\340\254\266_0B39-\340\254\271_0b37-\340\254\267_0B3D-\340\254\275_0B5C-\340\255\234_0B5D-\340\255\235_0b5c-\340\255\234_0B5F-\340\255\237_0B61-\340\255\241_0b60-\340\255\240_0B85-\340\256\205_0B8A-\340\256\212_0b87-\340\256\207_0B8E-\340\256\216_0B90-\340\256\220_0b8f-\340\256\217_0B92-\340\256\222_0B95-\340\256\225_0b93-\340\256\223_0B99-\340\256\231_0B9A-\340\256\232_0b99-\340\256\231_0B9C-\340\256\234_0B9E-\340\256\236_0B9F-\340\256\237_0b9e-\340\256\236_0BA3-\340\256\243_0BA4-\340\256\244_0ba3-\340\256\243_0BA8-\340\256\250_0BAA-\340\256\252_0ba9-\340\256\251_0BAE-\340\256\256_0BB5-\340\256\265_0bb1-\340\256\261_0BB7-\340\256\267_0BB9-\340\256\271_0bb8-\340\256\270_0C05-\340\260\205_0C0C-\340\260\214_0c08-\340\260\210_0C0E-\340\260\216_0C10-\340\260\220_0c0f-\340\260\217_0C12-\340\260\222_0C28-\340\260\250_0c1d-\340\260\235_0C2A-\340\260\252_0C33-\340\260\263_0c2e-\340\260\256_0C35-\340\260\265_0C39-\340\260\271_0c37-\340\260\267_0C60-\340\261\240_0C61-\340\261\241_0c60-\340\261\240_0C85-\340\262\205_0C8C-\340\262\214_0c88-\340\262\210_0C8E-\340\262\216_0C90-\340\262\220_0c8f-\340\262\217_0C92-\340\262\222_0CA8-\340\262\250_0c9d-\340\262\235_0CAA-\340\262\252_0CB3-\340\262\263_0cae-\340\262\256_0CB5-\340\262\265_0CB9-\340\262\271_0cb7-\340\262\267_0CDE-\340\263\236_0CE0-\340\263\240_0CE1-\340\263\241_0ce0-\340\263\240_0D05-\340\264\205_0D0C-\340\264\214_0d08-\340\264\210_0D0E-\340\264\216_0D10-\340\264\220_0d0f-\340\264\217_0D12-\340\264\222_0D28-\340\264\250_0d1d-\340\264\235_0D2A-\340\264\252_0D39-\340\264\271_0d31-\340\264\261_0D60-\340\265\240_0D61-\340\265\241_0d60-\340\265\240_0E01-\340\270\201_0E2E-\340\270\256_0e17-\340\270\227_0E30-\340\270\260_0E32-\340\270\262_0E33-\340\270\263_0e32-\340\270\262_0E40-\340\271\200_0E45-\340\271\205_0e42-\340\271\202_0E81-\340\272\201_0E82-\340\272\202_0e81-\340\272\201_0E84-\340\272\204_0E87-\340\272\207_0E88-\340\272\210_0e87-\340\272\207_0E8A-\340\272\212_0E8D-\340\272\215_0E94-\340\272\224_0E97-\340\272\227_0e95-\340\272\225_0E99-\340\272\231_0E9F-\340\272\237_0e9c-\340\272\234_0EA1-\340\272\241_0EA3-\340\272\243_0ea2-\340\272\242_0EA5-\340\272\245_0EA7-\340\272\247_0EAA-\340\272\252_0EAB-\340\272\253_0eaa-\340\272\252_0EAD-\340\272\255_0EAE-\340\272\256_0ead-\340\272\255_0EB0-\340\272\260_0EB2-\340\272\262_0EB3-\340\272\263_0eb2-\340\272\262_0EBD-\340\272\275_0EC0-\340\273\200_0EC4-\340\273\204_0ec2-\340\273\202_0F40-\340\275\200_0F47-\340\275\207_0f43-\340\275\203_0F49-\340\275\211_0F69-\340\275\251_0f59-\340\275\231_10A0-\341\202\240_10C5-\341\203\205_10b2-\341\202\262_10D0-\341\203\220_10F6-\341\203\266_10e3-\341\203\243_1100-\341\204\200_1102-\341\204\202_1103-\341\204\203_1102-\341\204\202_1105-\341\204\205_1107-\341\204\207_1106-\341\204\206_1109-\341\204\211_110B-\341\204\213_110C-\341\204\214_110b-\341\204\213_110E-\341\204\216_1112-\341\204\222_1110-\341\204\220_113C-\341\204\274_113E-\341\204\276_1140-\341\205\200_114C-\341\205\214_114E-\341\205\216_1150-\341\205\220_1154-\341\205\224_1155-\341\205\225_1154-\341\205\224_1159-\341\205\231_115F-\341\205\237_1161-\341\205\241_1160-\341\205\240_1163-\341\205\243_1165-\341\205\245_1167-\341\205\247_1169-\341\205\251_116D-\341\205\255_116E-\341\205\256_116d-\341\205\255_1172-\341\205\262_1173-\341\205\263_1172-\341\205\262_1175-\341\205\265_119E-\341\206\236_11A8-\341\206\250_11AB-\341\206\253_11AE-\341\206\256_11AF-\341\206\257_11ae-\341\206\256_11B7-\341\206\267_11B8-\341\206\270_11b7-\341\206\267_11BA-\341\206\272_11BC-\341\206\274_11C2-\341\207\202_11bf-\341\206\277_11EB-\341\207\253_11F0-\341\207\260_11F9-\341\207\271_1E00-\341\270\200_1E9B-\341\272\233_1e4d-\341\271\215_1EA0-\341\272\240_1EF9-\341\273\271_1ecc-\341\273\214_1F00-\341\274\200_1F15-\341\274\225_1f0a-\341\274\212_1F18-\341\274\230_1F1D-\341\274\235_1f1a-\341\274\232_1F20-\341\274\240_1F45-\341\275\205_1f32-\341\274\262_1F48-\341\275\210_1F4D-\341\275\215_1f4a-\341\275\212_1F50-\341\275\220_1F57-\341\275\227_1f53-\341\275\223_1F59-\341\275\231_1F5B-\341\275\233_1F5D-\341\275\235_1F5F-\341\275\237_1F7D-\341\275\275_1f6e-\341\275\256_1F80-\341\276\200_1FB4-\341\276\264_1f9a-\341\276\232_1FB6-\341\276\266_1FBC-\341\276\274_1fb9-\341\276\271_1FBE-\341\276\276_1FC2-\341\277\202_1FC4-\341\277\204_1fc3-\341\277\203_1FC6-\341\277\206_1FCC-\341\277\214_1fc9-\341\277\211_1FD0-\341\277\220_1FD3-\341\277\223_1fd1-\341\277\221_1FD6-\341\277\226_1FDB-\341\277\233_1fd8-\341\277\230_1FE0-\341\277\240_1FEC-\341\277\254_1fe6-\341\277\246_1FF2-\341\277\262_1FF4-\341\277\264_1ff3-\341\277\263_1FF6-\341\277\266_1FFC-\341\277\274_1ff9-\341\277\271_2126-\342\204\246_212A-\342\204\252_212B-\342\204\253_212a-\342\204\252_212E-\342\204\256_2180-\342\206\200_2182-\342\206\202_2181-\342\206\201_3041-\343\201\201_3094-\343\202\224_306a-\343\201\252_30A1-\343\202\241_30FA-\343\203\272_30cd-\343\203\215_3105-\343\204\205_312C-\343\204\254_3118-\343\204\230_AC00-\352\260\200_D7A3-\355\236\243_c1d1-\354\207\221 This is a PI target ?>\r\n]>\r\n<book/>\r\n\221",
  0,
  NULL,
  "This test case covers 149 legal character ranges plus 51 single legal characters for BaseChar in P85 using a PI target Name"
},
{
  "valid/P86/ibm86v01.xml",
  302,
  "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\r\n<!DOCTYPE book [\r\n<!ELEMENT book ANY>\r\n<!-- This test case covers 2 legal character ranges plus\r\n     1 discrete legal characters for production 86. -->\r\n<?NAME_4E00-\344\270\200_9FA5-\351\276\245_76d2-\347\233\222_3007-\343\200\207_3021-\343\200\241_3029-\343\200\251_3025-\343\200\245 This is a PI target ?>\r\n]>\r\n<book/>\r\n\245",
  0,
  NULL,
  "This test case covers 2 legal character ranges plus 1 single legal characters for IdeoGraphic in P86 using a PI target Name"
},
{
  "valid/P87/ibm87v01.xml",
  2200,
  "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\r\n<!DOCTYPE book [\r\n<!ELEMENT book ANY>\r\n<!-- This test case covers 65 legal character ranges plus\r\n     30 discrete legal characters for production 87. -->\r\n<?NAME_300-\314\200_345-\315\205_322-\314\242_360-\315\240_361-\315\241_360-\315\240_483-\322\203_486-\322\206_484-\322\204_591-\326\221_5A1-\326\241_599-\326\231_5A3-\326\243_5B9-\326\271_5ae-\326\256_5BB-\326\273_5BD-\326\275_5bc-\326\274_5BF-\326\277_5C1-\327\201_5C2-\327\202_5c1-\327\201_5C4-\327\204_64B-\331\213_652-\331\222_64e-\331\216_670-\331\260_6D6-\333\226_6DC-\333\234_6d9-\333\231_6DD-\333\235_6DF-\333\237_6de-\333\236_6E0-\333\240_6E4-\333\244_6e2-\333\242_6E7-\333\247_6E8-\333\250_6e7-\333\247_6EA-\333\252_6ED-\333\255_6eb-\333\253_0901-\340\244\201_0903-\340\244\203_0902-\340\244\202_093C-\340\244\274_093E-\340\244\276_094C-\340\245\214_0945-\340\245\205_094D-\340\245\215_0951-\340\245\221_0954-\340\245\224_0952-\340\245\222_0962-\340\245\242_0963-\340\245\243_0962-\340\245\242_0981-\340\246\201_0983-\340\246\203_0982-\340\246\202_09BC-\340\246\274_09BE-\340\246\276_09BF-\340\246\277_09C0-\340\247\200_09C4-\340\247\204_09c2-\340\247\202_09C7-\340\247\207_09C8-\340\247\210_09c7-\340\247\207_09CB-\340\247\213_09CD-\340\247\215_09cc-\340\247\214_09D7-\340\247\227_09E2-\340\247\242_09E3-\340\247\243_09e2-\340\247\242_0A02-\340\250\202_0A3C-\340\250\274_0A3E-\340\250\276_0A3F-\340\250\277_0A40-\340\251\200_0A42-\340\251\202_0a41-\340\251\201_0A47-\340\251\207_0A48-\340\251\210_0a47-\340\251\207_0A4B-\340\251\213_0A4D-\340\251\215_0a4c-\340\251\214_0A70-\340\251\260_0A71-\340\251\261_0a70-\340\251\260_0A81-\340\252\201_0A83-\340\252\203_0a82-\340\252\202_0ABC-\340\252\274_0ABE-\340\252\276_0AC5-\340\253\205_0ac1-\340\253\201_0AC7-\340\253\207_0AC9-\340\253\211_0ac8-\340\253\210_0ACB-\340\253\213_0ACD-\340\253\215_0acc-\340\253\214_0B01-\340\254\201_0B03-\340\254\203_0b02-\340\254\202_0B3C-\340\254\274_0B3E-\340\254\276_0B43-\340\255\203_0b40-\340\255\200_0B47-\340\255\207_0B48-\340\255\210_0b47-\340\255\207_0B4B-\340\255\213_0B4D-\340\255\215_0b4c-\340\255\214_0B56-\340\255\226_0B57-\340\255\227_0b56-\340\255\226_0B82-\340\256\202_0B83-\340\256\203_0b82-\340\256\202_0BBE-\340\256\276_0BC2-\340\257\202_0bc0-\340\257\200_0BC6-\340\257\206_0BC8-\340\257\210_0bc7-\340\257\207_0BCA-\340\257\212_0BCD-\340\257\215_0bcb-\340\257\213_0BD7-\340\257\227_0C01-\340\260\201_0C03-\340\260\203_0c02-\340\260\202_0C3E-\340\260\276_0C44-\340\261\204_0c41-\340\261\201_0C46-\340\261\206_0C48-\340\261\210_0c47-\340\261\207_0C4A-\340\261\212_0C4D-\340\261\215_0c4b-\340\261\213_0C55-\340\261\225_0C56-\340\261\226_0c55-\340\261\225_0C82-\340\262\202_0C83-\340\262\203_0c82-\340\262\202_0CBE-\340\262\276_0CC4-\340\263\204_0cc1-\340\263\201_0CC6-\340\263\206_0CC8-\340\263\210_0cc7-\340\263\207_0CCA-\340\263\212_0CCD-\340\263\215_0ccb-\340\263\213_0CD5-\340\263\225_0CD6-\340\263\226_0cd5-\340\263\225_0D02-\340\264\202_0D03-\340\264\203_0d02-\340\264\202_0D3E-\340\264\276_0D43-\340\265\203_0d40-\340\265\200_0D46-\340\265\206_0D48-\340\265\210_0d47-\340\265\207_0D4A-\340\265\212_0D4D-\340\265\215_0d4b-\340\265\213_0D57-\340\265\227_0E31-\340\270\261_0E34-\340\270\264_0E3A-\340\270\272_0e37-\340\270\267_0E47-\340\271\207_0E4E-\340\271\216_0e4a-\340\271\212_0EB1-\340\272\261_0EB4-\340\272\264_0EB9-\340\272\271_0eb6-\340\272\266_0EBB-\340\272\273_0EBC-\340\272\274_0ebb-\340\272\273_0EC8-\340\273\210_0ECD-\340\273\215_0eca-\340\273\212_0F18-\340\274\230_0F19-\340\274\231_0f18-\340\274\230_0F35-\340\274\265_0F37-\340\274\267_0F39-\340\274\271_0F3E-\340\274\276_0F3F-\340\274\277_0F71-\340\275\261_0F84-\340\276\204_0f7a-\340\275\272_0F86-\340\276\206_0F8B-\340\276\213_0f88-\340\276\210_0F90-\340\276\220_0F95-\340\276\225_0f92-\340\276\222_0F97-\340\276\227_0F99-\340\276\231_0FAD-\340\276\255_0fa3-\340\276\243_0FB1-\340\276\261_0FB7-\340\276\267_0fb4-\340\276\264_0FB9-\340\276\271_20D0-\342\203\220_20DC-\342\203\234_20d6-\342\203\226_20E1-\342\203\241_302A-\343\200\252_302F-\343\200\257_302c-\343\200\254_3099-\343\202\231_309A-\343\202\232 This is a PI target ?>\r\n]>\r\n<book/>\r\n\232",
  0,
  NULL,
  "This test case covers 65 legal character ranges plus 30 single legal characters for CombiningChar in P87 using a PI target Name"
},
{
  "valid/P88/ibm88v01.xml",
  621,
  "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\r\n<!DOCTYPE book [\r\n<!ELEMENT book ANY>\r\n<!-- This test case covers 15 legal character ranges plus\r\n     0 discrete legal characters for production 88. -->\r\n<?NAME_30-0_39-9_34-4_660-\331\240_669-\331\251_664-\331\244_6F0-\333\260_6F9-\333\271_6f4-\333\264_0966-\340\245\246_096F-\340\245\257_096a-\340\245\252_09E6-\340\247\246_09EF-\340\247\257_09ea-\340\247\252_0A66-\340\251\246_0A6F-\340\251\257_0a6a-\340\251\252_0AE6-\340\253\246_0AEF-\340\253\257_0aea-\340\253\252_0B66-\340\255\246_0B6F-\340\255\257_0b6a-\340\255\252_0BE7-\340\257\247_0BEF-\340\257\257_0beb-\340\257\253_0C66-\340\261\246_0C6F-\340\261\257_0c6a-\340\261\252_0CE6-\340\263\246_0CEF-\340\263\257_0cea-\340\263\252_0D66-\340\265\246_0D6F-\340\265\257_0d6a-\340\265\252_0E50-\340\271\220_0E59-\340\271\231_0e54-\340\271\224_0ED0-\340\273\220_0ED9-\340\273\231_0ed4-\340\273\224_0F20-\340\274\240_0F29-\340\274\251_0f24-\340\274\244 This is a PI target ?>\r\n]>\r\n<book/>\r\n\244",
  0,
  NULL,
  "This test case covers 15 legal character ranges for Digit in P88 using a PI target Name"
},
{
  "valid/P89/ibm89v01.xml",
  382,
  "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\r\n<!DOCTYPE book [\r\n<!ELEMENT book ANY>\r\n<!-- This test case covers 3 legal character ranges plus\r\n     8 discrete legal characters for production 89. -->\r\n<?NAME_0B7-\302\267_2D0-\313\220_2D1-\313\221_387-\316\207_640-\331\200_0E46-\340\271\206_0EC6-\340\273\206_3005-\343\200\205_3031-\343\200\261_3035-\343\200\265_3033-\343\200\263_309D-\343\202\235_309E-\343\202\236_309d-\343\202\235_30FC-\343\203\274_30FE-\343\203\276_30fd-\343\203\275 This is a PI target ?>\r\n]>\r\n<book/>\r\n\275",
  0,
  NULL,
  "This test case covers 3 legal character ranges plus 8 single legal characters for Extender in P89 using a PI target Name"
},
/* ==== from ibm ibm_oasis_not-wf.xml ==== */
{
  "not-wf/P01/ibm01n01.xml",
  96,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE book [\r\n   <!ELEMENT book ANY>\r\n]>\r\n<!-- element is missing -->",
  0,
  NULL,
  "Tests a document with no element. A well-formed document should have at lease one elements."
},
{
  "not-wf/P01/ibm01n02.xml",
  120,
  "<doc>Wrong ordering between prolog and element!</doc>\r\n<?xml version=\"1.0\"?>\r\n<!DOCTYPE doc [\r\n   <!ELEMENT doc ANY>\r\n]>",
  0,
  NULL,
  "Tests a document with wrong ordering of its prolog and element. The element occurs before the xml declaration and the DTD."
},
{
  "not-wf/P01/ibm01n03.xml",
  217,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE doc [\r\n   <!ELEMENT doc ANY>\r\n   <!ELEMENT title ANY>\r\n]>\r\n<doc>Wrong combination!</doc>\r\n<?PI after document element?>\r\n<title>Wrong combination!</title>\r\n<?PI after title element?>\r\n",
  0,
  NULL,
  "Tests a document with wrong combination of misc and element. One PI occurs between two elements."
},
{
  "not-wf/P02/ibm02n01.xml",
  91,
  "<!DOCTYPE book [\r\n<!ELEMENT book ANY>\r\n]>\r\n<!-- IllegalChar #x00\r\n in p02: \0 -->\r\n<book/>\r\n\0",
  0,
  NULL,
  "Tests a comment which contains an illegal Char: #x00"
},
{
  "not-wf/P02/ibm02n02.xml",
  91,
  "<!DOCTYPE book [\r\n<!ELEMENT book ANY>\r\n]>\r\n<!-- IllegalChar #x01\r\n in p02: \1 -->\r\n<book/>\r\n\1",
  0,
  NULL,
  "Tests a comment which contains an illegal Char: #x01"
},
{
  "not-wf/P02/ibm02n03.xml",
  91,
  "<!DOCTYPE book [\r\n<!ELEMENT book ANY>\r\n]>\r\n<!-- IllegalChar #x02\r\n in p02: \2 -->\r\n<book/>\r\n\2",
  0,
  NULL,
  "Tests a comment which contains an illegal Char: #x02"
},
{
  "not-wf/P02/ibm02n04.xml",
  91,
  "<!DOCTYPE book [\r\n<!ELEMENT book ANY>\r\n]>\r\n<!-- IllegalChar #x03\r\n in p02: \3 -->\r\n<book/>\r\n\3",
  0,
  NULL,
  "Tests a comment which contains an illegal Char: #x03"
},
{
  "not-wf/P02/ibm02n05.xml",
  91,
  "<!DOCTYPE book [\r\n<!ELEMENT book ANY>\r\n]>\r\n<!-- IllegalChar #x04\r\n in p02: \4 -->\r\n<book/>\r\n\4",
  0,
  NULL,
  "Tests a comment which contains an illegal Char: #x04"
},
{
  "not-wf/P02/ibm02n06.xml",
  91,
  "<!DOCTYPE book [\r\n<!ELEMENT book ANY>\r\n]>\r\n<!-- IllegalChar #x05\r\n in p02: \5 -->\r\n<book/>\r\n\5",
  0,
  NULL,
  "Tests a comment which contains an illegal Char: #x05"
},
{
  "not-wf/P02/ibm02n07.xml",
  91,
  "<!DOCTYPE book [\r\n<!ELEMENT book ANY>\r\n]>\r\n<!-- IllegalChar #x06\r\n in p02: \6 -->\r\n<book/>\r\n\6",
  0,
  NULL,
  "Tests a comment which contains an illegal Char: #x06"
},
{
  "not-wf/P02/ibm02n08.xml",
  91,
  "<!DOCTYPE book [\r\n<!ELEMENT book ANY>\r\n]>\r\n<!-- IllegalChar #x07\r\n in p02: \a -->\r\n<book/>\r\n",
  0,
  NULL,
  "Tests a comment which contains an illegal Char: #x07"
},
{
  "not-wf/P02/ibm02n09.xml",
  91,
  "<!DOCTYPE book [\r\n<!ELEMENT book ANY>\r\n]>\r\n<!-- IllegalChar #x08\r\n in p02: \b -->\r\n<book/>\r\n",
  0,
  NULL,
  "Tests a comment which contains an illegal Char: #x08"
},
{
  "not-wf/P02/ibm02n10.xml",
  91,
  "<!DOCTYPE book [\r\n<!ELEMENT book ANY>\r\n]>\r\n<!-- IllegalChar #x0b\r\n in p02: \v -->\r\n<book/>\r\n",
  0,
  NULL,
  "Tests a comment which contains an illegal Char: #x0B"
},
{
  "not-wf/P02/ibm02n11.xml",
  91,
  "<!DOCTYPE book [\r\n<!ELEMENT book ANY>\r\n]>\r\n<!-- IllegalChar #x0c\r\n in p02: \f -->\r\n<book/>\r\n",
  0,
  NULL,
  "Tests a comment which contains an illegal Char: #x0C"
},
{
  "not-wf/P02/ibm02n12.xml",
  91,
  "<!DOCTYPE book [\r\n<!ELEMENT book ANY>\r\n]>\r\n<!-- IllegalChar #x0e\r\n in p02: \16 -->\r\n<book/>\r\n\16",
  0,
  NULL,
  "Tests a comment which contains an illegal Char: #x0E"
},
{
  "not-wf/P02/ibm02n13.xml",
  91,
  "<!DOCTYPE book [\r\n<!ELEMENT book ANY>\r\n]>\r\n<!-- IllegalChar #x0f\r\n in p02: \17 -->\r\n<book/>\r\n\17",
  0,
  NULL,
  "Tests a comment which contains an illegal Char: #x0F"
},
{
  "not-wf/P02/ibm02n14.xml",
  91,
  "<!DOCTYPE book [\r\n<!ELEMENT book ANY>\r\n]>\r\n<!-- IllegalChar #x10\r\n in p02: \20 -->\r\n<book/>\r\n\20",
  0,
  NULL,
  "Tests a comment which contains an illegal Char: #x10"
},
{
  "not-wf/P02/ibm02n15.xml",
  91,
  "<!DOCTYPE book [\r\n<!ELEMENT book ANY>\r\n]>\r\n<!-- IllegalChar #x11\r\n in p02: \21 -->\r\n<book/>\r\n\21",
  0,
  NULL,
  "Tests a comment which contains an illegal Char: #x11"
},
{
  "not-wf/P02/ibm02n16.xml",
  91,
  "<!DOCTYPE book [\r\n<!ELEMENT book ANY>\r\n]>\r\n<!-- IllegalChar #x12\r\n in p02: \22 -->\r\n<book/>\r\n\22",
  0,
  NULL,
  "Tests a comment which contains an illegal Char: #x12"
},
{
  "not-wf/P02/ibm02n17.xml",
  91,
  "<!DOCTYPE book [\r\n<!ELEMENT book ANY>\r\n]>\r\n<!-- IllegalChar #x13\r\n in p02: \23 -->\r\n<book/>\r\n\23",
  0,
  NULL,
  "Tests a comment which contains an illegal Char: #x13"
},
{
  "not-wf/P02/ibm02n18.xml",
  91,
  "<!DOCTYPE book [\r\n<!ELEMENT book ANY>\r\n]>\r\n<!-- IllegalChar #x14\r\n in p02: \24 -->\r\n<book/>\r\n\24",
  0,
  NULL,
  "Tests a comment which contains an illegal Char: #x14"
},
{
  "not-wf/P02/ibm02n19.xml",
  91,
  "<!DOCTYPE book [\r\n<!ELEMENT book ANY>\r\n]>\r\n<!-- IllegalChar #x15\r\n in p02: \25 -->\r\n<book/>\r\n\25",
  0,
  NULL,
  "Tests a comment which contains an illegal Char: #x15"
},
{
  "not-wf/P02/ibm02n20.xml",
  91,
  "<!DOCTYPE book [\r\n<!ELEMENT book ANY>\r\n]>\r\n<!-- IllegalChar #x16\r\n in p02: \26 -->\r\n<book/>\r\n\26",
  0,
  NULL,
  "Tests a comment which contains an illegal Char: #x16"
},
{
  "not-wf/P02/ibm02n21.xml",
  91,
  "<!DOCTYPE book [\r\n<!ELEMENT book ANY>\r\n]>\r\n<!-- IllegalChar #x17\r\n in p02: \27 -->\r\n<book/>\r\n\27",
  0,
  NULL,
  "Tests a comment which contains an illegal Char: #x17"
},
{
  "not-wf/P02/ibm02n22.xml",
  91,
  "<!DOCTYPE book [\r\n<!ELEMENT book ANY>\r\n]>\r\n<!-- IllegalChar #x18\r\n in p02: \30 -->\r\n<book/>\r\n\30",
  0,
  NULL,
  "Tests a comment which contains an illegal Char: #x18"
},
{
  "not-wf/P02/ibm02n23.xml",
  91,
  "<!DOCTYPE book [\r\n<!ELEMENT book ANY>\r\n]>\r\n<!-- IllegalChar #x19\r\n in p02: \31 -->\r\n<book/>\r\n\31",
  0,
  NULL,
  "Tests a comment which contains an illegal Char: #x19"
},
{
  "not-wf/P02/ibm02n24.xml",
  91,
  "<!DOCTYPE book [\r\n<!ELEMENT book ANY>\r\n]>\r\n<!-- IllegalChar #x1a\r\n in p02: \32 -->\r\n<book/>\r\n\32",
  0,
  NULL,
  "Tests a comment which contains an illegal Char: #x1A"
},
{
  "not-wf/P02/ibm02n25.xml",
  91,
  "<!DOCTYPE book [\r\n<!ELEMENT book ANY>\r\n]>\r\n<!-- IllegalChar #x1b\r\n in p02: \33 -->\r\n<book/>\r\n\33",
  0,
  NULL,
  "Tests a comment which contains an illegal Char: #x1B"
},
{
  "not-wf/P02/ibm02n26.xml",
  91,
  "<!DOCTYPE book [\r\n<!ELEMENT book ANY>\r\n]>\r\n<!-- IllegalChar #x1c\r\n in p02: \34 -->\r\n<book/>\r\n\34",
  0,
  NULL,
  "Tests a comment which contains an illegal Char: #x1C"
},
{
  "not-wf/P02/ibm02n27.xml",
  91,
  "<!DOCTYPE book [\r\n<!ELEMENT book ANY>\r\n]>\r\n<!-- IllegalChar #x1d\r\n in p02: \35 -->\r\n<book/>\r\n\35",
  0,
  NULL,
  "Tests a comment which contains an illegal Char: #x1D"
},
{
  "not-wf/P02/ibm02n28.xml",
  91,
  "<!DOCTYPE book [\r\n<!ELEMENT book ANY>\r\n]>\r\n<!-- IllegalChar #x1e\r\n in p02: \36 -->\r\n<book/>\r\n\36",
  0,
  NULL,
  "Tests a comment which contains an illegal Char: #x1E"
},
{
  "not-wf/P02/ibm02n29.xml",
  91,
  "<!DOCTYPE book [\r\n<!ELEMENT book ANY>\r\n]>\r\n<!-- IllegalChar #x1f\r\n in p02: \37 -->\r\n<book/>\r\n\37",
  0,
  NULL,
  "Tests a comment which contains an illegal Char: #x1F"
},
{
  "not-wf/P02/ibm02n30.xml",
  95,
  "<!DOCTYPE book [\r\n<!ELEMENT book ANY>\r\n]>\r\n<!-- IllegalChar #xd800\r\n in p02: \355\240\200 -->\r\n<book/>\r\n\200",
  0,
  NULL,
  "Tests a comment which contains an illegal Char: #xD800"
},
{
  "not-wf/P02/ibm02n31.xml",
  95,
  "<!DOCTYPE book [\r\n<!ELEMENT book ANY>\r\n]>\r\n<!-- IllegalChar #xdfff\r\n in p02: \355\277\277 -->\r\n<book/>\r\n\277",
  0,
  NULL,
  "Tests a comment which contains an illegal Char: #xDFFF"
},
{
  "not-wf/P02/ibm02n32.xml",
  95,
  "<!DOCTYPE book [\r\n<!ELEMENT book ANY>\r\n]>\r\n<!-- IllegalChar #xfffe\r\n in p02: \357\277\276 -->\r\n<book/>\r\n\276",
  0,
  NULL,
  "Tests a comment which contains an illegal Char: #xFFFE"
},
{
  "not-wf/P02/ibm02n33.xml",
  95,
  "<!DOCTYPE book [\r\n<!ELEMENT book ANY>\r\n]>\r\n<!-- IllegalChar #xffff\r\n in p02: \357\277\277 -->\r\n<book/>\r\n\277",
  0,
  NULL,
  "Tests a comment which contains an illegal Char: #xFFFF"
},
{
  "not-wf/P03/ibm03n01.xml",
  138,
  "<!DOCTYPE book [\r\n<!ELEMENT book ANY>\r\n]>\r\n<!-- IllegalChar #x3000\r\n in p03: \343\200\200 -->\r\n<book>Illegal space 3000 in the end tag</book \343\200\200>\r\n\200",
  0,
  NULL,
  "Tests an end tag which contains an illegal space character #x3000 which follows the element name \"book\"."
},
{
  "not-wf/P04/ibm04n01.xml",
  95,
  "<!DOCTYPE IllegalNameChar! [\r\n   <!ELEMENT IllegalNameChar! EMPTY>\r\n]>\r\n<IllegalNameChar!/>\r\n\r\n",
  0,
  NULL,
  "Tests an element name which contains an illegal ASCII NameChar. \"IllegalNameChar\" is followed by #x21"
},
{
  "not-wf/P04/ibm04n02.xml",
  95,
  "<!DOCTYPE IllegalNameChar( [\r\n   <!ELEMENT IllegalNameChar( EMPTY>\r\n]>\r\n<IllegalNameChar(/>\r\n\r\n",
  0,
  NULL,
  "Tests an element name which contains an illegal ASCII NameChar. \"IllegalNameChar\" is followed by #x28"
},
{
  "not-wf/P04/ibm04n03.xml",
  95,
  "<!DOCTYPE IllegalNameChar) [\r\n   <!ELEMENT IllegalNameChar) EMPTY>\r\n]>\r\n<IllegalNameChar)/>\r\n\r\n",
  0,
  NULL,
  "Tests an element name which contains an illegal ASCII NameChar. \"IllegalNameChar\" is followed by #x29"
},
{
  "not-wf/P04/ibm04n04.xml",
  95,
  "<!DOCTYPE IllegalNameChar+ [\r\n   <!ELEMENT IllegalNameChar+ EMPTY>\r\n]>\r\n<IllegalNameChar+/>\r\n\r\n",
  0,
  NULL,
  "Tests an element name which contains an illegal ASCII NameChar. \"IllegalNameChar\" is followed by #x2B"
},
{
  "not-wf/P04/ibm04n05.xml",
  95,
  "<!DOCTYPE IllegalNameChar, [\r\n   <!ELEMENT IllegalNameChar, EMPTY>\r\n]>\r\n<IllegalNameChar,/>\r\n\r\n",
  0,
  NULL,
  "Tests an element name which contains an illegal ASCII NameChar. \"IllegalNameChar\" is followed by #x2C"
},
{
  "not-wf/P04/ibm04n06.xml",
  95,
  "<!DOCTYPE IllegalNameChar/ [\r\n   <!ELEMENT IllegalNameChar/ EMPTY>\r\n]>\r\n<IllegalNameChar//>\r\n\r\n",
  0,
  NULL,
  "Tests an element name which contains an illegal ASCII NameChar. \"IllegalNameChar\" is followed by #x2F"
},
{
  "not-wf/P04/ibm04n07.xml",
  95,
  "<!DOCTYPE IllegalNameChar; [\r\n   <!ELEMENT IllegalNameChar; EMPTY>\r\n]>\r\n<IllegalNameChar;/>\r\n\r\n",
  0,
  NULL,
  "Tests an element name which contains an illegal ASCII NameChar. \"IllegalNameChar\" is followed by #x3B"
},
{
  "not-wf/P04/ibm04n08.xml",
  95,
  "<!DOCTYPE IllegalNameChar< [\r\n   <!ELEMENT IllegalNameChar< EMPTY>\r\n]>\r\n<IllegalNameChar</>\r\n\r\n",
  0,
  NULL,
  "Tests an element name which contains an illegal ASCII NameChar. \"IllegalNameChar\" is followed by #x3C"
},
{
  "not-wf/P04/ibm04n09.xml",
  95,
  "<!DOCTYPE IllegalNameChar= [\r\n   <!ELEMENT IllegalNameChar= EMPTY>\r\n]>\r\n<IllegalNameChar=/>\r\n\r\n",
  0,
  NULL,
  "Tests an element name which contains an illegal ASCII NameChar. \"IllegalNameChar\" is followed by #x3D"
},
{
  "not-wf/P04/ibm04n10.xml",
  95,
  "<!DOCTYPE IllegalNameChar? [\r\n   <!ELEMENT IllegalNameChar? EMPTY>\r\n]>\r\n<IllegalNameChar?/>\r\n\r\n",
  0,
  NULL,
  "Tests an element name which contains an illegal ASCII NameChar. \"IllegalNameChar\" is followed by #x3F"
},
{
  "not-wf/P04/ibm04n11.xml",
  95,
  "<!DOCTYPE IllegalNameChar[ [\r\n   <!ELEMENT IllegalNameChar[ EMPTY>\r\n]>\r\n<IllegalNameChar[/>\r\n\r\n",
  0,
  NULL,
  "Tests an element name which contains an illegal ASCII NameChar. \"IllegalNameChar\" is followed by #x5B"
},
{
  "not-wf/P04/ibm04n12.xml",
  95,
  "<!DOCTYPE IllegalNameChar\\ [\r\n   <!ELEMENT IllegalNameChar\\ EMPTY>\r\n]>\r\n<IllegalNameChar\\/>\r\n\r\n",
  0,
  NULL,
  "Tests an element name which contains an illegal ASCII NameChar. \"IllegalNameChar\" is followed by #x5C"
},
{
  "not-wf/P04/ibm04n13.xml",
  95,
  "<!DOCTYPE IllegalNameChar] [\r\n   <!ELEMENT IllegalNameChar] EMPTY>\r\n]>\r\n<IllegalNameChar]/>\r\n\r\n",
  0,
  NULL,
  "Tests an element name which contains an illegal ASCII NameChar. \"IllegalNameChar\" is followed by #x5D"
},
{
  "not-wf/P04/ibm04n14.xml",
  95,
  "<!DOCTYPE IllegalNameChar^ [\r\n   <!ELEMENT IllegalNameChar^ EMPTY>\r\n]>\r\n<IllegalNameChar^/>\r\n\r\n",
  0,
  NULL,
  "Tests an element name which contains an illegal ASCII NameChar. \"IllegalNameChar\" is followed by #x5E"
},
{
  "not-wf/P04/ibm04n15.xml",
  95,
  "<!DOCTYPE IllegalNameChar` [\r\n   <!ELEMENT IllegalNameChar` EMPTY>\r\n]>\r\n<IllegalNameChar`/>\r\n\r\n",
  0,
  NULL,
  "Tests an element name which contains an illegal ASCII NameChar. \"IllegalNameChar\" is followed by #x60"
},
{
  "not-wf/P04/ibm04n16.xml",
  95,
  "<!DOCTYPE IllegalNameChar{ [\r\n   <!ELEMENT IllegalNameChar{ EMPTY>\r\n]>\r\n<IllegalNameChar{/>\r\n\r\n",
  0,
  NULL,
  "Tests an element name which contains an illegal ASCII NameChar. \"IllegalNameChar\" is followed by #x7B"
},
{
  "not-wf/P04/ibm04n17.xml",
  95,
  "<!DOCTYPE IllegalNameChar| [\r\n   <!ELEMENT IllegalNameChar| EMPTY>\r\n]>\r\n<IllegalNameChar|/>\r\n\r\n",
  0,
  NULL,
  "Tests an element name which contains an illegal ASCII NameChar. \"IllegalNameChar\" is followed by #x7C"
},
{
  "not-wf/P04/ibm04n18.xml",
  95,
  "<!DOCTYPE IllegalNameChar} [\r\n   <!ELEMENT IllegalNameChar} EMPTY>\r\n]>\r\n<IllegalNameChar}/>\r\n\r\n",
  0,
  NULL,
  "Tests an element name which contains an illegal ASCII NameChar. \"IllegalNameChar\" is followed by #x7D"
},
{
  "not-wf/P05/ibm05n01.xml",
  107,
  "<!DOCTYPE .A_name_starts_with. [\r\n   <!ELEMENT .A_name_starts_with. EMPTY>\r\n]>\r\n<.A_name_starts_with./>  \r\n",
  0,
  NULL,
  "Tests an element name which has an illegal first character. An illegal first character \".\" is followed by \"A_name-starts_with.\"."
},
{
  "not-wf/P05/ibm05n02.xml",
  107,
  "<!DOCTYPE -A_name_starts_With- [\r\n   <!ELEMENT -A_name_starts_With- EMPTY>\r\n]>\r\n<-A_name_starts_With-/>  \r\n",
  0,
  NULL,
  "Tests an element name which has an illegal first character. An illegal first character \"-\" is followed by \"A_name-starts_with-\"."
},
{
  "not-wf/P05/ibm05n03.xml",
  122,
  "<!DOCTYPE 5A_name_starts_with_digit [\r\n   <!ELEMENT 5A_name_starts_with_digit EMPTY>\r\n]>\r\n<5A_name_starts_with_digit/>  \r\n",
  0,
  NULL,
  "Tests an element name which has an illegal first character. An illegal first character \"5\" is followed by \"A_name-starts_with_digit\"."
},
{
  "not-wf/P09/ibm09n01.xml",
  223,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)> \r\n \t<!ENTITY FullName \"Snow%Man\">\r\n]>\r\n\r\n<!-- testing invalid entity value -->\r\n<student>My Name is &FullName;. </student>\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n ",
  0,
  NULL,
  "Tests an internal general entity with an invalid value. The entity \"Fullname\" contains \"%\"."
},
{
  "not-wf/P09/ibm09n02.xml",
  196,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)> \r\n \t<!ENTITY FullName \"Snow&Man\">\r\n]>\r\n\r\n<!-- testing invalid entity value -->\r\n<student>My Name is &FullName;. </student>",
  0,
  NULL,
  "Tests an internal general entity with an invalid value. The entity \"Fullname\" contains the ampersand character."
},
{
  "not-wf/P09/ibm09n03.xml",
  196,
  "<?xml version=\"1.0\"?> \r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)>\r\n\t<!ENTITY FullName \"Snow\"Man\"> \r\n]>\r\n\r\n<!-- testing invalid entity value -->\r\n<student>My Name is &FullName;. </student>",
  0,
  NULL,
  "Tests an internal general entity with an invalid value. The entity \"Fullname\" contains the double quote character in the middle."
},
{
  "not-wf/P09/ibm09n04.xml",
  196,
  "<?xml version=\"1.0\"?> \r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)>\r\n\t<!ENTITY FullName \"SnowMan> \r\n]>\r\n\r\n<!-- testing invalid entity value -->\r\n<student>My Name is &FullName;. </student>\r\n",
  0,
  NULL,
  "Tests an internal general entity with an invalid value. The closing bracket (double quote) is missing with the value of the entity \"FullName\"."
},
{
  "not-wf/P10/ibm10n01.xml",
  357,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)>\r\n\t<!ATTLIST student\r\n\t\tfirst CDATA #REQUIRED\r\n\t\tmiddle CDATA #IMPLIED\r\n\t\tlast CDATA #IMPLIED > \r\n\t<!ENTITY myfirst \"Snow\">\r\n\t<!ENTITY mymiddle \"I\">\r\n\t<!ENTITY mylast \"Man\">\r\n]>\r\n\r\n<!-- testing invalid attvalue -->\r\n<student first=\"Snow<Man\">My Name is SnowMan. </student>\r\n\r\n\r\n\r\n\r\n\r\n",
  0,
  NULL,
  "Tests an attribute with an invalid value. The value of the attribute \"first\" contains the character \"less than\"."
},
{
  "not-wf/P10/ibm10n02.xml",
  347,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)>\r\n\t<!ATTLIST student\r\n\t\tfirst CDATA #REQUIRED\r\n\t\tmiddle CDATA #IMPLIED\r\n\t\tlast CDATA #IMPLIED > \r\n\t<!ENTITY myfirst \"Snow\">\r\n\t<!ENTITY mymiddle \"I\">\r\n\t<!ENTITY mylast \"Man\">\r\n]>\r\n\r\n<!-- testing invalid attvalue -->\r\n<student first=\"Snow&Man\">My Name is SnowMan. </student>\r\n",
  0,
  NULL,
  "Tests an attribute with an invalid value. The value of the attribute \"first\" contains the character ampersand."
},
{
  "not-wf/P10/ibm10n03.xml",
  347,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)>\r\n\t<!ATTLIST student\r\n\t\tfirst CDATA #REQUIRED\r\n\t\tmiddle CDATA #IMPLIED\r\n\t\tlast CDATA #IMPLIED > \r\n\t<!ENTITY myfirst \"Snow\">\r\n\t<!ENTITY mymiddle \"I\">\r\n\t<!ENTITY mylast \"Man\">\r\n]>\r\n\r\n<!-- testing invalid attvalue -->\r\n<student first=\"Snow\"Man\">My Name is SnowMan. </student>\r\n",
  0,
  NULL,
  "Tests an attribute with an invalid value. The value of the attribute \"first\" contains the double quote character in the middle."
},
{
  "not-wf/P10/ibm10n04.xml",
  367,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)>\r\n\t<!ATTLIST student\r\n\t\tfirst CDATA #REQUIRED\r\n\t\tmiddle CDATA #IMPLIED\r\n\t\tlast CDATA #IMPLIED > \r\n\t<!ENTITY myfirst \"Snow\">\r\n\t<!ENTITY mymiddle \"I\">\r\n\t<!ENTITY mylast \"Man\">\r\n]>\r\n\r\n<!-- testing invalid attvalue with no closing bracket -->\r\n<student first=\"Snow >My Name is SnowMan. </student>\r\n",
  0,
  NULL,
  "Tests an attribute with an invalid value. The closing bracket (double quote) is missing with The value of the attribute \"first\"."
},
{
  "not-wf/P10/ibm10n05.xml",
  347,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)>\r\n\t<!ATTLIST student\r\n\t\tfirst CDATA #REQUIRED\r\n\t\tmiddle CDATA #IMPLIED\r\n\t\tlast CDATA #IMPLIED > \r\n\t<!ENTITY myfirst \"SNow\">\r\n\t<!ENTITY mymiddle \"I\">\r\n\t<!ENTITY mylast \"Man\">\r\n]>\r\n\r\n<!-- testing invalid attvalue -->\r\n<student first='Snow<Man'>My Name is SnowMan. </student>\r\n",
  0,
  NULL,
  "Tests an attribute with an invalid value. The value of the attribute \"first\" contains the character \"less than\"."
},
{
  "not-wf/P10/ibm10n06.xml",
  347,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)>\r\n\t<!ATTLIST student\r\n\t\tfirst CDATA #REQUIRED\r\n\t\tmiddle CDATA #IMPLIED\r\n\t\tlast CDATA #IMPLIED > \r\n\t<!ENTITY myfirst \"Snow\">\r\n\t<!ENTITY mymiddle \"I\">\r\n\t<!ENTITY mylast \"Man\">\r\n]>\r\n\r\n<!-- testing invalid attvalue -->\r\n<student first='Snow&Man'>My Name is SnowMan. </student>\r\n",
  0,
  NULL,
  "Tests an attribute with an invalid value. The value of the attribute \"first\" contains the character ampersand."
},
{
  "not-wf/P10/ibm10n07.xml",
  347,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)>\r\n\t<!ATTLIST student\r\n\t\tfirst CDATA #REQUIRED\r\n\t\tmiddle CDATA #IMPLIED\r\n\t\tlast CDATA #IMPLIED > \r\n\t<!ENTITY myfirst \"Snow\">\r\n\t<!ENTITY mymiddle \"I\">\r\n\t<!ENTITY mylast \"Man\">\r\n]>\r\n\r\n<!-- testing invalid attvalue -->\r\n<student first=\"Snow\"Man\">My Name is SnowMan. </student>\r\n",
  0,
  NULL,
  "Tests an attribute with an invalid value. The value of the attribute \"first\" contains the double quote character in the middle."
},
{
  "not-wf/P10/ibm10n08.xml",
  372,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)>\r\n\t<!ATTLIST student\r\n\t\tfirst CDATA #REQUIRED\r\n\t\tmiddle CDATA #IMPLIED\r\n\t\tlast CDATA #IMPLIED > \r\n\t<!ENTITY myfirst \"Snow\">\r\n\t<!ENTITY mymiddle \"I\">\r\n\t<!ENTITY mylast \"Man\">\r\n]>\r\n\r\n<!-- testing invalid attvalue with no closing single quote -->\r\n<student first='Snow >My Name is SnowMan. </student>\r\n",
  0,
  NULL,
  "Tests an attribute with an invalid value. The closing bracket (single quote) is missing with the value of the attribute \"first\"."
},
{
  "not-wf/P11/ibm11n01.xml",
  207,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student SYSTEM \"student\".dtd\"[\r\n\t<!ELEMENT student (#PCDATA)> \r\n]>\r\n\r\n<!-- testing invalid system literal  -->\r\n<student>My Name is SnowMan. </student>\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n ",
  0,
  NULL,
  "Tests SystemLiteral. The systemLiteral for the element \"student\" has a double quote character in the middle."
},
{
  "not-wf/P11/ibm11n02.xml",
  186,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student SYSTEM 'student'.dtd'[\r\n\t<!ELEMENT student (#PCDATA)> \r\n]>\r\n\r\n<!-- testing invalid system literal  -->\r\n<student>My Name is SnowMan. </student>\r\n",
  0,
  NULL,
  "Tests SystemLiteral. The systemLiteral for the element \"student\" has a single quote character in the middle."
},
{
  "not-wf/P11/ibm11n03.xml",
  209,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student SYSTEM \"student.DTD [\r\n\t<!ELEMENT student (#PCDATA)> \r\n]>\r\n\r\n<!-- testing invalid system literal with no closing bracket  -->\r\n<student>My Name is SnowMan. </student>\r\n",
  0,
  NULL,
  "Tests SystemLiteral. The closing bracket (double quote) is missing with the systemLiteral for the element \"student\"."
},
{
  "not-wf/P11/ibm11n04.xml",
  209,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student SYSTEM 'student.DTD [\r\n\t<!ELEMENT student (#PCDATA)> \r\n]>\r\n\r\n<!-- testing invalid system literal with no closing bracket  -->\r\n<student>My Name is SnowMan. </student>\r\n",
  0,
  NULL,
  "Tests SystemLiteral. The closing bracket (single quote) is missing with the systemLiteral for the element \"student\"."
},
{
  "not-wf/P12/ibm12n01.xml",
  243,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)> \r\n\t<!ENTITY info PUBLIC \"..\\info.dtd>\r\n]>\r\n\r\n<!-- testing invalid pubid literal with no closing bracket  -->\r\n<student>My Name is &info;. </student>\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n ",
  0,
  NULL,
  "Tests PubidLiteral. The closing bracket (double quote) is missing with the value of the PubidLiteral for the entity \"info\"."
},
{
  "not-wf/P12/ibm12n02.xml",
  201,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)> \r\n\t<!ENTITY info PUBLIC '..\\info'.dtd'>\r\n]>\r\n\r\n<!-- testing invalid pubid literal -->\r\n<student>My Name is &info;. </student>\r\n",
  0,
  NULL,
  "Tests PubidLiteral. The value of the PubidLiteral for the entity \"info\" has a single quote character in the middle.."
},
{
  "not-wf/P12/ibm12n03.xml",
  222,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)> \r\n\t<!ENTITY info PUBLIC '..\\info.dtd>\r\n]>\r\n\r\n<!-- testing invalid pubid literal with no closing bracket  -->\r\n<student>My Name is &info;. </student>",
  0,
  NULL,
  "Tests PubidLiteral. The closing bracket (single quote) is missing with the value of the PubidLiteral for the entity \"info\"."
},
{
  "not-wf/P13/ibm13n01.xml",
  202,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ENTITY info PUBLIC \"This is a {test} \" \"student.dtd\">\r\n]>\r\n\r\n<!-- testing invalid pubid char with {  -->\r\n<student>My Name is &info;. </student>\r\n\r\n\r\n\r\n\r\n ",
  0,
  NULL,
  "Tests PubidChar. The pubidChar of the PubidLiteral for the entity \"info\" contains the character \"{\"."
},
{
  "not-wf/P13/ibm13n02.xml",
  191,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ENTITY info PUBLIC \"This is a test~. \" \"student.dtd\">\r\n]>\r\n\r\n<!-- testing invalid pubid char with ~  -->\r\n<student>My Name is &info;. </student>",
  0,
  NULL,
  "Tests PubidChar. The pubidChar of the PubidLiteral for the entity \"info\" contains the character \"~\"."
},
{
  "not-wf/P13/ibm13n03.xml",
  206,
  "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n<!DOCTYPE student [\n\t<!ENTITY info PUBLIC \"This is a test \303\241 \" \"student.dtd\">\n]>\n\n<!-- testing invalid pubid char with \303\241  -->\n<student>My Name is &info;. </student>\n\n\241",
  0,
  NULL,
  "Tests PubidChar. The pubidChar of the PubidLiteral for the entity \"info\" contains the character double quote in the middle."
},
{
  "not-wf/P14/ibm14n01.xml",
  255,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)>\r\n\t<!ATTLIST student first CDATA #REQUIRED\r\n\t\t\t  last  CDATA #IMPLIED>\r\n]>\r\n\r\n<!-- testing invalid chardata string  -->\r\n<student first=\"Snow\">My name is Snow ]]> Man</student>\r\n\r\n\r\n",
  0,
  NULL,
  "Tests CharData. The content of the element \"student\" contains the sequence close-bracket close-bracket greater-than."
},
{
  "not-wf/P14/ibm14n02.xml",
  247,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)>\r\n\t<!ATTLIST student first CDATA #REQUIRED\r\n\t\t\t  last  CDATA #IMPLIED>\r\n]>\r\n\r\n<!-- testing invalid chardata string  -->\r\n<student first=\"Snow\">My name is Snow <Man </student>",
  0,
  NULL,
  "Tests CharData. The content of the element \"student\" contains the character \"less than\"."
},
{
  "not-wf/P14/ibm14n03.xml",
  248,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)>\r\n\t<!ATTLIST student first CDATA #REQUIRED\r\n\t\t\t  last  CDATA #IMPLIED>\r\n]>\r\n\r\n<!-- testing invalid chardata string  -->\r\n<student first=\"Snow\">My name is Snow&Man </student>\r\n",
  0,
  NULL,
  "Tests CharData. The content of the element \"student\" contains the character ampersand."
},
{
  "not-wf/P15/ibm15n01.xml",
  182,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)> \r\n]>\r\n\r\n<!-- testing invalid comment  -->\r\n<!------>\r\n<student>My Name is SnowMan. </student>\r\n\r\n\r\n\r\n\r\n\r\n\r\n ",
  0,
  NULL,
  "Tests comment. The text of the second comment contains the character \"-\"."
},
{
  "not-wf/P15/ibm15n02.xml",
  180,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)> \r\n]>\r\n\r\n<!-- testing invalid comment  -->\r\n<!-- Student's name ->\r\n<student>My Name is SnowMan. </student>",
  0,
  NULL,
  "Tests comment. The second comment has a wrong closing sequence \"-(greater than)\"."
},
{
  "not-wf/P15/ibm15n03.xml",
  180,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)> \r\n]>\r\n\r\n<!-- testing invalid comment  -->\r\n<!- student file-1 -->\r\n<student>My Name is SnowMan. </student>",
  0,
  NULL,
  "Tests comment. The second comment has a wrong beginning sequence \"(less than)!-\"."
},
{
  "not-wf/P15/ibm15n04.xml",
  196,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)> \r\n]>\r\n\r\n<!-- testing invalid comment  -->\r\n<!--student phone number 408-777-8888 \r\n<student>My Name is SnowMan. </student>",
  0,
  NULL,
  "Tests comment. The closing sequence is missing with the second comment."
},
{
  "not-wf/P16/ibm16n01.xml",
  214,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)> \r\n]>\r\n\r\n<!-- testing invalid PI with illegal sequence  -->\r\n<?MyInstruct This is ?> a test ?>\r\n<student>My Name is SnowMan. </student>\r\n\r\n\r\n",
  0,
  NULL,
  "Tests PI. The content of the PI includes the sequence \"?(greater than)?\"."
},
{
  "not-wf/P16/ibm16n02.xml",
  181,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)> \r\n]>\r\n\r\n\r\n<!-- testing invalid PI with missing PITarget  -->\r\n<??\?>\r\n<student>My Name is SnowMan. </student>",
  0,
  NULL,
  "Tests PI. The PITarget is missing in the PI."
},
{
  "not-wf/P16/ibm16n03.xml",
  197,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)> \r\n]>\r\n\r\n\r\n<!-- testing invalid PI with wrong closing sequence  -->\r\n<?MyInstruct >\r\n<student>My Name is SnowMan. </student>",
  0,
  NULL,
  "Tests PI. The PI has a wrong closing sequence \">\"."
},
{
  "not-wf/P16/ibm16n04.xml",
  190,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)> \r\n]>\r\n\r\n\r\n<!-- testing invalid PI with missing sequence  -->\r\n<?MyInstruct \r\n<student>My Name is SnowMan. </student>",
  0,
  NULL,
  "Tests PI. The closing sequence is missing in the PI."
},
{
  "not-wf/P17/ibm17n01.xml",
  190,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)> \r\n]>\r\n\r\n\r\n<!-- testing invalid PITarget  -->\r\n<?XML This is a test ?>\r\n<student>My Name is SnowMan. </student>\r\n\r\n\r\n",
  0,
  NULL,
  "Tests PITarget. The PITarget contains the string \"XML\"."
},
{
  "not-wf/P17/ibm17n02.xml",
  182,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)> \r\n]>\r\n\r\n<!-- testing invalid PITarget  -->\r\n<?xML This is a test ?>\r\n<student>My Name is SnowMan. </student>",
  0,
  NULL,
  "Tests PITarget. The PITarget contains the string \"xML\"."
},
{
  "not-wf/P17/ibm17n03.xml",
  182,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)> \r\n]>\r\n\r\n<!-- testing invalid PITarget  -->\r\n<?xml This is a test ?>\r\n<student>My Name is SnowMan. </student>",
  0,
  NULL,
  "Tests PITarget. The PITarget contains the string \"xml\"."
},
{
  "not-wf/P17/ibm17n04.xml",
  182,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)> \r\n]>\r\n\r\n<!-- testing invalid PITarget  -->\r\n<?xmL This is a test ?>\r\n<student>My Name is SnowMan. </student>",
  0,
  NULL,
  "Tests PITarget. The PITarget contains the string \"xmL\"."
},
{
  "not-wf/P18/ibm18n01.xml",
  207,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)> \r\n]>\r\n\r\n<!-- testing invalid CDSect with missing CDStart   -->\r\n<student>My Name is SnowMan. This is <normal> text]]></student>\r\n\r\n\r\n",
  0,
  NULL,
  "Tests CDSect. The CDStart is missing in the CDSect in the content of element \"student\"."
},
{
  "not-wf/P18/ibm18n02.xml",
  205,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)> \r\n]>\r\n\r\n<!-- testing invalid CDSect with missing CDEnd  -->\r\n<student>My Name is SnowMan. <![CDATA[This is <normal> text </student>",
  0,
  NULL,
  "Tests CDSect. The CDEnd is missing in the CDSect in the content of element \"student\"."
},
{
  "not-wf/P19/ibm19n01.xml",
  183,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)> \r\n]>\r\n\r\n<!-- testing invalid CDStart -->\r\n<![cdata[This is a test]]>\r\n<student>My Name is SnowMan. </student>",
  0,
  NULL,
  "Tests CDStart. The CDStart contains a lower case string \"cdata\"."
},
{
  "not-wf/P19/ibm19n02.xml",
  190,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)> \r\n]>\r\n\r\n<!-- testing invalid CDStart -->\r\n<![[CDATA[This is a test]]>\r\n<student>My Name is SnowMan. </student>\r\n\r\n\r\n",
  0,
  NULL,
  "Tests CDStart. The CDStart contains an extra character \"[\"."
},
{
  "not-wf/P19/ibm19n03.xml",
  183,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)> \r\n]>\r\n\r\n<!-- testing invalid CDStart -->\r\n<?[CDATA[This is a test]]>\r\n<student>My Name is SnowMan. </student>",
  0,
  NULL,
  "Tests CDStart. The CDStart contains a wrong character \"?\"."
},
{
  "not-wf/P20/ibm20n01.xml",
  226,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)> \r\n]>\r\n\r\n<!-- testing invalid CData with illegal sequence -->\r\n<![CDATA[<testing>This is ]]> a test</testing>]]>\r\n<student>My Name is SnowMan. </student>",
  0,
  NULL,
  "Tests CDATA with an illegal sequence. The CDATA contains the sequence close-bracket close-bracket greater-than."
},
{
  "not-wf/P21/ibm21n01.xml",
  187,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)> \r\n]>\r\n\r\n<!-- testing invalid CDEnd -->\r\n<![[CDATA[This is a test]>\r\n<student>My Name is SnowMan. </student>\r\n\r\n\r\n",
  0,
  NULL,
  "Tests CDEnd. One \"]\" is missing in the CDEnd."
},
{
  "not-wf/P21/ibm21n02.xml",
  182,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)> \r\n]>\r\n\r\n<!-- testing invalid CDEnd -->\r\n<![cdata[This is a test]]]>\r\n<student>My Name is SnowMan. </student>",
  0,
  NULL,
  "Tests CDEnd. An extra \"]\" is placed in the CDEnd."
},
{
  "not-wf/P21/ibm21n03.xml",
  181,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE student [\r\n\t<!ELEMENT student (#PCDATA)> \r\n]>\r\n\r\n<!-- testing invalid CDEnd -->\r\n<![CDATA[This is a test])>\r\n<student>My Name is SnowMan. </student>",
  0,
  NULL,
  "Tests CDEnd. A wrong character \")\" is placed in the CDEnd."
},
{
  "not-wf/P22/ibm22n01.xml",
  139,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc EMPTY>\r\n]>\r\n<?xml version=\"1.0\" encoding=\"ASCII\" ?>\r\n<doc/>\r\n<!-- Wrong ordering between DTD and XMLDecl -->",
  0,
  NULL,
  "Tests prolog with wrong field ordering. The XMLDecl occurs after the DTD."
},
{
  "not-wf/P22/ibm22n02.xml",
  139,
  "<!-- Wrong ordering Misc, DTD and XMLDecl -->\r\n<?xml version=\"1.0\" encoding=\"ASCII\" ?>\r\n<!DOCTYPE doc [\r\n<!ELEMENT doc EMPTY>\r\n]>\r\n<doc/>\r\n",
  0,
  NULL,
  "Tests prolog with wrong field ordering. The Misc (comment) occurs before the XMLDecl."
},
{
  "not-wf/P22/ibm22n03.xml",
  180,
  "<!-- Wrong ordering patter 4 -->\r\n<!DOCTYPE doc [\r\n<!ELEMENT doc EMPTY>\r\n]>\r\n<!-- Wrong ordering Misc, DTD, Misc, and XMLDecl -->\r\n<?xml version=\"1.0\" encoding=\"ASCII\" ?>\r\n<doc/>\r\n",
  0,
  NULL,
  "Tests prolog with wrong field ordering. The XMLDecl occurs after the DTD and a comment. The other comment occurs before the DTD."
},
{
  "not-wf/P23/ibm23n01.xml",
  135,
  "<?xml encoding=\"ASCII\" ?>\r\n<!DOCTYPE doc [\r\n   <!ELEMENT doc EMPTY>\r\n]>\r\n<doc/>\r\n<!-- Missing required field VersionInfo in XMLDecl -->",
  0,
  NULL,
  "Tests XMLDecl with a required field missing. The Versioninfo is missing in the XMLDecl."
},
{
  "not-wf/P23/ibm23n02.xml",
  165,
  "<?xml encoding='ASCII' version='1.0'?>\r\n<!DOCTYPE doc [\r\n   <!ELEMENT doc EMPTY>\r\n]>\r\n<doc/>\r\n<!-- Wrong ordering between VersionInfo and EncodingDecl in XMLDecl -->",
  0,
  NULL,
  "Tests XMLDecl with wrong field ordering. The VersionInfo occurs after the EncodingDecl."
},
{
  "not-wf/P23/ibm23n03.xml",
  183,
  "<?xml encoding='ASCII' standalone='yes' version='1.0'?>\r\n<!DOCTYPE doc [\r\n   <!ELEMENT doc EMPTY>\r\n]>\r\n<doc/>\r\n<!-- Wrong ordering EncodingDecl, SDDecl, and VersionInfo in XMLDecl -->",
  0,
  NULL,
  "Tests XMLDecl with wrong field ordering. The VersionInfo occurs after the SDDecl and the SDDecl occurs after the VersionInfo."
},
{
  "not-wf/P23/ibm23n04.xml",
  110,
  "<?XML version='1.0'?>\r\n<!DOCTYPE doc [\r\n   <!ELEMENT doc EMPTY>\r\n]>\r\n<doc/>\r\n<!-- Wrong keyword in XMLDecl -->",
  0,
  NULL,
  "Tests XMLDecl with wrong key word. An upper case string \"XML\" is used as the key word in the XMLDecl."
},
{
  "not-wf/P23/ibm23n05.xml",
  153,
  "<?xml version='1.0' encoding='ASCII' standalone='yes' >\r\n<!DOCTYPE doc [\r\n   <!ELEMENT doc EMPTY>\r\n]>\r\n<doc/>\r\n<!-- Wrong closing sequence in XMLDecl -->",
  0,
  NULL,
  "Tests XMLDecl with a wrong closing sequence \">\"."
},
{
  "not-wf/P23/ibm23n06.xml",
  154,
  "<!xml version='1.0' encoding='ASCII' standalone='yes' ?>\r\n<!DOCTYPE doc [\r\n   <!ELEMENT doc EMPTY>\r\n]>\r\n<doc/>\r\n<!-- Wrong opening sequence in XMLDecl -->",
  0,
  NULL,
  "Tests XMLDecl with a wrong opening sequence \"(less than)!\"."
},
{
  "not-wf/P24/ibm24n01.xml",
  117,
  "<?xml version= ?>\r\n<!DOCTYPE doc [\r\n   <!ELEMENT doc EMPTY>\r\n]>\r\n<doc/>\r\n<!-- VersioNum is missing in VersionInfo -->",
  0,
  NULL,
  "Tests VersionInfo with a required field missing. The VersionNum is missing in the VersionInfo in the XMLDecl."
},
{
  "not-wf/P24/ibm24n02.xml",
  113,
  "<?xmlversion='1.0' ?>\r\n<!DOCTYPE doc [\r\n   <!ELEMENT doc EMPTY>\r\n]>\r\n<doc/>\r\n<!-- S is missing in VersionInfo -->",
  0,
  NULL,
  "Tests VersionInfo with a required field missing. The white space is missing between the key word \"xml\" and the VersionInfo in the XMLDecl."
},
{
  "not-wf/P24/ibm24n03.xml",
  115,
  "<?xml version'1.0' ?>\r\n<!DOCTYPE doc [\r\n   <!ELEMENT doc EMPTY>\r\n]> \r\n<doc/>\r\n<!-- Eq is missing in VersionInfo -->",
  0,
  NULL,
  "Tests VersionInfo with a required field missing. The \"=\" (equal sign) is missing between the key word \"version\" and the VersionNum."
},
{
  "not-wf/P24/ibm24n04.xml",
  125,
  "<?xml '1.0'=version ?>\r\n<!DOCTYPE doc [\r\n   <!ELEMENT doc EMPTY>\r\n]>\r\n<doc/>\r\n<!-- Wrong ordering VersionNum Eq 'version' -->",
  0,
  NULL,
  "Tests VersionInfo with wrong field ordering. The VersionNum occurs before \"=\" and \"version\"."
},
{
  "not-wf/P24/ibm24n05.xml",
  123,
  "<?xml version'1.0'= ?>\r\n<!DOCTYPE doc [\r\n   <!ELEMENT doc EMPTY>\r\n]>\r\n<doc/>\r\n<!-- Wrong ordering version VersionNum Eq -->",
  0,
  NULL,
  "Tests VersionInfo with wrong field ordering. The \"=\" occurs after \"version\" and the VersionNum."
},
{
  "not-wf/P24/ibm24n06.xml",
  111,
  "<?xml Version='1.0' ?>\r\n<!DOCTYPE doc [\r\n   <!ELEMENT doc EMPTY>\r\n]>\r\n<doc/>\r\n<!-- Wrong key word 'Version' -->",
  0,
  NULL,
  "Tests VersionInfo with the wrong key word \"Version\"."
},
{
  "not-wf/P24/ibm24n07.xml",
  111,
  "<?xml versioN='1.0' ?>\r\n<!DOCTYPE doc [\r\n   <!ELEMENT doc EMPTY>\r\n]>\r\n<doc/>\r\n<!-- Wrong key word 'versioN' -->",
  0,
  NULL,
  "Tests VersionInfo with the wrong key word \"versioN\"."
},
{
  "not-wf/P24/ibm24n08.xml",
  118,
  "<?xml version='1.0\" ?>\r\n<!DOCTYPE doc [\r\n   <!ELEMENT doc EMPTY>\r\n]>\r\n<doc/>\r\n<!-- Mismatched qotes in VersionInfo -->",
  0,
  NULL,
  "Tests VersionInfo with mismatched quotes around the VersionNum. version = '1.0\" is used as the VersionInfo."
},
{
  "not-wf/P24/ibm24n09.xml",
  117,
  "<?xml version='1.0 ?>\r\n<!DOCTYPE doc [\r\n   <!ELEMENT doc EMPTY>\r\n]>\r\n<doc/>\r\n<!-- Mismatched qotes in VersionInfo -->",
  0,
  NULL,
  "Tests VersionInfo with mismatched quotes around the VersionNum. The closing bracket for the VersionNum is missing."
},
{
  "not-wf/P25/ibm25n01.xml",
  113,
  "<?xml version=='1.0' ?>\r\n<!DOCTYPE doc [\r\n   <!ELEMENT doc EMPTY>\r\n]>\r\n<doc/>\r\n<!-- Wrong key word \"==\" in Eq -->",
  0,
  NULL,
  "Tests eq with a wrong key word \"==\"."
},
{
  "not-wf/P25/ibm25n02.xml",
  115,
  "<?xml version eq '1.0' ?>\r\n<!DOCTYPE doc [\r\n   <!ELEMENT doc EMPTY>\r\n]>\r\n<doc/>\r\n<!-- Wrong key word \"eq\" in Eq -->",
  0,
  NULL,
  "Tests eq with a wrong key word \"eq\"."
},
{
  "not-wf/P26/ibm26n01.xml",
  124,
  "<?xml version='_#1.0' ?>\r\n<!DOCTYPE doc [\r\n   <!ELEMENT doc EMPTY>\r\n]>\r\n<doc/>\r\n<!-- illegal character '#' in VersionNum -->",
  0,
  NULL,
  "Tests VersionNum with an illegal character \"#\"."
},
{
  "not-wf/P27/ibm27n01.xml",
  155,
  "<?xml version=\"1.0\" ?>\r\n<!DOCTYPE animal [\r\n   <!ELEMENT animal ANY>\r\n]>\r\n<animal>Wrong type of Misc following this element!</animal>\r\n<!ELEMENT cat EMPTY>",
  0,
  NULL,
  "Tests type of Misc. An element declaration is used as a type of Misc After the element \"animal\"."
},
{
  "not-wf/P28/ibm28n01.xml",
  127,
  "<?xml version=\"1.0\" encoding='UTF-8'?>\r\n<!DOCTYPE SYSTEM \"ibm28n01.dtd\">\r\n<!-- Name is missing in doctypedecl --> \r\n<animal/>\r\n",
  0,
  NULL,
  "Tests doctypedecl with a required field missing. The Name \"animal\" is missing in the doctypedecl."
},
{
  "not-wf/P28/ibm28n02.xml",
  150,
  "<?xml version=\"1.0\" encoding='UTF-8'?>\r\n<!DOCTYPE [\r\n<!ELEMENT animal EMPTY>\r\n] animal>\r\n<!-- Wrong ordering [ ] Name in doctypedecl --> \r\n<animal/>\r\n",
  0,
  NULL,
  "Tests doctypedecl with wrong field ordering. The Name \"animal\" occurs after the markup declarations inside the \"[]\"."
},
{
  "not-wf/P28/ibm28n03.xml",
  217,
  "<?xml version=\"1.0\" encoding='UTF-8'?>\r\n<!DOCTYPE SYSTEM \"ibm28n01.dtd\" animal [\r\n   <!ATTLIST animal color CDATA #REQUIRED>\r\n]>\r\n<!-- Wrong ordering ExternalID Name [ ] in doctypedecl --> \r\n<animal color=\"yellow\"/>\r\n",
  0,
  NULL,
  "Tests doctypedecl with wrong field ordering. The Name \"animal\" occurs after the markup declarations inside the \"[]\"."
},
{
  "not-wf/P28/ibm28n04.xml",
  380,
  "<?xml version=\"1.0\" encoding='UTF-8'?>\r\n<!DOCTYPE aniaml [\r\n   <!ELEMENT animal ANY>\r\n   <!ENTITY generalE \"leopard\">\r\n   &generalE;\r\n   <!ENTITY % parameterE \"<!ELEMENT leopard EMPTY>\">\r\n   %parameterE;\r\n] animal>\r\n<!-- Wrong componet general entity reference occurs inside the DTD -->\r\n<!-- General entity sould be used in the document content --> \r\n<animal>&generalE</animal>\r\n",
  0,
  NULL,
  "Tests doctypedecl with general entity reference.The \"(ampersand)generalE\" occurs in the DTD."
},
{
  "not-wf/P28/ibm28n05.xml",
  151,
  "<?xml version=\"1.0\" encoding='UTF-8'?>\r\n<!DOCtYPE animal [\r\n   <!ELEMENT animal EMPTY>\r\n]>\r\n<!-- Wrong keyword DOCTYPE in doctypedecl --> \r\n<animal/>\r\n",
  0,
  NULL,
  "Tests doctypedecl with wrong key word. A wrong key word \"DOCtYPE\" occurs on line 2."
},
{
  "not-wf/P28/ibm28n06.xml",
  152,
  "<?xml version=\"1.0\" encoding='UTF-8'?>\r\n<!DOCTYPE animal [\r\n   <!ELEMENT animal EMPTY>\r\n>\r\n<!-- Bracket mismatch in [ ] in doctypedecl --> \r\n<animal/>\r\n",
  0,
  NULL,
  "Tests doctypedecl with mismatched brackets. The closing bracket \"]\" of the DTD is missing."
},
{
  "not-wf/P28/ibm28n07.xml",
  150,
  "<?xml version=\"1.0\" encoding='UTF-8'?>\r\n<!DOCTYPE animal {\r\n   <!ELEMENT animal EMPTY>\r\n]>\r\n<!-- Wrong bracket in [ ] in doctypedecl --> \r\n<animal/>\r\n",
  0,
  NULL,
  "Tests doctypedecl with wrong bracket. The opening bracket \"{\" occurs in the DTD."
},
{
  "not-wf/P28/ibm28n08.xml",
  152,
  "<?xml version=\"1.0\" encoding='UTF-8'?>\r\n<?DOCTYPE animal [\r\n   <!ELEMENT animal EMPTY>\r\n]>\r\n<!-- Wrong opening sequence in doctypedecl --> \r\n<animal/>\r\n",
  0,
  NULL,
  "Tests doctypedecl with wrong opening sequence. The opening sequence \"(less than)?DOCTYPE\" occurs in the DTD."
},
{
  "not-wf/p28a/ibm28an01.xml",
  617,
  "<?xml version=\"1.0\" encoding=\"utf-8\" ?>\r\n<!DOCTYPE animal SYSTEM \"ibm28an01.dtd\" [\r\n   <!ELEMENT animal (cat|tiger|leopard)+>\r\n   <!NOTATION animal_class SYSTEM \"ibm29v01.txt\">\r\n   <!ELEMENT cat ANY>\r\n   <!ENTITY forcat \"This is a small cat\">\r\n   <!ELEMENT tiger (#PCDATA)>\r\n   <!ELEMENT small EMPTY>\r\n   <!ELEMENT big EMPTY>\r\n   <!ATTLIST tiger color CDATA #REQUIRED>\r\n   <?sound \"This is a PI\" ?>\r\n   <!-- This is a comment -->\r\n]>\r\n<animal>\r\n   <cat>&forcat;</cat>\r\n   <tiger color=\"white\">This is a white tiger in Mirage!!</tiger>\r\n   <cat/>\r\n   <leopard>\r\n      <small/>\r\n      <big/>\r\n   </leopard>\r\n</animal>\r\n",
  0,
  NULL,
  "This test violates WFC:PE Between Declarations in Production 28a. The last character of a markup declaration is not contained in the same parameter-entity text replacement."
},
{
  "not-wf/P29/ibm29n01.xml",
  498,
  "<!DOCTYPE animal [\r\n   <?xml version=\"1.0\" encoding=\"ASCII\" ?>\r\n   <!-- Illegal markupdecl in DTD --> \r\n   <!ELEMENT animal (cat|tiger|leopard)+>\r\n   <!ELEMENT cat EMPTY>\r\n   <!ELEMENT tiger (#PCDATA)>\r\n   <!ELEMENT leopard ANY>\r\n   <!ELEMENT small EMPTY>\r\n   <!ELEMENT big EMPTY>\r\n   <!ATTLIST tiger color CDATA #REQUIRED>\r\n]>\r\n<animal>\r\n   <cat/>\r\n   <tiger color=\"white\">This is a white tiger in Mirage!!</tiger>\r\n   <cat/>\r\n   <leopard>\r\n      <small/>\r\n      <big/>\r\n   </leopard>\r\n</animal>\r\n",
  0,
  NULL,
  "Tests markupdecl with an illegal markup declaration. A XMLDecl occurs inside the DTD."
},
{
  "not-wf/P29/ibm29n02.xml",
  255,
  "<?xml version=\"1.0\" encoding='UTF-8'?>\r\n<!DOCTYPE animal [\r\n   <!ELEMENT animal ANY>\r\n   <!ENTITY % parameterE \"leopard EMPTY>\">\r\n   <!ELEMENT %parameterE;\r\n]>\r\n<!-- Parameter reference appears inside elementdecl in DTD -->\r\n<animal>Any content</animal>\r\n",
  0,
  NULL,
  "Tests WFC \"PEs in Internal Subset\". A PE reference occurs inside an elementdecl in the DTD."
},
{
  "not-wf/P29/ibm29n03.xml",
  269,
  "<?xml version=\"1.0\" encoding='UTF-8'?>\r\n<!DOCTYPE animal [\r\n   <!ELEMENT animal ANY>\r\n   <!ENTITY % parameterE \"color\">\r\n   <!ATTLIST animal %parameterE; CDATA #IMPLIED>\r\n]>\r\n<!-- Parameter reference appears inside AttlistDecl in DTD -->\r\n<animal>Any content</animal>\r\n",
  0,
  NULL,
  "Tests WFC \"PEs in Internal Subset\". A PE reference occurs inside an ATTlistDecl in the DTD."
},
{
  "not-wf/P29/ibm29n04.xml",
  268,
  "<?xml version=\"1.0\" encoding='UTF-8'?>\r\n<!DOCTYPE animal [\r\n   <!ELEMENT animal ANY>\r\n   <!ENTITY % parameterE \"A leopard\">\r\n   <!ENTITY content \"%parameterE;\">\r\n]>\r\n<!-- Parameter reference appears inside an entity declaration in DTD -->\r\n<animal>&content;</animal>\r\n",
  0,
  NULL,
  "Tests WFC \"PEs in Internal Subset\". A PE reference occurs inside an EntityDecl in the DTD."
},
{
  "not-wf/P29/ibm29n05.xml",
  247,
  "<?xml version=\"1.0\" encoding='UTF-8'?>\r\n<!DOCTYPE animal [\r\n   <!ELEMENT animal ANY>\r\n   <!ENTITY % parameterE \"A music file ?>\">\r\n   <?music %parameterE;\r\n]>\r\n<!-- Parameter reference appears inside a PI in DTD -->\r\n<animal>Any content</animal>\r\n",
  0,
  NULL,
  "Tests WFC \"PEs in Internal Subset\". A PE reference occurs inside a PI in the DTD."
},
{
  "not-wf/P29/ibm29n06.xml",
  258,
  "<?xml version=\"1.0\" encoding='UTF-8'?>\r\n<!DOCTYPE animal [\r\n   <!ELEMENT animal ANY>\r\n   <!ENTITY % parameterE \"A music file -->\">\r\n<!-- Parameter reference appears inside a comment in DTD -->\r\n   <!-- This is %parameterE;\r\n]>\r\n<animal>Any content</animal>\r\n",
  0,
  NULL,
  "Tests WFC \"PEs in Internal Subset\". A PE reference occurs inside a comment in the DTD."
},
{
  "not-wf/P29/ibm29n07.xml",
  266,
  "<?xml version=\"1.0\" encoding='UTF-8'?>\r\n<!DOCTYPE animal [\r\n   <!ELEMENT animal ANY>\r\n   <!ENTITY % parameterE \"cat SYSTEM\">\r\n   <!NOTATION %parameterE; \"cat.txt\">\r\n]>\r\n<!-- Parameter reference appears inside a NotationDecl in DTD -->\r\n<animal>Any content</animal>\r\n",
  0,
  NULL,
  "Tests WFC \"PEs in Internal Subset\". A PE reference occurs inside a NotationDecl in the DTD."
},
{
  "not-wf/P30/ibm30n01.xml",
  118,
  "<!DOCTYPE animal SYSTEM \"ibm30n01.dtd\">\r\n<animal/>\r\n<!-- Wrong ordering extSubsetDecl TextDecl in the external DTD -->",
  0,
  NULL,
  "Tests extSubset with wrong field ordering. In the file \"ibm30n01.dtd\", the TextDecl occurs after the extSubsetDecl (the element declaration)."
},
{
  "not-wf/P31/ibm31n01.xml",
  102,
  "<!DOCTYPE animal SYSTEM \"ibm31n01.dtd\">\r\n<animal/>\r\n<!-- Illegal extSubsetDecl in the external DTD -->",
  0,
  NULL,
  "Tests extSubsetDecl with an illegal field. A general entity reference occurs in file \"ibm31n01.dtd\"."
},
{
  "not-wf/P32/ibm32n01.xml",
  135,
  "<?xml version=\"1.0\"standalone=\"yes\" ?>\r\n<!DOCTYPE animal [\r\n   <!ELEMENT animal EMPTY>\r\n]>\r\n<!-- Missing a S in SDDecl -->\r\n<animal/>\r\n",
  0,
  NULL,
  "Tests SDDecl with a required field missing. The leading white space is missing with the SDDecl in the XMLDecl."
},
{
  "not-wf/P32/ibm32n02.xml",
  134,
  "<?xml version=\"1.0\" standalone\"yes\" ?>\r\n<!DOCTYPE animal [\r\n   <!ELEMENT animal EMPTY>\r\n]>\r\n<!-- Missing Eq in SDDecl -->\r\n<animal/>\r\n",
  0,
  NULL,
  "Tests SDDecl with a required field missing. The \"=\" sign is missing in the SDDecl in the XMLDecl."
},
{
  "not-wf/P32/ibm32n03.xml",
  138,
  "<?xml version=\"1.0\" Standalone=\"yes\" ?>\r\n<!DOCTYPE animal [\r\n   <!ELEMENT animal EMPTY>\r\n]>\r\n<!-- Wrong keyword in SDDecl -->\r\n<animal/>\r\n",
  0,
  NULL,
  "Tests SDDecl with wrong key word. The word \"Standalone\" occurs in the SDDecl in the XMLDecl."
},
{
  "not-wf/P32/ibm32n04.xml",
  138,
  "<?xml version=\"1.0\" standalone=\"Yes\" ?>\r\n<!DOCTYPE animal [\r\n   <!ELEMENT animal EMPTY>\r\n]>\r\n<!-- Wrong keyword in SDDecl -->\r\n<animal/>\r\n",
  0,
  NULL,
  "Tests SDDecl with wrong key word. The word \"Yes\" occurs in the SDDecl in the XMLDecl."
},
{
  "not-wf/P32/ibm32n05.xml",
  138,
  "<?xml version=\"1.0\" standalone=\"YES\" ?>\r\n<!DOCTYPE animal [\r\n   <!ELEMENT animal EMPTY>\r\n]>\r\n<!-- Wrong keyword in SDDecl -->\r\n<animal/>\r\n",
  0,
  NULL,
  "Tests SDDecl with wrong key word. The word \"YES\" occurs in the SDDecl in the XMLDecl."
},
{
  "not-wf/P32/ibm32n06.xml",
  126,
  "<?xml version=\"1.0\" standalone=\"No\" ?>\r\n<!DOCTYPE animal SYSTEM \"ibm32n06.dtd\">\r\n<!-- Wrong keyword in SDDecl -->\r\n<animal/>\r\n",
  0,
  NULL,
  "Tests SDDecl with wrong key word. The word \"No\" occurs in the SDDecl in the XMLDecl."
},
{
  "not-wf/P32/ibm32n07.xml",
  126,
  "<?xml version=\"1.0\" standalone=\"NO\" ?>\r\n<!DOCTYPE animal SYSTEM \"ibm32n06.dtd\">\r\n<!-- Wrong keyword in SDDecl -->\r\n<animal/>\r\n",
  0,
  NULL,
  "Tests SDDecl with wrong key word. The word \"NO\" occurs in the SDDecl in the XMLDecl."
},
{
  "not-wf/P32/ibm32n08.xml",
  139,
  "<?xml version=\"1.0\" standalone\"Yes\"= ?>\r\n<!DOCTYPE animal [\r\n   <!ELEMENT animal EMPTY>\r\n]>\r\n<!-- Wrong ordering in SDDecl -->\r\n<animal/>\r\n",
  0,
  NULL,
  "Tests SDDecl with wrong field ordering. The \"=\" sign occurs after the key word \"yes\" in the SDDecl in the XMLDecl."
},
{
  "not-wf/P32/ibm32n09.xml",
  423,
  "<?xml version=\"1.0\" standalone=\"yes\" ?>\r\n<!DOCTYPE animal SYSTEM \"ibm32n09.dtd\" [\r\n   <!ELEMENT animal (#PCDATA)>\r\n]>\r\n<!-- This is test violates WFC: Entity Declared in P68\r\n The standalone document declaration has the value \"yes\", there is an \r\n external markup declaration of an entity (other than amp, lt, gt, apos, quot), and references to this entity appear in the document. \r\n-->\r\n<animal>&animal_content;</animal>\r\n",
  0,
  NULL,
  "This is test violates WFC: Entity Declared in P68. The standalone document declaration has the value yes, BUT there is an external markup declaration of an entity (other than amp, lt, gt, apos, quot), and references to this entity appear in the document."
},
{
  "not-wf/P39/ibm39n01.xml",
  97,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n]>\r\n<root>missing end tag\r\n\r\n",
  0,
  NULL,
  "Tests element with a required field missing. The ETag is missing for the element \"root\"."
},
{
  "not-wf/P39/ibm39n02.xml",
  98,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n]>\r\nmissing start tag</root>\r\n",
  0,
  NULL,
  "Tests element with a required field missing. The STag is missing for the element \"root\"."
},
{
  "not-wf/P39/ibm39n03.xml",
  122,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n]>\r\n<root>\r\n<!--* Missing end tag and no content -->\r\n",
  0,
  NULL,
  "Tests element with required fields missing. Both the content and the ETag are missing in the element \"root\"."
},
{
  "not-wf/P39/ibm39n04.xml",
  129,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n]>\r\n</root>\r\n<!--* Missing start tag and no content -->\r\n\r\n\r\n",
  0,
  NULL,
  "Tests element with required fields missing. Both the content and the STag are missing in the element \"root\"."
},
{
  "not-wf/P39/ibm39n05.xml",
  114,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n]>\r\n</root>switched start and end tags<root>\r\n",
  0,
  NULL,
  "Tests element with wrong field ordering. The STag and the ETag are swapped in the element \"root\"."
},
{
  "not-wf/P39/ibm39n06.xml",
  108,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n]>\r\n<root></root>content after end tag\r\n",
  0,
  NULL,
  "Tests element with wrong field ordering. The content occurs after the ETag of the element \"root\"."
},
{
  "not-wf/P40/ibm40n01.xml",
  195,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root ANY>\r\n<!ATTLIST root attr1 CDATA #IMPLIED>\r\n<!ATTLIST root attr2 CDATA #IMPLIED>\r\n]>\r\n<attr1=\"any\">missing name in start tag</root>\r\n\r\n\r\n\r\n",
  0,
  NULL,
  "Tests STag with a required field missing. The Name \"root\" is in the STag of the element \"root\"."
},
{
  "not-wf/P40/ibm40n02.xml",
  200,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root ANY>\r\n<!ATTLIST root attr1 CDATA #IMPLIED>\r\n<!ATTLIST root attr2 CDATA #IMPLIED>\r\n]>\r\n<rootattr1=\"any\">missing white space in start tag</root>\r\n",
  0,
  NULL,
  "Tests STag with a required field missing. The white space between the Name \"root\" and the attribute \"attr1\" is missing in the STag of the element \"root\"."
},
{
  "not-wf/P40/ibm40n03.xml",
  196,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root ANY>\r\n<!ATTLIST root attr1 CDATA #IMPLIED>\r\n<!ATTLIST root attr2 CDATA #IMPLIED>\r\n]>\r\n<attr1=\"any\" root>Wrong ordering in start tag</root>\r\n",
  0,
  NULL,
  "Tests STag with wrong field ordering. The Name \"root\" occurs after the attribute \"attr1\" in the STag of the element \"root\"."
},
{
  "not-wf/P40/ibm40n04.xml",
  206,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root ANY>\r\n<!ATTLIST root attr1 CDATA #IMPLIED>\r\n<!ATTLIST root attr2 CDATA #IMPLIED>\r\n]>\r\n<!root attr1=\"any\">wrong begining sequence in start tag</root>\r\n",
  0,
  NULL,
  "Tests STag with a wrong opening sequence. The string \"(less than)!\" is used as the opening sequence for the STag of the element \"root\"."
},
{
  "not-wf/P40/ibm40n05.xml",
  220,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root ANY>\r\n<!ATTLIST root attr1 CDATA #IMPLIED>\r\n<!ATTLIST root attr2 CDATA #IMPLIED>\r\n]>\r\n<root attr1=\"any1\" attr1=\"any2\">duplicate attr names in start tag</root>\r\n\r\n\r\n",
  0,
  NULL,
  "Tests STag with duplicate attribute names. The attribute name \"attr1\" occurs twice in the STag of the element \"root\"."
},
{
  "not-wf/P41/ibm41n01.xml",
  189,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root ANY>\r\n<!ATTLIST root attr1 CDATA #IMPLIED>\r\n<!ATTLIST root attr2 CDATA #IMPLIED>\r\n]>\r\n<root =\"any\">missing name in Attribute</root>\r\n",
  0,
  NULL,
  "Tests Attribute with a required field missing. The attribute name is missing in the Attribute in the STag of the element \"root\"."
},
{
  "not-wf/P41/ibm41n02.xml",
  191,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root ANY>\r\n<!ATTLIST root attr1 CDATA #IMPLIED>\r\n<!ATTLIST root attr2 CDATA #IMPLIED>\r\n]>\r\n<root attr1\"any\">missing Eq in Attribute</root>\r\n",
  0,
  NULL,
  "Tests Attribute with a required field missing. The \"=\" is missing between the attribute name and the attribute value in the Attribute in the STag of the element \"root\"."
},
{
  "not-wf/P41/ibm41n03.xml",
  194,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root ANY>\r\n<!ATTLIST root attr1 CDATA #IMPLIED>\r\n<!ATTLIST root attr2 CDATA #IMPLIED>\r\n]>\r\n<root attr1= >missing AttValue in Attribute</root>\r\n",
  0,
  NULL,
  "Tests Attribute with a required field missing. The AttValue is missing in the Attribute in the STag of the element \"root\"."
},
{
  "not-wf/P41/ibm41n04.xml",
  195,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root ANY>\r\n<!ATTLIST root attr1 CDATA #IMPLIED>\r\n<!ATTLIST root attr2 CDATA #IMPLIED>\r\n]>\r\n<root \"any\">missing name and Eq in Attribute</root>\r\n",
  0,
  NULL,
  "Tests Attribute with a required field missing. The Name and the \"=\" are missing in the Attribute in the STag of the element \"root\"."
},
{
  "not-wf/P41/ibm41n05.xml",
  199,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root ANY>\r\n<!ATTLIST root attr1 CDATA #IMPLIED>\r\n<!ATTLIST root attr2 CDATA #IMPLIED>\r\n]>\r\n<root attr1>missing Eq and AttValue in Attribute</root>\r\n",
  0,
  NULL,
  "Tests Attribute with a required field missing. The \"=\" and the AttValue are missing in the Attribute in the STag of the element \"root\"."
},
{
  "not-wf/P41/ibm41n06.xml",
  198,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root ANY>\r\n<!ATTLIST root attr1 CDATA #IMPLIED>\r\n<!ATTLIST root attr2 CDATA #IMPLIED>\r\n]>\r\n<root = >missing Name and AttValue in Attribute</root>\r\n",
  0,
  NULL,
  "Tests Attribute with a required field missing. The Name and the AttValue are missing in the Attribute in the STag of the element \"root\"."
},
{
  "not-wf/P41/ibm41n07.xml",
  196,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root ANY>\r\n<!ATTLIST root attr1 CDATA #IMPLIED>\r\n<!ATTLIST root attr2 CDATA #IMPLIED>\r\n]>\r\n<root attr1\"any\"=>wrong ordering in Attribute</root>\r\n",
  0,
  NULL,
  "Tests Attribute with wrong field ordering. The \"=\" occurs after the Name and the AttValue in the Attribute in the STag of the element \"root\"."
},
{
  "not-wf/P41/ibm41n08.xml",
  196,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root ANY>\r\n<!ATTLIST root attr1 CDATA #IMPLIED>\r\n<!ATTLIST root attr2 CDATA #IMPLIED>\r\n]>\r\n<root \"any\"=attr1>wrong ordering in Attribute</root>\r\n",
  0,
  NULL,
  "Tests Attribute with wrong field ordering. The Name and the AttValue are swapped in the Attribute in the STag of the element \"root\"."
},
{
  "not-wf/P41/ibm41n09.xml",
  196,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root ANY>\r\n<!ATTLIST root attr1 CDATA #IMPLIED>\r\n<!ATTLIST root attr2 CDATA #IMPLIED>\r\n]>\r\n<root =attr1\"any\">wrong ordering in Attribute</root>\r\n",
  0,
  NULL,
  "Tests Attribute with wrong field ordering. The \"=\" occurs before the Name and the AttValue in the Attribute in the STag of the element \"root\"."
},
{
  "not-wf/P41/ibm41n10.xml",
  270,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root ANY>\r\n<!ATTLIST root attr1 CDATA #IMPLIED>\r\n<!ATTLIST root attr2 CDATA #IMPLIED>\r\n<!ENTITY aExternal SYSTEM \"ibm41n10.ent\">\r\n]>\r\n<root attr1=\"&aExternal;\">direct reference to external entinity in Attribute</root>\r\n",
  0,
  NULL,
  "Tests Attribute against WFC \"no external entity references\". A direct reference to the external entity \"aExternal\" is contained in the value of the attribute \"attr1\"."
},
{
  "not-wf/P41/ibm41n11.xml",
  307,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root ANY>\r\n<!ATTLIST root attr1 CDATA #IMPLIED>\r\n<!ATTLIST root attr2 CDATA #IMPLIED>\r\n<!ENTITY aExternal SYSTEM \"ibm41n11.ent\">\r\n<!ENTITY aIndirect \"&aExternal;\">\r\n]>\r\n<root attr1=\"&aIndirect;\">indirect reference to external entinity in Attribute</root>\r\n",
  0,
  NULL,
  "Tests Attribute against WFC \"no external entity references\". A indirect reference to the external entity \"aExternal\" is contained in the value of the attribute \"attr1\"."
},
{
  "not-wf/P41/ibm41n12.xml",
  330,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root ANY>\r\n<!ATTLIST root attr1 CDATA #IMPLIED>\r\n<!ATTLIST root attr2 CDATA #IMPLIED>\r\n<!NOTATION JPGformat SYSTEM \"JPGFormat\">\r\n<!ENTITY aImage SYSTEM \"image.jpg\" NDATA JPGformat>\r\n]>\r\n<root attr1=\"&aImage;\">direct reference to external unparsed entinity in Attribute</root>\r\n\r\n",
  0,
  NULL,
  "Tests Attribute against WFC \"no external entity references\". A direct reference to the external unparsed entity \"aImage\" is contained in the value of the attribute \"attr1\"."
},
{
  "not-wf/P41/ibm41n13.xml",
  301,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root ANY>\r\n<!ATTLIST root attr1 CDATA #IMPLIED>\r\n<!ATTLIST root attr2 CDATA #IMPLIED>\r\n<!ENTITY withlt \"have <lessthan> inside\">\r\n]>\r\n<root attr1=\"&withlt;\">Direct reference to an entity with &lt; as part of its replacement text in Attribute</root>\r\n",
  0,
  NULL,
  "Tests Attribute against WFC \"No (less than) character in Attribute Values\". The character \"less than\" is contained in the value of the attribute \"attr1\"."
},
{
  "not-wf/P41/ibm41n14.xml",
  338,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root ANY>\r\n<!ATTLIST root attr1 CDATA #IMPLIED>\r\n<!ATTLIST root attr2 CDATA #IMPLIED>\r\n<!ENTITY withlt \"have <lessthan> inside\">\r\n<!ENTITY aIndirect \"&withlt;\">\r\n]>\r\n<root attr1=\"&aIndirect;\">indirect reference to an entity with &lt; as part of its replacement text in Attribute</root>\r\n",
  0,
  NULL,
  "Tests Attribute against WFC \"No (less than) in Attribute Values\". The character \"less than\" is contained in the value of the attribute \"attr1\" through indirect internal entity reference."
},
{
  "not-wf/P42/ibm42n01.xml",
  147,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root ANY>\r\n<!ATTLIST root attr1 CDATA #IMPLIED>\r\n]>\r\n<root attr1=\"any\">missing Name in ETag</>\r\n",
  0,
  NULL,
  "Tests ETag with a required field missing. The Name is missing in the ETag of the element \"root\"."
},
{
  "not-wf/P42/ibm42n02.xml",
  163,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root ANY>\r\n<!ATTLIST root attr1 CDATA #IMPLIED>\r\n]>\r\n<root attr1=\"any\">Wrong begining sequence in ETag <\\root>\r\n",
  0,
  NULL,
  "Tests ETag with a wrong beginning sequence. The string \"(less than)\\\" is used as a beginning sequence of the ETag of the element \"root\"."
},
{
  "not-wf/P42/ibm42n03.xml",
  162,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root ANY>\r\n<!ATTLIST root attr1 CDATA #IMPLIED>\r\n]>\r\n<root attr1=\"any\">Wrong begining sequence in ETag <root>\r\n",
  0,
  NULL,
  "Tests ETag with a wrong beginning sequence. The string \"less than\" is used as a beginning sequence of the ETag of the element \"root\"."
},
{
  "not-wf/P42/ibm42n04.xml",
  170,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root ANY>\r\n<!ATTLIST root attr1 CDATA #IMPLIED>\r\n]>\r\n<root attr1=\"any\">Extra white space before Name in ETag </ root>\r\n",
  0,
  NULL,
  "Tests ETag with a wrong structure. An white space occurs between The beginning sequence and the Name of the ETag of the element \"root\"."
},
{
  "not-wf/P42/ibm42n05.xml",
  150,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root ANY>\r\n<!ATTLIST root attr1 CDATA #IMPLIED>\r\n]>\r\n<root> Attribute in ETag </root attr1=\"any\">\r\n",
  0,
  NULL,
  "Tests ETag with a wrong structure. The ETag of the element \"root\" contains an Attribute (attr1=\"any\")."
},
{
  "not-wf/P43/ibm43n01.xml",
  204,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!ENTITY % paaa \"a string\">\r\n]>\r\n<root>\r\n<!NOTATION nota1 SYSTEM \"bogus.not\">\r\n</root>\r\n<!--* NotationDecl in content -->",
  0,
  NULL,
  "Tests element content with a wrong option. A NotationDecl is used as the content of the element \"root\"."
},
{
  "not-wf/P43/ibm43n02.xml",
  188,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!ENTITY % paaa \"a string\">\r\n]>\r\n<root>\r\n<!ELEMENT ele1 ANY>\r\n</root>\r\n<!--* ElementDecl in content -->\r\n",
  0,
  NULL,
  "Tests element content with a wrong option. An elementdecl is used as the content of the element \"root\"."
},
{
  "not-wf/P43/ibm43n04.xml",
  213,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!ENTITY % paaa \"a string\">\r\n]>\r\n<root>\r\n<!ENTITY GE1 \"This is an entity declaration\">\r\n</root>\r\n<!--* EntityDecl in content -->\r\n",
  0,
  NULL,
  "Tests element content with a wrong option. An entitydecl is used as the content of the element \"root\"."
},
{
  "not-wf/P43/ibm43n05.xml",
  202,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!ENTITY % paaa \"a string\">\r\n]>\r\n<root>\r\n<!ATTLIST root attr1 ID #IMPLIED>\r\n</root>\r\n<!--* AttlistDecl in content -->\r\n",
  0,
  NULL,
  "Tests element content with a wrong option. An AttlistDecl is used as the content of the element \"root\"."
},
{
  "not-wf/P44/ibm44n01.xml",
  202,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root EMPTY>\r\n<!ATTLIST root attr1 CDATA #IMPLIED>\r\n<!ATTLIST root attr2 CDATA #IMPLIED>\r\n]>\r\n< />\r\n<!--* Missing Name and Attribute EmptyElemTag *-->\r\n",
  0,
  NULL,
  "Tests EmptyElemTag with a required field missing. The Name \"root\" is missing in the EmptyElemTag."
},
{
  "not-wf/P44/ibm44n02.xml",
  231,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root EMPTY>\r\n<!ATTLIST root attr1 CDATA #IMPLIED>\r\n<!ATTLIST root attr2 CDATA #IMPLIED>\r\n]>\r\n<attr1=\"any\" root/>\r\n<!--* Swithech positions of Name and Attribute EmptyElemTag *-->\r\n",
  0,
  NULL,
  "Tests EmptyElemTag with wrong field ordering. The Attribute (attri1 = \"any\") occurs before the name of the element \"root\" in the EmptyElemTag."
},
{
  "not-wf/P44/ibm44n03.xml",
  224,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root EMPTY>\r\n<!ATTLIST root attr1 CDATA #IMPLIED>\r\n<!ATTLIST root attr2 CDATA #IMPLIED>\r\n]>\r\n<root attr1=\"any\"\\>\r\n<!--* Wrong closing sequence in EmptyElemTag *-->\r\n\r\n\r\n\r\n\r\n",
  0,
  NULL,
  "Tests EmptyElemTag with wrong closing sequence. The string \"\\>\" is used as the closing sequence in the EmptyElemtag of the element \"root\"."
},
{
  "not-wf/P44/ibm44n04.xml",
  232,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root EMPTY>\r\n<!ATTLIST root attr1 CDATA #IMPLIED>\r\n<!ATTLIST root attr2 CDATA #IMPLIED>\r\n]>\r\n<root attr1=\"any1\" attr1=\"any2\"/>\r\n<!--* Duplicate Attribute Name in EmptyElemTag *-->\r\n",
  0,
  NULL,
  "Tests EmptyElemTag which against the WFC \"Unique Att Spec\". The attribute name \"attr1\" occurs twice in the EmptyElemTag of the element \"root\"."
},
{
  "not-wf/P45/ibm45n01.xml",
  163,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!--* Mising Name in elementdecl *-->\r\n<!ELEMENT (#PCDATA)>\r\n]>\r\n<root>Any content</root>\r\n\r\n\r\n",
  0,
  NULL,
  "Tests elementdecl with a required field missing. The Name is missing in the second elementdecl in the DTD."
},
{
  "not-wf/P45/ibm45n02.xml",
  170,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!--* Mising white space in elementdecl *-->\r\n<!ELEMENT aEle(#PCDATA)>\r\n]>\r\n<root>Any content</root>\r\n",
  0,
  NULL,
  "Tests elementdecl with a required field missing. The white space is missing between \"aEle\" and \"(#PCDATA)\" in the second elementdecl in the DTD."
},
{
  "not-wf/P45/ibm45n03.xml",
  162,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!--* Mising contentspec in elementdecl *-->\r\n<!ELEMENT root >\r\n]>\r\n<root>Any content</root>\r\n",
  0,
  NULL,
  "Tests elementdecl with a required field missing. The contentspec is missing in the second elementdecl in the DTD."
},
{
  "not-wf/P45/ibm45n04.xml",
  177,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!--* Mising contentspec and white space in elementdecl *-->\r\n<!ELEMENT root>\r\n]>\r\n<root>Any content</root>\r\n",
  0,
  NULL,
  "Tests elementdecl with a required field missing. The contentspec and the white space is missing in the second elementdecl in the DTD."
},
{
  "not-wf/P45/ibm45n05.xml",
  164,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!--* Mising Name S contentspec in elementdecl *-->\r\n<!ELEMENT >\r\n]>\r\n<root>Any content</root>\r\n",
  0,
  NULL,
  "Tests elementdecl with a required field missing. The Name, the white space, and the contentspec are missing in the second elementdecl in the DTD."
},
{
  "not-wf/P45/ibm45n06.xml",
  172,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!--* Wrong ordering in elementdecl *-->\r\n<!ELEMENT (#PCDATA) aElement >\r\n]>\r\n<root>Any content</root>\r\n",
  0,
  NULL,
  "Tests elementdecl with wrong field ordering. The Name occurs after the contentspec in the second elementdecl in the DTD."
},
{
  "not-wf/P45/ibm45n07.xml",
  179,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!--* Wrong begining sequence in elementdecl *-->\r\n<ELEMENT aElement (#PCDATA)>\r\n]>\r\n<root>Any content</root>\r\n",
  0,
  NULL,
  "Tests elementdecl with wrong beginning sequence. The string \"(less than)ELEMENT\" is used as the beginning sequence in the second elementdecl in the DTD."
},
{
  "not-wf/P45/ibm45n08.xml",
  179,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!--* Wrong Keyword: Element in elementdecl *-->\r\n<!Element aElement (#PCDATA)>\r\n]>\r\n<root>Any content</root>\r\n",
  0,
  NULL,
  "Tests elementdecl with wrong key word. The string \"Element\" is used as the key word in the second elementdecl in the DTD."
},
{
  "not-wf/P45/ibm45n09.xml",
  179,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!--* Wrong keyword: element in elementdecl *-->\r\n<!element aElement (#PCDATA)>\r\n]>\r\n<root>Any content</root>\r\n",
  0,
  NULL,
  "Tests elementdecl with wrong key word. The string \"element\" is used as the key word in the second elementdecl in the DTD."
},
{
  "not-wf/P46/ibm46n01.xml",
  175,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!--* Wrong keyword: empty in contentspec *-->\r\n<!ELEMENT aElement empty>\r\n]>\r\n<root>Any content</root>\r\n\r\n",
  0,
  NULL,
  "Tests contentspec with wrong key word. the string \"empty\" is used as the key word in the contentspec of the second elementdecl in the DTD."
},
{
  "not-wf/P46/ibm46n02.xml",
  174,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!--* Wrong keyword: Empty in contentspec *-->\r\n<!ELEMENT aElement Empty >\r\n]>\r\n<root>Any content</root>\r\n",
  0,
  NULL,
  "Tests contentspec with wrong key word. the string \"Empty\" is used as the key word in the contentspec of the second elementdecl in the DTD."
},
{
  "not-wf/P46/ibm46n03.xml",
  169,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!--* Wrong keyword: Any in contentspec *-->\r\n<!ELEMENT aElement Any>\r\n]>\r\n<root>Any content</root>\r\n",
  0,
  NULL,
  "Tests contentspec with wrong key word. the string \"Any\" is used as the key word in the contentspec of the second elementdecl in the DTD."
},
{
  "not-wf/P46/ibm46n04.xml",
  170,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!--* Wrong keyword: any in contentspec *-->\r\n<!ELEMENT aElement any >\r\n]>\r\n<root>Any content</root>\r\n",
  0,
  NULL,
  "Tests contentspec with wrong key word. the string \"any\" is used as the key word in the contentspec of the second elementdecl in the DTD."
},
{
  "not-wf/P46/ibm46n05.xml",
  180,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!--* Bogus content type: #CDATA in contentspec *-->\r\n<!ELEMENT aElement #CDATA>\r\n]>\r\n<root>Any content</root>\r\n",
  0,
  NULL,
  "Tests contentspec with a wrong option. The string \"#CDATA\" is used as the contentspec in the second elementdecl in the DTD."
},
{
  "not-wf/P47/ibm47n01.xml",
  165,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!--* Missing choice|seq in children *-->\r\n<!ELEMENT aElement + >\r\n]>\r\n<root>Any content</root>\r\n",
  0,
  NULL,
  "Tests children with a required field missing. The \"+\" is used as the choice or seq field in the second elementdecl in the DTD."
},
{
  "not-wf/P47/ibm47n02.xml",
  165,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!--* Missing choice|seq in children *-->\r\n<!ELEMENT aElement * >\r\n]>\r\n<root>Any content</root>\r\n",
  0,
  NULL,
  "Tests children with a required field missing. The \"*\" is used as the choice or seq field in the second elementdecl in the DTD."
},
{
  "not-wf/P47/ibm47n03.xml",
  165,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!--* Missing choice|seq in children *-->\r\n<!ELEMENT aElement ? >\r\n]>\r\n<root>Any content</root>\r\n",
  0,
  NULL,
  "Tests children with a required field missing. The \"?\" is used as the choice or seq field in the second elementdecl in the DTD."
},
{
  "not-wf/P47/ibm47n04.xml",
  188,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!ELEMENT a ANY>\r\n<!--* Wrong ordering in children *-->\r\n<!ELEMENT aElement *(a,a) >\r\n]>\r\n<root>Any content</root>\r\n\r\n\r\n",
  0,
  NULL,
  "Tests children with wrong field ordering. The \"*\" occurs before the seq field (a,a) in the second elementdecl in the DTD."
},
{
  "not-wf/P47/ibm47n05.xml",
  202,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!ELEMENT a ANY>\r\n<!ELEMENT b ANY>\r\n<!--* Wrong ordering in children *-->\r\n<!ELEMENT aElement +(a|a) >\r\n]>\r\n<root>Any content</root>\r\n",
  0,
  NULL,
  "Tests children with wrong field ordering. The \"+\" occurs before the choice field (a|a) in the second elementdecl in the DTD."
},
{
  "not-wf/P47/ibm47n06.xml",
  187,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!ELEMENT a ANY>\r\n<!--* Wrong keyword: ^  in children *-->\r\n<!ELEMENT aElement (a,a)^ >\r\n]>\r\n<root>Any content</root>\r\n",
  0,
  NULL,
  "Tests children with wrong key word. The \"^\" occurs after the seq field in the second elementdecl in the DTD."
},
{
  "not-wf/P48/ibm48n01.xml",
  194,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!ELEMENT a ANY>\r\n<!--* Missing seq|choice|Name in cp *-->\r\n<!ELEMENT aElement ((a,a)|+) >\r\n]>\r\n<root>Any content</root>\r\n\r\n\r\n",
  0,
  NULL,
  "Tests cp with a required fields missing. The field Name|choice|seq is missing in the second cp in the choice field in the third elementdecl in the DTD."
},
{
  "not-wf/P48/ibm48n02.xml",
  184,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!ELEMENT a ANY>\r\n<!--* Missing seq|choice|Name in cp *-->\r\n<!ELEMENT aElement (*) >\r\n]>\r\n<root>Any content</root>\r\n",
  0,
  NULL,
  "Tests cp with a required fields missing. The field Name|choice|seq is missing in the cp in the third elementdecl in the DTD."
},
{
  "not-wf/P48/ibm48n03.xml",
  192,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!ELEMENT a ANY>\r\n<!--* Missing seq|choice|Name in cp *-->\r\n<!ELEMENT aElement (?|(a,a)|a) >\r\n]>\r\n<root>Any content</root>\r\n",
  0,
  NULL,
  "Tests cp with a required fields missing. The field Name|choice|seq is missing in the first cp in the choice field in the third elementdecl in the DTD."
},
{
  "not-wf/P48/ibm48n04.xml",
  182,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!ELEMENT a ANY>\r\n<!--* wrong ordering in cp *-->\r\n<!ELEMENT aElement (+(a,a)|a) >\r\n]>\r\n<root>Any content</root>\r\n",
  0,
  NULL,
  "Tests cp with wrong field ordering. The \"+\" occurs before the seq (a,a) in the first cp in the choice field in the third elementdecl in the DTD."
},
{
  "not-wf/P48/ibm48n05.xml",
  200,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!ELEMENT a ANY>\r\n<!ELEMENT b ANY>\r\n<!--* wrong ordering in cp *-->\r\n<!ELEMENT aElement (*(a|b),a) >\r\n]>\r\n<root>Any content</root>\r\n",
  0,
  NULL,
  "Tests cp with wrong field ordering. The \"*\" occurs before the choice (a|b) in the first cp in the seq field in the third elementdecl in the DTD."
},
{
  "not-wf/P48/ibm48n06.xml",
  181,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!ELEMENT a ANY>\r\n<!--* wrong ordering in cp *-->\r\n<!ELEMENT aElement (a, ?a) >\r\n]>\r\n<root>Any content</root>\r\n\r\n",
  0,
  NULL,
  "Tests cp with wrong field ordering. The \"?\" occurs before the Name \"a\" in the second cp in the seq field in the third elementdecl in the DTD."
},
{
  "not-wf/P48/ibm48n07.xml",
  184,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!ELEMENT a ANY>\r\n<!--* wrong keyword: ^ in cp *-->\r\n<!ELEMENT aElement ( a^ | a ) >\r\n]>\r\n<root>Any content</root>\r\n",
  0,
  NULL,
  "Tests cp with wrong key word. The \"^\" occurs after the Name \"a\" in the first cp in the choice field in the third elementdecl in the DTD."
},
{
  "not-wf/P49/ibm49n01.xml",
  176,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!ELEMENT a ANY>\r\n<!--* missing cp in choice *-->\r\n<!ELEMENT aElement (|)* >\r\n]>\r\n<root>Any content</root>\r\n",
  0,
  NULL,
  "Tests choice with a required field missing. The two cps are missing in the choice field in the third elementdecl in the DTD."
},
{
  "not-wf/P49/ibm49n02.xml",
  198,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!ELEMENT a ANY>\r\n<!ELEMENT b ANY>\r\n<!--* missing cp in choice *-->\r\n<!ELEMENT aElement (a |b|)* >\r\n]>\r\n<root>Any content</root>\r\n",
  0,
  NULL,
  "Tests choice with a required field missing. The third cp is missing in the choice field in the fourth elementdecl in the DTD."
},
{
  "not-wf/P49/ibm49n03.xml",
  205,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!ELEMENT a ANY>\r\n<!ELEMENT b ANY>\r\n<!--* wrong separator: !  in choice *-->\r\n<!ELEMENT aElement (a!b)+ >\r\n]>\r\n<root>Any content</root>\r\n",
  0,
  NULL,
  "Tests choice with a wrong separator. The \"!\" is used as the separator in the choice field in the fourth elementdecl in the DTD."
},
{
  "not-wf/P49/ibm49n04.xml",
  202,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!ELEMENT a ANY>\r\n<!ELEMENT b ANY>\r\n<!--* missing separator in choice *-->\r\n<!ELEMENT aElement (a b) >\r\n]>\r\n<root>Any content</root>\r\n",
  0,
  NULL,
  "Tests choice with a required field missing. The separator \"|\" is missing in the choice field (a b)+ in the fourth elementdecl in the DTD."
},
{
  "not-wf/P49/ibm49n05.xml",
  203,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!ELEMENT a ANY>\r\n<!ELEMENT b ANY>\r\n<!--* extra separator in choice *-->\r\n<!ELEMENT aElement (a ||b)* >\r\n]>\r\n<root>Any content</root>\r\n",
  0,
  NULL,
  "Tests choice with an extra separator. An extra \"|\" occurs between a and b in the choice field in the fourth elementdecl in the DTD."
},
{
  "not-wf/P49/ibm49n06.xml",
  212,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!ELEMENT a ANY>\r\n<!ELEMENT b ANY>\r\n<!--* missing closing bracket in choice *-->\r\n<!ELEMENT aElement (a |b * >\r\n]>\r\n<root>Any content</root>\r\n\r\n",
  0,
  NULL,
  "Tests choice with a required field missing. The closing bracket \")\" is missing in the choice field (a |b * in the fourth elementdecl in the DTD."
},
{
  "not-wf/P50/ibm50n01.xml",
  190,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!ELEMENT a ANY>\r\n<!ELEMENT b ANY>\r\n<!--* missing cp in seq *-->\r\n<!ELEMENT aElement (,) >\r\n]>\r\n<root>Any content</root>\r\n",
  0,
  NULL,
  "Tests seq with a required field missing. The two cps are missing in the seq field in the fourth elementdecl in the DTD."
},
{
  "not-wf/P50/ibm50n02.xml",
  194,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!ELEMENT a ANY>\r\n<!ELEMENT b ANY>\r\n<!--* missing cp in seq *-->\r\n<!ELEMENT aElement (a,a,)+ >\r\n]>\r\n<root>Any content</root>\r\n",
  0,
  NULL,
  "Tests seq with a required field missing. The third cp is missing in the seq field in the fourth elementdecl in the DTD."
},
{
  "not-wf/P50/ibm50n03.xml",
  200,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!ELEMENT a ANY>\r\n<!ELEMENT b ANY>\r\n<!--* wrong separators in seq *-->\r\n<!ELEMENT aElement (a,a|b) >\r\n]>\r\n<root>Any content</root>\r\n",
  0,
  NULL,
  "Tests seq with a wrong separator. The \"|\" is used as the separator between a and b in the seq field in the fourth elementdecl in the DTD."
},
{
  "not-wf/P50/ibm50n04.xml",
  200,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!ELEMENT a ANY>\r\n<!ELEMENT b ANY>\r\n<!--* wrong separator in seq *-->\r\n<!ELEMENT aElement (a . b)* >\r\n]>\r\n<root>Any content</root>\r\n",
  0,
  NULL,
  "Tests seq with a wrong separator. The \".\" is used as the separator between a and b in the seq field in the fourth elementdecl in the DTD."
},
{
  "not-wf/P50/ibm50n05.xml",
  203,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!ELEMENT a ANY>\r\n<!ELEMENT b ANY>\r\n<!--* extra separator in seq *-->\r\n<!ELEMENT aElement ((a|b),,a)? >\r\n]>\r\n<root>Any content</root>\r\n",
  0,
  NULL,
  "Tests seq with an extra separator. An extra \",\" occurs between (a|b) and a in the seq field in the fourth elementdecl in the DTD."
},
{
  "not-wf/P50/ibm50n06.xml",
  207,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!ELEMENT a ANY>\r\n<!ELEMENT b ANY>\r\n<!--* missing separator in seq *-->\r\n<!ELEMENT aElement ((a|b) (b|a)) >\r\n]>\r\n<root>Any content</root>\r\n",
  0,
  NULL,
  "Tests seq with a required field missing. The separator between (a|b) and (b|a) is missing in the seq field in the fourth elementdecl in the DTD."
},
{
  "not-wf/P50/ibm50n07.xml",
  205,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!ELEMENT a ANY>\r\n<!ELEMENT b ANY>\r\n<!--* wrong closing bracket in seq *-->\r\n<!ELEMENT aElement (a, b]* >\r\n]>\r\n<root>Any content</root>\r\n",
  0,
  NULL,
  "Tests seq with wrong closing bracket. The \"]\" is used as the closing bracket in the seq field in the fourth elementdecl in the DTD."
},
{
  "not-wf/P51/ibm51n01.xml",
  213,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!ELEMENT a ANY>\r\n<!ELEMENT b ANY>\r\n<!--* wrong keyword : #pcdata in Mixed  *-->\r\n<!ELEMENT aElement (#pcdata)* >\r\n]>\r\n<root>Any content</root>\r\n",
  0,
  NULL,
  "Tests Mixed with a wrong key word. The string \"#pcdata\" is used as the key word in the Mixed field in the fourth elementdecl in the DTD."
},
{
  "not-wf/P51/ibm51n02.xml",
  222,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!ELEMENT a ANY>\r\n<!ELEMENT b ANY>\r\n<!--* #PCDATA must be the first in Mixed  *-->\r\n<!ELEMENT aElement ( a | b|#PCDATA)* >\r\n]>\r\n<root>Any content</root>\r\n",
  0,
  NULL,
  "Tests Mixed with wrong field ordering. The field #PCDATA does not occur as the first component in the Mixed field in the fourth elementdecl in the DTD."
},
{
  "not-wf/P51/ibm51n03.xml",
  205,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!ELEMENT a ANY>\r\n<!ELEMENT b ANY>\r\n<!--* Missing '|' in Mixed  *-->\r\n<!ELEMENT aElement ( #PCDATA a )* >\r\n]>\r\n<root>Any content</root>\r\n",
  0,
  NULL,
  "Tests Mixed with a separator missing. The separator \"|\" is missing in between #PCDATA and a in the Mixed field in the fourth elementdecl in the DTD."
},
{
  "not-wf/P51/ibm51n04.xml",
  209,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!ELEMENT a ANY>\r\n<!ELEMENT b ANY>\r\n<!--* wrong keyword: #CDATA in Mixed  *-->\r\n<!ELEMENT aElement (#CDATA) >\r\n]>\r\n<root>Any content</root>\r\n",
  0,
  NULL,
  "Tests Mixed with a wrong key word. The string \"#CDATA\" is used as the key word in the Mixed field in the fourth elementdecl in the DTD."
},
{
  "not-wf/P51/ibm51n05.xml",
  213,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!ELEMENT a ANY>\r\n<!ELEMENT b ANY>\r\n<!--* Missing '* after ')' in Mixed  *-->\r\n<!ELEMENT aElement ( #PCDATA | a ) >\r\n]>\r\n<root>Any content</root>",
  0,
  NULL,
  "Tests Mixed with a required field missing. The \"*\" is missing after the \")\" in the Mixed field in the fourth elementdecl in the DTD."
},
{
  "not-wf/P51/ibm51n06.xml",
  217,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!ELEMENT a ANY>\r\n<!ELEMENT b ANY>\r\n<!--* Wrong closing bracket in Mixed  *-->\r\n<!ELEMENT aElement ( #PCDATA | a ]* >\r\n]>\r\n<root>Any content</root>\r\n",
  0,
  NULL,
  "Tests Mixed with wrong closing bracket. The \"]\" is used as the closing bracket in the Mixed field in the fourth elementdecl in the DTD."
},
{
  "not-wf/P51/ibm51n07.xml",
  213,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!ELEMENT a ANY>\r\n<!ELEMENT b ANY>\r\n<!--* Missing closing bracket in Mixed  *-->\r\n<!ELEMENT aElement ( #PCDATA *>\r\n]>\r\n<root>Any content</root>\r\n",
  0,
  NULL,
  "Tests Mixed with a required field missing. The closing bracket \")\" is missing after (#PCDATA in the Mixed field in the fourth elementdecl in the DTD."
},
{
  "not-wf/P52/ibm52n01.xml",
  189,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!ELEMENT a ANY>\r\n<!--* Missing Name in AttlistDecl *-->\r\n<!ATTLIST attr1 CDATA #IMPLIED>\r\n]>\r\n<root>Any content</root>\r\n",
  0,
  NULL,
  "Tests AttlistDecl with a required field missing. The Name is missing in the AttlistDecl in the DTD."
},
{
  "not-wf/P52/ibm52n02.xml",
  197,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!ELEMENT a ANY>\r\n<!--* Missing white space after 'ATTLIST' *-->\r\n<!ATTLISTa attr1 ID #REQUIRED >\r\n]>\r\n<root>Any content</root>\r\n",
  0,
  NULL,
  "Tests AttlistDecl with a required field missing. The white space is missing between the beginning sequence and the name in the AttlistDecl in the DTD."
},
{
  "not-wf/P52/ibm52n03.xml",
  223,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!ELEMENT a ANY>\r\n<!--* Wrong ordering in AttlistDecl *-->\r\n<!ATTLIST attr1 CDATA \"defaultAttValue\" a attr2 CDATA #IMPLIED>\r\n]>\r\n<root>Any content</root>\r\n",
  0,
  NULL,
  "Tests AttlistDecl with wrong field ordering. The Name \"a\" occurs after the first AttDef in the AttlistDecl in the DTD."
},
{
  "not-wf/P52/ibm52n04.xml",
  203,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!ELEMENT a ANY>\r\n<!--* Wrong keyword: Attlist in AttlistDecl *-->\r\n<!Attlist a attr1 CDATA #REQUIRED >\r\n]>\r\n<root>Any content</root>\r\n",
  0,
  NULL,
  "Tests AttlistDecl with wrong key word. The string \"Attlist\" is used as the key word in the beginning sequence in the AttlistDecl in the DTD."
},
{
  "not-wf/P52/ibm52n05.xml",
  200,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!ELEMENT a ANY>\r\n<!--* Missing closing bracket in AttlistDecl *-->\r\n<!ATTLIST a \r\n<!--* random *-->\r\n]>\r\n<root>Any content</root>\r\n",
  0,
  NULL,
  "Tests AttlistDecl with a required field missing. The closing bracket \"greater than\" is missing in the AttlistDecl in the DTD."
},
{
  "not-wf/P52/ibm52n06.xml",
  202,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!ELEMENT a ANY>\r\n<!--* Wrong begining sequence in AttlistDecl *-->\r\n<ATTLIST a attr1 CDATA \"default\">\r\n]>\r\n<root>Any content</root>\r\n",
  0,
  NULL,
  "Tests AttlistDecl with wrong beginning sequence. The string \"(less than)ATTLIST\" is used as the beginning sequence in the AttlistDecl in the DTD."
},
{
  "not-wf/P53/ibm53n01.xml",
  189,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!ELEMENT a ANY>\r\n<!--* Missing DefaultDecl in AttDef *-->\r\n<!ATTLIST a attr1 CDATA >\r\n]>\r\n<root>Any content</root>\r\n\r\n\r\n",
  0,
  NULL,
  "Tests AttDef with a required field missing. The DefaultDecl is missing in the AttDef for the name \"attr1\" in the AttlistDecl in the DTD."
},
{
  "not-wf/P53/ibm53n02.xml",
  225,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!ELEMENT a ANY>\r\n<!--* Missing white space between AttType and DefaultDecl in AttDef *-->\r\n<!ATTLIST a attr1 (abc|def)\"def\">\r\n]>\r\n<root>Any content</root>\r\n",
  0,
  NULL,
  "Tests AttDef with a required field missing. The white space is missing between (abc|def) and \"def\" in the AttDef in the AttlistDecl in the DTD."
},
{
  "not-wf/P53/ibm53n03.xml",
  183,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!ELEMENT a ANY>\r\n<!--* Missing AttType in AttDef *-->\r\n<!ATTLIST a attr1 #IMPLIED>\r\n]>\r\n<root>Any content</root>\r\n",
  0,
  NULL,
  "Tests AttDef with a required field missing. The AttType is missing for \"attr1\" in the AttDef in the AttlistDecl in the DTD."
},
{
  "not-wf/P53/ibm53n04.xml",
  219,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!ELEMENT a ANY>\r\n<!--* Missing white space between Name and AttType in AttDef *-->\r\n<!ATTLIST a attr1(abc|def) \"abc\" >\r\n]>\r\n<root>Any content</root>\r\n",
  0,
  NULL,
  "Tests AttDef with a required field missing. The white space is missing between \"attr1\" and (abc|def) in the AttDef in the AttlistDecl in the DTD."
},
{
  "not-wf/P53/ibm53n05.xml",
  182,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!ELEMENT a ANY>\r\n<!--* Missing Name in AttDef *-->\r\n<!ATTLIST a (abc|def) \"def\" >\r\n]>\r\n<root>Any content</root>\r\n",
  0,
  NULL,
  "Tests AttDef with a required field missing. The Name is missing in the AttDef in the AttlistDecl in the DTD."
},
{
  "not-wf/P53/ibm53n06.xml",
  224,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!ELEMENT a ANY>\r\n<!--* Missing white space before Name in AttDef *-->\r\n<!ATTLIST a attr1 CDATA \"default\"attr2 ID #required>\r\n]>\r\n<root>Any content</root>\r\n",
  0,
  NULL,
  "Tests AttDef with a required field missing. The white space before the name \"attr2\" is missing in the AttDef in the AttlistDecl in the DTD."
},
{
  "not-wf/P53/ibm53n07.xml",
  189,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!ELEMENT a ANY>\r\n<!--* Wrong ordering in AttDef *-->\r\n<!ATTLIST a (abc|def) attr1 \"abc\">\r\n]>\r\n<root>Any content</root>\r\n",
  0,
  NULL,
  "Tests AttDef with wrong field ordering. The Name \"attr1\" occurs after the AttType in the AttDef in the AttlistDecl in the DTD."
},
{
  "not-wf/P53/ibm53n08.xml",
  190,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root [\r\n<!ELEMENT root (#PCDATA)>\r\n<!ELEMENT a ANY>\r\n<!--* wrong ordering in AttDef *-->\r\n<!ATTLIST a \"default\" CDATA attr1 >\r\n]>\r\n<root>Any content</root>\r\n",
  0,
  NULL,
  "Tests AttDef with wrong field ordering. The Name \"attr1\" occurs after the AttType and \"default\" occurs before the AttType in the AttDef in the AttlistDecl in the DTD."
},
{
  "not-wf/P54/ibm54n01.xml",
  217,
  "<?xml  version=\"1.0\"?>\r\n<!-- test for Production 54-->\r\n<!DOCTYPE AttrType\r\n[\r\n<!ELEMENT AttrType ANY>\r\n<!ELEMENT a EMPTY>\r\n<!ATTLIST a att BOGUSATTR #IMPLIED> \r\n]>\r\n<AttrType>\r\nGiving a Bogus attribute. \r\n</AttrType>",
  0,
  NULL,
  "Tests AttType with a wrong option. The string \"BOGUSATTR\" is used as the AttType in the AttlistDecl in the DTD."
},
{
  "not-wf/P54/ibm54n02.xml",
  249,
  "<?xml  version=\"1.0\"?>\r\n<!-- test for Production 54-->\r\n<!DOCTYPE AttrType\r\n[\r\n<!ELEMENT AttrType ANY>\r\n<!ELEMENT a EMPTY>\r\n<!ATTLIST a att PCDATA #IMPLIED> \r\n]>\r\n<AttrType>\r\nGiving a wrong AttType for the attribute. \r\n<a att=\"23\" ></a>\r\n</AttrType>",
  0,
  NULL,
  "Tests AttType with a wrong option. The string \"PCDATA\" is used as the AttType in the AttlistDecl in the DTD."
},
{
  "not-wf/P55/ibm55n01.xml",
  248,
  "<?xml  version=\"1.0\"?>\r\n<!-- test syntax invalid for Production 55-->\r\n<!DOCTYPE AttrType\r\n[\r\n<!ELEMENT AttrType (#PCDATA)>\r\n<!ELEMENT a EMPTY>\r\n<!ATTLIST a att cdata #IMPLIED>  \r\n]>\r\n<AttrType>\r\nGiving a lowercase for CDATA attribute.\r\n</AttrType>",
  0,
  NULL,
  "Tests StringType with a wrong key word. The lower case string \"cdata\" is used as the StringType in the AttType in the AttlistDecl in the DTD."
},
{
  "not-wf/P55/ibm55n02.xml",
  235,
  "<?xml  version=\"1.0\"?>\r\n<!-- test invalid syntax for Production 55-->\r\n<!DOCTYPE AttrType\r\n[\r\n<!ELEMENT AttrType (#PCDATA)>\r\n<!ELEMENT a EMPTY>\r\n<!ATTLIST a att #CDATA #IMPLIED> \r\n]>\r\n<AttrType>\r\nGiving a wrong character. \r\n</AttrType>",
  0,
  NULL,
  "Tests StringType with a wrong key word. The string \"#CDATA\" is used as the StringType in the AttType in the AttlistDecl in the DTD."
},
{
  "not-wf/P55/ibm55n03.xml",
  251,
  "<?xml  version=\"1.0\"?>\r\n<!-- test invalid syntax for Production 55-->\r\n<!DOCTYPE AttrType\r\n[\r\n<!ELEMENT AttrType (#PCDATA)>\r\n<!ELEMENT a EMPTY>\r\n<!ATTLIST a att CData #IMPLIED> \r\n]>\r\n<AttrType>\r\n Giving a wrong key word of the StringType.\r\n</AttrType>",
  0,
  NULL,
  "Tests StringType with a wrong key word. The string \"CData\" is used as the StringType in the AttType in the AttlistDecl in the DTD."
},
{
  "not-wf/P56/ibm56n01.xml",
  204,
  "<?xml  version=\"1.0\"?>\r\n<!-- test for Production 56-->\r\n<!DOCTYPE root\r\n[\r\n<!ELEMENT root ANY>\r\n<!ELEMENT a EMPTY>\r\n<!ATTLIST a attr id #REQUIRED>\r\n]>\r\n<root>\r\nInvalid TokenizedType id(lowercase)\r\n</root>",
  0,
  NULL,
  "Tests TokenizedType with wrong key word. The type \"id\" is used in the TokenizedType in the AttDef in the AttlistDecl in the DTD."
},
{
  "not-wf/P56/ibm56n02.xml",
  215,
  "<?xml  version=\"1.0\"?>\r\n<!-- test for Production 56-->\r\n<!DOCTYPE root\r\n[\r\n<!ELEMENT root ANY>\r\n<!ELEMENT a EMPTY>\r\n<!ATTLIST a attr Idref #REQUIRED>\r\n]>\r\n<root>\r\nInvalid TokenizedType Idref(case sensitive)\r\n</root>",
  0,
  NULL,
  "Tests TokenizedType with wrong key word. The type \"Idref\" is used in the TokenizedType in the AttDef in the AttlistDecl in the DTD."
},
{
  "not-wf/P56/ibm56n03.xml",
  217,
  "<?xml  version=\"1.0\"?>\r\n<!-- test for Production 56-->\r\n<!DOCTYPE root\r\n[\r\n<!ELEMENT root ANY>\r\n<!ELEMENT a EMPTY>\r\n<!ATTLIST a attr IdRefs #REQUIRED>\r\n]>\r\n<root>\r\nInvalid TokenizedType IdRefs(case sensitive)\r\n</root>",
  0,
  NULL,
  "Tests TokenizedType with wrong key word. The type\"Idrefs\" is used in the TokenizedType in the AttDef in the AttlistDecl in the DTD."
},
{
  "not-wf/P56/ibm56n04.xml",
  217,
  "<?xml  version=\"1.0\"?>\r\n<!-- test for Production 56-->\r\n<!DOCTYPE root\r\n[\r\n<!ELEMENT root ANY>\r\n<!ELEMENT a EMPTY>\r\n<!ATTLIST a attr EntitY #REQUIRED>\r\n]>\r\n<root>\r\nInvalid TokenizedType EntitY(case sensitive)\r\n</root>",
  0,
  NULL,
  "Tests TokenizedType with wrong key word. The type \"EntitY\" is used in the TokenizedType in the AttDef in the AttlistDecl in the DTD."
},
{
  "not-wf/P56/ibm56n05.xml",
  219,
  "<?xml  version=\"1.0\"?>\r\n<!-- test for Production 56-->\r\n<!DOCTYPE root\r\n[\r\n<!ELEMENT root ANY>\r\n<!ELEMENT a EMPTY>\r\n<!ATTLIST a attr nmTOKEN #REQUIRED>\r\n]>\r\n<root>\r\nInvalid TokenizedType nmTOKEN(case sensitive)\r\n</root>",
  0,
  NULL,
  "Tests TokenizedType with wrong key word. The type \"nmTOKEN\" is used in the TokenizedType in the AttDef in the AttlistDecl in the DTD."
},
{
  "not-wf/P56/ibm56n06.xml",
  221,
  "<?xml  version=\"1.0\"?>\r\n<!-- test for Production 56-->\r\n<!DOCTYPE root\r\n[\r\n<!ELEMENT root ANY>\r\n<!ELEMENT a EMPTY>\r\n<!ATTLIST a attr NMtokens #REQUIRED>\r\n]>\r\n<root>\r\nInvalid TokenizedType NMtokens(case sensitive)\r\n</root>",
  0,
  NULL,
  "Tests TokenizedType with wrong key word. The type \"NMtokens\" is used in the TokenizedType in the AttDef in the AttlistDecl in the DTD."
},
{
  "not-wf/P56/ibm56n07.xml",
  212,
  "<?xml  version=\"1.0\"?>\r\n<!-- test for Production 56-->\r\n<!DOCTYPE root\r\n[\r\n<!ELEMENT root ANY>\r\n<!ELEMENT a EMPTY>\r\n<!ATTLIST a attr #ID #REQUIRED>\r\n]>\r\n<root>\r\nInvalid TokenizedType #ID(Wrong Character)\r\n</root>",
  0,
  NULL,
  "Tests TokenizedType with wrong key word. The type \"#ID\" is used in the TokenizedType in the AttDef in the AttlistDecl in the DTD."
},
{
  "not-wf/P57/ibm57n01.xml",
  224,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 57-->\r\n<!DOCTYPE root\r\n [\r\n <!ELEMENT root EMPTY>\r\n <!ATTLIST root att NMTOKEN (a|b) #IMPLIED>\r\n ]>\r\n <root>\r\nThis test case tests the illegal enumerated types\r\n</root>",
  0,
  NULL,
  "Tests EnumeratedType with an illegal option. The string \"NMTOKEN (a|b)\" is used in the EnumeratedType in the AttlistDecl in the DTD."
},
{
  "not-wf/P58/ibm58n01.xml",
  300,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 58-->\r\n<!DOCTYPE test\r\n [\r\n <!ELEMENT test ANY>\r\n <!ELEMENT one EMPTY>\r\n <!NOTATION this SYSTEM \"alpha\">\r\n <!ATTLIST one attr notation (this) #IMPLIED>\r\n ]>\r\n <test>\r\nThis is a Negative test with notation (name) \r\nIt is case sensitive.\r\n</test>",
  0,
  NULL,
  "Tests NotationType with wrong key word. The lower case \"notation\" is used as the key word in the NotationType in the AttDef in the AttlistDecl in the DTD."
},
{
  "not-wf/P58/ibm58n02.xml",
  298,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 58-->\r\n<!DOCTYPE test\r\n [\r\n <!ELEMENT test ANY>\r\n <!ELEMENT one EMPTY>\r\n <!NOTATION this SYSTEM \"alpha\">\r\n <!ATTLIST one attr NOTATION this) #IMPLIED>\r\n ]>\r\n <test>\r\nThis is a Negative test with  (name) \r\nMissing the open parenthesis\r\n</test>",
  0,
  NULL,
  "Tests NotationType with a required field missing. The beginning bracket \"(\" is missing in the NotationType in the AttDef in the AttlistDecl in the DTD."
},
{
  "not-wf/P58/ibm58n03.xml",
  298,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 58-->\r\n<!DOCTYPE test\r\n [\r\n <!ELEMENT test ANY>\r\n <!ELEMENT one EMPTY>\r\n <!NOTATION this SYSTEM \"alpha\">\r\n <!ATTLIST one attr NOTATION () #IMPLIED>\r\n ]>\r\n <test>\r\nThis is a Negative test with  NOTATION () \r\nMissing the required field\r\n</test>",
  0,
  NULL,
  "Tests NotationType with a required field missing. The Name is missing in the \"()\" in the NotationType in the AttDef in the AttlistDecl in the DTD."
},
{
  "not-wf/P58/ibm58n04.xml",
  306,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 58-->\r\n<!DOCTYPE test\r\n [\r\n <!ELEMENT test ANY>\r\n <!ELEMENT one EMPTY>\r\n <!NOTATION this SYSTEM \"alpha\">\r\n <!ATTLIST one attr NOTATION (this #IMPLIED>\r\n ]>\r\n <test>\r\nThis is a Negative test with  NOTATION (Name \r\nMissing the closing brackets\r\n</test>",
  0,
  NULL,
  "Tests NotationType with a required field missing. The closing bracket is missing in the NotationType in the AttDef in the AttlistDecl in the DTD."
},
{
  "not-wf/P58/ibm58n05.xml",
  294,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 58-->\r\n<!DOCTYPE test\r\n [\r\n <!ELEMENT test ANY>\r\n <!ELEMENT one EMPTY>\r\n <!NOTATION this SYSTEM \"alpha\">\r\n <!ATTLIST one attr (this) NOTATION #IMPLIED>\r\n ]>\r\n <test>\r\nThis is a Negative test with (Name) NOTATION  \r\nWrong Ordering\r\n</test>",
  0,
  NULL,
  "Tests NotationType with wrong field ordering. The key word \"NOTATION\" occurs after \"(this)\" in the NotationType in the AttDef in the AttlistDecl in the DTD."
},
{
  "not-wf/P58/ibm58n06.xml",
  392,
  "<?xml  version=\"1.0\"?>\r\n<!-- Syntax test for Production 58-->\r\n<!DOCTYPE test\r\n [\r\n <!ELEMENT test ANY>\r\n <!ELEMENT one EMPTY>\r\n <!ELEMENT two EMPTY>\r\n <!NOTATION this SYSTEM \"alpha\">\r\n <!NOTATION that SYSTEM \"beta\">\r\n <!ATTLIST three attr NOTATION (this,that) #IMPLIED>\r\n ]>\r\n<test>\r\nNegative Test.\r\nThis test tests the presence of a correct seperator. There is a wrong seperator(,)\r\n</test>",
  0,
  NULL,
  "Tests NotationType with wrong separator. The \",\" is used as a separator between \"this\" and \"that\" in the NotationType in the AttDef in the AttlistDecl in the DTD."
},
{
  "not-wf/P58/ibm58n07.xml",
  299,
  "<?xml  version=\"1.0\"?>\r\n<!-- Syntax test for Production 58-->\r\n<!DOCTYPE test\r\n [\r\n <!ELEMENT test ANY>\r\n <!ELEMENT one EMPTY>\r\n <!ELEMENT two EMPTY>\r\n <!NOTATION this SYSTEM \"alpha\">\r\n <!ATTLIST three attr NOTATION(this) #IMPLIED>\r\n ]>\r\n<test>\r\nNegative Test.\r\nMissing space after NOTATION\r\n</test>",
  0,
  NULL,
  "Tests NotationType with a required field missing. The white space is missing between \"NOTATION\" and \"(this)\" in the NotationType in the AttDef in the AttlistDecl in the DTD."
},
{
  "not-wf/P58/ibm58n08.xml",
  309,
  "<?xml  version=\"1.0\"?>\r\n<!-- Syntax test for Production 58-->\r\n<!DOCTYPE test\r\n [\r\n <!ELEMENT test ANY>\r\n <!ELEMENT one EMPTY>\r\n <!ELEMENT two EMPTY>\r\n <!NOTATION this SYSTEM \"alpha\">\r\n <!ATTLIST three attr NOTATION (\"this\") #IMPLIED>\r\n ]>\r\n<test>\r\nNegative Test.\r\nPresence of quotes around the value\r\n</test>",
  0,
  NULL,
  "Tests NotationType with extra wrong characters. The double quote character occurs after \"(\" and before \")\" in the NotationType in the AttDef in the AttlistDecl in the DTD."
},
{
  "not-wf/P59/ibm59n01.xml",
  264,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 59-->\r\n<!DOCTYPE test\r\n [\r\n <!ELEMENT test ANY>\r\n <!ELEMENT one EMPTY>\r\n <!ELEMENT enum (#PCDATA)>\r\n <!ATTLIST one attr () #IMPLIED>\r\n ]>\r\n <test>\r\nThis is a Negative test\r\nMissing the required field\r\n</test>",
  0,
  NULL,
  "Tests Enumeration with required fields missing. The Nmtokens and \"|\"s are missing in the AttDef in the AttlistDecl in the DTD."
},
{
  "not-wf/P59/ibm59n02.xml",
  269,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 59-->\r\n<!DOCTYPE test\r\n [\r\n <!ELEMENT test ANY>\r\n <!ELEMENT one EMPTY>\r\n <!ELEMENT enum (#PCDATA)>\r\n <!ATTLIST one attr (enum #IMPLIED>\r\n ]>\r\n <test>\r\nThis is a Negative test\r\nMissing the closing brackets\r\n</test>",
  0,
  NULL,
  "Tests Enumeration with a required field missing. The closing bracket \")\" is missing in the AttDef in the AttlistDecl in the DTD."
},
{
  "not-wf/P59/ibm59n03.xml",
  300,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 59-->\r\n<!DOCTYPE test\r\n [\r\n <!ELEMENT test ANY>\r\n <!ELEMENT one EMPTY>\r\n <!ELEMENT two EMPTY>\r\n <!ELEMENT enum (#PCDATA)>\r\n <!ATTLIST one attr (enum,two) #IMPLIED>\r\n ]>\r\n <test>\r\nThis is a Negative test\r\nWrong Separator(, instead of |)\r\n</test>",
  0,
  NULL,
  "Tests Enumeration with wrong separator. The \",\" is used as the separator in the AttDef in the AttlistDecl in the DTD."
},
{
  "not-wf/P59/ibm59n04.xml",
  287,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 59-->\r\n<!DOCTYPE test\r\n [\r\n <!ELEMENT test ANY>\r\n <!ELEMENT one EMPTY>\r\n <!ELEMENT enum (#PCDATA)>\r\n <!ATTLIST one attr (\"enum\") #IMPLIED>\r\n ]>\r\n <test>\r\nThis is a Negative test\r\nIllegal presence of quotes around the value\r\n</test>",
  0,
  NULL,
  "Tests Enumeration with illegal presence. The double quotes occur around the Enumeration value in the AttDef in the AttlistDecl in the DTD."
},
{
  "not-wf/P59/ibm59n05.xml",
  271,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 59-->\r\n<!DOCTYPE test\r\n [\r\n <!ELEMENT test ANY>\r\n <!ELEMENT one EMPTY>\r\n <!ELEMENT enum (#PCDATA)>\r\n <!ATTLIST one attr enum) #IMPLIED>\r\n ]>\r\n <test>\r\nThis is a Negative test\r\nMissing the begining bracket  \r\n</test>",
  0,
  NULL,
  "Tests Enumeration with a required field missing. The white space is missing between in the AttDef in the AttlistDecl in the DTD."
},
{
  "not-wf/P59/ibm59n06.xml",
  269,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 59-->\r\n<!DOCTYPE test\r\n [\r\n <!ELEMENT test ANY>\r\n <!ELEMENT one EMPTY>\r\n <!ELEMENT enum (#PCDATA)>\r\n <!ATTLIST one attr enum) #IMPLIED>\r\n ]>\r\n <test>\r\nThis is a Negative test\r\nMissing the Opening brackets\r\n</test>",
  0,
  NULL,
  "Tests Enumeration with a required field missing. The beginning bracket \"(\" is missing in the AttDef in the AttlistDecl in the DTD."
},
{
  "not-wf/P60/ibm60n01.xml",
  258,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 60-->\r\n<!DOCTYPE Java \r\n [\r\n <!ELEMENT Java ANY>\r\n <!ELEMENT one EMPTY>\r\n <!ATTLIST one chapter CDATA #required>\r\n ]>\r\n<Java>\r\n<one chapter=\"Introduction\"></one>\r\nNegative Test. Case sensitive.\r\n</Java>",
  0,
  NULL,
  "Tests DefaultDecl with wrong key word. The string \"#required\" is used as the key word in the DefaultDecl in the AttDef in the AttlistDecl in the DTD."
},
{
  "not-wf/P60/ibm60n02.xml",
  256,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 60-->\r\n<!DOCTYPE Java \r\n [\r\n <!ELEMENT Java ANY>\r\n <!ELEMENT one EMPTY>\r\n <!ATTLIST one chapter CDATA #Implied>\r\n ]>\r\n<Java>\r\n<one chapter=\"Introduction\"></one>\r\nNegative test. Case Sensitive\r\n</Java>",
  0,
  NULL,
  "Tests DefaultDecl with wrong key word. The string \"Implied\" is used as the key word in the DefaultDecl in the AttDef in the AttlistDecl in the DTD."
},
{
  "not-wf/P60/ibm60n03.xml",
  258,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 60-->\r\n<!DOCTYPE Java \r\n [\r\n <!ELEMENT Java ANY>\r\n <!ELEMENT one EMPTY>\r\n <!ATTLIST one chapter CDATA !IMPLIED>\r\n ]>\r\n<Java>\r\n<one chapter=\"Introduction\"></one>\r\nNegative Test. Wrong Character.\r\n</Java>",
  0,
  NULL,
  "Tests DefaultDecl with wrong key word. The string \"!IMPLIED\" is used as the key word in the DefaultDecl in the AttDef in the AttlistDecl in the DTD."
},
{
  "not-wf/P60/ibm60n04.xml",
  291,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 60-->\r\n<!DOCTYPE Java \r\n [\r\n <!ELEMENT Java ANY>\r\n <!ELEMENT one EMPTY>\r\n <!ATTLIST one chapter CDATA #FIXED >\r\n ]>\r\n<Java>\r\n<one chapter=\"Introduction\"></one>\r\nNegative test. Missing required field(#FIXED should have a value)\r\n</Java>",
  0,
  NULL,
  "Tests DefaultDecl with a required field missing. There is no attribute value specified after the key word \"#FIXED\" in the DefaultDecl in the AttDef in the AttlistDecl in the DTD."
},
{
  "not-wf/P60/ibm60n05.xml",
  317,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 60-->\r\n<!DOCTYPE Java \r\n [\r\n <!ELEMENT Java ANY>\r\n <!ELEMENT one EMPTY>\r\n <!ATTLIST one chapter CDATA #FIXED\"Introduction\">\r\n ]>\r\n<Java>\r\n<one chapter=\"Introduction\"></one>\r\nNegative test. Missing required field(#FIXED should have a space before value)\r\n</Java>",
  0,
  NULL,
  "Tests DefaultDecl with a required field missing. The white space is missing between the key word \"#FIXED\" and the attribute value in the DefaultDecl in the AttDef in the AttlistDecl in the DTD."
},
{
  "not-wf/P60/ibm60n06.xml",
  269,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 60-->\r\n<!DOCTYPE Java \r\n [\r\n <!ELEMENT Java ANY>\r\n <!ELEMENT one EMPTY>\r\n <!ATTLIST one chapter CDATA \"Introduction\" #FIXED>\r\n ]>\r\n<Java>\r\n<one chapter=\"Introduction\"></one>\r\nNegative test. Wrong Ordering\r\n</Java>",
  0,
  NULL,
  "Tests DefaultDecl with wrong field ordering. The key word \"#FIXED\" occurs after the attribute value \"introduction\" in the DefaultDecl in the AttDef in the AttlistDecl in the DTD."
},
{
  "not-wf/P60/ibm60n07.xml",
  397,
  "<?xml  version=\"1.0\"?>\r\n<!-- WFC test for Production 60-->\r\n<!DOCTYPE Java \r\n [\r\n <!ELEMENT Java ANY>\r\n <!ELEMENT one EMPTY>\r\n <!ENTITY avalue \"<Introduction\">\r\n <!ATTLIST one chapter CDATA #REQUIRED>\r\n ]>\r\n<Java>\r\n<one chapter=\"&avalue;\"></one>\r\nNegative test. \r\nThe replacement text of any entity referred to directly or indirectly \r\nin an attribute value contains a less than character\r\n</Java>",
  0,
  NULL,
  "Tests DefaultDecl against WFC of P60. The text replacement of the entity \"avalue\" contains the \"less than\" character in the DefaultDecl in the AttDef in the AttlistDecl in the DTD."
},
{
  "not-wf/P60/ibm60n08.xml",
  294,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 60-->\r\n<!DOCTYPE Java \r\n [\r\n <!ELEMENT Java ANY>\r\n <!ELEMENT one EMPTY>\r\n <!ATTLIST one chapter CDATA #REQUIRED #IMPLIED>\r\n ]>\r\n<Java>\r\n<one chapter=\"Introduction\"></one>\r\nNegative Test. More than one Default type declarations.\r\n</Java>\r\n",
  0,
  NULL,
  "Tests DefaultDecl with more than one key word. The \"#REQUIRED\" and the \"#IMPLIED\" are used as the key words in the DefaultDecl in the AttDef in the AttlistDecl in the DTD."
},
{
  "not-wf/P61/ibm61n01.xml",
  136,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 61-->\r\n<!DOCTYPE animal SYSTEM \"ibm61n01.dtd\">\r\n<animal>\r\n <tiger/>\r\n</animal>\r\n",
  0,
  NULL,
  "Tests conditionalSect with a wrong option. The word \"NOTINCLUDE\" is used as part of an option which is wrong in the coditionalSect."
},
{
  "not-wf/P62/ibm62n01.xml",
  198,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 62-->\r\n<!DOCTYPE animal SYSTEM \"ibm62n01.dtd\">\r\n<animal>\r\n <tiger/>\r\nNegative test. Test includeSect with include(Case sensitive)\r\n</animal>\r\n",
  0,
  NULL,
  "Tests includeSect with wrong key word. The string \"include\" is used as a key word in the beginning sequence in the includeSect in the file ibm62n01.dtd."
},
{
  "not-wf/P62/ibm62n02.xml",
  175,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 62-->\r\n<!DOCTYPE animal SYSTEM \"ibm62n02.dtd\">\r\n<animal>\r\n <tiger/>\r\nNegative test.  An extra '[' is used.\r\n</animal>\r\n",
  0,
  NULL,
  "Tests includeSect with wrong beginning sequence. An extra \"[\" occurs in the beginning sequence in the includeSect in the file ibm62n02.dtd."
},
{
  "not-wf/P62/ibm62n03.xml",
  184,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 62-->\r\n<!DOCTYPE animal SYSTEM \"ibm62n03.dtd\">\r\n<animal>\r\n <tiger/>\r\nNegative test.  Wrong character is used is used.\r\n</animal>",
  0,
  NULL,
  "Tests includeSect with wrong beginning sequence. A wrong character \"?\" occurs in the beginning sequence in the includeSect in the file ibm62n03.dtd."
},
{
  "not-wf/P62/ibm62n04.xml",
  187,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 62-->\r\n<!DOCTYPE animal SYSTEM \"ibm62n04.dtd\">\r\n<animal>\r\n <tiger/>\r\nNegative test.  Missing the required field INCLUDE.\r\n</animal>",
  0,
  NULL,
  "Tests includeSect with a required field missing. The key word \"INCLUDE\" is missing in the includeSect in the file ibm62n04.dtd."
},
{
  "not-wf/P62/ibm62n05.xml",
  197,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 62-->\r\n<!DOCTYPE animal SYSTEM \"ibm62n05.dtd\">\r\n<animal>\r\n <tiger/>\r\nNegative test.  Missing the required field '[' after INCLUDE.\r\n</animal>",
  0,
  NULL,
  "Tests includeSect with a required field missing. The \"[\" is missing after the key word \"INCLUDE\" in the includeSect in the file ibm62n05.dtd."
},
{
  "not-wf/P62/ibm62n06.xml",
  224,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 62-->\r\n<!DOCTYPE animal SYSTEM \"ibm62n06.dtd\">\r\n<animal>\r\n <tiger/>\r\nNegative test.  Wrong Ordering. External subset declaration prior to the keyword INCLUDE\r\n</animal>",
  0,
  NULL,
  "Tests includeSect with wrong field ordering. The two external subset declarations occur before the key word \"INCLUDE\" in the includeSect in the file ibm62n06.dtd."
},
{
  "not-wf/P62/ibm62n07.xml",
  176,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 62-->\r\n<!DOCTYPE animal SYSTEM \"ibm62n07.dtd\">\r\n<animal>\r\n <tiger/>\r\nNegative test. Missing closing sequence.\r\n</animal>",
  0,
  NULL,
  "Tests includeSect with a required field missing. The closing sequence \"]](greater than)\" is missing in the includeSect in the file ibm62n07.dtd."
},
{
  "not-wf/P62/ibm62n08.xml",
  187,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 62-->\r\n<!DOCTYPE animal SYSTEM \"ibm62n08.dtd\">\r\n<animal>\r\n <tiger/>\r\nNegative test. Missing external subset declaration.\r\n</animal>",
  0,
  NULL,
  "Tests includeSect with a required field missing. One \"]\" is missing in the closing sequence in the includeSect in the file ibm62n08.dtd."
},
{
  "not-wf/P63/ibm63n01.xml",
  299,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 63-->\r\n<!DOCTYPE animal SYSTEM \"ibm63n01.dtd\"\r\n[\r\n<!ELEMENT animal ANY>\r\n<!ELEMENT tiger (#PCDATA)>\r\n<!ATTLIST animal a (tiger) #REQUIRED> \r\n]>\r\n<animal a = \"TIGER1\">\r\nNegative test. Case sensitive(ignore is used instead of IGNORE).\r\n</animal>",
  0,
  NULL,
  "Tests ignoreSect with wrong key word. The string \"ignore\" is used as a key word in the beginning sequence in the ignoreSect in the file ibm63n01.dtd."
},
{
  "not-wf/P63/ibm63n02.xml",
  229,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 63-->\r\n<!DOCTYPE animal SYSTEM \"ibm63n02.dtd\"\r\n[\r\n<!ATTLIST attr a (tiger) #REQUIRED> \r\n]>\r\n<animal a = \"TIGER1\">\r\nNegative test. Extra '[' is used before IGNORE.\r\n</animal>",
  0,
  NULL,
  "Tests ignoreSect with wrong beginning sequence. An extra \"[\" occurs in the beginning sequence in the ignoreSect in the file ibm63n02.dtd."
},
{
  "not-wf/P63/ibm63n03.xml",
  267,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 63-->\r\n<!DOCTYPE animal SYSTEM \"ibm63n03.dtd\"\r\n[\r\n<!ELEMENT animal ANY>\r\n<!ELEMENT tiger (#PCDATA)>\r\n<!ATTLIST animal a (tiger) #REQUIRED> \r\n]>\r\n<animal a = \"TIGER1\">\r\nNegative test.  Wrong character.\r\n</animal>",
  0,
  NULL,
  "Tests ignoreSect with wrong beginning sequence. A wrong character \"?\" occurs in the beginning sequence in the ignoreSect in the file ibm63n03.dtd."
},
{
  "not-wf/P63/ibm63n04.xml",
  305,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 63-->\r\n<!DOCTYPE animal SYSTEM \"ibm63n04.dtd\"\r\n[\r\n<!ELEMENT animal ANY>\r\n<!ELEMENT tiger (#PCDATA)>\r\n<!ATTLIST animal a (tiger) #REQUIRED> \r\n]>\r\n<animal a = \"TIGER1\">\r\nNegative test.  Missing required field(The keyword IGNORE is missing).\r\n</animal>",
  0,
  NULL,
  "Tests ignoreSect with a required field missing. The key word \"IGNORE\" is missing in the ignoreSect in the file ibm63n04.dtd."
},
{
  "not-wf/P63/ibm63n05.xml",
  305,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 63-->\r\n<!DOCTYPE animal SYSTEM \"ibm63n05.dtd\"\r\n[\r\n<!ELEMENT animal ANY>\r\n<!ELEMENT tiger (#PCDATA)>\r\n<!ATTLIST animal a (tiger) #REQUIRED> \r\n]>\r\n<animal a = \"TIGER1\">\r\nNegative test.  Missing required field( '[' is missing after IGNORE ).\r\n</animal>",
  0,
  NULL,
  "Tests ignoreSect with a required field missing. The \"[\" is missing after the key word \"IGNORE\" in the ignoreSect in the file ibm63n05.dtd."
},
{
  "not-wf/P63/ibm63n06.xml",
  252,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 63-->\r\n<!DOCTYPE animal SYSTEM \"ibm63n06.dtd\"\r\n[\r\n<!ATTLIST attr a (tiger) #REQUIRED> \r\n]>\r\n<animal a = \"TIGER1\">\r\nNegative test.  Wrong Ordering. Ignore sect contents preceding IGNORE.\r\n</animal>",
  0,
  NULL,
  "Tests includeSect with wrong field ordering. The two external subset declarations occur before the key word \"IGNORE\" in the ignoreSect in the file ibm63n06.dtd."
},
{
  "not-wf/P63/ibm63n07.xml",
  276,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 63-->\r\n<!DOCTYPE animal SYSTEM \"ibm63n07.dtd\"\r\n[\r\n<!ELEMENT animal ANY>\r\n<!ELEMENT tiger (#PCDATA)>\r\n<!ATTLIST animal a (tiger) #REQUIRED> \r\n]>\r\n<animal a = \"TIGER1\">\r\nNegative test.  Missing closing sequence.\r\n</animal>",
  0,
  NULL,
  "Tests ignoreSect with a required field missing. The closing sequence \"]](greater than)\" is missing in the ignoreSect in the file ibm63n07.dtd."
},
{
  "not-wf/P64/ibm64n01.xml",
  195,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 64-->\r\n<!DOCTYPE animal SYSTEM \"ibm64n01.dtd\"\r\n[\r\n<!ELEMENT animal ANY>\r\n]>\r\n<animal>\r\nNegative Test. Pattern2. Wrong character.\r\n</animal>",
  0,
  NULL,
  "Tests ignoreSectContents with wrong beginning sequence. The \"?\" occurs in beginning sequence the ignoreSectContents in the file ibm64n01.dtd."
},
{
  "not-wf/P64/ibm64n02.xml",
  204,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 64-->\r\n<!DOCTYPE animal SYSTEM \"ibm64n02.dtd\"\r\n[\r\n<!ELEMENT animal ANY>\r\n]>\r\n<animal>\r\nNegative Test. Pattern3. Missing closing sequence.\r\n</animal>",
  0,
  NULL,
  "Tests ignoreSectContents with a required field missing.The closing sequence is missing in the ignoreSectContents in the file ibm64n02.dtd."
},
{
  "not-wf/P64/ibm64n03.xml",
  204,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 64-->\r\n<!DOCTYPE animal SYSTEM \"ibm64n03.dtd\"\r\n[\r\n<!ELEMENT animal ANY>\r\n]>\r\n<animal>\r\nNegative Test. Pattern4. Missing opening sequence.\r\n</animal>",
  0,
  NULL,
  "Tests ignoreSectContents with a required field missing.The beginning sequence is missing in the ignoreSectContents in the file ibm64n03.dtd."
},
{
  "not-wf/P65/ibm65n01.xml",
  202,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 65-->\r\n<!DOCTYPE animal SYSTEM \"ibm65n01.dtd\"\r\n[\r\n<!ELEMENT animal ANY>\r\n]>\r\n<animal>\r\nNegative Test. Pattern1.Illegal sequence of ']]'\r\n</animal>",
  0,
  NULL,
  "Tests Ignore with illegal string included. The string \"]](greater than)\" is contained before \"this\" in the Ignore in the ignoreSectContents in the file ibm65n01.dtd"
},
{
  "not-wf/P65/ibm65n02.xml",
  179,
  "<?xml  version=\"1.0\"?>\r\n<!-- syntax test for Production 65-->\r\n<!DOCTYPE animal SYSTEM \"ibm65n02.dtd\"\r\n[\r\n<!ELEMENT animal ANY>\r\n]>\r\n<animal>\r\nNegative Test. Pattern2. \r\n</animal>",
  0,
  NULL,
  "Tests Ignore with illegal string included. The string \"(less than)![\" is contained before \"this\" in the Ignore in the ignoreSectContents in the file ibm65n02.dtd"
},
{
  "not-wf/P66/ibm66n01.xml",
  143,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root EMPTY>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!ENTITY aaa \"wrong charater reference: &#002f;\">\r\n]>\r\n<root/>\r\n",
  0,
  NULL,
  "Tests CharRef with an illegal character referred to. The \"#002f\" is used as the referred character in the CharRef in the EntityDecl in the DTD."
},
{
  "not-wf/P66/ibm66n02.xml",
  135,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root EMPTY>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n\r\n]>\r\n<root att=\"wrong character reference: &#x003a\"/>\r\n",
  0,
  NULL,
  "Tests CharRef with the semicolon character missing. The semicolon character is missing at the end of the CharRef in the attribute value in the STag of element \"root\"."
},
{
  "not-wf/P66/ibm66n03.xml",
  140,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root EMPTY>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!ENTITY aaa \"wrong charater reference: &49;\">\r\n]>\r\n<root/>\r\n",
  0,
  NULL,
  "Tests CharRef with an illegal character referred to. The \"49\" is used as the referred character in the CharRef in the EntityDecl in the DTD."
},
{
  "not-wf/P66/ibm66n04.xml",
  132,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root EMPTY>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n\r\n]>\r\n<root att=\"wrong charater reference:&#5~0;\"/>\r\n",
  0,
  NULL,
  "Tests CharRef with an illegal character referred to. The \"#5~0\" is used as the referred character in the attribute value in the EmptyElemTag of the element \"root\"."
},
{
  "not-wf/P66/ibm66n05.xml",
  144,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root EMPTY>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!ENTITY aaa \"wrong charater reference: &#x002g;\">\r\n]>\r\n<root/>\r\n",
  0,
  NULL,
  "Tests CharRef with an illegal character referred to. The \"#x002g\" is used as the referred character in the CharRef in the EntityDecl in the DTD."
},
{
  "not-wf/P66/ibm66n06.xml",
  134,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root EMPTY>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n\r\n]>\r\n<root att=\"wrong charater reference:&#x006G;\"/>\r\n",
  0,
  NULL,
  "Tests CharRef with an illegal character referred to. The \"#x006G\" is used as the referred character in the attribute value in the EmptyElemTag of the element \"root\"."
},
{
  "not-wf/P66/ibm66n07.xml",
  144,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root EMPTY>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!ENTITY aaa \"wrong charater reference: &#x0=2f;\">\r\n]>\r\n<root/>\r\n",
  0,
  NULL,
  "Tests CharRef with an illegal character referred to. The \"#0=2f\" is used as the referred character in the CharRef in the EntityDecl in the DTD."
},
{
  "not-wf/P66/ibm66n08.xml",
  133,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root EMPTY>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n\r\n]>\r\n<root att=\"wrong charater reference:&#56.0;\"/>\r\n",
  0,
  NULL,
  "Tests CharRef with an illegal character referred to. The \"#56.0\" is used as the referred character in the attribute value in the EmptyElemTag of the element \"root\"."
},
{
  "not-wf/P66/ibm66n09.xml",
  145,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root EMPTY>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!ENTITY aaa \"wrong charater reference: &#x00/2f;\">\r\n]>\r\n<root/>\r\n",
  0,
  NULL,
  "Tests CharRef with an illegal character referred to. The \"#x00/2f\" is used as the referred character in the CharRef in the EntityDecl in the DTD."
},
{
  "not-wf/P66/ibm66n10.xml",
  133,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root EMPTY>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n\r\n]>\r\n<root att=\"wrong charater reference: &#51);\"/>\r\n",
  0,
  NULL,
  "Tests CharRef with an illegal character referred to. The \"#51)\" is used as the referred character in the attribute value in the EmptyElemTag of the element \"root\"."
},
{
  "not-wf/P66/ibm66n11.xml",
  145,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root EMPTY>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!ENTITY aaa \"wrong charater reference: &#x00 2f;\">\r\n]>\r\n<root/>\r\n",
  0,
  NULL,
  "Tests CharRef with an illegal character referred to. The \"#00 2f\" is used as the referred character in the CharRef in the EntityDecl in the DTD."
},
{
  "not-wf/P66/ibm66n12.xml",
  148,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root EMPTY>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n\r\n]>\r\n<root att=\"wrong replacement charater reference: &#x0000;\" />\r\n",
  0,
  NULL,
  "Tests CharRef with an illegal character referred to. The \"#x0000\" is used as the referred character in the attribute value in the EmptyElemTag of the element \"root\"."
},
{
  "not-wf/P66/ibm66n13.xml",
  138,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root EMPTY>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n\r\n]>\r\n<root att=\"wrong replacement charater: &#x001f;\" />\r\n",
  0,
  NULL,
  "Tests CharRef with an illegal character referred to. The \"#x001f\" is used as the referred character in the attribute value in the EmptyElemTag of the element \"root\"."
},
{
  "not-wf/P66/ibm66n14.xml",
  139,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root EMPTY>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n\r\n]>\r\n<root att=\"wrong replacement charater : &#xfffe;\" />\r\n",
  0,
  NULL,
  "Tests CharRef with an illegal character referred to. The \"#xfffe\" is used as the referred character in the attribute value in the EmptyElemTag of the element \"root\"."
},
{
  "not-wf/P66/ibm66n15.xml",
  138,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root EMPTY>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n\r\n]>\r\n<root att=\"wrong replacement charater: &#xffff;\" />\r\n",
  0,
  NULL,
  "Tests CharRef with an illegal character referred to. The \"#xffff\" is used as the referred character in the attribute value in the EmptyElemTag of the element \"root\"."
},
{
  "not-wf/P68/ibm68n01.xml",
  149,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!ENTITY aaa \"aString\">\r\n]>\r\n<root>missing entity name &;</root>\r\n",
  0,
  NULL,
  "Tests EntityRef with a required field missing. The Name is missing in the EntityRef in the content of the element \"root\"."
},
{
  "not-wf/P68/ibm68n02.xml",
  156,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!ENTITY aaa \"aString\">\r\n]>\r\n<root att=\"&aaa\">missing semi-colon</root>\r\n",
  0,
  NULL,
  "Tests EntityRef with a required field missing. The semicolon is missing in the EntityRef in the attribute value in the element \"root\"."
},
{
  "not-wf/P68/ibm68n03.xml",
  163,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!ENTITY aaa \"aString\">\r\n]>\r\n<root>extra space after ampsand & aaa;</root>\r\n\r\n\r\n",
  0,
  NULL,
  "Tests EntityRef with an extra white space. A white space occurs after the ampersand in the EntityRef in the content of the element \"root\"."
},
{
  "not-wf/P68/ibm68n04.xml",
  173,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!ENTITY aaa \"aString\">\r\n]>\r\n<root att=\"&aAa;\">reference doesn't match delaration</root>\r\n",
  0,
  NULL,
  "Tests EntityRef which is against P68 WFC: Entity Declared. The name \"aAa\" in the EntityRef in the AttValue in the STage of the element \"root\" does not match the Name of any declared entity in the DTD."
},
{
  "not-wf/P68/ibm68n05.xml",
  126,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n]>\r\n<root>undefined entitiy &aaa; </root>\r\n",
  0,
  NULL,
  "Tests EntityRef which is against P68 WFC: Entity Declared. The entity with the name \"aaa\" in the EntityRef in the AttValue in the STag of the element \"root\" is not declared."
},
{
  "not-wf/P68/ibm68n06.xml",
  225,
  "<?xml version=\"1.0\" standalone=\"yes\"?>\r\n<!DOCTYPE root SYSTEM \"ibm68n06.dtd\"\r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n\r\n]>\r\n<root att=\"&aaa;\">entity declared externally but standalone is yes</root>\r\n",
  0,
  NULL,
  "Tests EntityRef which is against P68 WFC: Entity Declared. The entity with the name \"aaa\" in the EntityRef in the AttValue in the STag of the element \"root\" is externally declared, but standalone is \"yes\"."
},
{
  "not-wf/P68/ibm68n07.xml",
  203,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!--* Entity referenced before declared *-->\r\n<!ATTLIST root att1 CDATA \"&aaa;\">\r\n<!ENTITY aaa \"aString\">\r\n]>\r\n<root/>\r\n",
  0,
  NULL,
  "Tests EntityRef which is against P68 WFC: Entity Declared. The entity with the name \"aaa\" in the EntityRef in the AttValue in the STag of the element \"root\" is referred before declared."
},
{
  "not-wf/P68/ibm68n08.xml",
  275,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!ENTITY aaa \"aString\">\r\n<!NOTATION JPGformat SYSTEM \"JPGFormat\">\r\n<!ENTITY aImage SYSTEM \"image.jpg\" NDATA JPGformat>\r\n]>\r\n<root>unparsed entity reference in the wrong place &aImage;</root>\r\n",
  0,
  NULL,
  "Tests EntityRef which is against P68 WFC: Parsed Entity. The EntityRef in the AttValue in the STag of the element \"root\" contains the name \"aImage\" of an unparsed entity."
},
{
  "not-wf/P68/ibm68n09.xml",
  194,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!--* recursive entity reference *-->\r\n<!ENTITY aaa \"&bbb;\">\r\n<!ENTITY bbb \"&aaa;\">\r\n]>\r\n<root>&aaa;</root>\r\n\r\n",
  0,
  NULL,
  "Tests EntityRef which is against P68 WFC: No Recursion. The recursive entity reference occurs with the entity declarations for \"aaa\" and \"bbb\" in the DTD."
},
{
  "not-wf/P68/ibm68n10.xml",
  274,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!--* indirect recursive entity reference *-->\r\n<!ENTITY aaa \"&bbb;\">\r\n<!ENTITY bbb \"&ccc;\">\r\n<!ENTITY ccc \"&ddd;\">\r\n<!ENTITY ddd \"&eee;\">\r\n<!ENTITY eee \"&aaa;\">\r\n]>\r\n<root>&aaa;</root>\r\n\r\n\r\n",
  0,
  NULL,
  "Tests EntityRef which is against P68 WFC: No Recursion. The indirect recursive entity reference occurs with the entity declarations for \"aaa\", \"bbb\", \"ccc\", \"ddd\", and \"eee\" in the DTD."
},
{
  "not-wf/P69/ibm69n01.xml",
  179,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ENTITY % paaa \"<!ATTLIST root att CDATA #IMPLIED>\">\r\n<!--* incorrect PE reference *-->\r\n%;\r\n<!ENTITY aaa \"aString\">\r\n]>\r\n<root/>\r\n",
  0,
  NULL,
  "Tests PEReference with a required field missing. The Name \"paaa\" is missing in the PEReference in the DTD."
},
{
  "not-wf/P69/ibm69n02.xml",
  200,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ENTITY % paaa \"<!ATTLIST root att CDATA #IMPLIED>\">\r\n<!--* incorrect PE reference without semicolon *-->\r\n%paaa\r\n<!ENTITY aaa \"aString\">\r\n]>\r\n<root/>\r\n",
  0,
  NULL,
  "Tests PEReference with a required field missing. The semicolon is missing in the PEReference \"%paaa\" in the DTD."
},
{
  "not-wf/P69/ibm69n03.xml",
  224,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ENTITY % paaa \"<!ATTLIST root att CDATA #IMPLIED>\">\r\n<!--* incorrect PE reference with a extra white space charater *-->\r\n%paaa ;\r\n<!ENTITY aaa \"aString\">\r\n]>\r\n<root/>\r\n\r\n\r\n\r\n",
  0,
  NULL,
  "Tests PEReference with an extra white space. There is an extra white space occurs before \";\" in the PEReference in the DTD."
},
{
  "not-wf/P69/ibm69n04.xml",
  214,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ENTITY % paaa \"<!ATTLIST root att CDATA #IMPLIED>\">\r\n<!--* incorrect PE reference with a extra white space char *-->\r\n% paaa;\r\n<!ENTITY aaa \"aString\">\r\n]>\r\n<root/>\r\n",
  0,
  NULL,
  "Tests PEReference with an extra white space. There is an extra white space occurs after \"%\" in the PEReference in the DTD."
},
{
  "not-wf/P69/ibm69n05.xml",
  259,
  "<?xml version=\"1.0\" standalone=\"yes\"?>\r\n<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!--* PE referenced before declared, against WFC: entity declared -->\r\n%paaa;\r\n<!ENTITY % paaa \"<!ATTLIST root att CDATA #IMPLIED>\">\r\n<!ENTITY aaa \"aString\">\r\n]>\r\n<root/>\r\n",
  0,
  NULL,
  "Based on E29 substantial source: minutes XML-Syntax 1999-02-24 E38 in XML 1.0 Errata, this WFC does not apply to P69, but the VC Entity declared still apply. Tests PEReference which is against P69 WFC: Entity Declared. The PE with the name \"paaa\" is referred before declared in the DTD."
},
{
  "not-wf/P69/ibm69n06.xml",
  144,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!--* recursive PE reference -->\r\n<!ENTITY % paaa \"&bbb;\">\r\n<!ENTITY bbb \"%paaa;\">\r\n]>\r\n<root/>\r\n",
  0,
  NULL,
  "Tests PEReference which is against P69 WFC: No Recursion. The recursive PE reference occurs with the entity declarations for \"paaa\" and \"bbb\" in the DTD."
},
{
  "not-wf/P69/ibm69n07.xml",
  224,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!--* indirect recursive PE reference -->\r\n<!ENTITY % paaa \"&bbb;\">\r\n<!ENTITY bbb \"&ccc;\">\r\n<!ENTITY ccc \"&ddd;\">\r\n<!ENTITY ddd \"&eee;\">\r\n<!ENTITY eee \"%paaa;\">\r\n]>\r\n<root/>\r\n\r\n",
  0,
  NULL,
  "Tests PEReference which is against P69 WFC: No Recursion. The indirect recursive PE reference occurs with the entity declarations for \"paaa\", \"bbb\", \"ccc\", \"ddd\", and \"eee\" in the DTD."
},
{
  "not-wf/P71/ibm70n01.xml",
  197,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!ENTITY aaa \"aString\">\r\n<!--* mess up Entity Declaration *-->\r\n<root/>\r\n<!ENTITY % paaa \"aString\">\r\n]>\r\n<root/>\r\n",
  0,
  NULL,
  "Tests"
},
{
  "not-wf/P71/ibm71n01.xml",
  162,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!--* missing space  *-->\r\n<!ENTITYaaa \"aString\">\r\n\r\n]>\r\n<root>&aaa;</root>\r\n\r\n",
  0,
  NULL,
  "Tests EntityDecl with a required field missing. The white space is missing between the beginning sequence and the Name \"aaa\" in the EntityDecl in the DTD."
},
{
  "not-wf/P71/ibm71n02.xml",
  160,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!--* missing space  *-->\r\n<!ENTITY aaa\"aString\">\r\n\r\n]>\r\n<root>&aaa;</root>\r\n",
  0,
  NULL,
  "Tests EntityDecl with a required field missing. The white space is missing between the Name \"aaa\" and the EntityDef \"aString\" in the EntityDecl in the DTD."
},
{
  "not-wf/P71/ibm71n03.xml",
  155,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!--* missing EntityDef  *-->\r\n<!ENTITY aaa>\r\n\r\n]>\r\n<root>&aaa;</root>\r\n",
  0,
  NULL,
  "Tests EntityDecl with a required field missing. The EntityDef is missing in the EntityDecl with the Name \"aaa\" in the DTD."
},
{
  "not-wf/P71/ibm71n04.xml",
  156,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!--* missing name  *-->\r\n<!ENTITY \"aString\">\r\n\r\n]>\r\n<root>&aaa;</root>\r\n",
  0,
  NULL,
  "Tests EntityDecl with a required field missing. The Name is missing in the EntityDecl with the EntityDef \"aString\" in the DTD."
},
{
  "not-wf/P71/ibm71n05.xml",
  160,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!--* wrong ordering  *-->\r\n<!ENTITY \"aString\" aaa>\r\n]>\r\n<root>&aaa;</root>\r\n",
  0,
  NULL,
  "Tests EntityDecl with wrong ordering. The Name \"aaa\" occurs after the EntityDef in the EntityDecl in the DTD."
},
{
  "not-wf/P71/ibm71n06.xml",
  166,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!--* ENTITY in lower case  *-->\r\n<!entity aaa \"aString\">\r\n]>\r\n<root>&aaa;</root>\r\n",
  0,
  NULL,
  "Tests EntityDecl with wrong key word. The string \"entity\" is used as the key word in the beginning sequence in the EntityDecl in the DTD."
},
{
  "not-wf/P71/ibm71n07.xml",
  170,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!--* missing closing bracket  *-->\r\n<!ENTITY aaa \"aString\"\r\n\r\n]>\r\n<root>&aaa;</root>\r\n",
  0,
  NULL,
  "Tests EntityDecl with a required field missing. The closing bracket (greater than) is missing in the EntityDecl in the DTD."
},
{
  "not-wf/P71/ibm71n08.xml",
  171,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!--* exclamation mark missing  *-->\r\n<ENTITY aaa \"aString\">\r\n\r\n]>\r\n<root>&aaa;</root>\r\n",
  0,
  NULL,
  "Tests EntityDecl with a required field missing. The exclamation mark is missing in the beginning sequence in the EntityDecl in the DTD."
},
{
  "not-wf/P72/ibm72n01.xml",
  178,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!--* missing space  *-->\r\n<!ENTITY% paaa \"<!-- comments -->\">\r\n%paaa;\r\n]>\r\n<root/>\r\n\r\n\r\n\r\n\r\n\r\n",
  0,
  NULL,
  "Tests PEdecl with a required field missing. The white space is missing between the beginning sequence and the \"%\" in the PEDecl in the DTD."
},
{
  "not-wf/P72/ibm72n02.xml",
  163,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!--* missing name  *-->\r\n<!ENTITY % \"<!-- comments -->\">\r\n%paaa;\r\n]>\r\n<root/>\r\n",
  0,
  NULL,
  "Tests PEdecl with a required field missing. The Name is missing in the PEDecl in the DTD."
},
{
  "not-wf/P72/ibm72n03.xml",
  168,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!--* missing space  *-->\r\n<!ENTITY % paaa\"<!-- comments -->\">\r\n%paaa;\r\n]>\r\n<root/>\r\n",
  0,
  NULL,
  "Tests PEdecl with a required field missing. The white space is missing between the Name and the PEDef in the PEDecl in the DTD."
},
{
  "not-wf/P72/ibm72n04.xml",
  149,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!--* missing PEDef  *-->\r\n<!ENTITY % paaa>\r\n%paaa;\r\n]>\r\n<root/>\r\n",
  0,
  NULL,
  "Tests PEdecl with a required field missing. The PEDef is missing after the Name \"paaa\" in the PEDecl in the DTD."
},
{
  "not-wf/P72/ibm72n05.xml",
  167,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!--* wrong order  *-->\r\n<!ENTITY % \"<!-- comments -->\" paaa>\r\n%paaa;\r\n]>\r\n<root/>\r\n",
  0,
  NULL,
  "Tests PEdecl with wrong field ordering. The Name \"paaa\" occurs after the PEDef in the PEDecl in the DTD."
},
{
  "not-wf/P72/ibm72n06.xml",
  168,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!--* wrong order  *-->\r\n<!ENTITY \"<!-- comments -->\" % paaa >\r\n%paaa;\r\n]>\r\n<root/>\r\n",
  0,
  NULL,
  "Tests PEdecl with wrong field ordering. The \"%\" and the Name \"paaa\" occurs after the PEDef in the PEDecl in the DTD."
},
{
  "not-wf/P72/ibm72n07.xml",
  169,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!--* wrong keyword  *-->\r\n<!entity % paaa \"<!-- comments -->\">\r\n%paaa;\r\n]>\r\n<root/>\r\n",
  0,
  NULL,
  "Tests PEdecl with wrong key word. The string \"entity\" is used as the key word in the beginning sequence in the PEDecl in the DTD."
},
{
  "not-wf/P72/ibm72n08.xml",
  178,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!--* missing closing bracket  *-->\r\n<!ENTITY % paaa \"<!-- comments -->\"\r\n%paaa;\r\n]>\r\n<root/>\r\n",
  0,
  NULL,
  "Tests PEdecl with a required field missing. The closing bracket (greater than) is missing in the PEDecl in the DTD."
},
{
  "not-wf/P72/ibm72n09.xml",
  202,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!--* wrong closing sequence: extra exclamation mark *-->\r\n<!ENTITY% paaa \"<!-- comments -->\" !>\r\n%paaa;\r\n]>\r\n<root/>\r\n",
  0,
  NULL,
  "Tests PEdecl with wrong closing sequence. The string \"!(greater than)\" is used as the closing sequence in the PEDecl in the DTD."
},
{
  "not-wf/P73/ibm73n01.xml",
  239,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!NOTATION JPGformat SYSTEM \"JPGFormat\">\r\n<!--* wrong order: NDataDecl ExternalID  *-->\r\n<!ENTITY aImage NDATA JPGformat SYSTEM \"image.jpg\" >\r\n]>\r\n<root/>\r\n",
  0,
  NULL,
  "Tests EntityDef with wrong field ordering. The NDataDecl \"NDATA JPGformat\" occurs before the ExternalID in the EntityDef in the EntityDecl."
},
{
  "not-wf/P73/ibm73n03.xml",
  205,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!NOTATION JPGformat SYSTEM \"JPGFormat\">\r\n<!--* missing ExternalID  *-->\r\n<!ENTITY aImage NDATA JPGformat >\r\n]>\r\n<root/>\r\n",
  0,
  NULL,
  "Tests EntityDef with a required field missing. The ExternalID is missing before the NDataDecl in the EntityDef in the EntityDecl."
},
{
  "not-wf/P74/ibm74n01.xml",
  240,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!NOTATION JPGformat SYSTEM \"JPGFormat\">\r\n<!--* wrong PEDef: NDataDecl ExternalID  *-->\r\n<!ENTITY % pImage SYSTEM \"image.jpg\" NDATA JPGformat>\r\n]>\r\n<root/>\r\n",
  0,
  NULL,
  "Tests PEDef with extra fields. The NDataDecl occurs after the ExternalID in the PEDef in the PEDecl in the DTD."
},
{
  "not-wf/P75/ibm75n01.xml",
  168,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!--* wrong keyword: system  *-->\r\n<!ENTITY pImage system \"image.jpg\">\r\n]>\r\n<root/>\r\n",
  0,
  NULL,
  "Tests ExternalID with wrong key word. The string \"system\" is used as the key word in the ExternalID in the EntityDef in the EntityDecl."
},
{
  "not-wf/P75/ibm75n02.xml",
  196,
  "<?xml version=\"1.0\"?>\r\n<!--* wrong keyword: public  *-->\r\n<!DOCTYPE root \r\n    public \"-//W3C//DTD//EN\" \"empty.dtd\"\r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n]>\r\n<root/>\r\n",
  0,
  NULL,
  "Tests ExternalID with wrong key word. The string \"public\" is used as the key word in the ExternalID in the doctypedecl."
},
{
  "not-wf/P75/ibm75n03.xml",
  196,
  "<?xml version=\"1.0\"?>\r\n<!--* wrong keyword: Public  *-->\r\n<!DOCTYPE root \r\n    Public \"-//W3C//DTD//EN\" \"empty.dtd\"\r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n]>\r\n<root/>\r\n",
  0,
  NULL,
  "Tests ExternalID with wrong key word. The string \"Public\" is used as the key word in the ExternalID in the doctypedecl."
},
{
  "not-wf/P75/ibm75n04.xml",
  185,
  "<?xml version=\"1.0\"?>\r\n<!--* wrong order *-->\r\n<!DOCTYPE root \r\n    \"-//W3C//DTD//EN\" \"empty.dtd\" PUBLIC\r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n]>\r\n<root/>\r\n",
  0,
  NULL,
  "Tests ExternalID with wrong field ordering. The key word \"PUBLIC\" occurs after the PublicLiteral and the SystemLiteral in the ExternalID in the doctypedecl."
},
{
  "not-wf/P75/ibm75n05.xml",
  158,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!--* missing space *-->\r\n<!ENTITY pImage SYSTEM\"image.jpg\">\r\n]>\r\n<root/>\r\n",
  0,
  NULL,
  "Tests ExternalID with a required field missing. The white space between \"SYSTEM\" and the Systemliteral is missing in the ExternalID in the EntityDef in the EntityDecl in the DTD."
},
{
  "not-wf/P75/ibm75n06.xml",
  157,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!--* missing SystemLiterral *-->\r\n<!ENTITY pImage SYSTEM >\r\n]>\r\n<root/>\r\n",
  0,
  NULL,
  "Tests ExternalID with a required field missing. The Systemliteral is missing after \"SYSTEM\" in the ExternalID in the EntityDef in the EntityDecl in the DTD."
},
{
  "not-wf/P75/ibm75n07.xml",
  187,
  "<?xml version=\"1.0\"?>\r\n<!--* missing space  *-->\r\n<!DOCTYPE root \r\n    PUBLIC \"-//W3C//DTD//EN\"\"empty.dtd\"\r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n]>\r\n<root/>\r\n",
  0,
  NULL,
  "Tests ExternalID with a required field missing. The white space between the PublicLiteral and the Systemliteral is missing in the ExternalID in the doctypedecl."
},
{
  "not-wf/P75/ibm75n08.xml",
  191,
  "<?xml version=\"1.0\"?>\r\n<!--* missing keyword: PUBLIC  *-->\r\n<!DOCTYPE root \r\n    \"-//W3C//DTD//EN\" \"empty.dtd\"\r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n]>\r\n<root/>\r\n",
  0,
  NULL,
  "Tests ExternalID with a required field missing. The key word \"PUBLIC\" is missing in the ExternalID in the doctypedecl."
},
{
  "not-wf/P75/ibm75n09.xml",
  187,
  "<?xml version=\"1.0\"?>\r\n<!--* missing space  *-->\r\n<!DOCTYPE root \r\n    PUBLIC\"-//W3C//DTD//EN\" \"empty.dtd\"\r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n]>\r\n<root/>\r\n",
  0,
  NULL,
  "Tests ExternalID with a required field missing. The white space between \"PUBLIC\" and the PublicLiteral is missing in the ExternalID in the doctypedecl."
},
{
  "not-wf/P75/ibm75n10.xml",
  179,
  "<?xml version=\"1.0\"?>\r\n<!--* missing PubidLiteral  *-->\r\n<!DOCTYPE root \r\n    PUBLIC \".\\empty.dtd\"\r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n]>\r\n<root/>\r\n",
  0,
  NULL,
  "Tests ExternalID with a required field missing. The PublicLiteral is missing in the ExternalID in the doctypedecl."
},
{
  "not-wf/P75/ibm75n11.xml",
  184,
  "<?xml version=\"1.0\"?>\r\n<!--* missing System Literal *-->\r\n<!DOCTYPE root \r\n    public \"-//W3C//DTD//EN\"\r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n]>\r\n<root/>\r\n",
  0,
  NULL,
  "Tests ExternalID with a required field missing. The PublicLiteral is missing in the ExternalID in the doctypedecl."
},
{
  "not-wf/P75/ibm75n12.xml",
  157,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!--* wrong order *-->\r\n<!ENTITY pImage \"image.jpg\" SYSTEM>\r\n]>\r\n<root/>\r\n",
  0,
  NULL,
  "Tests ExternalID with a required field missing. The SystemLiteral is missing in the ExternalID in the doctypedecl."
},
{
  "not-wf/P75/ibm75n13.xml",
  185,
  "<?xml version=\"1.0\"?>\r\n<!--* wrong order *-->\r\n<!DOCTYPE root \r\n    \"-//W3C//DTD//EN\" PUBLIC \"empty.dtd\"\r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n]>\r\n<root/>\r\n",
  0,
  NULL,
  "Tests ExternalID with wrong field ordering. The key word \"PUBLIC\" occurs after the PublicLiteral in the ExternalID in the doctypedecl."
},
{
  "not-wf/P76/ibm76n01.xml",
  260,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!NOTATION JPGformat SYSTEM \"JPGFormat\">\r\n<!--* wrong keyword in NdataDecl: ndata *-->\r\n<!ENTITY aImage SYSTEM \"image.jpg\" ndata JPGformat>\r\n]>\r\n<root/>\r\n",
  0,
  NULL,
  "Tests NDataDecl with wrong key word. The string \"ndata\" is used as the key word in the NDataDecl in the EntityDef in the GEDecl."
},
{
  "not-wf/P76/ibm76n02.xml",
  260,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!NOTATION JPGformat SYSTEM \"JPGFormat\">\r\n<!--* wrong keyword in NdataDecl: NData *-->\r\n<!ENTITY aImage SYSTEM \"image.jpg\" NData JPGformat>\r\n]>\r\n<root/>\r\n",
  0,
  NULL,
  "Tests NDataDecl with wrong key word. The string \"NData\" is used as the key word in the NDataDecl in the EntityDef in the GEDecl."
},
{
  "not-wf/P76/ibm76n03.xml",
  252,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!NOTATION JPGformat SYSTEM \"JPGFormat\">\r\n<!--* missing space in NdataDecl *-->\r\n<!ENTITY aImage SYSTEM \"image.jpg\"NDATA JPGformat>\r\n]>\r\n<root/>\r\n",
  0,
  NULL,
  "Tests NDataDecl with a required field missing. The leading white space is missing in the NDataDecl in the EntityDef in the GEDecl."
},
{
  "not-wf/P76/ibm76n04.xml",
  257,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!NOTATION JPGformat SYSTEM \"JPGFormat\">\r\n<!--* missing keyword in NdataDecl : NDATA *-->\r\n<!ENTITY aImage SYSTEM \"image.jpg\" JPGformat>\r\n]>\r\n<root/>\r\n",
  0,
  NULL,
  "Tests NDataDecl with a required field missing. The key word \"NDATA\" is missing in the NDataDecl in the EntityDef in the GEDecl."
},
{
  "not-wf/P76/ibm76n05.xml",
  248,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!NOTATION JPGformat SYSTEM \"JPGFormat\">\r\n<!--* Missing Name field in NdataDecl *-->\r\n<!ENTITY aImage SYSTEM \"image.jpg\" NDATA>\r\n]>\r\n<root/>\r\n",
  0,
  NULL,
  "Tests NDataDecl with a required field missing. The Name after the key word \"NDATA\" is missing in the NDataDecl in the EntityDef in the GEDecl."
},
{
  "not-wf/P76/ibm76n06.xml",
  252,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!NOTATION JPGformat SYSTEM \"JPGFormat\">\r\n<!--* missing space in NdataDecl *-->\r\n<!ENTITY aImage SYSTEM \"image.jpg\" NDATAJPGformat>\r\n]>\r\n<root/>\r\n",
  0,
  NULL,
  "Tests NDataDecl with a required field missing. The white space between \"NDATA\" and the Name is missing in the NDataDecl in the EntityDef in the GEDecl."
},
{
  "not-wf/P76/ibm76n07.xml",
  251,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!NOTATION JPGformat SYSTEM \"JPGFormat\">\r\n<!--* wrong order in NdataDecl *-->\r\n<!ENTITY aImage SYSTEM \"image.jpg\" JPGformat NDATA>\r\n]>\r\n<root/>\r\n",
  0,
  NULL,
  "Tests NDataDecl with wrong field ordering. The key word \"NDATA\" occurs after the Name in the NDataDecl in the EntityDef in the GEDecl."
},
{
  "not-wf/P77/ibm77n01.xml",
  194,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!--* error in external entity  *-->\r\n<!ENTITY aExternal SYSTEM \"ibm77n01.ent\">\r\n]>\r\n<root>&aExternal;</root>\r\n",
  0,
  NULL,
  "Tests TextDecl with wrong field ordering. The VersionInfo occurs after the EncodingDecl in the TextDecl in the file \"ibm77n01.ent\"."
},
{
  "not-wf/P77/ibm77n02.xml",
  194,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!--* error in external entity  *-->\r\n<!ENTITY aExternal SYSTEM \"ibm77n02.ent\">\r\n]>\r\n<root>&aExternal;</root>\r\n",
  0,
  NULL,
  "Tests TextDecl with wrong key word. The string \"XML\" is used in the beginning sequence in the TextDecl in the file \"ibm77n02.ent\"."
},
{
  "not-wf/P77/ibm77n03.xml",
  192,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!--* error in external entity  *-->\r\n<!ENTITY % pExternal SYSTEM \"ibm77n03.ent\">\r\n%pExternal;\r\n]>\r\n<root/>\r\n",
  0,
  NULL,
  "Tests TextDecl with wrong closing sequence. The character \"greater than\" is used as the closing sequence in the TextDecl in the file \"ibm77n03.ent\"."
},
{
  "not-wf/P77/ibm77n04.xml",
  192,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!--* error in external entity  *-->\r\n<!ENTITY % pExternal SYSTEM \"ibm77n04.ent\">\r\n%pExternal;\r\n]>\r\n<root/>\r\n",
  0,
  NULL,
  "Tests TextDecl with a required field missing. The closing sequence is missing in the TextDecl in the file \"ibm77n04.ent\"."
},
{
  "not-wf/P78/ibm78n01.xml",
  200,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!--* error in external entity  *-->\r\n<!ENTITY aExternal SYSTEM \"ibm78n01.ent\">\r\n]>\r\n<root>&aExternal;</root>\r\n\r\n\r\n\r\n",
  0,
  NULL,
  "Tests extParsedEnt with wrong field ordering. The TextDecl occurs after the content in the file ibm78n01.ent."
},
{
  "not-wf/P78/ibm78n02.xml",
  194,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!--* error in external entity  *-->\r\n<!ENTITY aExternal SYSTEM \"ibm78n02.ent\">\r\n]>\r\n<root>&aExternal;</root>\r\n",
  0,
  NULL,
  "Tests extParsedEnt with extra field. A blank line occurs before the TextDecl in the file ibm78n02.ent."
},
{
  "not-wf/P79/ibm79n01.xml",
  192,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!--* error in external entity  *-->\r\n<!ENTITY % pExternal SYSTEM \"ibm79n01.ent\">\r\n%pExternal;\r\n]>\r\n<root/>\r\n",
  0,
  NULL,
  "Tests extPE with wrong field ordering. The TextDecl occurs after the extSubsetDecl (the white space and the comment) in the file ibm79n01.ent."
},
{
  "not-wf/P79/ibm79n02.xml",
  192,
  "<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!--* error in external entity  *-->\r\n<!ENTITY % pExternal SYSTEM \"ibm79n02.ent\">\r\n%pExternal;\r\n]>\r\n<root/>\r\n",
  0,
  NULL,
  "Tests extPE with extra field. A blank line occurs before the TextDecl in the file ibm78n02.ent."
},
{
  "not-wf/P80/ibm80n01.xml",
  189,
  "<?xml version=\"1.0\"encoding=\"UTF-8\"?>\r\n<!--* missing white space in above EncodingDecl *-->\r\n<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n]>\r\n<root/>\r\n",
  0,
  NULL,
  "Tests EncodingDecl with a required field missing. The leading white space is missing in the EncodingDecl in the XMLDecl."
},
{
  "not-wf/P80/ibm80n02.xml",
  181,
  "<?xml version=\"1.0\" encoding \"UTF-8\"?>\r\n<!--* missing Eq in above EncodingDecl *-->\r\n<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n]>\r\n<root/>\r\n",
  0,
  NULL,
  "Tests EncodingDecl with a required field missing. The \"=\" sign is missing in the EncodingDecl in the XMLDecl."
},
{
  "not-wf/P80/ibm80n03.xml",
  180,
  "<?xml version=\"1.0\" encoding= ?>\r\n<!--* missing EncName in above EncodingDecl *-->\r\n<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n]>\r\n<root/>\r\n",
  0,
  NULL,
  "Tests EncodingDecl with a required field missing. The double quoted EncName are missing in the EncodingDecl in the XMLDecl."
},
{
  "not-wf/P80/ibm80n04.xml",
  185,
  "<?xml version=\"1.0\" \"UTF-8\"encoding=?>\r\n<!--* wrong ordering in above EncodingDecl *-->\r\n<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n]>\r\n<root/>\r\n",
  0,
  NULL,
  "Tests EncodingDecl with wrong field ordering. The string \"encoding=\" occurs after the double quoted EncName in the EncodingDecl in the XMLDecl."
},
{
  "not-wf/P80/ibm80n05.xml",
  185,
  "<?xml version=\"1.0\" \"UTF-8\"=encoding?>\r\n<!--* wrong ordering in above EncodingDecl *-->\r\n<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n]>\r\n<root/>\r\n",
  0,
  NULL,
  "Tests EncodingDecl with wrong field ordering. The \"encoding\" occurs after the double quoted EncName in the EncodingDecl in the XMLDecl."
},
{
  "not-wf/P80/ibm80n06.xml",
  193,
  "<?xml version=\"1.0\" Encoding=\"UTF-8\"?>\r\n<!--* Wrong keyword Encoding in above EncodingDecl *-->\r\n<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n]>\r\n<root/>\r\n",
  0,
  NULL,
  "Tests EncodingDecl with wrong key word. The string \"Encoding\" is used as the key word in the EncodingDecl in the XMLDecl."
},
{
  "not-wf/P81/ibm81n01.xml",
  192,
  "<?xml version=\"1.0\" encoding=\"_UTF-8\"?>\r\n<!--* Illegal inital Charater in above EncName *-->\r\n<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n]>\r\n<root/>\r\n\r\n",
  0,
  NULL,
  "Tests EncName with an illegal character. The \"_\" is used as the first character in the EncName in the EncodingDecl in the XMLDecl."
},
{
  "not-wf/P81/ibm81n02.xml",
  190,
  "<?xml version=\"1.0\" encoding=\"-UTF-8\"?>\r\n<!--* Illegal inital Charater in above EncName *-->\r\n<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n]>\r\n<root/>\r\n",
  0,
  NULL,
  "Tests EncName with an illegal character. The \"-\" is used as the first character in the EncName in the EncodingDecl in the XMLDecl."
},
{
  "not-wf/P81/ibm81n03.xml",
  190,
  "<?xml version=\"1.0\" encoding=\".UTF-8\"?>\r\n<!--* Illegal inital Charater in above EncName *-->\r\n<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n]>\r\n<root/>\r\n",
  0,
  NULL,
  "Tests EncName with an illegal character. The \".\" is used as the first character in the EncName in the EncodingDecl in the XMLDecl."
},
{
  "not-wf/P81/ibm81n04.xml",
  190,
  "<?xml version=\"1.0\" encoding=\"8-UTF\"?>\r\n<!--* Illegal initial Charater in above EncName *-->\r\n<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n]>\r\n<root/>\r\n",
  0,
  NULL,
  "Tests EncName with illegal characters. The \"8-\" is used as the initial characters in the EncName in the EncodingDecl in the XMLDecl."
},
{
  "not-wf/P81/ibm81n05.xml",
  182,
  "<?xml version=\"1.0\" encoding=\"UTF~8\"?>\r\n<!--* Illegal Charater in above EncName *-->\r\n<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n]>\r\n<root/>\r\n",
  0,
  NULL,
  "Tests EncName with an illegal character. The \"~\" is used as one character in the EncName in the EncodingDecl in the XMLDecl."
},
{
  "not-wf/P81/ibm81n06.xml",
  182,
  "<?xml version=\"1.0\" encoding=\"UTF#8\"?>\r\n<!--* Illegal Charater in above EncName *-->\r\n<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n]>\r\n<root/>\r\n",
  0,
  NULL,
  "Tests EncName with an illegal character. The \"#\" is used as one character in the EncName in the EncodingDecl in the XMLDecl."
},
{
  "not-wf/P81/ibm81n07.xml",
  181,
  "<?xml version=\"1.0\" encoding=\"UTF:8\"?>\r\n<!--* IllegalCharater in above EncName *-->\r\n<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n]>\r\n<root/>\r\n",
  0,
  NULL,
  "Tests EncName with an illegal character. The \":\" is used as one character in the EncName in the EncodingDecl in the XMLDecl."
},
{
  "not-wf/P81/ibm81n08.xml",
  182,
  "<?xml version=\"1.0\" encoding=\"UTF/8\"?>\r\n<!--* Illegal Charater in above EncName *-->\r\n<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n]>\r\n<root/>\r\n",
  0,
  NULL,
  "Tests EncName with an illegal character. The \"/\" is used as one character in the EncName in the EncodingDecl in the XMLDecl."
},
{
  "not-wf/P81/ibm81n09.xml",
  182,
  "<?xml version=\"1.0\" encoding=\"UTF;8\"?>\r\n<!--* Illegal Charater in above EncName *-->\r\n<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n]>\r\n<root/>\r\n",
  0,
  NULL,
  "Tests EncName with an illegal character. The \";\" is used as one character in the EncName in the EncodingDecl in the XMLDecl."
},
{
  "not-wf/P82/ibm82n01.xml",
  260,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!--* Missing whitespace in NotationDecl *-->\r\n<!NOTATIONJPGformat SYSTEM \"JPGFormat\">\r\n<!ENTITY aImage SYSTEM \"image.jpg\" NDATA JPGformat>\r\n]>\r\n<root/>\r\n",
  0,
  NULL,
  "Tests NotationDecl with a required field missing. The white space after the beginning sequence of the NotationDecl is missing in the DTD."
},
{
  "not-wf/P82/ibm82n02.xml",
  258,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!--* Missing Name in NotationDecl *-->\r\n<!NOTATION PUBLIC \"-//JPG//DTD//JPGFormat\">\r\n<!ENTITY aImage SYSTEM \"image.jpg\" NDATA JPGformat>\r\n]>\r\n<root/>\r\n",
  0,
  NULL,
  "Tests NotationDecl with a required field missing. The Name in the NotationDecl is missing in the DTD."
},
{
  "not-wf/P82/ibm82n03.xml",
  255,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!--* Missing ExternalID or PublicID in NotationDecl *-->\r\n<!NOTATION JPGformat >\r\n<!ENTITY aImage SYSTEM \"image.jpg\" NDATA JPGformat>\r\n]>\r\n<root/>\r\n",
  0,
  NULL,
  "Tests NotationDecl with a required field missing. The externalID or the PublicID is missing in the NotationDecl in the DTD."
},
{
  "not-wf/P82/ibm82n04.xml",
  258,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!--* Wrong ordering in NotationDecl *-->\r\n<!NOTATION SYSTEM \"JPGFormat\" JPGformat >\r\n<!ENTITY aImage SYSTEM \"image.jpg\" NDATA JPGformat>\r\n]>\r\n<root/>\r\n",
  0,
  NULL,
  "Tests NotationDecl with wrong field ordering. The Name occurs after the \"SYSTEM\" and the externalID in the NotationDecl in the DTD."
},
{
  "not-wf/P82/ibm82n05.xml",
  266,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!--* Wrong keyword: notation in NotationDecl *-->\r\n<!notation JPGformat SYSTEM \"JPGFormat\">\r\n<!ENTITY aImage SYSTEM \"image.jpg\" NDATA JPGformat>\r\n]>\r\n<root/>\r\n",
  0,
  NULL,
  "Tests NotationDecl with wrong key word. The string \"notation\" is used as a key word in the NotationDecl in the DTD."
},
{
  "not-wf/P82/ibm82n06.xml",
  278,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!--* Missing closing bracket in NotationDecl *-->\r\n<!NOTATION JPGformat PUBLIC \"-//JPG//DTD//JPGFormat\"\r\n<!ENTITY aImage SYSTEM \"image.jpg\" NDATA JPGformat>\r\n]>\r\n<root/>\r\n",
  0,
  NULL,
  "Tests NotationDecl with a required field missing. The closing bracket (the greater than character) is missing in the NotationDecl."
},
{
  "not-wf/P82/ibm82n07.xml",
  295,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!--* Wrong beginning sequence in NotationDecl *-->\r\n<NOTATION JPGformat PUBLIC \"-//JPG//DTD//JPGFormat\">\r\n<!ENTITY aImage SYSTEM \"image.jpg\" NDATA JPGformat>\r\n]>\r\n<root/>\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n",
  0,
  NULL,
  "Tests NotationDecl with wrong beginning sequence. The \"!\" is missing in the beginning sequence in the NotationDecl in the DTD."
},
{
  "not-wf/P82/ibm82n08.xml",
  266,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!--* Wrong Closing sequence in NotationDecl *-->\r\n<!NOTATION JPGformat SYSTEM \"JPGFormat\"!>\r\n<!ENTITY aImage SYSTEM \"image.jpg\" NDATA JPGformat>\r\n]>\r\n<root/>\r\n",
  0,
  NULL,
  "Tests NotationDecl with wrong closing sequence. The extra \"!\" occurs in the closing sequence in the NotationDecl in the DTD."
},
{
  "not-wf/P83/ibm83n01.xml",
  267,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!--* Wrong keyword in PublicID *-->\r\n<!NOTATION JPGformat public \"-//JPG//DTD//JPGFormat\">\r\n<!ENTITY aImage SYSTEM \"image.jpg\" NDATA JPGformat>\r\n]>\r\n<root/>\r\n\r\n",
  0,
  NULL,
  "Tests PublicID with wrong key word. The string \"public\" is used as the key word in the PublicID in the NotationDecl in the DTD."
},
{
  "not-wf/P83/ibm83n02.xml",
  275,
  "r<?xml version=\"1.0\"?>\r\n<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!--* Wrong keyword : Public in PublicID *-->\r\n<!NOTATION JPGformat Public \"-//JPG//DTD//JPGFormat\">\r\n<!ENTITY aImage SYSTEM \"image.jpg\" NDATA JPGformat>\r\n]>\r\n<root/>\r\n",
  0,
  NULL,
  "Tests PublicID with wrong key word. The string \"Public\" is used as the key word in the PublicID in the NotationDecl in the DTD."
},
{
  "not-wf/P83/ibm83n03.xml",
  260,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!--* Missing keyword in PublicID *-->\r\n<!NOTATION JPGformat \"-//JPG//DTD//JPGFormat\">\r\n<!ENTITY aImage SYSTEM \"image.jpg\" NDATA JPGformat>\r\n]>\r\n<root/>\r\n",
  0,
  NULL,
  "Tests PublicID with a required field missing. The key word \"PUBLIC\" is missing in the PublicID in the NotationDecl in the DTD."
},
{
  "not-wf/P83/ibm83n04.xml",
  270,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!--* Missing White space in PublicID *-->\r\n<!NOTATION JPGformat PUBLIC\"-//JPG//DTD//JPGFormat\">\r\n<!ENTITY aImage SYSTEM \"image.jpg\" NDATA JPGformat>\r\n]>\r\n<root/>\r\n",
  0,
  NULL,
  "Tests PublicID with a required field missing. The white space between the \"PUBLIC\" and the PubidLiteral is missing in the PublicID in the NotationDecl in the DTD."
},
{
  "not-wf/P83/ibm83n05.xml",
  248,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!--* Missing PubidLiteral in PublicID *-->\r\n<!NOTATION JPGformat PUBLIC >\r\n<!ENTITY aImage SYSTEM \"image.jpg\" NDATA JPGformat>\r\n]>\r\n<root/>\r\n",
  0,
  NULL,
  "Tests PublicID with a required field missing. The PubidLiteral is missing in the PublicID in the NotationDecl in the DTD."
},
{
  "not-wf/P83/ibm83n06.xml",
  266,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE root \r\n[\r\n<!ELEMENT root (#PCDATA)>\r\n<!ATTLIST root att CDATA #IMPLIED>\r\n<!--* Wrong ordering in PublicID *-->\r\n<!NOTATION JPGformat \"-//JPG//DTD//JPGFormat\" PUBLIC>\r\n<!ENTITY aImage SYSTEM \"image.jpg\" NDATA JPGformat>\r\n]>\r\n<root/>\r\n",
  0,
  NULL,
  "Tests PublicID with wrong field ordering. The key word \"PUBLIC\" occurs after the PubidLiteral in the PublicID in the NotationDecl."
},
{
  "not-wf/P85/ibm85n01.xml",
  103,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\303\227 an illegal char #x0d7\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\227",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x00D7 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n02.xml",
  103,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\303\267 an illegal char #x0f7\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\267",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x00F7 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n03.xml",
  103,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\304\262 an illegal char #x132\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\262",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x0132 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n04.xml",
  103,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\304\263 an illegal char #x133\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\263",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x0133 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n05.xml",
  103,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\304\277 an illegal char #x13f\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\277",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x013F occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n06.xml",
  103,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\305\200 an illegal char #x140\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\200",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x0140 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n07.xml",
  103,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\305\211 an illegal char #x149\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\211",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x0149 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n08.xml",
  103,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\305\277 an illegal char #x17f\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\277",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x017F occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n09.xml",
  103,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\307\204 an illegal char #x1c4\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\204",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x01c4 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n10.xml",
  103,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\307\214 an illegal char #x1cc\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\214",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x01CC occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n100.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\256\266 an illegal char #x0bb6\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\266",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x0BB6 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n101.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\256\272 an illegal char #x0bba\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\272",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x0BBA occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n102.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\260\215 an illegal char #x0c0d\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\215",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x0C0D occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n103.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\260\221 an illegal char #x0c11\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\221",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x0C11 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n104.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\260\251 an illegal char #x0c29\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\251",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x0C29 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n105.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\260\264 an illegal char #x0c34\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\264",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x0C34 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n106.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\261\237 an illegal char #x0c5f\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\237",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x0C5F occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n107.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\261\242 an illegal char #x0c62\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\242",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x0C62 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n108.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\262\215 an illegal char #x0c8d\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\215",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x0C8D occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n109.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\262\221 an illegal char #x0c91\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\221",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x0C91 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n11.xml",
  103,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\307\261 an illegal char #x1f1\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\261",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x01F1 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n110.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\262\251 an illegal char #x0ca9\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\251",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x0CA9 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n111.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\262\264 an illegal char #x0cb4\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\264",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x0CB4 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n112.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\262\272 an illegal char #x0cba\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\272",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x0CBA occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n113.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\263\237 an illegal char #x0cdf\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\237",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x0CDF occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n114.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\263\242 an illegal char #x0ce2\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\242",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x0CE2 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n115.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\264\215 an illegal char #x0d0d\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\215",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x0D0D occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n116.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\264\221 an illegal char #x0d11\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\221",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x0D11 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n117.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\264\251 an illegal char #x0d29\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\251",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x0D29 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n118.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\264\272 an illegal char #x0d3a\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\272",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x0D3A occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n119.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\265\242 an illegal char #x0d62\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\242",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x0D62 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n12.xml",
  103,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\307\263 an illegal char #x1f3\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\263",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x01F3 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n120.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\270\257 an illegal char #x0e2f\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\257",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x0E2F occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n121.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\270\261 an illegal char #x0e31\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\261",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x0E31 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n122.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\270\264 an illegal char #x0e34\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\264",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x0E34 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n123.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\271\206 an illegal char #x0e46\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\206",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x0E46 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n124.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\272\203 an illegal char #x0e83\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\203",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x0E83 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n125.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\272\205 an illegal char #x0e85\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\205",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x0E85 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n126.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\272\211 an illegal char #x0e89\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\211",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x0E89 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n127.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\272\213 an illegal char #x0e8b\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\213",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x0E8B occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n128.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\272\216 an illegal char #x0e8e\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\216",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x0E8E occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n129.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\272\230 an illegal char #x0e98\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\230",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x0E98 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n13.xml",
  103,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\307\266 an illegal char #x1f6\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\266",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x01F6 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n130.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\272\240 an illegal char #x0ea0\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\240",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x0EA0 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n131.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\272\244 an illegal char #x0ea4\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\244",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x0EA4 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n132.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\272\246 an illegal char #x0ea6\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\246",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x0EA6 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n133.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\272\250 an illegal char #x0ea8\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\250",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x0EA8 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n134.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\272\254 an illegal char #x0eac\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\254",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x0EAC occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n135.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\272\257 an illegal char #x0eaf\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\257",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x0EAF occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n136.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\272\261 an illegal char #x0eb1\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\261",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x0EB1 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n137.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\272\264 an illegal char #x0eb4\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\264",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x0EB4 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n138.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\272\276 an illegal char #x0ebe\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\276",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x0EBE occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n139.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\273\205 an illegal char #x0ec5\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\205",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x0EC5 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n14.xml",
  103,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\307\271 an illegal char #x1f9\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\271",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x01F9 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n140.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\275\210 an illegal char #x0f48\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\210",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x0F48 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n141.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\275\252 an illegal char #x0f6a\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\252",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x0F6A occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n142.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\341\203\206 an illegal char #x10c6\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\206",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x10C6 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n143.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\341\203\267 an illegal char #x10f7\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\267",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x10F7 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n144.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\341\204\201 an illegal char #x1101\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\201",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x1011 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n145.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\341\204\204 an illegal char #x1104\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\204",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x1104 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n146.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\341\204\210 an illegal char #x1108\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\210",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x1108 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n147.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\341\204\212 an illegal char #x110a\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\212",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x110A occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n148.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\341\204\215 an illegal char #x110d\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\215",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x110D occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n149.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\341\204\273 an illegal char #x113b\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\273",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x113B occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n15.xml",
  103,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\307\271 an illegal char #x1f9\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\271",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x01F9 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n150.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\341\204\277 an illegal char #x113f\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\277",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x113F occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n151.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\341\205\201 an illegal char #x1141\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\201",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x1141 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n152.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\341\205\215 an illegal char #x114d\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\215",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x114D occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n153.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\341\205\217 an illegal char #x114f\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\217",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x114f occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n154.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\341\205\221 an illegal char #x1151\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\221",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x1151 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n155.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\341\205\226 an illegal char #x1156\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\226",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x1156 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n156.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\341\205\232 an illegal char #x115a\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\232",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x115A occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n157.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\341\205\242 an illegal char #x1162\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\242",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x1162 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n158.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\341\205\244 an illegal char #x1164\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\244",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x1164 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n159.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\341\205\246 an illegal char #x1166\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\246",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x1166 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n16.xml",
  103,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\310\260 an illegal char #x230\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\260",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x0230 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n160.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\341\205\253 an illegal char #x116b\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\253",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x116B occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n161.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\341\205\257 an illegal char #x116f\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\257",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x116F occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n162.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\341\205\264 an illegal char #x1174\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\264",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x1174 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n163.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\341\206\237 an illegal char #x119f\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\237",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x119F occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n164.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\341\206\254 an illegal char #x11ac\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\254",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x11AC occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n165.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\341\206\266 an illegal char #x11b6\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\266",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x11B6 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n166.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\341\206\271 an illegal char #x11b9\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\271",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x11B9 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n167.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\341\206\273 an illegal char #x11bb\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\273",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x11BB occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n168.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\341\207\203 an illegal char #x11c3\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\203",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x11C3 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n169.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\341\207\261 an illegal char #x11f1\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\261",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x11F1 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n17.xml",
  103,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\312\257 an illegal char #x2af\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\257",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x02AF occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n170.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\341\207\272 an illegal char #x11fa\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\272",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x11FA occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n171.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\341\272\234 an illegal char #x1e9c\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\234",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x1E9C occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n172.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\341\273\272 an illegal char #x1efa\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\272",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x1EFA occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n173.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\341\274\226 an illegal char #x1f16\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\226",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x1F16 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n174.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\341\274\236 an illegal char #x1f1e\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\236",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x1F1E occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n175.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\341\275\206 an illegal char #x1f46\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\206",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x1F46 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n176.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\341\275\217 an illegal char #x1f4f\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\217",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x1F4F occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n177.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\341\275\230 an illegal char #x1f58\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\230",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x1F58 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n178.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\341\275\232 an illegal char #x1f5a\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\232",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x1F5A occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n179.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\341\275\234 an illegal char #x1f5c\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\234",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x1F5C occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n18.xml",
  103,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\313\217 an illegal char #x2cf\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\217",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x02CF occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n180.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\341\275\236 an illegal char #x1f5e\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\236",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x1F5E occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n181.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\341\275\276 an illegal char #x1f7e\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\276",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x1F7E occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n182.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\341\276\265 an illegal char #x1fb5\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\265",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x1FB5 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n183.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\341\276\275 an illegal char #x1fbd\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\275",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x1FBD occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n184.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\341\276\277 an illegal char #x1fbf\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\277",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x1FBF occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n185.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\341\277\205 an illegal char #x1fc5\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\205",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x1FC5 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n186.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\341\277\215 an illegal char #x1fcd\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\215",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x1FCD occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n187.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\341\277\225 an illegal char #x1fd5\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\225",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x1FD5 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n188.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\341\277\234 an illegal char #x1fdc\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\234",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x1FDC occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n189.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\341\277\255 an illegal char #x1fed\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\255",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x1FED occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n19.xml",
  103,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\316\207 an illegal char #x387\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\207",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x0387 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n190.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\341\277\265 an illegal char #x1ff5\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\265",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x1FF5 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n191.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\341\277\275 an illegal char #x1ffd\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\275",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x1FFD occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n192.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\342\204\247 an illegal char #x2127\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\247",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x2127 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n193.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\342\204\257 an illegal char #x212f\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\257",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x212F occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n194.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\342\206\203 an illegal char #x2183\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\203",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x2183 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n195.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\343\202\225 an illegal char #x3095\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\225",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x3095 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n196.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\343\203\273 an illegal char #x30fb\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\273",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x30FB occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n197.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\343\204\255 an illegal char #x312d\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\255",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x312D occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n198.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\355\236\244 an illegal char #xd7a4\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\244",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #xD7A4 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n20.xml",
  103,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\316\213 an illegal char #x38b\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\213",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x038B occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n21.xml",
  103,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\316\242 an illegal char #x3a2\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\242",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x03A2 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n22.xml",
  103,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\317\217 an illegal char #x3cf\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\217",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x03CF occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n23.xml",
  103,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\317\227 an illegal char #x3d7\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\227",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x03D7 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n24.xml",
  103,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\317\235 an illegal char #x3dd\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\235",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x03DD occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n25.xml",
  103,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\317\241 an illegal char #x3e1\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\241",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x03E1 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n26.xml",
  103,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\317\264 an illegal char #x3f4\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\264",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x03F4 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n27.xml",
  103,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\320\215 an illegal char #x40d\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\215",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x040D occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n28.xml",
  103,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\321\220 an illegal char #x450\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\220",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x0450 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n29.xml",
  103,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\321\235 an illegal char #x45d\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\235",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x045D occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n30.xml",
  103,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\322\202 an illegal char #x482\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\202",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x0482 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n31.xml",
  103,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\323\205 an illegal char #x4c5\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\205",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x04C5 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n32.xml",
  103,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\323\206 an illegal char #x4c6\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\206",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x04C6 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n33.xml",
  103,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\323\211 an illegal char #x4c9\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\211",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x04C9 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n34.xml",
  103,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\323\254 an illegal char #x4ec\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\254",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x04EC occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n35.xml",
  103,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\323\255 an illegal char #x4ed\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\255",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x04ED occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n36.xml",
  103,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\323\266 an illegal char #x4f6\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\266",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x04F6 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n37.xml",
  103,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\323\272 an illegal char #x4fa\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\272",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x04FA occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n38.xml",
  103,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\325\227 an illegal char #x557\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\227",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x0557 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n39.xml",
  103,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\325\230 an illegal char #x558\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\230",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x0558 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n40.xml",
  103,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\326\207 an illegal char #x587\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\207",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x0587 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n41.xml",
  103,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\327\253 an illegal char #x5eb\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\253",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x05EB occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n42.xml",
  103,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\327\263 an illegal char #x5f3\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\263",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x05F3 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n43.xml",
  103,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\330\240 an illegal char #x620\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\240",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x0620 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n44.xml",
  103,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\330\273 an illegal char #x63b\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\273",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x063B occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n45.xml",
  103,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\331\213 an illegal char #x64b\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\213",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x064B occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n46.xml",
  103,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\332\270 an illegal char #x6b8\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\270",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x06B8 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n47.xml",
  103,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\332\277 an illegal char #x6bf\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\277",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x06BF occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n48.xml",
  103,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\333\217 an illegal char #x6cf\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\217",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x06CF occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n49.xml",
  103,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\333\224 an illegal char #x6d4\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\224",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x06D4 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n50.xml",
  103,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\333\226 an illegal char #x6d6\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\226",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x06D6 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n51.xml",
  103,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\333\247 an illegal char #x6e7\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\247",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x06E7 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n52.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\244\272 an illegal char #x093a\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\272",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x093A occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n53.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\244\276 an illegal char #x093e\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\276",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x093E occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n54.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\245\242 an illegal char #x0962\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\242",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x0962 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n55.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\246\215 an illegal char #x098d\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\215",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x098D occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n56.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\246\221 an illegal char #x0991\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\221",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x0991 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n57.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\246\222 an illegal char #x0992\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\222",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x0992 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n58.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\246\251 an illegal char #x09a9\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\251",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x09A9 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n59.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\246\261 an illegal char #x09b1\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\261",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x09B1 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n60.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\246\265 an illegal char #x09b5\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\265",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x09B5 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n61.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\246\272 an illegal char #x09ba\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\272",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x09BA occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n62.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\247\236 an illegal char #x09de\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\236",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x09DE occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n63.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\247\242 an illegal char #x09e2\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\242",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x09E2 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n64.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\247\262 an illegal char #x09f2\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\262",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x09F2 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n65.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\250\213 an illegal char #x0a0b\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\213",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x0A0B occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n66.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\250\221 an illegal char #x0a11\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\221",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x0A11 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n67.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\250\251 an illegal char #x0a29\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\251",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x0A29 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n68.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\250\261 an illegal char #x0a31\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\261",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x0A31 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n69.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\250\264 an illegal char #x0a34\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\264",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x0A34 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n70.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\250\267 an illegal char #x0a37\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\267",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x0A37 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n71.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\250\272 an illegal char #x0a3a\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\272",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x0A3A occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n72.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\251\235 an illegal char #x0a5d\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\235",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x0A5D occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n73.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\251\260 an illegal char #x0a70\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\260",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x0A70 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n74.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\251\265 an illegal char #x0a75\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\265",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x0A75 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n75.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\252\204 an illegal char #x0a84\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\204",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #xA84 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n76.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\252\274 an illegal char #x0abc\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\274",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x0ABC occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n77.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\252\222 an illegal char #x0a92\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\222",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x0A92 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n78.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\252\251 an illegal char #x0aa9\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\251",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x0AA9 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n79.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\252\261 an illegal char #x0ab1\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\261",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x0AB1 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n80.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\252\264 an illegal char #x0ab4\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\264",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x0AB4 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n81.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\252\272 an illegal char #x0aba\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\272",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x0ABA occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n82.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\254\204 an illegal char #x0b04\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\204",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x0B04 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n83.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\254\215 an illegal char #x0b0d\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\215",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x0B0D occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n84.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\254\221 an illegal char #x0b11\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\221",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x0B11 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n85.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\254\251 an illegal char #x0b29\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\251",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x0B29 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n86.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\254\261 an illegal char #x0b31\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\261",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x0B31 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n87.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\254\264 an illegal char #x0b34\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\264",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x0B34 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n88.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\254\272 an illegal char #x0b3a\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\272",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x0B3A occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n89.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\254\276 an illegal char #x0b3e\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\276",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x0B3E occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n90.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\255\236 an illegal char #x0b5e\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\236",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x0B5E occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n91.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\255\242 an illegal char #x0b62\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\242",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x0B62 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n92.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\256\213 an illegal char #x0b8b\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\213",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x0B8B occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n93.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\256\221 an illegal char #x0b91\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\221",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x0B91 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n94.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\256\230 an illegal char #x0b98\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\230",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x0B98 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n95.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\256\233 an illegal char #x0b9b\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\233",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x0B9B occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n96.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\256\235 an illegal char #x0b9d\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\235",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x0B9D occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n97.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\256\240 an illegal char #x0ba0\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\240",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x0BA0 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n98.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\256\247 an illegal char #x0ba7\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\247",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x0BA7 occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P85/ibm85n99.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\340\256\253 an illegal char #x0bab\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\253",
  0,
  NULL,
  "Tests BaseChar with an illegal character. The character #x0BAB occurs as the first character of the PITarget in the PI in the DTD."
},
{
  "not-wf/P86/ibm86n01.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\344\263\277 an illegal char #x4cff\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\277",
  0,
  NULL,
  "Tests Ideographic with an illegal character. The character #x4CFF occurs as the first character in the PITarget in the PI in the DTD."
},
{
  "not-wf/P86/ibm86n02.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\351\276\246 an illegal char #x9fa6\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\246",
  0,
  NULL,
  "Tests Ideographic with an illegal character. The character #x9FA6 occurs as the first character in the PITarget in the PI in the DTD."
},
{
  "not-wf/P86/ibm86n03.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\343\200\210 an illegal char #x3008\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\210",
  0,
  NULL,
  "Tests Ideographic with an illegal character. The character #x3008 occurs as the first character in the PITarget in the PI in the DTD."
},
{
  "not-wf/P86/ibm86n04.xml",
  105,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?\343\200\252 an illegal char #x302a\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\252",
  0,
  NULL,
  "Tests Ideographic with an illegal character. The character #x302A occurs as the first character in the PITarget in the PI in the DTD."
},
{
  "not-wf/P87/ibm87n01.xml",
  104,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\313\277 an illegal char #x2ff\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\277",
  0,
  NULL,
  "Tests CombiningChar with an illegal character. The character #x02FF occurs as the second character in the PITarget in the PI in the DTD."
},
{
  "not-wf/P87/ibm87n02.xml",
  104,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\315\206 an illegal char #x346\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\206",
  0,
  NULL,
  "Tests CombiningChar with an illegal character. The character #x0346 occurs as the second character in the PITarget in the PI in the DTD."
},
{
  "not-wf/P87/ibm87n03.xml",
  104,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\315\242 an illegal char #x362\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\242",
  0,
  NULL,
  "Tests CombiningChar with an illegal character. The character #x0362 occurs as the second character in the PITarget in the PI in the DTD."
},
{
  "not-wf/P87/ibm87n04.xml",
  104,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\322\207 an illegal char #x487\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\207",
  0,
  NULL,
  "Tests CombiningChar with an illegal character. The character #x0487 occurs as the second character in the PITarget in the PI in the DTD."
},
{
  "not-wf/P87/ibm87n05.xml",
  104,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\326\242 an illegal char #x5a2\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\242",
  0,
  NULL,
  "Tests CombiningChar with an illegal character. The character #x05A2 occurs as the second character in the PITarget in the PI in the DTD."
},
{
  "not-wf/P87/ibm87n06.xml",
  104,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\326\272 an illegal char #x5ba\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\272",
  0,
  NULL,
  "Tests CombiningChar with an illegal character. The character #x05BA occurs as the second character in the PITarget in the PI in the DTD."
},
{
  "not-wf/P87/ibm87n07.xml",
  104,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\326\276 an illegal char #x5be\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\276",
  0,
  NULL,
  "Tests CombiningChar with an illegal character. The character #x05BE occurs as the second character in the PITarget in the PI in the DTD."
},
{
  "not-wf/P87/ibm87n08.xml",
  104,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\327\200 an illegal char #x5c0\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\200",
  0,
  NULL,
  "Tests CombiningChar with an illegal character. The character #x05C0 occurs as the second character in the PITarget in the PI in the DTD."
},
{
  "not-wf/P87/ibm87n09.xml",
  104,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\327\203 an illegal char #x5c3\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\203",
  0,
  NULL,
  "Tests CombiningChar with an illegal character. The character #x05C3 occurs as the second character in the PITarget in the PI in the DTD."
},
{
  "not-wf/P87/ibm87n10.xml",
  104,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\331\223 an illegal char #x653\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\223",
  0,
  NULL,
  "Tests CombiningChar with an illegal character. The character #x0653 occurs as the second character in the PITarget in the PI in the DTD."
},
{
  "not-wf/P87/ibm87n11.xml",
  104,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\332\270 an illegal char #x6b8\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\270",
  0,
  NULL,
  "Tests CombiningChar with an illegal character. The character #x06B8 occurs as the second character in the PITarget in the PI in the DTD."
},
{
  "not-wf/P87/ibm87n12.xml",
  104,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\332\271 an illegal char #x6b9\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\271",
  0,
  NULL,
  "Tests CombiningChar with an illegal character. The character #x06B9 occurs as the second character in the PITarget in the PI in the DTD."
},
{
  "not-wf/P87/ibm87n13.xml",
  104,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\333\251 an illegal char #x6e9\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\251",
  0,
  NULL,
  "Tests CombiningChar with an illegal character. The character #x06E9 occurs as the second character in the PITarget in the PI in the DTD."
},
{
  "not-wf/P87/ibm87n14.xml",
  104,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\333\256 an illegal char #x6ee\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\256",
  0,
  NULL,
  "Tests CombiningChar with an illegal character. The character #x06EE occurs as the second character in the PITarget in the PI in the DTD."
},
{
  "not-wf/P87/ibm87n15.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\244\204 an illegal char #x0904\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\204",
  0,
  NULL,
  "Tests CombiningChar with an illegal character. The character #x0904 occurs as the second character in the PITarget in the PI in the DTD."
},
{
  "not-wf/P87/ibm87n16.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\244\273 an illegal char #x093b\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\273",
  0,
  NULL,
  "Tests CombiningChar with an illegal character. The character #x093B occurs as the second character in the PITarget in the PI in the DTD."
},
{
  "not-wf/P87/ibm87n17.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\245\216 an illegal char #x094e\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\216",
  0,
  NULL,
  "Tests CombiningChar with an illegal character. The character #x094E occurs as the second character in the PITarget in the PI in the DTD."
},
{
  "not-wf/P87/ibm87n18.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\245\225 an illegal char #x0955\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\225",
  0,
  NULL,
  "Tests CombiningChar with an illegal character. The character #x0955 occurs as the second character in the PITarget in the PI in the DTD."
},
{
  "not-wf/P87/ibm87n19.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\245\244 an illegal char #x0964\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\244",
  0,
  NULL,
  "Tests CombiningChar with an illegal character. The character #x0964 occurs as the second character in the PITarget in the PI in the DTD."
},
{
  "not-wf/P87/ibm87n20.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\246\204 an illegal char #x0984\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\204",
  0,
  NULL,
  "Tests CombiningChar with an illegal character. The character #x0984 occurs as the second character in the PITarget in the PI in the DTD."
},
{
  "not-wf/P87/ibm87n21.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\247\205 an illegal char #x09c5\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\205",
  0,
  NULL,
  "Tests CombiningChar with an illegal character. The character #x09C5 occurs as the second character in the PITarget in the PI in the DTD."
},
{
  "not-wf/P87/ibm87n22.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\247\211 an illegal char #x09c9\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\211",
  0,
  NULL,
  "Tests CombiningChar with an illegal character. The character #x09C9 occurs as the second character in the PITarget in the PI in the DTD."
},
{
  "not-wf/P87/ibm87n23.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\247\216 an illegal char #x09ce\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\216",
  0,
  NULL,
  "Tests CombiningChar with an illegal character. The character #x09CE occurs as the second character in the PITarget in the PI in the DTD."
},
{
  "not-wf/P87/ibm87n24.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\247\230 an illegal char #x09d8\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\230",
  0,
  NULL,
  "Tests CombiningChar with an illegal character. The character #x09D8 occurs as the second character in the PITarget in the PI in the DTD."
},
{
  "not-wf/P87/ibm87n25.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\247\244 an illegal char #x09e4\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\244",
  0,
  NULL,
  "Tests CombiningChar with an illegal character. The character #x09E4 occurs as the second character in the PITarget in the PI in the DTD."
},
{
  "not-wf/P87/ibm87n26.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\250\203 an illegal char #x0a03\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\203",
  0,
  NULL,
  "Tests CombiningChar with an illegal character. The character #x0A03 occurs as the second character in the PITarget in the PI in the DTD."
},
{
  "not-wf/P87/ibm87n27.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\250\275 an illegal char #x0a3d\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\275",
  0,
  NULL,
  "Tests CombiningChar with an illegal character. The character #x0A3D occurs as the second character in the PITarget in the PI in the DTD."
},
{
  "not-wf/P87/ibm87n28.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\251\206 an illegal char #x0a46\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\206",
  0,
  NULL,
  "Tests CombiningChar with an illegal character. The character #x0A46 occurs as the second character in the PITarget in the PI in the DTD."
},
{
  "not-wf/P87/ibm87n29.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\251\211 an illegal char #x0a49\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\211",
  0,
  NULL,
  "Tests CombiningChar with an illegal character. The character #x0A49 occurs as the second character in the PITarget in the PI in the DTD."
},
{
  "not-wf/P87/ibm87n30.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\251\216 an illegal char #x0a4e\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\216",
  0,
  NULL,
  "Tests CombiningChar with an illegal character. The character #x0A4E occurs as the second character in the PITarget in the PI in the DTD."
},
{
  "not-wf/P87/ibm87n31.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\252\200 an illegal char #x0a80\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\200",
  0,
  NULL,
  "Tests CombiningChar with an illegal character. The character #x0A80 occurs as the second character in the PITarget in the PI in the DTD."
},
{
  "not-wf/P87/ibm87n32.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\252\204 an illegal char #x0a84\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\204",
  0,
  NULL,
  "Tests CombiningChar with an illegal character. The character #x0A84 occurs as the second character in the PITarget in the PI in the DTD."
},
{
  "not-wf/P87/ibm87n33.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\252\273 an illegal char #x0abb\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\273",
  0,
  NULL,
  "Tests CombiningChar with an illegal character. The character #x0ABB occurs as the second character in the PITarget in the PI in the DTD."
},
{
  "not-wf/P87/ibm87n34.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\253\206 an illegal char #x0ac6\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\206",
  0,
  NULL,
  "Tests CombiningChar with an illegal character. The character #x0AC6 occurs as the second character in the PITarget in the PI in the DTD."
},
{
  "not-wf/P87/ibm87n35.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\253\212 an illegal char #x0aca\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\212",
  0,
  NULL,
  "Tests CombiningChar with an illegal character. The character #x0ACA occurs as the second character in the PITarget in the PI in the DTD."
},
{
  "not-wf/P87/ibm87n36.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\253\216 an illegal char #x0ace\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\216",
  0,
  NULL,
  "Tests CombiningChar with an illegal character. The character #x0ACE occurs as the second character in the PITarget in the PI in the DTD."
},
{
  "not-wf/P87/ibm87n37.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\254\204 an illegal char #x0b04\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\204",
  0,
  NULL,
  "Tests CombiningChar with an illegal character. The character #x0B04 occurs as the second character in the PITarget in the PI in the DTD."
},
{
  "not-wf/P87/ibm87n38.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\254\273 an illegal char #x0b3b\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\273",
  0,
  NULL,
  "Tests CombiningChar with an illegal character. The character #x0B3B occurs as the second character in the PITarget in the PI in the DTD."
},
{
  "not-wf/P87/ibm87n39.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\255\204 an illegal char #x0b44\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\204",
  0,
  NULL,
  "Tests CombiningChar with an illegal character. The character #x0B44 occurs as the second character in the PITarget in the PI in the DTD."
},
{
  "not-wf/P87/ibm87n40.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\255\212 an illegal char #x0b4a\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\212",
  0,
  NULL,
  "Tests CombiningChar with an illegal character. The character #x0B4A occurs as the second character in the PITarget in the PI in the DTD."
},
{
  "not-wf/P87/ibm87n41.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\255\216 an illegal char #x0b4e\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\216",
  0,
  NULL,
  "Tests CombiningChar with an illegal character. The character #x0B4E occurs as the second character in the PITarget in the PI in the DTD."
},
{
  "not-wf/P87/ibm87n42.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\255\230 an illegal char #x0b58\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\230",
  0,
  NULL,
  "Tests CombiningChar with an illegal character. The character #x0B58 occurs as the second character in the PITarget in the PI in the DTD."
},
{
  "not-wf/P87/ibm87n43.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\256\204 an illegal char #x0b84\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\204",
  0,
  NULL,
  "Tests CombiningChar with an illegal character. The character #x0B84 occurs as the second character in the PITarget in the PI in the DTD."
},
{
  "not-wf/P87/ibm87n44.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\257\203 an illegal char #x0bc3\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\203",
  0,
  NULL,
  "Tests CombiningChar with an illegal character. The character #x0BC3 occurs as the second character in the PITarget in the PI in the DTD."
},
{
  "not-wf/P87/ibm87n45.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\257\211 an illegal char #x0bc9\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\211",
  0,
  NULL,
  "Tests CombiningChar with an illegal character. The character #x0BC9 occurs as the second character in the PITarget in the PI in the DTD."
},
{
  "not-wf/P87/ibm87n46.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\257\226 an illegal char #x0bd6\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\226",
  0,
  NULL,
  "Tests CombiningChar with an illegal character. The character #x0BD6 occurs as the second character in the PITarget in the PI in the DTD."
},
{
  "not-wf/P87/ibm87n47.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\260\215 an illegal char #x0c0d\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\215",
  0,
  NULL,
  "Tests CombiningChar with an illegal character. The character #x0C0D occurs as the second character in the PITarget in the PI in the DTD."
},
{
  "not-wf/P87/ibm87n48.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\261\205 an illegal char #x0c45\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\205",
  0,
  NULL,
  "Tests CombiningChar with an illegal character. The character #x0C45 occurs as the second character in the PITarget in the PI in the DTD."
},
{
  "not-wf/P87/ibm87n49.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\261\211 an illegal char #x0c49\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\211",
  0,
  NULL,
  "Tests CombiningChar with an illegal character. The character #x0C49 occurs as the second character in the PITarget in the PI in the DTD."
},
{
  "not-wf/P87/ibm87n50.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\261\224 an illegal char #x0c54\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\224",
  0,
  NULL,
  "Tests CombiningChar with an illegal character. The character #x0C54 occurs as the second character in the PITarget in the PI in the DTD."
},
{
  "not-wf/P87/ibm87n51.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\262\201 an illegal char #x0c81\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\201",
  0,
  NULL,
  "Tests CombiningChar with an illegal character. The character #x0C81 occurs as the second character in the PITarget in the PI in the DTD."
},
{
  "not-wf/P87/ibm87n52.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\262\204 an illegal char #x0c84\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\204",
  0,
  NULL,
  "Tests CombiningChar with an illegal character. The character #x0C84 occurs as the second character in the PITarget in the PI in the DTD."
},
{
  "not-wf/P87/ibm87n53.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\263\205 an illegal char #x0cc5\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\205",
  0,
  NULL,
  "Tests CombiningChar with an illegal character. The character #x0CC5 occurs as the second character in the PITarget in the PI in the DTD."
},
{
  "not-wf/P87/ibm87n54.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\263\211 an illegal char #x0cc9\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\211",
  0,
  NULL,
  "Tests CombiningChar with an illegal character. The character #x0CC9 occurs as the second character in the PITarget in the PI in the DTD."
},
{
  "not-wf/P87/ibm87n55.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\263\224 an illegal char #x0cd4\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\224",
  0,
  NULL,
  "Tests CombiningChar with an illegal character. The character #x0CD4 occurs as the second character in the PITarget in the PI in the DTD."
},
{
  "not-wf/P87/ibm87n56.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\263\227 an illegal char #x0cd7\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\227",
  0,
  NULL,
  "Tests CombiningChar with an illegal character. The character #x0CD7 occurs as the second character in the PITarget in the PI in the DTD."
},
{
  "not-wf/P87/ibm87n57.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\264\204 an illegal char #x0d04\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\204",
  0,
  NULL,
  "Tests CombiningChar with an illegal character. The character #x0D04 occurs as the second character in the PITarget in the PI in the DTD."
},
{
  "not-wf/P87/ibm87n58.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\265\205 an illegal char #x0d45\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\205",
  0,
  NULL,
  "Tests CombiningChar with an illegal character. The character #x0D45 occurs as the second character in the PITarget in the PI in the DTD."
},
{
  "not-wf/P87/ibm87n59.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\265\211 an illegal char #x0d49\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\211",
  0,
  NULL,
  "Tests CombiningChar with an illegal character. The character #x0D49 occurs as the second character in the PITarget in the PI in the DTD."
},
{
  "not-wf/P87/ibm87n60.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\265\216 an illegal char #x0d4e\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\216",
  0,
  NULL,
  "Tests CombiningChar with an illegal character. The character #x0D4E occurs as the second character in the PITarget in the PI in the DTD."
},
{
  "not-wf/P87/ibm87n61.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\265\230 an illegal char #x0d58\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\230",
  0,
  NULL,
  "Tests CombiningChar with an illegal character. The character #x0D58 occurs as the second character in the PITarget in the PI in the DTD."
},
{
  "not-wf/P87/ibm87n62.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\270\277 an illegal char #x0e3f\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\277",
  0,
  NULL,
  "Tests CombiningChar with an illegal character. The character #x0E3F occurs as the second character in the PITarget in the PI in the DTD."
},
{
  "not-wf/P87/ibm87n63.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\270\273 an illegal char #x0e3b\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\273",
  0,
  NULL,
  "Tests CombiningChar with an illegal character. The character #x0E3B occurs as the second character in the PITarget in the PI in the DTD."
},
{
  "not-wf/P87/ibm87n64.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\271\217 an illegal char #x0e4f\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\217",
  0,
  NULL,
  "Tests CombiningChar with an illegal character. The character #x0E4F occurs as the second character in the PITarget in the PI in the DTD."
},
{
  "not-wf/P87/ibm87n66.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\272\272 an illegal char #x0eba\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\272",
  0,
  NULL,
  "Tests CombiningChar with an illegal character. The character #x0EBA occurs as the second character in the PITarget in the PI in the DTD."
},
{
  "not-wf/P87/ibm87n67.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\272\276 an illegal char #x0ebe\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\276",
  0,
  NULL,
  "Tests CombiningChar with an illegal character. The character #x0EBE occurs as the second character in the PITarget in the PI in the DTD."
},
{
  "not-wf/P87/ibm87n68.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\273\216 an illegal char #x0ece\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\216",
  0,
  NULL,
  "Tests CombiningChar with an illegal character. The character #x0ECE occurs as the second character in the PITarget in the PI in the DTD."
},
{
  "not-wf/P87/ibm87n69.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\274\232 an illegal char #x0f1a\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\232",
  0,
  NULL,
  "Tests CombiningChar with an illegal character. The character #x0F1A occurs as the second character in the PITarget in the PI in the DTD."
},
{
  "not-wf/P87/ibm87n70.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\274\266 an illegal char #x0f36\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\266",
  0,
  NULL,
  "Tests CombiningChar with an illegal character. The character #x0F36 occurs as the second character in the PITarget in the PI in the DTD."
},
{
  "not-wf/P87/ibm87n71.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\274\270 an illegal char #x0f38\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\270",
  0,
  NULL,
  "Tests CombiningChar with an illegal character. The character #x0F38 occurs as the second character in the PITarget in the PI in the DTD."
},
{
  "not-wf/P87/ibm87n72.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\274\273 an illegal char #x0f3b\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\273",
  0,
  NULL,
  "Tests CombiningChar with an illegal character. The character #x0F3B occurs as the second character in the PITarget in the PI in the DTD."
},
{
  "not-wf/P87/ibm87n73.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\274\272 an illegal char #x0f3a\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\272",
  0,
  NULL,
  "Tests CombiningChar with an illegal character. The character #x0F3A occurs as the second character in the PITarget in the PI in the DTD."
},
{
  "not-wf/P87/ibm87n74.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\275\260 an illegal char #x0f70\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\260",
  0,
  NULL,
  "Tests CombiningChar with an illegal character. The character #x0F70 occurs as the second character in the PITarget in the PI in the DTD."
},
{
  "not-wf/P87/ibm87n75.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\276\205 an illegal char #x0f85\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\205",
  0,
  NULL,
  "Tests CombiningChar with an illegal character. The character #x0F85 occurs as the second character in the PITarget in the PI in the DTD."
},
{
  "not-wf/P87/ibm87n76.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\276\214 an illegal char #x0f8c\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\214",
  0,
  NULL,
  "Tests CombiningChar with an illegal character. The character #x0F8C occurs as the second character in the PITarget in the PI in the DTD."
},
{
  "not-wf/P87/ibm87n77.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\276\226 an illegal char #x0f96\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\226",
  0,
  NULL,
  "Tests CombiningChar with an illegal character. The character #x0F96 occurs as the second character in the PITarget in the PI in the DTD."
},
{
  "not-wf/P87/ibm87n78.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\276\230 an illegal char #x0f98\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\230",
  0,
  NULL,
  "Tests CombiningChar with an illegal character. The character #x0F98 occurs as the second character in the PITarget in the PI in the DTD."
},
{
  "not-wf/P87/ibm87n79.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\276\260 an illegal char #x0fb0\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\260",
  0,
  NULL,
  "Tests CombiningChar with an illegal character. The character #x0FB0 occurs as the second character in the PITarget in the PI in the DTD."
},
{
  "not-wf/P87/ibm87n80.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\276\270 an illegal char #x0fb8\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\270",
  0,
  NULL,
  "Tests CombiningChar with an illegal character. The character #x0FB8 occurs as the second character in the PITarget in the PI in the DTD."
},
{
  "not-wf/P87/ibm87n81.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\276\272 an illegal char #x0fba\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\272",
  0,
  NULL,
  "Tests CombiningChar with an illegal character. The character #x0FBA occurs as the second character in the PITarget in the PI in the DTD."
},
{
  "not-wf/P87/ibm87n82.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\342\203\235 an illegal char #x20dd\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\235",
  0,
  NULL,
  "Tests CombiningChar with an illegal character. The character #x20DD occurs as the second character in the PITarget in the PI in the DTD."
},
{
  "not-wf/P87/ibm87n83.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\342\203\242 an illegal char #x20e2\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\242",
  0,
  NULL,
  "Tests CombiningChar with an illegal character. The character #x20E2 occurs as the second character in the PITarget in the PI in the DTD."
},
{
  "not-wf/P87/ibm87n84.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\343\200\260 an illegal char #x3030\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\260",
  0,
  NULL,
  "Tests CombiningChar with an illegal character. The character #x3030 occurs as the second character in the PITarget in the PI in the DTD."
},
{
  "not-wf/P87/ibm87n85.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\343\202\233 an illegal char #x309b\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\233",
  0,
  NULL,
  "Tests CombiningChar with an illegal character. The character #x309B occurs as the second character in the PITarget in the PI in the DTD."
},
{
  "not-wf/P88/ibm88n01.xml",
  100,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_) an illegal char #x29 in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0,
  NULL,
  "Tests Digit with an illegal character. The character #x0029 occurs as the second character in the PITarget in the PI in the DTD."
},
{
  "not-wf/P88/ibm88n02.xml",
  100,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_; an illegal char #x3b in PITarget ?>\r\n]>\r\n<animal/>\r\n",
  0,
  NULL,
  "Tests Digit with an illegal character. The character #x003B occurs as the second character in the PITarget in the PI in the DTD."
},
{
  "not-wf/P88/ibm88n03.xml",
  104,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\331\252 an illegal char #x66a\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\252",
  0,
  NULL,
  "Tests Digit with an illegal character. The character #x066A occurs as the second character in the PITarget in the PI in the DTD."
},
{
  "not-wf/P88/ibm88n04.xml",
  104,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\333\272 an illegal char #x6fa\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\272",
  0,
  NULL,
  "Tests Digit with an illegal character. The character #x06FA occurs as the second character in the PITarget in the PI in the DTD."
},
{
  "not-wf/P88/ibm88n05.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\245\260 an illegal char #x0970\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\260",
  0,
  NULL,
  "Tests Digit with an illegal character. The character #x0970 occurs as the second character in the PITarget in the PI in the DTD."
},
{
  "not-wf/P88/ibm88n06.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\247\262 an illegal char #x09f2\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\262",
  0,
  NULL,
  "Tests Digit with an illegal character. The character #x09F2 occurs as the second character in the PITarget in the PI in the DTD."
},
{
  "not-wf/P88/ibm88n08.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\253\260 an illegal char #x0af0\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\260",
  0,
  NULL,
  "Tests Digit with an illegal character. The character #x0AF0 occurs as the second character in the PITarget in the PI in the DTD."
},
{
  "not-wf/P88/ibm88n09.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\255\260 an illegal char #x0b70\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\260",
  0,
  NULL,
  "Tests Digit with an illegal character. The character #x0B70 occurs as the second character in the PITarget in the PI in the DTD."
},
{
  "not-wf/P88/ibm88n10.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\261\245 an illegal char #x0c65\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\245",
  0,
  NULL,
  "Tests Digit with an illegal character. The character #x0C65 occurs as the second character in the PITarget in the PI in the DTD."
},
{
  "not-wf/P88/ibm88n11.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\263\245 an illegal char #x0ce5\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\245",
  0,
  NULL,
  "Tests Digit with an illegal character. The character #x0CE5 occurs as the second character in the PITarget in the PI in the DTD."
},
{
  "not-wf/P88/ibm88n12.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\263\260 an illegal char #x0cf0\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\260",
  0,
  NULL,
  "Tests Digit with an illegal character. The character #x0CF0 occurs as the second character in the PITarget in the PI in the DTD."
},
{
  "not-wf/P88/ibm88n13.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\265\260 an illegal char #x0d70\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\260",
  0,
  NULL,
  "Tests Digit with an illegal character. The character #x0D70 occurs as the second character in the PITarget in the PI in the DTD."
},
{
  "not-wf/P88/ibm88n14.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\271\232 an illegal char #x0e5a\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\232",
  0,
  NULL,
  "Tests Digit with an illegal character. The character #x0E5A occurs as the second character in the PITarget in the PI in the DTD."
},
{
  "not-wf/P88/ibm88n15.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\273\232 an illegal char #x0eda\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\232",
  0,
  NULL,
  "Tests Digit with an illegal character. The character #x0EDA occurs as the second character in the PITarget in the PI in the DTD."
},
{
  "not-wf/P88/ibm88n16.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\274\252 an illegal char #x0f2a\r\n in PITarget ?>\r\n]>\r\n<animal/>\r\n\252",
  0,
  NULL,
  "Tests Digit with an illegal character. The character #x0F2A occurs as the second character in the PITarget in the PI in the DTD."
},
{
  "not-wf/P89/ibm89n01.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\302\266 an illegal extender #x0b6 in PITarget ?>\r\n]>\r\n<animal/>\r\n\266",
  0,
  NULL,
  "Tests Extender with an illegal character. The character #x00B6 occurs as the second character in the PITarget in the PI in the DTD."
},
{
  "not-wf/P89/ibm89n02.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\302\270 an illegal extender #x0b8 in PITarget ?>\r\n]>\r\n<animal/>\r\n\270",
  0,
  NULL,
  "Tests Extender with an illegal character. The character #x00B8 occurs as the second character in the PITarget in the PI in the DTD."
},
{
  "not-wf/P89/ibm89n03.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\313\222 an illegal extender #x2d2 in PITarget ?>\r\n]>\r\n<animal/>\r\n\222",
  0,
  NULL,
  "Tests Extender with an illegal character. The character #x02D2 occurs as the second character in the PITarget in the PI in the DTD."
},
{
  "not-wf/P89/ibm89n04.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\317\276 an illegal extender #x3fe in PITarget ?>\r\n]>\r\n<animal/>\r\n\276",
  0,
  NULL,
  "Tests Extender with an illegal character. The character #x03FE occurs as the second character in the PITarget in the PI in the DTD."
},
{
  "not-wf/P89/ibm89n05.xml",
  106,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\331\237 an illegal extender #x65f in PITarget ?>\r\n]>\r\n<animal/>\r\n\237",
  0,
  NULL,
  "Tests Extender with an illegal character. The character #x065F occurs as the second character in the PITarget in the PI in the DTD."
},
{
  "not-wf/P89/ibm89n06.xml",
  131,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\340\273\207 an illegal extender #x0ec7 in PITarget ?>\r\n]>\r\n<animal/>\r\n<IllegalExtender\340\273\207/>\r\n\207",
  0,
  NULL,
  "Tests Extender with an illegal character. The character #x0EC7 occurs as the second character in the PITarget in the PI in the DTD. [Also contains two top-level elements -- one should be removed]"
},
{
  "not-wf/P89/ibm89n07.xml",
  131,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\343\200\206 an illegal extender #x3006 in PITarget ?>\r\n]>\r\n<animal/>\r\n<IllegalExtender\343\200\206/>\r\n\206",
  0,
  NULL,
  "Tests Extender with an illegal character. The character #x3006 occurs as the second character in the PITarget in the PI in the DTD. [Also contains two top-level elements -- one should be removed]"
},
{
  "not-wf/P89/ibm89n08.xml",
  131,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\343\200\260 an illegal extender #x3030 in PITarget ?>\r\n]>\r\n<animal/>\r\n<IllegalExtender\343\200\260/>\r\n\260",
  0,
  NULL,
  "Tests Extender with an illegal character. The character #x3030 occurs as the second character in the PITarget in the PI in the DTD. [Also contains two top-level elements -- one should be removed]"
},
{
  "not-wf/P89/ibm89n09.xml",
  131,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\343\200\266 an illegal extender #x3036 in PITarget ?>\r\n]>\r\n<animal/>\r\n<IllegalExtender\343\200\266/>\r\n\266",
  0,
  NULL,
  "Tests Extender with an illegal character. The character #x3036 occurs as the second character in the PITarget in the PI in the DTD. [Also contains two top-level elements -- one should be removed]"
},
{
  "not-wf/P89/ibm89n10.xml",
  131,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\343\202\234 an illegal extender #x309c in PITarget ?>\r\n]>\r\n<animal/>\r\n<IllegalExtender\343\202\234/>\r\n\234",
  0,
  NULL,
  "Tests Extender with an illegal character. The character #x309C occurs as the second character in the PITarget in the PI in the DTD. [Also contains two top-level elements -- one should be removed]"
},
{
  "not-wf/P89/ibm89n11.xml",
  131,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\343\202\237 an illegal extender #x309f in PITarget ?>\r\n]>\r\n<animal/>\r\n<IllegalExtender\343\202\237/>\r\n\237",
  0,
  NULL,
  "Tests Extender with an illegal character. The character #x309F occurs as the second character in the PITarget in the PI in the DTD. [Also contains two top-level elements -- one should be removed]"
},
{
  "not-wf/P89/ibm89n12.xml",
  131,
  "<!DOCTYPE animal [\r\n<!ELEMENT animal ANY>\r\n<?_\343\203\277 an illegal extender #x30ff in PITarget ?>\r\n]>\r\n<animal/>\r\n<IllegalExtender\343\203\277/>\r\n\277",
  0,
  NULL,
  "Tests Extender with an illegal character. The character #x30FF occurs as the second character in the PITarget in the PI in the DTD. [Also contains two top-level elements -- one should be removed]"
},
/* ==== from eduni/namespaces/1.0 rmt-ns10.xml ==== */
{
  "001.xml",
  196,
  "<?xml version=\"1.0\"?>\n<!-- Namespace name test: a perfectly good http URI -->\n<!DOCTYPE foo [\n<!ELEMENT foo ANY>\n<!ATTLIST foo xmlns CDATA #IMPLIED>\n]>\n<foo xmlns=\"http://example.org/namespace\"/>\n",
  0,
  NULL,
  "Namespace name test: a perfectly good http URI"
},
{
  "002.xml",
  234,
  "<?xml version=\"1.0\"?>\n<!-- Namespace name test: a syntactically plausible URI with a \n     fictitious scheme -->\n<!DOCTYPE foo [\n<!ELEMENT foo ANY>\n<!ATTLIST foo xmlns CDATA #IMPLIED>\n]>\n<foo xmlns=\"zarquon://example.org/namespace\"/>\n",
  0,
  NULL,
  "Namespace name test: a syntactically plausible URI with a fictitious scheme"
},
{
  "003.xml",
  219,
  "<?xml version=\"1.0\"?>\n<!-- Namespace name test: a perfectly good http URI with a fragment -->\n<!DOCTYPE foo [\n<!ELEMENT foo ANY>\n<!ATTLIST foo xmlns CDATA #IMPLIED>\n]>\n<foo xmlns=\"http://example.org/namespace#apples\"/>\n",
  0,
  NULL,
  "Namespace name test: a perfectly good http URI with a fragment"
},
{
  "004.xml",
  187,
  "<?xml version=\"1.0\"?>\n<!-- Namespace name test: a relative URI (deprecated) -->\n<!DOCTYPE foo [\n<!ELEMENT foo ANY>\n<!ATTLIST foo xmlns CDATA #IMPLIED>]\n>\n<foo xmlns=\"namespaces/zaphod\"/>\n",
  0,
  NULL,
  "Namespace name test: a relative URI (deprecated)"
},
{
  "005.xml",
  195,
  "<?xml version=\"1.0\"?>\n<!-- Namespace name test: a same-document relative URI (deprecated) -->\n<!DOCTYPE foo [\n<!ELEMENT foo ANY>\n<!ATTLIST foo xmlns CDATA #IMPLIED>\n]>\n<foo xmlns=\"#beeblebrox\"/>\n",
  0,
  NULL,
  "Namespace name test: a same-document relative URI (deprecated)"
},
{
  "006.xml",
  217,
  "<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>\n<!-- Namespace name test: an http IRI that is not a URI -->\n<!DOCTYPE foo [\n<!ELEMENT foo ANY>\n<!ATTLIST foo xmlns CDATA #IMPLIED>\n]>\n<foo xmlns=\"http://example.org/ros\351\"/>\n\351",
  0,
  NULL,
  "Namespace name test: an http IRI that is not a URI"
},
{
  "007.xml",
  529,
  "<?xml version=\"1.0\"?>\n<!-- Namespace inequality test: different capitalization -->\n<!DOCTYPE foo [\n<!ELEMENT foo ANY>\n<!ATTLIST foo xmlns:a CDATA #IMPLIED\n              xmlns:b CDATA #IMPLIED\n              xmlns:c CDATA #IMPLIED>\n<!ELEMENT bar ANY>\n<!ATTLIST bar a:attr CDATA #IMPLIED\n              b:attr CDATA #IMPLIED\n              c:attr CDATA #IMPLIED>\n]>\n<foo xmlns:a=\"http://example.org/wine\"\n     xmlns:b=\"http://Example.org/wine\"\n     xmlns:c=\"http://example.org/Wine\">\n\n<bar a:attr=\"1\" b:attr=\"2\" c:attr=\"3\"/>\n\n</foo>\n\n",
  0,
  NULL,
  "Namespace inequality test: different capitalization"
},
{
  "008.xml",
  536,
  "<?xml version=\"1.0\"?>\n<!-- Namespace inequality test: different escaping -->\n<!DOCTYPE foo [\n<!ELEMENT foo ANY>\n<!ATTLIST foo xmlns:a CDATA #IMPLIED\n              xmlns:b CDATA #IMPLIED\n              xmlns:c CDATA #IMPLIED>\n<!ELEMENT bar ANY>\n<!ATTLIST bar a:attr CDATA #IMPLIED\n              b:attr CDATA #IMPLIED\n              c:attr CDATA #IMPLIED>\n]>\n<foo xmlns:a=\"http://example.org/~wilbur\"\n     xmlns:b=\"http://example.org/%7ewilbur\"\n     xmlns:c=\"http://example.org/%7Ewilbur\">\n\n<bar a:attr=\"1\" b:attr=\"2\" c:attr=\"3\"/>\n\n</foo>\n\n",
  0,
  NULL,
  "Namespace inequality test: different escaping"
},
{
  "009.xml",
  475,
  "<?xml version=\"1.0\"?>\n<!-- Namespace equality test: plain repetition -->\n<!DOCTYPE foo [\n<!ELEMENT foo ANY>\n<!ATTLIST foo xmlns:a CDATA #IMPLIED\n              xmlns:b CDATA #IMPLIED\n              xmlns:c CDATA #IMPLIED>\n<!ELEMENT bar ANY>\n<!ATTLIST bar a:attr CDATA #IMPLIED\n              b:attr CDATA #IMPLIED\n              c:attr CDATA #IMPLIED>\n]>\n<foo xmlns:a=\"http://example.org/~wilbur\"\n     xmlns:b=\"http://example.org/~wilbur\">\n\n<bar a:attr=\"1\" b:attr=\"2\"/>\n\n</foo>\n\n",
  0,
  NULL,
  "Namespace equality test: plain repetition"
},
{
  "010.xml",
  490,
  "<?xml version=\"1.0\"?>\n<!-- Namespace equality test: use of character reference -->\n<!DOCTYPE foo [\n<!ELEMENT foo ANY>\n<!ATTLIST foo xmlns:a CDATA #IMPLIED\n              xmlns:b CDATA #IMPLIED\n              xmlns:c CDATA #IMPLIED>\n<!ELEMENT bar ANY>\n<!ATTLIST bar a:attr CDATA #IMPLIED\n              b:attr CDATA #IMPLIED\n              c:attr CDATA #IMPLIED>\n]>\n<foo xmlns:a=\"http://example.org/~wilbur\"\n     xmlns:b=\"http://example.org/&#x7E;wilbur\">\n\n<bar a:attr=\"1\" b:attr=\"2\"/>\n\n</foo>\n\n",
  0,
  NULL,
  "Namespace equality test: use of character reference"
},
{
  "011.xml",
  508,
  "<?xml version=\"1.0\"?>\n<!-- Namespace equality test: use of entity reference -->\n<!DOCTYPE foo [\n<!ELEMENT foo ANY>\n<!ATTLIST foo xmlns:a CDATA #IMPLIED\n              xmlns:b CDATA #IMPLIED\n              xmlns:c CDATA #IMPLIED>\n<!ELEMENT bar ANY>\n<!ATTLIST bar a:attr CDATA #IMPLIED\n              b:attr CDATA #IMPLIED\n              c:attr CDATA #IMPLIED>\n<!ENTITY tilde \"~\">\n]>\n<foo xmlns:a=\"http://example.org/~wilbur\"\n     xmlns:b=\"http://example.org/&tilde;wilbur\">\n\n<bar a:attr=\"1\" b:attr=\"2\"/>\n\n</foo>\n\n",
  0,
  NULL,
  "Namespace equality test: use of entity reference"
},
{
  "012.xml",
  472,
  "<?xml version=\"1.0\"?>\n<!-- Namespace inequality test: equal after attribute value normalization -->\n<!DOCTYPE foo [\n<!ELEMENT foo ANY>\n<!ATTLIST foo xmlns:a CDATA #IMPLIED\n              xmlns:b NMTOKEN #IMPLIED\n              xmlns:c CDATA #IMPLIED>\n<!ELEMENT bar ANY>\n<!ATTLIST bar a:attr CDATA #IMPLIED\n              b:attr CDATA #IMPLIED\n              c:attr CDATA #IMPLIED>\n]>\n<foo xmlns:a=\"urn:xyzzy\"\n     xmlns:b=\" urn:xyzzy \">\n\n<bar a:attr=\"1\" b:attr=\"2\"/>\n\n</foo>\n\n",
  0,
  NULL,
  "Namespace inequality test: equal after attribute value normalization"
},
{
  "013.xml",
  98,
  "<?xml version=\"1.0\"?>\n<!-- Bad QName syntax: multiple colons -->\n<foo>\n<bar a:b:attr=\"1\"/>\n</foo>\n",
  0,
  NULL,
  "Bad QName syntax: multiple colons"
},
{
  "014.xml",
  71,
  "<?xml version=\"1.0\"?>\n<!-- Bad QName syntax: colon at end -->\n<foo: />\n",
  0,
  NULL,
  "Bad QName syntax: colon at end"
},
{
  "015.xml",
  73,
  "<?xml version=\"1.0\"?>\n<!-- Bad QName syntax: colon at start -->\n<:foo />\n",
  0,
  NULL,
  "Bad QName syntax: colon at start"
},
{
  "016.xml",
  102,
  "<?xml version=\"1.0\"?>\n<!-- Bad QName syntax: xmlns: -->\n<foo xmlns:=\"http://example.org/namespace\" />\n",
  0,
  NULL,
  "Bad QName syntax: xmlns:"
},
{
  "017.xml",
  71,
  "<?xml version=\"1.0\"?>\n<!-- Simple legal case: no namespaces -->\n<foo/>\n",
  0,
  NULL,
  "Simple legal case: no namespaces"
},
{
  "018.xml",
  112,
  "<?xml version=\"1.0\"?>\n<!-- Simple legal case: default namespace -->\n<foo xmlns=\"http://example.org/namespace\"/>\n",
  0,
  NULL,
  "Simple legal case: default namespace"
},
{
  "019.xml",
  115,
  "<?xml version=\"1.0\"?>\n<!-- Simple legal case: prefixed element -->\n<a:foo xmlns:a=\"http://example.org/namespace\"/>\n",
  0,
  NULL,
  "Simple legal case: prefixed element"
},
{
  "020.xml",
  126,
  "<?xml version=\"1.0\"?>\n<!-- Simple legal case: prefixed attribute -->\n<foo xmlns:a=\"http://example.org/namespace\" a:attr=\"1\"/>\n",
  0,
  NULL,
  "Simple legal case: prefixed attribute"
},
{
  "021.xml",
  150,
  "<?xml version=\"1.0\"?>\n<!-- Simple legal case: default namespace and unbinding -->\n<foo xmlns=\"http://example.org/namespace\">\n <foo xmlns=\"\"/>\n</foo>\n\n",
  0,
  NULL,
  "Simple legal case: default namespace and unbinding"
},
{
  "022.xml",
  184,
  "<?xml version=\"1.0\"?>\n<!-- Simple legal case: default namespace and rebinding -->\n<foo xmlns=\"http://example.org/namespace\">\n <foo xmlns=\"http://example.org/other-namespace\"/>\n</foo>\n\n",
  0,
  NULL,
  "Simple legal case: default namespace and rebinding"
},
{
  "023.xml",
  167,
  "<?xml version=\"1.0\"?>\n<!-- Illegal use of 1.1-style prefix unbinding in 1.0 document -->\n<a:foo xmlns:a=\"http://example.org/namespace\">\n <a:foo xmlns:a=\"\"/>\n</a:foo>\n\n",
  0,
  NULL,
  "Illegal use of 1.1-style prefix unbinding in 1.0 document"
},
{
  "024.xml",
  179,
  "<?xml version=\"1.0\"?>\n<!-- Simple legal case: prefix rebinding -->\n<a:foo xmlns:a=\"http://example.org/namespace\">\n <a:foo xmlns:a=\"http://example.org/other-namespace\"/>\n</a:foo>\n\n",
  0,
  NULL,
  "Simple legal case: prefix rebinding"
},
{
  "025.xml",
  63,
  "<?xml version=\"1.0\"?>\n<!-- Unbound element prefix -->\n<a:foo/>\n",
  0,
  NULL,
  "Unbound element prefix"
},
{
  "026.xml",
  74,
  "<?xml version=\"1.0\"?>\n<!-- Unbound attribute prefix -->\n<foo a:attr=\"1\"/>\n",
  0,
  NULL,
  "Unbound attribute prefix"
},
{
  "027.xml",
  118,
  "<?xml version=\"1.0\"?>\n<!-- Reserved prefixes and namespaces: using the xml prefix undeclared -->\n<foo xml:lang=\"en\"/>\n",
  0,
  NULL,
  "Reserved prefixes and namespaces: using the xml prefix undeclared"
},
{
  "028.xml",
  156,
  "<?xml version=\"1.0\"?>\n<!-- Reserved prefixes and namespaces: declaring the xml prefix correctly -->\n<foo xmlns:xml=\"http://www.w3.org/XML/1998/namespace\"/>\n",
  0,
  NULL,
  "Reserved prefixes and namespaces: declaring the xml prefix correctly"
},
{
  "029.xml",
  151,
  "<?xml version=\"1.0\"?>\n<!-- Reserved prefixes and namespaces: declaring the xml prefix incorrectly -->\n<foo xmlns:xml=\"http://example.org/namespace\"/>\n\n",
  0,
  NULL,
  "Reserved prefixes and namespaces: declaring the xml prefix incorrectly"
},
{
  "030.xml",
  170,
  "<?xml version=\"1.0\"?>\n<!-- Reserved prefixes and namespaces: binding another prefix\n     to the xml namespace -->\n<foo xmlns:yml=\"http://www.w3.org/XML/1998/namespace\"/>\n",
  0,
  NULL,
  "Reserved prefixes and namespaces: binding another prefix to the xml namespace"
},
{
  "031.xml",
  179,
  "<?xml version=\"1.0\"?>\n<!-- Reserved prefixes and namespaces: declaring the xmlns prefix\n     with its correct URI (illegal) -->\n<foo xmlns:xmlns=\"http://www.w3.org/2000/xmlns/\"/>\n",
  0,
  NULL,
  "Reserved prefixes and namespaces: declaring the xmlns prefix with its correct URI (illegal)"
},
{
  "032.xml",
  170,
  "<?xml version=\"1.0\"?>\n<!-- Reserved prefixes and namespaces: declaring the xmlns prefix\n     with an incorrect URI -->\n<foo xmlns:xmlns=\"http://example.org/namespace\"/>\n\n",
  0,
  NULL,
  "Reserved prefixes and namespaces: declaring the xmlns prefix with an incorrect URI"
},
{
  "033.xml",
  167,
  "<?xml version=\"1.0\"?>\n<!-- Reserved prefixes and namespaces: binding another prefix\n     to the xmlns namespace -->\n<foo xmlns:ymlns=\"http://www.w3.org/2000/xmlns/\"/>\n",
  0,
  NULL,
  "Reserved prefixes and namespaces: binding another prefix to the xmlns namespace"
},
{
  "034.xml",
  140,
  "<?xml version=\"1.0\"?>\n<!-- Reserved prefixes and namespaces: binding a reserved prefix -->\n<foo xmlns:xml2=\"http://example.org/namespace\"/>\n",
  0,
  NULL,
  "Reserved prefixes and namespaces: binding a reserved prefix"
},
{
  "035.xml",
  205,
  "<?xml version=\"1.0\"?>\n<!-- Attribute uniqueness: repeated identical attribute -->\n<foo xmlns:a=\"http://example.org/~wilbur\"\n     xmlns:b=\"http://example.org/~wilbur\">\n\n<bar a:attr=\"1\" a:attr=\"2\"/>\n\n</foo>\n",
  0,
  NULL,
  "Attribute uniqueness: repeated identical attribute"
},
{
  "036.xml",
  219,
  "<?xml version=\"1.0\"?>\n<!-- Attribute uniqueness: repeated attribute with different prefixes -->\n<foo xmlns:a=\"http://example.org/~wilbur\"\n     xmlns:b=\"http://example.org/~wilbur\">\n\n<bar a:attr=\"1\" b:attr=\"2\"/>\n\n</foo>\n",
  0,
  NULL,
  "Attribute uniqueness: repeated attribute with different prefixes"
},
{
  "037.xml",
  218,
  "<?xml version=\"1.0\"?>\n<!-- Attribute uniqueness: different attributes with same local name -->\n<foo xmlns:a=\"http://example.org/~wilbur\"\n     xmlns:b=\"http://example.org/~kipper\">\n\n<bar a:attr=\"1\" b:attr=\"2\"/>\n\n</foo>\n",
  0,
  NULL,
  "Attribute uniqueness: different attributes with same local name"
},
{
  "038.xml",
  193,
  "<?xml version=\"1.0\"?>\n<!-- Attribute uniqueness: prefixed and unprefixed attributes with same\n     local name -->\n<foo xmlns:a=\"http://example.org/~wilbur\">\n\n<bar a:attr=\"1\" attr=\"2\"/>\n\n</foo>\n",
  0,
  NULL,
  "Attribute uniqueness: prefixed and unprefixed attributes with same local name"
},
{
  "039.xml",
  301,
  "<?xml version=\"1.0\"?>\n<!-- Attribute uniqueness: prefixed and unprefixed attributes with same\n     local name, with default namespace -->\n<foo xmlns:a=\"http://example.org/~wilbur\"\n     xmlns:b=\"http://example.org/~kipper\"\n     xmlns=\"http://example.org/~wilbur\">\n\n<b:bar a:attr=\"1\" attr=\"2\"/>\n\n</foo>\n",
  0,
  NULL,
  "Attribute uniqueness: prefixed and unprefixed attributes with same local name, with default namespace"
},
{
  "040.xml",
  290,
  "<?xml version=\"1.0\"?>\n<!-- Attribute uniqueness: prefixed and unprefixed attributes with same\n     local name, with default namespace and element in default namespace -->\n<foo xmlns:a=\"http://example.org/~wilbur\"\n     xmlns=\"http://example.org/~wilbur\">\n\n<bar a:attr=\"1\" attr=\"2\"/>\n\n</foo>\n",
  0,
  NULL,
  "Attribute uniqueness: prefixed and unprefixed attributes with same local name, with default namespace and element in default namespace"
},
{
  "041.xml",
  244,
  "<?xml version=\"1.0\"?>\n<!-- Attribute uniqueness: prefixed and unprefixed attributes with same\n     local name, element in same namespace as prefixed attribute -->\n<foo xmlns:a=\"http://example.org/~wilbur\">\n\n<a:bar a:attr=\"1\" attr=\"2\"/>\n\n</foo>\n",
  0,
  NULL,
  "Attribute uniqueness: prefixed and unprefixed attributes with same local name, element in same namespace as prefixed attribute"
},
{
  "042.xml",
  69,
  "<?xml version=\"1.0\"?>\n<!-- Colon in PI name -->\n<?a:b bogus?>\n<foo/>\n",
  0,
  NULL,
  "Colon in PI name"
},
{
  "043.xml",
  119,
  "<?xml version=\"1.0\"?>\n<!-- Colon in entity name -->\n<!DOCTYPE foo [\n<!ELEMENT foo ANY>\n<!ENTITY a:b \"bogus\">\n]>\n<foo/>\n",
  0,
  NULL,
  "Colon in entity name"
},
{
  "044.xml",
  131,
  "<?xml version=\"1.0\"?>\n<!-- Colon in entity name -->\n<!DOCTYPE foo [\n<!ELEMENT foo ANY>\n<!NOTATION a:b SYSTEM \"notation\">\n]>\n<foo/>\n",
  0,
  NULL,
  "Colon in entity name"
},
{
  "045.xml",
  143,
  "<?xml version=\"1.0\"?>\n<!-- Colon in ID attribute name -->\n<!DOCTYPE foo [\n<!ELEMENT foo ANY>\n<!ATTLIST foo id ID #REQUIRED>\n]>\n<foo id=\"a:b\"/>\n",
  0,
  NULL,
  "Colon in ID attribute name"
},
{
  "046.xml",
  200,
  "<?xml version=\"1.0\"?>\n<!-- Colon in ID attribute name -->\n<!DOCTYPE foo [\n<!ELEMENT foo ANY>\n<!ATTLIST foo id  ID #IMPLIED\n              ref IDREF #IMPLIED>\n]>\n<foo ref=\"a:b\">\n <foo id=\"a:b\"/>\n</foo>\n",
  0,
  NULL,
  "Colon in ID attribute name"
},
/* ==== from eduni/namespaces/1.1 rmt-ns11.xml ==== */
{
  "001.xml",
  231,
  "<?xml version=\"1.1\" encoding=\"iso-8859-1\"?>\n<!-- Namespace name test: a perfectly good http IRI that is not a URI -->\n<!DOCTYPE foo [\n<!ELEMENT foo ANY>\n<!ATTLIST foo xmlns CDATA #IMPLIED>\n]>\n<foo xmlns=\"http://example.org/ros\351\"/>\n\351",
  0,
  NULL,
  "Namespace name test: a perfectly good http IRI that is not a URI"
},
{
  "002.xml",
  575,
  "<?xml version=\"1.1\" encoding=\"iso-8859-1\"?>\n<!-- Namespace inequality test: different escaping of non-ascii letter -->\n<!DOCTYPE foo [\n<!ELEMENT foo ANY>\n<!ATTLIST foo xmlns:a CDATA #IMPLIED\n              xmlns:b CDATA #IMPLIED\n              xmlns:c CDATA #IMPLIED>\n<!ELEMENT bar ANY>\n<!ATTLIST bar a:attr CDATA #IMPLIED\n              b:attr CDATA #IMPLIED\n              c:attr CDATA #IMPLIED>\n]>\n<foo xmlns:a=\"http://example.org/ros\351\"\n     xmlns:b=\"http://example.org/ros%c3%a9\"\n     xmlns:c=\"http://example.org/ros%c3%A9\">\n\n<bar a:attr=\"1\" b:attr=\"2\" c:attr=\"3\"/>\n\n</foo>\n\n\351",
  0,
  NULL,
  "Namespace inequality test: different escaping of non-ascii letter"
},
{
  "003.xml",
  129,
  "<?xml version=\"1.1\"?>\n<!-- 1.1 style prefix unbinding -->\n<foo xmlns:a=\"http://example.org/namespace\">\n <bar xmlns:a=\"\"/>\n</foo>\n",
  0,
  NULL,
  "1.1 style prefix unbinding"
},
{
  "004.xml",
  215,
  "<?xml version=\"1.1\"?>\n<!-- 1.1 style prefix unbinding and rebinding -->\n<foo xmlns:a=\"http://example.org/namespace\">\n <bar xmlns:a=\"\">\n  <foo xmlns:a=\"http://example.org/other-namespace\" a:attr=\"1\"/>\n </bar>\n</foo>\n",
  0,
  NULL,
  "1.1 style prefix unbinding and rebinding"
},
{
  "005.xml",
  148,
  "<?xml version=\"1.1\"?>\n<!-- Illegal use of prefix that has been unbound -->\n<foo xmlns:a=\"http://example.org/namespace\">\n <a:bar xmlns:a=\"\"/>\n</foo>\n",
  0,
  NULL,
  "Illegal use of prefix that has been unbound"
},
{
  "006.xml",
  592,
  "<?xml version=\"1.1\"?>\n<!-- Test whether non-Latin-1 characters are accepted in IRIs, and whether\n     they are correctly distinguished -->\n<!DOCTYPE foo [\n<!ELEMENT foo ANY>\n<!ATTLIST foo xmlns:a CDATA #IMPLIED\n              xmlns:b CDATA #IMPLIED\n              xmlns:c CDATA #IMPLIED>\n<!ELEMENT bar ANY>\n<!ATTLIST bar a:attr CDATA #IMPLIED\n              b:attr CDATA #IMPLIED\n              c:attr CDATA #IMPLIED>\n]>\n<foo xmlns:a=\"http://example.org/&#x50;\"\n     xmlns:b=\"http://example.org/&#x150;\"\n     xmlns:c=\"http://example.org/&#x250;\">\n\n<bar a:attr=\"1\" b:attr=\"2\" c:attr=\"3\"/>\n\n</foo>\n",
  0,
  NULL,
  "Test whether non-Latin-1 characters are accepted in IRIs, and whether they are correctly distinguished"
},
/* ==== from eduni/xml-1.1 xml11.xml ==== */
{
  "001.xml",
  111,
  "<?xml version=\"1.0\"?>\n<!-- External subset has later version number -->\n<!DOCTYPE foo SYSTEM \"001.dtd\">\n<foo/>\n",
  0,
  NULL,
  "External subset has later version number"
},
{
  "002.xml",
  130,
  "<?xml version=\"1.0\"?>\n<!-- External PE has later version number -->\n<!DOCTYPE foo [\n<!ENTITY % pe SYSTEM \"002.pe\">\n%pe;\n]>\n<foo/>\n",
  0,
  NULL,
  "External PE has later version number"
},
{
  "003.xml",
  166,
  "<?xml version=\"1.0\"?>\n<!-- External general entity has later version number -->\n<!DOCTYPE foo [\n<!ELEMENT foo ANY>\n<!ENTITY ent SYSTEM \"003.ent\">\n]>\n<foo>&ent;</foo>\n",
  0,
  NULL,
  "External general entity has later version number"
},
{
  "004.xml",
  164,
  "<!-- External general entity has later version number (no decl means 1.0) -->\n<!DOCTYPE foo [\n<!ELEMENT foo ANY>\n<!ENTITY ent SYSTEM \"004.ent\">\n]>\n<foo>&ent;</foo>\n",
  0,
  NULL,
  "External general entity has later version number (no decl means 1.0)"
},
{
  "005.xml",
  213,
  "<?xml version=\"1.0\"?>\n<!-- Indirect external general entity has later version number -->\n<!DOCTYPE foo [\n<!ELEMENT foo ANY>\n<!ENTITY ent1 SYSTEM \"005_1.ent\">\n<!ENTITY ent2 SYSTEM \"005_2.ent\">\n]>\n<foo>&ent1;</foo>\n",
  0,
  NULL,
  "Indirect external general entity has later version number"
},
{
  "006.xml",
  286,
  "<?xml version=\"1.1\"?>\n<!-- Second-level external general entity has later version number than\n     first-level, but not later than document, so not an error. -->\n<!DOCTYPE foo [\n<!ELEMENT foo ANY>\n<!ENTITY ent1 SYSTEM \"006_1.ent\">\n<!ENTITY ent2 SYSTEM \"006_2.ent\">\n]>\n<foo>&ent1;</foo>\n",
  61,
  "<?xml version=\"1.1\"?><foo>&#10;&#10;some text&#10;&#10;</foo>",
  "Second-level external general entity has later version number than first-level, but not later than document, so not an error."
},
{
  "007.xml",
  103,
  "<?xml version=\"1.1\"?>\n<!-- A vanilla XML 1.1 document -->\n<!DOCTYPE foo [\n<!ELEMENT foo ANY>\n]>\n<foo/>\n",
  32,
  "<?xml version=\"1.1\"?><foo></foo>",
  "A vanilla XML 1.1 document"
},
{
  "008.xml",
  112,
  "<?xml version=\"42.88\"?>\n<!-- an implausibly-versioned document -->\n<!DOCTYPE foo [\n<!ELEMENT foo ANY>\n]>\n<foo/>\n",
  0,
  NULL,
  "an implausibly-versioned document"
},
{
  "009.xml",
  172,
  "<?xml version=\"1.0\"?>\n<!-- External general entity has implausible version number -->\n<!DOCTYPE foo [\n<!ELEMENT foo ANY>\n<!ENTITY ent SYSTEM \"009.ent\">\n]>\n<foo>&ent;</foo>\n",
  0,
  NULL,
  "External general entity has implausible version number"
},
{
  "010.xml",
  164,
  "<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>\n<!-- Contains a C1 control, legal in XML 1.0, illegal in XML 1.1 -->\n<!DOCTYPE foo [\n<!ELEMENT foo ANY>\n]>\n<foo>\200</foo>\n\200",
  13,
  "<foo>\302\200</foo>\200",
  "Contains a C1 control, legal in XML 1.0, illegal in XML 1.1"
},
{
  "011.xml",
  164,
  "<?xml version=\"1.1\" encoding=\"iso-8859-1\"?>\n<!-- Contains a C1 control, legal in XML 1.0, illegal in XML 1.1 -->\n<!DOCTYPE foo [\n<!ELEMENT foo ANY>\n]>\n<foo>\200</foo>\n\200",
  0,
  NULL,
  "Contains a C1 control, legal in XML 1.0, illegal in XML 1.1"
},
{
  "012.xml",
  157,
  "<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>\n<!-- Contains a DEL, legal in XML 1.0, illegal in XML 1.1 -->\n<!DOCTYPE foo [\n<!ELEMENT foo ANY>\n]>\n<foo>\177</foo>\n\177",
  12,
  "<foo>\177</foo>\177",
  "Contains a DEL, legal in XML 1.0, illegal in XML 1.1"
},
{
  "013.xml",
  157,
  "<?xml version=\"1.1\" encoding=\"iso-8859-1\"?>\n<!-- Contains a DEL, legal in XML 1.0, illegal in XML 1.1 -->\n<!DOCTYPE foo [\n<!ELEMENT foo ANY>\n]>\n<foo>\177</foo>\n\177",
  0,
  NULL,
  "Contains a DEL, legal in XML 1.0, illegal in XML 1.1"
},
{
  "014.xml",
  126,
  "<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>\n<!-- Has a \"long s\" in a name, legal in XML 1.1, illegal in XML 1.0 -->\n<egg\305\277/>\n\n\277",
  0,
  NULL,
  "Has a \"long s\" in a name, legal in XML 1.1, illegal in XML 1.0 thru 4th edition"
},
{
  "015.xml",
  120,
  "<?xml version=\"1.1\" encoding=\"utf-8\"?>\n<!-- Has a \"long s\" in a name, legal in XML 1.1, illegal in XML 1.0 -->\n<egg\305\277/>\n\277",
  0,
  NULL,
  "Has a \"long s\" in a name, legal in XML 1.1, illegal in XML 1.0 thru 4th edition"
},
{
  "016.xml",
  133,
  "<?xml version=\"1.0\"?>\n<!-- Has a Byzantine Musical Symbol Kratimata in a name,\n     legal in XML 1.1, illegal in XML 1.0 -->\n<\360\235\200\262/>\n\262",
  0,
  NULL,
  "Has a Byzantine Musical Symbol Kratimata in a name, legal in XML 1.1, illegal in XML 1.0 thru 4th edition"
},
{
  "017.xml",
  133,
  "<?xml version=\"1.1\"?>\n<!-- Has a Byzantine Musical Symbol Kratimata in a name,\n     legal in XML 1.1, illegal in XML 1.0 -->\n<\360\235\200\262/>\n\262",
  0,
  NULL,
  "Has a Byzantine Musical Symbol Kratimata in a name, legal in XML 1.1, illegal in XML 1.0 thru 4th edition"
},
{
  "018.xml",
  98,
  "<?xml version=\"1.1\"?>\n<!-- Has the last legal namechar in XML 1.1, illegal in XML 1.0 -->\n<\363\257\277\277/>\n\277",
  0,
  NULL,
  "Has the last legal namechar in XML 1.1, illegal in XML 1.0 thru 4th edition"
},
{
  "019.xml",
  98,
  "<?xml version=\"1.0\"?>\n<!-- Has the last legal namechar in XML 1.1, illegal in XML 1.0 -->\n<\363\257\277\277/>\n\277",
  0,
  NULL,
  "Has the last legal namechar in XML 1.1, illegal in XML 1.0 thru 4th edition"
},
{
  "020.xml",
  142,
  "<?xml version=\"1.0\"?>\n<!-- Has the first character after the last legal namechar in XML 1.1,\n     illegal in both XML 1.0 and 1.1 -->\n<\363\260\200\200/>\n\200",
  0,
  NULL,
  "Has the first character after the last legal namechar in XML 1.1, illegal in both XML 1.0 and 1.1"
},
{
  "021.xml",
  142,
  "<?xml version=\"1.1\"?>\n<!-- Has the first character after the last legal namechar in XML 1.1,\n     illegal in both XML 1.0 and 1.1 -->\n<\363\260\200\200/>\n\200",
  0,
  NULL,
  "Has the first character after the last legal namechar in XML 1.1, illegal in both XML 1.0 and 1.1"
},
{
  "022.xml",
  224,
  "<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>\n<!-- Has a NEL character; legal in both XML 1.0 and 1.1, but different\n     canonical output because of normalization in 1.1 -->\n<!DOCTYPE foo [\n<!ELEMENT foo ANY>\n]>\n<foo>\205</foo>\n\205",
  13,
  "<foo>\302\205</foo>\205",
  "Has a NEL character; legal in both XML 1.0 and 1.1, but different canonical output because of normalization in 1.1"
},
{
  "023.xml",
  224,
  "<?xml version=\"1.1\" encoding=\"iso-8859-1\"?>\n<!-- Has a NEL character; legal in both XML 1.0 and 1.1, but different\n     canonical output because of normalization in 1.1 -->\n<!DOCTYPE foo [\n<!ELEMENT foo ANY>\n]>\n<foo>\205</foo>\n\205",
  37,
  "<?xml version=\"1.1\"?><foo>&#10;</foo>",
  "Has a NEL character; legal in both XML 1.0 and 1.1, but different canonical output because of normalization in 1.1"
},
{
  "024.xml",
  206,
  "<?xml version=\"1.0\"?>\n<!-- Has an LSEP character; legal in both XML 1.0 and 1.1, but different\n     canonical output because of normalization in 1.1 -->\n<!DOCTYPE foo [\n<!ELEMENT foo ANY>\n]>\n<foo>\342\200\250</foo>\n\250",
  14,
  "<foo>\342\200\250</foo>\250",
  "Has an LSEP character; legal in both XML 1.0 and 1.1, but different canonical output because of normalization in 1.1"
},
{
  "025.xml",
  206,
  "<?xml version=\"1.1\"?>\n<!-- Has an LSEP character; legal in both XML 1.0 and 1.1, but different\n     canonical output because of normalization in 1.1 -->\n<!DOCTYPE foo [\n<!ELEMENT foo ANY>\n]>\n<foo>\342\200\250</foo>\n\250",
  37,
  "<?xml version=\"1.1\"?><foo>&#10;</foo>",
  "Has an LSEP character; legal in both XML 1.0 and 1.1, but different canonical output because of normalization in 1.1"
},
{
  "026.xml",
  216,
  "<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>\n<!-- Has CR-NEL; legal in both XML 1.0 and 1.1, but different\n     canonical output because of normalization in 1.1 -->\n<!DOCTYPE foo [\n<!ELEMENT foo ANY>\n]>\n<foo>\r\205</foo>\n\205",
  18,
  "<foo>&#10;\302\205</foo>\205",
  "Has CR-NEL; legal in both XML 1.0 and 1.1, but different canonical output because of normalization in 1.1"
},
{
  "027.xml",
  216,
  "<?xml version=\"1.1\" encoding=\"iso-8859-1\"?>\n<!-- Has CR-NEL; legal in both XML 1.0 and 1.1, but different\n     canonical output because of normalization in 1.1 -->\n<!DOCTYPE foo [\n<!ELEMENT foo ANY>\n]>\n<foo>\r\205</foo>\n\205",
  37,
  "<?xml version=\"1.1\"?><foo>&#10;</foo>",
  "Has CR-NEL; legal in both XML 1.0 and 1.1, but different canonical output because of normalization in 1.1"
},
{
  "028.xml",
  259,
  "<?xml version=\"1.0\"?>\n<!-- Has CR-LSEP; legal in both XML 1.0 and 1.1, but different\n     canonical output because of normalization in 1.1.\n     Note that CR and LSEP are not combined into a single LF -->\n<!DOCTYPE foo [\n<!ELEMENT foo ANY>\n]>\n<foo>\r\342\200\250</foo>\n\250",
  19,
  "<foo>&#10;\342\200\250</foo>\250",
  "Has CR-LSEP; legal in both XML 1.0 and 1.1, but different canonical output because of normalization in 1.1. Note that CR and LSEP are not combined into a single LF"
},
{
  "029.xml",
  197,
  "<?xml version=\"1.1\"?>\n<!-- Has CR-LSEP; legal in both XML 1.0 and 1.1, but different\n     canonical output because of normalization in 1.1 -->\n<!DOCTYPE foo [\n<!ELEMENT foo ANY>\n]>\n<foo>\r\342\200\250</foo>\n\250",
  42,
  "<?xml version=\"1.1\"?><foo>&#10;&#10;</foo>",
  "Has CR-LSEP; legal in both XML 1.0 and 1.1, but different canonical output because of normalization in 1.1"
},
{
  "030.xml",
  259,
  "<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>\n<!-- Has a NEL character in an NMTOKENS attribute; well-formed in both\n     XML 1.0 and 1.1, but valid only in 1.1 -->\n<!DOCTYPE foo [\n<!ELEMENT foo ANY>\n<!ATTLIST foo bar NMTOKENS #IMPLIED>\n]>\n<foo bar=\"abc\205def\"/>\n\205",
  0,
  NULL,
  "Has a NEL character in an NMTOKENS attribute; well-formed in both XML 1.0 and 1.1, but valid only in 1.1"
},
{
  "031.xml",
  259,
  "<?xml version=\"1.1\" encoding=\"iso-8859-1\"?>\n<!-- Has a NEL character in an NMTOKENS attribute; well-formed in both\n     XML 1.0 and 1.1, but valid only in 1.1 -->\n<!DOCTYPE foo [\n<!ELEMENT foo ANY>\n<!ATTLIST foo bar NMTOKENS #IMPLIED>\n]>\n<foo bar=\"abc\205def\"/>\n\205",
  46,
  "<?xml version=\"1.1\"?><foo bar=\"abc def\"></foo>",
  "Has a NEL character in an NMTOKENS attribute; well-formed in both XML 1.0 and 1.1, but valid only in 1.1"
},
{
  "032.xml",
  241,
  "<?xml version=\"1.0\"?>\n<!-- Has an LSEP character in an NMTOKENS attribute; well-formed in both\n     XML 1.0 and 1.1, but valid only in 1.1 -->\n<!DOCTYPE foo [\n<!ELEMENT foo ANY>\n<!ATTLIST foo bar NMTOKENS #IMPLIED>\n]>\n<foo bar=\"abc\342\200\250def\"/>\n\250",
  0,
  NULL,
  "Has an LSEP character in an NMTOKENS attribute; well-formed in both XML 1.0 and 1.1, but valid only in 1.1"
},
{
  "033.xml",
  241,
  "<?xml version=\"1.1\"?>\n<!-- Has an LSEP character in an NMTOKENS attribute; well-formed in both\n     XML 1.0 and 1.1, but valid only in 1.1 -->\n<!DOCTYPE foo [\n<!ELEMENT foo ANY>\n<!ATTLIST foo bar NMTOKENS #IMPLIED>\n]>\n<foo bar=\"abc\342\200\250def\"/>\n\250",
  46,
  "<?xml version=\"1.1\"?><foo bar=\"abc def\"></foo>",
  "Has an LSEP character in an NMTOKENS attribute; well-formed in both XML 1.0 and 1.1, but valid only in 1.1"
},
{
  "034.xml",
  376,
  "<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>\n<!-- Has an NMTOKENS attribute containing a CR character that comes from a\n     character reference in an internal entity.  Because CR is in the\n     S production, this is valid in both XML 1.0 and 1.1. -->\n<!DOCTYPE foo [\n<!ELEMENT foo ANY>\n<!ATTLIST foo bar  NMTOKENS #IMPLIED>\n<!ENTITY val \"abc&#x0d;def\">\n]>\n<foo bar=\"&val;\" />\n",
  25,
  "<foo bar=\"abc def\"></foo>",
  "Has an NMTOKENS attribute containing a CR character that comes from a character reference in an internal entity. Because CR is in the S production, this is valid in both XML 1.0 and 1.1."
},
{
  "035.xml",
  376,
  "<?xml version=\"1.1\" encoding=\"iso-8859-1\"?>\n<!-- Has an NMTOKENS attribute containing a CR character that comes from a\n     character reference in an internal entity.  Because CR is in the\n     S production, this is valid in both XML 1.0 and 1.1. -->\n<!DOCTYPE foo [\n<!ELEMENT foo ANY>\n<!ATTLIST foo bar  NMTOKENS #IMPLIED>\n<!ENTITY val \"abc&#x0d;def\">\n]>\n<foo bar=\"&val;\" />\n",
  46,
  "<?xml version=\"1.1\"?><foo bar=\"abc def\"></foo>",
  "Has an NMTOKENS attribute containing a CR character that comes from a character reference in an internal entity. Because CR is in the S production, this is valid in both XML 1.0 and 1.1."
},
{
  "036.xml",
  442,
  "<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>\n<!-- Has an NMTOKENS attribute containing a NEL character that comes from a\n     character reference in an internal entity.  Because NEL is not in the\n     S production (even though real NELs are converted to LF on input),\n     this is invalid in both XML 1.0 and 1.1. -->\n<!DOCTYPE foo [\n<!ELEMENT foo ANY>\n<!ATTLIST foo bar  NMTOKENS #IMPLIED>\n<!ENTITY val \"abc&#x85;def\">\n]>\n<foo bar=\"&val;\" />\n",
  0,
  NULL,
  "Has an NMTOKENS attribute containing a NEL character that comes from a character reference in an internal entity. Because NEL is not in the S production (even though real NELs are converted to LF on input), this is invalid in both XML 1.0 and 1.1."
},
{
  "037.xml",
  442,
  "<?xml version=\"1.1\" encoding=\"iso-8859-1\"?>\n<!-- Has an NMTOKENS attribute containing a NEL character that comes from a\n     character reference in an internal entity.  Because NEL is not in the\n     S production (even though real NELs are converted to LF on input),\n     this is invalid in both XML 1.0 and 1.1. -->\n<!DOCTYPE foo [\n<!ELEMENT foo ANY>\n<!ATTLIST foo bar  NMTOKENS #IMPLIED>\n<!ENTITY val \"abc&#x85;def\">\n]>\n<foo bar=\"&val;\" />\n",
  0,
  NULL,
  "Has an NMTOKENS attribute containing a NEL character that comes from a character reference in an internal entity. Because NEL is not in the S production (even though real NELs are converted to LF on input), this is invalid in both XML 1.0 and 1.1."
},
{
  "038.xml",
  186,
  "<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>\n<!-- Contains a C0 control character (form-feed), illegal in\n     both XML 1.0 and 1.1 -->\n<!DOCTYPE foo [\n<!ELEMENT foo ANY>\n]>\n<foo>\f</foo>\n",
  0,
  NULL,
  "Contains a C0 control character (form-feed), illegal in both XML 1.0 and 1.1"
},
{
  "039.xml",
  186,
  "<?xml version=\"1.1\" encoding=\"iso-8859-1\"?>\n<!-- Contains a C0 control character (form-feed), illegal in\n     both XML 1.0 and 1.1 -->\n<!DOCTYPE foo [\n<!ELEMENT foo ANY>\n]>\n<foo>\f</foo>\n",
  0,
  NULL,
  "Contains a C0 control character (form-feed), illegal in both XML 1.0 and 1.1"
},
{
  "040.xml",
  189,
  "<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>\n<!-- Contains a C1 control character (partial line up), legal in\n     XML 1.0 but not 1.1 -->\n<!DOCTYPE foo [\n<!ELEMENT foo ANY>\n]>\n<foo>\214</foo>\n\214",
  13,
  "<foo>\302\214</foo>\214",
  "Contains a C1 control character (partial line up), legal in XML 1.0 but not 1.1"
},
{
  "041.xml",
  189,
  "<?xml version=\"1.1\" encoding=\"iso-8859-1\"?>\n<!-- Contains a C1 control character (partial line up), legal in\n     XML 1.0 but not 1.1 -->\n<!DOCTYPE foo [\n<!ELEMENT foo ANY>\n]>\n<foo>\214</foo>\n\214",
  0,
  NULL,
  "Contains a C1 control character (partial line up), legal in XML 1.0 but not 1.1"
},
{
  "042.xml",
  213,
  "<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>\n<!-- Contains a character reference to a C0 control character (form-feed),\n     legal in XML 1.1 but not 1.0 -->\n<!DOCTYPE foo [\n<!ELEMENT foo ANY>\n]>\n<foo>&#x0c;</foo>\n",
  0,
  NULL,
  "Contains a character reference to a C0 control character (form-feed), legal in XML 1.1 but not 1.0"
},
{
  "043.xml",
  213,
  "<?xml version=\"1.1\" encoding=\"iso-8859-1\"?>\n<!-- Contains a character reference to a C0 control character (form-feed),\n     legal in XML 1.1 but not 1.0 -->\n<!DOCTYPE foo [\n<!ELEMENT foo ANY>\n]>\n<foo>&#x0c;</foo>\n",
  37,
  "<?xml version=\"1.1\"?><foo>&#12;</foo>",
  "Contains a character reference to a C0 control character (form-feed), legal in XML 1.1 but not 1.0"
},
{
  "044.xml",
  248,
  "<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>\n<!-- Contains a character reference to a C1 control character (partial line\n     up), legal in both XML 1.0 and 1.1 (but for different reasons) -->\n<!DOCTYPE foo [\n<!ELEMENT foo ANY>\n]>\n<foo>&#x8c;</foo>\n",
  13,
  "<foo>\302\214</foo>\214",
  "Contains a character reference to a C1 control character (partial line up), legal in both XML 1.0 and 1.1 (but for different reasons)"
},
{
  "045.xml",
  248,
  "<?xml version=\"1.1\" encoding=\"iso-8859-1\"?>\n<!-- Contains a character reference to a C1 control character (partial line\n     up), legal in both XML 1.0 and 1.1 (but for different reasons) -->\n<!DOCTYPE foo [\n<!ELEMENT foo ANY>\n]>\n<foo>&#x8c;</foo>\n",
  38,
  "<?xml version=\"1.1\"?><foo>&#140;</foo>",
  "Contains a character reference to a C1 control character (partial line up), legal in both XML 1.0 and 1.1 (but for different reasons)"
},
{
  "046.xml",
  222,
  "<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>\n<!-- Has a NEL character in element content whitespace; well-formed in both\n     XML 1.0 and 1.1, but valid only in 1.1 -->\n<!DOCTYPE foo [\n<!ELEMENT foo (foo*)>\n]>\n<foo>\205</foo>\n\205",
  0,
  NULL,
  "Has a NEL character in element content whitespace; well-formed in both XML 1.0 and 1.1, but valid only in 1.1"
},
{
  "047.xml",
  222,
  "<?xml version=\"1.1\" encoding=\"iso-8859-1\"?>\n<!-- Has a NEL character in element content whitespace; well-formed in both\n     XML 1.0 and 1.1, but valid only in 1.1 -->\n<!DOCTYPE foo [\n<!ELEMENT foo (foo*)>\n]>\n<foo>\205</foo>\n\205",
  37,
  "<?xml version=\"1.1\"?><foo>&#10;</foo>",
  "Has a NEL character in element content whitespace; well-formed in both XML 1.0 and 1.1, but valid only in 1.1"
},
{
  "048.xml",
  205,
  "<?xml version=\"1.0\"?>\n<!-- Has an LSEP character in element content whitespace; well-formed in both\n     XML 1.0 and 1.1, but valid only in 1.1 -->\n<!DOCTYPE foo [\n<!ELEMENT foo (foo*)>\n]>\n<foo>\342\200\250</foo>\n\n\250",
  0,
  NULL,
  "Has an LSEP character in element content whitespace; well-formed in both XML 1.0 and 1.1, but valid only in 1.1"
},
{
  "049.xml",
  205,
  "<?xml version=\"1.1\"?>\n<!-- has an LSEP character in element content whitespace; well-formed in both\n     XML 1.0 and 1.1, but valid only in 1.1 -->\n<!DOCTYPE foo [\n<!ELEMENT foo (foo*)>\n]>\n<foo>\342\200\250</foo>\n\n\250",
  37,
  "<?xml version=\"1.1\"?><foo>&#10;</foo>",
  "has an LSEP character in element content whitespace; well-formed in both XML 1.0 and 1.1, but valid only in 1.1"
},
{
  "050.xml",
  333,
  "<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>\n<!-- Has element content whitespace containing a CR character that comes from\n     a character reference in an internal entity.  Because CR is in the\n     S production, this is valid in both XML 1.0 and 1.1. -->\n<!DOCTYPE foo [\n<!ELEMENT foo (foo*)>\n<!ENTITY e \"&#x0d;\">\n]>\n<foo>&e;</foo>\n",
  16,
  "<foo>&#13;</foo>",
  "Has element content whitespace containing a CR character that comes from a character reference in an internal entity. Because CR is in the S production, this is valid in both XML 1.0 and 1.1."
},
{
  "051.xml",
  333,
  "<?xml version=\"1.1\" encoding=\"iso-8859-1\"?>\n<!-- Has element content whitespace containing a CR character that comes from\n     a character reference in an internal entity.  Because CR is in the\n     S production, this is valid in both XML 1.0 and 1.1. -->\n<!DOCTYPE foo [\n<!ELEMENT foo (foo*)>\n<!ENTITY e \"&#x0d;\">\n]>\n<foo>&e;</foo>\n",
  37,
  "<?xml version=\"1.1\"?><foo>&#13;</foo>",
  "Has element content whitespace containing a CR character that comes from a character reference in an internal entity. Because CR is in the S production, this is valid in both XML 1.0 and 1.1."
},
{
  "052.xml",
  405,
  "<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>\n<!-- Has element content whitespace containing a NEL character that comes from\n     a character reference in an internal entity.  Because NEL is not in the\n     S production (even though real NELs are converted to LF on input),\n     this is invalid in both XML 1.0 and 1.1. -->\n<!DOCTYPE foo [\n<!ELEMENT foo (foo*)>\n<!ENTITY e \"abc&#x85;def\">\n]>\n<foo>&e;</foo>\n",
  0,
  NULL,
  "Has element content whitespace containing a NEL character that comes from a character reference in an internal entity. Because NEL is not in the S production (even though real NELs are converted to LF on input), this is invalid in both XML 1.0 and 1.1."
},
{
  "053.xml",
  405,
  "<?xml version=\"1.1\" encoding=\"iso-8859-1\"?>\n<!-- Has element content whitespace containing a NEL character that comes from\n     a character reference in an internal entity.  Because NEL is not in the\n     S production (even though real NELs are converted to LF on input),\n     this is invalid in both XML 1.0 and 1.1. -->\n<!DOCTYPE foo [\n<!ELEMENT foo (foo*)>\n<!ENTITY e \"abc&#x85;def\">\n]>\n<foo>&e;</foo>\n",
  0,
  NULL,
  "Has element content whitespace containing a NEL character that comes from a character reference in an internal entity. Because NEL is not in the S production (even though real NELs are converted to LF on input), this is invalid in both XML 1.0 and 1.1."
},
{
  "054.xml",
  497,
  "<?xml version=\"1.1\" encoding=\"iso-8859-1\"?>\n<!-- Contains a character reference to a C0 control character (form-feed)\n     in an entity value.  This will be legal (in XML 1.1) when the entity\n     declaration is parsed, but what about when it is used?  According to\n     the grammar in the CR spec, it should be illegal (because the \n     replacement text must match \"content\"), but this is probably not\n     intended. -->\n<!DOCTYPE foo [\n<!ELEMENT foo ANY>\n<!ENTITY e \"&#x0c;\">\n]>\n<foo>&e;</foo>\n",
  37,
  "<?xml version=\"1.1\"?><foo>&#12;</foo>",
  "Contains a character reference to a C0 control character (form-feed) in an entity value. This will be legal (in XML 1.1) when the entity declaration is parsed, but what about when it is used? According to the grammar in the CR spec, it should be illegal (because the replacement text must match \"content\"), but this is probably not intended. This will be fixed in the PR version."
},
{
  "055.xml",
  101,
  "<?xml version=\"1.1\"\205encoding=\"iso-8859-1\"?>\n<!-- Has a Latin-1 NEL in the XML declaration -->\n<foo/>\n\205",
  0,
  NULL,
  "Has a Latin-1 NEL in the XML declaration (to be made an error in PR)"
},
{
  "056.xml",
  95,
  "<?xml version=\"1.1\"\302\205encoding=\"utf-8\"?>\n<!-- Has a UTF-8 NEL in the XML declaration -->\n<foo/>\n\205",
  0,
  NULL,
  "Has a UTF-8 NEL in the XML declaration (to be made an error in PR)"
},
{
  "057.xml",
  97,
  "<?xml version=\"1.1\"\342\200\250encoding=\"utf-8\"?>\n<!-- Has a UTF-8 LSEP in the XML declaration -->\n<foo/>\n\250",
  0,
  NULL,
  "Has a UTF-8 LSEP in the XML declaration (to be made an error in PR)"
},
/* ==== from sun sun-invalid.xml ==== */
{
  "invalid/dtd01.xml",
  142,
  "<!DOCTYPE root [\n    <!ELEMENT y (#PCDATA|x|x)*>\n    <!-- element types can't repeat in mixed content -->\n    <!ELEMENT root ANY>\n]>\n\n<root/>\n",
  0,
  NULL,
  "Tests the No Duplicate Types VC"
},
{
  "invalid/dtd02.xml",
  122,
  "<!DOCTYPE paper [\n<!ELEMENT paper EMPTY>\n<!ENTITY Brittannica SYSTEM \"http://www.eb.com\" NDATA Encyclopaedia>\n]>\n<paper/>\n",
  0,
  NULL,
  "Tests the \"Notation Declared\" VC by using an undeclared notation name."
},
{
  "invalid/dtd03.xml",
  310,
  "<!DOCTYPE violation [\n<!ELEMENT violation (a,a,a,b)>\n<!ELEMENT a EMPTY>\n<!ELEMENT b EMPTY>\n    <!-- tests the \"element valid\" constraint for content\n\twhich doesn't match the declared content model.\n\t(there can be an infinite number of such tests...)\n\t-->\n]>\n<violation>\n    <a/>\n    <a/>\n    <b/>\n</violation>\n",
  0,
  NULL,
  "Tests the \"Element Valid\" VC (clause 2) by omitting a required element."
},
{
  "invalid/el01.xml",
  70,
  "<!DOCTYPE root [\n<!ELEMENT root ANY>\n]>\n<root> <undeclared/> </root>\n\n",
  0,
  NULL,
  "Tests the Element Valid VC (clause 4) by including an undeclared child element."
},
{
  "invalid/el02.xml",
  63,
  "<!DOCTYPE root [\n<!ELEMENT root EMPTY>\n]>\n<root><root/></root>\n",
  0,
  NULL,
  "Tests the Element Valid VC (clause 1) by including elements in an EMPTY content model."
},
{
  "invalid/el03.xml",
  142,
  "<!DOCTYPE root [\n<!ELEMENT root (#PCDATA|root)*>\n<!ELEMENT exception (#PCDATA)>\n]>\n<root>this is ok <exception>this isn't</exception> </root>\n",
  0,
  NULL,
  "Tests the Element Valid VC (clause 3) by including a child element not permitted by a mixed content model."
},
{
  "invalid/el04.xml",
  110,
  "<!DOCTYPE root [\n<!ELEMENT root ANY>\n<!ELEMENT exception (#PCDATA)>\n<!ELEMENT exception (#PCDATA)>\n]>\n<root/>\n",
  0,
  NULL,
  "Tests the Unique Element Type Declaration VC."
},
{
  "invalid/el05.xml",
  127,
  "<!DOCTYPE root [\n<!ELEMENT root (#PCDATA|repeat-till-done|repeat-till-done)*>\n<!ELEMENT repeat-till-done (#PCDATA)>\n]>\n<root/>\n",
  0,
  NULL,
  "Tests the No Duplicate Types VC."
},
{
  "invalid/el06.xml",
  133,
  "<!DOCTYPE root [\n<!ELEMENT root EMPTY>\n    <!-- in case parsers special-case builtin entities incorrectly -->\n]>\n<root>&amp;</root>\n\n",
  0,
  NULL,
  "Tests the Element Valid VC (clause 1), using one of the predefined internal entities inside an EMPTY content model."
},
{
  "invalid/id01.xml",
  130,
  "<!DOCTYPE root SYSTEM \"../valid/sa.dtd\">\n\n<!-- values of type ID must match \"name\" -->\n\n<root>\n    <attributes id=\"42a\"/>\n</root>\n",
  0,
  NULL,
  "Tests the ID (is a Name) VC"
},
{
  "invalid/id02.xml",
  182,
  "<!DOCTYPE root SYSTEM \"../valid/sa.dtd\">\n\n<!-- a name must not appear more than once as a value of type id -->\n\n<root>\n    <attributes id=\"a42\"/>\n    <attributes id=\"a42\"/>\n</root>\n\n",
  0,
  NULL,
  "Tests the ID (appears once) VC"
},
{
  "invalid/id03.xml",
  172,
  "<!DOCTYPE root SYSTEM \"../valid/sa.dtd\" [\n    <!ATTLIST attributes\n\tid2\tID\t#IMPLIED\n\t>\n]>\n\n<!-- no element type may have more than one ID attribute specified -->\n\n<root/>\n\n",
  0,
  NULL,
  "Tests the One ID per Element Type VC"
},
{
  "invalid/id04.xml",
  150,
  "<!DOCTYPE root [\n    <!ATTLIST root\n\tid2\tID\t\"x23\"\n\t>\n]>\n\n<!-- an ID attribute must have a declared default\n    of #IMPLIED or #REQUIRED\n-->\n\n<root/>\n\n",
  0,
  NULL,
  "Tests the ID Attribute Default VC"
},
{
  "invalid/id05.xml",
  182,
  "<!DOCTYPE root [\n    <!ELEMENT root ANY>\n    <!ATTLIST root\n\tid2\tID\t#FIXED \"x23\"\n\t>\n]>\n\n<!-- an ID attribute must have a declared default\n    of #IMPLIED or #REQUIRED\n-->\n\n<root/>\n\n\n",
  0,
  NULL,
  "Tests the ID Attribute Default VC"
},
{
  "invalid/id06.xml",
  196,
  "<!DOCTYPE root [\n    <!ELEMENT root ANY>\n    <!ATTLIST root\n\tid\tID\t#IMPLIED\n\tidref\tIDREF\t#IMPLIED\n\t>\n]>\n\n<!-- Values of type IDREF must match the name production -->\n\n<root idref=\"36d\">\n</root>\n\n\n",
  0,
  NULL,
  "Tests the IDREF (is a Name) VC"
},
{
  "invalid/id07.xml",
  228,
  "<!DOCTYPE root [\n    <!ELEMENT root ANY>\n    <!ATTLIST root\n\tid\tID\t#IMPLIED\n\tidref\tIDREF\t#IMPLIED\n\tidrefs\tIDREFS\t#IMPLIED\n\t>\n]>\n\n<!-- Values of type IDREFS must match the names production -->\n\n<root idrefs=\"d36 36d\">\n</root>\n\n\n\n",
  0,
  NULL,
  "Tests the IDREFS (is a Names) VC"
},
{
  "invalid/id08.xml",
  211,
  "<!DOCTYPE root [\n    <!ELEMENT root ANY>\n    <!ATTLIST root\n\tid\tID\t#IMPLIED\n\tidref\tIDREF\t#IMPLIED\n\t>\n]>\n\n<!-- each name must match the value of an id attribute on some element -->\n\n<root idref=\"d36d\">\n</root>\n\n\n",
  0,
  NULL,
  "Tests the IDREF (matches an ID) VC"
},
{
  "invalid/id09.xml",
  262,
  "<!DOCTYPE root [\n    <!ELEMENT root ANY>\n    <!ATTLIST root\n\tid\tID\t#IMPLIED\n\tidref\tIDREF\t#IMPLIED\n\tidrefs\tIDREFS\t#IMPLIED\n\t>\n]>\n\n<!-- each name must match the value of an id attribute on some element -->\n\n<root idrefs=\"d36 ee38\">\n    <root id=\"d36\"/>\n</root>\n\n\n\n",
  0,
  NULL,
  "Tests the IDREF (IDREFS matches an ID) VC"
},
{
  "invalid/not-sa01.xml",
  213,
  "<?xml version='1.0' standalone='yes'?>\n\n<!DOCTYPE root SYSTEM \"../valid/sa.dtd\">\n    \n<root>\n    <child>\n    The whitespace before and after this element keeps\n    this from being standalone.\n    </child>\n</root>\n",
  0,
  NULL,
  "Tests the Standalone Document Declaration VC, ensuring that optional whitespace causes a validity error."
},
{
  "invalid/not-sa02.xml",
  734,
  "<?xml version='1.0' standalone='yes'?>\n\n<!DOCTYPE attributes SYSTEM \"../valid/sa.dtd\" [\n    <!ENTITY internal \" number99\">\n]>\n\n    <!-- sync with ../valid/not-sa02.xml -->\n\n    <!--\n\tLOTS of normalized/defaulted attributes\n\tkeep this from being standalone\n\n\tXXX not the best of tests!!\n\t... each type of normalization/defaulting\n\tneeds separate testing\n    -->\n\n<attributes\n    notation =\t\" nonce \"\n    nmtoken =\t\" this-gets-normalized \"\n    nmtokens =\t\" this&#x0d;&#x0a; also\t gets&#x20; normalized \"\n    id =\t\"\t&internal; \"\n    idref =\t\" &internal;\n    \"\n    idrefs =\t\" &internal;  &internal;    &internal;\"\n    entity =\t\" unparsed-1 \"\n    entities =\t\"unparsed-1\n    unparsed-2\t\t\n\"\n    cdata =\t\"nothing happens to this one!\"\n    />\n",
  0,
  NULL,
  "Tests the Standalone Document Declaration VC, ensuring that attributes needing normalization cause a validity error."
},
{
  "invalid/not-sa04.xml",
  172,
  "<?xml version='1.0' standalone='yes'?>\n\n<!DOCTYPE attributes SYSTEM \"../valid/sa.dtd\" [\n    <!--\n\tattribute needs defaulting\n    -->\n]>\n\n<attributes/>\n\n<?pi equals three?>\n",
  0,
  NULL,
  "Tests the Standalone Document Declaration VC, ensuring that attributes needing defaulting cause a validity error."
},
{
  "invalid/not-sa05.xml",
  173,
  "<?xml version='1.0' standalone='yes'?>\n\n<!DOCTYPE attributes SYSTEM \"../valid/sa.dtd\" [\n    <!--\n\tTOKEN needs normalization\n    -->\n]>\n\n<attributes\n    token =\t\" c \"\n    />\n",
  0,
  NULL,
  "Tests the Standalone Document Declaration VC, ensuring that a token attribute that needs normalization causes a validity error."
},
{
  "invalid/not-sa06.xml",
  200,
  "<?xml version='1.0' standalone='yes'?>\n\n<!DOCTYPE attributes SYSTEM \"../valid/sa.dtd\" [\n    <!--\n\tNOTATION needs normalization\n    -->\n]>\n\n<attributes\n    token =\t\"b\"\n    notation =\t\" nonce \"\n    />\n\n",
  0,
  NULL,
  "Tests the Standalone Document Declaration VC, ensuring that a NOTATION attribute that needs normalization causes a validity error."
},
{
  "invalid/not-sa07.xml",
  212,
  "<?xml version='1.0' standalone='yes'?>\n\n<!DOCTYPE attributes SYSTEM \"../valid/sa.dtd\" [\n    <!--\n\tNMTOKEN needs normalization\n    -->\n]>\n\n<attributes\n    token =\t\"b\"\n    nmtoken =\t\" this-gets-normalized \"\n    />\n",
  0,
  NULL,
  "Tests the Standalone Document Declaration VC, ensuring that an NMTOKEN attribute needing normalization causes a validity error."
},
{
  "invalid/not-sa08.xml",
  238,
  "<?xml version='1.0' standalone='yes'?>\n\n<!DOCTYPE attributes SYSTEM \"../valid/sa.dtd\" [\n    <!--\n\tNMTOKENS needs normalization\n    -->\n]>\n\n<attributes\n    token =\t\"b\"\n    nmtokens =\t\" this&#x0d;&#x0a; also\t gets&#x20; normalized \"\n    />\n",
  0,
  NULL,
  "Tests the Standalone Document Declaration VC, ensuring that an NMTOKENS attribute needing normalization causes a validity error."
},
{
  "invalid/not-sa09.xml",
  187,
  "<?xml version='1.0' standalone='yes'?>\n\n<!DOCTYPE attributes SYSTEM \"../valid/sa.dtd\" [\n    <!--\n\tID needs normalization\n    -->\n]>\n\n<attributes\n    token =\t\"b\"\n    id =\t\"\tcindy \"\n    />\n",
  0,
  NULL,
  "Tests the Standalone Document Declaration VC, ensuring that an ID attribute needing normalization causes a validity error."
},
{
  "invalid/not-sa10.xml",
  212,
  "<?xml version='1.0' standalone='yes'?>\n\n<!DOCTYPE attributes SYSTEM \"../valid/sa.dtd\" [\n    <!--\n\tIDREF needs normalization\n    -->\n]>\n\n<attributes\n    token =\t\"b\"\n    id =\t\"id43\"\n    idref =\t\" id43\n    \"\n    />\n",
  0,
  NULL,
  "Tests the Standalone Document Declaration VC, ensuring that an IDREF attribute needing normalization causes a validity error."
},
{
  "invalid/not-sa11.xml",
  234,
  "<?xml version='1.0' standalone='yes'?>\n\n<!DOCTYPE attributes SYSTEM \"../valid/sa.dtd\" [\n    <!--\n\tIDREFS needs normalization\n    -->\n]>\n\n<attributes\n    token =\t\"b\"\n    id =\t\"date28\"\n    idrefs =\t\" date28   date28\n    date28\t\"\n    />\n",
  0,
  NULL,
  "Tests the Standalone Document Declaration VC, ensuring that an IDREFS attribute needing normalization causes a validity error."
},
{
  "invalid/not-sa12.xml",
  200,
  "<?xml version='1.0' standalone='yes'?>\n\n<!DOCTYPE attributes SYSTEM \"../valid/sa.dtd\" [\n    <!--\n\tENTITY needs normalization\n    -->\n]>\n\n<attributes\n    token =\t\"b\"\n    entity =\t\" unparsed-1 \"\n    />\n",
  0,
  NULL,
  "Tests the Standalone Document Declaration VC, ensuring that an ENTITY attribute needing normalization causes a validity error."
},
{
  "invalid/not-sa13.xml",
  232,
  "<?xml version='1.0' standalone='yes'?>\n\n<!DOCTYPE attributes SYSTEM \"../valid/sa.dtd\" [\n    <!--\n\tENTITIES needs normalization\n    -->\n]>\n\n<attributes\n    token =\t\"b\"\n    entities =\t\"\n    unparsed-1\n    \n    unparsed-2\n\t\t  \"\n    />\n",
  0,
  NULL,
  "Tests the Standalone Document Declaration VC, ensuring that an ENTITIES attribute needing normalization causes a validity error."
},
{
  "invalid/not-sa14.xml",
  291,
  "<?xml version='1.0' standalone='yes'?>\n\n<!DOCTYPE root SYSTEM \"../valid/sa.dtd\">\n    \n<root><![CDATA[\n    ]]><child>\n    The whitespace before and after this element keeps\n    this from being standalone.  (CDATA is just another\n    way to represent text...)\n    </child><![CDATA[\n]]></root>\n",
  0,
  NULL,
  "CDATA sections containing only whitespace do not match the nonterminal S, and cannot appear in these positions."
},
{
  "invalid/optional01.xml",
  79,
  "<!DOCTYPE root SYSTEM \"../valid/dtdtest.dtd\">\n<root>\n    <once></once>\n</root>\n",
  0,
  NULL,
  "Tests the Element Valid VC (clause 2) for one instance of \"children\" content model, providing no children where one is required."
},
{
  "invalid/optional02.xml",
  88,
  "<!DOCTYPE root SYSTEM \"../valid/dtdtest.dtd\">\n<root>\n    <once><e/><e/></once>\n</root>\n\n",
  0,
  NULL,
  "Tests the Element Valid VC (clause 2) for one instance of \"children\" content model, providing two children where one is required."
},
{
  "invalid/optional03.xml",
  82,
  "<!DOCTYPE root SYSTEM \"../valid/dtdtest.dtd\">\n<root>\n    <twice></twice>\n</root>\n\n",
  0,
  NULL,
  "Tests the Element Valid VC (clause 2) for one instance of \"children\" content model, providing no children where two are required."
},
{
  "invalid/optional04.xml",
  94,
  "<!DOCTYPE root SYSTEM \"../valid/dtdtest.dtd\">\n<root>\n    <twice><e/><e/><e/></twice>\n</root>\n\n",
  0,
  NULL,
  "Tests the Element Valid VC (clause 2) for one instance of \"children\" content model, providing three children where two are required."
},
{
  "invalid/optional05.xml",
  102,
  "<!DOCTYPE root SYSTEM \"../valid/dtdtest.dtd\">\n<root>\n    <once-or-twice-a></once-or-twice-a>\n</root>\n\n",
  0,
  NULL,
  "Tests the Element Valid VC (clause 2) for one instance of \"children\" content model, providing no children where one or two are required (one construction of that model)."
},
{
  "invalid/optional06.xml",
  103,
  "<!DOCTYPE root SYSTEM \"../valid/dtdtest.dtd\">\n<root>\n    <once-or-twice-b></once-or-twice-b>\n</root>\n\n\n",
  0,
  NULL,
  "Tests the Element Valid VC (clause 2) for one instance of \"children\" content model, providing no children where one or two are required (a second construction of that model)."
},
{
  "invalid/optional07.xml",
  103,
  "<!DOCTYPE root SYSTEM \"../valid/dtdtest.dtd\">\n<root>\n    <once-or-twice-c></once-or-twice-c>\n</root>\n\n\n",
  0,
  NULL,
  "Tests the Element Valid VC (clause 2) for one instance of \"children\" content model, providing no children where one or two are required (a third construction of that model)."
},
{
  "invalid/optional08.xml",
  103,
  "<!DOCTYPE root SYSTEM \"../valid/dtdtest.dtd\">\n<root>\n    <once-or-twice-d></once-or-twice-d>\n</root>\n\n\n",
  0,
  NULL,
  "Tests the Element Valid VC (clause 2) for one instance of \"children\" content model, providing no children where one or two are required (a fourth construction of that model)."
},
{
  "invalid/optional09.xml",
  103,
  "<!DOCTYPE root SYSTEM \"../valid/dtdtest.dtd\">\n<root>\n    <once-or-twice-e></once-or-twice-e>\n</root>\n\n\n",
  0,
  NULL,
  "Tests the Element Valid VC (clause 2) for one instance of \"children\" content model, providing no children where one or two are required (a fifth construction of that model)."
},
{
  "invalid/optional10.xml",
  115,
  "<!DOCTYPE root SYSTEM \"../valid/dtdtest.dtd\">\n<root>\n    <once-or-twice-a><e/><e/><e/></once-or-twice-a>\n</root>\n\n\n",
  0,
  NULL,
  "Tests the Element Valid VC (clause 2) for one instance of \"children\" content model, providing three children where one or two are required (a basic construction of that model)."
},
{
  "invalid/optional11.xml",
  116,
  "<!DOCTYPE root SYSTEM \"../valid/dtdtest.dtd\">\n<root>\n    <once-or-twice-b><e/><e/><e/></once-or-twice-b>\n</root>\n\n\n\n",
  0,
  NULL,
  "Tests the Element Valid VC (clause 2) for one instance of \"children\" content model, providing three children where one or two are required (a second construction of that model)."
},
{
  "invalid/optional12.xml",
  116,
  "<!DOCTYPE root SYSTEM \"../valid/dtdtest.dtd\">\n<root>\n    <once-or-twice-c><e/><e/><e/></once-or-twice-c>\n</root>\n\n\n\n",
  0,
  NULL,
  "Tests the Element Valid VC (clause 2) for one instance of \"children\" content model, providing three children where one or two are required (a third construction of that model)."
},
{
  "invalid/optional13.xml",
  116,
  "<!DOCTYPE root SYSTEM \"../valid/dtdtest.dtd\">\n<root>\n    <once-or-twice-d><e/><e/><e/></once-or-twice-d>\n</root>\n\n\n\n",
  0,
  NULL,
  "Tests the Element Valid VC (clause 2) for one instance of \"children\" content model, providing three children where one or two are required (a fourth construction of that model)."
},
{
  "invalid/optional14.xml",
  116,
  "<!DOCTYPE root SYSTEM \"../valid/dtdtest.dtd\">\n<root>\n    <once-or-twice-e><e/><e/><e/></once-or-twice-e>\n</root>\n\n\n\n",
  0,
  NULL,
  "Tests the Element Valid VC (clause 2) for one instance of \"children\" content model, providing three children where one or two are required (a fifth construction of that model)."
},
{
  "invalid/optional20.xml",
  101,
  "<!DOCTYPE root SYSTEM \"../valid/dtdtest.dtd\">\n<root>\n    <once-or-twice-a></once-or-twice-a>\n</root>\n",
  0,
  NULL,
  "Tests the Element Valid VC (clause 2) for one instance of \"children\" content model, providing no children where one or more are required (a sixth construction of that model)."
},
{
  "invalid/optional21.xml",
  102,
  "<!DOCTYPE root SYSTEM \"../valid/dtdtest.dtd\">\n<root>\n    <once-or-twice-b></once-or-twice-b>\n</root>\n\n",
  0,
  NULL,
  "Tests the Element Valid VC (clause 2) for one instance of \"children\" content model, providing no children where one or more are required (a seventh construction of that model)."
},
{
  "invalid/optional22.xml",
  102,
  "<!DOCTYPE root SYSTEM \"../valid/dtdtest.dtd\">\n<root>\n    <once-or-twice-c></once-or-twice-c>\n</root>\n\n",
  0,
  NULL,
  "Tests the Element Valid VC (clause 2) for one instance of \"children\" content model, providing no children where one or more are required (an eigth construction of that model)."
},
{
  "invalid/optional23.xml",
  102,
  "<!DOCTYPE root SYSTEM \"../valid/dtdtest.dtd\">\n<root>\n    <once-or-twice-d></once-or-twice-d>\n</root>\n\n",
  0,
  NULL,
  "Tests the Element Valid VC (clause 2) for one instance of \"children\" content model, providing no children where one or more are required (a ninth construction of that model)."
},
{
  "invalid/optional24.xml",
  102,
  "<!DOCTYPE root SYSTEM \"../valid/dtdtest.dtd\">\n<root>\n    <once-or-twice-e></once-or-twice-e>\n</root>\n\n",
  0,
  NULL,
  "Tests the Element Valid VC (clause 2) for one instance of \"children\" content model, providing no children where one or more are required (a tenth construction of that model)."
},
{
  "invalid/optional25.xml",
  118,
  "<!DOCTYPE root SYSTEM \"../valid/dtdtest.dtd\">\n<root>\n    <once-or-twice-e>No text allowed!</once-or-twice-e>\n</root>\n\n",
  0,
  NULL,
  "Tests the Element Valid VC (clause 2) for one instance of \"children\" content model, providing text content where one or more elements are required."
},
{
  "invalid/required00.xml",
  149,
  "<!DOCTYPE root [\n    <!ELEMENT root EMPTY>\n    <!ATTLIST root\n\treq CDATA #REQUIRED\n\t>\n]>\n\n<root/>\n\n<!-- doesn't include required 'req' attribute -->\n",
  0,
  NULL,
  "Tests the Required Attribute VC."
},
{
  "invalid/required01.xml",
  122,
  "<!DOCTYPE root [\n    <!ELEMENT root EMPTY>\n]>\n\n<root xml:space='preserve'/>\n\n    <!-- all attributes must be declared -->\n",
  0,
  NULL,
  "Tests the Attribute Value Type (declared) VC for the xml:space attribute"
},
{
  "invalid/required02.xml",
  116,
  "<!DOCTYPE root [\n    <!ELEMENT root EMPTY>\n]>\n\n<root xml:lang='en'/>\n\n    <!-- all attributes must be declared -->\n\n",
  0,
  NULL,
  "Tests the Attribute Value Type (declared) VC for the xml:lang attribute"
},
{
  "invalid/root.xml",
  178,
  "<?xml version='1.0' standalone='yes'?>\n\n<!DOCTYPE attributes SYSTEM \"../valid/sa.dtd\">\n\n<!-- the name in the dtd must match the element type of the root element -->\n    \n<root/>\n",
  0,
  NULL,
  "Tests the Root Element Type VC"
},
{
  "invalid/attr01.xml",
  236,
  "<!DOCTYPE root [\n<!ELEMENT root EMPTY>\n<!ATTLIST root\n    affiliated\tENTITY\t#REQUIRED\n    >\n    <!-- tests the \"entity name\" VC ... the \"entity declared\" clause,\n\tas applied to attributes of type ENTITY -->\n]>\n<root affiliated=\"food\"/>\n",
  0,
  NULL,
  "Tests the \"Entity Name\" VC for the ENTITY attribute type."
},
{
  "invalid/attr02.xml",
  398,
  "<!DOCTYPE root [\n<!ELEMENT root EMPTY>\n<!ATTLIST root\n    affiliated\tENTITIES\t#REQUIRED\n    >\n    <!-- tests the \"entity name\" VC ... the \"entity declared\" clause,\n\tas applied to attributes of type ENTITIES -->\n<!NOTATION fruit\n    PUBLIC \"-//International Grocery Consortium//Edible//Healthy//EN\">\n<!ENTITY apple SYSTEM \"http://www.apple.com\" NDATA fruit>\n]>\n<root affiliated=\"apple apple food\"/>\n",
  0,
  NULL,
  "Tests the \"Entity Name\" VC for the ENTITIES attribute type."
},
{
  "invalid/attr03.xml",
  517,
  "<!DOCTYPE root [\n<!ELEMENT root EMPTY>\n<!ATTLIST root\n    type\tNOTATION\t(fruit | vegetable)\t#REQUIRED\n    >\n<!NOTATION fruit\n    PUBLIC \"-//International Grocery Consortium//Edible//Healthy//EN\">\n<!NOTATION vegetable\n    PUBLIC \"-//International Grocery Consortium//Edible//Yucky//EN\">\n<!NOTATION candy\n    PUBLIC \"-//International Grocery Consortium//Edible//Yummy//EN\">\n\n    <!-- tests the 'must match one of the names included in the\n\tdeclaration' part of the \"Notation Attributes\" VC -->\n]>\n<root type=\"candy\"/>\n\n",
  0,
  NULL,
  "Tests the \"Notation Attributes\" VC for the NOTATION attribute type, first clause: value must be one of the ones that's declared."
},
{
  "invalid/attr04.xml",
  339,
  "<!DOCTYPE root [\n<!ELEMENT root EMPTY>\n<!ATTLIST root\n    type\tNOTATION\t(fruit | vegetable)\t#REQUIRED\n    >\n<!NOTATION fruit\n    PUBLIC \"-//International Grocery Consortium//Edible//Healthy//EN\">\n\n    <!-- tests the 'all notation names in the declaration must\n\tbe declared' part of the \"Notation Attributes\" VC -->\n]>\n<root type=\"fruit\"/>\n",
  0,
  NULL,
  "Tests the \"Notation Attributes\" VC for the NOTATION attribute type, second clause: the names in the declaration must all be declared."
},
{
  "invalid/attr05.xml",
  178,
  "<!DOCTYPE root [\n<!ELEMENT root EMPTY>\n<!ATTLIST root\n    token\tNMTOKEN\t\t#REQUIRED\n    >\n\n    <!-- tests the \"name token' VC for an NMTOKEN value -->\n]>\n<root token=\"dev@null\"/>\n",
  0,
  NULL,
  "Tests the \"Name Token\" VC for the NMTOKEN attribute type."
},
{
  "invalid/attr06.xml",
  188,
  "<!DOCTYPE root [\n<!ELEMENT root EMPTY>\n<!ATTLIST root\n    token\tNMTOKEN\t\t#REQUIRED\n    >\n\n    <!-- tests the \"name token' VC for an NMTOKENS value -->\n]>\n<root token=\"now is the time!?\"/>\n",
  0,
  NULL,
  "Tests the \"Name Token\" VC for the NMTOKENS attribute type."
},
{
  "invalid/attr07.xml",
  240,
  "<!DOCTYPE arbor [\n<!ELEMENT arbor EMPTY>\n<!ATTLIST arbor\n    type\t(fruit | vegetable)\t\"fruit\"\n    >\n    <!-- tests the 'must match one of the nmtokens included in the\n\tdeclaration' part of the \"Enumeration\" VC -->\n]>\n<arbor type=\"money\"/>\n\n",
  0,
  NULL,
  "Tests the \"Enumeration\" VC by providing a value which wasn't one of the choices."
},
{
  "invalid/attr08.xml",
  246,
  "<!DOCTYPE palimpest [\n<!ELEMENT palimpest EMPTY>\n<!ATTLIST palimpest\n    xmlns CDATA #FIXED \"http://java.sun.com/historical\"\n    >\n    <!-- tests the \"fixed attribute default\" vc -->\n]>\n\n<palimpest xmlns=\"http://over.the.rainbow.com/somewhere\"/>\n",
  0,
  NULL,
  "Tests the \"Fixed Attribute Default\" VC by providing the wrong value."
},
{
  "invalid/attr09.xml",
  371,
  "<!DOCTYPE collection [\n\n<!ELEMENT collection ANY>\n\n<!ELEMENT identifier EMPTY>\n<!ATTLIST identifier\n    value\tIDREF\t\"42\"\n    >\n    <!-- tests the \"attribute default legal\" vc -->\n\n<!ELEMENT identified EMPTY>\n<!ATTLIST identified\n    id\t\tID\t#REQUIRED\n    >\n]>\n\n<collection>\n    <identifier name=\"i-am-not-a-number\"/>\n    <identified id=\"i-am-not-a-number\"/>\n</collection>\n",
  0,
  NULL,
  "Tests the \"Attribute Default Legal\" VC by providing an illegal IDREF value."
},
{
  "invalid/attr10.xml",
  390,
  "<!DOCTYPE collection [\n\n<!ELEMENT collection ANY>\n\n<!ELEMENT identifier EMPTY>\n<!ATTLIST identifier\n    value\tIDREFS\t\"i-am-not-a-number 42\"\n    >\n    <!-- tests the \"attribute default legal\" vc -->\n\n<!ELEMENT identified EMPTY>\n<!ATTLIST identified\n    id\t\tID\t#REQUIRED\n    >\n]>\n\n<collection>\n    <identifier name=\"i-am-not-a-number\"/>\n    <identified id=\"i-am-not-a-number\"/>\n</collection>\n",
  0,
  NULL,
  "Tests the \"Attribute Default Legal\" VC by providing an illegal IDREFS value."
},
{
  "invalid/attr11.xml",
  409,
  "<!DOCTYPE reference [\r\n\r\n<!ELEMENT reference EMPTY>\r\n<!ATTLIST reference\r\n    value\tENTITY\t\"2orldbook\"\r\n    >\r\n    <!-- tests the \"attribute default legal\" vc -->\r\n\r\n<!NOTATION encyclopaedia PUBLIC \"-//fooCorp Inc//NOTATION something//EN\">\r\n<!ENTITY brittannica SYSTEM \"http://www.eb.com/\" NDATA encyclopaedia>\r\n<!ENTITY worldbook SYSTEM \"http://www.worldbook.com\">\r\n\r\n]>\r\n\r\n<reference value=\"brittannica\"/>\r\n",
  0,
  NULL,
  "Tests the \"Attribute Default Legal\" VC by providing an illegal ENTITY value."
},
{
  "invalid/attr12.xml",
  427,
  "<!DOCTYPE references [\r\n\r\n<!ELEMENT references EMPTY>\r\n<!ATTLIST references\r\n    value\tENTITIES\t\"brittannica 2orldbook\"\r\n    >\r\n    <!-- tests the \"attribute default legal\" vc -->\r\n\r\n<!NOTATION encyclopaedia PUBLIC \"-//fooCorp Inc//NOTATION something//EN\">\r\n<!ENTITY brittannica SYSTEM \"http://www.eb.com/\" NDATA encyclopaedia>\r\n<!ENTITY worldbook SYSTEM \"http://www.worldbook.com\">\r\n\r\n]>\r\n\r\n<references value=\"brittannica\"/>\r\n",
  0,
  NULL,
  "Tests the \"Attribute Default Legal\" VC by providing an illegal ENTITIES value."
},
{
  "invalid/attr13.xml",
  177,
  "<!DOCTYPE root [\n\n<!ELEMENT root EMPTY>\n<!ATTLIST root\n    value\tNMTOKEN\t\"alpha/beta\"\n    >\n    <!-- tests the \"attribute default legal\" vc -->\n]>\n\n<root value=\"brittannica\"/>\n\n",
  0,
  NULL,
  "Tests the \"Attribute Default Legal\" VC by providing an illegal NMTOKEN value."
},
{
  "invalid/attr14.xml",
  188,
  "<!DOCTYPE root [\n\n<!ELEMENT root EMPTY>\n<!ATTLIST root\n    value\tNMTOKENS\t\"alpha beta $gamma\"\n    >\n    <!-- tests the \"attribute default legal\" vc -->\n]>\n\n<root value=\"zeta eta iota\"/>\n\n\n",
  0,
  NULL,
  "Tests the \"Attribute Default Legal\" VC by providing an illegal NMTOKENS value."
},
{
  "invalid/attr15.xml",
  332,
  "<!DOCTYPE reference [\n\n<!ELEMENT reference EMPTY>\n<!ATTLIST reference\n    source\tNOTATION\t(brittannica | worldbook) \"encarta\"\n    >\n    <!-- tests the \"attribute default legal\" vc -->\n\n<!NOTATION brittannica SYSTEM \"http://www.eb.com/\">\n<!NOTATION worldbook SYSTEM \"http://www.worldbook.com\">\n\n]>\n\n<reference source=\"brittannica\"/>\n",
  0,
  NULL,
  "Tests the \"Attribute Default Legal\" VC by providing an illegal NOTATIONS value."
},
{
  "invalid/attr16.xml",
  191,
  "<!DOCTYPE root [\n\n<!ELEMENT root EMPTY>\n<!ATTLIST root\n    value\t(brittannica | worldbook) \"encarta\"\n    >\n    <!-- tests the \"attribute default legal\" vc -->\n]>\n\n<root value=\"brittannica\"/>\n",
  0,
  NULL,
  "Tests the \"Attribute Default Legal\" VC by providing an illegal enumeration value."
},
{
  "invalid/utf16b.xml",
  98,
  "\376\377\0<\0?\0x\0m\0l\0 \0v\0e\0r\0s\0i\0o\0n\0=\0'\0611\0.\0600\0'\0 \0e\0n\0c\0o\0d\0i\0n\0g\0=\0'\0U\0T\0F\0-\0611\0666\0'\0?\0>\0\n\0<\0r\0o\0o\0t\0/\0>\0\n\0",
  0,
  NULL,
  "Tests reading an invalid \"big endian\" UTF-16 document"
},
{
  "invalid/utf16l.xml",
  98,
  "\377\376<\0?\0x\0m\0l\0 \0v\0e\0r\0s\0i\0o\0n\0=\0'\0611\0.\0600\0'\0 \0e\0n\0c\0o\0d\0i\0n\0g\0=\0'\0U\0T\0F\0-\0611\0666\0'\0?\0>\0\n\0<\0r\0o\0o\0t\0/\0>\0\n\0",
  0,
  NULL,
  "Tests reading an invalid \"little endian\" UTF-16 document"
},
{
  "invalid/empty.xml",
  336,
  "<!--\r\n    From: \"Henry S. Thompson\" <ht@cogsci.ed.ac.uk>\r\n\r\n    I'd be interested in reports from validating parsers wrt the\r\n    following:\r\n-->\r\n\r\n<!DOCTYPE foo [\r\n<!ELEMENT foo (a+)>\r\n<!ENTITY empty ''>\r\n<!ENTITY space ' '>\r\n<!ELEMENT a EMPTY>]>\r\n<foo>\r\n&empty;\r\n<a/>\r\n&space;\r\n<a/>\r\n<![CDATA[]]>\r\n<a/>\r\n<![CDATA[ ]]>\r\n<a/>\r\n</foo>\r\n",
  0,
  NULL,
  "CDATA section containing only white space does not match the nonterminal S, and cannot appear in these positions."
},
/* ==== from sun sun-valid.xml ==== */
{
  "valid/pe01.xml",
  42,
  "<!DOCTYPE root SYSTEM \"pe01.dtd\">\n<root/>\n",
  0,
  NULL,
  "Parameter entities references are NOT RECOGNIZED in default attribute values."
},
{
  "valid/dtd00.xml",
  110,
  "<!DOCTYPE root [\n    <!ELEMENT root EMPTY>\n    <!ELEMENT x (#PCDATA)>\n    <!ELEMENT y (#PCDATA)*>\n]>\n\n<root/>\n",
  13,
  "<root></root>",
  "Tests parsing of alternative forms of text-only mixed content declaration."
},
{
  "valid/dtd01.xml",
  127,
  "<!DOCTYPE root [\n    <!ELEMENT root EMPTY>\n    <!ENTITY % PE \"this is a PE\">\n    <!-- %these; %are; %not; %PEs; -->\n]>\n<root/>\n",
  13,
  "<root></root>",
  "Comments don't get parameter entity expansion"
},
{
  "valid/element.xml",
  706,
  "<!DOCTYPE root [\n<!ELEMENT root ANY>\n<!ELEMENT empty EMPTY>\n<!ELEMENT mixed1 (#PCDATA)>\n<!ELEMENT mixed2 (#PCDATA)*>\n<!ELEMENT mixed3 (#PCDATA|empty)*>\n]>\n\n<root>\n    <empty/>\n\n    <mixed1/>\n    <mixed1></mixed1>\n\n    <mixed2/>\n    <mixed2></mixed2>\n\n    <mixed3/>\n    <mixed3></mixed3>\n\n    <mixed1>allowed</mixed1>\n    <mixed1><![CDATA[<allowed>]]></mixed1>\n\n    <mixed2>also</mixed2>\n    <mixed2><![CDATA[<% illegal otherwise %>]]></mixed2>\n\n    <mixed3>moreover</mixed3>\n\n    <mixed1>allowed &amp; stuff</mixed1>\n\n    <mixed2>also</mixed2>\n\n    <mixed3>moreover <empty></empty> </mixed3>\n    <mixed3>moreover <empty/> </mixed3>\n    <mixed3><empty/> </mixed3>\n    <mixed3><empty/> too</mixed3>\n\n</root>\n",
  705,
  "<root>&#10;    <empty></empty>&#10;&#10;    <mixed1></mixed1>&#10;    <mixed1></mixed1>&#10;&#10;    <mixed2></mixed2>&#10;    <mixed2></mixed2>&#10;&#10;    <mixed3></mixed3>&#10;    <mixed3></mixed3>&#10;&#10;    <mixed1>allowed</mixed1>&#10;    <mixed1>&lt;allowed&gt;</mixed1>&#10;&#10;    <mixed2>also</mixed2>&#10;    <mixed2>&lt;% illegal otherwise %&gt;</mixed2>&#10;&#10;    <mixed3>moreover</mixed3>&#10;&#10;    <mixed1>allowed &amp; stuff</mixed1>&#10;&#10;    <mixed2>also</mixed2>&#10;&#10;    <mixed3>moreover <empty></empty> </mixed3>&#10;    <mixed3>moreover <empty></empty> </mixed3>&#10;    <mixed3><empty></empty> </mixed3>&#10;    <mixed3><empty></empty> too</mixed3>&#10;&#10;</root>",
  "Tests clauses 1, 3, and 4 of the Element Valid validity constraint."
},
{
  "valid/ext01.xml",
  206,
  "<!DOCTYPE root [\n<!ELEMENT root ANY>\n<!ELEMENT foo ANY>\n<!ELEMENT bar ANY>\n<!ELEMENT is ANY>\n<!ENTITY root SYSTEM \"ext01.ent\">\n<!ENTITY null SYSTEM \"null.ent\">\n]>\n<root> &root; &root; &null; &null; </root>\n",
  244,
  "<root> &#10;&#10;<foo></foo>&#10;&#10;<bar></bar>&#10;&#10;<is> &quot;dumber than a bag full of hammers&quot; </is>&#10; &#10;&#10;<foo></foo>&#10;&#10;<bar></bar>&#10;&#10;<is> &quot;dumber than a bag full of hammers&quot; </is>&#10;   </root>",
  "Tests use of external parsed entities with and without content."
},
{
  "valid/ext02.xml",
  192,
  "<!DOCTYPE foo [\n<!ELEMENT foo (root*)>\n<!ELEMENT root EMPTY>\n<!ENTITY utf16b SYSTEM \"../invalid/utf16b.xml\">\n<!ENTITY utf16l SYSTEM \"../invalid/utf16l.xml\">\n]>\n\n<foo> &utf16b; &utf16l; </foo>\n",
  60,
  "<foo> &#10;<root></root>&#10; &#10;<root></root>&#10; </foo>",
  "Tests use of external parsed entities with different encodings than the base document."
},
{
  "valid/not-sa01.xml",
  199,
  "<?xml version='1.0' standalone='no'?>\n\n<!DOCTYPE root SYSTEM \"sa.dtd\">\n\n<root>\n    <child>\n    The whitespace before and after this element keeps\n    this from being standalone.\n    </child>\n</root>\n",
  307,
  "<!DOCTYPE root [\n<!NOTATION bar SYSTEM 'file:/dev/tty'>\n<!NOTATION foo PUBLIC '-//public id//foo' 'file:/dev/null'>\n<!NOTATION nonce SYSTEM 'file:/dev/null'>\n]>\n<root>&#10;    <child>&#10;    The whitespace before and after this element keeps&#10;    this from being standalone.&#10;    </child>&#10;</root>",
  "A non-standalone document is valid if declared as such."
},
{
  "valid/not-sa02.xml",
  669,
  "<?xml version='1.0' standalone='no'?>\n\n<!DOCTYPE attributes SYSTEM \"../valid/sa.dtd\" [\n    <!ENTITY internal \" number99\">\n]>\n\n    <!-- sync with ../invalid/not-sa02.xml -->\n\n    <!--\n\tlots of normalized/defaulted attributes\n\tkeep this from being standalone\n\n\tXXX not the best basis for negative tests!!\n    -->\n\n<attributes\n    notation =\t\" nonce \"\n    nmtoken =\t\" this-gets-normalized \"\n    nmtokens =\t\" this \t\n also\t gets normalized \"\n    id =\t\"\t&internal; \"\n    idref =\t\" &internal;\n    \"\n    idrefs =\t\" &internal;  &internal;    &internal;\"\n    entity =\t\" unparsed-1 \"\n    entities =\t\"unparsed-1\n    unparsed-2\t\t\n\"\n    cdata =\t\"nothing happens to this one!\"\n    />\n",
  444,
  "<!DOCTYPE attributes [\n<!NOTATION bar SYSTEM 'file:/dev/tty'>\n<!NOTATION foo PUBLIC '-//public id//foo' 'file:/dev/null'>\n<!NOTATION nonce SYSTEM 'file:/dev/null'>\n]>\n<attributes cdata=\"nothing happens to this one!\" entities=\"unparsed-1 unparsed-2\" entity=\"unparsed-1\" id=\"number99\" idref=\"number99\" idrefs=\"number99 number99 number99\" nmtoken=\"this-gets-normalized\" nmtokens=\"this also gets normalized\" notation=\"nonce\" token=\"a\"></attributes>",
  "A non-standalone document is valid if declared as such."
},
{
  "valid/not-sa03.xml",
  635,
  "<?xml version='1.0' standalone='no'?>\n\n<!DOCTYPE attributes SYSTEM \"sa.dtd\" [\n    <!--\n\tThis one is almost standalone since the values\n\tare pre-normalized in this document, and the\n\tdefaulted attribute is explicit.\n    \n\tBUT the entity refs are both external and need\n\tnormalization.\n    -->\n]>\n\n<attributes\n    token =\t\"b\"\n    notation =\t\"foo\"\n    nmtoken =\t\"this-gets-normalized\"\n    nmtokens =\t\"this also gets normalized\"\n    id =\t\"&internal;\"\n    idref =\t\"&internal;\"\n    idrefs =\t\"&internal; &internal; &internal;\"\n    entity =\t\"unparsed-1\"\n    entities =\t\"unparsed-1 unparsed-2\"\n    cdata =\t\"nothing happens to this one!\"\n    />\n",
  452,
  "<!DOCTYPE attributes [\n<!NOTATION bar SYSTEM 'file:/dev/tty'>\n<!NOTATION foo PUBLIC '-//public id//foo' 'file:/dev/null'>\n<!NOTATION nonce SYSTEM 'file:/dev/null'>\n]>\n<attributes cdata=\"nothing happens to this one!\" entities=\"unparsed-1 unparsed-2\" entity=\"unparsed-1\" id=\"internal42\" idref=\"internal42\" idrefs=\"internal42 internal42 internal42\" nmtoken=\"this-gets-normalized\" nmtokens=\"this also gets normalized\" notation=\"foo\" token=\"b\"></attributes>",
  "A non-standalone document is valid if declared as such."
},
{
  "valid/not-sa04.xml",
  716,
  "<?xml version='1.0' standalone='no'?>\n\n<!DOCTYPE attributes SYSTEM \"sa.dtd\" [\n    <!--\n\tThis one isn't standalone since it's got a defaulted\n\tattribute (token) and one needing normalization\n\t(notation).\n    -->\n\n    <!ATTLIST attributes\n\ttoken\t\t(a|b|c)\t\t\"a\"\n\tnotation\t(nonce|foo|bar)\t#IMPLIED\n\t>\n    <!ENTITY internal \"internal&number;\">\n    <!ENTITY number \"42\">\n]>\n\n<attributes\n    notation =\t\" nonce \"\n    nmtoken =\t\"this-gets-normalized\"\n    nmtokens =\t\"this also gets normalized\"\n    id =\t\"&internal;\"\n    idref =\t\"&internal;\"\n    idrefs =\t\"&internal; &internal; &internal;\"\n    entity =\t\"unparsed-1\"\n    entities =\t\"unparsed-1 unparsed-2\"\n    cdata =\t\"nothing happens to this one!\"\n    />\n\n<?pi equals three?>\n",
  473,
  "<!DOCTYPE attributes [\n<!NOTATION bar SYSTEM 'file:/dev/tty'>\n<!NOTATION foo PUBLIC '-//public id//foo' 'file:/dev/null'>\n<!NOTATION nonce SYSTEM 'file:/dev/null'>\n]>\n<attributes cdata=\"nothing happens to this one!\" entities=\"unparsed-1 unparsed-2\" entity=\"unparsed-1\" id=\"internal42\" idref=\"internal42\" idrefs=\"internal42 internal42 internal42\" nmtoken=\"this-gets-normalized\" nmtokens=\"this also gets normalized\" notation=\"nonce\" token=\"a\"></attributes><?pi equals three?>",
  "A non-standalone document is valid if declared as such."
},
{
  "valid/notation01.xml",
  143,
  "<?xml version=\"1.0\"?>\n<!DOCTYPE test SYSTEM \"notation01.dtd\" [\n    <!ENTITY applydsssl SYSTEM \"applydsssl.gif\" NDATA GIF>\n]>\n<test>test</test>\n",
  154,
  "<!DOCTYPE test [\n<!NOTATION GIF PUBLIC '+//ISBN 0-7923-9432-1::Graphic Notation//NOTATION CompuServe Graphic Interchange Format//EN'>\n]>\n<test>test</test>",
  "NOTATION declarations don't need SYSTEM IDs; and externally declared notations may be used to declare unparsed entities in the internal DTD subset. The notation must be reported to the application."
},
{
  "valid/optional.xml",
  1636,
  "<!DOCTYPE root SYSTEM \"dtdtest.dtd\">\n<root>\n    <!--\n\tThis primarily bangs on different ways of expressing\n\toptionality in content models.\n    -->\n    <once><e/></once>\n\n    <twice><e/><e/></twice>\n\n\n    <once-or-twice-a><e/></once-or-twice-a>\n    <once-or-twice-b><e/></once-or-twice-b>\n    <once-or-twice-c><e/></once-or-twice-c>\n    <once-or-twice-d><e/></once-or-twice-d>\n    <once-or-twice-e><e/></once-or-twice-e>\n\n    <once-or-twice-a><e/><e/></once-or-twice-a>\n    <once-or-twice-b><e/><e/></once-or-twice-b>\n    <once-or-twice-c><e/><e/></once-or-twice-c>\n    <once-or-twice-d><e/><e/></once-or-twice-d>\n    <once-or-twice-e><e/><e/></once-or-twice-e>\n\n\n    <once-or-more-a><e/></once-or-more-a>\n    <once-or-more-b><e/></once-or-more-b>\n    <once-or-more-c><e/></once-or-more-c>\n    <once-or-more-d><e/></once-or-more-d>\n    <once-or-more-e><e/></once-or-more-e>\n\n    <once-or-more-a><e/><e/></once-or-more-a>\n    <once-or-more-b><e/><e/></once-or-more-b>\n    <once-or-more-c><e/><e/></once-or-more-c>\n    <once-or-more-d><e/><e/></once-or-more-d>\n    <once-or-more-e><e/><e/></once-or-more-e>\n\n    <once-or-more-a><e/><e/><e/></once-or-more-a>\n    <once-or-more-b><e/><e/><e/></once-or-more-b>\n    <once-or-more-c><e/><e/><e/></once-or-more-c>\n    <once-or-more-d><e/><e/><e/></once-or-more-d>\n    <once-or-more-e><e/><e/><e/></once-or-more-e>\n\n    <once-or-more-a><e/><e/><e/><e/></once-or-more-a>\n    <once-or-more-b><e/><e/><e/><e/></once-or-more-b>\n    <once-or-more-c><e/><e/><e/><e/></once-or-more-c>\n    <once-or-more-d><e/><e/><e/><e/></once-or-more-d>\n    <once-or-more-e><e/><e/><e/><e/></once-or-more-e>\n\n\n</root>\n",
  1884,
  "<root>&#10;    &#10;    <once><e></e></once>&#10;&#10;    <twice><e></e><e></e></twice>&#10;&#10;&#10;    <once-or-twice-a><e></e></once-or-twice-a>&#10;    <once-or-twice-b><e></e></once-or-twice-b>&#10;    <once-or-twice-c><e></e></once-or-twice-c>&#10;    <once-or-twice-d><e></e></once-or-twice-d>&#10;    <once-or-twice-e><e></e></once-or-twice-e>&#10;&#10;    <once-or-twice-a><e></e><e></e></once-or-twice-a>&#10;    <once-or-twice-b><e></e><e></e></once-or-twice-b>&#10;    <once-or-twice-c><e></e><e></e></once-or-twice-c>&#10;    <once-or-twice-d><e></e><e></e></once-or-twice-d>&#10;    <once-or-twice-e><e></e><e></e></once-or-twice-e>&#10;&#10;&#10;    <once-or-more-a><e></e></once-or-more-a>&#10;    <once-or-more-b><e></e></once-or-more-b>&#10;    <once-or-more-c><e></e></once-or-more-c>&#10;    <once-or-more-d><e></e></once-or-more-d>&#10;    <once-or-more-e><e></e></once-or-more-e>&#10;&#10;    <once-or-more-a><e></e><e></e></once-or-more-a>&#10;    <once-or-more-b><e></e><e></e></once-or-more-b>&#10;    <once-or-more-c><e></e><e></e></once-or-more-c>&#10;    <once-or-more-d><e></e><e></e></once-or-more-d>&#10;    <once-or-more-e><e></e><e></e></once-or-more-e>&#10;&#10;    <once-or-more-a><e></e><e></e><e></e></once-or-more-a>&#10;    <once-or-more-b><e></e><e></e><e></e></once-or-more-b>&#10;    <once-or-more-c><e></e><e></e><e></e></once-or-more-c>&#10;    <once-or-more-d><e></e><e></e><e></e></once-or-more-d>&#10;    <once-or-more-e><e></e><e></e><e></e></once-or-more-e>&#10;&#10;    <once-or-more-a><e></e><e></e><e></e><e></e></once-or-more-a>&#10;    <once-or-more-b><e></e><e></e><e></e><e></e></once-or-more-b>&#10;    <once-or-more-c><e></e><e></e><e></e><e></e></once-or-more-c>&#10;    <once-or-more-d><e></e><e></e><e></e><e></e></once-or-more-d>&#10;    <once-or-more-e><e></e><e></e><e></e><e></e></once-or-more-e>&#10;&#10;&#10;</root>",
  "Tests declarations of \"children\" content models, and the validity constraints associated with them."
},
{
  "valid/required00.xml",
  108,
  "<!DOCTYPE root [\n    <!ELEMENT root EMPTY>\n    <!ATTLIST root\n\treq CDATA #REQUIRED\n\t>\n]>\n\n<root req=\"foo\"/>\n",
  23,
  "<root req=\"foo\"></root>",
  "Tests the #REQUIRED attribute declaration syntax, and the associated validity constraint."
},
{
  "valid/sa01.xml",
  258,
  "<?xml version='1.0' standalone='yes'?>\n\n<!DOCTYPE root [\n    <!ELEMENT root (child)*>\n    <!ELEMENT child (#PCDATA)>\n]>\n\n<root>\n    <child>\n    The whitespace around this element would be\n    invalid as standalone were the DTD external.\n    </child>\n</root>\n",
  156,
  "<root>&#10;    <child>&#10;    The whitespace around this element would be&#10;    invalid as standalone were the DTD external.&#10;    </child>&#10;</root>",
  "A document may be marked 'standalone' if any optional whitespace is defined within the internal DTD subset."
},
{
  "valid/sa02.xml",
  1310,
  "<?xml version='1.0' standalone='yes'?>\n\n<!DOCTYPE attributes [\n    <!ELEMENT attributes EMPTY>\n\n    <!--\n\t2.9 gives validity constraints applying to attributes\n\tin standalone docs:  no external defaults or decls\n\tcausing normalization.\n\n\t3.3.3 describes the normalization rules\n    -->\n\n    <!ATTLIST attributes\n\ttoken\t\t(a|b|c)\t\t\"a\"\n\tnotation\t(nonce|foo|bar)\t#IMPLIED\n\tnmtoken\t\tNMTOKEN\t\t#IMPLIED\n\tnmtokens\tNMTOKENS\t#IMPLIED\n\tid\t\tID\t\t#IMPLIED\n\tidref\t\tIDREF\t\t#IMPLIED\n\tidrefs\t\tIDREFS\t\t#IMPLIED\n\tentity\t\tENTITY\t\t#IMPLIED\n\tentities\tENTITIES\t#IMPLIED\n\tcdata\t\tCDATA\t\t#IMPLIED\n\t>\n    \n    <!ENTITY internal \" internal&number; \">\n    <!ENTITY number \"42\">\n\n    <!NOTATION nonce SYSTEM \"file:/dev/null\">\n    <!NOTATION foo PUBLIC \"-//public id//foo\" \"file:/dev/null\">\n    <!NOTATION bar SYSTEM \"file:/dev/tty\">\n\n    <!ENTITY unparsed-1 PUBLIC \"-//some public//ID\" \"file:/dev/console\"\n\t\t\tNDATA nonce>\n    <!ENTITY unparsed-2 SYSTEM \"scheme://host/data\"\n\t\t\tNDATA foo>\n]>\n\n<attributes\n    notation =\t\" nonce \"\n    nmtoken =\t\" this-gets-normalized \"\n    nmtokens =\t\" this\t\n also\t gets normalized \"\n    id =\t\"\t&internal; \"\n    idref =\t\" &internal;\n    \"\n    idrefs =\t\" &internal;  &internal;    &internal;\"\n    entity =\t\" unparsed-1 \"\n    entities =\t\"unparsed-1 unparsed-2\"\n    cdata =\t\"nothing happens to this one!\"\n    />\n",
  454,
  "<!DOCTYPE attributes [\n<!NOTATION bar SYSTEM 'file:/dev/tty'>\n<!NOTATION foo PUBLIC '-//public id//foo' 'file:/dev/null'>\n<!NOTATION nonce SYSTEM 'file:/dev/null'>\n]>\n<attributes cdata=\"nothing happens to this one!\" entities=\"unparsed-1 unparsed-2\" entity=\"unparsed-1\" id=\"internal42\" idref=\"internal42\" idrefs=\"internal42 internal42 internal42\" nmtoken=\"this-gets-normalized\" nmtokens=\"this also gets normalized\" notation=\"nonce\" token=\"a\"></attributes>",
  "A document may be marked 'standalone' if any attributes that need normalization are defined within the internal DTD subset."
},
{
  "valid/sa03.xml",
  765,
  "<?xml version='1.0' standalone='yes'?>\n\n<!DOCTYPE attributes SYSTEM \"sa.dtd\" [\n    <!--\n\tThis one is standalone since the values are\n\tpre-normalized in this document, and the\n\tdefaulted attribute is explicit.\n    \n\tSimilarly the entity refs are internal and\n\tdon't need normalization ... the unparsed\n\tentities (and entities) aren't \"references\"\n    -->\n    <!ENTITY internal \"internal&number;\">\n    <!ENTITY number \"42\">\n]>\n\n<attributes\n    token =\t\"b\"\n    notation =\t\"foo\"\n    nmtoken =\t\"this-gets-normalized\"\n    nmtokens =\t\"this also gets normalized\"\n    id =\t\"&internal;\"\n    idref =\t\"&internal;\"\n    idrefs =\t\"&internal; &internal; &internal;\"\n    entity =\t\"unparsed-1\"\n    entities =\t\"unparsed-1 unparsed-2\"\n    cdata =\t\"nothing happens to this one!\"\n    />\n",
  452,
  "<!DOCTYPE attributes [\n<!NOTATION bar SYSTEM 'file:/dev/tty'>\n<!NOTATION foo PUBLIC '-//public id//foo' 'file:/dev/null'>\n<!NOTATION nonce SYSTEM 'file:/dev/null'>\n]>\n<attributes cdata=\"nothing happens to this one!\" entities=\"unparsed-1 unparsed-2\" entity=\"unparsed-1\" id=\"internal42\" idref=\"internal42\" idrefs=\"internal42 internal42 internal42\" nmtoken=\"this-gets-normalized\" nmtokens=\"this also gets normalized\" notation=\"foo\" token=\"b\"></attributes>",
  "A document may be marked 'standalone' if any the defined entities need expanding are internal, and no attributes need defaulting or normalization. On output, requires notations to be correctly reported."
},
{
  "valid/sa04.xml",
  1050,
  "<?xml version='1.0' standalone='yes'?>\n\n<!DOCTYPE attributes SYSTEM \"sa.dtd\" [\n    <!--\n\tThis one is standalone since the values are\n\tpre-normalized in this document, except that\n\tone defaulted attribute is (re)defined internally\n\tand so is one normalized one.\n    \n        Similarly the entity refs are internal.  Unparsed\n        entities and notations are not listed among the\n        items that must not be externally declared in\n        standalone documents, even though processors must\n        in theory report their identifiers.\n\n    -->\n\n    <!ATTLIST attributes\n\ttoken\t\t(a|b|c)\t\t\"a\"\n\tnotation\t(nonce|foo|bar)\t#IMPLIED\n\t>\n    <!ENTITY internal \"internal&number;\">\n    <!ENTITY number \"42\">\n]>\n\n<attributes\n    notation =\t\" nonce \"\n    nmtoken =\t\"this-gets-normalized\"\n    nmtokens =\t\"this also gets normalized\"\n    id =\t\"&internal;\"\n    idref =\t\"&internal;\"\n    idrefs =\t\"&internal; &internal; &internal;\"\n    entity =\t\"unparsed-1\"\n    entities =\t\"unparsed-1 unparsed-2\"\n    cdata =\t\"nothing happens to this one!\"\n    />\n\n<?pi equals three?>\n",
  473,
  "<!DOCTYPE attributes [\n<!NOTATION bar SYSTEM 'file:/dev/tty'>\n<!NOTATION foo PUBLIC '-//public id//foo' 'file:/dev/null'>\n<!NOTATION nonce SYSTEM 'file:/dev/null'>\n]>\n<attributes cdata=\"nothing happens to this one!\" entities=\"unparsed-1 unparsed-2\" entity=\"unparsed-1\" id=\"internal42\" idref=\"internal42\" idrefs=\"internal42 internal42 internal42\" nmtoken=\"this-gets-normalized\" nmtokens=\"this also gets normalized\" notation=\"nonce\" token=\"a\"></attributes><?pi equals three?>",
  "Like sa03 but relies on attribute defaulting defined in the internal subset. On output, requires notations to be correctly reported."
},
{
  "valid/sa05.xml",
  162,
  "<?xml version='1.0' standalone='yes'?>\n\n<!DOCTYPE root SYSTEM \"sa.dtd\">\n\n<root><child>\n    No whitespace before or after this standalone element.\n</child></root>\n",
  257,
  "<!DOCTYPE root [\n<!NOTATION bar SYSTEM 'file:/dev/tty'>\n<!NOTATION foo PUBLIC '-//public id//foo' 'file:/dev/null'>\n<!NOTATION nonce SYSTEM 'file:/dev/null'>\n]>\n<root><child>&#10;    No whitespace before or after this standalone element.&#10;</child></root>",
  "Like sa01 but this document is standalone since it has no optional whitespace. On output, requires notations to be correctly reported."
},
{
  "valid/sgml01.xml",
  336,
  "<!DOCTYPE root [\n    <!ELEMENT root EMPTY>\n    <!--\n\tSGML dislikes token reuse.  It's legal XML, so any\n\tXML parser must accept it, though it's discouraged\n\tin documents \"for interoperability\"\n    -->\n    <!ATTLIST root\n\tstatus\t\t(initial-draft|revision|final) \"initial-draft\"\n\tposition\t(first|intermediate|final) \"first\"\n\t>\n]>\n\n<root/>\n",
  53,
  "<root position=\"first\" status=\"initial-draft\"></root>",
  "XML permits token reuse, while SGML does not."
},
{
  "valid/v-lang01.xml",
  104,
  "<!DOCTYPE root [\n<!ELEMENT root EMPTY>\n<!ATTLIST root xml:lang CDATA #IMPLIED>\n]>\n<root xml:lang=\"en\"/>\n",
  27,
  "<root xml:lang=\"en\"></root>",
  "Tests a lowercase ISO language code."
},
{
  "valid/v-lang02.xml",
  108,
  "<!DOCTYPE root [\n<!ELEMENT root EMPTY>\n<!ATTLIST root xml:lang CDATA #IMPLIED>\n]>\n<root xml:lang=\"en-IN\"/>\n\n",
  30,
  "<root xml:lang=\"en-IN\"></root>",
  "Tests a ISO language code with a subcode."
},
{
  "valid/v-lang03.xml",
  118,
  "<!DOCTYPE root [\n<!ELEMENT root EMPTY>\n<!ATTLIST root xml:lang CDATA #IMPLIED>\n]>\n<root xml:lang=\"i-klingon-whorf\"/>\n\n",
  40,
  "<root xml:lang=\"i-klingon-whorf\"></root>",
  "Tests a IANA language code with a subcode."
},
{
  "valid/v-lang04.xml",
  123,
  "<!DOCTYPE root [\n<!ELEMENT root EMPTY>\n<!ATTLIST root xml:lang CDATA #IMPLIED>\n]>\n<root xml:lang=\"x-dialect-valleygirl\"/>\n\n",
  45,
  "<root xml:lang=\"x-dialect-valleygirl\"></root>",
  "Tests a user language code with a subcode."
},
{
  "valid/v-lang05.xml",
  105,
  "<!DOCTYPE root [\n<!ELEMENT root EMPTY>\n<!ATTLIST root xml:lang CDATA #IMPLIED>\n]>\n<root xml:lang=\"DE\"/>\n\n",
  27,
  "<root xml:lang=\"DE\"></root>",
  "Tests an uppercase ISO language code."
},
{
  "valid/v-lang06.xml",
  109,
  "<!DOCTYPE root [\n<!ELEMENT root EMPTY>\n<!ATTLIST root xml:lang CDATA #IMPLIED>\n]>\n<root xml:lang=\"X-Java\"/>\n\n",
  31,
  "<root xml:lang=\"X-Java\"></root>",
  "Tests a user language code."
},
{
  "valid/pe00.xml",
  54,
  "<!DOCTYPE root SYSTEM \"pe00.dtd\">\n<root>&book;</root>\n",
  89,
  "<root>La Peste: Albert Camus,&#10;\302\251 1947 \303\211ditions Gallimard. All rights reserved</root>\211",
  "Tests construction of internal entity replacement text, using an example in the XML specification."
},
{
  "valid/pe03.xml",
  267,
  "<!DOCTYPE root [\n<!ELEMENT root (p)>\n<!ELEMENT p (#PCDATA)>\n<!-- Example 1 from XML spec 1.0 Appendix D -->\n<!ENTITY example \"<p>An ampersand (&#38;#38;) may be escaped\nnumerically (&#38;#38;#38) or with a general entity (&amp;amp;).</p>\" >\n]>\n<root>&example;</root>\n",
  120,
  "<root><p>An ampersand (&amp;) may be escaped&#10;numerically (&amp;#38) or with a general entity (&amp;amp;).</p></root>",
  "Tests construction of internal entity replacement text, using an example in the XML specification."
},
{
  "valid/pe02.xml",
  252,
  "<?xml version='1.0'?>\n<!DOCTYPE test [\n<!ELEMENT test (#PCDATA) >\n<!ENTITY % xx '&#37;zz;'>\n<!ENTITY % zz '&#60;!ENTITY tricky \"error-prone\" >' >\n%xx;\n]>\n<test>This sample shows a &tricky; method.</test>\n<!-- Example 2 from XML spec 1.0 Appendix D -->\n",
  52,
  "<test>This sample shows a error-prone method.</test>",
  "Tests construction of internal entity replacement text, using a complex example in the XML specification."
},
/* ==== from sun sun-not-wf.xml ==== */
{
  "not-wf/not-sa03.xml",
  194,
  "<?xml version='1.0' standalone='yes'?>\n\n<!DOCTYPE attributes SYSTEM \"../valid/sa.dtd\" [\n    <!--\n\tExternal entity ref\n    -->\n]>\n\n<attributes\n    token =\t\"b\"\n    id =\t\"external-&number;\"\n    />\n",
  0,
  NULL,
  "Tests the Entity Declared WFC, ensuring that a reference to externally defined entity causes a well-formedness error."
},
{
  "not-wf/attlist01.xml",
  148,
  "<!DOCTYPE root [\n    <!ELEMENT root EMPTY>\n\n    <!-- SGML-ism:  illegal attribute types -->\n\n    <!ATTLIST root\n\tnumber\tNUTOKEN\t\"1\"\n\t>\n\n]>\n\n<root/>\n",
  0,
  NULL,
  "SGML's NUTOKEN is not allowed."
},
{
  "not-wf/attlist02.xml",
  154,
  "<!DOCTYPE root [\n    <!ELEMENT root EMPTY>\n\n    <!-- SGML-ism:  illegal attribute types -->\n\n    <!ATTLIST root\n\tnumber\tNUTOKENS\t\"1 2 3\"\n\t>\n\n]>\n\n<root/>\n\n",
  0,
  NULL,
  "SGML's NUTOKENS attribute type is not allowed."
},
{
  "not-wf/attlist03.xml",
  149,
  "<!DOCTYPE root [\n    <!ELEMENT root EMPTY>\n\n    <!-- SGML-ism:  illegal attribute types -->\n\n    <!ATTLIST root\n\tchoice\t(a,b,c)\t\"a\"\n\t>\n\n]>\n\n<root/>\n\n",
  0,
  NULL,
  "Comma doesn't separate enumerations, unlike in SGML."
},
{
  "not-wf/attlist04.xml",
  148,
  "<!DOCTYPE root [\n    <!ELEMENT root EMPTY>\n\n    <!-- SGML-ism:  illegal attribute types -->\n\n    <!ATTLIST root\n\tnumber\tNUMBER\t\"1\"\n\t>\n\n]>\n\n<root/>\n\n",
  0,
  NULL,
  "SGML's NUMBER attribute type is not allowed."
},
{
  "not-wf/attlist05.xml",
  156,
  "<!DOCTYPE root [\n    <!ELEMENT root EMPTY>\n\n    <!-- SGML-ism:  illegal attribute types -->\n\n    <!ATTLIST root\n\tnumbers\tNUMBERS\t\"1 2 3 4\"\n\t>\n\n]>\n\n<root/>\n\n",
  0,
  NULL,
  "SGML's NUMBERS attribute type is not allowed."
},
{
  "not-wf/attlist06.xml",
  150,
  "<!DOCTYPE root [\n    <!ELEMENT root EMPTY>\n\n    <!-- SGML-ism:  illegal attribute types -->\n\n    <!ATTLIST root\n\tnumber\tNAME\t\"Elvis\"\n\t>\n\n]>\n\n<root/>\n\n",
  0,
  NULL,
  "SGML's NAME attribute type is not allowed."
},
{
  "not-wf/attlist07.xml",
  154,
  "<!DOCTYPE root [\n    <!ELEMENT root EMPTY>\n\n    <!-- SGML-ism:  illegal attribute types -->\n\n    <!ATTLIST root\n\tnumber\tNAMES\t\"The King\"\n\t>\n\n]>\n\n<root/>\n\n",
  0,
  NULL,
  "SGML's NAMES attribute type is not allowed."
},
{
  "not-wf/attlist08.xml",
  155,
  "<!DOCTYPE root [\n    <!ELEMENT root EMPTY>\n\n    <!-- SGML-ism:  illegal attribute default -->\n\n    <!ATTLIST root\n\tlanguage\tCDATA\t#CURRENT\n\t>\n\n]>\n\n<root/>\n",
  0,
  NULL,
  "SGML's #CURRENT is not allowed."
},
{
  "not-wf/attlist09.xml",
  145,
  "<!DOCTYPE root [\n    <!-- SGML-ism:  illegal attribute default -->\n\n    <!ATTLIST root\n\tlanguage\tCDATA\t#CONREF\n\t>\n\n]>\n\n<root language=\"Dutch\"/>\n\n",
  0,
  NULL,
  "SGML's #CONREF is not allowed."
},
{
  "not-wf/attlist10.xml",
  206,
  "<!DOCTYPE root [\n<!ELEMENT root ANY>\n<!ATTLIST root att1 CDATA #IMPLIED>\n<!ATTLIST root att2 CDATA #IMPLIED>\n]>\n<root att1=\"value1\"att2=\"value2\">\n    <!-- whitespace required between attributes -->\n</root>\n",
  0,
  NULL,
  "Whitespace required between attributes"
},
{
  "not-wf/attlist11.xml",
  199,
  "<!DOCTYPE root [\n<!ELEMENT root ANY>\n<!ATTLIST root att1 CDATA #IMPLIED>\n<!ATTLIST root att2 CDATA #IMPLIED>\n]>\n<root att1=\"value1\"att2=\"value2\"/>\n    <!-- whitespace required between attributes -->\n",
  0,
  NULL,
  "Whitespace required between attributes"
},
{
  "not-wf/cond01.xml",
  77,
  "<!DOCTYPE root SYSTEM \"cond.dtd\" [\n    <!ENTITY % MAYBE \"CDATA\">\n]>\n\n<root/>\n",
  0,
  NULL,
  "Only INCLUDE and IGNORE are conditional section keywords"
},
{
  "not-wf/cond02.xml",
  73,
  "<!DOCTYPE root SYSTEM \"cond.dtd\" [\n    <!ENTITY % MAYBE \"\">\n]>\n\n<root/>\n\n",
  0,
  NULL,
  "Must have keyword in conditional sections"
},
{
  "not-wf/content01.xml",
  107,
  "<!DOCTYPE root [\n    <!-- no whitespace before '?', '*', '+' -->\n    <!ELEMENT root ((root) ?)>\n]>\n<root/>\n",
  0,
  NULL,
  "No whitespace before \"?\" in content model"
},
{
  "not-wf/content02.xml",
  108,
  "<!DOCTYPE root [\n    <!-- no whitespace before '?', '*', '+' -->\n    <!ELEMENT root ((root) *)>\n]>\n<root/>\n\n",
  0,
  NULL,
  "No whitespace before \"*\" in content model"
},
{
  "not-wf/content03.xml",
  106,
  "<!DOCTYPE root [\n    <!-- no whitespace before '?', '*', '+' -->\n    <!ELEMENT root (root +)>\n]>\n<root/>\n\n",
  0,
  NULL,
  "No whitespace before \"+\" in content model"
},
{
  "not-wf/decl01.xml",
  150,
  "<!DOCTYPE root [\n    <!ELEMENT root EMPTY>\n    <!ENTITY % ent01 SYSTEM \"decl01.ent\">\n\n    <!-- the entity is an illegal PE -->\n    %ent01;\n]>\n<root/>\n",
  0,
  NULL,
  "External entities may not have standalone decls."
},
{
  "not-wf/dtd00.xml",
  160,
  "<!DOCTYPE root [\n    <!ELEMENT root (foo, bar? foo)>\n\t<!-- comma omitted -->\n    <!ELEMENT foo EMPTY>\n    <!ELEMENT bar EMPTY>\n]>\n\n<root> <foo/> <foo/> </root>\n",
  0,
  NULL,
  "Comma mandatory in content model"
},
{
  "not-wf/dtd01.xml",
  179,
  "<!DOCTYPE root [\n    <!ELEMENT root (foo, bar? | foo)>\n\t<!-- comma swapped for vertical bar -->\n    <!ELEMENT foo EMPTY>\n    <!ELEMENT bar EMPTY>\n]>\n\n<root> <foo/> <foo/> </root>\n",
  0,
  NULL,
  "Can't mix comma and vertical bar in content models"
},
{
  "not-wf/dtd02.xml",
  139,
  "<!DOCTYPE root [\n    <!ELEMENT root EMPTY>\n    <!-- correct PE ref syntax -->\n    <!ENTITY % foo \"<!ATTLIST root>\">\n    % foo;\n]>\n\n<root/>\n",
  0,
  NULL,
  "PE name immediately after \"%\""
},
{
  "not-wf/dtd03.xml",
  143,
  "<!DOCTYPE root [\n    <!ELEMENT root EMPTY>\n    <!-- correct PE ref syntax -->\n    <!ENTITY % foo \"<!ATTLIST root>\">\n    %foo\n    ;\n]>\n\n<root/>\n",
  0,
  NULL,
  "PE name immediately followed by \";\""
},
{
  "not-wf/dtd04.xml",
  158,
  "<!DOCTYPE root [\n    <!ELEMENT root EMPTY>\n    <!-- PUBLIC id must be quoted -->\n    <!ENTITY foo PUBLIC -//BadCorp//DTD-foo-1.0//EN \"elvis.ent\">\n]>\n\n<root/>\n",
  0,
  NULL,
  "PUBLIC literal must be quoted"
},
{
  "not-wf/dtd05.xml",
  128,
  "<!DOCTYPE root [\n    <!ELEMENT root EMPTY>\n    <!-- SYSTEM id must be quoted -->\n    <!ENTITY foo SYSTEM elvis.ent>\n]>\n\n<root/>\n",
  0,
  NULL,
  "SYSTEM identifier must be quoted"
},
{
  "not-wf/dtd07.xml",
  73,
  "<!DOCTYPE root SYSTEM \"dtd07.dtd\" [\n    <!ELEMENT root EMPTY>\n]>\n<root/>\n",
  0,
  NULL,
  "Text declarations (which optionally begin any external entity) are required to have \"encoding=...\"."
},
{
  "not-wf/element00.xml",
  35,
  "<root>\n    Incomplete end tag.\n</ro",
  0,
  NULL,
  "EOF in middle of incomplete ETAG"
},
{
  "not-wf/element01.xml",
  37,
  "<root>\n    Incomplete end tag.\n</root",
  0,
  NULL,
  "EOF in middle of incomplete ETAG"
},
{
  "not-wf/element02.xml",
  87,
  "<!DOCTYPE html [ <!ELEMENT html ANY> ]>\n<html>\n    <% @ LANGUAGE=\"VBSCRIPT\" %>\n</html>\n",
  0,
  NULL,
  "Illegal markup (<%@ ... %>)"
},
{
  "not-wf/element03.xml",
  101,
  "<!DOCTYPE html [ <!ELEMENT html ANY> ]>\n<html>\n    <% document.println (\"hello, world\"); %>\n</html>\n\n",
  0,
  NULL,
  "Illegal markup (<% ... %>)"
},
{
  "not-wf/element04.xml",
  80,
  "<!DOCTYPE root [ <!ELEMENT root ANY> ]>\n<root>\n    <!ELEMENT foo EMPTY>\n</root>\n",
  0,
  NULL,
  "Illegal markup (<!ELEMENT ... >)"
},
{
  "not-wf/encoding01.xml",
  48,
  "<?xml version=\"1.0\" encoding=\" utf-8\"?>\n<root/>\n",
  0,
  NULL,
  "Illegal character \" \" in encoding name"
},
{
  "not-wf/encoding02.xml",
  46,
  "<?xml version=\"1.0\" encoding=\"a/b\"?>\n<root/>\n\n",
  0,
  NULL,
  "Illegal character \"/\" in encoding name"
},
{
  "not-wf/encoding03.xml",
  56,
  "<?xml version=\"1.0\" encoding=\"just&#41;word\"?>\n<root/>\n\n",
  0,
  NULL,
  "Illegal character reference in encoding name"
},
{
  "not-wf/encoding04.xml",
  48,
  "<?xml version=\"1.0\" encoding=\"utf:8\"?>\n<root/>\n\n",
  0,
  NULL,
  "Illegal character \":\" in encoding name"
},
{
  "not-wf/encoding05.xml",
  64,
  "<?xml version=\"1.0\" encoding=\"@import(sys-encoding)\"?>\n<root/>\n\n",
  0,
  NULL,
  "Illegal character \"@\" in encoding name"
},
{
  "not-wf/encoding06.xml",
  114,
  "<?xml version=\"1.0\" encoding=\"XYZ+999\"?>\n\n<!-- WF ... but illegal encoding name, also a fatal error --> \n\n<root/>\n",
  0,
  NULL,
  "Illegal character \"+\" in encoding name"
},
{
  "not-wf/encoding07.xml",
  224,
  "<!DOCTYPE root [\n    <!ELEMENT root EMPTY>\n\n    <!--\n\treusing this entity; it's got no markup decls,\n\tso it's legal except for a missing \"encoding=...\".\n    -->\n    <!ENTITY empty SYSTEM \"dtd07.dtd\">\n]>\n<root>&empty;</root>\n",
  0,
  NULL,
  "Text declarations (which optionally begin any external entity) are required to have \"encoding=...\"."
},
{
  "not-wf/pi.xml",
  99,
  "<!DOCTYPE root [\n<!ELEMENT root EMPTY>\n<!-- space before PI data and ?> -->\n<?bad-pi+?>\n]>\n<root/>\n",
  0,
  NULL,
  "No space between PI target name and data"
},
{
  "not-wf/pubid01.xml",
  158,
  "<!DOCTYPE root [\n    <!ELEMENT root EMPTY>\n\n    <!-- illegal public ID characters -->\n\n    <!ENTITY e PUBLIC \"this isn&apos;t allowed\" \"ignored\">\n]>\n\n<root/>\n",
  0,
  NULL,
  "Illegal entity ref in public ID"
},
{
  "not-wf/pubid02.xml",
  145,
  "<!DOCTYPE root [\n    <!ELEMENT root EMPTY>\n\n    <!-- illegal public ID characters -->\n\n    <!ENTITY e PUBLIC \"<illegal>\" \"ignored\">\n]>\n\n<root/>\n\n",
  0,
  NULL,
  "Illegal characters in public ID"
},
{
  "not-wf/pubid03.xml",
  145,
  "<!DOCTYPE root [\n    <!ELEMENT root EMPTY>\n\n    <!-- illegal public ID characters -->\n\n    <!ENTITY e PUBLIC \"[illegal]\" \"ignored\">\n]>\n\n<root/>\n\n",
  0,
  NULL,
  "Illegal characters in public ID"
},
{
  "not-wf/pubid04.xml",
  147,
  "<!DOCTYPE root [\n    <!ELEMENT root EMPTY>\n\n    <!-- illegal public ID characters -->\n\n    <!ENTITY e PUBLIC \"{ illegal }\" \"ignored\">\n]>\n\n<root/>\n\n",
  0,
  NULL,
  "Illegal characters in public ID"
},
{
  "not-wf/pubid05.xml",
  127,
  "<!DOCTYPE root [\n\n    <!-- SGML-ism: publid ID without system ID -->\n\n    <!ENTITY e PUBLIC \"this is not allowed\">\n]>\n\n<root/>\n",
  0,
  NULL,
  "SGML-ism: public ID without system ID"
},
{
  "not-wf/sgml01.xml",
  95,
  "<!DOCTYPE root [\n    <!ELEMENT root EMPTY>\n\n    <!-- SGML-ism:  omitted end tag -->\n]>\n\n<root>\n",
  0,
  NULL,
  "SGML-ism: omitted end tag for EMPTY content"
},
{
  "not-wf/sgml02.xml",
  121,
  " <?xml version=\"1.0\"?>\n    <!-- SGML-ism:  XML PI not at beginning -->\n<!DOCTYPE root [ <!ELEMENT root EMPTY> ]>\n<root/>\n",
  0,
  NULL,
  "XML declaration must be at the very beginning of a document; it\"s not a processing instruction"
},
{
  "not-wf/sgml03.xml",
  93,
  "<!DOCTYPE root [ <!ELEMENT root EMPTY> ]>\n\n    <!-- SGML-ism:  -- inside comment -->\n<root/>\n",
  0,
  NULL,
  "Comments may not contain \"--\""
},
{
  "not-wf/sgml04.xml",
  189,
  "<!DOCTYPE root [\n    <!-- SGML-ism:  multiple attlist types -->\n\n    <!ELEMENT root EMPTY>\n    <!ELEMENT branch EMPTY>\n\n    <!ATTLIST (root|branch)\n\tTreeType CDATA #REQUIRED\n\t>\n]>\n\n<root/>\n",
  0,
  NULL,
  "ATTLIST declarations apply to only one element, unlike SGML"
},
{
  "not-wf/sgml05.xml",
  209,
  "<!DOCTYPE root [\n    <!-- SGML-ism:  multiple element types -->\n\n    <!ELEMENT root EMPTY>\n    <!ELEMENT leaves EMPTY>\n    <!ELEMENT branch EMPTY>\n\n    <!ELEMENT (bush|tree) (root,leaves,branch)>\n]>\n\n<root/>\n\n",
  0,
  NULL,
  "ELEMENT declarations apply to only one element, unlike SGML"
},
{
  "not-wf/sgml06.xml",
  154,
  "<!DOCTYPE root [\n    <!-- Web-SGML-ism:  global attlist types -->\n\n    <!ELEMENT root EMPTY>\n\n    <!ATTLIST #ALL\n\tTreeType CDATA #REQUIRED\n\t>\n]>\n\n<root/>\n",
  0,
  NULL,
  "ATTLIST declarations are never global, unlike in SGML"
},
{
  "not-wf/sgml07.xml",
  112,
  "<!DOCTYPE root [\n    <!-- SGML-ism:  omitted tag minimzation spec -->\n    <!ELEMENT root - o EMPTY>\n]>\n\n<root/>\n",
  0,
  NULL,
  "SGML Tag minimization specifications are not allowed"
},
{
  "not-wf/sgml08.xml",
  113,
  "<!DOCTYPE root [\n    <!-- SGML-ism:  omitted tag minimzation spec -->\n    <!ELEMENT root - - EMPTY>\n]>\n\n<root/>\n\n",
  0,
  NULL,
  "SGML Tag minimization specifications are not allowed"
},
{
  "not-wf/sgml09.xml",
  112,
  "<!DOCTYPE root [\n    <!-- SGML-ism:  exception spec -->\n\n    <!ELEMENT footnote (para*) -footnote>\n]>\n\n<root/>\n\n",
  0,
  NULL,
  "SGML Content model exception specifications are not allowed"
},
{
  "not-wf/sgml10.xml",
  136,
  "<!DOCTYPE root [\n    <!-- SGML-ism:  exception spec -->\n    <!ELEMENT section (header,(para|section))* +(annotation|todo)>\n]>\n\n<root/>\n\n",
  0,
  NULL,
  "SGML Content model exception specifications are not allowed"
},
{
  "not-wf/sgml11.xml",
  99,
  "<!DOCTYPE root [\n    <!-- SGML-ism:  CDATA content type -->\n    <!ELEMENT ROOT CDATA>\n]>\n\n<root/>\n\n",
  0,
  NULL,
  "CDATA is not a valid content model spec"
},
{
  "not-wf/sgml12.xml",
  102,
  "<!DOCTYPE root [\n    <!-- SGML-ism:  RCDATA content type -->\n    <!ELEMENT ROOT RCDATA>\n]>\n\n<root/>\n\n\n",
  0,
  NULL,
  "RCDATA is not a valid content model spec"
},
{
  "not-wf/sgml13.xml",
  197,
  "<!DOCTYPE root [\n    <!-- SGML-ism:  unordered content type -->\n    <!ELEMENT ROOT (a & b & c)>\n    <!ELEMENT a EMPTY>\n    <!ELEMENT b EMPTY>\n    <!ELEMENT c EMPTY>\n]>\n\n<root><b/><c/><a/></root>\n\n\n",
  0,
  NULL,
  "SGML Unordered content models not allowed"
},
/* ==== from sun sun-error.xml ==== */
{
  "not-wf/uri01.xml",
  115,
  "<!DOCTYPE root [\n<!ELEMENT root EMPTY>\n<!-- URI fragments disallowed -->\n<!ENTITY foo SYSTEM \"foo#bar\">\n]>\n<root/>\n",
  0,
  NULL,
  "SYSTEM ids may not have URI fragments"
},
/* ==== from xmltest xmltest.xml ==== */
{
  "not-wf/sa/001.xml",
  32,
  "<doc>\r\n<doc\r\n?\r\n<a</a>\r\n</doc>\r\n",
  0,
  NULL,
  "Attribute values must start with attribute names, not \"?\"."
},
{
  "not-wf/sa/002.xml",
  32,
  "<doc>\r\n<.doc></.doc>\r\n</doc>\r\n\r\n",
  0,
  NULL,
  "Names may not start with \".\"; it's not a Letter."
},
{
  "not-wf/sa/003.xml",
  18,
  "<doc><? ?></doc>\r\n",
  0,
  NULL,
  "Processing Instruction target name is required."
},
{
  "not-wf/sa/004.xml",
  32,
  "<doc><?target some data></doc>\r\n",
  0,
  NULL,
  "SGML-ism: processing instructions end in '?>' not '>'."
},
{
  "not-wf/sa/005.xml",
  32,
  "<doc><?target some data?</doc>\r\n",
  0,
  NULL,
  "Processing instructions end in '?>' not '?'."
},
{
  "not-wf/sa/006.xml",
  42,
  "<doc><!-- a comment -- another --></doc>\r\n",
  0,
  NULL,
  "XML comments may not contain \"--\""
},
{
  "not-wf/sa/007.xml",
  25,
  "<doc>&amp no refc</doc>\r\n",
  0,
  NULL,
  "General entity references have no whitespace after the entity name and before the semicolon."
},
{
  "not-wf/sa/008.xml",
  22,
  "<doc>&.entity;</doc>\r\n",
  0,
  NULL,
  "Entity references must include names, which don't begin with '.' (it's not a Letter or other name start character)."
},
{
  "not-wf/sa/009.xml",
  18,
  "<doc>&#RE;</doc>\r\n",
  0,
  NULL,
  "Character references may have only decimal or numeric strings."
},
{
  "not-wf/sa/010.xml",
  18,
  "<doc>A & B</doc>\r\n",
  0,
  NULL,
  "Ampersand may only appear as part of a general entity reference."
},
{
  "not-wf/sa/011.xml",
  16,
  "<doc a1></doc>\r\n",
  0,
  NULL,
  "SGML-ism: attribute values must be explicitly assigned a value, it can't act as a boolean toggle."
},
{
  "not-wf/sa/012.xml",
  19,
  "<doc a1=v1></doc>\r\n",
  0,
  NULL,
  "SGML-ism: attribute values must be quoted in all cases."
},
{
  "not-wf/sa/013.xml",
  21,
  "<doc a1=\"v1'></doc>\r\n",
  0,
  NULL,
  "The quotes on both ends of an attribute value must match."
},
{
  "not-wf/sa/014.xml",
  24,
  "<doc a1=\"<foo>\"></doc>\r\n",
  0,
  NULL,
  "Attribute values may not contain literal '<' characters."
},
{
  "not-wf/sa/015.xml",
  17,
  "<doc a1=></doc>\r\n",
  0,
  NULL,
  "Attribute values need a value, not just an equals sign."
},
{
  "not-wf/sa/016.xml",
  26,
  "<doc a1=\"v1\" \"v2\"></doc>\r\n",
  0,
  NULL,
  "Attribute values need an associated name."
},
{
  "not-wf/sa/017.xml",
  22,
  "<doc><![CDATA[</doc>\r\n",
  0,
  NULL,
  "CDATA sections need a terminating ']]>'."
},
{
  "not-wf/sa/018.xml",
  32,
  "<doc><![CDATA [ stuff]]></doc>\r\n",
  0,
  NULL,
  "CDATA sections begin with a literal '<![CDATA[', no space."
},
{
  "not-wf/sa/019.xml",
  10,
  "<doc></>\r\n",
  0,
  NULL,
  "End tags may not be abbreviated as '</>'."
},
{
  "not-wf/sa/020.xml",
  24,
  "<doc a1=\"A & B\"></doc>\r\n",
  0,
  NULL,
  "Attribute values may not contain literal '&' characters except as part of an entity reference."
},
{
  "not-wf/sa/021.xml",
  22,
  "<doc a1=\"a&b\"></doc>\r\n",
  0,
  NULL,
  "Attribute values may not contain literal '&' characters except as part of an entity reference."
},
{
  "not-wf/sa/022.xml",
  25,
  "<doc a1=\"&#123:\"></doc>\r\n",
  0,
  NULL,
  "Character references end with semicolons, always!"
},
{
  "not-wf/sa/023.xml",
  21,
  "<doc 12=\"34\"></doc>\r\n",
  0,
  NULL,
  "Digits are not valid name start characters."
},
{
  "not-wf/sa/024.xml",
  28,
  "<doc>\r\n<123></123>\r\n</doc>\r\n",
  0,
  NULL,
  "Digits are not valid name start characters."
},
{
  "not-wf/sa/025.xml",
  16,
  "<doc>]]></doc>\r\n",
  0,
  NULL,
  "Text may not contain a literal ']]>' sequence."
},
{
  "not-wf/sa/026.xml",
  17,
  "<doc>]]]></doc>\r\n",
  0,
  NULL,
  "Text may not contain a literal ']]>' sequence."
},
{
  "not-wf/sa/027.xml",
  25,
  "<doc>\r\n<!-- abc\r\n</doc>\r\n",
  0,
  NULL,
  "Comments must be terminated with \"-->\"."
},
{
  "not-wf/sa/028.xml",
  44,
  "<doc>\r\n<?a pi that is not closed\r\n</doc>\r\n\r\n",
  0,
  NULL,
  "Processing instructions must end with '?>'."
},
{
  "not-wf/sa/029.xml",
  23,
  "<doc>abc]]]>def</doc>\r\n",
  0,
  NULL,
  "Text may not contain a literal ']]>' sequence."
},
{
  "not-wf/sa/030.xml",
  49,
  "<doc>A form feed (\f) is not legal in data</doc>\r\n",
  0,
  NULL,
  "A form feed is not a legal XML character."
},
{
  "not-wf/sa/031.xml",
  58,
  "<doc><?pi a form feed (\f) is not allowed in a pi?></doc>\r\n",
  0,
  NULL,
  "A form feed is not a legal XML character."
},
{
  "not-wf/sa/032.xml",
  65,
  "<doc><!-- a form feed (\f) is not allowed in a comment --></doc>\r\n",
  0,
  NULL,
  "A form feed is not a legal XML character."
},
{
  "not-wf/sa/033.xml",
  20,
  "<doc>abc\33def</doc>\r\n\33",
  0,
  NULL,
  "An ESC (octal 033) is not a legal XML character."
},
{
  "not-wf/sa/034.xml",
  65,
  "<doc\f>A form-feed is not white space or a name character</doc\f>\r\n",
  0,
  NULL,
  "A form feed is not a legal XML character."
},
{
  "not-wf/sa/035.xml",
  33,
  "<doc>1 < 2 but not in XML</doc>\r\n",
  0,
  NULL,
  "The '<' character is a markup delimiter and must start an element, CDATA section, PI, or comment."
},
{
  "not-wf/sa/036.xml",
  27,
  "<doc></doc>\r\nIllegal data\r\n",
  0,
  NULL,
  "Text may not appear after the root element."
},
{
  "not-wf/sa/037.xml",
  20,
  "<doc></doc>\r\n&#32;\r\n",
  0,
  NULL,
  "Character references may not appear after the root element."
},
{
  "not-wf/sa/038.xml",
  37,
  "<doc x=\"foo\" y=\"bar\" x=\"baz\"></doc>\r\n",
  0,
  NULL,
  "Tests the \"Unique Att Spec\" WF constraint by providing multiple values for an attribute."
},
{
  "not-wf/sa/039.xml",
  21,
  "<doc><a></aa></doc>\r\n",
  0,
  NULL,
  "Tests the Element Type Match WFC - end tag name must match start tag name."
},
{
  "not-wf/sa/040.xml",
  26,
  "<doc></doc>\r\n<doc></doc>\r\n",
  0,
  NULL,
  "Provides two document elements."
},
{
  "not-wf/sa/041.xml",
  21,
  "<doc/>\r\n<doc></doc>\r\n",
  0,
  NULL,
  "Provides two document elements."
},
{
  "not-wf/sa/042.xml",
  15,
  "<doc/></doc/>\r\n",
  0,
  NULL,
  "Invalid End Tag"
},
{
  "not-wf/sa/043.xml",
  22,
  "<doc/>\r\nIllegal data\r\n",
  0,
  NULL,
  "Provides #PCDATA text after the document element."
},
{
  "not-wf/sa/044.xml",
  14,
  "<doc/><doc/>\r\n",
  0,
  NULL,
  "Provides two document elements."
},
{
  "not-wf/sa/045.xml",
  22,
  "<doc>\r\n<a/\r\n</doc>\r\n\r\n",
  0,
  NULL,
  "Invalid Empty Element Tag"
},
{
  "not-wf/sa/046.xml",
  24,
  "<doc>\r\n<a/</a>\r\n</doc>\r\n",
  0,
  NULL,
  "This start (or empty element) tag was not terminated correctly."
},
{
  "not-wf/sa/047.xml",
  23,
  "<doc>\r\n<a / >\r\n</doc>\r\n",
  0,
  NULL,
  "Invalid empty element tag invalid whitespace"
},
{
  "not-wf/sa/048.xml",
  29,
  "<doc>\r\n</doc>\r\n<![CDATA[]]>\r\n",
  0,
  NULL,
  "Provides a CDATA section after the root element."
},
{
  "not-wf/sa/049.xml",
  58,
  "<doc>\r\n<a><![CDATA[xyz]]]></a>\r\n<![CDATA[]]></a>\r\n</doc>\r\n",
  0,
  NULL,
  "Missing start tag"
},
{
  "not-wf/sa/050.xml",
  0,
  "",
  0,
  NULL,
  "Empty document, with no root element."
},
{
  "not-wf/sa/051.xml",
  47,
  "<!-- a comment -->\r\n<![CDATA[]]>\r\n<doc></doc>\r\n",
  0,
  NULL,
  "CDATA is invalid at top level of document."
},
{
  "not-wf/sa/052.xml",
  40,
  "<!-- a comment -->\r\n&#32;\r\n<doc></doc>\r\n",
  0,
  NULL,
  "Invalid character reference."
},
{
  "not-wf/sa/053.xml",
  13,
  "<doc></DOC>\r\n",
  0,
  NULL,
  "End tag does not match start tag."
},
{
  "not-wf/sa/054.xml",
  73,
  "<!DOCTYPE doc [\r\n<!ENTITY foo PUBLIC \"some public id\">\r\n]>\r\n<doc></doc>\r\n",
  0,
  NULL,
  "PUBLIC requires two literals."
},
{
  "not-wf/sa/055.xml",
  30,
  "<!DOCTYPE doc [\r\n<doc></doc>\r\n",
  0,
  NULL,
  "Invalid Document Type Definition format."
},
{
  "not-wf/sa/056.xml",
  48,
  "<!DOCTYPE doc -- a comment -- []>\r\n<doc></doc>\r\n",
  0,
  NULL,
  "Invalid Document Type Definition format - misplaced comment."
},
{
  "not-wf/sa/057.xml",
  74,
  "<!DOCTYPE doc [\r\n<!ENTITY e \"whatever\" -- a comment -->\r\n]>\r\n<doc></doc>\r\n",
  0,
  NULL,
  "This isn't SGML; comments can't exist in declarations."
},
{
  "not-wf/sa/058.xml",
  98,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n<!ATTLIST doc a1 (foo,bar) #IMPLIED>\r\n]>\r\n<doc></doc>\r\n",
  0,
  NULL,
  "Invalid character , in ATTLIST enumeration"
},
{
  "not-wf/sa/059.xml",
  90,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n<!ATTLIST doc a1 NMTOKEN v1>\r\n]>\r\n<doc></doc>\r\n",
  0,
  NULL,
  "String literal must be in quotes."
},
{
  "not-wf/sa/060.xml",
  93,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n<!ATTLIST doc a1 NAME #IMPLIED>\r\n]>\r\n<doc></doc>\r\n",
  0,
  NULL,
  "Invalid type NAME defined in ATTLIST."
},
{
  "not-wf/sa/061.xml",
  72,
  "<!DOCTYPE doc [\r\n<!ENTITY e PUBLIC \"whatever\"\"e.ent\">\r\n]>\r\n<doc></doc>\r\n",
  0,
  NULL,
  "External entity declarations require whitespace between public and system IDs."
},
{
  "not-wf/sa/062.xml",
  60,
  "<!DOCTYPE doc [\r\n<!ENTITY foo\"some text\">\r\n]>\r\n<doc></doc>\r\n",
  0,
  NULL,
  "Entity declarations need space after the entity name."
},
{
  "not-wf/sa/063.xml",
  51,
  "<!DOCTYPE doc [\r\n<![INCLUDE[ ]]>\r\n]>\r\n<doc></doc>\r\n",
  0,
  NULL,
  "Conditional sections may only appear in the external DTD subset."
},
{
  "not-wf/sa/064.xml",
  88,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n<!ATTLIST e a1 CDATA\"foo\">\r\n]>\r\n<doc></doc>\r\n",
  0,
  NULL,
  "Space is required between attribute type and default values in <!ATTLIST...> declarations."
},
{
  "not-wf/sa/065.xml",
  97,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n<!ATTLIST doc a1(foo|bar) #IMPLIED>\r\n]>\r\n<doc></doc>\r\n",
  0,
  NULL,
  "Space is required between attribute name and type in <!ATTLIST...> declarations."
},
{
  "not-wf/sa/066.xml",
  97,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n<!ATTLIST doc a1 (foo|bar)#IMPLIED>\r\n]>\r\n<doc></doc>\r\n",
  0,
  NULL,
  "Required whitespace is missing."
},
{
  "not-wf/sa/067.xml",
  90,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n<!ATTLIST doc a1 (foo)\"foo\">\r\n]>\r\n<doc></doc>\r\n",
  0,
  NULL,
  "Space is required between attribute type and default values in <!ATTLIST...> declarations."
},
{
  "not-wf/sa/068.xml",
  102,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n<!ATTLIST doc a1 NOTATION(foo) #IMPLIED>\r\n]>\r\n<doc></doc>\r\n",
  0,
  NULL,
  "Space is required between NOTATION keyword and list of enumerated choices in <!ATTLIST...> declarations."
},
{
  "not-wf/sa/069.xml",
  146,
  "<!DOCTYPE doc [\r\n<!NOTATION eps SYSTEM \"eps.exe\">\r\n<!-- missing space before NDATA -->\r\n<!ENTITY foo SYSTEM \"foo.eps\"NDATA eps>\r\n]>\r\n<doc></doc>\r\n",
  0,
  NULL,
  "Space is required before an NDATA entity annotation."
},
{
  "not-wf/sa/070.xml",
  59,
  "<!-- a comment ending with three dashes --->\r\n<doc></doc>\r\n",
  0,
  NULL,
  "XML comments may not contain \"--\""
},
{
  "not-wf/sa/071.xml",
  101,
  "<!DOCTYPE doc [\r\n<!ENTITY e1 \"&e2;\">\r\n<!ENTITY e2 \"&e3;\">\r\n<!ENTITY e3 \"&e1;\">\r\n]>\r\n<doc>&e1;</doc>\r\n",
  0,
  NULL,
  "ENTITY can't reference itself directly or indirectly."
},
{
  "not-wf/sa/072.xml",
  18,
  "<doc>&foo;</doc>\r\n",
  0,
  NULL,
  "Undefined ENTITY foo."
},
{
  "not-wf/sa/073.xml",
  61,
  "<!DOCTYPE doc [\r\n<!ENTITY e \"whatever\">\r\n]>\r\n<doc>&f;</doc>\r\n",
  0,
  NULL,
  "Undefined ENTITY f."
},
{
  "not-wf/sa/074.xml",
  79,
  "<!DOCTYPE doc [\r\n<!ENTITY e \"</foo><foo>\">\r\n]>\r\n<doc>\r\n<foo>&e;</foo>\r\n</doc>\r\n",
  0,
  NULL,
  "Internal general parsed entities are only well formed if they match the \"content\" production."
},
{
  "not-wf/sa/075.xml",
  108,
  "<!DOCTYPE doc [\r\n<!ENTITY e1 \"&e2;\">\r\n<!ENTITY e2 \"&e3;\">\r\n<!ENTITY e3 \"&e1;\">\r\n]>\r\n<doc a=\"&e1;\"></doc>\r\n\r\n",
  0,
  NULL,
  "ENTITY can't reference itself directly or indirectly."
},
{
  "not-wf/sa/076.xml",
  23,
  "<doc a=\"&foo;\"></doc>\r\n",
  0,
  NULL,
  "Undefined ENTITY foo."
},
{
  "not-wf/sa/077.xml",
  67,
  "<!DOCTYPE doc [\r\n<!ENTITY foo \"&bar;\">\r\n]>\r\n<doc a=\"&foo;\"></doc>\r\n",
  0,
  NULL,
  "Undefined ENTITY bar."
},
{
  "not-wf/sa/078.xml",
  92,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n<!ATTLIST doc a CDATA \"&foo;\">\r\n]>\r\n<doc></doc>\r\n",
  0,
  NULL,
  "Undefined ENTITY foo."
},
{
  "not-wf/sa/079.xml",
  154,
  "<!DOCTYPE doc [\r\n<!ENTITY e1 \"&e2;\">\r\n<!ENTITY e2 \"&e3;\">\r\n<!ENTITY e3 \"&e1;\">\r\n<!ELEMENT doc (#PCDATA)>\r\n<!ATTLIST doc a CDATA \"&e1;\">\r\n]>\r\n<doc></doc>\r\n",
  0,
  NULL,
  "ENTITY can't reference itself directly or indirectly."
},
{
  "not-wf/sa/080.xml",
  161,
  "<!DOCTYPE doc [\r\n<!ENTITY e1 \"&e2;\">\r\n<!ENTITY e2 \"&e3;\">\r\n<!ENTITY e3 \"&e1;\">\r\n<!ELEMENT doc (#PCDATA)>\r\n<!ATTLIST doc a CDATA #FIXED \"&e1;\">\r\n]>\r\n<doc></doc>\r\n",
  0,
  NULL,
  "ENTITY can't reference itself directly or indirectly."
},
{
  "not-wf/sa/081.xml",
  68,
  "<!DOCTYPE doc [\r\n<!ENTITY e SYSTEM \"nul\">\r\n]>\r\n<doc a=\"&e;\"></doc>\r\n",
  0,
  NULL,
  "This tests the No External Entity References WFC, since the entity is referred to within an attribute."
},
{
  "not-wf/sa/082.xml",
  116,
  "<!DOCTYPE doc [\r\n<!ENTITY e SYSTEM \"nul\">\r\n<!ELEMENT doc (#PCDATA)>\r\n<!ATTLIST doc a CDATA \"&e;\">\r\n]>\r\n<doc></doc>\r\n",
  0,
  NULL,
  "This tests the No External Entity References WFC, since the entity is referred to within an attribute."
},
{
  "not-wf/sa/083.xml",
  71,
  "<!DOCTYPE doc [\r\n<!ENTITY e SYSTEM \"nul\" NDATA n>\r\n]>\r\n<doc>&e;</doc>\r\n",
  0,
  NULL,
  "Undefined NOTATION n."
},
{
  "not-wf/sa/084.xml",
  124,
  "<!DOCTYPE doc [\r\n<!ENTITY e SYSTEM \"nul\" NDATA n>\r\n<!ELEMENT doc (#PCDATA)>\r\n<!ATTLIST doc a CDATA \"&e;\">\r\n]>\r\n<doc></doc>\r\n",
  0,
  NULL,
  "Tests the Parsed Entity WFC by referring to an unparsed entity. (This precedes the error of not declaring that entity's notation, which may be detected any time before the DTD parsing is completed.)"
},
{
  "not-wf/sa/085.xml",
  51,
  "<!DOCTYPE doc PUBLIC \"[\" \"null.ent\">\r\n<doc></doc>\r\n",
  0,
  NULL,
  "Public IDs may not contain \"[\"."
},
{
  "not-wf/sa/086.xml",
  71,
  "<!DOCTYPE doc [\r\n<!ENTITY foo PUBLIC \"[\" \"null.xml\">\r\n]>\r\n<doc></doc>\r\n",
  0,
  NULL,
  "Public IDs may not contain \"[\"."
},
{
  "not-wf/sa/087.xml",
  73,
  "<!DOCTYPE doc [\r\n<!NOTATION foo PUBLIC \"[\" \"null.ent\">\r\n]>\r\n<doc></doc>\r\n",
  0,
  NULL,
  "Public IDs may not contain \"[\"."
},
{
  "not-wf/sa/088.xml",
  117,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n<!ATTLIST doc a CDATA #IMPLIED>\r\n<!ENTITY e '\"'>\r\n]>\r\n<doc a=\"&e;></doc>\r\n",
  0,
  NULL,
  "Attribute values are terminated by literal quote characters, and any entity expansion is done afterwards."
},
{
  "not-wf/sa/089.xml",
  78,
  "<!DOCTYPE doc [\r\n<!ENTITY % foo SYSTEM \"foo.xml\" NDATA bar>\r\n]>\r\n<doc></doc>\r\n",
  0,
  NULL,
  "Parameter entities \"are\" always parsed; NDATA annotations are not permitted."
},
{
  "not-wf/sa/090.xml",
  74,
  "<!DOCTYPE doc [\r\n<!ENTITY e \"<foo a='&#60;'></foo>\">\r\n]>\r\n<doc>&e;</doc>\r\n",
  0,
  NULL,
  "Attributes may not contain a literal \"<\" character; this one has one because of reference expansion."
},
{
  "not-wf/sa/091.xml",
  102,
  "<!DOCTYPE doc [\r\n<!NOTATION n SYSTEM \"n\">\r\n<!ENTITY % foo SYSTEM \"foo.xml\" NDATA n>\r\n]>\r\n<doc></doc>\r\n",
  0,
  NULL,
  "Parameter entities \"are\" always parsed; NDATA annotations are not permitted."
},
{
  "not-wf/sa/092.xml",
  74,
  "<!DOCTYPE doc [\r\n<!ENTITY e \"<foo a='&#38;'></foo>\">\r\n]>\r\n<doc>&e;</doc>\r\n",
  0,
  NULL,
  "The replacement text of this entity has an illegal reference, because the character reference is expanded immediately."
},
{
  "not-wf/sa/093.xml",
  19,
  "<doc>&#X58;</doc>\r\n",
  0,
  NULL,
  "Hexadecimal character references may not use the uppercase 'X'."
},
{
  "not-wf/sa/094.xml",
  36,
  "<?xml VERSION=\"1.0\"?>\r\n<doc></doc>\r\n",
  0,
  NULL,
  "Prolog VERSION must be lowercase."
},
{
  "not-wf/sa/095.xml",
  53,
  "<?xml encoding=\"UTF-8\" version=\"1.0\"?>\r\n<doc></doc>\r\n",
  0,
  NULL,
  "VersionInfo must come before EncodingDecl."
},
{
  "not-wf/sa/096.xml",
  51,
  "<?xml version=\"1.0\"encoding=\"UTF-8\" ?>\r\n<doc></doc>",
  0,
  NULL,
  "Space is required before the standalone declaration."
},
{
  "not-wf/sa/097.xml",
  52,
  "<?xml version=\"1.0' encoding=\"UTF-8\" ?>\r\n<doc></doc>",
  0,
  NULL,
  "Both quotes surrounding VersionNum must be the same."
},
{
  "not-wf/sa/098.xml",
  48,
  "<?xml version=\"1.0\" version=\"1.0\"?>\r\n<doc></doc>",
  0,
  NULL,
  "Only one \"version=...\" string may appear in an XML declaration."
},
{
  "not-wf/sa/099.xml",
  46,
  "<?xml version=\"1.0\" valid=\"no\" ?>\r\n<doc></doc>",
  0,
  NULL,
  "Only three pseudo-attributes are in the XML declaration, and \"valid=...\" is not one of them."
},
{
  "not-wf/sa/100.xml",
  54,
  "<?xml version=\"1.0\" standalone=\"YES\" ?>\r\n<doc></doc>\r\n",
  0,
  NULL,
  "Only \"yes\" and \"no\" are permitted as values of \"standalone\"."
},
{
  "not-wf/sa/101.xml",
  54,
  "<?xml version=\"1.0\" encoding=\" UTF-8\"?>\r\n<doc></doc>\r\n",
  0,
  NULL,
  "Space is not permitted in an encoding name."
},
{
  "not-wf/sa/102.xml",
  38,
  "<?xml version=\"1.0 \" ?>\r\n<doc></doc>\r\n",
  0,
  NULL,
  "Provides an illegal XML version number; spaces are illegal."
},
{
  "not-wf/sa/103.xml",
  62,
  "<!DOCTYPE doc [\r\n<!ENTITY e \"&#60;foo>\">\r\n]>\r\n<doc>&e;</doc>\r\n",
  0,
  NULL,
  "End-tag required for element foo."
},
{
  "not-wf/sa/104.xml",
  64,
  "<!DOCTYPE doc [\r\n<!ENTITY e \"<foo>\">\r\n]>\r\n<doc>&e;</foo></doc>\r\n",
  0,
  NULL,
  "Internal general parsed entities are only well formed if they match the \"content\" production."
},
{
  "not-wf/sa/105.xml",
  43,
  "<?pi stuff?>\r\n<![CDATA[]]>\r\n<doc>\r\n</doc>\r\n",
  0,
  NULL,
  "Invalid placement of CDATA section."
},
{
  "not-wf/sa/106.xml",
  31,
  "<?pi data?>\r\n&#32;<doc></doc>\r\n",
  0,
  NULL,
  "Invalid placement of entity declaration."
},
{
  "not-wf/sa/107.xml",
  48,
  "<!DOCTYPE doc [\r\n<![CDATA[]]>\r\n]>\r\n<doc></doc>\r\n",
  0,
  NULL,
  "Invalid document type declaration. CDATA alone is invalid."
},
{
  "not-wf/sa/108.xml",
  32,
  "<doc>\r\n<![CDATA [  ]]>\r\n</doc>\r\n",
  0,
  NULL,
  "No space in '<![CDATA['."
},
{
  "not-wf/sa/109.xml",
  53,
  "<!DOCTYPE doc [\r\n<!ENTITY e \"<doc></doc>\">\r\n]>\r\n&e;\r\n",
  0,
  NULL,
  "Tags invalid within EntityDecl."
},
{
  "not-wf/sa/110.xml",
  55,
  "<!DOCTYPE doc [\r\n<!ENTITY e \"\">\r\n]>\r\n<doc></doc>\r\n&e;\r\n",
  0,
  NULL,
  "Entity reference must be in content of element."
},
{
  "not-wf/sa/111.xml",
  63,
  "<!DOCTYPE doc [\r\n<!ENTITY e \"foo='bar'\">\r\n]>\r\n<doc &e;></doc>\r\n",
  0,
  NULL,
  "Entiry reference must be in content of element not Start-tag."
},
{
  "not-wf/sa/112.xml",
  33,
  "<doc>\r\n<![cdata[data]]>\r\n</doc>\r\n",
  0,
  NULL,
  "CDATA sections start '<![CDATA[', not '<!cdata['."
},
{
  "not-wf/sa/113.xml",
  55,
  "<!DOCTYPE doc [\r\n<!ENTITY % foo \"&\">\r\n]>\r\n<doc></doc>\r\n",
  0,
  NULL,
  "Parameter entity values must use valid reference syntax; this reference is malformed."
},
{
  "not-wf/sa/114.xml",
  53,
  "<!DOCTYPE doc [\r\n<!ENTITY foo \"&\">\r\n]>\r\n<doc></doc>\r\n",
  0,
  NULL,
  "General entity values must use valid reference syntax; this reference is malformed."
},
{
  "not-wf/sa/115.xml",
  63,
  "<!DOCTYPE doc [\r\n<!ENTITY e \"&#38;\">\r\n]>\r\n<doc a=\"&e;\"></doc>\r\n",
  0,
  NULL,
  "The replacement text of this entity is an illegal character reference, which must be rejected when it is parsed in the context of an attribute value."
},
{
  "not-wf/sa/116.xml",
  62,
  "<!DOCTYPE doc [\r\n<!ENTITY e \"&#38;#9\">\r\n]>\r\n<doc>&e;7;</doc>\r\n",
  0,
  NULL,
  "Internal general parsed entities are only well formed if they match the \"content\" production. This is a partial character reference, not a full one."
},
{
  "not-wf/sa/117.xml",
  62,
  "<!DOCTYPE doc [\r\n<!ENTITY e \"&#38;\">\r\n]>\r\n<doc>&e;#97;</doc>\r\n",
  0,
  NULL,
  "Internal general parsed entities are only well formed if they match the \"content\" production. This is a partial character reference, not a full one."
},
{
  "not-wf/sa/118.xml",
  58,
  "<!DOCTYPE doc [\r\n<!ENTITY e \"#\">\r\n]>\r\n<doc>&&e;97;</doc>\r\n",
  0,
  NULL,
  "Entity reference expansion is not recursive."
},
{
  "not-wf/sa/119.xml",
  66,
  "<!DOCTYPE doc [\r\n<!ENTITY e \"&#38;\">\r\n]>\r\n<doc>\r\n&e;#38;\r\n</doc>\r\n",
  0,
  NULL,
  "Internal general parsed entities are only well formed if they match the \"content\" production. This is a partial character reference, not a full one."
},
{
  "not-wf/sa/120.xml",
  62,
  "<!DOCTYPE doc [\r\n<!ENTITY e \"&#38;\">\r\n]>\r\n<doc>\r\n&e;\r\n</doc>\r\n",
  0,
  NULL,
  "Character references are expanded in the replacement text of an internal entity, which is then parsed as usual. Accordingly, & must be doubly quoted - encoded either as &amp; or as &#38;#38;."
},
{
  "not-wf/sa/121.xml",
  64,
  "<!DOCTYPE doc [\r\n<!ENTITY #DEFAULT \"default\">\r\n]>\r\n<doc></doc>\r\n",
  0,
  NULL,
  "A name of an ENTITY was started with an invalid character."
},
{
  "not-wf/sa/122.xml",
  64,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (a, (b) | c)?>\r\n]>\r\n<doc></doc>\r\n",
  0,
  NULL,
  "Invalid syntax mixed connectors are used."
},
{
  "not-wf/sa/123.xml",
  60,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc ((doc?)))>\r\n]>\r\n<doc></doc>\r\n",
  0,
  NULL,
  "Invalid syntax mismatched parenthesis."
},
{
  "not-wf/sa/124.xml",
  65,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (doc|#PCDATA)*>\r\n]>\r\n<doc></doc>\r\n",
  0,
  NULL,
  "Invalid format of Mixed-content declaration."
},
{
  "not-wf/sa/125.xml",
  62,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc ((#PCDATA))>\r\n]>\r\n<doc></doc>\r\n",
  0,
  NULL,
  "Invalid syntax extra set of parenthesis not necessary."
},
{
  "not-wf/sa/126.xml",
  61,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)+>\r\n]>\r\n<doc></doc>\r\n",
  0,
  NULL,
  "Invalid syntax Mixed-content must be defined as zero or more."
},
{
  "not-wf/sa/127.xml",
  61,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)?>\r\n]>\r\n<doc></doc>\r\n",
  0,
  NULL,
  "Invalid syntax Mixed-content must be defined as zero or more."
},
{
  "not-wf/sa/128.xml",
  56,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc CDATA>\r\n]>\r\n<doc></doc>\r\n",
  0,
  NULL,
  "Invalid CDATA syntax."
},
{
  "not-wf/sa/129.xml",
  64,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc - - (#PCDATA)>\r\n]>\r\n<doc></doc>\r\n",
  0,
  NULL,
  "Invalid syntax for Element Type Declaration."
},
{
  "not-wf/sa/130.xml",
  64,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (doc?) +(foo)>\r\n]>\r\n<doc></doc>\r\n",
  0,
  NULL,
  "Invalid syntax for Element Type Declaration."
},
{
  "not-wf/sa/131.xml",
  64,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (doc?) -(foo)>\r\n]>\r\n<doc></doc>\r\n",
  0,
  NULL,
  "Invalid syntax for Element Type Declaration."
},
{
  "not-wf/sa/132.xml",
  80,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (a, (b, c), (d, (e, f) | g))?>\r\n]>\r\n<doc></doc>\r\n",
  0,
  NULL,
  "Invalid syntax mixed connectors used."
},
{
  "not-wf/sa/133.xml",
  56,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (a *)>\r\n]>\r\n<doc></doc>\r\n",
  0,
  NULL,
  "Illegal whitespace before optional character causes syntax error."
},
{
  "not-wf/sa/134.xml",
  56,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (a) *>\r\n]>\r\n<doc></doc>\r\n",
  0,
  NULL,
  "Illegal whitespace before optional character causes syntax error."
},
{
  "not-wf/sa/135.xml",
  59,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (a & b)?>\r\n]>\r\n<doc></doc>\r\n",
  0,
  NULL,
  "Invalid character used as connector."
},
{
  "not-wf/sa/136.xml",
  64,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc O O (#PCDATA)>\r\n]>\r\n<doc></doc>\r\n",
  0,
  NULL,
  "Tag omission is invalid in XML."
},
{
  "not-wf/sa/137.xml",
  59,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc(#PCDATA)>\r\n]>\r\n<doc></doc>\r\n",
  0,
  NULL,
  "Space is required before a content model."
},
{
  "not-wf/sa/138.xml",
  58,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (doc*?)>\r\n]>\r\n<doc></doc>\r\n",
  0,
  NULL,
  "Invalid syntax for content particle."
},
{
  "not-wf/sa/139.xml",
  53,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc ()>\r\n]>\r\n<doc></doc>\r\n",
  0,
  NULL,
  "The element-content model should not be empty."
},
{
  "not-wf/sa/140.xml",
  74,
  "<!DOCTYPE doc [\r\n<!ENTITY e \"<&#x309a;></&#x309a;>\">\r\n]>\r\n<doc>&e;</doc>\r\n",
  0,
  NULL,
  "Character '&#x309a;' is a CombiningChar, not a Letter, and so may not begin a name."
},
{
  "not-wf/sa/141.xml",
  74,
  "<!DOCTYPE doc [\r\n<!ENTITY e \"<X&#xe5c;></X&#xe5c;>\">\r\n]>\r\n<doc>&e;</doc>\r\n",
  0,
  NULL,
  "Character #x0E5C is not legal in XML names."
},
{
  "not-wf/sa/142.xml",
  64,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n]>\r\n<doc>&#0;</doc>\r\n",
  0,
  NULL,
  "Character #x0000 is not legal anywhere in an XML document."
},
{
  "not-wf/sa/143.xml",
  65,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n]>\r\n<doc>&#31;</doc>\r\n",
  0,
  NULL,
  "Character #x001F is not legal anywhere in an XML document."
},
{
  "not-wf/sa/144.xml",
  68,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n]>\r\n<doc>&#xFFFF;</doc>\r\n",
  0,
  NULL,
  "Character #xFFFF is not legal anywhere in an XML document."
},
{
  "not-wf/sa/145.xml",
  68,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n]>\r\n<doc>&#xD800;</doc>\r\n",
  0,
  NULL,
  "Character #xD800 is not legal anywhere in an XML document. (If it appeared in a UTF-16 surrogate pair, it'd represent half of a UCS-4 character and so wouldn't really be in the document.)"
},
{
  "not-wf/sa/146.xml",
  70,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n]>\r\n<doc>&#x110000;</doc>\r\n",
  0,
  NULL,
  "Character references must also refer to legal XML characters; #x00110000 is one more than the largest legal character."
},
{
  "not-wf/sa/147.xml",
  38,
  "\r\n<?xml version=\"1.0\"?>\r\n<doc></doc>\r\n",
  0,
  NULL,
  "XML Declaration may not be preceded by whitespace."
},
{
  "not-wf/sa/148.xml",
  46,
  "<!-- -->\r\n<?xml version=\"1.0\"?>\r\n<doc></doc>\r\n",
  0,
  NULL,
  "XML Declaration may not be preceded by comments or whitespace."
},
{
  "not-wf/sa/149.xml",
  83,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n<?xml version=\"1.0\"?>\r\n]>\r\n<doc></doc>\r\n",
  0,
  NULL,
  "XML Declaration may not be within a DTD."
},
{
  "not-wf/sa/150.xml",
  38,
  "<doc>\r\n<?xml version=\"1.0\"?>\r\n</doc>\r\n",
  0,
  NULL,
  "XML declarations may not be within element content."
},
{
  "not-wf/sa/151.xml",
  38,
  "<doc>\r\n</doc>\r\n<?xml version=\"1.0\"?>\r\n",
  0,
  NULL,
  "XML declarations may not follow document content."
},
{
  "not-wf/sa/152.xml",
  39,
  "<?xml encoding=\"UTF-8\"?>\r\n<doc></doc>\r\n",
  0,
  NULL,
  "XML declarations must include the \"version=...\" string."
},
{
  "not-wf/sa/153.xml",
  103,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n<!ENTITY e \"<?xml encoding='UTF-8'?>\">\r\n]>\r\n<doc>&e;</doc>\r\n",
  0,
  NULL,
  "Text declarations may not begin internal parsed entities; they may only appear at the beginning of external parsed (parameter or general) entities."
},
{
  "not-wf/sa/154.xml",
  36,
  "<?XML version=\"1.0\"?>\r\n<doc></doc>\r\n",
  0,
  NULL,
  "'<?XML ...?>' is neither an XML declaration nor a legal processing instruction target name."
},
{
  "not-wf/sa/155.xml",
  36,
  "<?xmL version=\"1.0\"?>\r\n<doc></doc>\r\n",
  0,
  NULL,
  "'<?xmL ...?>' is neither an XML declaration nor a legal processing instruction target name."
},
{
  "not-wf/sa/156.xml",
  38,
  "<doc>\r\n<?xMl version=\"1.0\"?>\r\n</doc>\r\n",
  0,
  NULL,
  "'<?xMl ...?>' is neither an XML declaration nor a legal processing instruction target name."
},
{
  "not-wf/sa/157.xml",
  24,
  "<doc>\r\n<?xmL?>\r\n</doc>\r\n",
  0,
  NULL,
  "'<?xmL ...?>' is not a legal processing instruction target name."
},
{
  "not-wf/sa/158.xml",
  143,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n<!NOTATION gif PUBLIC \"image/gif\" \"\">\r\n<!ATTLIST #NOTATION gif a1 CDATA #IMPLIED>\r\n]>\r\n<doc></doc>\r\n",
  0,
  NULL,
  "SGML-ism: \"#NOTATION gif\" can't have attributes."
},
{
  "not-wf/sa/159.xml",
  104,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n<!ENTITY e \"<![CDATA[Tim & Michael]]>\">\r\n]>\r\n<doc>&e;</doc>\r\n",
  0,
  NULL,
  "Uses '&' unquoted in an entity declaration, which is illegal syntax for an entity reference."
},
{
  "not-wf/sa/160.xml",
  99,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n<!ENTITY % e \"\">\r\n<!ENTITY foo \"%e;\">\r\n]>\r\n<doc></doc>\r\n",
  0,
  NULL,
  "Violates the PEs in Internal Subset WFC by using a PE reference within a declaration."
},
{
  "not-wf/sa/161.xml",
  81,
  "<!DOCTYPE doc [\r\n<!ENTITY % e \"#PCDATA\">\r\n<!ELEMENT doc (%e;)>\r\n]>\r\n<doc></doc>\r\n",
  0,
  NULL,
  "Violates the PEs in Internal Subset WFC by using a PE reference within a declaration."
},
{
  "not-wf/sa/162.xml",
  102,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n<!ENTITY % e1 \"\">\r\n<!ENTITY % e2 \"%e1;\">\r\n]>\r\n<doc></doc>\r\n",
  0,
  NULL,
  "Violates the PEs in Internal Subset WFC by using a PE reference within a declaration."
},
{
  "not-wf/sa/163.xml",
  83,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n<!ENTITY % e \"\">\r\n]>\r\n%e;\r\n<doc></doc>\r\n",
  0,
  NULL,
  "Invalid placement of Parameter entity reference."
},
{
  "not-wf/sa/164.xml",
  83,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n<!ENTITY % e \"\">\r\n] %e; >\r\n<doc></doc>\r\n",
  0,
  NULL,
  "Invalid placement of Parameter entity reference."
},
{
  "not-wf/sa/165.xml",
  77,
  "<!DOCTYPE doc [\r\n<!ENTITY% e \"\">\r\n<!ELEMENT doc (#PCDATA)>\r\n]>\r\n<doc></doc>\r\n",
  0,
  NULL,
  "Parameter entity declarations must have a space before the '%'."
},
{
  "not-wf/sa/166.xml",
  16,
  "<doc>\357\277\277</doc>\r\n\277",
  0,
  NULL,
  "Character FFFF is not legal anywhere in an XML document."
},
{
  "not-wf/sa/167.xml",
  16,
  "<doc>\357\277\276</doc>\r\n\276",
  0,
  NULL,
  "Character FFFE is not legal anywhere in an XML document."
},
{
  "not-wf/sa/168.xml",
  16,
  "<doc>\355\240\200</doc>\r\n\200",
  0,
  NULL,
  "An unpaired surrogate (D800) is not legal anywhere in an XML document."
},
{
  "not-wf/sa/169.xml",
  16,
  "<doc>\355\260\200</doc>\r\n\200",
  0,
  NULL,
  "An unpaired surrogate (DC00) is not legal anywhere in an XML document."
},
{
  "not-wf/sa/170.xml",
  17,
  "<doc>\367\200\200\200</doc>\r\n\200",
  0,
  NULL,
  "Four byte UTF-8 encodings can encode UCS-4 characters which are beyond the range of legal XML characters (and can't be expressed in Unicode surrogate pairs). This document holds such a character."
},
{
  "not-wf/sa/171.xml",
  27,
  "<!-- \357\277\277 -->\r\n<doc></doc>\r\n\277",
  0,
  NULL,
  "Character FFFF is not legal anywhere in an XML document."
},
{
  "not-wf/sa/172.xml",
  25,
  "<?pi \357\277\277?>\r\n<doc></doc>\r\n\277",
  0,
  NULL,
  "Character FFFF is not legal anywhere in an XML document."
},
{
  "not-wf/sa/173.xml",
  21,
  "<doc a=\"\357\277\277\"></doc>\r\n\277",
  0,
  NULL,
  "Character FFFF is not legal anywhere in an XML document."
},
{
  "not-wf/sa/174.xml",
  28,
  "<doc><![CDATA[\357\277\277]]></doc>\r\n\277",
  0,
  NULL,
  "Character FFFF is not legal anywhere in an XML document."
},
{
  "not-wf/sa/175.xml",
  81,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n<!ENTITY % e \"\357\277\277\">\r\n]>\r\n<doc></doc>\r\n\277",
  0,
  NULL,
  "Character FFFF is not legal anywhere in an XML document."
},
{
  "not-wf/sa/176.xml",
  50,
  "<!DOCTYPE doc [\n<!ELEMENT doc (#PCDATA)>\n]>\n<doc>\n",
  0,
  NULL,
  "Start tags must have matching end tags."
},
{
  "not-wf/sa/177.xml",
  64,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n]>\r\n<doc>A\357\277\277</doc>\r\n\277",
  0,
  NULL,
  "Character FFFF is not legal anywhere in an XML document."
},
{
  "not-wf/sa/178.xml",
  102,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n<!ATTLIST doc a CDATA #IMPLIED>\r\n]>\r\n<doc a=\"&#34;></doc>\r\n",
  0,
  NULL,
  "Invalid syntax matching double quote is missing."
},
{
  "not-wf/sa/179.xml",
  54,
  "<!DOCTYPE doc [\r\n<!ENTITY e \"&#34;>\r\n]>\r\n<doc></doc>\r\n",
  0,
  NULL,
  "Invalid syntax matching double quote is missing."
},
{
  "not-wf/sa/180.xml",
  107,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n<!ATTLIST doc a CDATA \"&e;\">\r\n<!ENTITY e \"v\">\r\n]>\r\n<doc></doc>\r\n",
  0,
  NULL,
  "The Entity Declared WFC requires entities to be declared before they are used in an attribute list declaration."
},
{
  "not-wf/sa/181.xml",
  95,
  "<!DOCTYPE doc [\r\n<!ENTITY e \"&#60;![CDATA[\">\r\n<!ELEMENT doc (#PCDATA)>\r\n]>\r\n<doc>&e;]]></doc>\r\n",
  0,
  NULL,
  "Internal parsed entities must match the content production to be well formed."
},
{
  "not-wf/sa/182.xml",
  90,
  "<!DOCTYPE doc [\r\n<!ENTITY e \"&#60;!--\">\r\n<!ELEMENT doc (#PCDATA)>\r\n]>\r\n<doc>&e;--></doc>\r\n",
  0,
  NULL,
  "Internal parsed entities must match the content production to be well formed."
},
{
  "not-wf/sa/183.xml",
  91,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA | foo*)* >\r\n<!ELEMENT foo EMPTY>\r\n]>\r\n<doc></doc>\r\n",
  0,
  NULL,
  "Mixed content declarations may not include content particles."
},
{
  "not-wf/sa/184.xml",
  94,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA | (foo))* >\r\n<!ELEMENT foo EMPTY>\r\n]>\r\n<doc></doc>\r\n\r\n",
  0,
  NULL,
  "In mixed content models, element names must not be parenthesized."
},
{
  "not-wf/sa/185.xml",
  89,
  "<?xml version=\"1.0\" standalone=\"yes\"?>\r\n<!DOCTYPE doc SYSTEM \"185.ent\">\r\n<doc>&e;</doc>\r\n",
  0,
  NULL,
  "Tests the Entity Declared WFC. Note: a nonvalidating parser is permitted not to report this WFC violation, since it would need to read an external parameter entity to distinguish it from a violation of the Standalone Declaration VC."
},
{
  "not-wf/sa/186.xml",
  104,
  "<!DOCTYPE a [\r\n<!ELEMENT a EMPTY>\r\n<!ATTLIST a b CDATA #IMPLIED d CDATA #IMPLIED>\r\n]>\r\n<a b=\"c\"d=\"e\"/>\r\n",
  0,
  NULL,
  "Whitespace is required between attribute/value pairs."
},
{
  "not-wf/not-sa/001.xml",
  46,
  "<!DOCTYPE doc SYSTEM \"001.ent\">\r\n<doc></doc>\r\n",
  0,
  NULL,
  "Conditional sections must be properly terminated (\"]>\" used instead of \"]]>\")."
},
{
  "not-wf/not-sa/002.xml",
  119,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n<!ENTITY % e \"<?xml version='1.0' encoding='UTF-8'?>\">\r\n%e;\r\n]>\r\n<doc></doc>",
  0,
  NULL,
  "Processing instruction target names may not be \"XML\" in any combination of cases."
},
{
  "not-wf/not-sa/003.xml",
  46,
  "<!DOCTYPE doc SYSTEM \"003.ent\">\r\n<doc></doc>\r\n",
  0,
  NULL,
  "Conditional sections must be properly terminated (\"]]>\" omitted)."
},
{
  "not-wf/not-sa/004.xml",
  46,
  "<!DOCTYPE doc SYSTEM \"004.ent\">\r\n<doc></doc>\r\n",
  0,
  NULL,
  "Conditional sections must be properly terminated (\"]]>\" omitted)."
},
{
  "not-wf/not-sa/005.xml",
  46,
  "<!DOCTYPE doc SYSTEM \"005.ent\">\r\n<doc></doc>\r\n",
  0,
  NULL,
  "Tests the Entity Declared VC by referring to an undefined parameter entity within an external entity."
},
{
  "not-wf/not-sa/006.xml",
  46,
  "<!DOCTYPE doc SYSTEM \"006.ent\">\r\n<doc></doc>\r\n",
  0,
  NULL,
  "Conditional sections need a '[' after the INCLUDE or IGNORE."
},
{
  "not-wf/not-sa/007.xml",
  46,
  "<!DOCTYPE doc SYSTEM \"007.ent\">\r\n<doc></doc>\r\n",
  0,
  NULL,
  "A <!DOCTYPE ...> declaration may not begin any external entity; it's only found once, in the document entity."
},
{
  "not-wf/not-sa/008.xml",
  46,
  "<!DOCTYPE doc SYSTEM \"008.ent\">\r\n<doc></doc>\r\n",
  0,
  NULL,
  "In DTDs, the '%' character must be part of a parameter entity reference."
},
{
  "not-wf/not-sa/009.xml",
  46,
  "<!DOCTYPE doc SYSTEM \"009.ent\">\r\n<doc></doc>\r\n",
  0,
  NULL,
  "This test violates WFC:PE Between Declarations in Production 28a. The last character of a markup declaration is not contained in the same parameter-entity text replacement."
},
{
  "not-wf/ext-sa/001.xml",
  67,
  "<!DOCTYPE doc [\r\n<!ENTITY e SYSTEM \"001.ent\">\r\n]>\r\n<doc>&e;</doc>\r\n",
  0,
  NULL,
  "Tests the No Recursion WFC by having an external general entity be self-recursive."
},
{
  "not-wf/ext-sa/002.xml",
  93,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n<!ENTITY e SYSTEM \"002.ent\">\r\n]>\r\n<doc>&e;</doc>\r\n",
  0,
  NULL,
  "External entities have \"text declarations\", which do not permit the \"standalone=...\" attribute that's allowed in XML declarations."
},
{
  "not-wf/ext-sa/003.xml",
  93,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n<!ENTITY e SYSTEM \"003.ent\">\r\n]>\r\n<doc>&e;</doc>\r\n",
  0,
  NULL,
  "Only one text declaration is permitted; a second one looks like an illegal processing instruction (target names of \"xml\" in any case are not allowed)."
},
{
  "invalid/002.xml",
  46,
  "<!DOCTYPE doc SYSTEM \"002.ent\">\r\n<doc></doc>\r\n",
  0,
  NULL,
  "Tests the \"Proper Group/PE Nesting\" validity constraint by fragmenting a content model between two parameter entities."
},
{
  "invalid/005.xml",
  46,
  "<!DOCTYPE doc SYSTEM \"005.ent\">\r\n<doc></doc>\r\n",
  0,
  NULL,
  "Tests the \"Proper Declaration/PE Nesting\" validity constraint by fragmenting an element declaration between two parameter entities."
},
{
  "invalid/006.xml",
  46,
  "<!DOCTYPE doc SYSTEM \"006.ent\">\r\n<doc></doc>\r\n",
  0,
  NULL,
  "Tests the \"Proper Declaration/PE Nesting\" validity constraint by fragmenting an element declaration between two parameter entities."
},
{
  "invalid/not-sa/022.xml",
  46,
  "<!DOCTYPE doc SYSTEM \"022.ent\">\r\n<doc></doc>\r\n",
  0,
  NULL,
  "Test the \"Proper Conditional Section/ PE Nesting\" validity constraint."
},
{
  "valid/sa/001.xml",
  60,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n]>\r\n<doc></doc>\r\n",
  11,
  "<doc></doc>",
  "Test demonstrates an Element Type Declaration with Mixed Content."
},
{
  "valid/sa/002.xml",
  61,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n]>\r\n<doc ></doc>\r\n",
  11,
  "<doc></doc>",
  "Test demonstrates that whitespace is permitted after the tag name in a Start-tag."
},
{
  "valid/sa/003.xml",
  61,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n]>\r\n<doc></doc >\r\n",
  11,
  "<doc></doc>",
  "Test demonstrates that whitespace is permitted after the tag name in an End-tag."
},
{
  "valid/sa/004.xml",
  102,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n<!ATTLIST doc a1 CDATA #IMPLIED>\r\n]>\r\n<doc a1=\"v1\"></doc>\r\n",
  19,
  "<doc a1=\"v1\"></doc>",
  "Test demonstrates a valid attribute specification within a Start-tag."
},
{
  "valid/sa/005.xml",
  104,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n<!ATTLIST doc a1 CDATA #IMPLIED>\r\n]>\r\n<doc a1 = \"v1\"></doc>\r\n",
  19,
  "<doc a1=\"v1\"></doc>",
  "Test demonstrates a valid attribute specification within a Start-tag that contains whitespace on both sides of the equal sign."
},
{
  "valid/sa/006.xml",
  102,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n<!ATTLIST doc a1 CDATA #IMPLIED>\r\n]>\r\n<doc a1='v1'></doc>\r\n",
  19,
  "<doc a1=\"v1\"></doc>",
  "Test demonstrates that the AttValue within a Start-tag can use a single quote as a delimter."
},
{
  "valid/sa/007.xml",
  65,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n]>\r\n<doc>&#32;</doc>\r\n",
  12,
  "<doc> </doc>",
  "Test demonstrates numeric character references can be used for element content."
},
{
  "valid/sa/008.xml",
  85,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n]>\r\n<doc>&amp;&lt;&gt;&quot;&apos;</doc>\r\n",
  31,
  "<doc>&amp;&lt;&gt;&quot;'</doc>",
  "Test demonstrates character references can be used for element content."
},
{
  "valid/sa/009.xml",
  66,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n]>\r\n<doc>&#x20;</doc>\r\n",
  12,
  "<doc> </doc>",
  "Test demonstrates that PubidChar can be used for element content."
},
{
  "valid/sa/010.xml",
  103,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n<!ATTLIST doc a1 CDATA #IMPLIED>\r\n]>\r\n<doc a1=\"v1\" ></doc>\r\n",
  19,
  "<doc a1=\"v1\"></doc>",
  "Test demonstrates that whitespace is valid after the Attribute in a Start-tag."
},
{
  "valid/sa/011.xml",
  128,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n<!ATTLIST doc a1 CDATA #IMPLIED a2 CDATA #IMPLIED>\r\n]>\r\n<doc a1=\"v1\" a2=\"v2\"></doc>\r\n",
  27,
  "<doc a1=\"v1\" a2=\"v2\"></doc>",
  "Test demonstrates mutliple Attibutes within the Start-tag."
},
{
  "valid/sa/012.xml",
  100,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n<!ATTLIST doc : CDATA #IMPLIED>\r\n]>\r\n<doc :=\"v1\"></doc>\r\n",
  18,
  "<doc :=\"v1\"></doc>",
  "Uses a legal XML 1.0 name consisting of a single colon character (disallowed by the latest XML Namespaces draft)."
},
{
  "valid/sa/013.xml",
  124,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n<!ATTLIST doc _.-0123456789 CDATA #IMPLIED>\r\n]>\r\n<doc _.-0123456789=\"v1\"></doc>\r\n",
  30,
  "<doc _.-0123456789=\"v1\"></doc>",
  "Test demonstrates that the Attribute in a Start-tag can consist of numerals along with special characters."
},
{
  "valid/sa/014.xml",
  150,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n<!ATTLIST doc abcdefghijklmnopqrstuvwxyz CDATA #IMPLIED>\r\n]>\r\n<doc abcdefghijklmnopqrstuvwxyz=\"v1\"></doc>\r\n",
  43,
  "<doc abcdefghijklmnopqrstuvwxyz=\"v1\"></doc>",
  "Test demonstrates that all lower case letters are valid for the Attribute in a Start-tag."
},
{
  "valid/sa/015.xml",
  150,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n<!ATTLIST doc ABCDEFGHIJKLMNOPQRSTUVWXYZ CDATA #IMPLIED>\r\n]>\r\n<doc ABCDEFGHIJKLMNOPQRSTUVWXYZ=\"v1\"></doc>\r\n",
  43,
  "<doc ABCDEFGHIJKLMNOPQRSTUVWXYZ=\"v1\"></doc>",
  "Test demonstrates that all upper case letters are valid for the Attribute in a Start-tag."
},
{
  "valid/sa/016.xml",
  66,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n]>\r\n<doc><?pi?></doc>\r\n",
  18,
  "<doc><?pi ?></doc>",
  "Test demonstrates that Processing Instructions are valid element content."
},
{
  "valid/sa/017.xml",
  83,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n]>\r\n<doc><?pi some data ? > <??\?></doc>\r\n",
  34,
  "<doc><?pi some data ? > <??\?></doc>",
  "Test demonstrates that Processing Instructions are valid element content and there can be more than one."
},
{
  "valid/sa/018.xml",
  77,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n]>\r\n<doc><![CDATA[<foo>]]></doc>\r\n",
  22,
  "<doc>&lt;foo&gt;</doc>",
  "Test demonstrates that CDATA sections are valid element content."
},
{
  "valid/sa/019.xml",
  74,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n]>\r\n<doc><![CDATA[<&]]></doc>\r\n",
  20,
  "<doc>&lt;&amp;</doc>",
  "Test demonstrates that CDATA sections are valid element content and that ampersands may occur in their literal form."
},
{
  "valid/sa/020.xml",
  77,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n]>\r\n<doc><![CDATA[<&]>]]]></doc>\r\n",
  26,
  "<doc>&lt;&amp;]&gt;]</doc>",
  "Test demonstractes that CDATA sections are valid element content and that everyting between the CDStart and CDEnd is recognized as character data not markup."
},
{
  "valid/sa/021.xml",
  78,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n]>\r\n<doc><!-- a comment --></doc>\r\n",
  11,
  "<doc></doc>",
  "Test demonstrates that comments are valid element content."
},
{
  "valid/sa/022.xml",
  80,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n]>\r\n<doc><!-- a comment ->--></doc>\r\n",
  11,
  "<doc></doc>",
  "Test demonstrates that comments are valid element content and that all characters before the double-hypen right angle combination are considered part of thecomment."
},
{
  "valid/sa/023.xml",
  79,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n<!ENTITY e \"\">\r\n]>\r\n<doc>&e;</doc>\r\n",
  11,
  "<doc></doc>",
  "Test demonstrates that Entity References are valid element content."
},
{
  "valid/sa/024.xml",
  116,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (foo)>\r\n<!ELEMENT foo (#PCDATA)>\r\n<!ENTITY e \"&#60;foo></foo>\">\r\n]>\r\n<doc>&e;</doc>\r\n",
  22,
  "<doc><foo></foo></doc>",
  "Test demonstrates that Entity References are valid element content and also demonstrates a valid Entity Declaration."
},
{
  "valid/sa/025.xml",
  100,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (foo*)>\r\n<!ELEMENT foo (#PCDATA)>\r\n]>\r\n<doc><foo/><foo></foo></doc>\r\n",
  33,
  "<doc><foo></foo><foo></foo></doc>",
  "Test demonstrates an Element Type Declaration and that the contentspec can be of mixed content."
},
{
  "valid/sa/026.xml",
  96,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (foo*)>\r\n<!ELEMENT foo EMPTY>\r\n]>\r\n<doc><foo/><foo></foo></doc>\r\n",
  33,
  "<doc><foo></foo><foo></foo></doc>",
  "Test demonstrates an Element Type Declaration and that EMPTY is a valid contentspec."
},
{
  "valid/sa/027.xml",
  94,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (foo*)>\r\n<!ELEMENT foo ANY>\r\n]>\r\n<doc><foo/><foo></foo></doc>\r\n",
  33,
  "<doc><foo></foo><foo></foo></doc>",
  "Test demonstrates an Element Type Declaration and that ANY is a valid contenspec."
},
{
  "valid/sa/028.xml",
  83,
  "<?xml version=\"1.0\"?>\r\n<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n]>\r\n<doc></doc>\r\n",
  11,
  "<doc></doc>",
  "Test demonstrates a valid prolog that uses double quotes as delimeters around the VersionNum."
},
{
  "valid/sa/029.xml",
  83,
  "<?xml version='1.0'?>\r\n<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n]>\r\n<doc></doc>\r\n",
  11,
  "<doc></doc>",
  "Test demonstrates a valid prolog that uses single quotes as delimters around the VersionNum."
},
{
  "valid/sa/030.xml",
  85,
  "<?xml version = \"1.0\"?>\r\n<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n]>\r\n<doc></doc>\r\n",
  11,
  "<doc></doc>",
  "Test demonstrates a valid prolog that contains whitespace on both sides of the equal sign in the VersionInfo."
},
{
  "valid/sa/031.xml",
  100,
  "<?xml version='1.0' encoding=\"UTF-8\"?>\r\n<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n]>\r\n<doc></doc>\r\n",
  11,
  "<doc></doc>",
  "Test demonstrates a valid EncodingDecl within the prolog."
},
{
  "valid/sa/032.xml",
  100,
  "<?xml version='1.0' standalone='yes'?>\r\n<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n]>\r\n<doc></doc>\r\n",
  11,
  "<doc></doc>",
  "Test demonstrates a valid SDDecl within the prolog."
},
{
  "valid/sa/033.xml",
  117,
  "<?xml version='1.0' encoding=\"UTF-8\" standalone='yes'?>\r\n<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n]>\r\n<doc></doc>\r\n",
  11,
  "<doc></doc>",
  "Test demonstrates that both a EncodingDecl and SDDecl are valid within the prolog."
},
{
  "valid/sa/034.xml",
  55,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n]>\r\n<doc/>\r\n",
  11,
  "<doc></doc>",
  "Test demonstrates the correct syntax for an Empty element tag."
},
{
  "valid/sa/035.xml",
  56,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n]>\r\n<doc />\r\n",
  11,
  "<doc></doc>",
  "Test demonstrates that whitespace is permissible after the name in an Empty element tag."
},
{
  "valid/sa/036.xml",
  73,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n]>\r\n<doc></doc>\r\n<?pi data?>\r\n",
  22,
  "<doc></doc><?pi data?>",
  "Test demonstrates a valid processing instruction."
},
{
  "valid/sa/037.xml",
  80,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n]>\r\n<doc></doc>\r\n<!-- comment -->\r\n\r\n",
  11,
  "<doc></doc>",
  "Test demonstrates a valid comment and that it may appear anywhere in the document including at the end."
},
{
  "valid/sa/038.xml",
  80,
  "<!-- comment -->\r\n<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n]>\r\n<doc></doc>\r\n\r\n",
  11,
  "<doc></doc>",
  "Test demonstrates a valid comment and that it may appear anywhere in the document including the beginning."
},
{
  "valid/sa/039.xml",
  73,
  "<?pi data?>\r\n<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n]>\r\n<doc></doc>\r\n",
  22,
  "<?pi data?><doc></doc>",
  "Test demonstrates a valid processing instruction and that it may appear at the beginning of the document."
},
{
  "valid/sa/040.xml",
  125,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n<!ATTLIST doc a1 CDATA #IMPLIED>\r\n]>\r\n<doc a1=\"&quot;&lt;&amp;&gt;&apos;\"></doc>\r\n",
  37,
  "<doc a1=\"&quot;&lt;&amp;&gt;'\"></doc>",
  "Test demonstrates an Attribute List declaration that uses a StringType as the AttType."
},
{
  "valid/sa/041.xml",
  105,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n<!ATTLIST doc a1 CDATA #IMPLIED>\r\n]>\r\n<doc a1=\"&#65;\"></doc>\r\n",
  18,
  "<doc a1=\"A\"></doc>",
  "Test demonstrates an Attribute List declaration that uses a StringType as the AttType and also expands the CDATA attribute with a character reference."
},
{
  "valid/sa/042.xml",
  98,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n]>\r\n<doc>&#00000000000000000000000000000000065;</doc>\r\n",
  12,
  "<doc>A</doc>",
  "Test demonstrates an Attribute List declaration that uses a StringType as the AttType and also expands the CDATA attribute with a character reference. The test also shows that the leading zeros in the character reference are ignored."
},
{
  "valid/sa/043.xml",
  108,
  "<!DOCTYPE doc [\r\n<!ATTLIST doc a1 CDATA #IMPLIED>\r\n<!ELEMENT doc (#PCDATA)>\r\n]>\r\n<doc a1=\"foo\r\nbar\"></doc>\r\n",
  24,
  "<doc a1=\"foo bar\"></doc>",
  "An element's attributes may be declared before its content model; and attribute values may contain newlines."
},
{
  "valid/sa/044.xml",
  187,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (e*)>\r\n<!ELEMENT e EMPTY>\r\n<!ATTLIST e a1 CDATA \"v1\" a2 CDATA \"v2\" a3 CDATA #IMPLIED>\r\n]>\r\n<doc>\r\n<e a3=\"v3\"/>\r\n<e a1=\"w1\"/>\r\n<e a2=\"w2\" a3=\"v3\"/>\r\n</doc>\r\n",
  116,
  "<doc>&#10;<e a1=\"v1\" a2=\"v2\" a3=\"v3\"></e>&#10;<e a1=\"w1\" a2=\"v2\"></e>&#10;<e a1=\"v1\" a2=\"w2\" a3=\"v3\"></e>&#10;</doc>",
  "Test demonstrates that the empty-element tag must be use for an elements that are declared EMPTY."
},
{
  "valid/sa/045.xml",
  120,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n<!ATTLIST doc a1 CDATA \"v1\">\r\n<!ATTLIST doc a1 CDATA \"z1\">\r\n]>\r\n<doc></doc>\r\n",
  19,
  "<doc a1=\"v1\"></doc>",
  "Tests whether more than one definition can be provided for the same attribute of a given element type with the first declaration being binding."
},
{
  "valid/sa/046.xml",
  120,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n<!ATTLIST doc a1 CDATA \"v1\">\r\n<!ATTLIST doc a2 CDATA \"v2\">\r\n]>\r\n<doc></doc>\r\n",
  27,
  "<doc a1=\"v1\" a2=\"v2\"></doc>",
  "Test demonstrates that when more than one AttlistDecl is provided for a given element type, the contents of all those provided are merged."
},
{
  "valid/sa/047.xml",
  64,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n]>\r\n<doc>X\r\nY</doc>\r\n",
  18,
  "<doc>X&#10;Y</doc>",
  "Test demonstrates that extra whitespace is normalized into single space character."
},
{
  "valid/sa/048.xml",
  61,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n]>\r\n<doc>]</doc>\r\n",
  12,
  "<doc>]</doc>",
  "Test demonstrates that character data is valid element content."
},
{
  "valid/sa/049.xml",
  124,
  "\377\376<\0!\0D\0O\0C\0T\0Y\0P\0E\0 \0d\0o\0c\0 \0[\0\r\0\n\0<\0!\0E\0L\0E\0M\0E\0N\0T\0 \0d\0o\0c\0 \0(\0#\0P\0C\0D\0A\0T\0A\0)\0>\0\r\0\n\0]\0>\0\r\0\n\0<\0d\0o\0c\0>\0\243\0<\0/\0d\0o\0c\0>\0\r\0\n\0",
  13,
  "<doc>\302\243</doc>\243",
  "Test demonstrates that characters outside of normal ascii range can be used as element content."
},
{
  "valid/sa/050.xml",
  132,
  "\377\376<\0!\0D\0O\0C\0T\0Y\0P\0E\0 \0d\0o\0c\0 \0[\0\r\0\n\0<\0!\0E\0L\0E\0M\0E\0N\0T\0 \0d\0o\0c\0 \0(\0#\0P\0C\0D\0A\0T\0A\0)\0>\0\r\0\n\0]\0>\0\r\0\n\0<\0d\0o\0c\0>\0@\16\b\16!\16*\16L\16<\0/\0d\0o\0c\0>\0\r\0\n\0",
  26,
  "<doc>\340\271\200\340\270\210\340\270\241\340\270\252\340\271\214</doc>\214",
  "Test demonstrates that characters outside of normal ascii range can be used as element content."
},
{
  "valid/sa/051.xml",
  140,
  "\377\376<\0!\0D\0O\0C\0T\0Y\0P\0E\0 \0@\16\b\16!\16*\16L\16 \0[\0\r\0\n\0<\0!\0E\0L\0E\0M\0E\0N\0T\0 \0@\16\b\16!\16*\16L\16 \0 \0(\0#\0P\0C\0D\0A\0T\0A\0)\0>\0\r\0\n\0]\0>\0\r\0\n\0<\0@\16\b\16!\16*\16L\16>\0<\0/\0@\16\b\16!\16*\16L\16>\0\r\0\n\0",
  35,
  "<\340\271\200\340\270\210\340\270\241\340\270\252\340\271\214></\340\271\200\340\270\210\340\270\241\340\270\252\340\271\214>\214",
  "The document is encoded in UTF-16 and uses some name characters well outside of the normal ASCII range."
},
{
  "valid/sa/052.xml",
  68,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n]>\r\n<doc>\360\220\200\200\364\217\277\275</doc>\r\n\275",
  19,
  "<doc>\360\220\200\200\364\217\277\275</doc>\275",
  "The document is encoded in UTF-8 and the text inside the root element uses two non-ASCII characters, encoded in UTF-8 and each of which expands to a Unicode surrogate pair."
},
{
  "valid/sa/053.xml",
  97,
  "<!DOCTYPE doc [\r\n<!ENTITY e \"<e/>\">\r\n<!ELEMENT doc (e)>\r\n<!ELEMENT e EMPTY>\r\n]>\r\n<doc>&e;</doc>\r\n",
  18,
  "<doc><e></e></doc>",
  "Tests inclusion of a well-formed internal entity, which holds an element required by the content model."
},
{
  "valid/sa/054.xml",
  72,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n]>\r\n\r\n\r\n<doc\r\n></doc\r\n>\r\n\r\n\r\n",
  11,
  "<doc></doc>",
  "Test demonstrates that extra whitespace within Start-tags and End-tags are nomalized into single spaces."
},
{
  "valid/sa/055.xml",
  74,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n]>\r\n<?pi  data?>\r\n<doc></doc>\r\n",
  22,
  "<?pi data?><doc></doc>",
  "Test demonstrates that extra whitespace within a processing instruction willnormalized into s single space character."
},
{
  "valid/sa/056.xml",
  104,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n]>\r\n<doc>&#x0000000000000000000000000000000000000041;</doc>\r\n",
  12,
  "<doc>A</doc>",
  "Test demonstrates an Attribute List declaration that uses a StringType as the AttType and also expands the CDATA attribute with a character reference. The test also shows that the leading zeros in the character reference are ignored."
},
{
  "valid/sa/057.xml",
  55,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (a*)>\r\n]>\r\n<doc></doc>\r\n",
  11,
  "<doc></doc>",
  "Test demonstrates an element content model whose element can occur zero or more times."
},
{
  "valid/sa/058.xml",
  111,
  "<!DOCTYPE doc [\r\n<!ATTLIST doc a1 NMTOKENS #IMPLIED>\r\n<!ELEMENT doc (#PCDATA)>\r\n]>\r\n<doc a1=\" 1  \t2 \t\"></doc>\r\n",
  20,
  "<doc a1=\"1 2\"></doc>",
  "Test demonstrates that extra whitespace be normalized into a single space character in an attribute of type NMTOKENS."
},
{
  "valid/sa/059.xml",
  227,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (e*)>\r\n<!ELEMENT e EMPTY>\r\n<!ATTLIST e a1 CDATA #IMPLIED a2 CDATA #IMPLIED a3 CDATA #IMPLIED>\r\n]>\r\n<doc>\r\n<e a1=\"v1\" a2=\"v2\" a3=\"v3\"/>\r\n<e a1=\"w1\" a2=\"v2\"/>\r\n<e a1=\"v1\" a2=\"w2\" a3=\"v3\"/>\r\n</doc>\r\n",
  116,
  "<doc>&#10;<e a1=\"v1\" a2=\"v2\" a3=\"v3\"></e>&#10;<e a1=\"w1\" a2=\"v2\"></e>&#10;<e a1=\"v1\" a2=\"w2\" a3=\"v3\"></e>&#10;</doc>",
  "Test demonstrates an Element Type Declaration that uses the contentspec of EMPTY. The element cannot have any contents and must always appear as an empty element in the document. The test also shows an Attribute-list declaration with multiple AttDef's."
},
{
  "valid/sa/060.xml",
  67,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n]>\r\n<doc>X&#10;Y</doc>\r\n",
  18,
  "<doc>X&#10;Y</doc>",
  "Test demonstrates the use of decimal Character References within element content."
},
{
  "valid/sa/061.xml",
  66,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n]>\r\n<doc>&#163;</doc>\r\n",
  13,
  "<doc>\302\243</doc>\243",
  "Test demonstrates the use of decimal Character References within element content."
},
{
  "valid/sa/062.xml",
  87,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n]>\r\n<doc>&#xe40;&#xe08;&#xe21;\340\270\252\340\271\214</doc>\r\n\214",
  26,
  "<doc>\340\271\200\340\270\210\340\270\241\340\270\252\340\271\214</doc>\214",
  "Test demonstrates the use of hexadecimal Character References within element."
},
{
  "valid/sa/063.xml",
  108,
  "<!DOCTYPE \340\271\200\340\270\210\340\270\241\340\270\252\340\271\214 [\r\n<!ELEMENT \340\271\200\340\270\210\340\270\241\340\270\252\340\271\214 (#PCDATA)>\r\n]>\r\n<\340\271\200\340\270\210\340\270\241\340\270\252\340\271\214></\340\271\200\340\270\210\340\270\241\340\270\252\340\271\214>\r\n\214",
  35,
  "<\340\271\200\340\270\210\340\270\241\340\270\252\340\271\214></\340\271\200\340\270\210\340\270\241\340\270\252\340\271\214>\214",
  "The document is encoded in UTF-8 and the name of the root element type uses non-ASCII characters."
},
{
  "valid/sa/064.xml",
  79,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n]>\r\n<doc>&#x10000;&#x10FFFD;</doc>\r\n",
  19,
  "<doc>\360\220\200\200\364\217\277\275</doc>\275",
  "Tests in-line handling of two legal character references, which each expand to a Unicode surrogate pair."
},
{
  "valid/sa/065.xml",
  81,
  "<!DOCTYPE doc [\r\n<!ENTITY e \"&#60;\">\r\n<!ELEMENT doc (#PCDATA)>\r\n]>\r\n<doc></doc>\r\n",
  11,
  "<doc></doc>",
  "Tests ability to define an internal entity which can't legally be expanded (contains an unquoted <)."
},
{
  "valid/sa/066.xml",
  155,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n<!ATTLIST doc a1 CDATA #IMPLIED>\r\n<!-- 34 is double quote -->\r\n<!ENTITY e1 \"&#34;\">\r\n]>\r\n<doc a1=\"&e1;\"></doc>\r\n",
  23,
  "<doc a1=\"&quot;\"></doc>",
  "Expands a CDATA attribute with a character reference."
},
{
  "valid/sa/067.xml",
  65,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n]>\r\n<doc>&#13;</doc>\r\n",
  16,
  "<doc>&#13;</doc>",
  "Test demonstrates the use of decimal character references within element content."
},
{
  "valid/sa/068.xml",
  84,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n<!ENTITY e \"&#13;\">\r\n]>\r\n<doc>&e;</doc>\r\n",
  16,
  "<doc>&#13;</doc>",
  "Tests definition of an internal entity holding a carriage return character reference, which must not be normalized before reporting to the application. Line break normalization only occurs when parsing external parsed entities."
},
{
  "valid/sa/069.xml",
  93,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n<!NOTATION n PUBLIC \"whatever\">\r\n]>\r\n<doc></doc>\r\n",
  62,
  "<!DOCTYPE doc [\n<!NOTATION n PUBLIC 'whatever'>\n]>\n<doc></doc>",
  "Verifies that an XML parser will parse a NOTATION declaration; the output phase of this test ensures that it's reported to the application."
},
{
  "valid/sa/070.xml",
  81,
  "<!DOCTYPE doc [\r\n<!ENTITY % e \"<!ELEMENT doc (#PCDATA)>\">\r\n%e;\r\n]>\r\n<doc></doc>\r\n",
  11,
  "<doc></doc>",
  "Verifies that internal parameter entities are correctly expanded within the internal subset."
},
{
  "valid/sa/071.xml",
  90,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n<!ATTLIST doc a ID #IMPLIED>\r\n]>\r\n<doc></doc>\r\n",
  11,
  "<doc></doc>",
  "Test demonstrates that an AttlistDecl can use ID as the TokenizedType within the Attribute type. The test also shows that IMPLIED is a valid DefaultDecl."
},
{
  "valid/sa/072.xml",
  93,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n<!ATTLIST doc a IDREF #IMPLIED>\r\n]>\r\n<doc></doc>\r\n",
  11,
  "<doc></doc>",
  "Test demonstrates that an AttlistDecl can use IDREF as the TokenizedType within the Attribute type. The test also shows that IMPLIED is a valid DefaultDecl."
},
{
  "valid/sa/073.xml",
  94,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n<!ATTLIST doc a IDREFS #IMPLIED>\r\n]>\r\n<doc></doc>\r\n",
  11,
  "<doc></doc>",
  "Test demonstrates that an AttlistDecl can use IDREFS as the TokenizedType within the Attribute type. The test also shows that IMPLIED is a valid DefaultDecl."
},
{
  "valid/sa/074.xml",
  94,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n<!ATTLIST doc a ENTITY #IMPLIED>\r\n]>\r\n<doc></doc>\r\n",
  11,
  "<doc></doc>",
  "Test demonstrates that an AttlistDecl can use ENTITY as the TokenizedType within the Attribute type. The test also shows that IMPLIED is a valid DefaultDecl."
},
{
  "valid/sa/075.xml",
  96,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n<!ATTLIST doc a ENTITIES #IMPLIED>\r\n]>\r\n<doc></doc>\r\n",
  11,
  "<doc></doc>",
  "Test demonstrates that an AttlistDecl can use ENTITIES as the TokenizedType within the Attribute type. The test also shows that IMPLIED is a valid DefaultDecl."
},
{
  "valid/sa/076.xml",
  192,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n<!ATTLIST doc a NOTATION (n1|n2) #IMPLIED>\r\n<!NOTATION n1 SYSTEM \"http://www.w3.org/\">\r\n<!NOTATION n2 SYSTEM \"http://www.w3.org/\">\r\n]>\r\n<doc></doc>\r\n",
  116,
  "<!DOCTYPE doc [\n<!NOTATION n1 SYSTEM 'http://www.w3.org/'>\n<!NOTATION n2 SYSTEM 'http://www.w3.org/'>\n]>\n<doc></doc>",
  "Verifies that an XML parser will parse a NOTATION attribute; the output phase of this test ensures that both notations are reported to the application."
},
{
  "valid/sa/077.xml",
  93,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n<!ATTLIST doc a (1|2) #IMPLIED>\r\n]>\r\n<doc></doc>\r\n",
  11,
  "<doc></doc>",
  "Test demonstrates that an AttlistDecl can use an EnumeratedType within the Attribute type. The test also shows that IMPLIED is a valid DefaultDecl."
},
{
  "valid/sa/078.xml",
  100,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n<!ATTLIST doc a CDATA #REQUIRED>\r\n]>\r\n<doc a=\"v\"></doc>\r\n",
  17,
  "<doc a=\"v\"></doc>",
  "Test demonstrates that an AttlistDecl can use an StringType of CDATA within the Attribute type. The test also shows that REQUIRED is a valid DefaultDecl."
},
{
  "valid/sa/079.xml",
  101,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n<!ATTLIST doc a CDATA #FIXED \"v\">\r\n]>\r\n<doc a=\"v\"></doc>\r\n",
  17,
  "<doc a=\"v\"></doc>",
  "Test demonstrates that an AttlistDecl can use an StringType of CDATA within the Attribute type. The test also shows that FIXED is a valid DefaultDecl and that a value can be given to the attribute in the Start-tag as well as the AttListDecl."
},
{
  "valid/sa/080.xml",
  95,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n<!ATTLIST doc a CDATA #FIXED \"v\">\r\n]>\r\n<doc></doc>\r\n",
  17,
  "<doc a=\"v\"></doc>",
  "Test demonstrates that an AttlistDecl can use an StringType of CDATA within the Attribute type. The test also shows that FIXED is a valid DefaultDecl and that an value can be given to the attribute."
},
{
  "valid/sa/081.xml",
  140,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (a, b, c)>\r\n<!ELEMENT a (a?)>\r\n<!ELEMENT b (b*)>\r\n<!ELEMENT c (a | b)+>\r\n]>\r\n<doc><a/><b/><c><a/></c></doc>\r\n",
  39,
  "<doc><a></a><b></b><c><a></a></c></doc>",
  "Test demonstrates the use of the optional character following a name or list to govern the number of times an element or content particles in the list occur."
},
{
  "valid/sa/082.xml",
  90,
  "<!DOCTYPE doc [\r\n<!ENTITY % e SYSTEM \"e.dtd\">\r\n<!ELEMENT doc (#PCDATA)>\r\n]>\r\n<doc></doc>\r\n",
  11,
  "<doc></doc>",
  "Tests that an external PE may be defined (but not referenced)."
},
{
  "valid/sa/083.xml",
  101,
  "<!DOCTYPE doc [\r\n<!ENTITY % e PUBLIC 'whatever' \"e.dtd\">\r\n<!ELEMENT doc (#PCDATA)>\r\n]>\r\n<doc></doc>\r\n",
  11,
  "<doc></doc>",
  "Tests that an external PE may be defined (but not referenced)."
},
{
  "valid/sa/084.xml",
  54,
  "<!DOCTYPE doc [<!ELEMENT doc (#PCDATA)>]><doc></doc>\r\n",
  11,
  "<doc></doc>",
  "Test demonstrates that although whitespace can be used to set apart markup for greater readability it is not necessary."
},
{
  "valid/sa/085.xml",
  102,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n<!ENTITY % e \"<foo>\">\r\n<!ENTITY e \"\">\r\n]>\r\n<doc>&e;</doc>\r\n",
  11,
  "<doc></doc>",
  "Parameter and General entities use different namespaces, so there can be an entity of each type with a given name."
},
{
  "valid/sa/086.xml",
  100,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n<!ENTITY e \"\">\r\n<!ENTITY e \"<foo>\">\r\n]>\r\n<doc>&e;</doc>\r\n",
  11,
  "<doc></doc>",
  "Tests whether entities may be declared more than once, with the first declaration being the binding one."
},
{
  "valid/sa/087.xml",
  107,
  "<!DOCTYPE doc [\r\n<!ENTITY e \"<foo/&#62;\">\r\n<!ELEMENT doc (foo)>\r\n<!ELEMENT foo EMPTY>\r\n]>\r\n<doc>&e;</doc>\r\n",
  22,
  "<doc><foo></foo></doc>",
  "Tests whether character references in internal entities are expanded early enough, by relying on correct handling to make the entity be well formed."
},
{
  "valid/sa/088.xml",
  87,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n<!ENTITY e \"&lt;foo>\">\r\n]>\r\n<doc>&e;</doc>\r\n",
  22,
  "<doc>&lt;foo&gt;</doc>",
  "Tests whether entity references in internal entities are expanded late enough, by relying on correct handling to make the expanded text be valid. (If it's expanded too early, the entity will parse as an element that's not valid in that context.)"
},
{
  "valid/sa/089.xml",
  108,
  "<!DOCTYPE doc [\r\n<!ENTITY e \"&#x10000;&#x10FFFD;&#x10FFFF;\">\r\n<!ELEMENT doc (#PCDATA)>\r\n]>\r\n<doc>&e;</doc>\r\n",
  23,
  "<doc>\360\220\200\200\364\217\277\275\364\217\277\277</doc>\277",
  "Tests entity expansion of three legal character references, which each expand to a Unicode surrogate pair."
},
{
  "valid/sa/090.xml",
  150,
  "<!DOCTYPE doc [\r\n<!ATTLIST e a NOTATION (n) #IMPLIED>\r\n<!ELEMENT doc (e)*>\r\n<!ELEMENT e (#PCDATA)>\r\n<!NOTATION n PUBLIC \"whatever\">\r\n]>\r\n<doc></doc>\r\n",
  62,
  "<!DOCTYPE doc [\n<!NOTATION n PUBLIC 'whatever'>\n]>\n<doc></doc>",
  "Verifies that an XML parser will parse a NOTATION attribute; the output phase of this test ensures that the notation is reported to the application."
},
{
  "valid/sa/091.xml",
  181,
  "<!DOCTYPE doc [\r\n<!NOTATION n SYSTEM \"http://www.w3.org/\">\r\n<!ENTITY e SYSTEM \"http://www.w3.org/\" NDATA n>\r\n<!ELEMENT doc (#PCDATA)>\r\n<!ATTLIST doc a ENTITY \"e\">\r\n]>\r\n<doc></doc>\r\n",
  78,
  "<!DOCTYPE doc [\n<!NOTATION n SYSTEM 'http://www.w3.org/'>\n]>\n<doc a=\"e\"></doc>",
  "Verifies that an XML parser will parse an ENTITY attribute; the output phase of this test ensures that the notation is reported to the application, and for validating parsers it further tests that the entity is so reported."
},
{
  "valid/sa/092.xml",
  102,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (a)*>\r\n<!ELEMENT a EMPTY>\r\n]>\r\n<doc>\r\n<a/>\r\n    <a/>\t<a/>\r\n\r\n\r\n</doc>\r\n",
  65,
  "<doc>&#10;<a></a>&#10;    <a></a>&#9;<a></a>&#10;&#10;&#10;</doc>",
  "Test demostrates that extra whitespace is normalized into a single space character."
},
{
  "valid/sa/093.xml",
  59,
  "<!DOCTYPE doc [\n<!ELEMENT doc (#PCDATA)>\n]>\n<doc>\n\n\n</doc>\n",
  26,
  "<doc>&#10;&#10;&#10;</doc>",
  "Test demonstrates that extra whitespace is not intended for inclusion in the delivered version of the document."
},
{
  "valid/sa/094.xml",
  112,
  "<!DOCTYPE doc [\r\n<!ENTITY % e \"foo\">\r\n<!ELEMENT doc (#PCDATA)>\r\n<!ATTLIST doc a1 CDATA \"%e;\">\r\n]>\r\n<doc></doc>\r\n",
  20,
  "<doc a1=\"%e;\"></doc>",
  "Attribute defaults with a DTD have special parsing rules, different from other strings. That means that characters found there may look like an undefined parameter entity reference \"within a markup declaration\", but they aren't ... so they can't be violating the PEs in Internal Subset WFC."
},
{
  "valid/sa/095.xml",
  141,
  "<!DOCTYPE doc [\r\n<!ATTLIST doc a1 CDATA #IMPLIED>\r\n<!ATTLIST doc a1 NMTOKENS #IMPLIED>\r\n<!ELEMENT doc (#PCDATA)>\r\n]>\r\n<doc a1=\"1  2\"></doc>\r\n",
  21,
  "<doc a1=\"1  2\"></doc>",
  "Basically an output test, this requires extra whitespace to be normalized into a single space character in an attribute of type NMTOKENS."
},
{
  "valid/sa/096.xml",
  99,
  "<!DOCTYPE doc [\r\n<!ATTLIST doc a1 NMTOKENS \" 1  \t2 \t\">\r\n<!ELEMENT doc (#PCDATA)>\r\n]>\r\n<doc></doc>\r\n",
  20,
  "<doc a1=\"1 2\"></doc>",
  "Test demonstrates that extra whitespace is normalized into a single space character in an attribute of type NMTOKENS."
},
{
  "valid/sa/097.xml",
  157,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n<!ENTITY % e SYSTEM \"097.ent\">\r\n<!ATTLIST doc a1 CDATA \"v1\">\r\n%e;\r\n<!ATTLIST doc a2 CDATA \"v2\">\r\n]>\r\n<doc></doc>\r\n",
  19,
  "<doc a1=\"v1\"></doc>",
  "Basically an output test, this tests whether an externally defined attribute declaration (with a default) takes proper precedence over a subsequent internal declaration."
},
{
  "valid/sa/098.xml",
  71,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n]>\r\n<doc><?pi x\r\ny?></doc>\r\n",
  21,
  "<doc><?pi x\ny?></doc>",
  "Test demonstrates that extra whitespace within a processing instruction is converted into a single space character."
},
{
  "valid/sa/099.xml",
  100,
  "<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n]>\r\n<doc></doc>\r\n",
  11,
  "<doc></doc>",
  "Test demonstrates the name of the encoding can be composed of lowercase characters."
},
{
  "valid/sa/100.xml",
  101,
  "<!DOCTYPE doc [\r\n<!ENTITY e PUBLIC \";!*#@$_%\" \"100.xml\">\r\n<!ELEMENT doc (#PCDATA)>\r\n]>\r\n<doc></doc>\r\n",
  11,
  "<doc></doc>",
  "Makes sure that PUBLIC identifiers may have some strange characters. NOTE: The XML editors have said that the XML specification errata will specify that parameter entity expansion does not occur in PUBLIC identifiers, so that the '%' character will not flag a malformed parameter entity reference."
},
{
  "valid/sa/101.xml",
  81,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n<!ENTITY e \"&#34;\">\r\n]>\r\n<doc></doc>\r\n",
  11,
  "<doc></doc>",
  "This tests whether entity expansion is (incorrectly) done while processing entity declarations; if it is, the entity value literal will terminate prematurely."
},
{
  "valid/sa/102.xml",
  103,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n<!ATTLIST doc a CDATA #IMPLIED>\r\n]>\r\n<doc a=\"&#34;\"></doc>\r\n",
  22,
  "<doc a=\"&quot;\"></doc>",
  "Test demonstrates that a CDATA attribute can pass a double quote as its value."
},
{
  "valid/sa/103.xml",
  69,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n]>\r\n<doc>&#60;doc></doc>\r\n",
  22,
  "<doc>&lt;doc&gt;</doc>",
  "Test demonstrates that an attribute can pass a less than sign as its value."
},
{
  "valid/sa/104.xml",
  101,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n<!ATTLIST doc a CDATA #IMPLIED>\r\n]>\r\n<doc a=\"x\ty\"></doc>\r\n",
  19,
  "<doc a=\"x y\"></doc>",
  "Test demonstrates that extra whitespace within an Attribute of a Start-tag is normalized to a single space character."
},
{
  "valid/sa/105.xml",
  104,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n<!ATTLIST doc a CDATA #IMPLIED>\r\n]>\r\n<doc a=\"x&#9;y\"></doc>\r\n",
  22,
  "<doc a=\"x&#9;y\"></doc>",
  "Basically an output test, this requires a CDATA attribute with a tab character to be passed through as one space."
},
{
  "valid/sa/106.xml",
  105,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n<!ATTLIST doc a CDATA #IMPLIED>\r\n]>\r\n<doc a=\"x&#10;y\"></doc>\r\n",
  23,
  "<doc a=\"x&#10;y\"></doc>",
  "Basically an output test, this requires a CDATA attribute with a newline character to be passed through as one space."
},
{
  "valid/sa/107.xml",
  105,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n<!ATTLIST doc a CDATA #IMPLIED>\r\n]>\r\n<doc a=\"x&#13;y\"></doc>\r\n",
  23,
  "<doc a=\"x&#13;y\"></doc>",
  "Basically an output test, this requires a CDATA attribute with a return character to be passed through as one space."
},
{
  "valid/sa/108.xml",
  121,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n<!ENTITY e \"\r\n\">\r\n<!ATTLIST doc a CDATA #IMPLIED>\r\n]>\r\n<doc a=\"x&e;y\"></doc>\r\n",
  19,
  "<doc a=\"x y\"></doc>",
  "This tests normalization of end-of-line characters (CRLF) within entities to LF, primarily as an output test."
},
{
  "valid/sa/109.xml",
  98,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n<!ATTLIST doc a CDATA #IMPLIED>\r\n]>\r\n<doc a=\"\"></doc>\r\n",
  16,
  "<doc a=\"\"></doc>",
  "Test demonstrates that an attribute can have a null value."
},
{
  "valid/sa/110.xml",
  129,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n<!ENTITY e \"&#13;&#10;\">\r\n<!ATTLIST doc a CDATA #IMPLIED>\r\n]>\r\n<doc a=\"x&e;y\"></doc>\r\n",
  20,
  "<doc a=\"x  y\"></doc>",
  "Basically an output test, this requires that a CDATA attribute with a CRLF be normalized to one space."
},
{
  "valid/sa/111.xml",
  123,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n<!ATTLIST doc a NMTOKENS #IMPLIED>\r\n]>\r\n<doc a=\"&#32;x&#32;&#32;y&#32;\"></doc>\r\n",
  19,
  "<doc a=\"x y\"></doc>",
  "Character references expanding to spaces doesn't affect treatment of attributes."
},
{
  "valid/sa/112.xml",
  89,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (a | b)>\r\n<!ELEMENT a (#PCDATA)>\r\n]>\r\n<doc><a></a></doc>\r\n",
  18,
  "<doc><a></a></doc>",
  "Test demonstrates shows the use of content particles within the element content."
},
{
  "valid/sa/113.xml",
  91,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n<!ATTLIST e a CDATA #IMPLIED>\r\n]>\r\n<doc></doc>\r\n",
  11,
  "<doc></doc>",
  "Test demonstrates that it is not an error to have attributes declared for an element not itself declared."
},
{
  "valid/sa/114.xml",
  96,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n<!ENTITY e \"<![CDATA[&foo;]]>\">\r\n]>\r\n<doc>&e;</doc>\r\n",
  20,
  "<doc>&amp;foo;</doc>",
  "Test demonstrates that all text within a valid CDATA section is considered text and not recognized as markup."
},
{
  "valid/sa/115.xml",
  103,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n<!ENTITY e1 \"&e2;\">\r\n<!ENTITY e2 \"v\">\r\n]>\r\n<doc>&e1;</doc>\r\n",
  12,
  "<doc>v</doc>",
  "Test demonstrates that an entity reference is processed by recursively processing the replacement text of the entity."
},
{
  "valid/sa/116.xml",
  74,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n]>\r\n<doc><![CDATA[\r\n]]></doc>\r\n",
  16,
  "<doc>&#10;</doc>",
  "Test demonstrates that a line break within CDATA will be normalized."
},
{
  "valid/sa/117.xml",
  86,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n<!ENTITY rsqb \"]\">\r\n]>\r\n<doc>&rsqb;</doc>\r\n",
  12,
  "<doc>]</doc>",
  "Test demonstrates that entity expansion is done while processing entity declarations."
},
{
  "valid/sa/118.xml",
  87,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n<!ENTITY rsqb \"]]\">\r\n]>\r\n<doc>&rsqb;</doc>\r\n",
  13,
  "<doc>]]</doc>",
  "Test demonstrates that entity expansion is done while processing entity declarations."
},
{
  "valid/sa/119.xml",
  66,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc ANY>\r\n]>\r\n<doc><!-- -\303\241 --></doc>\r\n\241",
  11,
  "<doc></doc>",
  "Comments may contain any legal XML characters; only the string \"--\" is disallowed."
},
{
  "valid/not-sa/001.xml",
  73,
  "<!DOCTYPE doc SYSTEM \"001.ent\" [\r\n<!ELEMENT doc EMPTY>\r\n]>\r\n<doc></doc>\r\n",
  11,
  "<doc></doc>",
  "Test demonstrates the use of an ExternalID within a document type definition."
},
{
  "valid/not-sa/002.xml",
  73,
  "<!DOCTYPE doc SYSTEM \"002.ent\" [\r\n<!ELEMENT doc EMPTY>\r\n]>\r\n<doc></doc>\r\n",
  11,
  "<doc></doc>",
  "Test demonstrates the use of an ExternalID within a document type definition."
},
{
  "valid/not-sa/003.xml",
  48,
  "<!DOCTYPE doc SYSTEM \"003-1.ent\">\r\n<doc></doc>\r\n",
  19,
  "<doc a1=\"v1\"></doc>",
  "Test demonstrates the expansion of an external parameter entity that declares an attribute."
},
{
  "valid/not-sa/004.xml",
  48,
  "<!DOCTYPE doc SYSTEM \"004-1.ent\">\r\n<doc></doc>\r\n",
  22,
  "<doc a1=\"value\"></doc>",
  "Expands an external parameter entity in two different ways, with one of them declaring an attribute."
},
{
  "valid/not-sa/005.xml",
  48,
  "<!DOCTYPE doc SYSTEM \"005-1.ent\">\r\n<doc></doc>\r\n",
  19,
  "<doc a1=\"v1\"></doc>",
  "Test demonstrates the expansion of an external parameter entity that declares an attribute."
},
{
  "valid/not-sa/006.xml",
  81,
  "<!DOCTYPE doc SYSTEM \"006.ent\" [\r\n<!ATTLIST doc a1 CDATA \"v1\">\r\n]>\r\n<doc></doc>\r\n",
  27,
  "<doc a1=\"v1\" a2=\"w2\"></doc>",
  "Test demonstrates that when more than one definition is provided for the same attribute of a given element type only the first declaration is binding."
},
{
  "valid/not-sa/007.xml",
  46,
  "<!DOCTYPE doc SYSTEM \"007.ent\">\r\n<doc></doc>\r\n",
  19,
  "<doc a1=\"v1\"></doc>",
  "Test demonstrates the use of an Attribute list declaration within an external entity."
},
{
  "valid/not-sa/008.xml",
  57,
  "<!DOCTYPE doc PUBLIC \"whatever\" \"008.ent\">\r\n<doc></doc>\r\n",
  19,
  "<doc a1=\"v1\"></doc>",
  "Test demonstrates that an external identifier may include a public identifier."
},
{
  "valid/not-sa/009.xml",
  92,
  "<!DOCTYPE doc PUBLIC \"whatever\" \"009.ent\" [\r\n<!ATTLIST doc a2 CDATA \"v2\">\r\n]>\r\n<doc></doc>\r\n",
  27,
  "<doc a1=\"v1\" a2=\"v2\"></doc>",
  "Test demonstrates that an external identifier may include a public identifier."
},
{
  "valid/not-sa/010.xml",
  81,
  "<!DOCTYPE doc SYSTEM \"010.ent\" [\r\n<!ATTLIST doc a1 CDATA \"v1\">\r\n]>\r\n<doc></doc>\r\n",
  19,
  "<doc a1=\"v1\"></doc>",
  "Test demonstrates that when more that one definition is provided for the same attribute of a given element type only the first declaration is binding."
},
{
  "valid/not-sa/011.xml",
  71,
  "<!DOCTYPE doc [\r\n<!ENTITY % e SYSTEM \"011.ent\">\r\n%e;\r\n]>\r\n<doc></doc>\r\n",
  19,
  "<doc a1=\"v1\"></doc>",
  "Test demonstrates a parameter entity declaration whose parameter entity definition is an ExternalID."
},
{
  "valid/not-sa/012.xml",
  71,
  "<!DOCTYPE doc [\r\n<!ENTITY % e SYSTEM \"012.ent\">\r\n%e;\r\n]>\r\n<doc></doc>\r\n",
  19,
  "<doc a1=\"v1\"></doc>",
  "Test demonstrates an enternal parsed entity that begins with a text declaration."
},
{
  "valid/not-sa/013.xml",
  46,
  "<!DOCTYPE doc SYSTEM \"013.ent\">\r\n<doc></doc>\r\n",
  19,
  "<doc a1=\"v1\"></doc>",
  "Test demonstrates the use of the conditional section INCLUDE that will include its contents as part of the DTD."
},
{
  "valid/not-sa/014.xml",
  76,
  "<!DOCTYPE doc SYSTEM \"014.ent\" [\r\n<!ENTITY % e \"INCLUDE\">\r\n]>\r\n<doc></doc>\r\n",
  19,
  "<doc a1=\"v1\"></doc>",
  "Test demonstrates the use of the conditional section INCLUDE that will include its contents as part of the DTD. The keyword is a parameter-entity reference."
},
{
  "valid/not-sa/015.xml",
  75,
  "<!DOCTYPE doc SYSTEM \"015.ent\" [\r\n<!ENTITY % e \"IGNORE\">\r\n]>\r\n<doc></doc>\r\n",
  19,
  "<doc a2=\"v2\"></doc>",
  "Test demonstrates the use of the conditonal section IGNORE the will ignore its content from being part of the DTD. The keyword is a parameter-entity reference."
},
{
  "valid/not-sa/016.xml",
  76,
  "<!DOCTYPE doc SYSTEM \"016.ent\" [\r\n<!ENTITY % e \"INCLUDE\">\r\n]>\r\n<doc></doc>\r\n",
  19,
  "<doc a1=\"v1\"></doc>",
  "Test demonstrates the use of the conditional section INCLUDE that will include its contents as part of the DTD. The keyword is a parameter-entity reference."
},
{
  "valid/not-sa/017.xml",
  46,
  "<!DOCTYPE doc SYSTEM \"017.ent\">\r\n<doc></doc>\r\n",
  19,
  "<doc a1=\"v1\"></doc>",
  "Test demonstrates a parameter entity declaration that contains an attribute list declaration."
},
{
  "valid/not-sa/018.xml",
  46,
  "<!DOCTYPE doc SYSTEM \"018.ent\">\r\n<doc></doc>\r\n",
  19,
  "<doc a1=\"v1\"></doc>",
  "Test demonstrates an EnternalID whose contents contain an parameter entity declaration and a attribute list definition."
},
{
  "valid/not-sa/019.xml",
  46,
  "<!DOCTYPE doc SYSTEM \"019.ent\">\r\n<doc></doc>\r\n",
  19,
  "<doc a1=\"v1\"></doc>",
  "Test demonstrates that a parameter entity will be expanded with spaces on either side."
},
{
  "valid/not-sa/020.xml",
  46,
  "<!DOCTYPE doc SYSTEM \"020.ent\">\r\n<doc></doc>\r\n",
  19,
  "<doc a1=\"v1\"></doc>",
  "Parameter entities expand with spaces on either side."
},
{
  "valid/not-sa/021.xml",
  46,
  "<!DOCTYPE doc SYSTEM \"021.ent\">\r\n<doc></doc>\r\n",
  19,
  "<doc a1=\"v1\"></doc>",
  "Test demonstrates a parameter entity declaration that contains a partial attribute list declaration."
},
{
  "valid/not-sa/023.xml",
  46,
  "<!DOCTYPE doc SYSTEM \"023.ent\">\r\n<doc></doc>\r\n",
  19,
  "<doc a1=\"v1\"></doc>",
  "Test demonstrates the use of a parameter entity reference within an attribute list declaration."
},
{
  "valid/not-sa/024.xml",
  46,
  "<!DOCTYPE doc SYSTEM \"024.ent\">\r\n<doc></doc>\r\n",
  19,
  "<doc a1=\"v1\"></doc>",
  "Constructs an <!ATTLIST...> declaration from several PEs."
},
{
  "valid/not-sa/025.xml",
  46,
  "<!DOCTYPE doc SYSTEM \"025.ent\">\r\n<doc></doc>\r\n",
  18,
  "<doc a1=\"x\"></doc>",
  "Test demonstrates that when more that one definition is provided for the same entity only the first declaration is binding."
},
{
  "valid/not-sa/026.xml",
  135,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc ANY>\r\n<!ENTITY % e SYSTEM \"026.ent\">\r\n%e;\r\n<!ATTLIST doc a1 CDATA \"x1\" a2 CDATA \"x2\">\r\n]>\r\n<doc></doc>\r\n",
  27,
  "<doc a1=\"w1\" a2=\"x2\"></doc>",
  "Test demonstrates that when more that one definition is provided for the same attribute of a given element type only the first declaration is binding."
},
{
  "valid/not-sa/027.xml",
  46,
  "<!DOCTYPE doc SYSTEM \"027.ent\">\r\n<doc></doc>\r\n",
  11,
  "<doc></doc>",
  "Test demonstrates a parameter entity reference whose value is NULL."
},
{
  "valid/not-sa/028.xml",
  46,
  "<!DOCTYPE doc SYSTEM \"028.ent\">\r\n<doc></doc>\r\n",
  19,
  "<doc a1=\"v1\"></doc>",
  "Test demonstrates the use of the conditional section INCLUDE that will include its contents."
},
{
  "valid/not-sa/029.xml",
  46,
  "<!DOCTYPE doc SYSTEM \"029.ent\">\r\n<doc></doc>\r\n",
  19,
  "<doc a1=\"v2\"></doc>",
  "Test demonstrates the use of the conditonal section IGNORE the will ignore its content from being used."
},
{
  "valid/not-sa/030.xml",
  46,
  "<!DOCTYPE doc SYSTEM \"030.ent\">\r\n<doc></doc>\r\n",
  11,
  "<doc></doc>",
  "Test demonstrates the use of the conditonal section IGNORE the will ignore its content from being used."
},
{
  "valid/not-sa/031.xml",
  51,
  "<!DOCTYPE doc SYSTEM \"031-1.ent\">\r\n<doc>&e;</doc>\r\n",
  60,
  "<doc>&lt;!ATTLIST doc a1 CDATA &quot;v1&quot;&gt;&#10;</doc>",
  "Expands a general entity which contains a CDATA section with what looks like a markup declaration (but is just text since it's in a CDATA section)."
},
{
  "valid/ext-sa/001.xml",
  93,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n<!ENTITY e SYSTEM \"001.ent\">\r\n]>\r\n<doc>&e;</doc>\r\n",
  20,
  "<doc>Data&#10;</doc>",
  "A combination of carriage return line feed in an external entity must be normalized to a single newline."
},
{
  "valid/ext-sa/002.xml",
  93,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n<!ENTITY e SYSTEM \"002.ent\">\r\n]>\r\n<doc>&e;</doc>\r\n",
  15,
  "<doc>Data</doc>",
  "A carriage return (also CRLF) in an external entity must be normalized to a single newline."
},
{
  "valid/ext-sa/003.xml",
  93,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n<!ENTITY e SYSTEM \"003.ent\">\r\n]>\r\n<doc>&e;</doc>\r\n",
  11,
  "<doc></doc>",
  "Test demonstrates that the content of an element can be empty. In this case the external entity is an empty file."
},
{
  "valid/ext-sa/004.xml",
  93,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n<!ENTITY e SYSTEM \"004.ent\">\r\n]>\r\n<doc>&e;</doc>\r\n",
  20,
  "<doc>Data&#10;</doc>",
  "A carriage return (also CRLF) in an external entity must be normalized to a single newline."
},
{
  "valid/ext-sa/005.xml",
  108,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (e*)>\r\n<!ELEMENT e EMPTY>\r\n<!ENTITY e SYSTEM \"005.ent\">\r\n]>\r\n<doc>&e;</doc>\r\n",
  32,
  "<doc><e></e><e></e><e></e></doc>",
  "Test demonstrates the use of optional character and content particles within an element content. The test also show the use of external entity."
},
{
  "valid/ext-sa/006.xml",
  116,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA|e)*>\r\n<!ELEMENT e EMPTY>\r\n<!ENTITY e SYSTEM \"006.ent\">\r\n]>\r\n<doc>&e;</doc>\r\n",
  58,
  "<doc>Data&#10;<e></e>&#10;More data&#10;<e></e>&#10;</doc>",
  "Test demonstrates the use of optional character and content particles within mixed element content. The test also shows the use of an external entity and that a carriage control line feed in an external entity must be normalized to a single newline."
},
{
  "valid/ext-sa/007.xml",
  95,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n<!ENTITY e SYSTEM \"007.ent\">\r\n]>\r\n<doc>X&e;Z</doc>\r\n",
  14,
  "<doc>XYZ</doc>",
  "Test demonstrates the use of external entity and how replacement text is retrieved and processed."
},
{
  "valid/ext-sa/008.xml",
  95,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n<!ENTITY e SYSTEM \"008.ent\">\r\n]>\r\n<doc>X&e;Z</doc>\r\n",
  14,
  "<doc>XYZ</doc>",
  "Test demonstrates the use of external entity and how replacement text is retrieved and processed. Also tests the use of an EncodingDecl of UTF-16."
},
{
  "valid/ext-sa/009.xml",
  93,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n<!ENTITY e SYSTEM \"009.ent\">\r\n]>\r\n<doc>&e;</doc>\r\n",
  16,
  "<doc>&#10;</doc>",
  "A carriage return (also CRLF) in an external entity must be normalized to a single newline."
},
{
  "valid/ext-sa/011.xml",
  123,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n<!ENTITY e PUBLIC \"a not very interesting file\" \"011.ent\">\r\n]>\r\n<doc>&e;</doc>\r\n",
  21,
  "<doc>xyzzy&#10;</doc>",
  "Test demonstrates the use of a public identifier with and external entity. The test also show that a carriage control line feed combination in an external entity must be normalized to a single newline."
},
{
  "valid/ext-sa/012.xml",
  179,
  "<!DOCTYPE doc [\r\n<!ENTITY e1 \"&e2;\">\r\n<!ENTITY e2 \"&e3;\">\r\n<!ENTITY e3 SYSTEM \"012.ent\">\r\n<!ENTITY e4 \"&e5;\">\r\n<!ENTITY e5 \"(e5)\">\r\n<!ELEMENT doc (#PCDATA)>\r\n]>\r\n<doc>&e1;</doc>\r\n",
  15,
  "<doc>(e5)</doc>",
  "Test demonstrates both internal and external entities and that processing of entity references may be required to produce the correct replacement text."
},
{
  "valid/ext-sa/013.xml",
  180,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (e)>\r\n<!ELEMENT e (#PCDATA)>\r\n<!ATTLIST e\r\n  a1 CDATA \"a1 default\"\r\n  a2 NMTOKENS \"a2 default\"\r\n>\r\n<!ENTITY x SYSTEM \"013.ent\">\r\n]>\r\n<doc>&x;</doc>\r\n",
  50,
  "<doc><e a1=\"a1 default\" a2=\"a2 default\"></e></doc>",
  "Test demonstrates that whitespace is handled by adding a single whitespace to the normalized value in the attribute list."
},
{
  "valid/ext-sa/014.xml",
  93,
  "<!DOCTYPE doc [\r\n<!ELEMENT doc (#PCDATA)>\r\n<!ENTITY e SYSTEM \"014.ent\">\r\n]>\r\n<doc>&e;</doc>\r\n",
  18,
  "<doc>\357\273\277data</doc>\277",
  "Test demonstrates use of characters outside of normal ASCII range."
},
