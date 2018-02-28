/* STYB - Styblo Marek (zadani 1) SNORT  */
/*
* styb-dyna.c verze 0.1
*
* Operace s jednosmernymi dynamickymi strukturami
*
* Marek Styblo 02.12.2003
* Dodelat: uvolni_pamet();
*			kam_varianta, problem s pocet_utoku ( inicializuje to kam_utok na 1 a pak jeste kam_varianta na 1 ) 
*			+smazat vypisy pri ruseni heapu
*			+pomocne vypisy projed
*			+upravit projed varianty, kde se postupne vypisuji pismena abecedy pretypovanim intu na char
*
*/

/* ********* SYStemove hlavickove soubory */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>


/* ********* VLAStni hlavickove */

#include "styb-main.h"
#include "styb-io.h"
#include "styb-vyst.h"

/* ********* deklarace IMPORTovanych objektu */

/* ********* definice GLOBALnich promennych */

/* ********* lokalni definice symbolickych konstant: #define */

/* ********* lokalni definice novych typu: typedef */

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


static UT_HLAV *p_ut_hlav=NULL;

/* ********* definice LOCALnich promennych */

/* ********* uplne funkcni prototypy LOCAL funkci */

UTOKY *zarad_na_zacatek_utoky(char s_retezec[MAX_RADKA], UTOKY *p_prvni);
UTOKY *zarad_dovnitr_utoky(char s_retezec[MAX_RADKA], UTOKY *p_pred_aktualni);
UTOKY *zarad_na_konec_utoky(char s_retezec[MAX_RADKA], UTOKY *p_posledni_utok);
UTOKY *kam_utok(UTOK *p_utok);

VARIANTY *kam_varianta(char s_hled_var[], UTOKY *aktual_utok);
VARIANTY *alokuj_variantu(char s_retezec[MAX_RADKA]);
VARIANTY *init_seznam_varianty(char s_retezec[MAX_RADKA], UTOKY *p_utok); 
VARIANTY *zarad_na_zacatek_varianty(char s_retezec[MAX_RADKA], UTOKY *p_utok);
VARIANTY *zarad_dovnitr_varianty(char s_retezec[MAX_RADKA], VARIANTY *p_pred_aktualni);
VARIANTY *zarad_na_konec_varianty(char s_retezec[MAX_RADKA], VARIANTY *p_posledni);

IP_ADRESY *kam_ip(char s_hled_ip[MAX_RADKA], UTOKY *p_utok);
void serad_ip_adresy(UTOKY *p_utok);
IP_ADRESY *alokuj_ip_adresu(char s_retezec[MAX_RADKA]);
IP_ADRESY *init_seznam_ip(char s_retezec[MAX_RADKA], UTOKY *p_utok);
IP_ADRESY *zarad_na_zacatek_ip_adresy(char s_retezec[MAX_RADKA], UTOKY *p_utok, t_cetn cetnost);
IP_ADRESY *zarad_dovnitr_ip_adresy(char s_retezec[MAX_RADKA], IP_ADRESY *p_pred_aktualni, t_cetn cetnost);
IP_ADRESY *zarad_na_konec_ip_adresy(char s_retezec[MAX_RADKA], IP_ADRESY *p_posledni, t_cetn cetnost);

DATUMY *kam_datum(t_dat dat, VARIANTY *p_var);
DATUMY *alokuj_datum(t_dat dat);
DATUMY *init_seznam_datumy(t_dat dat, VARIANTY *p_var);
DATUMY *zarad_na_zacatek_datumy(t_dat dat, VARIANTY *p_var);
DATUMY *zarad_dovnitr_datumy(t_dat dat, DATUMY *p_pred_aktualni);
DATUMY *zarad_na_konec_datumy(t_dat dat, DATUMY *p_posledni);

/* uklid pameti */
UTOKY *zrus_prvek_utok(UTOKY *p_ut);
VARIANTY *zrus_prvek_var(VARIANTY *p_var);
DATUMY *zrus_prvek_dat(DATUMY *p_dat);
IP_ADRESY *zrus_prvek_ip(IP_ADRESY *p_ip);


/* vypis typy */
int projed_utoky(UTOKY *p_utok, FILE *f);
int projed_varianty(VARIANTY *p_var, FILE *f);
/* END vypis typy */

/* vypis -podsite */
int vyhledej_utok(FILE *f, char s_hled_ut[], UTOKY **p_utok);
int vypis_typy_podsite(FILE *f, UTOKY *p_utok);
int vypis_znak(FILE *f, char c, int pocet);
/* END vypis -podsite */

/* vypis datumu k variante */
int cti_var_cislo(char s_var[], int pozice, int *cislo);
int cti_var_pism(char s_var[], int pozice, char *pismeno);
int zkonvertuj_pismeno_na_cislo(char pismeno[]);
int vyhledej_utok_var(int *cislo_ut, UTOKY **p_utok);
int vyhledej_variantu(int cislo_var, UTOKY *p_utok, VARIANTY **p_var);
int vypis_datumy(FILE *f, VARIANTY *p_var, int *cislo_ut, char pismeno_var[]);
/* END vypis datumu k variante */


/* ********* uplne funkcni prototypy GLOBAL funkci */
int pridej_zaznam(UTOK *p_zarazovany);

int dyna_vypis_typy(FILE *f);
int dyna_vypis_podsite(FILE *f, char s_hled_ut[]);
int dyna_vypis_datumy(FILE *f, char s_hled_var[]);

int uklid_heap_pamet(void);
void chyba_alokace_heap(void);


/* ********* main() */

/* ********* definice LOCALnich funkci */

void odstran_enter(char s_buffer[])
{
	s_buffer[strlen(s_buffer)-1]='\0';
}


/* ------------------------chyba_alokace_heap-----------------------------------
 * Vypise varovne hlaseni o nedostatku velikosti heapu a uklidi dynamicke prom 
 */
 
void chyba_alokace_heap(void)
{
	fprintf(stderr,"Snort: Chyba pri alokaci heapu, uvolnuji pamet a koncim..");
	uklid_heap_pamet();
}
/* --END chyba_alokace_heap---------------------------------------------------*/
	
/* -------------------alokuj_utok-----------------------------------------------
 * Alokuje dynamickou pamet pro prvek seznamu UTOKY
 * a naplni jeho vyznamovou hodnotu promenou 's_retezec'
 * nastavi ukazatele na seznamy: VARIANTY a IP_ADRESY NULL
 * Vraci: ukazatel na alokovanou polozku 
 */
UTOKY *alokuj_utok(char s_retezec[MAX_RADKA]) 
{
	UTOKY *p_novy;

	if ( (p_novy=(UTOKY *) malloc(sizeof(UTOKY)) ) == NULL )
		chyba_alokace_heap();
			
	if ( (p_novy->nazev_utok=(char *) malloc( (1+strlen(s_retezec))*sizeof(char)) ) == NULL )
		chyba_alokace_heap();
	
	strcpy(p_novy->nazev_utok, s_retezec);
	p_novy->pocet_utoku=1;     
	p_novy->p_first_var=NULL;
	p_novy->p_first_ip=NULL;
	return(p_novy);
}
/* --END alokuj utok----------------------------------------------------------*/


/* -------------------zarad_na_zacatek_utoky------------------------------------
 * Prida novou polozku PRED polozku na kterou ukazuje pointer p_prvni 
 * Nastavi ukazatel hlavickove struktury hlavniho seznamu UTOKY (p_ut_hlav->p_first_ut)
 * na novy prvek 
 * Vraci: ukazatel na zarazenou polozku
 */
