#pragma once

#define EXIT_OK 0
#define EXIT_OPER_MISSING 1
#define EXIT_OPER_INVALID 2
#define EXIT_ARGS_BADSIZE 3
#define EXIT_FILE_LOADERR 4
#define EXIT_FILE_READERR 5
#define EXIT_FILE_WRITERR 6
#define EXIT_MEMO_ALFAULT 7
#define EXIT_MEMO_IXFAULT 8

#define EXMSG_ROOT "An error occured whilst "

#define EXMSG_TYPE_READ "reading the input file : "
#define EXMSG_TYPE_WRITE "writing to the input file : "
#define EXMSG_TYPE_MEMORY "accessing memory : "

#define EXMSG_READ EXMSG_ROOT EXMSG_TYPE_READ
#define EXMSG_WRITE EXMSG_ROOT EXMSG_TYPE_WRITE
#define EXMSG_CORRUPT EXMSG_ROOT EXMSG_TYPE_READ "Corrupt input data!\n"
#define EXMSG_BADHEAD EXMSG_ROOT EXMSG_TYPE_READ "Uncompatible header!\n"
#define EXMSG_TOOLARGE EXMSG_ROOT EXMSG_TYPE_READ "File too large! (4 Gibibytes of size and more are not allowed)\n"
#define EXMSG_BADALLOC EXMSG_ROOT EXMSG_TYPE_MEMORY "Dynamic allocation failed!"
#define EXMSG_INDEX EXMSG_ROOT EXMSG_TYPE_MEMORY "Maximum buffer size exceeded!"