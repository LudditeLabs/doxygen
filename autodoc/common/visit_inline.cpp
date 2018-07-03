#include "Python.h"
#include "autodoc/common/visitor.h"
#include <memory>
#include "docparser.h"
#include "htmlentity.h"
#include "autodoc/common/pydocutilstree.h"


void PyDocVisitor::visit(DocWord *node)
{
    printf("visit(DocWord)\n");
    m_textBuf.append(node->word());
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visit(DocWhiteSpace *node)
{
    printf("visit(DocWhiteSpace)\n");

    // Add space only if prev node is not style.
    if (!m_styled)
        m_textBuf.append(node->chars());
    m_styled = false;
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visit(DocLineBreak *)
{
    printf("visit(DocLineBreak)\n");
    m_textBuf.append("\n");
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visit(DocLinkedWord *node)
{
    printf("visit(DocLinkedWord)\n");
    // TODO: what node to use?
    m_textBuf.append(node->word());
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visit(DocSymbol *node)
{
    // Special symbols.
    // See also HtmlEntityMapper, http://tobybartels.name/characters/
    printf("visit(DocSymbol)\n");
    switch (node->symbol()) {
    case DocSymbol::Sym_nbsp:
        m_textBuf.append(" ");
        break;
    case DocSymbol::Sym_copy:
        m_textBuf.append("(C)");
        break;
    case DocSymbol::Sym_reg:
        m_textBuf.append("(R)");
        break;
    case DocSymbol::Sym_trade:
        m_textBuf.append("(TM)");
        break;
    case DocSymbol::Sym_frac14:
        m_textBuf.append("1/4");
        break;
    case DocSymbol::Sym_frac12:
        m_textBuf.append("1/2");
        break;
    case DocSymbol::Sym_frac34:
        m_textBuf.append("3/4");
        break;
    case DocSymbol::Sym_times:
        m_textBuf.append("*");
        break;
    case DocSymbol::Sym_divide:
        m_textBuf.append("/");
        break;
    case DocSymbol::Sym_hellip:
        m_textBuf.append("...");
        break;
    case DocSymbol::Sym_prime:
        m_textBuf.append("'");
        break;
    case DocSymbol::Sym_Prime:
        m_textBuf.append("\"");
        break;
    case DocSymbol::Sym_frasl:
        m_textBuf.append("/");
        break;
    case DocSymbol::Sym_larr:
        m_textBuf.append("<-");
        break;
    case DocSymbol::Sym_rarr:
        m_textBuf.append("->");
        break;
    case DocSymbol::Sym_minus:
        m_textBuf.append("-");
        break;
    case DocSymbol::Sym_lowast:
        m_textBuf.append("*");
        break;
    case DocSymbol::Sym_sim:
        m_textBuf.append("~");
        break;
    case DocSymbol::Sym_ne:
        m_textBuf.append("!=");
        break;
    case DocSymbol::Sym_le:
        m_textBuf.append("<=");
        break;
    case DocSymbol::Sym_gt:
        m_textBuf.append(">=");
        break;
    case DocSymbol::Sym_sdot:
        m_textBuf.append(".");
        break;
    case DocSymbol::Sym_tilde:
        m_textBuf.append("~");
        break;
    case DocSymbol::Sym_ndash:
        m_textBuf.append("-");
        break;
    case DocSymbol::Sym_mdash:
        m_textBuf.append("--");
        break;
    case DocSymbol::Sym_sect:
        // Ignore this symbol.
        break;
    default:
        const char *symbol = HtmlEntityMapper::instance()->utf8(node->symbol());
        if (symbol)
            m_textBuf.append(symbol);
        else
            printf("WARNING: invalid symbol!");
        break;
    }
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visit(DocURL *node)
{
    printf("visit(DocURL)\n");
    maybeCreateTextNode();

    QCString refuri;
    if (node->isEmail())
        refuri.append("mailto:");
    refuri.append(node->url());

    PyObjectPtr args = PyTuple_New(0);
    PyDict kw;
    kw.setField("refuri", refuri);

    PyObjectPtr ref = m_tree->create("reference", args, kw.get());
    PyObjectPtr txtnode = m_tree->createTextNode(node->url());
    m_tree->addTo(ref, txtnode);
    m_tree->addToCurrent(ref);
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visit(DocHorRuler *node)
{
    printf("visit(DocHorRuler)\n");
    // TODO: add <hruler/> like node.
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visit(DocVerbatim *node)
{
    printf("visit(DocVerbatim)\n");
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visit(DocAnchor *node)
{
    printf("visit(DocAnchor)\n");
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visit(DocInclude *node)
{
    printf("visit(DocInclude)\n");
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visit(DocIncOperator *node)
{
    printf("visit(DocIncOperator)\n");
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visit(DocFormula *node)
{
    printf("visit(DocFormula)\n");
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visit(DocIndexEntry *node)
{
    // An entry in the index.
    printf("visit(DocIndexEntry)\n");
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visit(DocSimpleSectSep *)
{
    // Separator between two simple sections of the same type.
    // Nothing to add to docutils tree.
    printf("visit(DocSimpleSectSep)\n");
}
//-----------------------------------------------------------------------------

void PyDocVisitor::visit(DocCite *node)
{
    // Citation of some bibliographic reference.
    printf("visit(DocCite)\n");
}
//-----------------------------------------------------------------------------