UTOKY *zarad_na_zacatek_utoky(char s_retezec[MAX_RADKA], UTOKY *p_prvni)
{
	p_prvni=(alokuj_utok(s_retezec));
	p_prvni->p_next_utok=p_ut_hlav->p_first_ut;     /* dosavadni prvni se stava druhym */
	p_ut_hlav->p_first_ut=p_prvni;	     		    /* novy zarazuji na zacatek */

	return(p_prvni);
}
/* --END zarad_na_zacatek_utoky-----------------------------------------------*/

/* -------------------zarad_dovnitr_utoky---------------------------------------
 * Prida novou polozku na pozici ZA prvek, na ktery ukazuje p_pred_aktualni
 * Vraci: ukazatel na zarazenou polozku
 */
UTOKY *zarad_dovnitr_utoky(char s_retezec[MAX_RADKA], UTOKY *p_pred_aktualni)
{
	UTOKY *p_zaloha; /* ulozeni zalohy pointeru na vetsi utok, nez aktualni */

	p_zaloha=p_pred_aktualni->p_next_utok;
	p_pred_aktualni->p_next_utok=(alokuj_utok(s_retezec));
	p_pred_aktualni->p_next_utok->p_next_utok=p_zaloha;
	
	return(p_pred_aktualni->p_next_utok);
}
/* --END zarad_dovnitr_utoky--------------------------------------------------*/

/* -------------------zarad_na_konec_utoky--------------------------------------
 * Prida novou polozku ZA polozku na kterou ukazuje pointer p_posledni
 * Vraci: ukazatel na zarazenou polozku
 */ 
UTOKY *zarad_na_konec_utoky(char s_retezec[MAX_RADKA], UTOKY *p_posledni)
{
	
	p_posledni->p_next_utok=(alokuj_utok(s_retezec)); /* zretezeni do seznamu utoky */
	p_posledni->p_next_utok->p_next_utok=NULL;	/* konec zarazovaneho ukoncim NULLem */

	return(p_posledni->p_next_utok);
}
/* --END zarad_na_konec_utoky-------------------------------------------------*/

/* -------------------------------kam_utok--------------------------------------
 * Prohledava seznam UTOKY dokud jsou prvky abecedne mensi nez s_hled_ut ( zarazovany retezec )
 * Provede analyzu pozice v seznamu UTOKY, na jejimz zaklade vytvori a zaradi novy prvek
 *
 * Pokud vytvarim novy prvek v seznamu UTOKY --> byl nalezen NOVy utok --> zvysim pocitadlo
 * hlavickove strkuktury p_ut_hlav->pocet_ruz_utoku++
 *
 * Pri nalezeni stejneho utoku v seznamu pouze inkrementuje pocitadlo pocet_utoku
 *
 * Vraci: ukazatel na nove zarazenou polozku v seznamu UTOKY
 */
UTOKY *kam_utok(UTOK *p_utok)
{
	UTOKY *p_aktualni, *p_pred_aktualni;
	UTOKY *p_novy_ut;
	char s_hled_ut[MAX_RADKA];

	strcpy(s_hled_ut, p_utok->nazev_utok);
	
      /* POZOR tady to musim !!! vyresit s *p_ut_hlav */
	if ( (p_ut_hlav) == NULL ) { /* seznam je treba zinicializovat, vcetne hlavicky */
		p_novy_ut=alokuj_utok(s_hled_ut);
		p_novy_ut->p_next_utok=NULL;
		/* inicializace hlavicky, ktera je "korenem stromu" */
	  if ( ((p_ut_hlav)=(UT_HLAV *) malloc( sizeof(UT_HLAV) ) ) == NULL )
	    chyba_alokace_heap();

	   (p_ut_hlav)->p_first_ut=p_novy_ut;
       (p_ut_hlav)->pocet_ruz_utoku=1;
       (p_ut_hlav)->pocatek_datum=p_utok->datum;
	return(p_novy_ut);
	}

	p_aktualni=(p_ut_hlav)->p_first_ut;
	p_pred_aktualni=p_aktualni;
	
	   

	 /* dokud je zarazovany retezec vetsi (strcmp vraci 1) nez aktualni v seznamu */
	while ( (p_aktualni != NULL ) && ((strcmp(s_hled_ut, p_aktualni->nazev_utok)) > 0) ) {
		
	
        

		p_pred_aktualni=p_aktualni;
		p_aktualni=p_aktualni->p_next_utok;

		
	}
   /* printf("\n--Pomocny vypis celeho seznamu: \n");
	dyna_pom_vypis();
	printf("aktualni:   \n   %s\n", p_aktualni->nazev_utok);
	printf("pred_aktualni:    \n   %s\n", p_pred_aktualni->nazev_utok);
	printf("hledany:   %s\n", s_hled_ut);!!!!*/

	if ( (p_aktualni) == NULL ) {						      /* jsem na konci seznamu */
		(p_ut_hlav)->pocet_ruz_utoku++;
		return(zarad_na_konec_utoky(s_hled_ut, p_pred_aktualni));
	}
	else 
		if ( (strcmp(s_hled_ut, p_aktualni->nazev_utok)) == 0 ) { /* zarazovany utok jiz je v seznamu */
			p_aktualni->pocet_utoku++;
			return(p_aktualni);
		}
		else 
			if ( (p_aktualni == (p_ut_hlav)->p_first_ut) ) {  /* zarazovany patri na zacatek seznamu */
				(p_ut_hlav)->pocet_ruz_utoku++;
				return(zarad_na_zacatek_utoky(s_hled_ut, p_aktualni));
			}
			else {
				(p_ut_hlav)->pocet_ruz_utoku++;
				return(zarad_dovnitr_utoky(s_hled_ut,p_pred_aktualni)); /* zarazovany patri nekam dovnitr seznamu */
			
			}
			
}
/* --END kam_utok-------------------------------------------------------------*/

/* -------------------alokuj_variantu-------------------------------------------
 * Alokuje dynamickou pamet pro prvek seznamu VARIANTY
 * a naplni jeho vyznamovou hodnotu promenou 's_retezec'
 * nastavi ukazatele na seznam: DATUMY
 * Vraci: ukazatel na alokovanou polozku 
 */
VARIANTY *alokuj_variantu(char s_retezec[MAX_RADKA]) 
{
	VARIANTY *p_novy;

	if ( (p_novy=(VARIANTY *) malloc(sizeof(VARIANTY)) ) == NULL )
		chyba_alokace_heap();
			
	if ( (p_novy->nazev_var=(char *) malloc( (1+strlen(s_retezec))*sizeof(char)) ) == NULL )
		chyba_alokace_heap();
	
	strcpy(p_novy->nazev_var, s_retezec);
	p_novy->pocet_var=1;
	p_novy->p_first_dat=NULL;
	
	return(p_novy);
}
/* --END alokuj variantu------------------------------------------------------*/

VARIANTY *init_seznam_varianty(char s_retezec[MAX_RADKA], UTOKY *p_utok) 
{
	(p_utok->p_first_var) = (alokuj_variantu(s_retezec));
	p_utok->p_first_var->p_first_dat=NULL;
	p_utok->p_first_var->p_next_var=NULL;
	return(p_utok->p_first_var);
}

/* -------------------zarad_na_zacatek_varianty---------------------------------
 * Prida novou polozku PRED polozku na kterou ukazuje pointer p_prvni 
 * Nastavi ukazatel prvku seznamu UTOKY (utoky->p_first_var) na novy prvek 
 * Vraci: ukazatel na zarazenou polozku
 */
