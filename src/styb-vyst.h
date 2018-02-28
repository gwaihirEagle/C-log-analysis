/* STYB - Styblo Marek (zadani 1) SNORT */
/*
 * styb-vyst.h verze 0.1
 *
 * hlavickovy pro soubor styb-vyst.c,styb-io.c
 *
 * Marek Styblo 9.11.2003
 *
 */

 /* ********* definice symbolickych konstant vyuzivanych i v jinych modulech */

   
 /* ********* definice maker s parametry */

 /* ********* definice globalnich typu */

 /* ********* deklarace globalnich promennych prislusneho modulu : extern neco */

 /* ********* uplne funkcni prototypy GLOBALnich funkci prislusneho modulu : extern fce */

#ifndef vypisy
  #define vypisy
 
  extern void vypis_hlav(FILE *f);
  extern void vypis_par(FILE *f);
  extern void vypis_inter(FILE *f);
  extern void vypis_nap(FILE *f);
  extern void chyba_par(FILE *f);
  extern void chyba_soub(FILE *f,char f_nazev[]);
  extern void zadne_utoky(FILE *f);

#endif

