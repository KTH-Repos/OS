#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

const int disk_size = 200;
const int g_nof_cylinder_requests = 10;
int g_cylinder_requests[g_nof_cylinder_requests];
int g_sorted_cylinder_requests[g_nof_cylinder_requests];


int Sort_Int_Comparator(const void* num_a, const void * num_b);
void Initialize_Cylinder_Requests_Randomly();

/*All algorithms below are able to handle the following edge case:
if there are no cylinder requests (if the g_nof_cylinder_requests
list is empty) then the number of head movements equals 0 in all
algorithms.

The SCAN, C-SCAN, LOOK and C-LOOK algorithms are able to
handle the following edge cases: there are no down request
only up requests; there are no up requests only down request;
there are no up request or down requests. The last case means
that the g_nof_cylinder_requests list is empty.
*/
int FCFS_Movements(int initial_disk_head_position, bool debug_mode);
int SSTF_Movements(int initial_disk_head_position, bool debug_mode);
int SCAN_Movements(int initial_disk_head_position, bool debug_mode);
int C_SCAN_Movements(int initial_disk_head_position, bool debug_mode);
int LOOK_Movements(int initial_disk_head_position, bool debug_mode);
int C_LOOK_Movements(int initial_disk_head_position, bool debug_mode);


int main(int argc, char* argv[])
{
    int initial_disk_head_position = atoi(argv[1]);
    Initialize_Cylinder_Requests_Randomly();

    printf("\n");

    int nof_movements = -1;

    nof_movements = FCFS_Movements(initial_disk_head_position, false);
    printf("FCFS_Movements: %d\n\n", nof_movements);

    nof_movements = SSTF_Movements(initial_disk_head_position, false);
    printf("SSTF_Movements: %d\n\n", nof_movements);

    nof_movements = SCAN_Movements(initial_disk_head_position, false);
    printf("SCAN_Movements: %d\n\n", nof_movements);

    nof_movements = C_SCAN_Movements(initial_disk_head_position, false);
    printf("C_SCAN_Movements: %d\n\n", nof_movements);

    nof_movements = LOOK_Movements(initial_disk_head_position, false);
    printf("LOOK_Movements: %d\n\n", nof_movements);

    nof_movements = C_LOOK_Movements(initial_disk_head_position, false);
    printf("C_LOOK_Movements: %d\n", nof_movements);

    printf("\n");

    return 0;
}


int Sort_Int_Comparator(const void* num_a, const void * num_b)
{
   return (*(int*) num_a - *(int*) num_b);
}


void Initialize_Cylinder_Requests_Randomly()
{
    srand(time(NULL));

    for(int index = 0; index < g_nof_cylinder_requests; index++)
    {
        g_cylinder_requests[index] = rand() % disk_size;
        g_sorted_cylinder_requests[index] = g_cylinder_requests[index];

    }

    qsort(g_sorted_cylinder_requests, g_nof_cylinder_requests, sizeof(int), Sort_Int_Comparator);
}


int FCFS_Movements(int initial_disk_head_position, bool debug_mode)
{
    int nof_head_movements = 0;
    if(g_nof_cylinder_requests <= 0) return nof_head_movements;

    nof_head_movements = abs(initial_disk_head_position - g_cylinder_requests[0]);
    if(debug_mode == true) printf("abs(%d - %d) = %d\n", initial_disk_head_position, g_cylinder_requests[0], abs(initial_disk_head_position - g_cylinder_requests[0]));

    for(int index = 0; index < g_nof_cylinder_requests - 1; index++)
    {
        nof_head_movements += abs(g_cylinder_requests[index] - g_cylinder_requests[index + 1]);
        if(debug_mode == true) printf("abs(%d - %d) = %d\n", g_cylinder_requests[index], g_cylinder_requests[index + 1], abs(g_cylinder_requests[index] - g_cylinder_requests[index + 1]));
    }

    return nof_head_movements;
}


int SSTF_Movements(int initial_disk_head_position, bool debug_mode)
{
    int nof_head_movements = 0;
    if(g_nof_cylinder_requests <= 0) return nof_head_movements;

    int nof_visited_cylinders = 0;
    bool visited_cylinders[g_nof_cylinder_requests];
    for(int index = 0; index < g_nof_cylinder_requests; index++) visited_cylinders[index] = false;

    int current_head_position = initial_disk_head_position;

    while(nof_visited_cylinders < g_nof_cylinder_requests)
    {
        int min_index = -1;
        int min_value = 10000;

        for(int index = 0; index < g_nof_cylinder_requests; index++)
        {
            if(current_head_position == g_cylinder_requests[index]) continue;

            int difference = abs(current_head_position - g_cylinder_requests[index]);

            if(difference < min_value && visited_cylinders[index] == false)
            {
                min_value = difference;
                min_index = index;
            }
        }

        visited_cylinders[min_index] = true;
        nof_visited_cylinders++;
        nof_head_movements += min_value;
        if(debug_mode == true) printf("abs(%d - %d) = %d\n", current_head_position, g_cylinder_requests[min_index], min_value);
        current_head_position = g_cylinder_requests[min_index];
    }

    return nof_head_movements;
}


