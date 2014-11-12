#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "patan.h"



/*******************************************************/


/**********************************************************/



/*****************************************************************************/

/* TODO:
 * This is a generic way to parse. It is missing to output what has been parsed.
 * We are keeping it simple below.
*/

/*
QSList*
patan_input_format_parse (const char * filename, PatanFilenameFormat patan_fmt)
{
  FILE *f;
  char c;


  f = fopen (filename, "r");

  if (!f)
    return NULL;


  do {
    int i, j;
    char something[1000];
    i = 0;
    while ((c = fgetc (f)) && isdigit (c) && (c != ' ')) {
      // TODO: This way to stop reading is really ugly 
      if (c == EOF)
        break;
      something[i] = c;
      i++;
    }
    // TODO: This way to stop reading is really ugly
    if (c == EOF)
      break;
    something[i] = '\0';
    printf ("id: %s\n", something);

    i = 0;
    c = fgetc (f);
    something[i] = c;
    i++;
    if (isalpha (c)) { 
      while ((c = fgetc (f)) && (isalpha (c) || (c == ' '))) {
        something[i] = c;
        i++;
      }
    } else {
      while ((c = fgetc (f)) && (isdigit (c)) && (c != ' ')) {
        something[i] = c;
        i++;
      }
    }
    something[i - 1] = '\0';

    printf ("%s\n", something);

    if ((c == ' ') || (isdigit (c))) {
      i = 0;
      if (isdigit (c)) {
        something[i] = c;
        i++;
      }
      while ((c = fgetc (f)) && isdigit (c) && (c != ' ')) {
        something[i] = c;
        i++;
      }
      something[i] = '\0';
      printf ("MOMO: %s\n", something);

      i = 0;
      while ((c = fgetc (f)) && isdigit (c)) {
        something[i] = c;
        i++;
      }
      something[i] = '\0';
      printf ("day: %s\n", something);

      i = 0;
      while ((c = fgetc (f)) && isdigit (c)) {
        something[i] = c;
        i++;
      }
      something[i] = '\0';
      printf ("month: %s\n", something);

      i = 0;
      while ((c = fgetc (f)) && isdigit (c)) {
        something[i] = c;
        i++;
      }
      something[i] = '\0';
      printf ("year: %s\n", something);

    }
    printf ("\n");
  } while (c != EOF);

  fclose (f);
  
}

*/


void
patan_parse_asistencia (const char * filename, QHashTable *fiestas,
    QHashTable *alumnos)
{
  FILE *f;
  char c;

  f = fopen (filename, "r");

  if (!f)
    return;


  do {
    int i, j;
    char something[1000];
    i = 0;
    char *id_fiesta, *id_alumno;
    QHashKeyValue *fiesta_kv, *alumno_kv;
    FiestaValue *fiesta_val;
    AlumnoValue *alumno_val;

    while ((c = fgetc (f)) && isdigit (c) && (c != ' ')) {
      /* TODO: This way to stop reading is really ugly */
      if (c == EOF)
        break;
      something[i] = c;
      i++;
    }
    /* TODO: This way to stop reading is really ugly */
    if (c == EOF)
      break;
    something[i] = '\0';
    id_fiesta = something;

    fiesta_kv = q_hash_table_get_key_value_by_key (fiestas, id_fiesta);
    fiesta_val = FIESTA_VALUE (fiesta_kv->value);

    i = 0;
    while ((c = fgetc (f)) && (isdigit (c)) && (c != ' ')) {
      something[i] = c;
      i++;
    }
    something[i] = '\0';
    id_alumno = something;

    alumno_kv = q_hash_table_get_key_value_by_key (alumnos, id_alumno);
    alumno_val = ALUMNO_VALUE (alumno_kv->value);

    fiesta_val->alumnos = q_slist_prepend (fiesta_val->alumnos, alumno_kv);
    alumno_val->fiestas = q_slist_prepend (alumno_val->fiestas, fiesta_kv);

  } while (c != EOF);

  fclose (f);
}


/***************************************************************************/




/* ALUMNOS */



/* FIESTAS (Oscar) */



/* Utilidades */
void
patan_registrar_asistencia (QHashKeyValue * alumno_kv,
    QHashKeyValue * fiesta_kv)
{
    FiestaValue *fiesta_val;
    AlumnoValue *alumno_val;
    alumno_val = ALUMNO_VALUE (alumno_kv->value);
    fiesta_val = FIESTA_VALUE (fiesta_kv->value);

    fiesta_val->alumnos = q_slist_prepend (fiesta_val->alumnos, alumno_kv);
    alumno_val->fiestas = q_slist_prepend (alumno_val->fiestas, fiesta_kv);
}

/*********************************************/



