#ifndef AUTODOC_COMMON_VISITOR_H
#define AUTODOC_COMMON_VISITOR_H

#include <string>
#include <memory>
#include <qcstring.h>
#include "docvisitor.h"
#include "autodoc/common/utils.h"

#ifndef PyObject_HEAD
struct _object;
typedef _object PyObject;
#endif

class Definition;
class MemberDef;
class DocNode;
class PyDocutilsTree;


void pickleDocTree(const QCString &fileName,
                   int lineNr,
                   Definition *scope,
                   MemberDef *md,
                   const QCString &text);
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


/** This visitor builds python tree object. */
class PyDocVisitor: public DocVisitor
{
public:
    PyDocVisitor(const QCString &fileName, int lineNumber);
    ~PyDocVisitor();

    PyObject* document() const;
    PyObject* takeDocument();

    //--------------------------------------
    // visitor functions for leaf nodes
    //--------------------------------------

    void visit(DocWord *);
    void visit(DocWhiteSpace *);
    void visit(DocLinkedWord *);
    void visit(DocLineBreak *);
    void visit(DocStyleChange *);
    void visit(DocSymbol *);
    void visit(DocURL *);
    void visit(DocHorRuler *);
    void visit(DocVerbatim *);
    void visit(DocAnchor *);
    void visit(DocInclude *);
    void visit(DocIncOperator *);
    void visit(DocFormula *);
    void visit(DocIndexEntry *);
    void visit(DocSimpleSectSep *);
    void visit(DocCite *);

    //--------------------------------------
    // visitor functions for compound nodes
    //--------------------------------------

    void visitPre(DocRoot *);
    void visitPost(DocRoot *);
    void visitPre(DocPara *);
    void visitPost(DocPara *);
    void visitPre(DocText *);
    void visitPost(DocText *);
    void visitPre(DocParBlock *);
    void visitPost(DocParBlock *);
    void visitPre(DocParamSect *);
    void visitPost(DocParamSect *);
    void visitPre(DocParamList *);
    void visitPost(DocParamList *);
    void visitPre(DocSimpleSect *);
    void visitPost(DocSimpleSect *);

    void visitPre(DocAutoList *);
    void visitPost(DocAutoList *);
    void visitPre(DocAutoListItem *);
    void visitPost(DocAutoListItem *);
    void visitPre(DocTitle *);
    void visitPost(DocTitle *);
    void visitPre(DocSimpleList *);
    void visitPost(DocSimpleList *);
    void visitPre(DocSimpleListItem *);
    void visitPost(DocSimpleListItem *);
    void visitPre(DocSection *);
    void visitPost(DocSection *);
    void visitPre(DocHtmlList *);
    void visitPost(DocHtmlList *) ;
    void visitPre(DocHtmlListItem *);
    void visitPost(DocHtmlListItem *);
    //void visitPre(DocHtmlPre *);
    //void visitPost(DocHtmlPre *);
    void visitPre(DocHtmlDescList *);
    void visitPost(DocHtmlDescList *);
    void visitPre(DocHtmlDescTitle *);
    void visitPost(DocHtmlDescTitle *);
    void visitPre(DocHtmlDescData *);
    void visitPost(DocHtmlDescData *);
    void visitPre(DocHtmlTable *);
    void visitPost(DocHtmlTable *);
    void visitPre(DocHtmlRow *);
    void visitPost(DocHtmlRow *);
    void visitPre(DocHtmlCell *);
    void visitPost(DocHtmlCell *);
    void visitPre(DocHtmlCaption *);
    void visitPost(DocHtmlCaption *);
    void visitPre(DocInternal *);
    void visitPost(DocInternal *);
    void visitPre(DocHRef *);
    void visitPost(DocHRef *);
    void visitPre(DocHtmlHeader *);
    void visitPost(DocHtmlHeader *);
    void visitPre(DocImage *);
    void visitPost(DocImage *);
    void visitPre(DocDotFile *);
    void visitPost(DocDotFile *);

    void visitPre(DocMscFile *);
    void visitPost(DocMscFile *);
    void visitPre(DocDiaFile *);
    void visitPost(DocDiaFile *);
    void visitPre(DocLink *);
    void visitPost(DocLink *);
    void visitPre(DocRef *);
    void visitPost(DocRef *);
    void visitPre(DocSecRefItem *);
    void visitPost(DocSecRefItem *);
    void visitPre(DocSecRefList *);
    void visitPost(DocSecRefList *);
    //void visitPre(DocLanguage *);
    //void visitPost(DocLanguage *);
    void visitPre(DocXRefItem *);
    void visitPost(DocXRefItem *);
    void visitPre(DocInternalRef *);
    void visitPost(DocInternalRef *);
    void visitPre(DocCopy *);
    void visitPost(DocCopy *);
    void visitPre(DocHtmlBlockQuote *);
    void visitPost(DocHtmlBlockQuote *);
    void visitPre(DocVhdlFlow *);
    void visitPost(DocVhdlFlow *);

private:
    std::unique_ptr<PyDocutilsTree> m_tree;
    PyObjectPtr m_fieldList;

    QCString m_fileName;
    int m_lineNumber;
    QCString m_textBuf;

    int m_paramType;

    /**
     * Create text node from content in m_textBuf.
     */
    bool maybeCreateTextNode();
    bool maybeFinishCurrentPara(DocNode *node);
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


#endif // AUTODOC_COMMON_VISITOR_H
