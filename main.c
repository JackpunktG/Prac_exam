#include "question_parse.h"

int main(int argc, char* argv[])
{
    Frage_Controller* controller = controller_init("IHK_Fragen/IHK_Fragen.txt");

    if (argc < 2)
    {
        printf("Args: %s <fragen_anzahl> <y*> <datei_name*>\n*optional; <y> typst file zu behalten\n", argv[0]);
        return 1;
    }
    const uint8_t frage_anzahl = atoi(argv[1]);
    printf("Frage anzahl: %u\n", frage_anzahl);
    if (frage_anzahl < 1 || frage_anzahl > controller->frage_count)
    {
        printf("Frage anzahl muss zwischen 1 and %u sein\n", controller->frage_count > 255 ? 255 : controller->frage_count);
        return 1;
    }
    const bool keep_typst = (argc > 3 && argv[2][0] == 'y') || (argc == 3 && argv[2][0] == 'y' && argv[2][1] == '\0');


    pruefung_generieren(controller, fragen_auswaehlen(controller, frage_anzahl), frage_anzahl, !keep_typst, argc > 3 ? argv[3] : NULL);
    arena_destroy(controller->arena);

    return 0;
}