/*Assumption 1: the initial direction of the head movement
is down/left. In otherwords, it handles cylinder requests
smaller than or equal to the initial disk head postition
before it handles cylinder requests larger than the initial
disk head position.

Assumption 2: if there are no down requests then the algorithm
will still move the head from the initial disk head position to
0 before it handles the up requests. (This assumption was made
since it was not mentioned in the lectures and it was not discussed
in the book).*/
int SCAN_Movements(int initial_disk_head_position, bool debug_mode)
{
    int nof_head_movements = 0;
    if(g_nof_cylinder_requests <= 0) return nof_head_movements;

    int nof_down_requests = 0;
    int nof_up_requests = 0;

    for(int index = 0; index < g_nof_cylinder_requests; index++)
    {
        if(g_sorted_cylinder_requests[index] <= initial_disk_head_position)
        {
            nof_down_requests++;
        }
        else if(g_sorted_cylinder_requests[index] > initial_disk_head_position)
        {
            nof_up_requests++;
        }
    }

    int down_requests[nof_down_requests];
    int down_requests_index = 0;
    int up_requests[nof_up_requests];
    int up_requests_index = 0;

    for(int index = 0; index < g_nof_cylinder_requests; index++)
    {
        if(g_sorted_cylinder_requests[index] <= initial_disk_head_position)
        {
            down_requests[down_requests_index] = g_sorted_cylinder_requests[index];
            down_requests_index++;
        }
        else if(g_sorted_cylinder_requests[index] > initial_disk_head_position)
        {
            up_requests[up_requests_index] = g_sorted_cylinder_requests[index];
            up_requests_index++;
        }
    }

    if(nof_down_requests > 0)
    {
        nof_head_movements += abs(initial_disk_head_position - down_requests[nof_down_requests - 1]);
        if(debug_mode == true) printf("abs(%d - %d) = %d\n", initial_disk_head_position, down_requests[nof_down_requests - 1], abs(initial_disk_head_position - down_requests[nof_down_requests - 1]));
    }
    else
    {
        nof_head_movements += abs(initial_disk_head_position - 0);
        if(debug_mode == true) printf("abs(%d - %d) = %d\n", initial_disk_head_position, 0, abs(initial_disk_head_position - 0));
    }

    if(nof_down_requests > 0)
    {
        for(int index = nof_down_requests - 1; index >= 1; index--)
        {
            nof_head_movements += abs(down_requests[index] - down_requests[index - 1]);
            if(debug_mode == true) printf("abs(%d - %d) = %d\n", down_requests[index], down_requests[index - 1], abs(down_requests[index] - down_requests[index - 1]));
        }

        nof_head_movements += abs(down_requests[0] - 0);
        if(debug_mode == true) printf("abs(%d - %d) = %d\n", down_requests[0], 0, abs(down_requests[0] - 0));
    }

    if(nof_up_requests > 0)
    {
        nof_head_movements += abs(0 - up_requests[0]);
        if(debug_mode == true) printf("abs(%d - %d) = %d\n", 0, up_requests[0], abs(0 - up_requests[0]));

        for(int index = 0; index < nof_up_requests - 1; index++)
        {
            nof_head_movements += abs(up_requests[index] - up_requests[index + 1]);
            if(debug_mode == true) printf("abs(%d - %d) = %d\n", up_requests[index], up_requests[index + 1], abs(up_requests[index] - up_requests[index + 1]));
        }
    }

    return nof_head_movements;
}


