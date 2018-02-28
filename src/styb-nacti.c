/* STYB - Styblo Marek (zadani 1) SNORT */
/*
* styb-jadro.c verze 0.1
*
* Fce provadejici operace nad retezci, vlstni jadro programu
*
* Marek Styblo 02.12.2003
*
*/ 

/* ********* systemove hlavickove soubory */

#include <string.h> 
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

/* ********* vlastni hlavickove */

#include "styb-main.h"
#include "styb-io.h"
#include "styb-dyna.h"
#include "styb-vyst.h"


/* ********* deklarace IMPORTovanych objektu */

/* ********* definice GLOBALnich promennych */

/* ********* lokalni definice symbolickych konstant: #define */

/* ********* lokalni definice novych typu: typedef */

/* ********* definice LOCALnich promennych */

/* ********* uplne funkcni prototypy LOCAL funkci */

int kontrola_tisk_znak(char s_retezec[]);
int kontrola_enter(char s_retezec[]);
int kontrola_255(char s_retezec[]);
int kontrola_cele_cis(char s_retezec[]);

int kontrola_prvni_kod_cislo(char s_kod[], int pozice, char c_stop);
int kontrola_prvni_kod(char s_kod[]);
int kontrola_prvni_utok(char s_retezec[]);
int ret_in_ret(char s_ret[],char s_podret[]);
int kontrola_prvni_varianta(char s_ret[], int pozice,char s_var[], char c_stop);
int kontrola_prvni_konec(char s_ret[],int pozice, char s_ref[], char s_var[]);
int kontrola_prvni_radka(char buffer[], UTOK *p_utok);

int kontrola_druha_konst_retezec(char s_retezec[],int pozice, char s_ref[]);
int kontrola_druha_nacti_ret(char s_retezec[], int pozice, char c_stop, char s_vyst[]);
int kontrola_druha_radka(char buffer[]);

int kontrola_cislo_meze(char s_cislo[], int dolni, long int horni);
int kontrola_treti_nacti_podret(char s_ret[], int pozice, char c_stop, int dolni, int horni, char s_vyst[]);
int kontrola_treti_cas_dat(char s_ret[], int *p_datum, long int *p_cas);

int kontrola_treti_ip_adresa(char s_ip_adresa[], char **p_ip_vyst);
int kontrola_treti_dalsi_znaky_za_ip(char buffer[], char s_hledany[]);
int kontrola_treti_radka(char buffer[], UTOK *p_utok);

int kontrola_prvni_treti(char buffer[MAX_RADKA], FILE *fr, UTOK *p_utok, long *p_radka);

/* ********* uplne funkcni prototypy GLOBAL funkci */

int nacti_vypis_typy(char f_nazev[]);
int nacti_vypis_podsite(char f_nazev[], char s_hled_ut[]);
int nacti_vypis_datumy(char f_nazev[], char s_hled_var[]);

int vytvor_strukturu(char f_nazev[]);


/* ********* main() */

/* ********* definice LOCALnich funkci */

/* ------------------------kontrola_tisk_znak-----------------------------------
 * Otestuje retezec na netisknutelne znaky
 * Vraci: 1 .. zadny netisknutelny znak v retezci
 *        0 .. vyskyt netisknutelneho znaku
*/
int kontrola_tisk_znak(char s_retezec[])
{
  int i=0;

  while ( s_retezec[i] != '\0' ) {
    if ( isprint(s_retezec[i]) == 0 )
      return(0);  /* nalezen netisknutelny znak */
    i++;
  }
  return(1); /* vsechny znaky tisknutelne */
}
/* --END kontrola_tisk_znak---------------------------------------------------*/


/* --------------------------kontrola_enter-------------------------------------
 * Otestuje existenci znaku '\n' na predposledni pozici retezce, tj. pred '\0'
 * Pokud se vyskytuje pred znakem '\0' znak '\n', tak '\n' nahradi '\0'
 * Pozn. retezce autom volane odkazem --> zmena se projevi ven
 * pokud pred '\0' vyskyt EOF --> EOF nesmaze
 * Vraci: 1 .. preposledni znak '\n'
 *        0 .. jinak
 */
int kontrola_enter(char s_retezec[])
{
  if ( ( s_retezec[strlen(s_retezec)-1] ) == '\n' ) {
    s_retezec[strlen(s_retezec)-1] = '\0';
    return(1);
  }

  return(0);
}
/* --END kontrola_enter-------------------------------------------------------/*

  /* --------------------------kontrola_255------------------------------------
 * Otestuje, zda ma nactena radka mene rovno 255 znaku
 * Vraci: 1 .. OK
 *        0 .. radka > 255 znaku
 */
int kontrola_255(char s_retezec[])
{
  if ( ( strlen(s_retezec) ) > 255 ) {
    return(0);
  }

  return(1);
}
/* --END kontrola_255---------------------------------------------------------/*



/* -----------------------------kontrola_cele_cis-------------------------------
 * Otestuje retezec, jestli se sklada pouze z cisel (isdigit)
 * Vraci: 1 .. vsechno cisla
 *        0 .. nekde jiny znak
 */
int kontrola_cele_cis(char s_retezec[])
{
  int i=0;   /* pozice v s_retezec */

  while ( s_retezec[i] != '\0' ) {
    if ( isdigit(s_retezec[i]) == 0 )
      return(0);  /* znak neni cislem */
    i++;
  }

  if ( i==0 )
    return(0);  /* retezec prazdny, prvni znak '\0' */
  else
    return(1);
}
/* --END kontrola_cele_cis----------------------------------------------------*/

