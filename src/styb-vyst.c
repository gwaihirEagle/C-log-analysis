/* STYB - Styblo Marek (zadani 1) SNORT */
/*
* styb-vyst.c verze 0.1
*
* Obhospodaruje vypisy na stdout a stderr
*
* Marek Styblo 02.12.2003
*
*/

/* ********* systemove hlavickove soubory */

#include <stdio.h>
#include <stdlib.h>

/* ********* vlastni hlavickove */

#include "styb-main.h"
#include "styb-io.h"

/* ********* deklarace IMPORTovanych objektu */

/* ********* definice GLOBALnich promennych */

/* ********* lokalni definice symbolickych konstant: #define */

/* ********* lokalni definice novych typu: typedef */

/* ********* definice LOCALnich promennych */

/* ********* uplne funkcni prototypy LOCAL funkci */

/* ********* main() */


/* ********* definice LOCALnich funkci */

/* ********* definice GLOBALnich funkci */

void vypis_hlav(FILE *f);
void vypis_par(FILE *f);
void vypis_inter(FILE *f);
void vypis_nap(FILE *f);
void chyba_par(FILE *f);
void chyba_soub(FILE *f,char f_nazev[]);
void zadne_utoky(FILE *f);

/* --------------vypis_hlav---------------------------
* Vypise text hlavicky napovedy na zvoleny vystup f
* parametry: f..voleny vstup (soubor, stderr ..)
*/
void vypis_hlav(FILE *f)
{
  fprintf(f,"------------------------------------------------------------------------- \n");
  fprintf(f,"Zadani \"snort\" \n");
  fprintf(f,"Zpracoval Marek Styblo (STYB) 2003 \n");
  fprintf(f,"e-mail: marek.styblo@post.cz \n\n");
}
/* --END vypis_hlav----------------------------------*/

/* --------------vypis_par---------------------------
* Vypise vsechny moznosti prepinacu prikazove radky
* parametry: f .. voleny vstup (soubor, stderr ..)
*/
void vypis_par(FILE *f)
{
  fprintf(f,"Parametry prikazove radky: \n");
  fprintf(f," snort.exe ................................ Vypis napovedy \n");
  fprintf(f," snort.exe -h ............................. Vypis napovedy \n");
  fprintf(f," snort.exe -help .......................... Vypis napovedy \n");

  fprintf(f," snort.exe soubor.dat ..................... Interaktivni rezim \n");
  fprintf(f," snort.exe soubor.dat -typy ............... Vypis typu a variant pruniku \n");
  fprintf(f," snort.exe soubor.dat -podsite <typ> ...... Vypis podsiti s pruniky \n");
  fprintf(f," snort.exe soubor.dat -varianty <seznam> .. Vypis dnu vyskytu variant \n");
}
/* --END vypis_par-----------------------------------*/

/* --------------vypis_inter--------------------------
* Vypise moznosti voleb interaktivniho menu, NE samotne menu
* parametry: f..voleny vstup (soubor, stderr ..)
*/
void vypis_inter(FILE *f)
{
  fprintf(f, "\n Volby menu interaktivniho rezimu:  \n");
  fprintf(f," 1 - Napoveda \n");
  fprintf(f," 2 - Typy pruniku \n");
  fprintf(f," 3 - Podsite pro typ pruniku \n");
  fprintf(f," 4 - Varianty pro podsit a den \n");
  fprintf(f," 5 - Konec \n\n");
  fprintf(f,"------------------------------------------------------------------------- \n");
}
/* --END vypis_inter-----------------------------------*/

/* --------------vypis_nap----------------------------
* Vypise text napovedy na zvoleny vystup f
* parametry: f..voleny vstup (soubor, stderr ..)
*/
void vypis_nap(FILE *f)
{
  vypis_hlav(f);
  vypis_par(f);
  vypis_inter(f);
}
/* --END vypis_nap------------------------------------*/

/* --------------chyba_par-----------------------------
* Vypise mozne parametry prikazove radky, pri chybe v prepinaci
*/
void chyba_par(FILE *f)
{
  fprintf(f,"Spatne zadany parametr \n\n");
  vypis_par(stdout);
}
/* --END chyba_par------------------------------------

/* --------------chyba_soub-----------------------------
* Vypise chybu pri otevirani souboru
* parametry: f .. chybovy vystup
*             f_nazev .. jmeno zadaneho souboru, ktery neexistuje
*/
void chyba_soub(FILE *f,char f_nazev[])
{
  fprintf(f,"Zadany nazev souboru: '%s' NEEXISTUJE v aktualnim pwd \n", f_nazev);
  fprintf(f,"Program bude ukoncen.. \n");
  cekej('q');
  exit(0);
}
/* --END chyba_par------------------------------------*/

/* --------------zadne utoky--------------------------
 * Provede se pri nenalezeni utoku
 */ 
void zadne_utoky(FILE *f)
{
	fprintf(f,"---------------------------------------------------------------------\n");
	fprintf(f,"Zadne pokusy nebyly zaznamenany\n");
	fprintf(f,"---------------------------------------------------------------------\n");
}
/* --END zadne_utoky----------------------------------*/


