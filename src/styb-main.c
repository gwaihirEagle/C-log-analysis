/* STYB - Styblo Marek (zadani 1) SNORT */
/*
* PC-Snort verze 0.1
*
* Parsuje textovy soubor s vyberem zajimavych zaznamu, vypis statistik
*
* Marek Styblo 02.12.2003
*/

/* ********* systemove hlavickove soubory */

#include <stdio.h>


/* ********* vlastni hlavickove */

#include "styb-main.h"
#include "styb-io.h"

/* ********* deklarace IMPORTovanych objektu */

/* ********* definice GLOBALnich promennych */

/* ********* lokalni definice symbolickych konstant: #define */

/* ********* lokalni definice novych typu: typedef */

/* ********* definice LOCALnich promennych */

/* ********* uplne funkcni prototypy LOCAL funkci */

/* main() */
int main(int argc, char *argv[])
{
  argc=4;
  argv[1]="win.dat";
  argv[2]="-varianty";
  argv[3]="1a,1b,1c,1r,1a";


  test_io_par(argc,argv);
  #ifdef LADENI
    /*obnov_obraz();*/
  #endif
  return 0;
}

/* ********* definice LOCALnich funkci */

/* ********* definice GLOBALnich funkci */