/* -------------------------kontrola_prvni_kod_cislo----------------------------
 * Otestuje <kod> na prvni radce, jestli odpovida vzoru [int:int:int]
 * Parametry: s_kod .. cteny retezec
 *            pozice .. pozice v retezci s_kod
 *            stop .. znak ukoncujici cteni retezce s_kod
 * Vraci: 1 .. shoda
 *        0 .. nekde chyba
 */
int kontrola_prvni_kod_cislo(char s_kod[], int pozice, char c_stop)
{
  unsigned int i=pozice;        /* inicializace zacatku cteni v retezci s_kod */
  char s_aktual[255];  /* podretezec z retezce s_kod */
  int j=0;             /* index do cteneho podretezce */

  while ( ( s_kod[i] != c_stop) && ( i<strlen(s_kod) ) )  {
    s_aktual[j]=s_kod[i];
    i++;
    j++;
  } /* while */

  if ( s_kod[i] != c_stop )
    return(-1);  /* za cislem v kodu neni ukoncujici znak */

  s_aktual[j]='\0';

  if ( kontrola_cele_cis(s_aktual) == 0 )
    return(-1);  /* v kodu neco jineho nez int */

  if ( i<strlen(s_kod) )
    i++;  /* pokud nejsem na kocni s_kod posunu se na dalsi cislo za ':' */

  return(i);
}
/* --END kontrola_prvni_kod-----------------------------------------------------*/


/* --------------------------kontrola_prvni_kod---------------------------------
 * Otestuje <kod> na prvni radce, jestli odpovida vzoru [int:int:int]
 * Vraci: 1 .. shoda
 *        0 .. nekde chyba
 */
int kontrola_prvni_kod(char s_kod[])
{
  unsigned int pozice=0;  /* aktualni pozice v analyzovanem retezci s_kod */

  if ( s_kod[pozice] != '[' ) {
    #ifdef LADENI
      fprintf(stderr,"snort: Chyba prvni radka: kod ");
      getchar();
    #endif
    return(0);  /* prvni znak neni predepsana '[' */
  }

  if ( (pozice+1)<strlen(s_kod) )
    pozice++; /* posuv na prvni zajimave cislo */

  /* test cisla za zavorkou '[' */
  if ( (pozice=kontrola_prvni_kod_cislo(s_kod, pozice, ':')) == -1 ) {
    #ifdef LADENI
      fprintf(stderr,"snort: Chyba prvni radka: kod - 1. cislo ");
      getchar();
    #endif
    return(0);
  }

 /* test prostredniho cisla */
 if ( (pozice=kontrola_prvni_kod_cislo(s_kod, pozice, ':')) == -1 ) {
    #ifdef LADENI
      fprintf(stderr,"snort: Chyba prvni radka: kod - 2. cislo ");
      getchar();
    #endif
    return(0);
 }

 /* test koncoveho cisla */
 if ( (pozice=kontrola_prvni_kod_cislo(s_kod, pozice, ']')) == -1 ) {
    #ifdef LADENI
      fprintf(stderr,"snort: Chyba prvni radka: kod - 3. cislo ");
      getchar();
    #endif
    return(0);
 }

 return(1);
}
/* --END kontrola_prvni_kod-----------------------------------------------------*/

/* ------------------------kontrola_prvni_utok----------------------------------
 * Otestuje pritomnost JEN velkych pismen, popr pomlcky v s_retezec[]
 * Vraci: 1 .. v poradku
 *        0 .. nekde chyba
 */
int kontrola_prvni_utok(char s_retezec[])
{
  int i=0;  /* pozice v retezci */

  while ( s_retezec[i] != '\0' ) {
    if ( isupper(s_retezec[i]) == 0 ) { /* nejaky znak neni velke pismeno */
      if ( s_retezec[i] != '-' ) { /* posledni moznost, ze je pomlcka */
        #ifdef LADENI
          fprintf(stderr,"snort: Chyba prvni radka: utok ");
          getchar();
        #endif
      return(0);
      }
    }
  i++;
  }

  return(1);
}
/* --END kontrola_prvni_utok----------------------------------------------------
                                     
/* --------------------------ret_in_ret-----------------------------------------
 * Zjisti relativni pozici zacatku podretezce s_podret[] v retezci s_ret[]
 * Vraci: relativni (pocitano od indexu 0!!) pozici s_pod
 *        -1 .. podretezec s_utok se nevyskytuje v retezci
 */
int ret_in_ret(char s_ret[],char s_podret[])
{
  char *p_pod;     /* ukazatel na zacatek s_podret v s_retezci */
  int pozice_pod;  /* pozice s_podret v s_ret relativne od pocatku s_ret */

  if ( ( p_pod = strstr(s_ret,s_podret) ) == NULL )
    return(-1);  /* s_podret se nevyskytuje v s_ret */

  pozice_pod = p_pod - s_ret;  /* pozice od pocatku s_ret, na ktere je s_ret */

  return(pozice_pod);
}
/* --END ret_in_ret-------------------------------------------------------------

/* --------------------------kontrola_prvni_varianta----------------------------
 * Nacte podretezec s_var[] v retezci s_ret[] pocinaje pozici pozice,
 * ocekava ukonceni cteni podretezce znakem c_stop
 * Vraci: relativni pozici konce podretezce v retezci + stop retezec
 *        0 .. nekde chyba
 */
