#include <stdio.h>
#include <string.h>
#include "hp_file.h"
#include "record.h"
#include "bf.h"

// Function to print all contents of a heap file for testing
void print_file_contents(char *filename) {
    int file_desc;

    if (HP_OpenFile(filename, &file_desc) < 0) {
        printf("Error opening heap file: %s\n", filename);
        return;
    }

    int last_block_id = HP_GetIdOfLastBlock(file_desc);

    printf("Contents of file %s:\n", filename);
    
    for (int block_id = 1; block_id <= last_block_id; block_id++) {
        int record_count = HP_GetRecordCounter(file_desc, block_id);

        for (int rec_num = 0; rec_num < record_count; rec_num++) {
            Record record;
            if (HP_GetRecord(file_desc, block_id, rec_num, &record) != 0) {
                printf("Error retrieving record from file (block: %d, record: %d)\n", block_id, rec_num);
                HP_Unpin(file_desc, block_id);
                continue;
            }

            printf("Block: %d, Record: %d -> {ID: %d, Name: %s, Surname: %s, City: %s}\n",
                   block_id, rec_num, record.id, record.name, record.surname, record.city);

            HP_Unpin(file_desc, block_id);
        }
    }

    HP_CloseFile(file_desc);
}

void bnl_join(char *filename1, char *filename2) {
    int file_desc_r, file_desc_s;

    // print_file_contents(filename1);
    // print_file_contents(filename2);

    // Open the files
    if (HP_OpenFile(filename1, &file_desc_r) < 0) {
        printf("Error opening heap file R: %s\n", filename1);
        return;
    }

    if (HP_OpenFile(filename2, &file_desc_s) < 0) {
        printf("Error opening heap file S: %s\n", filename2);
        HP_CloseFile(file_desc_r);
        return;
    }

    // Get the last block ID for both files
    int last_block_r = HP_GetIdOfLastBlock(file_desc_r);
    int last_block_s = HP_GetIdOfLastBlock(file_desc_s);

    // Iterate over all blocks of file1
    for (int block_r = 1; block_r <= last_block_r; block_r++) { // Start from block 1
        int record_count_r = HP_GetRecordCounter(file_desc_r, block_r);

        // Iterate over all blocks of file2
        for (int block_s = 1; block_s <= last_block_s; block_s++) { // Start from block 1
            int record_count_s = HP_GetRecordCounter(file_desc_s, block_s);

            // Loop through all records in block_r (from file1)
            for (int rec_r = 0; rec_r < record_count_r; rec_r++) {
                Record record_r;
                if (HP_GetRecord(file_desc_r, block_r, rec_r, &record_r) != 0) {
                    printf("Error retrieving record from R (block: %d, record: %d)\n", block_r, rec_r);
                    HP_Unpin(file_desc_r, block_r); // Always unpin the block on error
                    continue;
                }

                // Loop through all records in block_s (from file2)
                for (int rec_s = 0; rec_s < record_count_s; rec_s++) {
                    Record record_s;
                    if (HP_GetRecord(file_desc_s, block_s, rec_s, &record_s) != 0) {
                        printf("Error retrieving record from S (block: %d, record: %d)\n", block_s, rec_s);
                        HP_Unpin(file_desc_s, block_s);
                        continue;
                    }

                    // Join condition: Compare both Name and Surname
                    if (strcmp(record_r.name, record_s.name) == 0 && strcmp(record_r.surname, record_s.surname) == 0) {
                        // If names and surnames are equal, print the matching pair
                        printf("Match found: (R: {%d, %s, %s, %s}, S: {%d, %s, %s, %s})\n",
                               record_r.id, record_r.name, record_r.surname, record_r.city,
                               record_s.id, record_s.name, record_s.surname, record_s.city);
                    }

                    HP_Unpin(file_desc_s, block_s);
                }

                HP_Unpin(file_desc_r, block_r);
            }
        }
    }

    // Close the files once processing is complete
    HP_CloseFile(file_desc_r);
    HP_CloseFile(file_desc_s);
}