#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hp_file.h"
#include "record.h"
#include "bf.h"

// Function to print all contents of a heap file
void print_file_contents(char *filename) {
    int file_desc;

    // Open the heap file
    if (HP_OpenFile(filename, &file_desc) < 0) {
        printf("Error opening heap file: %s\n", filename);
        return;
    }

    // Get the last block ID of the file
    int last_block_id = HP_GetIdOfLastBlock(file_desc);

    printf("Contents of file %s:\n", filename);
    
    // Iterate through all blocks of the file
    for (int block_id = 1; block_id <= last_block_id; block_id++) { // Skip metadata block (block 0)
        int record_count = HP_GetRecordCounter(file_desc, block_id);

        // Iterate through all records in the current block
        for (int rec_num = 0; rec_num < record_count; rec_num++) {
            Record record;
            if (HP_GetRecord(file_desc, block_id, rec_num, &record) != 0) {
                printf("Error retrieving record from file (block: %d, record: %d)\n", block_id, rec_num);
                HP_Unpin(file_desc, block_id);
                continue;
            }

            // Print the record
            printf("Block: %d, Record: %d -> {ID: %d, Name: %s, Surname: %s, City: %s}\n",
                   block_id, rec_num, record.id, record.name, record.surname, record.city);

            // Unpin the block after processing the record
            HP_Unpin(file_desc, block_id);
        }
    }

    // Close the file
    HP_CloseFile(file_desc);
}

void bnl_join(char *filename1, char *filename2) {
    int file_desc_r, file_desc_s;
    print_file_contents(filename1);
    print_file_contents(filename2);

    // Open the first heap file (R)
    if (HP_OpenFile(filename1, &file_desc_r) < 0) {
        printf("Error opening heap file R: %s\n", filename1);
        return;
    }

    // Open the second heap file (S)
    if (HP_OpenFile(filename2, &file_desc_s) < 0) {
        printf("Error opening heap file S: %s\n", filename2);
        HP_CloseFile(file_desc_r);
        return;
    }

    // Get the ID of the last block for both files
    int last_block_r = HP_GetIdOfLastBlock(file_desc_r);
    int last_block_s = HP_GetIdOfLastBlock(file_desc_s);

    // Iterate through all blocks of R
    for (int block_r = 1; block_r <= last_block_r; block_r++) { // Skip metadata block (block 0)
        int record_count_r = HP_GetRecordCounter(file_desc_r, block_r);

        // Iterate through all blocks of S
        for (int block_s = 1; block_s <= last_block_s; block_s++) { // Skip metadata block (block 0)
            int record_count_s = HP_GetRecordCounter(file_desc_s, block_s);

            // For each record in block R
            for (int rec_r = 0; rec_r < record_count_r; rec_r++) {
                Record record_r;
                if (HP_GetRecord(file_desc_r, block_r, rec_r, &record_r) != 0) {
                    printf("Error retrieving record from R (block: %d, record: %d)\n", block_r, rec_r);
                    HP_Unpin(file_desc_r, block_r); // Ensure block is unpinned if there's an error
                    continue;
                }

                // For each record in block S
                for (int rec_s = 0; rec_s < record_count_s; rec_s++) {
                    Record record_s;
                    if (HP_GetRecord(file_desc_s, block_s, rec_s, &record_s) != 0) {
                        printf("Error retrieving record from S (block: %d, record: %d)\n", block_s, rec_s);
                        HP_Unpin(file_desc_s, block_s); // Ensure block is unpinned if there's an error
                        continue;
                    }

                    // Check if the join condition is satisfied (r.A == s.B)
                    if (record_r.id == record_s.id) { // Example join condition
                        printf("Match found: (R: {%d, %s, %s, %s}, S: {%d, %s, %s, %s})\n",
                               record_r.id, record_r.name, record_r.surname, record_r.city,
                               record_s.id, record_s.name, record_s.surname, record_s.city);
                    }

                    // Unpin the block for record_s after processing
                    HP_Unpin(file_desc_s, block_s);
                }

                // Unpin the block for record_r after processing
                HP_Unpin(file_desc_r, block_r);
            }
        }
    }

    // Close the files after processing
    HP_CloseFile(file_desc_r);
    HP_CloseFile(file_desc_s);
}