int kontrola_prvni_varianta(char s_ret[], int pozice, char s_var[], char c_stop)
{
  unsigned int i=pozice;  /* pocatecni pozice v s_ret */
  int j=0;       /* indexovani retezce pro s_var */

  if (s_ret[i] == c_stop ) /* chybi uplne varianta */ /* mezeru ulozi */
	  return(0);

  while ( ( s_ret[i] != c_stop ) && ( i<strlen(s_ret) ) ) {
    s_var[j]=s_ret[i];
    i++;j++;
  }

  if ( i<2 ) /* za utokem byla mezera, pak znak a za nim BEZ mezery '[' --> chyba */
    return(0);

  if ( j==2 ) /* varianta je jeden znak */
    s_var[j-1]='\0'; 

  /* zapisu '\0' namisto prvniho "prebytecneho" prvku za s_var, coz je ' ' */
  if ( ( i<strlen(s_ret) ) && (j>2) ) 
    s_var[j-1]='\0';

  return(pozice+j);
}
/* --END kontrola_prvni_varianta----------------------------------------------*/

/* -----------------------kontrola_prvni_konec----------------------------------
 * Nacte retezec s_var z retezce s_ret pozcinaje pozici do KONce retezce s_ret
 * Srovna nacteny s_var[] s referencnim s_ref
 * Vraci: 1 .. retezce jsou stejne, + odkazem nacteny retezec s_var[]
 *        0 .. jinak
 */
int kontrola_prvni_konec(char s_ret[],int pozice, char s_ref[], char s_var[])
{
  unsigned int i=pozice;         /* zacinam v s_ret na pozici */
  int j=0;              /* index pro ulozeni s_var[] */

  while ( i<strlen(s_ret) ) {  /* jsem uvnitr retezce s_ret */
    s_var[j]=s_ret[i];
    i++;j++;
  }

  s_var[j]='\0';
  if ( strcmp(s_var,"[**]") != 0 )  {
    #ifdef LADENI
      fprintf(stderr,"snort: Chyba prvni radka: posledni [**] ");
      getchar();
    #endif
    return(0);  /* s_ret, s_var nejsou stejne retezce */
  }

  return(1);
}
/* --END kontrola_prvni_konec-------------------------------------------------*/

/* --------------------------kontrola_prvni_radka-------------------------------
 * Provede tesy predepsanych formatu prvni radky nactene v bufferu
 * Vraci: 1 .. Vsechny testy uspesne
 *        0 .. jinak
 */
int kontrola_prvni_radka(char buffer[], UTOK *p_utok )
{
  char s_uvod[MAX_RADKA]; /* retezec pro nacteni [**] */
  char s_konec[MAX_RADKA]; /* retezec pro nacteni [**] */
  char s_kod[MAX_RADKA];  /* retezec pro nacteni vzoru [int:int:int] */
  char s_utok[MAX_RADKA];  /* retezec pro nacteni typu utoku */
  char s_varianta[MAX_RADKA];  /* retezec pro nacteni varianty utoku */
  int pozice_ut;               /* pozice s_utok[] v buffer[] */
  int pozice_za_ut;  /* pozice ZA s_utok[] v buffer[] + mezera */
  int pozice_za_var;  /* pozice ZA s_varianta[] v buffer[] + mezera */

  static int radka_soub=0;

  /* smazani znaku '\n' na konci radky, pokud existuje */
  kontrola_enter(buffer);
  
  
  if ( kontrola_255(buffer) == 0 ) {
	#ifdef LADENI
      fprintf(stderr,"snort: Chyba prvni radka: vetsi 255 znaku ");
      getchar();
    #endif
	  return(0); /* radka vetsi 255 znaku */
  }

  sscanf(buffer,"%s %s %s ", s_uvod, s_kod, s_utok);

  if ( kontrola_tisk_znak(buffer) == 0 ) {
	#ifdef LADENI
      fprintf(stderr,"snort: Chyba prvni radka: netisknutelny znak ");
      getchar();
    #endif
    return(0);  /* vyskyt netisknutelneho znaku */
  }	

  if ( strcmp(s_uvod,"[**]") ) {
    #ifdef LADENI
      fprintf(stderr,"snort: Chyba prvni radka: prvni [**] ");
      getchar();
    #endif
    return(0);  /* nespravny format [**] */
  }

  if ( kontrola_prvni_kod(s_kod) == 0 )
    return(0);  /* nespravny format [int:int:int] */

  if ( kontrola_prvni_utok(s_utok) == 0 )
    return(0);  /* <utok> neobsahuje POUZE VELKA pismena, nebo '-' */

  /* vyhledani pozice s_utok v retezci buffer */
  if ( ( pozice_ut=ret_in_ret(buffer,s_utok) ) == -1 ) /* s_utok se nevyskytuje v bufferu */
    return(0);

  /* !! prepokladam oddeleni s_utok a dalsi polozky alespon 1 mezerou !!*/
  /* ukazatel na pozici za ret utok +1 mezera */
  pozice_za_ut = pozice_ut+strlen(s_utok)+1;

  /* nacteni s_varianty */
  if ( (pozice_za_var=kontrola_prvni_varianta(buffer, pozice_za_ut, s_varianta, '[') ) == 0 )
	  return(0); /* chyba ve variante */

  if ( kontrola_prvni_konec(buffer,pozice_za_var,"[**]",s_konec) == 0 )
    return(0);  /* konec nema predepsany format "[**]" */

  /* pokud program "propadl" az sem, byla cela prvni radka v poradku */
   
   /* !!!! Naplneni polozek nazvu utoku a typu varianty !!!! */
  strcpy(p_utok->nazev_utok,s_utok);
  strcpy(p_utok->nazev_var,s_varianta);
  return(1);
}
/* --END kontrola_prvni_radka--------------------------------------------------*/

