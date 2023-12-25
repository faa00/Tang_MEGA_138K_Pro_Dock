/* IMPORT */

/* Includes: */
#include <errno.h>
#include "stub.h"


/* Definitions: */

__attribute__((used))
int _close(int fd)
{
  return _stub(EBADF);
}
