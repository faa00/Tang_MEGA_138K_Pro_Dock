/* IMPORT */

#ifndef _NUCLEI_SYS_STUB_H
#define _NUCLEI_SYS_STUB_H

/* Includes: */
#include <stdint.h>
#include <unistd.h>


/* Declarations: */
void write_hex(int fd, unsigned long int hex);


/* Definitions: */

__attribute__((used))
static inline int _stub(int err)
{
  return -1;
}

#endif /* _NUCLEI_SYS_STUB_H */