/* --------------------kontrola_druha_konst_retezec-----------------------------
* Zkontroluje pritomnost podretezce retezce s_ref[] zacinajiciho na pozici pozice
* v retezci s_retezec[]
* Vraci: pozici v s_retezec[] za s_ref[], kde skoncila s porovnavanim
*        -1 .. jinak
*/
int kontrola_druha_konst_retezec(char s_retezec[],int pozice, char s_ref[] )
{
  unsigned int i=pozice;
  int j=0;        /* index do podretezce */

    /* test, jestli prectena radka neni uplne "vadna" */
  if ( strlen(s_ref) > strlen(s_retezec) )
    return(-1); /* prectena radka je uplne "vadna" CELA mensi nez referencni */

  while ( i<(pozice+strlen(s_ref)) ) {
    if ( s_retezec[i] != s_ref[j] )
      return(-1);
    i++;
    j++;
  }

  return(i);
}
/* --END kontrola_druha_konst_retezec-----------------------------------------*/

/* -----------------------kontrola_druha_nacti_ret------------------------------
* Nacte retezec s_vyst[] na pozici pozice v s_retezec[] a cte dokud
* nenarazi na znak c_stop
* Vraci: aktualni pozici v s_retezec, + odkazem s_vyst[]
*        -1 .. pokud nemela zkratka spravne ukonceni znakem ']'
*/
int kontrola_druha_nacti_ret(char s_retezec[], int pozice, char c_stop, char s_vyst[])
{
  unsigned int i=pozice;
  int j=0;

  while ( ( s_retezec[i] != c_stop ) && ( i<strlen(s_retezec) ) ) {
    s_vyst[j]=s_retezec[i];
    i++;j++;
  }
  s_vyst[j]='\0';

  /*i=i+2;  preskoceni mezery v buffer + inicializace pozice na dalsi znak */
  if ( (i)>strlen(s_retezec) ) /* podivam se buffer[i], jestli tam neni '\0' */
    return(-1);  /* chybel v s_retezec c_stop znak */
  else
    return(i);  /* aktualni pozice v bufferu */
}
/* --END kontrola_druha_nacti_ret---------------------------------------------*/

/* -------------------------kontrola_druha_radka--------------------------------
 * Provede tesy predepsanych formatu druhe radky
 * Vraci: 1 .. Vsechny testy uspesne
 *        0 .. jinak
 */
int kontrola_druha_radka(char buffer[])
{
  unsigned int pozice=0;               /* aktualni pozice v bufferu */
  char s_zkratka[MAX_RADKA];  /* ulozeni zkratky: "sid" atp. */
  char s_priorita[MAX_RADKA]; /* ulozeni cisla priority: "2" atp. */

  kontrola_enter(buffer); /* odfiltruje znak enteru */

  if ( kontrola_255(buffer) == 0 ) {
	#ifdef LADENI
      fprintf(stderr,"snort: Chyba druha radka: vetsi 255 znaku ");
      getchar();
    #endif
	  return(0); /* radka vetsi 255 znaku */
  }

  if ( kontrola_tisk_znak(buffer) == 0 ) {
	#ifdef LADENI
      fprintf(stderr,"snort: Chyba druha radka: netisknutelny znak ");
      getchar();
    #endif
    return(0);  /* vyskyt netisknutelneho znaku */
  }	
  
  if ( (pozice=kontrola_druha_konst_retezec(buffer, pozice, "[Classification: ")) == -1 ) {
    #ifdef LADENI
      fprintf(stderr,"snort: Chyba druha radka: '[Classification: ' ");
      getchar();
    #endif
    return(0); /* neshoduje se uvodni konstanta "[Classification: " */
  }

  if ( (pozice=kontrola_druha_nacti_ret(buffer, pozice, ']', s_zkratka)) == -1 ) {
    #ifdef LADENI
      fprintf(stderr,"snort: Chyba druha radka: zkratka ");
      getchar();
    #endif
    return(0); /* chybi zarazka za zkratkou ']' */
  }

  if ( pozice<strlen(buffer) )
    pozice=pozice+2; /* nastaveni ukazovatka na zacatek [Priority.. */

  if ( (pozice=kontrola_druha_konst_retezec(buffer, pozice, "[Priority: ")) == -1 ) {
    #ifdef LADENI
      fprintf(stderr,"snort: Chyba druha radka: '[Priority ' ");
      getchar();
    #endif
    return(0); /* neshoduje se uvodni konstanta "[Priority: " */
  }

  if ( (pozice=kontrola_druha_nacti_ret(buffer, pozice, ']', s_priorita)) == -1 ) {
    #ifdef LADENI
      fprintf(stderr,"snort: Chyba druha radka: 'Priority id ' ");
      getchar();
    #endif
    return(0); /* chybi zarazka ']' za cislem Priority */
  }

  if ( kontrola_cele_cis(s_priorita) == 0 ) {
    #ifdef LADENI
      fprintf(stderr,"snort: Chyba druha radka: Priorita not cislo ");
      getchar();
    #endif
    return(0); /* priorita neni cele cislo */
  }

  if ( buffer[pozice] != ']' ) {
    #ifdef LADENI
      fprintf(stderr,"snort: Chyba druha radka: Priorita neukoncena ");
      getchar();
    #endif
    return(0); /* cislo priority neukonceno znakem ']' */
  }

  if ( (pozice+1) != strlen(buffer) ) {
    #ifdef LADENI
      fprintf(stderr,"snort: Chyba druha radka: Za Prioritou dalsi znaky ");
      getchar();
    #endif
    return(0);  /* za Prioritou jsou dalsi znaky */
  }

   /* pokud program "propadl" az sem, byla cela druha radka v poradku */
 
  return(1);
}
/* --END kontrola_druha_radka-------------------------------------------------*/

