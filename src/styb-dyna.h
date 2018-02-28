/* STYB - Styblo Marek (zadani 1) SNORT  */
/*
 * styb-dyna.h verze 0.1
 *
 * Hlavickovy pro styb-main.c
 *
 * Marek Styblo 02.12.2003
 * Dodelat:
 */


/* ********* definice symbolickych konstant vyuzivanych i v jinych modulech */

/* ********* definice maker s parametry */

/* ********* definice globalnich typu */

/* zabezpeceni vicenasobneho vkladani */
#ifndef dyna

  #define dyna

  typedef int t_dat;         /* typ pro ukladani datumu v ciselnem formatu */
  typedef long int t_cetn;   /* cetnosti vyskytu utoku, variant, datumu, ip adres */
 
  typedef struct utok{
	char nazev_utok[255];  /* rezec s nazvem typu utoku: WEB-ISS atd. */
	char nazev_var[255];   /* rezec s nazvem typu utoku: access via atd. */
	int datum;			   /* rezec s nazvem typu utoku: WEB-ISS atd. */
	char ip[255];          /* rezec s cislem ip adresy */
  } UTOK;

  /* dynamicky seznam datumu pristupu pres danou variantu utoku */
  typedef struct datumy {
	int datum;		        	   /* rezec s nazvem typu utoku: WEB-ISS atd. */
	t_cetn pocet_dat;	           /* pocitadlo pristupu pres danou variantu utoku v jednom dni */
	struct datumy *p_next_dat;     /* ukazatel na dalsi datum, kvuli zretezeni */
  } DATUMY;

  /* dynamicky seznam variant utoku */
  typedef struct varianty {
	char *nazev_var;	         /* rezec s nazvem typu utoku: access via atd. */
	t_cetn pocet_var;   	     /* pocitadlo duplicitne se vyskytujicich stejnych variant */
	struct datumy *p_first_dat;  /* ukazatel na seznam datumu pristupu pres danou variantu utoku */
	struct varianty *p_next_var; /* ukazatel na dalsi variantu, kvuli zretezeni */
  } VARIANTY;

  /* seznam ip adres z kterych byli provedeny utoky */
  typedef struct ip_adresy {
	char *nazev_ip;             /* rezec s cislem ip adresy */
	t_cetn pocet_ip; 	        /* pocet pristupu z dane IP adresy */
	struct ip_adresy *p_next_ip;/* ukazatel na dalsi ip_adresu kvuli zretezeni */
  } IP_ADRESY;
	
  /* dynamicky seznam typu utoku */
  typedef struct utoky {
	char *nazev_utok;		    	/* rezec s nazvem typu utoku: WEB-ISS atd. */
	t_cetn pocet_utoku; 		    /* pocitadlo duplicitne se vyskytujicich stejnych utoku */
	struct varianty *p_first_var;	/* ukazatel na prvni variantu k danemu utoku */
	struct ip_adresy *p_first_ip;	/* ukazatel na prvni ip_adresu k danemu utoku */
	struct utoky *p_next_utok;		/* ukazatel na dalsi utok,kvuli zretezeni */
  } UTOKY;

  /* hlavickova struktura, nesouci informaci o RUZnych utocich v seznamu utoku */
  typedef struct ut_hlav {
	int pocet_ruz_utoku;
	t_dat pocatek_datum;
	t_dat konec_datum;
	struct utoky *p_first_ut;
  }UT_HLAV;

  /* ********* deklarace globalnich promennych prislusneho modulu: extern neco */

  /* ********* uplne funkcni prototypy GLOBALnich funkci prislusneho modulu : extern fce */

  extern int pridej_zaznam(UTOK *p_zarazovany);

  extern int dyna_vypis_typy(FILE *f);
  extern int dyna_vypis_podsite(FILE *f, char s_hled_ut[]);
  extern int dyna_vypis_datumy(FILE *f, char s_hled_var[]);

  extern int uklid_heap_pamet(void);
  extern void chyba_alokace_heap(void);

  extern void dyna_pom_vypis(void);

#endif /*dyna */


