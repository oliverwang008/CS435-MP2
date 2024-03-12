#include <cstdio>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


int main(int argc, char** argv) {
    //printf("Number of arguments: %d", argc);
    if (argc != 4) {
        printf("Usage: ./linkstate topofile messagefile changesfile\n");
        return -1;
    }

    FILE *fpTopology = fopen(argv[1], "r");

    if (!fpTopology) {
        printf("Error in opening topofile");
    }

    printf("Open file: %s\n", "topofile");

    char cr;
    size_t lines = 1;
    char eol = '\n';

    while (cr != EOF ) {
        if (cr == '\n') {
            lines++;
        }
        cr = getc(fpTopology);
    }

    printf("Number of lines: %ld\n", lines);

    rewind(fpTopology);

    
    char *data[lines];

    for (size_t i = 0; i < lines; i++) {
        data[i] = NULL;
        size_t n = 0;

        getline(&data[i], &n, fpTopology);

        if (ferror(fpTopology)) {
            printf("Error when reading topofile");
        }
    }

    strncat(data[lines-1], &eol, 1);

    for (size_t i = 0; i < lines; i++) {
        printf("%s", data[i]);
    }

    fclose(fpTopology);

    FILE *fpChanges = fopen(argv[3], "r");

    if (!fpChanges) {
        printf("Error in opening changesfile");
    }

    printf("Open file: %s\n", "changesfile");

    cr = NULL;
    size_t linesChanges = 1;

    while (cr != EOF ) {
        if (cr == '\n') {
            linesChanges++;
        }
        cr = getc(fpChanges);
    }
    
    printf("Number of lines: %ld\n", linesChanges);

    rewind(fpChanges);

    
    char *dataChanges[linesChanges];

    for (size_t i = 0; i < linesChanges; i++) {
        dataChanges[i] = NULL;
        size_t n = 0;

        getline(&dataChanges[i], &n, fpChanges);

        if (ferror(fpChanges)) {
            printf("Error when reading changefile");
        }
    }

    strncat(dataChanges[linesChanges-1], &eol, 1);


    for (size_t i = 0; i < linesChanges; i++) {
        char nodesChanges[3];
        strncpy(nodesChanges, dataChanges[i], 3);
        for (size_t j = 0; j < lines; j++) {
            char nodes[3];
            strncpy(nodes, data[j], 3);
            if (strncmp(nodes, nodesChanges, 3) == 0 ) {
                data[j] = dataChanges[i];
            }
        }
        for (size_t k = 0; k < lines; k++) {

            printf("%s", data[k]);

        }
        printf("\nbreak\n\n");

    }

    for (size_t i = 0; i < lines; i++) {
        free(data[i]);
    }


    
    
    fclose(fpChanges);

    FILE *fpOut;
    fpOut = fopen("output.txt", "w");
    fclose(fpOut);

    // You may choose to use std::fstream instead
    // std::ofstream fpOut("output.txt");

    return 0;
}
