/* 
 * ping2csv.c 
 * 
 * Tämä komentoriviohjelma lukee Windows- ja Linux-käyttöjärjestelmän 
 * "ping"-komennon tuottamia tekstitiedoston ja muuntaa sen tiedot 
 * halutulla merkillä erotelluksi tekstitiedostoksi (esim. CSV-muoto).
 * 
 * Desimaalimuotoiset ping-vasteajat muunnetaan kokonaisluvuiksi
 * kertomalla ne sadalla. Esim:
 *      "187 ms" ---> "18700"
 * 
 * Kääntäminen:
 *      gcc -o ping2csv -Wall -g ping2csv.c
 * 
 * Käyttö:
 *      ./ping2csv -[w|l] "<pingtiedosto>" "," > "<csvtiedosto>"
 * Valitsimella "-w" ja "-l" asetetaan ping-tiedoston muodoksi joko 
 * Windows tai Linux.
 * 
 * 
 * */

#include "ping2csv.h"


//struct Tietokone * g_tietokoneet = NULL;
int g_toimintatila = TILA_VIRHE;


int main (int argc, char * argv[]) {
    FILE * data_in = NULL;
    char data_puskuri[MAX_RIVIPITUUS + 1];
    int rivi_tyyppi = RIVITYYPPI_TYHJA;
    int tietokone_lkm = 0;
    char * data_in_rivi = NULL;
    struct Tietokone tietokone;
    char erotinmerkki = '\0';
    int tila = TILA_VIRHE;
    struct Mittaus mittaus;
    
    /* Jos käyttäjä joko ei antanut yhtään tai antoi liian monta
     * komentoriviargumenttia, poistutaan. */
    if (argc != 4) {
            ohje(argv[0]);
            exit(0);
    }
    
    /* Linuxin tai windowsin ping-tiedostomuodon valinta */
    if (argv[1][0] == '-') {
        switch (argv[1][1]) {
        case 'L':
        case 'l':
            tila = TILA_LINUX;
            break;
        
        case 'W':
        case 'w':
            tila = TILA_WINDOWS;
            break;
        
        default:
            puts("Virheellinen tiedostomuodon määrite (-w/-l)! ");
            ohje(argv[0]);
            exit(1);
            break;
        }
            
    } else {
        ohje(argv[0]);
        exit(1);
    }
    
    g_toimintatila = tila;
    
    erotinmerkki = argv[3][0];
    
    if (erotinmerkki == '\0') {
        erotinmerkki = ',';
    }
    
    /* avataan tiedosto */
    data_in = fopen(argv[2], "r");
    
    /* Jos ei löydy */
    if (data_in == NULL) {
        perror("Tiedostoa ei löytynyt.");
        exit(1);
    }
    
    /* Nollaus */
    data_puskuri[MAX_RIVIPITUUS + 1] = '\0';
    
    tulosta_otsikko(erotinmerkki);
    
    /* Luetaan rivit puskuriin ja tallennetaan */
    while (feof(data_in) == 0) {
        data_in_rivi = lue_rivi(data_in, data_puskuri);
        
        rivi_tyyppi = kasittele_rivi(data_in_rivi);
        
        switch (rivi_tyyppi) {
        case RIVITYYPPI_TIETOKONE:
            tietokone = uusi_tietokone(data_in_rivi, tietokone_lkm);
            ++tietokone_lkm;
            break;
                
        case RIVITYYPPI_MITTAUS:
            mittaus = uusi_mittaus(data_in_rivi, &tietokone);
            tulosta_mittaus(tietokone, mittaus, erotinmerkki);
            break;
                
        case RIVITYYPPI_TYHJA:
            break;
        
        default: 
            break;
        }
    }
    
    
    /* 
     * Loppusiivous 
     */
    fclose(data_in);
    data_puskuri[MAX_RIVIPITUUS + 1] = '\0';
    
    return 0;
}



void tulosta_otsikko(char erotinmerkki) {
    char kentta_host[] = "address";
    char ping_num[] = "ping_num";
    char ping_value[] = "ping_ms*100";
    
    printf("%s%c%s%c%s\n",kentta_host, erotinmerkki, ping_num, \
           erotinmerkki, ping_value);
}



void tulosta_mittaus(struct Tietokone tietokone, \
                     struct Mittaus mittaus, char erotinmerkki) {
    
    printf("%s%c%d%c%d\n",tietokone.osoite, erotinmerkki, \
           (tietokone.mittaukset_lkm + 1), erotinmerkki, mittaus.arvo);
}



/* Luo uuden Tietokone-tietueen.
 * Lukee tiedot tekstiriviltä, tallentaa tietueeseen ja palauttaa sen.
 */
struct Tietokone uusi_tietokone(char * rivi, int num) {
    struct Tietokone tietokone;
    char merkki = '\0';
    int num_merkki = 0;
    int i = 0;
        
    tietokone.mittaukset_lkm = 0;
    
    num_merkki = 0;
    merkki = rivi[num_merkki];
    
    /* Käydään läpi riviä kunnes tulee eka numeromerkki 
     * tai max. rivin pituus tulee vastaan. */
    while ((isdigit(merkki) == 0) && (num_merkki < (MAX_RIVIPITUUS - 1))) {
        ++num_merkki;
        merkki = rivi[num_merkki];
    }
    
    /* Haetaan IP-osoite */
    if (num_merkki < (MAX_RIVIPITUUS - 1)) {
        
        /* Ekasta numeromerkistä välilyöntiin saakka */
        i = 0;
        merkki = '\0';
        while ((num_merkki < MAX_RIVIPITUUS) && \
               (merkki != ' ') && \
               (i < (OSOITE_MAXPITUUS + 1))) 
        {
            merkki = rivi[num_merkki];
            
            if (isdigit(merkki) || (merkki == '.')) {
                tietokone.osoite[i] = rivi[num_merkki];
                ++i;
            }
            
            ++num_merkki;
        }
        
        /* Roskamerkit pois osoitteen lopusta */
        tietokone.osoite[i] = '\0';
    }
    