VARIANTY *zarad_na_zacatek_varianty(char s_retezec[MAX_RADKA], UTOKY *p_utok)
{
	VARIANTY *p_prvni;

	p_prvni=(alokuj_variantu(s_retezec));
	p_prvni->p_next_var=p_utok->p_first_var;     /* dosavadni prvni se stava druhym */
	p_prvni->pocet_var=1;
	p_prvni->p_first_dat=NULL;
	p_utok->p_first_var=p_prvni;      
	
	return(p_prvni);
}
/* --END zarad_na_zacatek_varianty-----------------------------------------------*/

/* -------------------zarad_dovnitr_varianty---------------------------------------
 * Prida novou polozku na pozici ZA prvek, na ktery ukazuje p_pred_aktualni
 * Vraci: ukazatel na zarazenou polozku
 */
VARIANTY *zarad_dovnitr_varianty(char s_retezec[MAX_RADKA], VARIANTY *p_pred_aktualni)
{
	VARIANTY *p_zaloha; /* ulozeni zalohy pointeru na vetsi variantu, nez aktualni */

	p_zaloha=p_pred_aktualni->p_next_var;
	p_pred_aktualni->p_next_var=(alokuj_variantu(s_retezec));
	p_pred_aktualni->p_next_var->pocet_var=1;
	p_pred_aktualni->p_next_var->p_first_dat=NULL;
	p_pred_aktualni->p_next_var->p_next_var=p_zaloha;
	
	return(p_pred_aktualni->p_next_var);
}
/* --END zarad_dovnitr_varianty--------------------------------------------------*/

/* -------------------zarad_na_konec_varianty----------------------------------
 * Prida novou polozku ZA polozku na kterou ukazuje pointer p_posledni
 * Vraci: ukazatel na zarazenou polozku
 */ 
VARIANTY *zarad_na_konec_varianty(char s_retezec[MAX_RADKA], VARIANTY *p_posledni)
{
	
	p_posledni->p_next_var=(alokuj_variantu(s_retezec)); /* zretezeni do seznamu varianty */
	p_posledni->p_next_var->pocet_var=1;		/* prvni nova varianta */
	p_posledni->p_next_var->p_first_dat=NULL;   /* musim inicializovat seznam datumu */
	p_posledni->p_next_var->p_next_var=NULL;	/* konec zarazovaneho ukoncim NULLem */

	return(p_posledni->p_next_var);
}
/* --END zarad_na_konec_varianty---------------------------------------------*/


/* -------------------------------kam_varianta--------------------------------
 * Zacne prohledavat seznam VARIANTY, na jehoz prvni polozku ukazuje p_utok->p_first_var
 * Prohledava seznam VARIANTY dokud jsou prvky abecedne mensi nez s_hled_var ( zarazovany retezec )
 * Provede analyzu pozice v seznamu VARIANTY, na jejimz zaklade vytvori a zaradi novy prvek
 *
 * Pri nalezeni stejne varianty v seznamu pouze inkrementuje pocitadlo p_utok->pocet_utoku++
 *
 * Vraci: ukazatel na nove zarazenou polozku v seznamu VARIANTY
 */
VARIANTY *kam_varianta(char s_hled_var[], UTOKY *p_utok)
{
	VARIANTY *p_aktualni, *p_pred_aktualni;
	
	
	p_aktualni=p_utok->p_first_var;
	p_pred_aktualni=p_aktualni;
	
	if ( p_aktualni == NULL ) { /* Nove inicializovany utok, ktery ma odkaz na seznam VARIANTY == NULL */
		return(init_seznam_varianty(s_hled_var,p_utok)); 
	}
	
	 /* dokud je zarazovany retezec mensi (strcmp vraci -1) nez aktualni v seznamu */
	while ( (p_aktualni != NULL ) && ((strcmp(s_hled_var, p_aktualni->nazev_var)) > 0) ) {
		p_pred_aktualni=p_aktualni;
		p_aktualni=p_aktualni->p_next_var;
	}

	if ( (p_aktualni) == NULL ) {	/* jsem na konci seznamu, tedy zarazovany jeste neni v seznamu */
		return(zarad_na_konec_varianty(s_hled_var, p_pred_aktualni));
	}
	else 
		if ( (strcmp(s_hled_var, p_aktualni->nazev_var)) == 0 ) { /* zarazovana varianta jiz je v seznamu */
			p_aktualni->pocet_var++;
			return(p_aktualni);
		}
		else 
			if  ( p_aktualni == (p_utok->p_first_var) )  {  /* zarazovany patri na zacatek seznamu */
				return(zarad_na_zacatek_varianty(s_hled_var, p_utok));
			}
			else {
				return(zarad_dovnitr_varianty(s_hled_var,p_pred_aktualni)); /* zarazovany patri nekam dovnitr seznamu */
			
			}
			
}
/* --END kam_varianta--------------------------------------------------------*/

/* -------------------alokuj_datum-------------------------------------------
 * Alokuje dynamickou pamet pro prvek seznamu DATUMY
 * a naplni jeho vyznamovou hodnotu promenou dat
 * nastavi ukazatele na seznam: DATUMY
 * Vraci: ukazatel na alokovanou polozku 
 */
DATUMY *alokuj_datum(t_dat dat) 
{
	DATUMY *p_novy;

	if ( (p_novy=(DATUMY *) malloc(sizeof(DATUMY)) ) == NULL )
		chyba_alokace_heap();
			
	p_novy->datum=dat;
	p_novy->pocet_dat=1;

	
	return(p_novy);
}
/* --END alokuj datum--------------------------------------------------------*/

DATUMY *init_seznam_datumy(t_dat dat, VARIANTY *p_var) 
{
	(p_var->p_first_dat) = (alokuj_datum(dat));
	p_var->p_first_dat->p_next_dat=NULL;
	
	return(p_var->p_first_dat);
}

/* -------------------zarad_na_zacatek_datumy---------------------------------
 * Prida novou polozku PRED polozku na kterou ukazuje pointer p_prvni 
 * Nastavi ukazatel prvku seznamu VARIANTY (varianty->p_first_dat) na novy prvek 
 * Vraci: ukazatel na zarazenou polozku
 */
DATUMY *zarad_na_zacatek_datumy(t_dat dat, VARIANTY *p_var)
{
	DATUMY *p_prvni;

	p_prvni=(alokuj_datum(dat));
	p_prvni->p_next_dat=p_var->p_first_dat;  /* dosavadni prvni se stava druhym */
	p_prvni->pocet_dat=1;
	p_var->p_first_dat=p_prvni;      
	
	return(p_prvni);
}
/* --END zarad_na_zacatek_datumy---------------------------------------------*/

/* -------------------zarad_dovnitr_datumy---------------------------------------
 * Prida novou polozku na pozici ZA prvek, na ktery ukazuje p_pred_aktualni
 * Vraci: ukazatel na zarazenou polozku
 */
DATUMY *zarad_dovnitr_datumy(t_dat dat, DATUMY *p_pred_aktualni)
{
	DATUMY *p_zaloha; /* ulozeni zalohy pointeru na vetsi datum, nez aktualni */

	p_zaloha=p_pred_aktualni->p_next_dat;
	p_pred_aktualni->p_next_dat=(alokuj_datum(dat));
	p_pred_aktualni->p_next_dat->pocet_dat=1;
	p_pred_aktualni->p_next_dat->p_next_dat=p_zaloha;
	
	return(p_pred_aktualni->p_next_dat);
}
/* --END zarad_dovnitr_datumy--------------------------------------------------*/