/* --------------------------kontrola_cislo_meze--------------------------------
 * Prevede retezec s_cislo[] na ciselnou reprezentaci a testuje,
 * zda patri cislo do intervalu <dolni,horni>
 * Vraci: 1 .. uspech
 *        0 .. jinak
 */
int kontrola_cislo_meze(char s_cislo[], int dolni, long int horni)
{
  int cas_dat; /* ulozeni stringu do intu */

  sscanf(s_cislo, "%d", &cas_dat);
  if ( (cas_dat>=dolni) && (cas_dat<=horni) )
    return(1);
  else
    return(0);
}
/* --END kontrola_cislo_meze--------------------------------------------------*/

/* ------------------------kontrola_nacti_podret--------------------------------
 * Provede testy formatu retezce tvaru: cislo'ukoncujici znak'
 * Vraci: pozici v ctenem retezci, +odkazem nacteny podretezec
 *        -1 .. chyba v datumu
 */
int kontrola_treti_nacti_podret(char s_ret[], int pozice, char c_stop, int dolni, int horni, char s_vyst[])
  {
    unsigned int i=pozice;
    int j=0; /* index pro ulozeni dne, mesice */

    while ( ( s_ret[i] != c_stop ) && ( i< strlen(s_ret) ) ) {
      s_vyst[j]=s_ret[i];
      i++;
      j++;
    }

      if ( (i<strlen(s_ret)) && ( s_ret[i] != c_stop ) )
        return(-1); /* v retezci neni urceny znak */


    s_vyst[j]='\0';
    if ( ( kontrola_cele_cis(s_vyst) == 0 ) || (kontrola_cislo_meze(s_vyst,dolni,horni) == 0) )
      return(-1); /* neni cislo, nebo nevyhovuje rozsahu <dolni,horni> */

    return(i);
}
/* --END kontrola_treti_dat---------------------------------------------------*/

/* ------------------------kontrola_nacti_cas_dat-------------------------------
 * Provede testy formatu datumu a casu: mes/den-hod:min:sec.tik ulozeneho v s_ret[]
 * Vraci: 1 .. formaty v poradku, +odkazem nactene *p_datum, *p_cas
 *        0 .. chyba v datumu, nebo case
 */
int kontrola_treti_cas_dat(char s_ret[], int *p_datum, long int *p_cas)
{
  char *p_cas_dat; /* ukazatel pro strcat-ovani dne k mesici, pro konverzi na cislo */
  char s_mes[MAX_RADKA];  /* ulozeni vypreparovaneho mesice */
  char s_den[MAX_RADKA];  /* ulozeni vypreparovaneho dne */
  char s_hod[MAX_RADKA];  /* ulozeni vypreparovane hodiny */
  char s_min[MAX_RADKA];  /* ulozeni vypreparovane minuty */
  char s_sec[MAX_RADKA];  /* ulozeni vypreparovane vteriny */
  char s_tik[MAX_RADKA];  /* ulozeni vypreparovaneho tiku */
  char jednicka[MAX_RADKA]="1";  /* TRIK, pridavam na zacatek cisla predstavujiciho den jednicku, abych mel
                              * dny a mesice i s nulami 1+0102 */

  int pozice=0;         /* aktualni pozice v bufferu */

  /* test mesice zakonceneho '/' */
  if ( (pozice=kontrola_treti_nacti_podret(s_ret, pozice, '/', 1, 12, s_mes)) == -1 ) {
    #ifdef LADENI
      fprintf(stderr,"snort: Chyba treti radka: datum: mesic ");
      getchar();
    #endif
    return(0);
  }
  pozice++; /* presun na den */


  /* otestovani dne cisla zakonceneho '-' */
  if ( (pozice=kontrola_treti_nacti_podret(s_ret, pozice, '-', 1, 31, s_den)) == -1 ) {
    #ifdef LADENI
      fprintf(stderr,"snort: Chyba treti radka: datum: den ");
      getchar();
    #endif
    return(0);
  }
  pozice++; /* presun na hodinu */

  /* test hodiny zakoncene ':' */
  if ( (pozice=kontrola_treti_nacti_podret(s_ret, pozice, ':', 0, 23, s_hod)) == -1 ) {
    #ifdef LADENI
      fprintf(stderr,"snort: Chyba treti radka: cas: hodina ");
      getchar();
    #endif
    return(0);
  }
  pozice++; /* presun na minutu */

  /* otestovani minuty zakoncene ':' */
  if ( (pozice=kontrola_treti_nacti_podret(s_ret, pozice, ':', 0, 59, s_min)) == -1 ) {
    #ifdef LADENI
      fprintf(stderr,"snort: Chyba treti radka: cas: minuta ");
      getchar();
    #endif
    return(0);
  }
  pozice++; /* presun na vterinu */

  /* otestovani sec zakoncene '.' */
  if ( (pozice=kontrola_treti_nacti_podret(s_ret, pozice, '.', 0, 59, s_sec)) == -1 ) {
    #ifdef LADENI
      fprintf(stderr,"snort: Chyba treti radka: cas: vterina ");
      getchar();
    #endif
    return(0);
  }
  pozice++; /* presun na tiky */

  /* otestovani tiku zakonceneho ' ' */
  if ( (pozice=kontrola_treti_nacti_podret(s_ret, pozice, ' ', 0, 999999, s_tik)) == -1 ) {
    #ifdef LADENI
      fprintf(stderr,"snort: Chyba treti radka: cas: tik ");
      getchar();
    #endif
    return(0);
  }

  /* pripojeni dne k mesici */
  p_cas_dat=strcat(s_mes, s_den);
  p_cas_dat=strcat(jednicka,p_cas_dat);  /* jednicka, viz definice prom */
  /* konverze charovho formatu datumu na ciselny */
  sscanf(p_cas_dat, "%d", p_datum);

  /* obnoveni JEDNICKY */
  jednicka[0]='1';
  jednicka[1]='\0';


  /* pripojeni sec,min,hod */
  p_cas_dat=strcat(s_min, s_sec);
  p_cas_dat=strcat(s_hod,p_cas_dat);
  p_cas_dat=strcat(jednicka,p_cas_dat);  /* jednicka, viz definice prom */

  /* konverze charoveho formatu casu na ciselny */
  sscanf(p_cas_dat, "%ld", p_cas);

  return(1);
}
/* --END kontrola_treti_cas_dat-----------------------------------------------*/


