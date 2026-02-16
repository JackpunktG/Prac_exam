// define DEBUG to have asserts on and debug printouts to stderr
#ifndef DEBUG
#define NDEBUG
#endif
#ifdef DEBUG
#define DEBUG_PRINT(fmt, ...) fprintf(stderr, "[DEBUG] %s:%d:%s(): " fmt, \
        __FILE__, __LINE__, __func__, __VA_ARGS__)
#else
#define DEBUG_PRINT(fmt, ...) // Do nothing in release builds

#endif

#define ARENA_MEMORY_IMPLEMENTATION
#include "lib/arena_memory.h"
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <time.h>

typedef struct
{
    char* data;
    uint64_t length;
} String;

typedef enum
{
    FRAGE_STANDARD,
    FRAGE_DIREKT_PDF,
    FRAGE_PDF_TEIL
} Frage_Type;

typedef struct
{
    uint32_t thema;
    uint8_t frage_sub_count;
    uint8_t wichtigkeit;
    uint8_t gewaehlt_flag;
    uint8_t frage_data_type;
    String* szenario;
    String** fragen;
    String** schreib_platz;
} Frage;

typedef struct
{
    Frage** frage;
    uint16_t frage_count;
    uint16_t thema_count;
    String* thema_string[32];
    Arena* arena;
} Frage_Controller;

Frage_Controller* controller_init(const char* path);
void print(String* string);


void print_binary_32(const uint32_t var_32)
{
    for (int i = 31; i >= 0; --i)
    {
        if (var_32 & (1<<i))
            printf("1");
        else printf("0");
    }
    printf("\n");
}

void print_binary_16(const uint16_t var_16)
{
    for (int i = 15; i >= 0; --i)
    {
        if (var_16 & (1<<i))
            printf("1");
        else printf("0");
    }
    printf("\n");
}

void print_binary_8(const uint8_t var_8)
{

    for (int i = 7; i >= 0; --i)
    {
        if (var_8 & (1<<i))
            printf("1");
        else printf("0");
    }
    printf("\n");
}

void print(String* string)
{
    for(uint32_t i = 0; i < string->length; ++i)
        printf("%c",string->data[i]);
    printf("\n");
}

String* string_cbuf(Frage_Controller* controller, const char* buffer)
{
    const uint32_t length = strlen(buffer);
    String* s = (String*)arena_alloc(controller->arena, sizeof(String) + sizeof(char) * length, NULL);
    s->length = length;
    s->data = (char*)(s +1);

    memcpy(s->data, buffer, length);
    return s;
}
String* string_cnbuf(Frage_Controller* controller, const char* buffer, uint32_t lower_index, uint32_t upper_index)
{
    const uint32_t length = upper_index - lower_index;
    String* s = (String*)arena_alloc(controller->arena, sizeof(String) + sizeof(char) * length, NULL);
    s->length = length;
    s->data = (char*)(s +1);

    memcpy(s->data, buffer + lower_index, length);
    return s;
}


bool string_ccmp(String* s1, String* s2)
{
    if (s1->length != s2->length)
        return false;

    for(uint32_t i = 0; i < s1->length; ++i)
    {
        char a = (s1->data[i] >= 'A' && s1->data[i] <= 'Z') ? s1->data[i] | 0x20 : s1->data[i];
        char b = (s2->data[i] >= 'A' && s2->data[i] <= 'Z') ? s2->data[i] | 0x20 : s2->data[i];
        if (a != b)
            return false;
    }
    return true;
}

bool string_cmp(String* s1, String* s2)
{
    if (s1->length != s2->length)
        return false;

    for(uint32_t i = 0; i < s1->length; ++i)
    {
        if (s1->data[i] != s2->data[i])
            return false;
    }
    return true;
}

void string_trim(String* string)
{
    uint32_t start_index = 0;
    uint32_t end_index = string->length - 1;

    while ((string->data[start_index] == ' ' || string->data[start_index] == '\n' || string->data[start_index] == '\t') && start_index < string->length)
        ++start_index;

    while ((string->data[end_index] == ' ' || string->data[end_index] == '\n' || string->data[end_index] == '\t') && end_index > 0)
        --end_index;

    if (start_index > end_index)
        string->length = 0;
    else
    {
        uint32_t new_length = end_index - start_index + 1;
        memmove(string->data, string->data + start_index, new_length);
        string->length = new_length;
    }

}