/* -------------------zarad_na_konec_datumy----------------------------------
 * Prida novou polozku ZA polozku na kterou ukazuje pointer p_posledni
 * Vraci: ukazatel na zarazenou polozku
 */ 
DATUMY *zarad_na_konec_datumy(t_dat dat, DATUMY *p_posledni)
{
	
	p_posledni->p_next_dat=(alokuj_datum(dat)); /* zretezeni do seznamu datumy */
	p_posledni->p_next_dat->pocet_dat=1;		/* prvni novy datum */
	p_posledni->p_next_dat->p_next_dat=NULL;	/* konec zarazovaneho ukoncim NULLem */

	return(p_posledni->p_next_dat);
}
/* --END zarad_na_konec_datumy---------------------------------------------*/

/* -------------------------------kam_datum--------------------------------
 * Zacne prohledavat seznam DATUMY, na jehoz prvni polozku ukazuje p_var->p_first_dat
 * Prohledava seznam DATUMY dokud jsou prvky ciselne ( mam takto zkonvertovany datum: 1MesDen) mensi
 * nez zarazovany datum
 * Provede analyzu pozice v seznamu VARIANTY, na jejimz zaklade vytvori a zaradi novy prvek,
 *
 * Pri nalezeni stejneho datumu v seznamu pouze inkrementuje pocitadlo
 *
 * Vraci: ukazatel na nove zarazenou polozku v seznamu DATUMY
 */
DATUMY *kam_datum(t_dat dat, VARIANTY *p_var)
{
	DATUMY *p_aktualni, *p_pred_aktualni;
	
	
	(p_ut_hlav)->konec_datum=dat;  /* prubezny citac datumu, prepoklada, ze zaznamy jsou do
									 * souboru ukladany sekvencne podle dat */
	
	p_aktualni=p_var->p_first_dat;
	p_pred_aktualni=p_aktualni;
	
	if ( p_aktualni == NULL ) { /* Nove inicializovana varianta, ktera ma odkaz na seznam DATUMY == NULL */
		return(init_seznam_datumy(dat, p_var)); 
	}
	
	 /* dokud je zarazovany datum mensi nez aktualni v seznamu */
	while ( (p_aktualni != NULL ) && (dat > p_aktualni->datum) ) {
		p_pred_aktualni=p_aktualni;
		p_aktualni=p_aktualni->p_next_dat;
	}

	if ( (p_aktualni) == NULL ) {	/* jsem na konci seznamu, tedy zarazovany jeste neni v seznamu */
		return(zarad_na_konec_datumy(dat, p_pred_aktualni));
	}
	else 
		if ( dat == (p_aktualni->datum) ) { /* zarazovany datum jiz je v seznamu */
			p_aktualni->pocet_dat++;
			return(p_aktualni);
		}
		else 
			if  ( p_aktualni == (p_var->p_first_dat) )  {  /* zarazovany patri na zacatek seznamu */
				return(zarad_na_zacatek_datumy(dat, p_var));
			}
			else {
				return(zarad_dovnitr_datumy(dat,p_pred_aktualni)); /* zarazovany patri nekam dovnitr seznamu */
			
			}
			
}
/* --END kam_varianta--------------------------------------------------------*/

/* -------------------alokuj_ip_adresu-----------------------------------------
 * Alokuje dynamickou pamet pro prvek seznamu IP_ADRESY
 * a naplni jeho vyznamovou hodnotu promenou 's_retezec'
 * nastavi ukazatele na seznam: IP_ADRESY
 * Vraci: ukazatel na alokovanou polozku 
 */
IP_ADRESY *alokuj_ip_adresu(char s_retezec[MAX_RADKA]) 
{
	IP_ADRESY *p_novy;

	if ( (p_novy=(IP_ADRESY *) malloc(sizeof(IP_ADRESY)) ) == NULL )
		chyba_alokace_heap();
			
	if ( (p_novy->nazev_ip=(char *) malloc( (1+strlen(s_retezec))*sizeof(char)) ) == NULL )
		chyba_alokace_heap();
	strcpy(p_novy->nazev_ip, s_retezec);
	p_novy->pocet_ip=1;
	
	return(p_novy);
}
/* --END alokuj_ip_adresu----------------------------------------------------*/


IP_ADRESY *init_seznam_ip(char s_retezec[MAX_RADKA], UTOKY *p_utok) 
{
	(p_utok->p_first_ip) = (alokuj_ip_adresu(s_retezec));
	p_utok->p_first_ip->p_next_ip=NULL;

	return(p_utok->p_first_ip);
}

/* -------------------zarad_na_zacatek_ip_adresy-------------------------------
 * Prida novou polozku PRED polozku na kterou ukazuje pointer p_prvni 
 * Nastavi ukazatel prvku seznamu UTOKY (p_utok->p_first_ip) na novy prvek 
 * Vraci: ukazatel na zarazenou polozku
 */
IP_ADRESY *zarad_na_zacatek_ip_adresy(char s_retezec[MAX_RADKA], UTOKY *p_utok, t_cetn cetnost)
{
	IP_ADRESY *p_prvni;

	p_prvni=(alokuj_ip_adresu(s_retezec));
	p_prvni->p_next_ip=p_utok->p_first_ip;  /* dosavadni prvni se stava druhym */
	p_prvni->pocet_ip=cetnost;
	p_utok->p_first_ip=p_prvni;      
	
	return(p_prvni);
}
/* --END zarad_na_zacatek_ip_adresy-------------------------------------------*/

/* -------------------zarad_dovnitr_ip_adresy-----------------------------------
 * Prida novou polozku na pozici ZA prvek, na ktery ukazuje p_pred_aktualni
 * Vraci: ukazatel na zarazenou polozku
 */
IP_ADRESY *zarad_dovnitr_ip_adresy(char s_retezec[MAX_RADKA], IP_ADRESY *p_pred_aktualni, t_cetn cetnost)
{
	IP_ADRESY *p_zaloha; /* ulozeni zalohy pointeru na vetsi, nebo rovnu cetnost, nez aktualni */

	p_zaloha=p_pred_aktualni->p_next_ip;
	p_pred_aktualni->p_next_ip=(alokuj_ip_adresu(s_retezec));
	p_pred_aktualni->p_next_ip->pocet_ip=cetnost;
	p_pred_aktualni->p_next_ip->p_next_ip=p_zaloha;
	
	return(p_pred_aktualni->p_next_ip);
}
/* --END zarad_dovnitr_ip_adresy-----------------------------------------------*/

/* -------------------zarad_na_konec_ip_adresy---------------------------------
 * Prida novou polozku ZA polozku na kterou ukazuje pointer p_posledni
 * Vraci: ukazatel na zarazenou polozku
 */ 
IP_ADRESY *zarad_na_konec_ip_adresy(char s_retezec[MAX_RADKA], IP_ADRESY *p_posledni, t_cetn cetnost)
{
	
	p_posledni->p_next_ip=(alokuj_ip_adresu(s_retezec)); /* zretezeni do seznamu ip_adresy */
	p_posledni->p_next_ip->pocet_ip=cetnost;	         /* prvni nova ip_adresa */
	p_posledni->p_next_ip->p_next_ip=NULL;	             /* konec zarazovaneho ukoncim NULLem */

	return(p_posledni->p_next_ip);
}
/* --END zarad_na_konec_ip_adresy--------------------------------------------*/


