#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>


const int g_size_page_bytes = 256;
const int g_nof_pages = 256;
const int g_size_frame_bytes = 256;
const int g_nof_frames = 256;
const int g_nof_logical_addresses = 1000;
const int g_nof_tlb_entries = 16;
int g_frame_counter = 0;
int g_nof_page_faults = 0;
int g_nof_tlb_hits = 0;
int g_tlb_element_index = 0;
int g_logical_addresses[g_nof_logical_addresses];
int g_page_table[g_nof_pages];
int g_tlb[g_nof_tlb_entries][2];
char g_physical_memory[g_nof_frames * g_size_frame_bytes];


void Initialize_Page_Table();
void Initialize_TLB();
void Load_Logical_Addresses(char* file_name);
int* Calculate_Page_Number_And_Page_Offset(int logical_address);
int Get_Frame_Number_TLB(int page_number);
void Update_TLB(int page_number, int frame_number);
int Get_Frame_Number_Page_Table(int page_number);
int Handle_Page_Fault_And_Get_Frame_Number(int page_number);
void Load_Page_Into_Physical_Memory(int page_number);
int Get_Signed_Byte_Value_At_Physical_Address(int physical_address);


int main(int argc, char* argv[])
{
    Initialize_Page_Table();
    Initialize_TLB();
    Load_Logical_Addresses(argv[1]);

    for(int index_l_a = 0; index_l_a < g_nof_logical_addresses; index_l_a++)
    {
        int logical_address = g_logical_addresses[index_l_a];
        int* page_offset_number = Calculate_Page_Number_And_Page_Offset(logical_address);
        int page_offset = page_offset_number[0];
        int page_number = page_offset_number[1];
        free(page_offset_number);

        int frame_number = Get_Frame_Number_TLB(page_number);

        if(frame_number == -1)
        {
            frame_number = Get_Frame_Number_Page_Table(page_number);
            if(frame_number != -1) Update_TLB(page_number, frame_number);
        }
        else g_nof_tlb_hits++;

        if(frame_number == -1)
        {
            frame_number = Handle_Page_Fault_And_Get_Frame_Number(page_number);
            Update_TLB(page_number, frame_number);
            g_nof_page_faults++;
        }

        int physical_address = frame_number * g_size_frame_bytes + page_offset;

        int signed_byte_value = Get_Signed_Byte_Value_At_Physical_Address(physical_address);

        printf("Virtual Address: %d ", logical_address);
        printf("Physical Address: %d ", physical_address);
        printf("Value: %d\n", signed_byte_value);
    }

    printf("\nPage Fault Rate: %f\n", (double) g_nof_page_faults / (double) g_nof_logical_addresses);
    printf("TLB Hit Rate: %f\n", (double) g_nof_tlb_hits / (double) g_nof_logical_addresses);

    return 0;
}


void Initialize_Page_Table()
{
    for(int page_number = 0; page_number < g_nof_pages; page_number++)
    {
        g_page_table[page_number] = -1;
    }
}


void Initialize_TLB()
{
    for(int tlb_index = 0; tlb_index < g_nof_tlb_entries; tlb_index++)
    {
        int page_number = -1;
        int frame_number = -1;
        g_tlb[tlb_index][0] = page_number;
        g_tlb[tlb_index][1] = frame_number;
    }
}


void Load_Logical_Addresses(char* file_name)
{
    char file_path[100] = "data/";
    strcat(file_path, file_name);
    FILE* file_ptr = fopen(file_path, "r");
    int size_carr_logical_address = 10;
    char carr_logical_address[size_carr_logical_address];

    for(int index_l_a = 0; index_l_a < g_nof_logical_addresses; index_l_a++)
    {
        for(int index_c = 0; index_c < size_carr_logical_address; index_c++)
        {
            char character = fgetc(file_ptr);

            if(feof(file_ptr) == true) break;
            if(character == '\n')
            {
                carr_logical_address[index_c] = '\0';
                break;
            }

            carr_logical_address[index_c] = character;
        }

        if(feof(file_ptr) == true) break;
        else g_logical_addresses[index_l_a] = atoi(carr_logical_address);
    }

    fclose(file_ptr);
}


int* Calculate_Page_Number_And_Page_Offset(int logical_address)
{
    int int_b7_to_b0_mask = 0x000000ff;
    int int_b15_to_b8_mask = 0x0000ff00;

    int* page_offset_number = (int*) malloc(sizeof(int) * 2);

    page_offset_number[0] = (logical_address & int_b7_to_b0_mask);
    page_offset_number[1] = (logical_address & int_b15_to_b8_mask) >> 8;

    return page_offset_number;
}


int Get_Frame_Number_TLB(int page_number)
{
    for(int tlb_index = 0; tlb_index < g_nof_tlb_entries; tlb_index++)
    {
        if(g_tlb[tlb_index][0] == page_number)
        {
            return g_tlb[tlb_index][1];
        }
    }

    return -1;
}


void Update_TLB(int page_number, int frame_number)
{
    g_tlb[g_tlb_element_index][0] = page_number;
    g_tlb[g_tlb_element_index][1] = frame_number;
    g_tlb_element_index++;
    if(g_tlb_element_index > 15) g_tlb_element_index = 0;
}


int Get_Frame_Number_Page_Table(int page_number)
{
    return g_page_table[page_number];
}


int Handle_Page_Fault_And_Get_Frame_Number(int page_number)
{
    g_page_table[page_number] = g_frame_counter;
    Load_Page_Into_Physical_Memory(page_number);
    g_frame_counter++;

    return g_page_table[page_number];
}


void Load_Page_Into_Physical_Memory(int page_number)
{
    FILE* file_ptr = fopen("data/BACKING_STORE.bin", "r");
    fseek(file_ptr, page_number * g_size_page_bytes, SEEK_SET);
    fread(g_physical_memory + g_frame_counter * g_size_frame_bytes, sizeof(char), g_size_frame_bytes, file_ptr);
    fclose(file_ptr);
}


int Get_Signed_Byte_Value_At_Physical_Address(int physical_address)
{
    return (int) g_physical_memory[physical_address];
}