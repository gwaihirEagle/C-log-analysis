/* STYB - Styblo Marek (zadani 1) SNORT  */
/*
* styb-io.c verze 0.1
*
* Zakladni vstupne vystupni operace
*
* autor Marek Styblo datum 02.12.2003
* Dodelat:
*  + f_otevri_test .. vhodna reakci pri prepisovani v modu "w"
*
*
*/

/* ********* systemove hlavickove soubory */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* ********* vlastni hlavickove */

#include "styb-main.h"
#include "styb-vyst.h"
#include "styb-nacti.h"
#include "styb-dyna.h"


/* ********* deklarace IMPORTovanych objektu */

/* ********* definice GLOBALnich promennych */

/* ********* lokalni definice symbolickych konstant: #define */

/* ********* lokalni definice novych typu: typedef */

/* ********* definice LOCALnich promennych */

/* ********* uplne funkcni prototypy LOCAL funkci */

int cti_znak(FILE *f, int *znak);
int zobraz_menu(char f_nazev[]);

/* ********* uplne funkcni prototypy GLOBAL funkci */
void clear_buffer(void);
void obnov_obraz(FILE *f);
void cekej(int znak_k);
void oramuj_text(FILE *f,char s[], int znak);
int mala_velka(int *c);

FILE *f_otevri_test(char f_nazev[], char mod[]);
int close_test(FILE *f, char f_nazev[]);
int test_io_par(int argc, char *argv[]);
int preved_dat_cislo_retezec(int datum, char s_datum[]);

/* ********* main() */

/* definice LOCALnich funkci */

/* --------------------clear_buffer--------------------
 * Vycisti znakovy buffer pocinaje prectenym do PRECTENI znaku <LF>
 */
void clear_buffer(void)
{
  int c;

  while ( (c=getchar()) != '\n')
    ;
}
/* --END clear_buffer------------------------------*/

/* ---------------obnov_obraz------------------------
 * Vypise 25 znaku enteru '\n' pro "vycisteni" obrazovky
 */
void obnov_obraz(FILE *f)
{
  int i;

  for(i=1; i<=25; i++) {
    fprintf(f,"\n");
  }
}
/* --END obnov_obraz-------------------------------*/

/* --------------------cekej-------------------------
 * Zastavi program a ceka na stisk klavesy q
 *  znak_k .. konec vykonavani procedury
 */
void cekej(int znak_k)
{
  int c;
  printf("--Pokracovani klavesou %c \n", znak_k);
  while ( (c=getchar()) != znak_k)
   ;
  clear_buffer(); /* vycisti znaky \n */
}
/* --END cekej-------------------------------------*/

/* ---------------oramuj_text------------------------
 * Vypise hezky oramovany text
 * Parametry: znak  .. pouzije se k oramovani s .. textu
 */
void oramuj_text(FILE *f,char s[], int znak)
{
  unsigned int i;

  for(i=0; i<(strlen(s)+4); i++) {
    putchar(znak);
  }
  fprintf(f,"\n%c ", znak);

  i=0;
  while (i<(strlen(s))) {
    fprintf(f,"%c", s[i]);
    i++;
  }
  fprintf(f," %c\n", znak);

  for(i=0; i<(strlen(s)+4); i++) {
    fprintf(f,"%c",znak);
  }
  fprintf(f,"\n");
}
/* --END oramuj_text------------------------------*/


/* ---------------mala_velka--------------------------
 * Prevede male pismeno na velke
 * Vraci: 0 .. znak c nebyl pismenem
 *        znak c .. pokud vstup male --> vystup velke, vstup velke --> beze zmeny
 * !Upozorneni: pri 0 navratove hodnote, musis vypsat jako %d, %c nulu nevypise!
 */
int mala_velka(int *c)
{
  if ( isalpha((*c)) ) {

	  if ( islower((*c)) ) 
       (*c)=toupper((*c));
  return( (int) (*c));
  } /* isalpha */

  else /* zadany znak nebylo pismeno */
    return(0);
}
/* --END mala_velka----------------------------------*/

/* ---------------cti_znak---------------------------
 * Precte znak ze zvoleneho vstupu (soubor,stdin),
 * pokud byl vstupem, stdin, vycisti buffer
 * Vraci: precteny znak
 */