/*Asummption 1: the initial direction of the head movement
is up/right. In otherwords, it handles cylinder requests
larger than or equal to the initial disk head postition
before it handles cylinder requests smaller than the initial
disk head position.

Assumption 2: if there are
no down requests then the algorithm will still move the head
from the (disk size - 1) position to 0 after it has handled
all up requests. (This assumption was made since it was not
mentioned in the lectures and it was not discussed in the
book).*/
int C_SCAN_Movements(int initial_disk_head_position, bool debug_mode)
{
    int nof_head_movements = 0;
    if(g_nof_cylinder_requests <= 0) return nof_head_movements;

    int nof_down_requests = 0;
    int nof_up_requests = 0;

    for(int index = 0; index < g_nof_cylinder_requests; index++)
    {
        if(g_sorted_cylinder_requests[index] < initial_disk_head_position)
        {
            nof_down_requests++;
        }
        else if(g_sorted_cylinder_requests[index] >= initial_disk_head_position)
        {
            nof_up_requests++;
        }
    }

    int down_requests[nof_down_requests];
    int down_requests_index = 0;
    int up_requests[nof_up_requests];
    int up_requests_index = 0;

    for(int index = 0; index < g_nof_cylinder_requests; index++)
    {
        if(g_sorted_cylinder_requests[index] < initial_disk_head_position)
        {
            down_requests[down_requests_index] = g_sorted_cylinder_requests[index];
            down_requests_index++;
        }
        else if(g_sorted_cylinder_requests[index] >= initial_disk_head_position)
        {
            up_requests[up_requests_index] = g_sorted_cylinder_requests[index];
            up_requests_index++;
        }
    }

    if(nof_up_requests > 0)
    {
        nof_head_movements += abs(initial_disk_head_position - up_requests[0]);
        if(debug_mode == true) printf("abs(%d - %d) = %d\n", initial_disk_head_position, up_requests[0], abs(initial_disk_head_position - up_requests[0]));

        for(int index = 0; index < nof_up_requests - 1; index++)
        {
            if(debug_mode == true) printf("abs(%d - %d) = %d\n", up_requests[index], up_requests[index + 1], abs(up_requests[index] - up_requests[index + 1]));
            nof_head_movements += abs(up_requests[index] - up_requests[index + 1]);
        }

        nof_head_movements += abs(up_requests[nof_up_requests - 1] - (disk_size - 1));
        if(debug_mode == true) printf("abs(%d - %d) = %d\n", up_requests[nof_up_requests - 1], (disk_size - 1), abs(up_requests[nof_up_requests - 1] - (disk_size - 1)));
    }
    else
    {
        nof_head_movements += abs(initial_disk_head_position - (disk_size - 1));
        if(debug_mode == true) printf("abs(%d - %d) = %d\n", initial_disk_head_position, (disk_size - 1), abs(initial_disk_head_position - (disk_size - 1)));
    }

    nof_head_movements += abs((disk_size - 1) - 0);
    if(debug_mode == true) printf("abs(%d - %d) = %d\n", (disk_size - 1), 0, abs((disk_size - 1) - 0));

    if(nof_down_requests > 0)
    {
        nof_head_movements += abs(0 - down_requests[0]);
        if(debug_mode == true) printf("abs(%d - %d) = %d\n", 0, down_requests[0], abs(0 - down_requests[0]));

        for(int index = 0; index < nof_down_requests - 1; index++)
        {
            if(debug_mode == true) printf("abs(%d - %d) = %d\n", down_requests[index], down_requests[index + 1], abs(down_requests[index] - down_requests[index + 1]));
            nof_head_movements += abs(down_requests[index] - down_requests[index + 1]);
        }
    }

    return nof_head_movements;
}


/*This implementation makes the assumption that the initial
direction of the head movement is down/left. In otherwords,
it handles cylinder requests smaller than or equal to the
initial disk head postition before it handles cylinder
requests larger than the initial disk head position.
*/
int LOOK_Movements(int initial_disk_head_position, bool debug_mode)
{
    int nof_head_movements = 0;
    if(g_nof_cylinder_requests <= 0) return nof_head_movements;

    int nof_down_requests = 0;
    int nof_up_requests = 0;

    for(int index = 0; index < g_nof_cylinder_requests; index++)
    {
        if(g_sorted_cylinder_requests[index] <= initial_disk_head_position)
        {
            nof_down_requests++;
        }
        else if(g_sorted_cylinder_requests[index] > initial_disk_head_position)
        {
            nof_up_requests++;
        }
    }

    int down_requests[nof_down_requests];
    int down_requests_index = 0;
    int up_requests[nof_up_requests];
    int up_requests_index = 0;

    for(int index = 0; index < g_nof_cylinder_requests; index++)
    {
        if(g_sorted_cylinder_requests[index] <= initial_disk_head_position)
        {
            down_requests[down_requests_index] = g_sorted_cylinder_requests[index];
            down_requests_index++;
        }
        else if(g_sorted_cylinder_requests[index] > initial_disk_head_position)
        {
            up_requests[up_requests_index] = g_sorted_cylinder_requests[index];
            up_requests_index++;
        }
    }

    if(nof_down_requests > 0)
    {
        nof_head_movements += abs(initial_disk_head_position - down_requests[nof_down_requests - 1]);
        if(debug_mode == true) printf("abs(%d - %d) = %d\n", initial_disk_head_position, down_requests[nof_down_requests - 1], abs(initial_disk_head_position - down_requests[nof_down_requests - 1]));
    }
    else
    {
        nof_head_movements += abs(initial_disk_head_position - up_requests[0]);
        if(debug_mode == true) printf("abs(%d - %d) = %d\n", initial_disk_head_position, up_requests[0], abs(initial_disk_head_position - up_requests[0]));        
    }

    if(nof_down_requests > 0)
    {
        for(int index = nof_down_requests - 1; index >= 1; index--)
        {
            if(debug_mode == true) printf("abs(%d - %d) = %d\n", down_requests[index], down_requests[index - 1], abs(down_requests[index] - down_requests[index - 1]));
            nof_head_movements += abs(down_requests[index] - down_requests[index - 1]);
        }

        if(nof_up_requests > 0)
        {
            nof_head_movements += abs(down_requests[0] - up_requests[0]);
            if(debug_mode == true) printf("abs(%d - %d) = %d\n", down_requests[0], up_requests[0], abs(down_requests[0] - up_requests[0]));
        }
    }

    if(up_requests > 0)
    {
        for(int index = 0; index < nof_up_requests - 1; index++)
        {
            if(debug_mode == true) printf("abs(%d - %d) = %d\n", up_requests[index], up_requests[index + 1], abs(up_requests[index] - up_requests[index + 1]));
            nof_head_movements += abs(up_requests[index] - up_requests[index + 1]);
        }
    }

    return nof_head_movements;
}


