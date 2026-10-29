/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/
#ifndef HTFILTER_H
#define HTFILTER_H
#include "WWWLib.h"
extern HTNetBefore HTProxyFilter;
extern HTNetBefore HTRuleFilter;
extern HTNetBefore HTMemoryCacheFilter;
    
extern HTNetBefore HTCredentialsFilter;
extern HTNetAfter HTInfoFilter;
extern HTNetAfter HTRedirectFilter;
extern HTNetAfter HTUseProxyFilter;
extern HTNetAfter HTAuthFilter;
extern HTNetAfter HTAuthInfoFilter;
extern HTNetAfter HTLogFilter;
extern HTNetAfter HTRefererFilter;
#endif /* HTFILTER_H */