int cti_znak(FILE *f, int *znak)
{
  *znak = getc(f);
  if ( f == stdin )
    clear_buffer();
  return(*znak);
}
/* --END cti_znak-----------------------------------*/


/* ********* definice GLOBALnich funkci */

/* --------------f_otevri_test-------------------------
 * Otevre textovy soubor pro cteni, nebo zapis s testem otevreni a prepisu
 * Test provadi na NULL otevrenim souboru pro cteni
 * Parametry: mod .. string pro volbu modu otevreni souboru
 * Vraci: ukazatel na buffer pro soubor  .. otevreni uspech , NULL ..jinak
 */
FILE *f_otevri_test(char f_nazev[], char mod[])
{
  FILE *fr;
  int c;  /* potvrzeni prepisu A,N */

  /* v modu "w" test na prepis existujiciho souboru */
  if (mod[0] == 'w') {  /* nejdrive otestuju existenci otevrenim na cteni */
    if ( (fr=fopen(f_nazev,"r")) != NULL ) {
      fprintf(stderr," snort: Soubor s nazvem '%s' existuje, chces prepsat? [A,N]: ", f_nazev);
      while ( ( cti_znak(stdin,&c), mala_velka(&c) == 0 ) || ( ( c != 'A' ) && ( c != 'N' ) ) )  {
             /* dokud neni spravna odpoved na prepis */
        fprintf(stdout," snort: Zadej [A,N]: ", f_nazev);
      }
      if ( c == 'A') {
         #ifdef LADENI
           fprintf(stdout,"Prepisuji soubor ..");
         #endif
         return( fr = fopen(f_nazev,mod) );
      }
      else {
        #ifdef LADENI
          fprintf(stdout," Vracim null, nic neprepisuji");
        #endif
        return(NULL);
      }
    }
    else /* soubor pro mod 'r' neslo otevrit --> neexistuje --> muzu vytvorit novy */
      return( fr = fopen(f_nazev,mod) );
  } /* mod 'w' */

  if ( (fr=fopen(f_nazev,mod)) == NULL )  {
    fprintf(stderr," snort: Soubor '%s' se nepodarilo otevrit pro cteni \n", f_nazev);
    return(fr);
  }
  return(fr);
}
/* --END f_otevri_test-----------------------------*/

/* -------------close_test---------------------------
 * Zavre soubor s testem, zda se jej podarilo uzavrit
 * Vraci 1 .. Uspech, EOF .. NEuspech
 */
int close_test(FILE *f, char f_nazev[])
{
 if ( fclose(f) == EOF ) {
   fprintf(stderr," snort: Soubor '%s' se nepodarilo uzavrit \n", f_nazev);
   return(EOF);   /* nepodarilo */
 }
 else
   return(1);  /* podarilo */
}
/* --END close_test--------------------------------*/

/* --------------test_io_par-------------------------
 * Zjisti pocet parametru prikazove radky
 * Na zaklade poctu parametru a jejich charakteru
 * preda rizeni prislusnym funkcnim blokum
 */
int test_io_par(int argc, char *argv[])
{
  if ( argc == 1 ) {
    vypis_nap(stdout);
  }
  if ( argc == 2 ) {  /* test 1.prepinace */
    if ( ( strcmp("-h", argv[1]) ) == 0 )
        vypis_nap(stdout);
    else
      if ( ( strcmp("-help", argv[1]) ) == 0 )
        vypis_nap(stdout);
      else
        if ( ( strchr(argv[1],'.') ) != NULL )  /* otestuji, jestli byl zadan soubor */
		  zobraz_menu(argv[1]); /* modul styb-nacti.c */
        else      /* nezabrali prepinace, ani nazev soub --> chybny prepinac */
          chyba_par(stderr);
  } /* argc == 2 */
  if (argc == 3) {
	  if ( ( strcmp("-typy", argv[2]) ) == 0 )
		  nacti_vypis_typy(argv[1]); /* modul styb-nacti.c */
	  else 
		  chyba_par(stderr);
  }
  if (argc == 4) {
	  if ( ( strcmp("-podsite", argv[2]) ) == 0 )
		  nacti_vypis_podsite(argv[1], argv[3]); /* modul styb-nacti.c */
	  else 
		  if ( ( strcmp("-varianty", argv[2]) ) == 0 )
		  nacti_vypis_datumy(argv[1], argv[3]); /* modul styb-nacti.c */
		  else
			  chyba_par(stderr);
	  
		
  }

return(1);

}
/* --END test_io_par--------------------------------*/

