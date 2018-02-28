/* STYB - Styblo Marek (zadani 1) SNORT */
/*
 * styb-jadro.h verze 0.1
 *
 * hlavickovy pro soubor styb-jadro.c
 *
 * MArek Styblo 02.12.2003
 *
 */

 /* ********* definice symbolickych konstant vyuzivanych i v jinych modulech */

 /* ********* definice maker s parametry */

 
 /* ********* deklarace globalnich promennych modulu main.c: extern neco */

 /* ********* uplne funkcni prototypy GLOBALnich funkci modulu main.c: extern fce */

#ifndef nacti
  #define nacti

  extern int nacti_vypis_typy(char f_nazev[]);
  extern int nacti_vypis_podsite(char f_nazev[], char s_hled_ut[]);
  extern int nacti_vypis_datumy(char f_nazev[], char s_hled_var[]);

  extern int vytvor_strukturu(char f_nazev[]);

#endif









