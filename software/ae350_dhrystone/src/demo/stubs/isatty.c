/* IMPORT */

/* Includes: */
#include <unistd.h>
#include "stub.h"


/* Definitions: */

__attribute__((used))
int _isatty(int fd)
{
  if (fd == STDOUT_FILENO || fd == STDERR_FILENO)
  {
    return 1;
  }

  return 0;
}
