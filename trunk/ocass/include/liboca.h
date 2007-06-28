
#ifndef _LIBOCA_H_
#define _LIBOCA_H_ 1

#include "ca_types.h"
#include "ca_flock.h"

CA_DECLARE(CAErrno) CA_Startup(void);
CA_DECLARE(void)    CA_Cleanup(void);

CA_DECLARE(CAErrno) CA_RTCSEnter(void);
CA_DECLARE(CAErrno) CA_RTCSLeave(void);

#define CA_RT_PL_FNAME      (TEXT("ocass_rt.lock"))

CA_DECLARE(CAErrno) CA_RTPLockEnter(CAFLockOpt lockOpt);
CA_DECLARE(CAErrno) CA_RTPLockLeave(void);


#endif /* !defined(_LIBOCA_H_) */
