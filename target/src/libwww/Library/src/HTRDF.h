#ifndef HTRDF_H
#define HTRDF_H

#include "HTHash.h"

#define RDFMS "http://www.w3.org/1999/02/22-rdf-syntax-ns#"
#define RDFSCHEMA "http://www.w3.org/TR/WD-rdf-schema#"
#define XMLSCHEMA "xml"
typedef struct _HTTriple HTTriple;

extern HTTriple * HTTriple_new (char * p, char * s, char * o);
extern BOOL HTTriple_delete (HTTriple * me);
extern void HTTriple_print (HTTriple * me);
extern char * HTTriple_subject (HTTriple * me);
extern char * HTTriple_predicate (HTTriple * me);
extern char * HTTriple_object (HTTriple * me);
typedef struct _HTElement HTElement;

extern HTElement * HTElement_new (char * sName, HTAssocList * al);
extern HTElement * HTElement_new2 (char * sContent);
extern BOOL HTElement_addData (HTElement *me, char * sContent);
extern BOOL HTElement_delete (HTElement * me);
extern BOOL HTElement_addChild (HTElement * me, HTElement * element);
extern BOOL HTElement_addAttribute (HTElement * me, char * sName, char * sValue);
extern BOOL HTElement_removeAttribute (HTElement * me, char * sName);
extern char * HTElement_getAttribute (HTElement * me, char * sName);
extern char * HTElement_getAttribute2 (HTElement * me, char * sNamespace, char * sName);
extern BOOL HTElement_addTarget (HTElement * me, HTElement * element);
extern HTElement * HTElement_target (HTElement * me);
extern BOOL HTElement_instanceOfData (HTElement * me);
typedef struct _HTRDFParser HTRDF;

extern HTRDF * HTRDF_new (void);
extern BOOL HTRDF_delete (HTRDF * me);
typedef void HTRDFCallback_new (
	HTStream *		me,
	HTRequest *		request,
	HTFormat 		target_format,
	HTStream *		target_stream,
	HTRDF *                 rdfparser,
	void *                  context);
extern BOOL HTRDF_registerNewParserCallback (HTRDFCallback_new *, void * context);
typedef void HTTripleCallback_new (
	HTRDF *		rdfp,
	HTTriple *	t,
	void *		context);
extern BOOL HTRDF_registerNewTripleCallback (
	HTRDF *			me,
	HTTripleCallback_new * 	cbf,
	void * 			context);
extern BOOL HTRDF_setSource (HTRDF * me, char * source);
extern BOOL HTRDF_resolve(HTRDF *me);
extern BOOL HTRDF_processXML(HTRDF *me, HTElement *root);
extern HTElement * HTRDF_root(HTRDF *me);
extern char * HTRDF_namespace (HTRDF * me, char * sPrefix);
extern BOOL HTRDF_parseLiteral(HTRDF *me);
extern BOOL HTRDF_parseResource(HTRDF *me);
extern void HTRDF_resolveLater(HTRDF *me,HTElement *e);
extern void HTRDF_registerID(HTRDF *me, char * sID,HTElement *e);
extern void HTRDF_registerResource(HTRDF *me,HTElement *e);
extern HTElement *HTRDF_lookforNode(HTRDF *me, char * sID);
extern BOOL HTRDF_isRDF(HTRDF *me, HTElement *ele);
extern BOOL HTRDF_isRDFroot(HTRDF *me, HTElement *ele);
extern BOOL HTRDF_isDescription(HTRDF *me, HTElement *ele);
extern BOOL HTRDF_isTypedPredicate(HTRDF *me, HTElement *e);
extern BOOL HTRDF_isContainer(HTRDF *me, HTElement *e);
extern BOOL HTRDF_isBag(HTRDF *me, HTElement *e);
extern BOOL HTRDF_isAlternative(HTRDF *me, HTElement *e);
extern BOOL HTRDF_isSequence(HTRDF *me, HTElement *e);
extern BOOL HTRDF_isListItem (HTRDF *me, HTElement *e);
extern BOOL HTRDF_processRDF(HTRDF *me, HTElement *ele);
extern char * HTRDF_processDescription (
	HTRDF *		me,
	HTElement *	description,
	BOOL		inPredicate,
	BOOL		reificate,
	BOOL		createBag);
extern char * HTRDF_processTypedNode(HTRDF *me, HTElement *e);
extern char * HTRDF_getResource(HTRDF *me,HTElement *e);
extern char * HTRDF_newReificationID (HTRDF *me);
extern void HTRDF_addTriple (HTRDF *me, char * sPredicate, char * sSubject,
			     char * sObject);
extern void HTRDF_createBags (HTRDF *me, BOOL b);
extern void HTRDF_setOutputStream (HTRDF *me, HTStream *ostream);
extern HTConverter HTRDFParser_new;
extern HTConverter HTRDFToTriples;
extern BOOL HTRDF_parseFile (const char *file_name, 
                             HTTripleCallback_new * new_triple_callback,
                             void *context);
extern BOOL HTRDF_parseBuffer (const char *buffer, 
		               const char *buffer_name, 
                               int buffer_len, 
                               HTTripleCallback_new * new_triple_callback,
                               void *context);
#endif