/* -------------------------------kam_ip--------------------------------
 * Zacne prohledavat seznam IP_ADRESY, na jehoz prvni polozku ukazuje p_utok->p_first_ip
 * Prohledava seznam VARIANTY dokud nenajde zarazovanou ip%adresu v seznamu, nebo nedojde na konec
 * pokud dojde na konec, zaradi novou ip%adresu na konec
 * pokud nalezne polozku v seznamu, zvysi pocitadlo pocet_ip
 *
 * Vraci: ukazatel na nove zarazenou polozku v seznamu VARIANTY
 */
IP_ADRESY *kam_ip(char s_hled_ip[MAX_RADKA],  UTOKY *p_utok)
{
	IP_ADRESY *p_aktualni, *p_pred_aktualni;
	
	
	p_aktualni=p_utok->p_first_ip;
	p_pred_aktualni=p_aktualni;
	
	if ( p_aktualni == NULL ) { /* Nove inicializovany utok, ktery ma odkaz na seznam IP_ADRESY == NULL */
		return(init_seznam_ip(s_hled_ip, p_utok)); 
	}
	
	 /* dokud neni zarazovany retezec stejny (strcmp vraci 0) jako aktualni v seznamu, nebo nejsem na konci */
	while ( (p_aktualni != NULL ) && ((strcmp(s_hled_ip, p_aktualni->nazev_ip)) != 0) ) {
		p_pred_aktualni=p_aktualni;
		p_aktualni=p_aktualni->p_next_ip;
	}



	if ( (p_aktualni) == NULL ) {	/* jsem na konci seznamu, tedy zarazovany jeste neni v seznamu */
		return(zarad_na_konec_ip_adresy(s_hled_ip, p_pred_aktualni, 1));
	}
	else 
		if ( (strcmp(s_hled_ip, p_aktualni->nazev_ip)) == 0 ) { /* zarazovana ip_adresa jiz je v seznamu */
			p_aktualni->pocet_ip++;
			return(p_aktualni);
		}

	return(NULL); /* pokud nezabere ani jedna vetev, ALE to by se nemelo stat nikdy */
			
}
/* --END kam_ip---------------------------------------------------------------*/

/* -------------------------------zrus_prvek_utok-------------------------------
 * Zrusi prvni prvek seznamu UTOKY a posune ukazovatko na prvni prvek seznamu (p_ut) na dalsi 
 */
UTOKY *zrus_prvek_utok(UTOKY *p_ut)
{
	UTOKY *p_ruseny;

	p_ruseny=p_ut;
	p_ut=p_ut->p_next_utok;
	free(p_ruseny);

	return(p_ut);

}
/* --END zrus_prek_utok---------------------------------------------------------*/

/* -------------------------------zrus_prvek_var---------------------------------
 * Zrusi prvni prvek seznamu VARIANTY a posune ukazovatko na prvni prvek seznamu (p_var) na dalsi 
 */
VARIANTY *zrus_prvek_var(VARIANTY *p_var)
{
	VARIANTY *p_ruseny;

	p_ruseny=p_var;
	p_var=p_var->p_next_var;
	free(p_ruseny);

	return(p_var);

}
/* --END zrus_prvek_var---------------------------------------------------------*/

/* -------------------------------zrus_prvek_dat---------------------------------
 * Zrusi prvni prvek seznamu DATUMY a posune ukazovatko na prvni prvek seznamu (p_dat) na dalsi 
 */
DATUMY *zrus_prvek_dat(DATUMY *p_dat)
{
	DATUMY *p_ruseny;

	p_ruseny=p_dat;
	p_dat=p_dat->p_next_dat;
	free(p_ruseny);

	return(p_dat);

}
/* --END zrus_prvek_dat---------------------------------------------------------*/

/* -------------------------------zrus_prvek_ip---------------------------------
 * Zrusi prvni prvek seznamu IP_ADRESY a posune ukazovatko na prvni prvek seznamu (p_ip) na dalsi 
 */
IP_ADRESY *zrus_prvek_ip(IP_ADRESY *p_ip)
{
	IP_ADRESY *p_ruseny;

	p_ruseny=p_ip;
	p_ip=p_ip->p_next_ip;
	free(p_ruseny);

	return(p_ip);

}
/* --END zrus_prvek_ip---------------------------------------------------------*/

/* -------------------------------serad_ip_adresy-------------------------------
 * Seradi seznam IP_ADRESY prislusejici k danemu utoku p_utok podle cetnosti sestupne
 */
void serad_ip_adresy(UTOKY *p_utok)
{
	IP_ADRESY *p_puv_ip;    /* prechodne ulozeni pocatku seznamu ip adres */
	IP_ADRESY *p_ruseny;    /* pomocny ukazatel na ruseny prvek */
	IP_ADRESY *p_novy_ip;   /* beha v nove vytvarenem seznamu */
	IP_ADRESY *p_novy_pred_ip; /* ulozeni predchoziho prvku */


	p_puv_ip=p_utok->p_first_ip; /* zaloha seznamu ip_adres */

	/* "vymaz" seznamu ip_adres, naplneni pouze jednim puvodnim prvkem */
	/* odkaz na puvodni seznam je v p_puv_ip */
	p_utok->p_first_ip=init_seznam_ip(p_puv_ip->nazev_ip, p_utok); 
	p_utok->p_first_ip->pocet_ip=p_puv_ip->pocet_ip;
	
	p_ruseny=p_puv_ip; /* jeden prvek z puvodniho seznamu jsem zaradil --> puvodni seznam uberu */
	p_puv_ip=p_puv_ip->p_next_ip;
	free(p_ruseny);

	 /* razeni nove vytvareneho seznamu ip-cek podle cetnosti vykystu sestupne */
	while ( p_puv_ip != NULL ) {
	  p_novy_ip=p_utok->p_first_ip; /* ulozeni ukazatele na prvni prvek v novem seznamu */
	  p_novy_pred_ip=p_novy_ip;

		/* dokud je p_puv_ip->pocet_ip mensi nez aktualni prvek v novem seznamu */
	  while ( (p_novy_ip != NULL ) && (p_puv_ip->pocet_ip < p_novy_ip->pocet_ip) ) {
		p_novy_pred_ip=p_novy_ip;
		p_novy_ip=p_novy_ip->p_next_ip;
	  }

	  if ( (p_novy_ip) == NULL ) {	/* jsem na konci seznamu, tedy zarazovany je nejmensi(cetnost) */
	    (zarad_na_konec_ip_adresy(p_puv_ip->nazev_ip, p_novy_pred_ip, p_puv_ip->pocet_ip));
	  }
	    else 
			if  ( p_novy_ip == (p_utok->p_first_ip) )  {  /* zarazovany patri na zacatek seznamu */
			  (zarad_na_zacatek_ip_adresy(p_puv_ip->nazev_ip, p_utok, p_puv_ip->pocet_ip));
	  		}
			else {
			  (zarad_dovnitr_ip_adresy(p_puv_ip->nazev_ip, p_novy_pred_ip, p_puv_ip->pocet_ip));
				/* zarazovany patri nekam dovnitr seznamu */
			}

	p_puv_ip=zrus_prvek_ip(p_puv_ip);

	}
			
}
/* --END kam_varianta--------------------------------------------------------*/

/* -----------------------uklid_heap_pamet--------------------------------------
 * Zrusi dynamicke seznamy alokovane v heap pameti
 * Vraci: 1 .. pokud vse ok
 */
