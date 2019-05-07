/* This file prints the last error from a system or library function call,
 * and then exits with an error code.
 */

#include <stdio.h> /* for perror() */
#include <stdlib.h> /* for exit() */

void DieWithError(char *errorMessage)
{
  perror(errorMessage);
  exit(1);
}