void string_prep(String* string)
{
    string_trim(string);
    if (string->data[0] >= 'a' && string->data[0] <= 'z')
        string->data[0] &= ~0x20;
    for (uint32_t i = 1; i < string->length; ++i)
    {
        if (string->data[i] >= 'A' && string->data[i] <= 'Z')
            string->data[i] |= 0x20;
    }
}

String* string_from_token(const char* buffer)
{
    String* string = (String*)malloc(sizeof(String*));
    uint32_t length = strlen(buffer);
    string->data = (char*)malloc(sizeof(char) * length);
    string->length = length;
    memcpy(string->data, buffer, length);
    string_prep(string);

    return string;
}

void free_string(String* string)
{
    free(string->data);
    free(string);
}

uint8_t thema_bitwise(Frage_Controller* controller, String* thema)
{
    if (controller->thema_count < 1)
    {
        controller->thema_string[0] = (String*)arena_alloc(controller->arena, sizeof(String) + sizeof(char) * thema->length, NULL);
        controller->thema_string[0]->length = thema->length;
        controller->thema_string[0]->data = (char*)(controller->thema_string[0] + 1);
        memcpy(controller->thema_string[0]->data, thema->data, sizeof(char) * thema->length);
        ++controller->thema_count;
        return 0;
    }

    for (uint8_t i = 0; i < controller->thema_count; ++i)
    {
        if (string_ccmp(controller->thema_string[i], thema))
            return i;
    }

    controller->thema_string[controller->thema_count] = (String*)arena_alloc(controller->arena, sizeof(String) + sizeof(char) * thema->length, NULL);
    controller->thema_string[controller->thema_count]->length = thema->length;
    controller->thema_string[controller->thema_count]->data = (char*)(controller->thema_string[controller->thema_count] + 1);
    memcpy(controller->thema_string[controller->thema_count]->data, thema->data,sizeof(char) * thema->length);
    ++controller->thema_count;
    return controller->thema_count -1;
}

typedef enum
{
    UNDEFINED,
    THEMA,
    FRAGE_SUB_COUNT,
    SZENARIO,
    FRAGE,
    FRAGE_NUMBER,
    SCHREIB_PLATZ,
    FRAGE_FLAGGEN
} Line_Spec;