/* --------------kontrola_treti_ip_adresa--------------------
* Zkontroluje format CELE JEDNE IP adresy
* p_ip_vyst .. okazatel na retezec, do ktereho se ulozi ip adresa PODSite, tzn pouze 
* cislo.cislo.cislo
* Vrati: 1 .. format cisel v poradku
*        0 .. jinak
*/
int kontrola_treti_ip_adresa(char s_ip_adresa[], char **p_ip_vyst)
{
  int pozice=0; /* pozice v IP adrese, an zacatku pocatek retezce */

  /* s_ip(i) pouzivam jen kvuli te procedure, mne staci IP adresu zkontrolovat
   * ulozenou ji mam uz v s_ip_adresa */
  char s_ip_1[MAX_RADKA];  /* ulozeni prvniho cisla IP adresy */
  char s_ip_2[MAX_RADKA];  /* ulozeni druheho cisla IP adresy */
  char s_ip_3[MAX_RADKA];  /* ulozeni tretiho cisla IP adresy */
  char s_ip_4[MAX_RADKA];  /* ulozeni ctvrteho cisla IP adresy */
  char s_ip_5[MAX_RADKA];  /* ulozeni portu IP adresy */

 
  if ( (pozice=kontrola_treti_nacti_podret(s_ip_adresa,pozice,'.',0,255,s_ip_1)) == -1 ) {
    #ifdef LADENI
      fprintf(stderr,"snort: Chyba treti radka: IP: prvni cis ");
      getchar();
    #endif
    return(0);  /* prvni cislo IP adresy spatne */
  }
  pozice++; /* prechod na dalsi cislo IP-adresy */

  if ( (pozice=kontrola_treti_nacti_podret(s_ip_adresa,pozice,'.',0,255,s_ip_2)) == -1 ) {
    #ifdef LADENI
      fprintf(stderr,"snort: Chyba treti radka: IP: druhe cis ");
      getchar();
    #endif
    return(0);  /* druhe cislo IP adresy spatne */
  }
  pozice++; /* prechod na dalsi cislo IP-adresy */

  if ( (pozice=kontrola_treti_nacti_podret(s_ip_adresa,pozice,'.',0,255,s_ip_3)) == -1 ) {
    #ifdef LADENI
      fprintf(stderr,"snort: Chyba treti radka: IP: treti cis ");
      getchar();
    #endif
    return(0);  /* treti cislo IP adresy spatne */
  }
  pozice++; /* prechod na dalsi cislo IP-adresy */

  if ( (pozice=kontrola_treti_nacti_podret(s_ip_adresa,pozice,':',0,255,s_ip_4)) == -1 ) {
    #ifdef LADENI
      fprintf(stderr,"snort: Chyba treti radka: IP: ctvrte cis ");
      getchar();
    #endif
    return(0);  /* ctvrte cislo IP adresy spatne */
  }

  if ( s_ip_adresa[pozice-1] == ':' ) {
    pozice++; /* prechod na port IP-adresy */
    if ( (pozice=kontrola_treti_nacti_podret(s_ip_adresa,pozice,' ',1,999999,s_ip_5)) == -1 ) {
    #ifdef LADENI
      fprintf(stderr,"snort: Chyba treti radka: IP: port ");
      getchar();
    #endif
    return(0);  /* port IP adresy spatne */
    }
  }
  /* alokace heapu pro ulozeni cisla ip podsite jako retezce */
  /* 2, kvuli oddeleni cisel v ip-adrese teckami */
  if ( ((*p_ip_vyst)=(char *) malloc( (2+strlen(s_ip_1)+strlen(s_ip_2)+strlen(s_ip_3) )*sizeof(char)  ) ) == NULL )
		chyba_alokace_heap();

  strcpy((*p_ip_vyst), s_ip_1);
  strcat((*p_ip_vyst),".");
  strcat((*p_ip_vyst), s_ip_2);
  strcat((*p_ip_vyst), ".");
  strcat((*p_ip_vyst), s_ip_3);

			

  return(1);
}
/* --END kontrola_treti_ip-------------------------------*/

