#include "Python.h"
#include "autodoc/common/visitor.h"
#include "docparser.h"
#include "htmlentity.h"
#include "autodoc/common/docutilstree.h"
#include <memory>

namespace autodoc {

void DocutilsVisitor::visit(DocWord *node)
{
    TRACE_VISIT("visit(DocWord)\n");
    if (!beforePre(node))
        return;
    m_textBuf.append(node->word());
}
//-----------------------------------------------------------------------------

void DocutilsVisitor::visit(DocWhiteSpace *node)
{
    TRACE_VISIT("visit(DocWhiteSpace)\n");
    if (!beforePre(node))
        return;

    // Add space only if prev node is not style.
    if (!m_skipNextWhitespace)
        m_textBuf.append(node->chars());
    m_skipNextWhitespace = false;
}
//-----------------------------------------------------------------------------

void DocutilsVisitor::visit(DocLineBreak *node)
{
    TRACE_VISIT("visit(DocLineBreak)\n");
    if (!beforePre(node))
        return;
    m_textBuf.append("\n");
}
//-----------------------------------------------------------------------------

void DocutilsVisitor::visit(DocLinkedWord *node)
{
    TRACE_VISIT("visit(DocLinkedWord)\n");
    if (!beforePre(node))
        return;
    // TODO: what node to use?
    m_textBuf.append(node->word());
}
//-----------------------------------------------------------------------------

void DocutilsVisitor::visit(DocSymbol *node)
{
    // Special symbols.
    // See also HtmlEntityMapper, http://tobybartels.name/characters/
    TRACE_VISIT("visit(DocSymbol)\n");
    if (!beforePre(node))
        return;
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

void DocutilsVisitor::visit(DocURL *node)
{
    TRACE_VISIT("visit(DocURL)\n");
    if (!beforePre(node))
        return;

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

void DocutilsVisitor::visit(DocHorRuler *node)
{
    TRACE_VISIT("visit(DocHorRuler)\n");
    if (!beforePre(node))
        return;
    // TODO: add <hruler/> like node.
}
//-----------------------------------------------------------------------------

void DocutilsVisitor::visit(DocVerbatim *node)
{
    // Verbatim, unparsed text fragment.
    TRACE_VISIT("visit(DocVerbatim)\n");
    if (!beforePre(node))
        return;
    
    maybeCreateTextNode();
    maybeFinishCurrentPara(node);

    switch(node->type())
    {
    case DocVerbatim::Code:
    case DocVerbatim::Verbatim:
    {
       PyObjectPtr args = PyTuple_New(0);
       PyDict kw;

       QCString text = node->text();
       if (!text.isEmpty())
       {
           // Remove trailing line break, it's redundant for docutils.
           if (text.at(text.size() - 1) == '\n')
               text.remove(text.size() - 1, 1);

           PyObjectPtr txtnode = m_tree->createTextNode(text);
           kw.setField("rawsource", txtnode);
           kw.setField("text", txtnode);
       }

       text = node->language();
       if (!text.isEmpty())
       {
           // remove dot from language name: .py -> py
           if (text.at(0) == '.')
               text.remove(0, 1);
           kw.setField("lang", text);
       }

       // This flag is set in validatingParseDoc(),
       // and we always call this function with 'false' value.
       // if (node->isExample())
       //     kw.setField("filename", node->exampleFile());

       PyObjectPtr block = m_tree->create("literal_block", args, kw.get());
       m_tree->addToCurrent(block);
       m_skipNextWhitespace = true;
       break;
    }
    case DocVerbatim::HtmlOnly:
    case DocVerbatim::RtfOnly:
    case DocVerbatim::ManOnly:
    case DocVerbatim::LatexOnly:
    case DocVerbatim::DocbookOnly:
    case DocVerbatim::XmlOnly:
        /* nothing */
        break;

    // TODO: add support, it can be some generic node.
    case DocVerbatim::Dot:
        //        visitPreStart(m_t, "dot", s->hasCaption(), this, s->children(), QCString(""), FALSE, DocImage::Html, s->width(), s->height());
        //        filter(s->text());
        //        visitPostEnd(m_t, "dot");
        break;
    case DocVerbatim::Msc:
        //        visitPreStart(m_t, "msc", s->hasCaption(), this, s->children(),  QCString(""), FALSE, DocImage::Html, s->width(), s->height());
        //        filter(s->text());
        //        visitPostEnd(m_t, "msc");
        break;
    case DocVerbatim::PlantUML:
        //        visitPreStart(m_t, "plantuml", s->hasCaption(), this, s->children(),  QCString(""), FALSE, DocImage::Html, s->width(), s->height());
        //        filter(s->text());
        //        visitPostEnd(m_t, "plantuml");
        break;
    }
}
//-----------------------------------------------------------------------------

void DocutilsVisitor::visit(DocAnchor *node)
{
    TRACE_VISIT("visit(DocAnchor)\n");
    if (!beforePre(node))
        return;
}
//-----------------------------------------------------------------------------

void DocutilsVisitor::visit(DocInclude *node)
{
    TRACE_VISIT("visit(DocInclude)\n");
    if (!beforePre(node))
        return;
}
//-----------------------------------------------------------------------------

void DocutilsVisitor::visit(DocIncOperator *node)
{
    TRACE_VISIT("visit(DocIncOperator)\n");
    if (!beforePre(node))
        return;
}
//-----------------------------------------------------------------------------

void DocutilsVisitor::visit(DocFormula *node)
{
    TRACE_VISIT("visit(DocFormula)\n");
    if (!beforePre(node))
        return;
}
//-----------------------------------------------------------------------------

void DocutilsVisitor::visit(DocIndexEntry *node)
{
    // An entry in the index.
    TRACE_VISIT("visit(DocIndexEntry)\n");
    if (!beforePre(node))
        return;
}
//-----------------------------------------------------------------------------

void DocutilsVisitor::visit(DocCite *node)
{
    // Citation of some bibliographic reference.
    TRACE_VISIT("visit(DocCite)\n");
    if (!beforePre(node))
        return;
}
//-----------------------------------------------------------------------------

} // namespace autodoc
