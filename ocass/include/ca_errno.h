/**
 *
 */

#ifndef _CA_ERRNO_H_
#define _CA_ERRNO_H_ 1

/**
 * @{
 */

typedef int CAErrno;
#define CA_ERR_SUCCESS              (0)
#define CA_ERR_EOF                  (200)

#define CA_ERR_FATAL                (-900)
#define CA_ERR_NOT_IMPL             (-901)
#define CA_ERR_BAD_ARG              (-902)
#define CA_ERR_NO_MEM               (-903)
#define CA_ERR_SYS_CALL             (-905)
#define CA_ERR_TIMEOUT              (-907)
#define CA_ERR_OPT_CANCEL           (-908)
#define CA_ERR_OBJ_CANNOT_FIND      (-909)

#define CA_ERR_FNAME_TOO_LONG       (-1000)
#define CA_ERR_FNAME_CANNOT_FIND    (-1001)
#define CA_ERR_FNAME_BAD            (-1002)


/** @} */

/**
 * @{
 */

#define CA_PROC_EXIT_OK             (  0)
#define CA_PROC_EXIT_INIT_FAILED    ( 25)
#define CA_PROC_EXIT_ALREADY_RUN    ( 30)
#define CA_PROC_EXIT_NO_RUNNING     ( 31)
#define CA_PROC_EXIT_USEAGE         ( 50)
#define CA_PROC_EXIT_FATAL          (101)

/** @} */

/**
 * @{
 */

#define CA_THREAD_EXIT_OK             (  0)

/** @} */

#endif /* !defined(_CA_ERRNO_H_) */
