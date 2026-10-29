/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/
#ifndef HTDIALOG_H
#define HTDIALOG_H
#include "WWWLib.h"
#define HT_MSG_ENGLISH_INITIALIZER \
    "Please enter username:", \
    "Please enter username for proxy authentication:", \
    "Please enter username for this FTP server:", \
    "Password:", \
    "Please give name of file to save in:", \
    "Plase enter account:", \
    "You might not be allowed to use this method here, continue?", \
    "Location has moved, continue?", \
    "A new set of rules is requested to be added to your setup - continue?", \
    "This file already exists - replace existing file?", \
    "Authentication failed - retry?", \
    "Proxy authentication failed - retry?", \
    "This method has already been performed - repeat operation?", \
    "This document is very big - continue operation?", \
    "The source document for this operation has moved - continue operation \
with new location?", \
    "The destination document for this operation has moved - continue \
operation with new location?", \
    "A redirection may change the behavior of this method - proceed anyway?", \
    "An automatic request for changing proxy has been encountered - continue?", \
    "The persistent cache is already in use by another user. If this is not \
the case then you can manually delete this lock and restart.", \
    "The server has sent you a cookie - accept?"
/*    CODE  ERROR MESSAGE				ERROR URL */
#define HTERR_ENGLISH_INITIALIZER \
    { 100, "Continue", 					"information" }, \
    { 101, "Switching Protocols",			"information" }, \
    { 200, "OK", 					"success" }, \
    { 201, "Created", 					"success" }, \
    { 202, "Accepted", 					"success" }, \
    { 203, "Non-authoritative Information",		"success" }, \
    { 204, "Document Updated",				"success" }, \
    { 205, "Reset Content",				"success" }, \
    { 206, "Partial Content",				"success" }, \
    { 207, "Partial Update OK",				"success" }, \
    { 300, "Multiple Choices",				"redirection" }, \
    { 301, "Moved Permanently",				"redirection" }, \
    { 302, "Found",  			                "redirection" }, \
    { 303, "See Other",					"redirection" }, \
    { 304, "Not Modified",       			"redirection" }, \
    { 305, "Use Proxy",					"redirection" }, \
    { 306, "Proxy Redirect",				"redirection" }, \
    { 307, "Temporary Redirect",			"redirection" }, \
    { 400, "Bad Request", 				"client_error" }, \
    { 401, "Unauthorized",				"client_error" }, \
    { 402, "Payment Required", 				"client_error" }, \
    { 403, "Forbidden", 				"client_error" }, \
    { 404, "Not Found",		       			"client_error" }, \
    { 405, "Method Not Allowed",	 		"client_error" }, \
    { 406, "Not Acceptable",		 		"client_error" }, \
    { 407, "Proxy Authentication Required", 		"client_error" }, \
    { 408, "Request Timeout",		 		"client_error" }, \
    { 409, "Conflict",			 		"client_error" }, \
    { 410, "Gone",			 		"client_error" }, \
    { 411, "Length Required",		 		"client_error" }, \
    { 412, "Precondition Failed",	 		"client_error" }, \
    { 413, "Request Entity Too Large",	 		"client_error" }, \
    { 414, "Request-URI Too Large",	 		"client_error" }, \
    { 415, "Unsupported Media Type",	 		"client_error" }, \
    { 416, "Range Not Satisfiable",	 		"client_error" }, \
    { 417, "Expectation Failed",	 		"client_error" }, \
    { 418, "Reauthentication Required",	 		"client_error" }, \
    { 419, "Proxy Reauthentication Reuired", 		"client_error" }, \
    { 500, "Internal Server Error",			"server_error" }, \
    { 501, "Not Implemented", 				"server_error" }, \
    { 502, "Bad Gateway", 				"server_error" }, \
    { 503, "Service Unavailable",			"server_error" }, \
    { 504, "Gateway Timeout", 				"server_error" }, \
    { 505, "HTTP Version not supported",		"server_error" }, \
    { 506, "Partial update Not Implemented",		"server_error" }, \
 \
    /* Cache Warnings */ \
    { 10,  "Response is Stale",				"cache" }, \
    { 11,  "Revalidation Failed",			"cache" }, \
    { 12,  "Disconnected Opeartion",			"cache" }, \
    { 13,  "Heuristic Expiration",			"cache" }, \
    { 14,  "Transformation Applied",			"cache" }, \
    { 99,  "Cache warning", 				"cache" }, \
 \
    /* Non-HTTP Error codes and warnings */ \
    { 0,   "Can't locate remote host", 			"internal" }, \
    { 0,   "No host name found", 			"internal" }, \
    { 0,   "No file name found or file not accessible", "internal" }, \
    { 0,   "FTP server replies", 			"internal" }, \
    { 0,   "FTP server doesn't reply", 			"internal" }, \
    { 0,   "FTP login failure", 			"internal" }, \
    { 0,   "Server timed out", 				"internal" }, \
    { 0,   "Gopher-server replies", 			"internal" }, \
    { 0,   "Data transfer interrupted", 		"internal" }, \
    { 0,   "Connection establishment interrupted", 	"internal" }, \
    { 0,   "CSO-server replies", 			"internal" }, \
    { 0,   "This is probably a HTTP server 0.9 or less","internal" }, \
    { 0,   "Bad, Incomplete, or Unknown Response",	"internal" }, \
    { 0,   "Unknown access authentication scheme",	"internal" }, \
    { 0,   "News-server replies",			"internal" }, \
    { 0,   "Trying `ftp://' instead of `file://'",	"internal" }, \
    { 0,   "Too many redirections",			"internal" }, \
    { 0,   "Method not suited for automatic redirection","internal" }, \
    { 0,   "Premature End Of File",			"internal" }, \
    { 0,   "Response from WAIS Server too Large - Extra lines \
ignored","internal"}, \
    { 0,   "WAIS-server doesn't return any data", 	"internal" }, \
    { 0,   "Can't connect to WAIS-server",		"internal" }, \
    { 0,   "operation failed",				"internal" }, \
    { 0,   "Wrong or unknown access scheme",		"internal" }, \
    { 0,   "Access scheme not allowed in this context",	"internal" }, \
    { 0,   "When you are connected, you can log in",	"internal" }, \
    { 0,   "This version has expired and will be automatically reloaded", "internal" }, \
    { 0,   "Loading new rules must be explicitly acknowledged", "internal" }, \
    { 0,   "Automatic proxy redirection must be explicitly acknowledged", "internal" }
extern char * HTDialog_errorMessage (HTRequest * request, HTAlertOpcode op,
			             int msgnum, const char * dfault,
				     void * input);
extern char * HTDialog_progressMessage (HTRequest * request, HTAlertOpcode op,
			                int msgnum, const char * dfault,
				        void * input);
extern HTAlertCallback HTConfirm;
		
extern HTAlertCallback HTPrompt;
extern HTAlertCallback HTPromptPassword;
extern HTAlertCallback HTPromptUsernameAndPassword;
extern HTAlertCallback HTAlert;
extern HTAlertCallback HTProgress;
extern HTAlertCallback HTError_print;
extern BOOL HTError_response (HTRequest * request, HTAlertOpcode op,
			      int msgnum, const char * dfault, void * input,
			      HTAlertPar * reply);
#endif /* HTDIALOG_H */