int uklid_heap_pamet(void)
{
	FILE *f;

	f=fopen("vyss","w");

	/* zrusi UTOKY */
	while ( (p_ut_hlav->p_first_ut) != NULL ) {
		fprintf(f,"utok: %s", p_ut_hlav->p_first_ut->nazev_utok);
	    fprintf(f,"    pocet: %d \n", p_ut_hlav->p_first_ut->pocet_utoku);
		/* zrusi VARIANTY k prislusnemu utoku */
		while ( (p_ut_hlav->p_first_ut->p_first_var) != NULL ) {
			fprintf(f,"varianta: %s", p_ut_hlav->p_first_ut->p_first_var->nazev_var);
        	fprintf(f,"    pocet:  %Ld \n", p_ut_hlav->p_first_ut->p_first_var->pocet_var);
			/*zrusi DATUMY k prislusne variante */
			while ( ((p_ut_hlav->p_first_ut->p_first_var->p_first_dat) != NULL ) ) {
				fprintf(f,"datum: %d", p_ut_hlav->p_first_ut->p_first_var->p_first_dat->datum);
		    	fprintf(f,"   pocet:%d \n\n", p_ut_hlav->p_first_ut->p_first_var->p_first_dat->pocet_dat);

					p_ut_hlav->p_first_ut->p_first_var->p_first_dat=zrus_prvek_dat(p_ut_hlav->p_first_ut->p_first_var->p_first_dat);
			}

		p_ut_hlav->p_first_ut->p_first_var=zrus_prvek_var(p_ut_hlav->p_first_ut->p_first_var);
		}
		/* zrusi IP_ADRESY k prislusnemu utoku */
		while ( (p_ut_hlav->p_first_ut->p_first_ip) != NULL ) {
			p_ut_hlav->p_first_ut->p_first_ip=zrus_prvek_ip(p_ut_hlav->p_first_ut->p_first_ip);
		}

	p_ut_hlav->p_first_ut=zrus_prvek_utok(p_ut_hlav->p_first_ut);
	}

	free(p_ut_hlav); /* zruseni hlavicky udkazujici na vsechny seznamy */

	fclose(f);
	return(1);

}
/* --END uklid_heap_pamet--------------------------------------------------------*/



/* >>>>>>>>>>>>>>>>>>> nasleduji VYHLEDAVACI fce >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

/* >>>>>>>>>> VYPIS "-typy" >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

/*
 * ---------GLOBAL cast dole--------------vypis_typy_utoku-------------------------
 */

/* -----------------------projed_utoky--------------------------------------------
 * Prohleda a vypise seznam typu utoku, zacne na vstupnim ukazateli p_utok
 * Vraci: 1 .. pokud vse ok
 */
int projed_utoky(UTOKY *p_utok, FILE *f)
{
	int i=1; /* pocitadlo ruznych utoku, pro vypis utoku na obraz */

    while (p_utok != NULL ) {
  	fprintf(f,"%d: %s  (%d) \n", i, p_utok->nazev_utok, p_utok->pocet_utoku);
    projed_varianty(p_utok->p_first_var, f); /* zanori se do varianty k prislusnemu utoku */
    p_utok=p_utok->p_next_utok;
	i++;
	}

	return(1);
}
/* --END projed_utoky----------------------------------------------------------*/

/* -----------------------projed_varianty----------------------------------------
 * Prohleda a vypise seznam variant utoku, zacne na vstupnim ukazateli p_var
 * Vraci: 1 .. pokud vse ok
 */
int projed_varianty(VARIANTY *p_var, FILE *f)
{
	int i=97;  /* pocitadlo ruznych variant, zacina na malem pismene ordinalni cislo 97 */

	while (p_var != NULL ) {
	   fprintf(f,"   %c:   %s \n", i, p_var->nazev_var);
	   p_var=p_var->p_next_var;
	   i++;
	}
	
    return(1);
}
/* --END projed_varianty--------------------------------------------------------*/


/* >>>>>>>>>>>END vypis "-typy" >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/


/* >>>>>>>>>> VYPIS "-podsite UTOK" >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

/* -----------GLOBAL cast dole ----dyna_vypis_podsite---------------------------*/

 /* -----------------------vyhledej_utok-----------------------------------------
 * Vyhleda utok s_hled_ut[] v seznamu utoku .. p_utok
 * vyuziva GLOBalni promennou modulu styb-dyna.c --> p_ut_hlav
 * Vraci: 1 .. pokud vse ok
 */
int vyhledej_utok(FILE *f, char s_hled_ut[], UTOKY **p_utok)
{
	

	if ( (p_ut_hlav) == NULL ) 
		zadne_utoky(f); /* modul styb-vyst.c */
	else {
		(*p_utok)=p_ut_hlav->p_first_ut;
		while ( ( (*p_utok) != NULL ) && (( strcmp((*p_utok)->nazev_utok, s_hled_ut) ) != 0 ) ) {
			(*p_utok)=(*p_utok)->p_next_utok;
		}
	}

	return(1);
		
}
/* --END vyhledej_utok--------------------------------------------------------*/

/* -----------------------vypis__typy_podsite-------------------------------------
 * Vypise zahlavi vypisu typu utoku + vypise seznam ip adres, provede jejich formatovani
 * + vypis cetnosti hvezdami
 * vyuziva GLOBalni promennou modulu styb-dyna.c --> p_ut_hlav
 * Vraci: 1 .. pokud vse ok
 */
int vypis_typy_podsite(FILE *f, UTOKY *p_utok)
{
	IP_ADRESY *p_ip;

	char s_dat_poc[MAX_RADKA]; /* datum prvniho utoku */
	char s_dat_kon[MAX_RADKA]; /* datum posledniho utoku */
	float float_pocet_hvezd;   /* prevod real->integer, zaokrouhleni vypisu hvezd */
	int int_pocet_hvezd;	   /* prevod, viz vyse */

	/* vypis zahlavi vypisu typu utoku */
	preved_dat_cislo_retezec(p_ut_hlav->pocatek_datum, s_dat_poc);
	preved_dat_cislo_retezec(p_ut_hlav->konec_datum, s_dat_kon);
	fprintf(f,"---------------------------------------------------------------------\n");
	fprintf(f,"Data o %s od %s do %s \n\n", p_utok->nazev_utok, s_dat_poc, s_dat_kon);

	p_ip=p_utok->p_first_ip;
	while ( p_ip != NULL ) {
		/* vypocet relativni cetnosti */
		float_pocet_hvezd = ( ((float) p_ip->pocet_ip) / ( (float)p_utok->pocet_utoku) )*50;
		int_pocet_hvezd=(int) float_pocet_hvezd;
		fprintf(f,"%s", p_ip->nazev_ip);
		vypis_znak(f, ' ', (11-strlen(p_ip->nazev_ip)) ); /* 11 podsit --> dorovnani mezerami */
		fprintf(f,"-> %d\t", p_ip->pocet_ip);
		vypis_znak(f, '*', int_pocet_hvezd);
		fprintf(f,"\n");
		p_ip=p_ip->p_next_ip;
	}
	fprintf(f,"---------------------------------------------------------------------\n");
	
	#ifdef LADENI
	  getchar();
	#endif

	return(1);
}
/* --END vypis_typy_podsite---------------------------------------------------*/


  /* -----------------------vypis_znak--------------------------------------------
 * Vypise relativni cetnost .. pocet znaku .. c
 * Vraci: 1 .. pokud vse ok
 */