/*This implementation makes the assumption that the initial
direction of the head movement is up/right. In otherwords,
it handles cylinder requests larger than or equal to the
initial disk head postition before it handles cylinder
requests smaller than the initial disk head position.*/
int C_LOOK_Movements(int initial_disk_head_position, bool debug_mode)
{
    int nof_head_movements = 0;
    if(g_nof_cylinder_requests <= 0) return nof_head_movements;

    int nof_down_requests = 0;
    int nof_up_requests = 0;

    for(int index = 0; index < g_nof_cylinder_requests; index++)
    {
        if(g_sorted_cylinder_requests[index] < initial_disk_head_position)
        {
            nof_down_requests++;
        }
        else if(g_sorted_cylinder_requests[index] >= initial_disk_head_position)
        {
            nof_up_requests++;
        }
    }

    int down_requests[nof_down_requests];
    int down_requests_index = 0;
    int up_requests[nof_up_requests];
    int up_requests_index = 0;

    for(int index = 0; index < g_nof_cylinder_requests; index++)
    {
        if(g_sorted_cylinder_requests[index] < initial_disk_head_position)
        {
            down_requests[down_requests_index] = g_sorted_cylinder_requests[index];
            down_requests_index++;
        }
        else if(g_sorted_cylinder_requests[index] >= initial_disk_head_position)
        {
            up_requests[up_requests_index] = g_sorted_cylinder_requests[index];
            up_requests_index++;
        }
    }

    if(nof_up_requests > 0)
    {
        nof_head_movements += abs(initial_disk_head_position - up_requests[0]);
        if(debug_mode == true) printf("abs(%d - %d) = %d\n", initial_disk_head_position, up_requests[0], abs(initial_disk_head_position - up_requests[0]));

        for(int index = 0; index < nof_up_requests - 1; index++)
        {
            if(debug_mode == true) printf("abs(%d - %d) = %d\n", up_requests[index], up_requests[index + 1], abs(up_requests[index] - up_requests[index + 1]));
            nof_head_movements += abs(up_requests[index] - up_requests[index + 1]);
        }

        if(nof_down_requests > 0)
        {
            nof_head_movements += abs(up_requests[nof_up_requests - 1] - down_requests[0]);
            if(debug_mode == true) printf("abs(%d - %d) = %d\n", up_requests[nof_up_requests - 1], down_requests[0], abs(up_requests[nof_up_requests - 1] - down_requests[0]));
        }
    }
    else
    {
        nof_head_movements += abs(initial_disk_head_position - down_requests[0]);
        if(debug_mode == true) printf("abs(%d - %d) = %d\n", initial_disk_head_position, down_requests[0], abs(initial_disk_head_position - down_requests[0]));
    }

    if(nof_down_requests > 0)
    {
        for(int index = 0; index < nof_down_requests - 1; index++)
        {
            if(debug_mode == true) printf("abs(%d - %d) = %d\n", down_requests[index], down_requests[index + 1], abs(down_requests[index] - down_requests[index + 1]));
            nof_head_movements += abs(down_requests[index] - down_requests[index + 1]);
        }
    }

    return nof_head_movements;
}