Frage_Controller* controller_init(const char* path)
{
    // counts the number of questions
    uint32_t frage_count = 0;
    char buffer[512];
    FILE* file_ptr = fopen(path, "r");
    while(fgets(buffer, sizeof(buffer), file_ptr))
        if (strncmp(buffer, "#frage = {", 10) == 0)
            ++frage_count;

    rewind(file_ptr);



    Arena* arena = arena_init(ARENA_BLOCK_SIZE, 8);
    if (arena == NULL)
        return NULL;
    Frage_Controller* controller = (Frage_Controller*)arena_alloc(arena, sizeof(Frage_Controller), NULL);
    memset(controller, 0, sizeof(Frage_Controller));
    controller->arena = arena;
    controller->frage_count = frage_count;
    controller->frage = (Frage**)arena_alloc(controller->arena, sizeof(String*) * controller->frage_count, NULL);


    memset(buffer, 0, sizeof(buffer));
    Line_Spec line_spec = UNDEFINED;
    Frage* input_frage = NULL;
    uint8_t sub_frage = 0;
    uint16_t frage_index = 0;

    while(fgets(buffer, sizeof(buffer), file_ptr))
    {
        if (line_spec == UNDEFINED && strncmp(buffer, "#frage = {", 10) == 0)
        {
            line_spec = FRAGE;
            if (input_frage != NULL)
                assert(false && "ERROR - null expected when parsing next frage");
            input_frage = (Frage*)arena_alloc(controller->arena, sizeof(Frage), NULL);
        }
        else if (line_spec == FRAGE && strncmp(buffer, "[Thema]", 7) == 0)
            line_spec = THEMA;
        else if (line_spec == THEMA)
        {
            char* themen = (char*)malloc(sizeof(char) * strlen(buffer));
            memcpy(themen, buffer, sizeof(char) * strlen(buffer));
            char* token = strtok(themen, ",");
            while(token != NULL)
            {
                String* thema = string_from_token(token);
                input_frage->thema |= (1<<thema_bitwise(controller, thema));
                free_string(thema);
                thema = NULL;
                token = strtok(NULL, ",");
            }
            line_spec = FRAGE_SUB_COUNT;
            free(token);
            free(themen);
        }
        else if (line_spec == FRAGE_SUB_COUNT)
        {
            if (strncmp(buffer, "[PDF_page_count: ", 17) == 0) //direkt pdf
            {
                sscanf(buffer, "[PDF_page_count: %hhu]", &input_frage->frage_sub_count);
                input_frage->frage_data_type = FRAGE_DIREKT_PDF;
            }
            else if (strncmp(buffer, "[PDF_part: ", 11) == 0)
            {
                sscanf(buffer, "[PDF_part: %hhu]", &input_frage->frage_sub_count);
                input_frage->frage_data_type = FRAGE_PDF_TEIL;
            }
            else
            {
                sscanf(buffer, "[Frage_sub_count: %hhu]", &input_frage->frage_sub_count);
                input_frage->fragen = (String**)arena_alloc(arena, sizeof(String*) * input_frage->frage_sub_count, NULL);
                input_frage->schreib_platz = (String**)arena_alloc(arena, sizeof(String*) * input_frage->frage_sub_count, NULL);
            }
            line_spec = SZENARIO;
        }
        else if (line_spec == SZENARIO)
        {
            if (input_frage->frage_data_type == FRAGE_DIREKT_PDF || input_frage->frage_data_type == FRAGE_PDF_TEIL) //direkt pdf
            {
                input_frage->szenario = string_cnbuf(controller, buffer, 6, strlen(buffer) - 2);
                line_spec = FRAGE_FLAGGEN;
            }
            else
            {
                uint8_t lines;
                sscanf(buffer, "[Szenario: %hhu]", &lines);

                char szenario[2000];
                uint32_t i = 0;
                uint8_t count = 0;
                while(fgets(buffer, sizeof(buffer), file_ptr))
                {
                    memcpy(szenario + i, buffer, sizeof(char) * strlen(buffer));
                    i += strlen(buffer);
                    ++count;
                    if (count == lines)
                        break;
                }
                szenario[--i] = '\0'; // remove the last newline character
                input_frage->szenario = string_cbuf(controller, szenario);

                input_frage->frage_data_type = FRAGE_STANDARD;
                line_spec = FRAGE_NUMBER;
            }
        }
        else if (line_spec == FRAGE_NUMBER)
        {
            input_frage->fragen[sub_frage] = string_cnbuf(controller, buffer, 8, strlen(buffer) - 2);
            line_spec = SCHREIB_PLATZ;
        }
        else if (line_spec == SCHREIB_PLATZ)
        {
            input_frage->schreib_platz[sub_frage++] = string_cnbuf(controller, buffer, 22, strlen(buffer) - 2);

            if (sub_frage == input_frage->frage_sub_count)
            {
                sub_frage = 0;
                line_spec = FRAGE_FLAGGEN;
            }
            else
                line_spec = FRAGE_NUMBER;
        }
        else if (line_spec == FRAGE_FLAGGEN)
        {
//[00000000, wichtigkeit: 1]
            uint8_t wichtigkeit;
            uint8_t flaggen[9];
            sscanf(buffer, "[%8[01], wichtigkeit: %hhu]", flaggen, &wichtigkeit);
            for (uint8_t i = 0; i < 8; ++i)
                if (flaggen[i] == '1')
                    input_frage->gewaehlt_flag |= (1<<(7-i));

            if (wichtigkeit > 5)
                input_frage->wichtigkeit = 5;
            else if (wichtigkeit == 0)
                input_frage->wichtigkeit = 1;
            else
                input_frage->wichtigkeit = wichtigkeit;

            controller->frage[frage_index++] = input_frage;
            input_frage = NULL;
            line_spec = UNDEFINED;
        }

    }
    return controller;
}

