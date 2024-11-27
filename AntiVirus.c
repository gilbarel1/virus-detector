#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INPUT_SIZE 10

typedef struct virus {
    unsigned short SigSize;
    char virusName[16];
    unsigned char* sig;
} virus;

typedef struct link link;
struct link {
link *nextVirus;
virus *vir;
};

char magicNum[4];
link* list_of_viruses = NULL;

// used fread manual and the assignment to understand how to read the virus struct from the file
virus* readVirus(FILE* file){

   /*Checking wether to file position reached the end of the file. */
   if (feof(file)) {
        return NULL;
    }

    virus *v = malloc(sizeof(virus));
    if (v == NULL) {
        printf("Memory allocation failed\n");
        return NULL;
    }

    if (fread(&v->SigSize, sizeof(unsigned short), 1, file) != 1) {
        free(v);
        return NULL; // Could be end of file or read error
    }

    /* The ntohs() function converts the unsigned short integer netshort
     from network byte order to host byte order.
     got insights from here: https://linux.die.net/man/3/ntohs 
    */
    if(memcmp(magicNum, "VIRB", 4) == 0)
        v->SigSize = ntohs(v->SigSize);

    if (fread(v->virusName, sizeof(char), 16, file) != 16) {
        free(v);
        return NULL;
    }

    v->virusName[15] = '\0';
    
    // Allocate memory for sig based on sigSize
    v->sig = malloc(v->SigSize);
    if (v->sig == NULL) {
        printf("Memory allocation failed for signature\n");
        free(v);
        return NULL;
    }

    // Read sig (sigSize bytes)
    if (fread(v->sig, sizeof(unsigned char), v->SigSize, file) != v->SigSize) {
        free(v->sig);
        free(v);
        return NULL;
    }
    return v;
}

// My code

void printVirus(virus* virus, FILE* output){
    fprintf(output, "Virus name: %s\n", virus->virusName);
    fprintf(output, "Virus size: %d\n", virus->SigSize);
    fprintf(output, "signature:\n");
    for(int i = 0; i < virus->SigSize; i++){
        fprintf(output, "%02hhx ", virus->sig[i]);
    }
    fprintf(output, "\n\n");
}

void list_print(link *virus_list, FILE* file){
    link *current = virus_list;
    while (current != NULL){
        printVirus(current->vir, file);
        current = current->nextVirus;
    }
}

link* list_append(link* virus_list, virus* data){
    link *newLink = malloc(sizeof(link));
    if (newLink == NULL) {
        printf("Memory allocation failed\n");
        return NULL;
    }
    newLink->vir = data;
    newLink->nextVirus = NULL;
    if (virus_list == NULL) {
        return newLink;
    }
    link *current = virus_list;
    while (current->nextVirus != NULL) {
        current = current->nextVirus;
    }
    current->nextVirus = newLink;
    return virus_list;
}

void list_free(link *virus_list){
    link *current = virus_list;
    while (current != NULL){
        link *next = current->nextVirus;
        free(current->vir->sig);
        free(current->vir);
        free(current);
        current = next;
    }
}

// My code

void load_sig(){
    FILE* file;
    char fileName[256];
    printf("Enter signature file name: ");
    fgets(fileName, 256, stdin);
    sscanf(fileName, "%s", fileName);
    file = fopen(fileName, "r");
    if (file == NULL) {
        printf("Error: Failed to open file %s\n", fileName);
        return;
    }
    fread(magicNum, sizeof(char), 4, file);
    if(memcmp(magicNum, "VIRB", 4) != 0 && memcmp(magicNum, "VIRL", 4) != 0){
        printf("Error: File is not a virus signature file\n");
    }
    virus *v;
    while ((v = readVirus(file)) != NULL) {
        list_of_viruses = list_append(list_of_viruses, v);
    }
    fclose(file);
    if(list_of_viruses != NULL)
        printf("Loaded %s successfully\n", fileName);
}

void print_signatures(){
    list_print(list_of_viruses, stdout);
}

// My code

