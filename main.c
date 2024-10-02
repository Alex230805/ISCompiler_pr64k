#include <stdio.h>
#include <stdlib.h>
#include "lib/list.h"
#include <ctype.h>


#define COMPILER_DEBUG_INFO


void showData(listHeader * InstructionList);

uint16_t hexStringConverter(char string[]);
int write(uint8_t buffer[], uint16_t opcode, int step, FILE * pointer);
uint16_t hexDigitConverter(char s);

//path to mnemonic microcode reference
char path_microcode_1[] = "./RomDatabase/rom1/micro.micunit";
char path_microcode_2[] = "./RomDatabase/rom2/micro.micunit";
char path_microcode_3[] = "./RomDatabase/rom3/micro.micunit";
char path_microcode_4[] = "./RomDatabase/rom4/micro.micunit";
char path_microcode_5[] = "./RomDatabase/flag_rom/micro.f";

//path to hexadecimal microcode reference
char path_hex_1[] = "./RomDatabase/rom1/rom_hex.micunit";
char path_hex_2[] = "./RomDatabase/rom2/rom_hex.micunit";
char path_hex_3[] = "./RomDatabase/rom3/rom_hex.micunit";
char path_hex_4[] = "./RomDatabase/rom4/rom_hex.micunit";
char path_hex_5[] = "./RomDatabase/flag_rom/rom_hex.f";
//path to binary file
char path_bin_1[] = "./out/rom1.bin";
char path_bin_2[] = "./out/rom2.bin";
char path_bin_3[] = "./out/rom3.bin";
char path_bin_4[] = "./out/rom4.bin";
char path_bin_5[] = "./out/flag_rom.bin";


char path_instruction_set[][128] = {

};

const int INST_LIMIT = 162; // remember to set the instruction limit 