String* read_entire_file(Frage_Controller* controller, const char* path)
{
    // Getting file size
    char ch;
    uint32_t length = 0;
    FILE *fptr = fopen(path, "r");
    while ((ch = fgetc(fptr)) != EOF)
        ++length;

    rewind(fptr);

    String* string = (String*)arena_alloc(controller->arena, sizeof(String) + length * sizeof(char), NULL);
    if (string == NULL)
        return NULL;
    string->length = length;
    string->data = (char*)(string + 1);

    uint32_t i = 0;
    while ((ch = fgetc(fptr)) != EOF)
    {
        assert(string->length != i && "ERROR - whilst copying string data, lenght incorrectly calculated");
        string->data[i++] = ch;
    }

    fclose(fptr);

    return string;
}

// Returns 1 on success, 0 on failure (entropy exhausted)
static inline int rdseed32(uint32_t *seed)
{
    unsigned char ok;
    __asm__ __volatile__ (
        "rdseed %0\n\t"
        "setc %1"
        : "=r" (*seed), "=qm" (ok)
    );
    return (int)ok;
}
uint32_t random_seed()
{
    uint32_t seed;

    int retries = 10;
    while (retries-- > 0)
    {
        if (rdseed32(&seed))
        {
            DEBUG_PRINT("RDSEED succeeded: seed = 0x%08X\n", seed);
            return seed;
        }
    }
    assert(false && "ERROR - RDSEED failed\n");
    return 0;
}


uint32_t* fragen_auswaehlen(Frage_Controller* controller, uint8_t frage_count)
{
    if (controller->frage_count < frage_count)
    {
        printf("Frage menge nicht groß genug\n");
        return NULL;
    }
    uint32_t* frage_index = (uint32_t*)arena_alloc(controller->arena, sizeof(uint32_t) * frage_count, NULL);

    srand(random_seed());

    for (uint8_t i = 0; i < frage_count; )
    {
        uint32_t tmp = rand() % (controller->frage_count);
        bool unique = true;
        for(uint8_t k = 0; k < i; ++k)
            if (tmp == frage_index[k])
                unique = false;
        if (unique)
            frage_index[i++] = tmp;
    }

    return frage_index;
}

uint32_t first_page(String* string)
{
    char tmp[24];
    uint8_t i = 0;
    while(string->data[i] != ' ')
    {
        tmp[i] = string->data[i];
        ++i;
    }
    tmp[++i] = '\0';

    memmove(string->data, string->data + i, string->length - i);
    string->length -= (i);

    return atoi(tmp);
}

enum
{
    LINIEN = 'L',
    BILD = 'I',
    PLATZ = 'P'
};