void detect_virus(char *buffer, unsigned int size, link *virus_list) {
    link *current = virus_list;
    while (current != NULL) {
        virus *v = current->vir;
        unsigned int sigSize = v->SigSize;
        for (unsigned int i = 0; i <= size - sigSize; i++) {
            if (memcmp(buffer + i, v->sig, sigSize) == 0) {
                printf("Starting byte location: %02X\n", i);
                printf("Virus name: %s\n", v->virusName);
                printf("Signature size: %u\n\n", sigSize);
            }
        }
        current = current->nextVirus;
    }
}

// My code

void detect_viruses(){
    char fileName[256];
    printf("Enter suspected file name: ");
    fgets(fileName, sizeof(fileName), stdin);
    sscanf(fileName, "%s", fileName);

    FILE *file = fopen(fileName, "rb");
    if (file == NULL) {
        printf("Error: Failed to open file %s\n", fileName);
        return;
    }

    unsigned int size = 10000; // 10KB buffer
    char *buffer = malloc(size);
    if (buffer == NULL) {
        printf("Memory allocation failed\n");
        fclose(file);
        return;
    }

    unsigned int bytesRead = fread(buffer, 1, size, file); // Read 10KB from file into buffer
    fclose(file);

    detect_virus(buffer, bytesRead, list_of_viruses);
    free(buffer);
}

// My code

void neutralize_virus(char *fileName, int signatureOffset){
    char ret = 0xC3; // Return opcode
    FILE *file = fopen(fileName, "r+b");
    if (file == NULL) {
        printf("Error: Failed to open file %s\n", fileName);
        return;
    }
    fseek(file, signatureOffset, SEEK_SET);
    fwrite(&ret, 1, 1, file);
    fclose(file);
}

// My code

void fix_file(){
    char fileName[256];
    printf("Enter suspected file name: ");
    fgets(fileName, sizeof(fileName), stdin);
    sscanf(fileName, "%s", fileName);
    FILE *file = fopen(fileName, "rb");
    if (file == NULL) {
        printf("Error: Failed to open file %s\n", fileName);
        return;
    }

    unsigned int size = 10000; // 10KB buffer
    char *buffer = malloc(size);
    if (buffer == NULL) {
        printf("Memory allocation failed\n");
        fclose(file);
        return;
    }

    fread(buffer, 1, size, file); // Read 10KB from file into buffer

    link* current = list_of_viruses;
    while (current != NULL) {
        virus *v = current->vir;
        unsigned int sigSize = v->SigSize;
        for (unsigned int i = 0; i <= size - sigSize; i++) {
            if (memcmp(buffer + i, v->sig, sigSize) == 0) {
                neutralize_virus(fileName, i); // Neutralize the virus
                if (v->sig != NULL && v->SigSize > 0) {
                    v->sig[0] = 0xC3; // Set the first byte to the RET instruction (0xC3) in the list of viruses
                }
            }
        }
        current = current->nextVirus;
    }
    fclose(file);
    free(buffer);
}

void quit(){
    free(list_of_viruses);
    exit(0);
}

/* I used the similar code in Lab 1 from here */

struct fun_desc {
  char *name;
  void (*fun)(void);
};

struct fun_desc menu[] = { 
{ "Load signatures", load_sig },
{ "Print signatures", print_signatures },
{ "Detect viruses", detect_viruses },
{ "Fix file", fix_file },
{ "Quit", quit }, 
{ NULL, NULL } };

void menu_display(){
    int i = 0;
    printf("Select operation from the following menu (ctrl^D for exit):\n");
    while (menu[i].name != NULL){
        printf("%d) %s\n", (i+1), menu[i].name);
        i++;
    }
    printf("Option: ");
}

int main(int argc, char **argv){
    int option;
    list_of_viruses = NULL;
    char input[INPUT_SIZE];
    int menu_size = sizeof(menu) / sizeof(struct fun_desc) - 1;
    menu_display();
    while (fgets(input, INPUT_SIZE, stdin) != NULL){
        sscanf(input, "%d", &option);
        if (option < 1 || option > menu_size){
            printf("Not within bounds\n");
        } else {
            printf("Within bounds\n");
            menu[option-1].fun();
        }
        printf("\n");
        menu_display();
    }
    quit();
    return 0;
}