    return tietokone;
}



/* Varataan muistia uudelle Mittaus-tietueelle,
 * luetaan tiedot riviltä ja tallennetaan tietueeseen.
 * Palautetaan osoitin tietueeseen. */
struct Mittaus uusi_mittaus(char * rivi, \
                            struct Tietokone * p_tietokone) {
    struct Mittaus mittaus;
    char merkki = '\0';
    int i = 0;
    int merkki_nro = 0;
    char temp[10] = {'\0'};  /* väliaikaisvarasto */
    int erotin_laskuri = 0;
    int arvo_aika;
    int n;
    float arvo_temp = 0.0f;
        
    /* Luetaan riviä kunnes vastaan tulee n:s '='-merkki
     * (n = 2 (Windows), n = 3 (Linux))
     * tai rivin max-pituuteen saakka. */
    switch (g_toimintatila) {
    case TILA_WINDOWS:
        n = 2;
        break;
            
    case TILA_LINUX:
        n = 3;
        break;
    }
    
    merkki_nro = 0;
    while ((erotin_laskuri < n) && (merkki_nro < MAX_RIVIPITUUS)) {
        merkki = rivi[merkki_nro];
        
        if (merkki == '=') {
            ++erotin_laskuri;
        }
        
        ++merkki_nro;
    }
    
    /* merkki-nron pitäisi nyt olla time-arvon kohdalla 
     * Haetaan merkki. */
    merkki = rivi[merkki_nro];
    
    /* jos tämä merkki on luku, piste tai pilkku, luetaan 
     * sen jälkeen tulevat muut merkit väliaikaiseen 
     * merkkijonotaulukkoon */
    i = 0;
                 
    while (onko_numeerinen_merkki(merkki, merkki_nro)) {
        temp[i] = merkki;
        
        ++i;
        ++merkki_nro;
        
        merkki = rivi[merkki_nro];
    }
    
    /* ping-ajan käsittely */
    switch (g_toimintatila) {
    case TILA_WINDOWS:  /* ms-aika kokonaislukuna */
        arvo_aika = atoi(temp) * 100;
        break;
            
    case TILA_LINUX: /* ms-aika desimaalilukuna */
        arvo_temp = (float) atof(temp);
        arvo_aika = (int) (arvo_temp * 100.0f);
    }
    
    mittaus.arvo = arvo_aika;
    
    /* Lisätään tämän tietokoneen mittausten laskuria */
    ++(p_tietokone -> mittaukset_lkm);
    
    return mittaus;
}


bool onko_numeerinen_merkki(char merkki, int nro) {
    bool merkki_ok = false;
    
    merkki_ok = (  (nro < MAX_RIVIPITUUS) && \
                   (isdigit(merkki)       || \
                   (merkki == '.')        || \
                   (merkki == ','))          );
    return merkki_ok;
}


char * lue_rivi(FILE * tiedosto, char * puskuri) {
    char * rivi = NULL;
    
    rivi = fgets(puskuri, MAX_RIVIPITUUS, tiedosto);
    return rivi;
}



int kasittele_rivi(char * rivi) {
    int rivi_pituus;
    char * loytynyt_merkkijono = NULL;
    int rivi_tyyppi = RIVITYYPPI_TYHJA;
    
    if (rivi == NULL) {
        return RIVITYYPPI_TYHJA;
    }
    
    rivi_pituus = strlen(rivi);
    
    if (rivi_pituus < 5) {
        return RIVITYYPPI_TYHJA;
    }
        
    /* Rivityypin päättely */
    switch (g_toimintatila) {
    case TILA_WINDOWS:
        loytynyt_merkkijono = strstr(rivi, "Pinging");
        break;
    
    case TILA_LINUX:
        loytynyt_merkkijono = strstr(rivi, "PING");
        break;
    }
    
    /* Rivi: uuden ping-session alku: */
    if (loytynyt_merkkijono != NULL) {
        rivi_tyyppi = RIVITYYPPI_TIETOKONE;
    }
    
    /* Rivi: ping-ajan sisältävä rivi: */
    if (loytynyt_merkkijono == NULL) {
        switch (g_toimintatila) {
        case TILA_WINDOWS:
            loytynyt_merkkijono = strstr(rivi, "Reply from");
            break;
                
        case TILA_LINUX:
            loytynyt_merkkijono = strstr(rivi, "bytes from");
            break;
        }
        
        if (loytynyt_merkkijono != NULL) {
            rivi_tyyppi = RIVITYYPPI_MITTAUS;
        }
    }
    
    return rivi_tyyppi;
}



void ohje(char * nimi) {
        puts("Tämä ohjelma muuntaa Windowsin ping-komennon tuottaman");
        puts("datan halutulla erotinmerkillä erotelluksi tiedoksi.");
        puts("Muunnettu tieto kirjoitetaan näytölle.");
        puts("Käyttöohje:");
        puts("");
        printf("\t%s -wl \"pingdata.txt\" \",\" > \"output_file.csv\"", nimi);
        puts("\n");
        puts("Jossa \"pingdata.txt\" on ping-dataa sisältävä teksti-");
        puts("tiedosto, -w/-l asettaa joko Windows- tai Linux-muotoisen tiedoston,");
        puts("\",\" on haluttu erotinmerkki ja \"output_file.csv\"");
        puts("on tiedosto, johon ohjelman tuottamat tiedot kirjoitetaan.");
        
}
