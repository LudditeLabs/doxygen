// Since Python may define some pre-processor definitions which affect
// the standard headers on some systems, you must include Python.h before any
// standard headers are included.
#include "Python.h"
#include <cstdlib>
#include "qdir.h"
#include "qfileinfo.h"
#include "qtextcodec.h"
#include "qcstringlist.h"
#include "doxygen.h"
#include "sqlite3gen.h"
#include "config.h"
#include "configimpl.h"
#include "searchindex.h"
#include "store.h"
#include "message.h"
#include "autodoc/common/context.h"
#include "autodoc/common/utils.h"

#define OPTPARSE_IMPLEMENTATION
#define OPTPARSE_API static
#include "autodoc/optparse/optparse.h"


/** Init configuration. */
void init_config()
{
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

    // TODO: create temp dir in platform-independent way.
    ConfigImpl_getString("OUTPUT_DIRECTORY") = QCString("/tmp/contentdb");
}
//-----------------------------------------------------------------------------


void print_short_usage(const char *prog)
{
    QFileInfo info(prog);
    msg("USAGE: %s [OPTIONS] PATH...\n", info.fileName().data());
}
//-----------------------------------------------------------------------------


void print_usage(const char *prog)
{
    QFileInfo info(prog);

    msg("Tool to generate content database from program sources.\n\n");
    msg("USAGE: %s [OPTIONS] PATH...\n", info.fileName().data());
    msg("\n");
    msg("OPTIONS:\n");
    msg("  --help,-h            Print this help message.\n");
    msg("  --tmp,-T <path>      Temporary directory.\n");
    msg("  --exclude,-e <path>  Files and/or dirs to exclude from processing.\n");
    msg("                       Relative paths are relative to current working\n");
    msg("                       directory.\n");
    msg("  --exclude-pattern,-x <pattern>\n");
    msg("                       Exclude wildcard pattern.\n");
    msg("  --file-patterns,-p <pattern>\n");
    msg("                       File wildcard patterns separated with semicolon.\n");
    msg("  --out,-o             Output content DB filename.\n");
}
//-----------------------------------------------------------------------------

int parse_cli(autodoc::Context *context, char **argv)
{
    int option;
    struct optparse options;
    struct optparse_long longopts[] = {
        {"help",            'h', OPTPARSE_NONE},
        {"tmp",             'T', OPTPARSE_REQUIRED},
        {"exclude",         'e', OPTPARSE_REQUIRED},
        {"exclude-pattern", 'x', OPTPARSE_REQUIRED},
        {"file-patterns",   'p', OPTPARSE_REQUIRED},
        {"out",             'o', OPTPARSE_REQUIRED},
        {0}
    };

    // Parse command line args.

    optparse_init(&options, argv);
    while ((option = optparse_long(&options, longopts, NULL)) != -1) {
        switch (option) {
        case 'h':
            print_usage(argv[0]);
            return EXIT_SUCCESS;
        case 'T':
            ConfigImpl_getString("OUTPUT_DIRECTORY") = options.optarg;
            break;
        case 'e':
            // Note: relative paths are relative to the directory from which doxygen is run.
            ConfigImpl_getList("EXCLUDE").append(options.optarg);
            break;
        case 'x':
            ConfigImpl_getList("EXCLUDE_PATTERNS").append(options.optarg);
            break;
        case 'p':
        {
            QCStringList lst = QCStringList::split(';', options.optarg);
            auto &file_patterns = ConfigImpl_getList("FILE_PATTERNS");
            file_patterns.clear();
            for (auto &v: lst)
                file_patterns.append(v.data());
            break;
        }
        case 'o':
            context->setContentDbFilename(options.optarg);
            break;
        case '?':
            fprintf(stderr, "%s: %s\n", argv[0], options.errmsg);
            return EXIT_FAILURE;
        }
    }

    QCString filename = autodocCtx()->contentDbFilename();
    if (filename.isEmpty())
    {
        QDir dir(ConfigImpl_getString("OUTPUT_DIRECTORY"));
        filename = dir.filePath("content.db").data();
        autodocCtx()->setContentDbFilename(filename);
    }

    // Remaining args.
    char *arg;
    QStrList &inputs = ConfigImpl_getList("INPUT");
    while ((arg = optparse_arg(&options)))
        inputs.append(arg);

    return -1;
}
//-----------------------------------------------------------------------------

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        print_short_usage(argv[0]);
        exit(1);
    }

    autodoc::Context *context = autodoc::Context::instance();
    autodoc::PyInitHelper pyinit;

    initDoxygen();
    init_config();

    int res = parse_cli(context, argv);
    if (res != -1)
        exit(res);

    // TODO: don't remove in update mode!
    context->removeDbFile();

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
