#ifndef PING2CSV_H
#define PING2CSV_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>

/* Maksimipituudet tiedoston riveille. */
#define MAX_RIVIPITUUS          100
#define OSOITE_MAXPITUUS        15

/* Mitä tietoa tiedoston rivi sisältää. */
#define RIVITYYPPI_TIETOKONE    1
#define RIVITYYPPI_MITTAUS      2
#define RIVITYYPPI_TYHJA        3

/* Mistä käyttöjärjestelmästä tiedosto on tuotettu. */
#define TILA_LINUX              1
#define TILA_WINDOWS            2
#define TILA_VIRHE              0


/* Tietuetyyppi mittausdatapisteelle */
struct Mittaus {
        int tunniste;
        int arvo;
        //struct Mittaus * p_mittaus;
};

/* Tietuetyyppi tietokoneelle */
struct Tietokone {
        int tunniste;
        char osoite[OSOITE_MAXPITUUS + 1];
        //struct Mittaus * mittaukset;
        //struct Tietokone * p_tietokone;
        int mittaukset_lkm;
};


/* Funktioiden esittelyt */
void tulosta_otsikko(char erotinmerkki);
void tulosta_mittaus(struct Tietokone tietokone, \
                     struct Mittaus mittaus, char erotinmerkki);
struct Tietokone uusi_tietokone(char * rivi, int num);
struct Mittaus uusi_mittaus(char * rivi, \
                            struct Tietokone * p_tietokone);
bool onko_numeerinen_merkki(char merkki, int nro);
char * lue_rivi(FILE * tiedosto, char * puskuri);
int kasittele_rivi(char * rivi);
void ohje(char * nimi);







#endif