int main(){
    FILE * bin1;
    FILE * bin2;
    FILE * bin3;
    FILE * bin4;
    FILE * bin5;
    char sel;
    //define main struct 
    listHeader InstructionList = initializeHeader();
    romContainer RomData = initializeRomContainer();

    //load database
    loadDatabase(path_hex_1, path_microcode_1,&RomData, rom1);
    loadDatabase(path_hex_2, path_microcode_2,&RomData, rom2);
    loadDatabase(path_hex_3, path_microcode_3,&RomData, rom3);
    loadDatabase(path_hex_4, path_microcode_4,&RomData, rom4);
    loadDatabase(path_hex_5, path_microcode_5, &RomData, rom5);

    #if defined (COMPILER_DEBUG_INFO)
        printf("1st rom -> database loaded\n");
        printf("2nd rom -> database loaded\n");
        printf("3rd rom -> database loaded\n");
        printf("4th rom -> database loaded\n");
        printf("5ft rom -> database loaded\n");
    #endif
    
    for(int i=0;i<INST_LIMIT+8;i++){
        char buffer_in_1[64][64];
        char buffer_in_2[64][64];
        char buffer_in_3[64][64];
        char buffer_in_4[64][64];
        char buffer_in_5[64][64];
        node * n;
        FILE * inst; 
        if((inst = fopen(path_instruction_set[i], "r")) == NULL){
            exit(0);
        }else{
            n = (node*)malloc(sizeof(node));
            n->next = NULL;
            fscanf(inst, "%s %s %d", &n->name, &n->ADDRESS, &n->step);
            if(i >= INST_LIMIT){
                for(int z=0;z<n->step;z++){
                    fscanf(inst, "%s", &buffer_in_5[z]);
                }
            }else{
                for(int z=0;z<n->step;z++){
                    fscanf(inst, "%s %s %s %s", &buffer_in_4[z], &buffer_in_3[z],&buffer_in_2[z],&buffer_in_1[z]);
                }
            }
            if(i >= INST_LIMIT){
                for(int z=0;z<n->step;z++){
                    strcpy(n->microcode_rom5[z], buffer_in_5[z]);
                }
                #if defined (COMPILER_DEBUG_INFO)
                    printf("Flag/status requeset Instructions parsed: %d\n", i+1);
                #endif
            }else{
                for(int z=0;z<n->step;z++){
                    strcpy(n->microcode_rom1[z], buffer_in_1[z]);
                    strcpy(n->microcode_rom2[z], buffer_in_2[z]);
                    strcpy(n->microcode_rom3[z], buffer_in_3[z]);
                    strcpy(n->microcode_rom4[z], buffer_in_4[z]);
                }
                #if defined (COMPILER_DEBUG_INFO)
                    printf("Instructions parsed: %d\n", i+1);
                #endif
            }
            listAddInstNode(&InstructionList,n);
        }
        fclose(inst);
    }

    printf("Compiling");
    for(int i=0;i<INST_LIMIT+8;i++){
        char buffer_out_1[64][64];
        char buffer_out_2[64][64];
        char buffer_out_3[64][64];
        char buffer_out_4[64][64];
        char buffer_out_5[64][64];
        for(int z=0;z<InstructionList.pointer->step;z++){
            if(i >= INST_LIMIT){
                extractFromDatabase(&RomData, InstructionList.pointer->microcode_rom5[z], buffer_out_5[z], rom5);
                #if defined (COMPILER_DEBUG_INFO)
                    printf("Info extracted in micro-step %d for flag return control signal %s: %s\n",i+1,InstructionList.pointer->name,buffer_out_5[z]);
                #endif
                strcpy(InstructionList.pointer->microcode_rom5[z], buffer_out_5[z]);
            }else{
                extractFromDatabase(&RomData, InstructionList.pointer->microcode_rom1[z],buffer_out_1[z], rom1);
                extractFromDatabase(&RomData, InstructionList.pointer->microcode_rom2[z],buffer_out_2[z], rom2);
                extractFromDatabase(&RomData, InstructionList.pointer->microcode_rom3[z],buffer_out_3[z], rom3);
                extractFromDatabase(&RomData, InstructionList.pointer->microcode_rom4[z],buffer_out_4[z], rom4);
                #if defined (COMPILER_DEBUG_INFO)
                    printf("Info extracted in micro-step %d for instruction %s : %s %s %s %s\n",i+1,InstructionList.pointer->name,buffer_out_4[z],buffer_out_3[z],buffer_out_2[z],buffer_out_1[z]);
                #endif
                strcpy(InstructionList.pointer->microcode_rom1[z],buffer_out_1[z]);
                strcpy(InstructionList.pointer->microcode_rom2[z],buffer_out_2[z]);
                strcpy(InstructionList.pointer->microcode_rom3[z],buffer_out_3[z]);
                strcpy(InstructionList.pointer->microcode_rom4[z],buffer_out_4[z]);
            }
        }
        if(InstructionList.pointer->next != NULL){
            InstructionList.pointer = InstructionList.pointer->next;
        }

    }
    InstructionList.pointer = InstructionList.first;
    
    
    printf("Saving compiled instruction set");
    bin1 = fopen(path_bin_1, "w+b");
    bin2 = fopen(path_bin_2, "w+b");
    bin3 = fopen(path_bin_3, "w+b");
    bin4 = fopen(path_bin_4, "w+b");
    bin5 = fopen(path_bin_5, "w+b");
    if(bin1 == NULL || bin2 == NULL || bin3 == NULL || bin4 == NULL || bin5 == NULL){
        #if defined (COMPILER_DEBUG_INFO)
            printf("\nFailed while opening output binary file, aborting ..\n");
        #endif
        exit(0);
    }else{
        uint8_t buffer_out_1[64];
        uint8_t buffer_out_2[64];
        uint8_t buffer_out_3[64];
        uint8_t buffer_out_4[64];
        uint8_t buffer_out_5[64];
        for(int i=0;i<INST_LIMIT+8;i++){
            if(i >= INST_LIMIT){
                for(int z=0;z<InstructionList.pointer->step;z++){
                    buffer_out_5[z] = hexStringConverter(InstructionList.pointer->microcode_rom5[z]);
                }
            }else{
                for(int z=0;z<InstructionList.pointer->step;z++){
                    buffer_out_1[z] = hexStringConverter(InstructionList.pointer->microcode_rom1[z]);
                    buffer_out_2[z] = hexStringConverter(InstructionList.pointer->microcode_rom2[z]);
                    buffer_out_3[z] = hexStringConverter(InstructionList.pointer->microcode_rom3[z]);
                    buffer_out_4[z] = hexStringConverter(InstructionList.pointer->microcode_rom4[z]);
                }
            }
            if(i >= INST_LIMIT){
                uint16_t address_f = hexStringConverter(InstructionList.pointer->ADDRESS);
                write(buffer_out_5, address_f, InstructionList.pointer->step,bin5);
            }else{
                uint16_t address = hexStringConverter(InstructionList.pointer->ADDRESS);
                write(buffer_out_1, address ,InstructionList.pointer->step,bin1);
                write(buffer_out_2, address ,InstructionList.pointer->step,bin2);
                write(buffer_out_3, address ,InstructionList.pointer->step,bin3);
                write(buffer_out_4, address ,InstructionList.pointer->step,bin4);
            }
            if(InstructionList.pointer->next != NULL){
                InstructionList.pointer = InstructionList.pointer->next;
            }
        }
    }
    fclose(bin1);
    fclose(bin2);
    fclose(bin3);
    fclose(bin4);
    fclose(bin5);
    InstructionList.pointer = InstructionList.first;
    printf("Show written data? (Y/N): ");
    scanf("%c", &sel);
    if(sel == 'Y' || sel == 'y'){
        showData(&InstructionList);
    }
    InstructionList.pointer = InstructionList.first;
    freeISET(&InstructionList);
    freeRomDatabase(&RomData);
    return 0;
}


