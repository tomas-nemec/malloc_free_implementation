#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define ABS(N) ((N<0)?(-N):(N))

void *pamat;

void defrag(int *ptr)       //funkcia na spajanie volnych blokov
{

void *akt= ptr;                                //pointer na pohyb
void *beg = pamat + 2*sizeof(int);                  //zaciatok casti ktora je vyhradena pre alokovanie
void *end = pamat + 80-sizeof(int);      // koniec tejto casti -> aby som nevyletel mimo

int DlzkaCelehoBloku = *(int *)akt;     //velkost noveho pospajaneho bloku

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    if( (akt != beg) && ( (akt + *(int *)akt + sizeof(int)) != end) )       //aby som neskakal mimo pamate = akt blok neni ani na zaciatku ani na konci
    {
        void *prev = akt - sizeof(int) - *(int *)(akt - sizeof(int)) - sizeof(int);        //pointer na hlavicku predosleho bloku - smer do lava
        void *next = akt + *(int *)akt +2*sizeof(int);                                      //pointer na paticku nasledujuceho v smere do prava
        next += ABS(*(int *)next) + 4;

        void *LeftSide = akt;
        void *RightSide = akt + *(int *)akt + sizeof(int);
        while(*(int *)prev > 0 )        // tu skacem do lava po hlavickach a kontrolujem ci su volne
        {

            if(prev == beg)             //ak je to zaciatok, ukoncim skakanie aby osm nesiel mimo
            {
                DlzkaCelehoBloku += *(int *)prev+2*sizeof(int);         //zvacsujem velkost 2 hlavicky + size bloku
                LeftSide = prev;
                break;
            }
            DlzkaCelehoBloku += *(int *)prev + 2*sizeof(int);       //zvacsujem velkost : hlavicka + paticka + size bloku
            LeftSide = prev;                        //od tadialto
            prev = prev - sizeof(int) - *(int *)(prev - sizeof(int)) - sizeof(int);     //skocim na halvicku predosleho smerom v lavo

        }

        while(*(int *)next > 0)             //skakanie smerom doprava
        {
            if(next == end)             //ak je to blok na konci a je volny, pripocitam a ukoncim aby osm nesiel mimo pamate
            {
                DlzkaCelehoBloku += *(int *)next +2*sizeof(int);
                RightSide = next;
                break;
            }
            DlzkaCelehoBloku += *(int *)next + 2*sizeof(int);
            RightSide = next;
            next += 4;                              //skocim na hlavicku nasledujuceho bloku v pravo
            next = next + *(int *)next + 4;            //z hlavicky skocim na paticku
        }

        *(int *)LeftSide = DlzkaCelehoBloku;            //novy blok hlava a pata
        *(int *)RightSide = DlzkaCelehoBloku;

        for(int i = sizeof(int); i<DlzkaCelehoBloku;i++)   //oznac za volne
        {
            *((char *)LeftSide+i) = -1;
        }
        *(int *)(pamat+4) = 0;

    }
////////////////////////////////////////////////////////////////////////////////////////////////////////
    else if(akt != beg)     //akt blok je na konci, pojdem iba smerom dolava
    {
        void *prev = akt - sizeof(int) - *(int *)(akt - sizeof(int)) - sizeof(int);        //pointer na hlavicku predosleho bloku
        void *LeftSide = prev;
        void *RightSide = akt + *(int *)akt + sizeof(int);

        while(*(int *)prev > 0 )                //skacem smerom dolava po halvickach az kym nenatrafim an alokovanu
        {
            if(prev == beg)             //ak je to zaciatok, osetrenie ab ysme nesli mimo
            {
                DlzkaCelehoBloku += *(int *)prev+2*sizeof(int);
                LeftSide = prev;
                break;
            }
            DlzkaCelehoBloku += *(int *)prev + 2*sizeof(int);       //kedze spajam dva bloky takze 2 hlavicky + size
            LeftSide = prev;                        //od tadialto
            prev = prev - sizeof(int) - *(int *)(prev - sizeof(int)) - sizeof(int);     //skocim na dalsiu hlavku

        }


        *(int *)LeftSide = DlzkaCelehoBloku;            //nastavim velkost noveho bloku
        *(int *)RightSide = DlzkaCelehoBloku;
        for(int i = sizeof(int); i<DlzkaCelehoBloku;i++)   //oznac za volne
        {
            *((char *)LeftSide+i) = -1;
        }
        *(int *)(pamat+4) = 0;      //vymazem pointer

    }
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    else if( (akt + *(int *)akt + sizeof(int)) != end)  //akt blok je na zaciatku, pojdem iba smerom doprava
    {
        void *next = akt + *(int *)akt +2*sizeof(int);                                      //pointer na paticku nasledujuceho v pravo
        next += ABS(*(int *)next) + 4;                                                  //tu sa dostanem do paticky


        void *LeftSide = akt;
        void *RightSide = next;

        while(*(int *)next > 0)             //skacem smerom doprava po patickach blokov
        {
            if(next == end)             //ak je to koniec, osetrenie aby som nesiel mimo
            {
                DlzkaCelehoBloku += *(int *)next + 2*sizeof(int);            //size + 2 halvicky
                RightSide = next;
                break;
            }

            DlzkaCelehoBloku += *(int *)next + 2*sizeof(int);
            RightSide = next;                           //aktualizujem pravu stranu aby ukazovala na koniec aktualneho bloku
            next += 4;
            next = next + *(int *)next + 4;     //skacem dalej
        }

        *(int *)LeftSide = DlzkaCelehoBloku;            //nastavim novu halviku a paticku
        *(int *)RightSide = DlzkaCelehoBloku;

        for(int i = sizeof(int); i<DlzkaCelehoBloku;i++)   //oznac za volne
        {
            *((char *)LeftSide+i) = -1;
        }
        *(int *)(pamat+4) = 0;


    }


}

