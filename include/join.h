#ifndef JOIN_H
#define JOIN_H


#include <stdbool.h>


/** Performs a Block-Nested Loop Join (BNLJ) between two heap 
 * files. Joins records from `filename1` (outer relation) and 
 * `filename2` (inner relation) based on a specified condition, 
 * processing `filename1` in blocks and scanning `filename2` for 
 * each block. Matching records are printed.**/
void bnl_join(char *filename1, char *filename2);

#endif