void showData(listHeader * InstructionList){
    printf("\nData written to memory: \n\n");
    for(int i=0;i<INST_LIMIT;i++){
        printf("\nInstruction name: %s\n", InstructionList->pointer->name);
        printf("Instruction steps: %d\n", InstructionList->pointer->step);
        printf("Instruction address: %s\n", InstructionList->pointer->ADDRESS);
        for(int i=0;i<InstructionList->pointer->step;i++){
            printf("Data from step %d : %s %s %s %s\n", i+1, InstructionList->pointer->microcode_rom4[i],InstructionList->pointer->microcode_rom3[i],InstructionList->pointer->microcode_rom2[i],InstructionList->pointer->microcode_rom1[i]);
        }
        if(InstructionList->pointer->next != NULL){
            InstructionList->pointer = InstructionList->pointer->next;
        }
    }
}





uint16_t hexDigitConverter(char s){
    if(isdigit(s)){
        return s - '0';
    }else{
         return toupper(s) - 'A' + 10;
    }
}

uint16_t hexStringConverter(char string[]){
    uint16_t HexString = 0x0000;
    int len = strlen(string);
    for(int i=0;i<len;i++){
        if(!isxdigit(string[i]))
            return -1;
       int cache = hexDigitConverter(string[i]);
       HexString = (HexString << 4) | cache; 
    }
    return HexString;
}

int write(uint8_t buffer[], uint16_t opcode, int step, FILE *pointer){
    int statusOp = 0;
    uint16_t address = 0x0000;
    for(int i=0;i<step;i++){
        uint16_t wAdr = address | opcode;
        fseek(pointer ,wAdr, SEEK_SET);
        fwrite(&buffer[i],sizeof(char), 1, pointer);
        address = (address >> 8) & 0xFF;
        address++;
        if(address > 0xFF){
            address &= 0xFF;
        }
        address = (address << 8) |(opcode & 0xFF);
    }
    statusOp = 1;
    return statusOp;
}
