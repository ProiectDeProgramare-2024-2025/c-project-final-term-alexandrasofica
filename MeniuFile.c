#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

#ifdef _WIN32
#define CLEAR "cls"
#else
#define CLEAR "clear"
#endif

#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define RESET   "\033[0m"
#define PURPLE  "\033[35m"
#define WHITE   "\033[97m"
#define WHITE_BLUE "\033[36m"

#define NR_SALI 11
#define REZERVARI 100
#define OPEN 8
#define CLOSED 18
#define FILENAME "rezervare.txt"

typedef struct {
    int nr_sala;
    char nume[50];
    int rezervari[CLOSED - OPEN];
} Sala;

typedef struct {
    int nr_sala;
    int zi, luna, an;
    int ora;
    char nume_rezervant[50];
} Rezervare;

Sala sali[NR_SALI] = {
    {1, "Sala Conferinta", {0}},
    {2, "Sala Sedinte", {0}},
    {3, "Sala Brainstorming", {0}},
    {4, "Birou Open-Space Etaj 1", {0}},
    {5, "Birou Open-Space Etaj 2", {0}},
    {6, "Birou Open-Space Etaj 3", {0}},
    {7, "Sala IT", {0}},
    {8, "Laborator Testare", {0}},
    {9, "Sala Interviuri", {0}},
    {10, "Sala VIP", {0}},
    {11, "Sala de Recreere", {0}}
};

Rezervare rezervari[REZERVARI];
int nr_rezervari = 0;

int esteAnBisect(int an) {
    return (an % 4 == 0 && an % 100 != 0) || (an % 400 == 0);
}