/* --------------kontrola_treti_dalsi_znaky_za_ip------------
 * Zjisti pozici posledni ip_adresy v bufferu, pote pricte k teto
 *  pozici velikost retezce posledni ip_adresy
 *
 * Vraci: 0 .. pokud se vysledna pozice nerovna velikosti bufferu
 *        1 .. za posledni ip adresou nejsou jiz dalsi znaky
 */ 
int kontrola_treti_dalsi_znaky_za_ip(char buffer[], char s_hledany[])
{
	char *p_ip;
	unsigned int pozice;   /*  relativni pozice zacatku druhe ip_adresy v bufferu */
	
	p_ip=strstr(buffer, s_hledany);
    pozice=p_ip-buffer;          
	pozice=pozice+strlen(s_hledany);

  if ( (pozice) != strlen(buffer) )
	  return(0);
  else
	  return(1);
}
/* --END kontrola_treti_dalsi_znaky_za_ip----------------------*/

/* --------------kontrola_treti_radka------------------
 * Provede tesy predepsanych formatu treti radky
 * Vraci: 1 .. Vsechny testy uspesne
 *        0 .. jinak
 */
int kontrola_treti_radka(char buffer[], UTOK *p_utok)
{
  char s_cas_dat[MAX_RADKA];
  char s_ip_zdroj[MAX_RADKA];
  char s_sipka[MAX_RADKA];
  char s_ip_cil[MAX_RADKA];
  char *p_ip_vyst;          /* pomocny retezec nesouci IP adresu PODSITE */
  int datum;   /* ulozeni zkonvertovaneho (s jednickou) datumu */
  long int cas;  /* ulozeni zkonvertovaneho (s jednickou) casu */


  kontrola_enter(buffer); /* odfiltruje znak enteru */

  if ( kontrola_255(buffer) == 0 ) {
	#ifdef LADENI
      fprintf(stderr,"snort: Chyba treti radka: vetsi 255 znaku ");
      getchar();
    #endif
	  return(0); /* radka vetsi 255 znaku */
  }

  if ( kontrola_tisk_znak(buffer) == 0 ) {
	#ifdef LADENI
      fprintf(stderr,"snort: Chyba treti radka: netisknutelny znak ");
      getchar();
    #endif
    return(0);  /* vyskyt netisknutelneho znaku */
  }	

  /* jednotlive polozky oddeleny mezerou --> ctu 4x retezec sscanf */
  sscanf(buffer,"%s %s %s %s ", s_cas_dat, s_ip_zdroj, s_sipka, s_ip_cil);

  if ( kontrola_treti_cas_dat(buffer, &datum, &cas) == 0 )
    return(0);

  if ( kontrola_treti_ip_adresa(s_ip_zdroj, &p_ip_vyst) == 0 )
    return(0);

   /* !!!! Naplneni polozek retezce ip adresy PODsite !!!! */
  strcpy(p_utok->ip,p_ip_vyst);

  /* zdrojovou IP adresu nenasleduje sipka */
  if ( strcmp("->",s_sipka) != 0 )
    return(0);

  if ( kontrola_treti_ip_adresa(s_ip_cil, &p_ip_vyst) == 0 )
    return(0);

  if ( kontrola_treti_dalsi_znaky_za_ip(buffer,s_ip_cil) == 0) {
    #ifdef LADENI
      fprintf(stderr,"snort: Chyba treti radka: Za koncovou ip dalsi znaky ");
      getchar();
    #endif
    return(0);  /* za s_ip_cil jsou dalsi znaky */
  }

  /* !!!! Naplneni polozek cisla datumu !!!! */
  p_utok->datum=datum;
  return(1);
  

}
/* --END kontrola_treti_radka------------------------*/

/* --------------vytvor_strukturu--------------------
 * Provede testy predepsanych formatu prvni..treti radky jednotlivych
 * bloku zaznamu o utocich a pokud blok ok, zaradi zajimave hodnoty do dynamickych seznamu
 * Vraci: 1 .. Uspesne dobehl na konec souboru
 *        0 .. jinak
 */
