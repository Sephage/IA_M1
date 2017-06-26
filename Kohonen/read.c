#include <stdio.h>
#include <stdlib.h>

#include "kohonen.h"

int init_input(Coordinate** coordinate, char* filename)
{
    FILE *file = fopen ( filename, "r" );

    int number_file = -1;
    int i = 0;
    int coord = 0;
    if ( file != NULL )
    {
        char line [128]; /* or other suitable maximum line size */
        puts("Init inputs");
        fgets(line, sizeof line, file);
        if(line == NULL){
            perror("number of line didn't read\n");
            return 1;
        }
        number_file = atoi(line);

        *coordinate = malloc(number_file*sizeof(Coordinate));
        Coordinate* coordinate2 = *coordinate;
        while(fgets(line, 3*sizeof(char), file) != NULL) /* read a line */
        {
            if(coord == 0){
                coordinate2[i].x = (float) atoi(line);
            }
            else{
                coordinate2[i].y = (float) atoi(line);
                i++;
            }
            coord = coord * -1 + 1;
            fgets(line, 2, file); //Read ; or \n
        }
        fclose ( file );
    }
    else
    {
        perror (filename); 
        exit(1);
    }
    puts("End of init inputs");
    return number_file;
}