int memory_free(void *valid_ptr)
{
    void *akt = valid_ptr - sizeof(int);        //vratim sa na hlavicku bloku
    *(int *)akt = ABS(*(int *)akt);         //zmenim hodnotu na kladnu
    *(int *)(akt + *(int *)akt +sizeof(int)) = *(int *)akt; //nastavim paticka == hlavicka

    for(int i = sizeof(int); i<*(int *)akt + sizeof(int);i++)   //oznac za volne
    {
        *((char *)akt+i) = -1;
    }
    *(int *)(pamat+4) = akt; //nastavim pointer aby ukazoval na tento napsoledy uvolneny blok, vyuzivam pri defrag
    return 0;
}

int memory_check(void *ptr) //kontrola platneho ukazovatela
{
    if (*(int *)(ptr-sizeof(int)) < 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

void *memory_alloc(unsigned int size)   //void lebo mozu byt odlisne data (float, int,...)
{
    if(size==0)          // kontrola parametra
    {
        return NULL;
    }
    if(*(int *)(pamat+4) != 0)  //ak pointer pre "LAST_FREED" != 0, poslem naposledy uvolneny blok na defragmentaciu
    {
       defrag(*(int *)(pamat+4)) ;
    }

    void *akt = pamat + 2*sizeof(int);          //tymto sa budem pohybovat
    void *end = pamat + *(int *)pamat-sizeof(int);      // koniec pamate
    void *p = NULL;    //pointer pre returnovanie miesta uzivatelovi


    while(1)
    {
        if(*(int *)akt == size ||  ((*(int *)akt > size) && (*(int *)akt-size < 10) )   )      //pripad ze sa neoplati rozdelovat pamat ale blok je vhodny
        {
            *(int *)akt *= (-1);        //oznacim za allocated, zmenim hodnotu na zapornu
            *(int *)(akt + ABS(*(int *)akt) +sizeof(int)) = *(int *)akt;    //tak isto aj paticku

            p = akt + sizeof(int);                                     //nastavim pointer za hlavicku volneho bloku ktory budem returnovat uzivatelovi

            for(int i = sizeof(int); i<ABS(*(int *)akt) + sizeof(int);i++)
            {
                *((char *)akt+i) = 1;
            }
            break;
        }

        else if( (*(int *)akt > 0) && (*(int *)akt > size + (2*sizeof(int) + 2)))        //pripad ked rozdelujem bloky
        {
            int newSize;

            newSize = ABS(*(int *)akt) - size - 2*sizeof(int);       //velkost volneho bloku, kt vznikne za alokovanym
            *(int *)akt = size * (-1);                                      //halvicka
            *(int *)(akt + size+sizeof(int)) = *(int *)akt;              //paticka
            p = akt + sizeof(int);                                      //nastavim pointer na return
            for(int i = sizeof(int); i<ABS(*(int *)akt) + sizeof(int);i++)
            {

                *((char *)akt+i) = 1;
            }

            *(int *)(akt + size+ 2*sizeof(int)) = newSize;     //nova hlavicka
            *(int *)(akt + size+ 2*sizeof(int) + newSize + sizeof(int)) = newSize; //nova paticka
            for(int i = sizeof(int); i<ABS(newSize) + sizeof(int);i++)
            {

                *(char *)(akt + size+ 2*sizeof(int)+ i) = -1;
            }
            break;
        }

        akt += ABS(*(int *)akt) + sizeof(int); //toto je pre skakanie po halvickach , skocim na paticku
        if(akt == end){     // skontrolujem ci sa nenachadzam na konci pamate
            break;
        }

        akt += 4;       //ak sa nenachadzam na konci tak skocim na nasledujucu hlavicku a ideme zase zhora
    }


    return p;

}

void memory_init(void *ptr , unsigned int size) //pride to pole na zaciatku cize cela pamat
{
    for(int i = 0; i<size;i++)
    {
        *((char *)ptr+i) = -1;
    }

    pamat = ptr;    //nastavime globalnu premennu na zaciatok pola, uz sa nebude menit
    *(int *)(ptr) = size;   //zaciatok pola si bude drzat velkost celkovu
    *(int *)(ptr+4) = 0;    //pointer ktory bude mat bud 0 alebo bude ukazovat na naposledy uvolneny blok
    *(int *)(ptr+8) = size - (4*sizeof(int));       //HLAVICKA dostupna pamat na alokovanie
    *(int *)(ptr+(size - sizeof(int))) = size - (4*sizeof(int));    //PATICKA dostupnej pamate

}


void test(char *region, char **pointer , int MinSize, int MaxSize, int minMemory, int maxMemory, int defrag)
{
    int totalBytes = 0;      //kolko som sa pokusil alokovat
    int totalBlocks = 0;
    int allocatedBytes = 0;     // kolko sa naozaj alokovalo
    int allocatedBlocks = 0;
    int randomSize = 0;     //nahodna velkost bloku
    int randomMem = 0;    // nahodna velkost regionu
    int defragSuccess = 0;
    memset(region,0,100000);
    randomMem = (rand() % (maxMemory-minMemory+1)) + minMemory;
    memory_init(region,randomMem);      //vytvorim si pamat o nahodnej velkosti

    int j=0;
    int POSSIBLE = randomMem - 16;

    while(1)
    {
        if(POSSIBLE < MinSize)
        {
            break;
        }
        else
        {
            randomSize = (rand() % (MaxSize-MinSize+1)) + MinSize;
            pointer[j] = memory_alloc(randomSize);
            POSSIBLE -= randomSize;
            totalBytes += randomSize;
            totalBlocks++;
            if(pointer[j])
            {
                allocatedBlocks++;
                allocatedBytes += randomSize;
                j++;
            }

        }

    }
    //  uvolnenie pre pred testom defragmentacie
    for (int i = 0; i < j; i++) {
        if (memory_check(pointer[i]))
        {
            memory_free(pointer[i]);
        }
        else
        {
            printf("\nChyba pri memory_check");
        }
    }

    if(defrag)
    {
        char* defragmentacia = memory_alloc(randomMem-16);
        if(defragmentacia)
        {
            defragSuccess = 1;
        }
    }

    //nastav hodnoty v alokovanom
    float result = ((float)allocatedBlocks / totalBlocks) * 100;
    float result_bytes = ((float)allocatedBytes / totalBytes) * 100;

    printf("\nSize of memory: %d bytes", randomMem);
    printf("\nAllocated blocks:  %.2f%%", result);
    printf("\nAllocated bytes: %.2f%%",result_bytes);
    if(defragSuccess)
    {
        printf("\nDefrag was successful !!!\n");
    }
    printf("\n////////////////////////////////////////////////////////////////\n");
}

int main() {

//    char region[100000];
//    char* pointer[1000];
//
//    test(region, pointer, 8, 24, 50, 100, 1);
//    test(region, pointer, 8, 1000, 10000, 20000, 1);
//    test(region, pointer, 8, 35000, 50000, 99000, 1);


    char region[80];
    memory_init(region,80);
    char* pointer = memory_alloc(3);
    char* pointer1 = memory_alloc(4);
    char* pointer2 = memory_alloc(5);
    if(pointer2)
    {
        memset(pointer2,9,5);
    }
    char* pointer3 = memory_alloc(2);
    char* pointer4 = memory_alloc(10);
    if(pointer2) memory_free(pointer2);
    if(pointer) memory_free(pointer);
    if(pointer1) memory_free(pointer1);
    if(pointer3) memory_free(pointer3);
    if(pointer4) memory_free(pointer4);
    char* pointer7 = memory_alloc(64);



    return 0;
}


