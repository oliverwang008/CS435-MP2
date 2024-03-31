#include <cstdio>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>

int main(int argc, char** argv) {
    // printf("Number of arguments: %d", argc);
    if (argc != 4) {
        printf("Usage: ./linkstate topofile messagefile changesfile\n");
        return -1;
    }

    // open files
    FILE *fpMessages = fopen(argv[2], "r");
    FILE *fpTopology = fopen(argv[1], "r");
    FILE *fpChanges = fopen(argv[3], "r");

    FILE *fpOut;
    fpOut = fopen("output.txt", "w");

    // initiate arrays of nodes
    int maxNode = 0;
    int nodes[100];
    int graph[100][100];
    int next[100][100];

    for (int i = 0; i < 100; i++) {
        for (int j = 0; j < 100; j++) {
            graph[i][j] = 0;
            next[i][j] = 0;
        }
        nodes[i] = 0;
    }

    // read files

    char line[256]; // Adjust the buffer size as needed

    while (fgets(line, sizeof(line), fpTopology) != NULL) {
        if (line[0] != '\n' && strlen(line) > 4) {
            
            //printf("%s", line); // Process the line (e.g., print it)
            int src = 0;
            int des = 0;
            int link = 0;
            
            int c = 0;

            while (line[c] != ' ') {
                c++;
            }
            int spaceOne = c;
            
            if (spaceOne == 1) {
                src = line[0] - '0';
            } else {
                char srcStr[2];
                srcStr[0] = line[0];
                srcStr[1] = line[1];
                src = (line[0] - '0') * 10 + (line[1] - '0');
            }
            
            if (src > maxNode && src < 100) {
                maxNode = src;
            }
            nodes[src] = src;

            c++;
            
            while(line[c] != ' ') {
                c++;
            }
            int spaceTwo = c;
            if (spaceTwo - spaceOne == 2) {
                des = line[spaceTwo - 1] - '0';
            } else {
                char desStr[2];
                desStr[0] = line[spaceTwo - 2];
                desStr[1] = line[spaceTwo - 1];
                des = (line[spaceTwo - 2] - '0') * 10 + (line[spaceTwo - 1] - '0');
            }
            
            if (des > maxNode && des < 100) {
                maxNode = des;
            }
            nodes[des] = des;

            char linkStr[strlen(line) - spaceTwo - 1];
            
            for (int i = 0; i < strlen(line) - spaceTwo - 1; i++) {
                linkStr[i] = line[i + spaceTwo + 1];
            }
            
            link = atoi(linkStr);

            if (src > 0 && des > 0 && src < 100 && des < 100) {
                graph[src][des] = link;
                graph[des][src] = link;
            }

            //printf("from %d to %d distance %d\n", src, des, link);
        }
    }

    // calculate forwarding tables for each node

    int dist[maxNode + 1];
    int visit[maxNode + 1];
    int prev[maxNode + 1];

    //printf("\nforwarding tables:\n\n");
    for (int src = 1; src <= maxNode; src++) {
        if (nodes[src] > 0) {
            //printf("forward table entries for node %d\n", src);
            for (int i = 0; i <= maxNode; i++) {
                dist[i] = INT_MAX;
                visit[i] = 0;
                prev[i] = -1;
            }

            // get minimum distance from source to each point
            dist[src] = 0;
            for (int i = 1; i <= maxNode; i++) {
                int j;
                int minDist = INT_MAX;
                for (int k = 1; k <= maxNode; k++) {
                    if (!visit[k] && dist[k] <= minDist) {
                        minDist = dist[k];
                        j = k;
                    }
                }
                visit[j] = 1;
                for (int k = 1; k <= maxNode; k++) {
                    if (visit[k] == 0 && graph[j][k] > 0 && dist[j] < INT_MAX) {
                        if (dist[j] + graph[j][k] < dist[k]) {
                            dist[k] = dist[j] + graph[j][k];
                            prev[k] = j;
                        } else if (dist[j] + graph[j][k] == dist[k] && dist[k] < INT_MAX && j < prev[k]) {
                            prev[k] = j;
                        }
                    }
                }
            }

            //get paths from source to dest
            int revPath[maxNode];

            for (int des = 1; des <= maxNode; des++) {
                if (nodes[des] > 0) {
                    //printf("from %d to %d distance is %d\n", src, des, dist[des]);
                    for (int j = 0; j < maxNode; j++) {
                        revPath[j] = 0;
                    }
                    int before = des;
                    int p = 0;
                    while (prev[before] > 0 && prev[before] != src) {
                        before = prev[before];
                        revPath[p] = before;
                        p++;
                    }

                    //printf("going through nodes ");
                    for (int j = p - 1; j >=0; j--) {
                        //printf("%d ", revPath[j]);
                    }
                    //printf("\n");
                    int nextNode = des;
                    if (p > 0) {
                        nextNode = revPath[p - 1];
                    }
                    if (dist[des] < INT_MAX) {
                        fprintf(fpOut, "%d %d %d\n", des, nextNode, dist[des]);
                        next[src][des] = nextNode;
                    }
                }
            }
        }
    }

    while (fgets(line, sizeof(line), fpMessages) != NULL) {
        if (line[0] != '\n' && strlen(line) > 4) {
            int src;
            int des;
            
            int c = 0;
            while (line[c] != ' ') {
                c++;
            }
            int spaceOne = c;
            
            if (spaceOne == 1) {
                src = line[0] - '0';
            } else {
                char srcStr[2];
                srcStr[0] = line[0];
                srcStr[1] = line[1];
                src = (line[0] - '0') * 10 + (line[1] - '0');
            }

            c++;
            
            while(line[c] != ' ') {
                c++;
            }
            int spaceTwo = c;
            if (spaceTwo - spaceOne == 2) {
                des = line[spaceTwo - 1] - '0';
            } else {
                char desStr[2];
                desStr[0] = line[spaceTwo - 2];
                desStr[1] = line[spaceTwo - 1];
                des = (line[spaceTwo - 2] - '0') * 10 + (line[spaceTwo - 1] - '0');
            }
            
            if (src < 100 && des < 100) {
                if (next[src][des] > 0) {
                    int nextNode = next[src][des];
                    int cost = graph[src][nextNode];
                    int hops[maxNode];
                    hops[0] = src;
                    for (int i = 1; i < maxNode; i++) {
                        hops[i] = 0;
                    }
                    int count = 0;
                    int currentNode;
                    
                    while (nextNode != des) {
                        count = count + 1;
                        currentNode = nextNode;
                        nextNode = next[currentNode][des];
                        cost = cost + graph[currentNode][nextNode];
                        hops[count] = currentNode;
                    }
                    
                    fprintf(fpOut, "from %d to %d cost %d hops ", src, des, cost);
                    
                    for (int i = 0; i <= count; i++) {
                        fprintf(fpOut, "%d ", hops[i]);
                    }
                    fprintf(fpOut, "message ");

                } else {
                    fprintf(fpOut, "from %d to %d cost infinite hops unreachable message ", src, des);
                    //printf("from %d to %d cost infinite hops unreachable message ", src, des);
                }
                for (int i = 0; i < strlen(line) - spaceTwo - 1; i++) {
                    if (line[i + spaceTwo + 1] != '\n') {
                                fprintf(fpOut, "%c", line[i + spaceTwo + 1]);
                    }
                }
                fprintf(fpOut, "\n");
            }
        }
    }

    fclose(fpMessages);
    
    while (fgets(line, sizeof(line), fpChanges) != NULL) {
        if (line[0] != '\n' && strlen(line) > 4) {
            //printf("%s", line); // Process the line (e.g., print it)
            int nodeOne;
            int nodeTwo;
            int link;
            
            int c = 0;
            while (line[c] != ' ') {
                c++;
            }
            int spaceOne = c;
            
            if (spaceOne == 1) {
                nodeOne = line[0] - '0';
            } else {
                char srcStr[2];
                srcStr[0] = line[0];
                srcStr[1] = line[1];
                nodeOne = (line[0] - '0') * 10 + (line[1] - '0');
            }

            if (nodeOne > maxNode && nodeOne < 100) {
                maxNode = nodeOne;
            }
            nodes[nodeOne] = nodeOne;

            c++;
            
            while(line[c] != ' ') {
                c++;
            }
            int spaceTwo = c;
            if (spaceTwo - spaceOne == 2) {
                nodeTwo = line[spaceTwo - 1] - '0';
            } else {
                char desStr[2];
                desStr[0] = line[spaceTwo - 2];
                desStr[1] = line[spaceTwo - 1];
                nodeTwo = (line[spaceTwo - 2] - '0') * 10 + (line[spaceTwo - 1] - '0');
            }

            if (nodeTwo > maxNode && nodeTwo < 100) {
                maxNode = nodeTwo;
            }
            nodes[nodeTwo] = nodeTwo;

            char linkStr[strlen(line) - spaceTwo - 1];
            
            for (int i = 0; i < strlen(line) - spaceTwo - 1; i++) {
                linkStr[i] = line[i + spaceTwo + 1];
            }
            link = atoi(linkStr);
            if (nodeOne < 100 && nodeTwo < 100 && nodeOne > 0 && nodeTwo > 0) {
                if (link > 0) {
                    graph[nodeOne][nodeTwo] = link;
                    graph[nodeTwo][nodeOne] = link;
                } else {
                    graph[nodeOne][nodeTwo] = 0;
                    graph[nodeTwo][nodeOne] = 0;
                }
            }
            
            for (int i = 0; i < 100; i++) {
                for (int j = 0; j < 100; j++) {
                    next[i][j] = 0;
                }
            }
            

            //printf("from %d to %d distance %d\n", nodeOne, nodeTwo, link);

            //printf("\nforwarding tables after change:\n\n");
            for (int src = 1; src <= maxNode; src++) {
                if (nodes[src] > 0) {
                    //printf("forward table entries for node %d\n", src);
                    for (int i = 0; i <= maxNode; i++) {
                        dist[i] = INT_MAX;
                        visit[i] = 0;
                        prev[i] = -1;
                    }

                    // get minimum distance from source to each point
                    dist[src] = 0;
                    for (int i = 1; i <= maxNode; i++) {
                        int j;
                        int minDist = INT_MAX;
                        for (int k = 1; k <= maxNode; k++) {
                            if (!visit[k] && dist[k] <= minDist) {
                                minDist = dist[k];
                                j = k;
                            }
                        }
                        visit[j] = 1;
                        for (int k = 1; k <= maxNode; k++) {
                            if (visit[k] == 0 && graph[j][k] > 0 && dist[j] < INT_MAX) {
                                if (dist[j] + graph[j][k] < dist[k]) {
                                    dist[k] = dist[j] + graph[j][k];
                                    prev[k] = j;
                                } else if (dist[j] + graph[j][k] == dist[k] && dist[k] < INT_MAX && j < prev[k]) {
                                    prev[k] = j;
                                }
                            }
                        }
                    }

                    //get paths from source to dest
                    int revPath[maxNode];

                    for (int des = 1; des <= maxNode; des++) {
                        if (nodes[des] > 0) {
                            //printf("from %d to %d distance is %d\n", src, des, dist[des]);
                            for (int j = 0; j < maxNode; j++) {
                                revPath[j] = 0;
                            }
                            int before = des;
                            int p = 0;
                            while (prev[before] > 0 && prev[before] != src) {
                                before = prev[before];
                                revPath[p] = before;
                                p++;
                            }

                            //printf("going through nodes ");
                            for (int j = p - 1; j >=0; j--) {
                                //printf("%d ", revPath[j]);
                            }
                            //printf("\n");
                            int nextNode = des;
                            if (p > 0) {
                                nextNode = revPath[p - 1];
                            }
                            if (dist[des] < INT_MAX) {
                                fprintf(fpOut, "%d %d %d\n", des, nextNode, dist[des]);
                                next[src][des] = nextNode;
                            }
                        }
                    }
                }
            }

            fpMessages = fopen(argv[2], "r");

            while (fgets(line, sizeof(line), fpMessages) != NULL) {
                if (line[0] != '\n' && strlen(line) > 4) {
                    int src;
                    int des;
                    
                    int c = 0;
                    while (line[c] != ' ') {
                        c++;
                    }
                    int spaceOne = c;
                    
                    if (spaceOne == 1) {
                        src = line[0] - '0';
                    } else {
                        char srcStr[2];
                        srcStr[0] = line[0];
                        srcStr[1] = line[1];
                        src = (line[0] - '0') * 10 + (line[1] - '0');
                    }

                    c++;
                    
                    while(line[c] != ' ') {
                        c++;
                    }
                    int spaceTwo = c;
                    if (spaceTwo - spaceOne == 2) {
                        des = line[spaceTwo - 1] - '0';
                    } else {
                        char desStr[2];
                        desStr[0] = line[spaceTwo - 2];
                        desStr[1] = line[spaceTwo - 1];
                        des = (line[spaceTwo - 2] - '0') * 10 + (line[spaceTwo - 1] - '0');
                    }
                    
                    if (src < 100 && des < 100) {
                        if (next[src][des] > 0) {
                            int nextNode = next[src][des];
                            int cost = graph[src][nextNode];
                            int hops[maxNode];
                            hops[0] = src;
                            for (int i = 1; i < maxNode; i++) {
                                hops[i] = 0;
                            }
                            int count = 0;
                            int currentNode;
                            
                            while (nextNode != des) {
                                count = count + 1;
                                currentNode = nextNode;
                                nextNode = next[currentNode][des];
                                cost = cost + graph[currentNode][nextNode];
                                hops[count] = currentNode;
                            }
                            
                            fprintf(fpOut, "from %d to %d cost %d hops ", src, des, cost);
                            
                            for (int i = 0; i <= count; i++) {
                                fprintf(fpOut, "%d ", hops[i]);
                            }
                            fprintf(fpOut, "message ");

                        } else {
                            fprintf(fpOut, "from %d to %d cost infinite hops unreachable message ", src, des);
                            //printf("from %d to %d cost infinite hops unreachable message ", src, des);
                        }
                        for (int i = 0; i < strlen(line) - spaceTwo - 1; i++) {
                            if (line[i + spaceTwo + 1] != '\n') {
                                fprintf(fpOut, "%c", line[i + spaceTwo + 1]);
                            }
                        }
                        fprintf(fpOut, "\n");
                    }
                }
            }
            fclose(fpMessages);
        }
    }

    // close files
    fclose(fpTopology);
    fclose(fpChanges);
    fclose(fpOut);
    

    // You may choose to use std::fstream instead
    // std::ofstream fpOut("output.txt");

    return 0;
}
