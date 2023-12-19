/* IMPORT */

/* Includes: */
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include "stub.h"


/* Definitions: */

__attribute__((used))
int _fstat(int fd, struct stat* st)
{
  if (isatty(fd))
  {
    st->st_mode = S_IFCHR;
    return 0;
  }

  return _stub(EBADF);
}