int vypis_znak(FILE *f, char c, int pocet)
{
	int i;

	for(i=0; i<pocet; i++) 
		fprintf(f,"%c",c);

	return(1);
}
/* --END vypis_znak-----------------------------------------------------------*/

/* >>>>>>>>>>>END vypis "-podsite UTOK" >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/


/* >>>>>>>>>> VYPIS "-varianty retezec" >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

/* ---GLOBAL casta dole--------------dyna_vypis_datumy------------------------*/

/* -----------------------cti_var_cislo-----------------------------------------
 * Cte retezec s_var, dokud je v nem cislo
 * Vraci: POZICI v retezci za cislem
 *		  0 .. pokud v retezci pouze cislo (cekal jsem jeste pismeno )
 *		  + odkazem zkonvertovane nactene cislo
 */
int cti_var_cislo(char s_var[], int pozice, int *cislo)
{
	unsigned int i=pozice;
	int j=0;
	char s_cislo[MAX_RADKA];


	while ( (isdigit(s_var[i]) != 0 ) && (i<strlen(s_var)) ) {
		(s_cislo[j])=s_var[i];
		i++;j++;
	}
	(s_cislo[j])='\0';

	if ( i<strlen(s_var) ) {
		sscanf(s_cislo, "%d", cislo);
		return(i); 
	}
	else
		return(0); /* v zadanem retezci bylo pouze cislo, nebo vubec nic */
}
/* --END cti_var_cislo---------------------------------------------------*/

/* -----------------------cti_var_pismeno----------------------------------
 * Cte retezec s_var, dokud je v nem pismeno
 * Vraci: POZICI v retezci za pismenem
 *		  0 .. pokud v retezci vice pismen, nebo cislo ( cekal jsem jedno pismeno, napr: a )
 *		  + odkazem zkonvertovane nactene pismeno
 */
int cti_var_pism(char s_var[], int pozice, char *pismeno)
{
	unsigned int i=pozice;
	int j=0;
	char s_pismeno[MAX_RADKA];


	while ( (isalpha(s_var[i]) != 0 ) && (i<strlen(s_var)) ) {
		s_pismeno[j]=s_var[i];
		i++;j++;
	}

	if (j>1) /* nebylo zadano pouze jedno pismeno */
		return(0);

	(s_pismeno[j])='\0';
	strcpy(pismeno, s_pismeno);


	if ( ( i<strlen(s_var) ) && (s_var[i] == ',') ) /* hledam vice utoku oddelenych
													 * carkami */
	  return(i+1); /* preskoceni ',' */
	else 
		if ( (i) == strlen(s_var) ) /* jsem na konci retezce */
			return(i);
		else 
			return(0); /* nekde v retezci chyba */

}
/* --END cti_var_pism---------------------------------------------------*/

/* -----------------------zkonvertuj_pismeno------------------------------
 * Provede transformaci pismene na cislo, Pr: a ~ 1, c ~ 3
 */
int zkonvertuj_pismeno_na_cislo(char pismeno[])
{
	return(pismeno[0]-'a');
}
/* --END zkonvertuj_pismeno_na_cislo------------------------------------*/

/* -----------------------vyhledej_utok_var-------------------------------
 * Vyhleda utok na zaklade jeho pozice v seznamu utoku
 * pozice udana cislem cislo_ut --> projizdim sekvencne utoky a citam cislo aktual_utok
 * vyuziva GLOBalni promennou modulu styb-dyna.c --> p_ut_hlav
 * Vraci: 1 .. pokud utok v seznamu
 *		  0 .. dojel jsem na konec --> NULL, nebo senam prazdny
 */
int vyhledej_utok_var(int *cislo_ut, UTOKY **p_utok)
{
	
	int aktual_utok=1; /* pozice v seznamu utoku */
	
	if ( (p_ut_hlav) == NULL ) { /* seznam utoku je prazdny */
		return(0);
	}

	(*p_utok)=p_ut_hlav->p_first_ut;
	while ( (aktual_utok<(*cislo_ut)) && ( (*p_utok) != NULL) ) {
		(*p_utok)=(*p_utok)->p_next_utok;
		aktual_utok++;
	}

	if ((*p_utok) == NULL)
		return(0);
	else 
		return(1);
}
/* --END vyhledej_utok_var---------------------------------------------*/

/* -----------------------vyhledej_variantu------------------------------
 * Vyhleda variantu na zaklade jeho pozice v seznamu utoku
 * pozice udana cislem cislo_var --> projizdim sekvencne varianty a citam aktual_var
 * Vraci: 1 .. pokud utok v seznamu
 *		  0 .. dojel jsem na konec --> NULL, nebo senam prazdny
 */
int vyhledej_variantu(int cislo_var, UTOKY *p_utok, VARIANTY **p_var)
{
	int aktual_var=0; /* pozice v seznamu variant */

	(*p_var)=p_utok->p_first_var;
	while ( (aktual_var<cislo_var) && ((*p_var) != NULL) ) {
		(*p_var)=(*p_var)->p_next_var;
		aktual_var++;
	}
	
	if ((*p_var) == NULL)
		return(0);
	else 
		return(1);
}
/* --END vyhledej_variantu---------------------------------------------*/

/* -----------------------vypis_datumy------------------------------
 * Vypise seznam datumu ( datumy prevede z ciselneho formatu na retezcovy )
 * na zaklade vyhledaneho schematu: utok -> varianta -> varianta.p_first_dat
 * Vraci: 1 .. pokud vse ok
 */
int vypis_datumy(FILE *f, VARIANTY *p_var, int *cislo_ut, char pismeno_var[])
{
	DATUMY *p_dat;
	char s_dat[MAX_RADKA];

	if (p_var == NULL)
		return(0);
	
	fprintf(f,"%d%s: %s\n", (*cislo_ut), pismeno_var, p_var->nazev_var);
    fprintf(f,"  ");
	p_dat=p_var->p_first_dat;
	while (p_dat != NULL ) {
		preved_dat_cislo_retezec(p_dat->datum, s_dat);
		fprintf(f,"%s ", s_dat);
		p_dat=p_dat->p_next_dat;

	}
	#ifdef LADENI
	 getchar();
	#endif

	return(1);

}
/* --END vypis_datumy---------------------------------------------*/

/* >>>>>>>>>>>END vypis "-varianty retezec" >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/


/* ********* definice GLOBALnich funkci */

/* -------------------pridej_zaznam------------------------------------------
 * Prida polozku struktury UTOK->nazev_utok do seznamu pomoci fce kam_utok, ktera
 * vyhleda pozici pro zarazovanou polozku v seznamu UTOKY -> zaradi ji na prislusnou pozici
 *
 * Prida polozku struktury UTOK->nazev_var do seznamu pomoci fce kam_var, pricemz vyuzije
 * navratove hodnoty fce kam_utok, aby vedela pod jaky utok zaradit variantu
 *
 * Prida polozku struktury UTOK->datum do seznamu pomoci fce kam_dat, pricemz vyuzije
 * navratove hodnoty fci kam_utok a kam_varianta, aby vedela pod jakou variantu zaradit datum
 *
 * Prida polozku struktury UTOK->ip do seznamu pomoci fce kam_ip, pricemz vyuzije
 * navratove hodnoty fce kam_utok aby vedela pod jaky utok zaradit ip_adresu
 * Vraci: 1 .. pri uspesnem zarazeni vsech polozek do seznamu
 */
