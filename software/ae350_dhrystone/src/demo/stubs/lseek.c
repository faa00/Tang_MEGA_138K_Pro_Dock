/* IMPORT */

/* Includes: */
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include "stub.h"


/* Definitions: */

__attribute__((used))
off_t _lseek(int fd, off_t ptr, int dir)
{
  if (isatty(fd))
  {
    return 0;
  }

  return _stub(EBADF);
}
