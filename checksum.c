/* This file calculates a file's checksum. A string specifying the path is
 * passed in, and an unsigned int representing the sum of bytes is returned
 */

#include <stdio.h>

unsigned int calculateChecksum(char* path) {
  char next; /* next character read */
  unsigned int checksum = 0;
  FILE* file = fopen(path, "r");
  for(;;) {
    next = fgetc(file);
    if(next == EOF)
      break;
    checksum += next;
  }
  fclose(file);
  return checksum;
}