int pridej_zaznam(UTOK *p_zarazovany)
{
	UTOKY *aktual_utok;   /* pod jaky prvek seznamu UTOKY budou zarazovany varianta, datum, ip */
	VARIANTY *aktual_var; /* pod jaky prvek seznamu VARIANTY bude zarazovan datum */
		
		
		/* seznam utoku obsahuje alespon jeden prvek --> zarazuji prvky
		 * tak, aby byli v seznamu pro sekvencnim cteni podle abecedy */
		aktual_utok=kam_utok(p_zarazovany);
		aktual_var=kam_varianta(p_zarazovany->nazev_var,aktual_utok);
		kam_datum(p_zarazovany->datum, aktual_var);
		kam_ip(p_zarazovany->ip, aktual_utok);
		serad_ip_adresy(aktual_utok);
	

	return(1);
}
/* --END pridej_zaznam--------------------------------------------------------*/

/* -----------------------vypis_typy_utoku--------------------------------------
 * Projede typy utoku a vypise k nim prislusejici varianty
 * vyuziva GLOBalni promennou modulu styb-dyna.c --> p_ut_hlav
 * Vraci: 1 .. pokud vse ok
 */
int dyna_vypis_typy(FILE *f)
{
	char s_dat_poc[MAX_RADKA]; /* datum prvniho utoku */
	char s_dat_kon[MAX_RADKA]; /* datum posledniho utoku */


	if ( (p_ut_hlav) == NULL ) 
		zadne_utoky(f);
	else { /* existuje alespon jeden zaznam */
		 /* prevede ciselny format datumu na retezcovy */
		preved_dat_cislo_retezec(p_ut_hlav->pocatek_datum, s_dat_poc);
		preved_dat_cislo_retezec(p_ut_hlav->konec_datum, s_dat_kon);
		fprintf(f,"---------------------------------------------------------------------\n");
		fprintf(f,"Data od %s do %s \n", s_dat_poc, s_dat_kon);
		projed_utoky(p_ut_hlav->p_first_ut, f);
		fprintf(f,"---------------------------------------------------------------------\n");
	}	
	#ifdef LADENI
		getchar();
	#endif

	return(1);
	
}
/* --END dyna_vypis_typy--------------------------------------------------------*/

/* -----------------------dyna_vypis_podsite--------------------------------------
 * Vyhleda utok s_hled_ut[] v seznamu utoku a vypise k nemu prislusejici seznam ip adres
 * vyuziva GLOBalni promennou modulu styb-dyna.c --> p_ut_hlav
 * Vraci: 1 .. pokud vse ok
 */
int dyna_vypis_podsite(FILE *f, char s_hled_ut[])
{
	UTOKY *p_utok;

	vyhledej_utok(f, s_hled_ut, &p_utok);
	if ( p_utok == NULL )
		 zadne_utoky(f); /* modul styb-vyst.c */
	else
		vypis_typy_podsite(stdout, p_utok);
    #ifdef LADENI
	  getchar();
	#endif

	return(1);
}
/* --END dyna_vypis_podsite----------------------------------------------*/

/* -----------------------dyna_vypis_datumy--------------------------------
 * Analyzuje retezec s_hled_var (Pr: 1b,2a)
 * na zaklade cisla z retezce vyhleda pozici v seznamu utoku
 * na zaklade pismene z retezce vyhleda pozici v seznamu variant
 * vypise vsechny datumy k prislusne variante
 * Vraci: 1 .. pokud vse ok
 *		  0 .. pokud byla v zadanem retezci s_hled_var chyba predepsaneho formatu
 */
int	dyna_vypis_datumy(FILE *f, char s_hled_var[])
{
	unsigned int pozice=0;  /* pozice v s_hled_var[] */
	int *cislo_ut;			/* prevod cisla z s_hled_var */
	int cislo_var;			/* transformace pismene z s_hled_var na cislo */
	char *pismeno_var;		/* vypreparovani pismene za cislem z s_hled_var */
	UTOKY *p_utok;
	VARIANTY *p_var;
	char s_dat_poc[MAX_RADKA]; /* prevod pocatecniho datumu na retezec */
	char s_dat_kon[MAX_RADKA]; /* prevod koncoveho datumu na retezec */
	char carka[]=",";

	if ( (cislo_ut=(int *) malloc(sizeof(int)) ) == NULL )
		chyba_alokace_heap();

	if ( (pismeno_var=(char *) malloc(sizeof(char)) ) == NULL )
		chyba_alokace_heap();
	
	if ( (pozice=cti_var_cislo(s_hled_var, pozice, cislo_ut)) == 0 ) { /* chyba v cisle, nebo zadano pouze cislo */
			fprintf(stderr, "Snort: Chyba pri volbe varianty \nPriklad vyberu varianty: Pr: 1b,12a \n");
			return(0);
	}
	if ( (pozice=cti_var_pism(s_hled_var, pozice, pismeno_var)) == 0) { /* chyba v pismene */
			fprintf(stderr, "Snort: Chyba pri volbe varianty \nPriklad vyberu varianty: Pr: 1b,12a \n");
			return(0);
	}
	
	/* prvni vyber utoku a variantu neni mimo meze */
	preved_dat_cislo_retezec(p_ut_hlav->pocatek_datum, s_dat_poc);
	preved_dat_cislo_retezec(p_ut_hlav->konec_datum, s_dat_kon);
	fprintf(f,"---------------------------------------------------------------------\n");
	/* v zadani se vyskytuje "," --> hledanych variant je vice > 1 --> variantach, jinak variante */
	if ( (strchr(s_hled_var, carka)) == NULL )
		fprintf(f,"Data o variante %s od %s do %s\n\n", s_hled_var, s_dat_poc, s_dat_kon);
	else
		fprintf(f,"Data o variantach %s od %s do %s\n\n", s_hled_var, s_dat_poc, s_dat_kon);
	
	while ( pozice<=strlen(s_hled_var) ) {

		cislo_var=zkonvertuj_pismeno_na_cislo(pismeno_var);
	
		vyhledej_utok_var(cislo_ut, &p_utok);
		if (p_utok != 0) {
			vyhledej_variantu(cislo_var, p_utok, &p_var);
			if (p_var != 0) {
				vypis_datumy(f, p_var, cislo_ut, pismeno_var);
			}
			else 
				fprintf(f,"%d%s:  [Neexistujici pismeno ve vypisu typu utoku a variant]\n", (*cislo_ut), pismeno_var);
		}
		else 
			fprintf(f,"%d%s:  [Neexistujici pismeno ve vypisu typu utoku a variant]\n", (*cislo_ut), pismeno_var);

	
		if ( (pozice<=strlen(s_hled_var)) ) { /* pokud nebyli pozadovany zadne dalsi vypisy datumu k dalsi variante */

			if ( (pozice=cti_var_cislo(s_hled_var, pozice, cislo_ut)) == 0 ) { /* chyba v cisle, nebo zadano pouze cislo */
				fprintf(stderr, "Snort: Chyba pri volbe varianty \nPriklad vyberu varianty: Pr: 1b,12a \n");
				return(0);
			}

			if ( (pozice=cti_var_pism(s_hled_var, pozice, pismeno_var)) == 0) { /* chyba v pismene */
				fprintf(stderr, "Snort: Chyba pri volbe varianty \nPriklad vyberu varianty: Pr: 1b,12a \n");
				return(0);
			}
		}
	
	} /* while */

	return(1);

}
/* --END dyna_vypis_datumy---------------------------------------------*/

