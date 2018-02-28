/* STYB - Styblo Marek (zadani 1) SNORT */
/*
 * styb-io.h verze 0.1
 *
 * hlavickovy pro soubor styb-io.c
 *
 * Marek Styblo 08.12.2003
 *
 */

 /* ********* definice symbolickych konstant vyuzivanych i v jinych modulech */

 /* ********* definice maker s parametry */

 /* ********* definice globalnich typu */

 /* ********* deklarace globalnich promennych prislusneho modulu : extern neco */

 /* ********* uplne funkcni prototypy GLOBALnich funkci prislusneho modulu : extern fce */

  
#ifndef vstup_vyst
  #define vstup_vyst

  extern void clear_buffer(void);
  extern void obnov_obraz(FILE *f);
  extern void cekej(int znak_k);
  extern void oramuj_text(FILE *f,char s[], int znak);
  extern int mala_velka(int *c);

  extern FILE *f_otevri_test(char f_nazev[], char mod[]);
  extern int close_test(FILE *f, char f_nazev[]);
  extern int test_io_par(int argc, char *argv[]);
  extern int preved_dat_cislo_retezec(int datum, char s_datum[]);

#endif