int esteDataValida(int zi, int luna, int an) {
    if (an < 1900 || luna < 1 || luna > 12 || zi < 1) return 0;

    int zileLuna[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    if (esteAnBisect(an)) zileLuna[1] = 29;

    return zi <= zileLuna[luna - 1];
}


void afisareRezervariDinFisier() {
    system(CLEAR);
    FILE* file = fopen(FILENAME, "r");
    if (file == NULL) {
        printf("Nu exista rezervari salvate in fisier!\n");
    }
    else {
        printf(PURPLE "===== LISTA REZERVARI DIN FISIER =====\n\n" RESET );
        int nr_sala, zi, luna, an, ora;
        char nume[50];
        int gasit = 0;
        while (fscanf(file, "%d %d %d %d %d %49[^\n]\n", &nr_sala, &zi, &luna, &an, &ora, nume) == 6) {
            printf("Sala %d rezervata pe %02d/%02d/%04d la ora %d:00 de %s\n",
                nr_sala, zi, luna, an, ora, nume);
            gasit = 1;
        }
        if (!gasit) {
            printf(RED "Nu exista rezervari inregistrate!\n" RED);
        }
        fclose(file);
    }
    printf(WHITE "\nApasa ENTER pentru a reveni la meniu..." WHITE);
    getchar(); getchar();
}


void incarcaRezervari() {
    FILE* file = fopen(FILENAME, "r");
    if (file == NULL) return;
    for (int i = 0; i < NR_SALI; i++)
        for (int j = 0; j < CLOSED - OPEN; j++)
            sali[i].rezervari[j] = 0;
    nr_rezervari = 0;
    char line[100];
    while (fgets(line, sizeof(line), file)) {
        int nr_sala, zi, luna, an, ora;
        char nume[50];
        if (sscanf(line, "%d %d %d %d %d %49[^\n]", &nr_sala, &zi, &luna, &an, &ora, nume) == 6) {
            if (nr_sala < 1 || nr_sala > NR_SALI) continue;
            if (ora < OPEN || ora >= CLOSED) continue;
            sali[nr_sala - 1].rezervari[ora - OPEN] = 1;
            rezervari[nr_rezervari].nr_sala = nr_sala;
            rezervari[nr_rezervari].zi = zi;
            rezervari[nr_rezervari].luna = luna;
            rezervari[nr_rezervari].an = an;
            rezervari[nr_rezervari].ora = ora;
            strcpy(rezervari[nr_rezervari].nume_rezervant, nume);
            nr_rezervari++;
        }
    }
    fclose(file);
}

void salveazaRezervari() {
    FILE* file = fopen(FILENAME, "w");
    if (file == NULL) return;
    for (int i = 0; i < nr_rezervari; i++)
        fprintf(file, "%d %d %d %d %d %s\n",
            rezervari[i].nr_sala,
            rezervari[i].zi,
            rezervari[i].luna,
            rezervari[i].an,
            rezervari[i].ora,
            rezervari[i].nume_rezervant);
    fclose(file);
}

void afisareSali() {
    printf(WHITE_BLUE "\n===== LISTA SALILOR =====\n" WHITE_BLUE);
    for (int i = 0; i < NR_SALI; i++)
        printf("\033[97mNR SALA: %d | NUME: %s\n\033[97m", sali[i].nr_sala, sali[i].nume);
}

void afisareOreDisponibile(int sala_id) {
    system(CLEAR);
    printf(GREEN "\n===== ORE DISPONIBILE PENTRU SALA %d =====\n" GREEN, sala_id);
    int oreLibere = 0;
    for (int i = 0; i < CLOSED - OPEN; i++) {
        if (sali[sala_id - 1].rezervari[i] == 0) {
            oreLibere = 1;
            printf("%d:00 |", OPEN + i);
        }
    }
    if (!oreLibere) printf("Nicio ora libera!");
    printf(WHITE "\n\nApasa ENTER pentru a reveni la meniu..." WHITE);
    getchar(); getchar();
}

void rezervareSala() {
    system(CLEAR);
    int id, ora, zi, luna, an;
    char nume[50];

    printf(BLUE "\n===== REZERVARE SALĂ =====\n" RESET);
    afisareSali();

    printf(YELLOW "Introduceti numărul sălii pentru rezervare (1-%d): " RESET, NR_SALI);
    scanf("%d", &id);
    if (id < 1 || id > NR_SALI) {
        printf(RED "Număr sală invalid!\n" RESET);
        getchar(); getchar();
        return;
    }

    printf(YELLOW "Introduceti data rezervării (format zz ll aaaa): " RESET);
    scanf("%d %d %d", &zi, &luna, &an);
    if (!esteDataValida(zi, luna, an)) {
        printf(RED "Dată invalidă!\n" RESET);
        getchar(); getchar();
        return;
    }

    afisareOreDisponibile(id);

    printf(YELLOW "Introduceti ora dorită (între %d si %d): " RESET, OPEN, CLOSED - 1);
    scanf("%d", &ora);
    if (ora < OPEN || ora >= CLOSED || sali[id - 1].rezervari[ora - OPEN]) {
        printf(RED "Ora indisponibilă sau invalidă!\n" RESET);
        getchar(); getchar();
        return;
    }

    printf(YELLOW "Introduceti numele persoanei care face rezervarea: " RESET);
    scanf(" %49[^\n]", nume);

    sali[id - 1].rezervari[ora - OPEN] = 1;
    Rezervare r = { id, zi, luna, an, ora };
    strcpy(r.nume_rezervant, nume);
    rezervari[nr_rezervari++] = r;

    salveazaRezervari();

    printf(GREEN "\nSala %d a fost rezervată cu succes pentru %02d/%02d/%04d la ora %d:00 de %s\n" RESET,
        id, zi, luna, an, ora, nume);

    printf("\nApăsati ENTER pentru a reveni la meniu...");
    getchar(); getchar();
}



void stergeRezervare() {
    int id, zi, luna, an, ora;
    printf(WHITE_BLUE "INTRODUCETI NR SALII: " RESET); scanf("%d", &id);
    printf(WHITE_BLUE "INTRODUCETI DATA (zi luna an): " RESET); scanf("%d %d %d", &zi, &luna, &an);
    if (!esteDataValida(zi, luna, an)) {
        printf(RED "Data introdusa nu este valida!\n" RESET);
        getchar(); getchar(); return;
    }
    printf(WHITE_BLUE "INTRODUCETI ORA: " RESET); scanf("%d", &ora);
    int found = -1;
    for (int i = 0; i < nr_rezervari; i++) {
        if (rezervari[i].nr_sala == id && rezervari[i].zi == zi && rezervari[i].luna == luna &&
            rezervari[i].an == an && rezervari[i].ora == ora) {
            found = i;
            break;
        }
    }
    if (found != -1) {
        for (int i = found; i < nr_rezervari - 1; i++) {
            rezervari[i] = rezervari[i + 1];
        }
        nr_rezervari--;
        sali[id - 1].rezervari[ora - OPEN] = 0;
        salveazaRezervari();
        printf(GREEN "\nRezervarea a fost stearsa!\n" RESET);
    }
    else {
        printf(RED "\nNu s-a gasit rezervarea specificata!\n" RESET);
    }
    printf("\nApasa ENTER pentru a reveni la meniu...");
    getchar(); getchar();
}


void vizualizareOrarSali() {
    system(CLEAR);
    int zi, luna, an;
    printf(WHITE_BLUE "Introduceti data pentru orar (format zz ll aaaa): " RESET);
    scanf("%d %d %d", &zi, &luna, &an);
    if (!esteDataValida(zi, luna, an)) {
        printf(RED "Data introdusă nu este validă!\n" RESET);
        getchar(); getchar(); return;
    }

    printf(PURPLE "\nORAR PENTRU TOATE SĂLILE PE %02d/%02d/%04d\n\n" PURPLE, zi, luna, an);
    for (int i = 0; i < NR_SALI; i++) {
        printf("Sala %d - %s:\n", sali[i].nr_sala, sali[i].nume);
        for (int ora = OPEN; ora < CLOSED; ora++) {
            int ocupat = 0;
            for (int j = 0; j < nr_rezervari; j++) {
                if (rezervari[j].nr_sala == sali[i].nr_sala &&
                    rezervari[j].ora == ora &&
                    rezervari[j].zi == zi &&
                    rezervari[j].luna == luna &&
                    rezervari[j].an == an) {
                    ocupat = 1;
                    break;
                }
            }
            printf("%02d:00 | %s\n", ora, ocupat ? RED "Rezervat" RESET : GREEN "Liber" RESET);
        }
        printf("\n");
    }
    printf("\nApăsati ENTER pentru a reveni la meniu...");
    getchar(); getchar();
}
int main() {
    setlocale(LC_ALL, "");
    incarcaRezervari();
    getchar();
    int optiune;
    while (1) {
        system(CLEAR);
        printf(BLUE "===== MENIU PRINCIPAL =====\n" RESET);
        printf("1. Rezervare sala\n");
        printf("2. Ore disponibile pentru o sala\n");
        printf("3. Orar pentru toate salile\n");
        printf("4. Stergere rezervare\n");
        printf("5. Afisare rezervari din fisier\n");
        printf("6. Afisare sali\n");
        printf("7. Iesire\n");
        printf("\nALEGETI O OPTIUNE: ");
        scanf("%d", &optiune);
        switch (optiune) {
        case 1: rezervareSala(); break;
        case 2: {
            int sala_id;
            printf("Introduceti numarul salii (1-%d): ", NR_SALI);
            scanf("%d", &sala_id);
            if (sala_id < 1 || sala_id > NR_SALI) { getchar(); getchar(); break; }
            afisareOreDisponibile(sala_id);
            break;
        }
        case 3: vizualizareOrarSali(); break;
        case 4: stergeRezervare(); break;
        case 5: afisareRezervariDinFisier(); break;
        case 6: afisareSali(); getchar(); getchar(); break; 
        case 7: return 0;
        default: getchar(); getchar();
        }


    }
}




