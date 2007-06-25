/**
 *
 */

#ifndef _CC_INNER_H_
#define _CC_INNER_H_ 1

struct _CC_Wrk
{
    int x;
};


DWORD WINAPI CC_WrkThread(void *pThArg);

#endif /* !defined(_CC_INNER_H_) */