/* --------------preved_dat_cislo_retezec-------------
 * Prevede ciselny format datumu 1MesDen (Pr: 10203, je 3.2.) na textovy 
 * format den.mesic.
 *
 * Vraci: 1 .. pokud vse OK
 */
int preved_dat_cislo_retezec(int datum, char s_datum[])
{
	char s_den[MAX_RADKA];
	char s_mes[MAX_RADKA];

	sprintf(s_datum, "%d", datum);
	
	/* ulozeni mesice do stringu */
	if (s_datum[1]=='0') { /* mesic je jednociferne cislo */
		s_mes[0]=s_datum[2];
		s_mes[1]='\0';
	}
	else {
		s_mes[0]=s_datum[1];
		s_mes[1]=s_datum[2];
		s_mes[2]='\0';
	}
	/* ulozeni dne do stringu */
	if (s_datum[3]=='0') { /* den je jednociferne cislo */
		s_den[0]=s_datum[4];
		s_den[1]='\0';
	}
	else {
		s_den[0]=s_datum[3];
		s_den[1]=s_datum[4];
		s_den[2]='\0';
	}

	/* spojeni dne , mesice do jednoho datumu s formatovacimi teckami */
	strcpy(s_datum, s_den);
	strcat(s_datum, ".");
	strcat(s_datum, s_mes);
	strcat(s_datum, ".");

	return(1);

}
/* --END preved_dat_cislo_retezec--------------------------------*/

/* --------------zobraz_menu----------------------------------
 * Zobrazi jednoduche textove menu s moznosti vyberu polozek
 * pomoci cisel 1..5
 * Vraci: 1
 */
int zobraz_menu(char f_nazev[])
{
	FILE *f;

	int volba=1; /* volba zadana uzivatelem */
	char s_hled_ut[MAX_RADKA]; /* hledany utok, zadany uzivatelem */


	if ( (f=f_otevri_test(f_nazev,"r")) == NULL ) 
		exit(0);
	
	vypis_inter(stdout);
	fprintf(stdout, "Tvoje volba (1..5): ");
	
	if ( vytvor_strukturu(f_nazev) == 0 ) { /* pri vytvareni struktury doslo k chybe v souborech */
	  fprintf(stderr," Snort: Chyba pri vytvareni dynamickych struktur");
	  uklid_heap_pamet();
	  exit(0);
	}
	
	while ( volba != 5 ) {
		
		 fscanf(stdin,"%d", &volba);

		 if (volba == 1) {
			 vypis_nap(stdout);
		     fprintf(stdout,"Stiskni cokoliv");
			 obnov_obraz(stdout);
		 }
		 if (volba == 2)
			 nacti_vypis_typy(f_nazev);

		 if (volba == 3) {
			 fprintf(stdout," Zadej typ utoku (Pr: WEB-ISS): ");
			 fscanf(stdin,"%s", s_hled_ut);		
			 nacti_vypis_podsite(f_nazev, s_hled_ut); /* modul styb-nacti.c */
			 getchar();
			 fprintf(stdout,"Stiskni cokoliv");
			 obnov_obraz(stdout);
		 }

		 if (volba == 4) {
			 fprintf(stdout," Zadej indexy utoku,varianty ve formatu..cislo,pismeno (Pr: 1b): ");
			 fscanf(stdin,"%s", s_hled_ut);		
		     nacti_vypis_datumy(f_nazev, s_hled_ut); /* modul styb-nacti.c */
			 getchar();
			 fprintf(stdout,"Stiskni cokoliv");
			 obnov_obraz(stdout);
		 }
		 if (volba>4)
			 fprintf(stdout,"Musis zadat cislo mezi 1..5");

		 vypis_inter(stdout);
	     fprintf(stdout, "Tvoje volba (1..5): ");

	} /* while */

	return(1);
}
/* --END zobraz_menu--------------------------------*/