int vytvor_strukturu(char f_nazev[])
{
  long *p_radka;   /* pocitadlo radky ve vstupnim souboru */
  UTOK *p_utok;
  FILE *fr;
  FILE *fw;
  char radka[MAX_RADKA];

   
  
fw=fopen("vyst_ch.txt","w");
  p_utok=(UTOK *) malloc(sizeof(UTOK));
 

  p_radka=( long *) malloc(sizeof( long));
  
  if ( (fr = f_otevri_test(f_nazev,"r")) == NULL ) {
    #ifdef LADENI
      fprintf(stderr," snort: Soubor '%s' se nepodarilo otevrit", f_nazev);
    #endif
    return(0); /* muzu, nejsou alokovany dynamicke promenne */
  }

  (*p_radka)=0;
   while (fgets(radka, MAX_RADKA, fr) != NULL ) {
    /*printf("%Ld \n",((*p_radka)=(*p_radka)+1) );*/
	
	/*if ( (*p_radka) > 84) { 
		//dyna_pom_vypis();
		fprintf(stdout,"cekam %d radka", (*p_radka));
		getchar();getchar();
	}*/

	if ( kontrola_prvni_treti(radka, fr, p_utok, p_radka) == 1 ) {
		pridej_zaznam(p_utok);
	/*	#ifdef LADENI
		if ((*p_radka) == 1906) {
				nacti_vypis_typy("win.dat");
				getchar();
		}*/
	/*	fprintf(fw,"%s\n", p_utok->nazev_utok);
			fprintf(fw,"%s\n", p_utok->nazev_var);
			fprintf(fw,"%d\n", p_utok->datum);
			fprintf(fw,"%s\n\n", p_utok->ip);
		#endif*/
	}
  	
    if (radka[0] != '\n' ) {
	  while ( (fgets(radka, MAX_RADKA, fr) != NULL ) && (radka[0] != '\n' ) ) {
		  
		  ((*p_radka)=(*p_radka)+1);
		  
	  }	
    ((*p_radka)=(*p_radka)+1); /* pri precteni '\n' nezvysi pocitadlo radek */
	}

  }

  if ( ( close_test(fr,f_nazev) ) == EOF )
    return(0);
  
  return(1);
} 
/* --END vytvor_strukturu-------------------------------------*/

/* --------------kontrola_prvni_treti-------------------------
 * Zkusi nacist tri vyznamove radky
 * Vraci: 0 .. pokud se na nektere radce objevi odchylka od predepsaneho formatu dat
 *		  1 .. vsechny tri radky OK
 * Parametry: f_nazev .. nazev vstupniho souboru
 */
int kontrola_prvni_treti(char buffer[MAX_RADKA], FILE *fr, UTOK *p_utok,  long *p_radka)
{
  
  if ( kontrola_prvni_radka(buffer, p_utok ) == 0 ) {
	  fprintf(stdout,"Snort: %d. radka chyba \n", (*p_radka));
	  #ifdef LADENI
        fprintf(stderr,"snort: Prvni radka chyba \n");
		getchar();      
	  #endif
        return(0);

    }
    fgets(buffer, MAX_RADKA, fr);
	((*p_radka)=(*p_radka)+1);
    if ( kontrola_druha_radka(buffer) == 0 ) {
      fprintf(stdout,"Snort: %d. radka chyba \n", (*p_radka));
	  #ifdef LADENI
        fprintf(stderr,"snort: Druha radka chyba \n");
		getchar();
      #endif
        return(0);
    }

    fgets(buffer, MAX_RADKA, fr);
	((*p_radka)=(*p_radka)+1);
    if ( kontrola_treti_radka(buffer, p_utok) == 0 ) {
	  fprintf(stdout,"Snort: %d. radka chyba \n", ((*p_radka)=(*p_radka)+1));
	   #ifdef LADENI
        fprintf(stderr,"snort: Treti radka chyba \n");
		getchar();
      #endif
        return(0);
    }

	return(1);


}
/* --END vypis_utoky------------------------------------*/


/* ********* definice GLOBALnich funkci */

/* >>>>>>>>> VYPISY >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

/* --------------nacti_vypis_typy-----------------------------------
 * Vytvori dynamickou strukturu a vypise vsechny utoky
 * vyuziva modu styb-dyna.c
 * Vraci: 1
 */
int nacti_vypis_typy(char f_nazev[])
{
	
	if ( vytvor_strukturu(f_nazev) == 0 ) { /* pri vytvareni struktury doslo k chybe v souborech */
		uklid_heap_pamet();
		exit(0);
    }
	dyna_vypis_typy(stdout); /* modul styb-dyna.c */

	return(1);

}
/* --END nacti_vypis_typy-----------------------------------------*/

/* --------------nacti_vypis_podsite-------------------------------------
 * Vytvori dynamickou strukturu a vypise varianty k s_hled_ut
 * vyuziva modu styb-dyna.c
 * Vraci: 1
 */
int nacti_vypis_podsite(char f_nazev[], char s_hled_ut[])
{
	
	if ( vytvor_strukturu(f_nazev) == 0 ) { /* pri vytvareni struktury doslo k chybe v souborech */
		uklid_heap_pamet();
		exit(0);
    }
	dyna_vypis_podsite(stdout, s_hled_ut); /* modul styb-dyna.c */
    
	return(1);
}
/* --END nacti_vypis_podsite----------------------------------*/

/* --------------nacti_vypis_datumy-----------------------------------
 * Vytvori dynamickou strukturu a vypise datumy k dane variante dane
 * indexy ulozenymi v retezci s_hled_ut
 * vyuziva modu styb-dyna.c
 * Vraci: 1
 */
int nacti_vypis_datumy(char f_nazev[], char s_hled_var[])
{
	
	if ( vytvor_strukturu(f_nazev) == 0 ) { /* pri vytvareni struktury doslo k chybe v souborech */
		uklid_heap_pamet();
		exit(0);
    }

	dyna_vypis_datumy(stdout, s_hled_var); /* modul styb-dyna.c */

	return(1);
}
/* --END nacti_vypis_datumy----------------------------------------*/



			  	