void pruefung_generieren(Frage_Controller* controller, uint32_t* fragen_index, const uint8_t frage_count, const bool delete_typst_file, const char* datei_name)
{
    time_t seconds = time(NULL);
    // Convert to broken-down local time
    struct tm *local = localtime(&seconds);
    char buffer[128];
    strftime(buffer, sizeof(buffer), "%h_%m_%s_pruefung.typ", local);
    printf("Generating file: %s\n", buffer);
    bool from_pdf = false;

    FILE* file_ptr = fopen(buffer, "w");

    fprintf(file_ptr, "#set page(margin: (x: 1.5cm, y: 1.5cm))\n");
    fprintf(file_ptr, "#set text(size: 9pt, font: \"P052\")\n\n");

    for (uint8_t i = 0; i < frage_count; ++i)
    {
        Frage* frage = controller->frage[fragen_index[i]];

        switch (frage->frage_data_type)
        {
        case FRAGE_STANDARD:
        {
            if (from_pdf)
                fprintf(file_ptr, "#pagebreak()\n");
            from_pdf = false;

            fprintf(file_ptr, "= Frage: %u\n", i +1);

            fwrite(frage->szenario->data, 1, frage->szenario->length, file_ptr);
            fputc('\\', file_ptr);
            fputc('\n', file_ptr);

            for (uint8_t k = 0; k < frage->frage_sub_count; ++k)
            {
                fwrite(frage->fragen[k]->data, 1, frage->fragen[k]->length, file_ptr);
                fputc('\\', file_ptr);
                fputc('\n', file_ptr);
                switch (frage->schreib_platz[k]->data[0])
                {
                case LINIEN:
                {
                    uint8_t lines;
                    sscanf(frage->schreib_platz[k]->data, "L%hhu", &lines);
                    for(uint8_t j = 0; j < lines; ++j)
                        fprintf(file_ptr, "#v(1em) #line(length: 100%%)\n");
                    break;
                }
                case BILD:
                {
                    char path[100];
                    memcpy(path, frage->schreib_platz[k]->data + 2, frage->schreib_platz[k]->length - 3);
                    path[frage->schreib_platz[k]->length -3] = '\0';
                    fprintf(file_ptr, "#image(\"IHK_Fragen/dateien/%s\", height: 60%%)\n", path);
                    break;
                }
                case PLATZ:
                {
                    uint8_t lines;
                    sscanf(frage->schreib_platz[k]->data, "P%hhu", &lines);
                    fprintf(file_ptr, "#v(%uem)\n", lines);
                    break;
                }
                default:
                    assert(false);
                }
            }
            break;
        }
        case FRAGE_DIREKT_PDF:
        {
            char path[100];
            memcpy(path, frage->szenario->data, frage->szenario->length);
            path[frage->szenario->length] = '\0';
            if (i != 0)
                fprintf(file_ptr, "#pagebreak()\n");
            for(uint8_t k = 0; k < frage->frage_sub_count; ++k)
            {
                fprintf(file_ptr, "#align(horizon + center)[#image(\"IHK_Fragen/dateien/%s\", page: %i, width: 100%%, fit: \"contain\")]", path, k +1);
                if (k +1 == frage->frage_sub_count && i +1 == frage_count)
                    break;
                if (k +1 != frage->frage_sub_count)
                    fprintf(file_ptr, "\n\n#pagebreak()");
            }
            from_pdf = true;
            break;
        }
        case FRAGE_PDF_TEIL:
        {
            char path[100];
            uint32_t start_page_number = first_page(frage->szenario);
            memcpy(path, frage->szenario->data, frage->szenario->length);
            path[frage->szenario->length] = '\0';
            if (i != 0)
                fprintf(file_ptr, "#pagebreak()\n");
            for(uint8_t k = 0; k < frage->frage_sub_count; ++k)
            {
                fprintf(file_ptr, "#align(horizon + center)[#image(\"IHK_Fragen/dateien/%s\", page: %u, width: 100%%, fit: \"contain\")]", path, start_page_number++);
                if (k +1 == frage->frage_sub_count && i +1 == frage_count)
                    break;
                if (k +1 != frage->frage_sub_count)
                    fprintf(file_ptr, "\n\n#pagebreak()");
            }
            from_pdf = true;
            break;
        }
        default:
            assert(false && "ERROR - unknown frage data type");

        }

        fprintf(file_ptr, "\n\n");
    }

    fclose(file_ptr);


    char command[256];
    snprintf(command, sizeof(command), "typst compile %s", buffer);

    int compile_typst = system(command);

    if (compile_typst == -1)
    {
        perror("system() failed");
    }
    else
    {
        printf("Command exited with code: %d\n", WEXITSTATUS(compile_typst));
    }

    if (delete_typst_file)
    {
        if (remove(buffer) != 0)
            perror("Error deleting file");
        else
            printf("File deleted successfully\n");
    }
    if (datei_name != NULL)
    {
        if (!delete_typst_file)
        {
            char rename_command[256];
            snprintf(rename_command, sizeof(rename_command), "mv %s %s.typ", buffer, datei_name);
            int move_result = system(rename_command);
            if (move_result == -1)
            {
                perror("system() failed");
            }
            else
            {
                printf("Move command exited with code: %d\n", WEXITSTATUS(move_result));
            }
        }
        buffer[strlen(buffer) - 4] = '\0';
        char pdf_name[128];
        snprintf(pdf_name, sizeof(pdf_name), "%s", buffer);
        char rename_pdf_command[256];
        snprintf(rename_pdf_command, sizeof(rename_pdf_command), "mv %s.pdf %s.pdf", pdf_name, datei_name);
        int rename_pdf_result = system(rename_pdf_command);
        if (rename_pdf_result == -1)
        {
            perror("system() failed");
        }
        else
        {
            printf("Rename PDF command exited with code: %d\n", WEXITSTATUS(rename_pdf_result));
        }
    }


    return;
}

