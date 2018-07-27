// Since Python may define some pre-processor definitions which affect
// the standard headers on some systems, you must include Python.h before any
// standard headers are included.
#include "Python.h"
#include <cstdlib>
#include "qdir.h"
#include "qfileinfo.h"
#include "qtextcodec.h"
#include "doxygen.h"
#include "sqlite3gen.h"
#include "config.h"
#include "configimpl.h"
#include "searchindex.h"
#include "store.h"
#include "autodoc/common/utils.h"


int main(int argc, char **argv)
{
    if (argc<2)
    {
        printf("Usage: %s [source_file | source_dir]\n", argv[0]);
        exit(1);
    }

    autodoc::PyInitHelper pyinit;

    initDoxygen();

    Config::init();

    // disable html output
    ConfigImpl_getBool("GENERATE_HTML") = FALSE;

    // disable latex output
    ConfigImpl_getBool("GENERATE_LATEX") = FALSE;
    ConfigImpl_getBool("GENERATE_DOCSET") = FALSE;
    ConfigImpl_getBool("GENERATE_HTMLHELP") = FALSE;
    ConfigImpl_getBool("GENERATE_CHI") = FALSE;
    ConfigImpl_getBool("GENERATE_QHP") = FALSE;
    ConfigImpl_getBool("GENERATE_ECLIPSEHELP") = FALSE;
    ConfigImpl_getBool("GENERATE_TODOLIST") = FALSE;
    ConfigImpl_getBool("GENERATE_TESTLIST") = FALSE;
    ConfigImpl_getBool("GENERATE_BUGLIST") = FALSE;
    ConfigImpl_getBool("GENERATE_DEPRECATEDLIST") = FALSE;
    ConfigImpl_getBool("GENERATE_HTML") = FALSE;
    ConfigImpl_getBool("GENERATE_RTF") = FALSE;
    ConfigImpl_getBool("GENERATE_MAN") = FALSE;
    ConfigImpl_getBool("GENERATE_XML") = FALSE;
    ConfigImpl_getBool("GENERATE_DOCBOOK") = FALSE;
    ConfigImpl_getBool("GENERATE_AUTOGEN_DEF") = FALSE;
    ConfigImpl_getBool("GENERATE_PERLMOD") = FALSE;
    ConfigImpl_getBool("GENERATE_LEGEND") = FALSE;

    ConfigImpl_getBool("RECURSIVE") = TRUE;


    // If the INLINE_INHERITED_MEMB tag is set to YES, doxygen will show all
    // inherited members of a class in the documentation of that class as if those
    // members were ordinary class members. Constructors, destructors and assignment
    // operators of the base classes will not be shown.
    ConfigImpl_getBool("INLINE_INHERITED_MEMB") = FALSE;


    // If the JAVADOC_AUTOBRIEF tag is set to YES then doxygen will interpret the
    // first line (until the first dot) of a Javadoc-style comment as the brief
    // description. If set to NO, the Javadoc-style will behave just like regular Qt-
    // style comments (thus requiring an explicit @brief command for a brief
    // description.)
    // The default value is: NO.
    ConfigImpl_getBool("JAVADOC_AUTOBRIEF") = FALSE;

    // If the INHERIT_DOCS tag is set to YES then an undocumented member inherits the
    // documentation from any documented member that it re-implements.
    // The default value is: YES.
    ConfigImpl_getBool("INHERIT_DOCS") = TRUE;

    // If the MARKDOWN_SUPPORT tag is enabled then doxygen pre-processes all comments
    // according to the Markdown format, which allows for more readable
    // documentation. See http://daringfireball.net/projects/markdown/ for details.
    // The output of markdown processing is further processed by doxygen, so you can
    // mix doxygen, HTML, and XML commands with Markdown formatting. Disable only in
    // case of backward compatibilities issues.
    ConfigImpl_getBool("MARKDOWN_SUPPORT") = TRUE;

    // be quiet
    ConfigImpl_getBool("QUIET") = FALSE;

    // turn off warnings
    ConfigImpl_getBool("WARNINGS") = FALSE;
    ConfigImpl_getBool("WARN_IF_UNDOCUMENTED") = FALSE;
    ConfigImpl_getBool("WARN_IF_DOC_ERROR") = FALSE;

    // Extract as much as possible
    ConfigImpl_getBool("EXTRACT_ALL") = TRUE;
    ConfigImpl_getBool("EXTRACT_STATIC") = TRUE;
    ConfigImpl_getBool("EXTRACT_PRIVATE") = TRUE;
    ConfigImpl_getBool("EXTRACT_LOCAL_METHODS") = TRUE;

    // Extract source browse information, needed
    // to make doxygen gather the cross reference info
    ConfigImpl_getBool("SOURCE_BROWSER") = TRUE;

    // set the input
    ConfigImpl_getList("INPUT").append(argv[1]);

    printf("set OUTPUT_DIRECTORY\n");
    ConfigImpl_getString("OUTPUT_DIRECTORY") = QCString("/tmp/autodocdb");

    QDir dir("/tmp/autodocdb");
    dir.remove("doxygen_sqlite3.db");

    checkConfiguration();
    adjustConfiguration();

    parseInput();

    generateSqlite3();

    // From generateOutput() - doxygen.cpp
    cleanUpDoxygen();
    finializeSearchIndexer();
    Doxygen::symbolStorage->close();
    QDir thisDir;
    thisDir.remove(Doxygen::objDBFileName);
    Config::deinit();
    // QTextCodec::deleteAllCodecs();
    delete Doxygen::symbolMap;
    delete Doxygen::clangUsrMap;
    delete Doxygen::symbolStorage;

    return 0;
}
