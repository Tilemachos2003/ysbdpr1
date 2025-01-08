#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <bf.h>
#include "join.h"
#include "record.h"
#include "hp_file.h"

#define HP_FILE1 "data1.db"
#define HP_FILE2 "data2.db"

void createAndPopulateHeapFile(char *filename, int recNum);

int main()
{
  BF_Init(LRU);
  createAndPopulateHeapFile(HP_FILE1, 4000);
  createAndPopulateHeapFile(HP_FILE2, 4000);

  bnl_join(HP_FILE1,HP_FILE2);  
}

void createAndPopulateHeapFile(char *filename, int recNum)
{
  HP_CreateFile(filename);

  int file_desc;
  HP_OpenFile(filename, &file_desc);

  Record record;
  srand(12569874);
  for (int id = 0; id < recNum; ++id)
  {
    record = randomRecord();
    HP_InsertEntry(file_desc, record);
  }
  HP_CloseFile(file_desc);
}
