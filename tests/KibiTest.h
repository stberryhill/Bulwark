/* 
 * A minimal, single-header C89 testing framework in under a Kibibyte.
 * S. Tyler Berryhill
 */

#ifndef _KIBITEST_H
#define _KIBITEST_H

#include <stdio.h>
#include <stdlib.h>

void nop() {}

int _kCurrentTestFailed; /* Don't modify externally */
void (*_kBefore)() = nop;
void (*_kAfter)() = nop;

#define Kibi_Test(function) ({\
  _kCurrentTestFailed = 0;\
  printf("%s:\n", #function);\
  _kBefore();\
  function();\
  _kAfter();\
  if (_kCurrentTestFailed) {\
    printf("...FAILED.\n\n");\
    exit(EXIT_FAILURE);\
  } else {\
    printf("... PASSED.\n\n");\
  }\
})

#define Kibi_AssertTrue(condition) ({\
  if (!(condition)) {\
    _kCurrentTestFailed = 1;\
    printf("\tError - Assert true failed @ %s:%d\n", __FILE__, __LINE__);\
  }\
})

#define Kibi_AssertFalse(condition) ({\
  if (condition) {\
    _kCurrentTestFailed = 1;\
    printf("\tError - Assert false failed @ %s:%d\n", __FILE__, __LINE__);\
  }\
})

#define Kibi_ForEach(before, after) ({\
  _kBefore = before;\
  _kAfter = after;\
})

#endif