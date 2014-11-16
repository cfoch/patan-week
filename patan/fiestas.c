#include <ctype.h>
#include <stdio.h>
#include "fiestas.h"

/* TODO Liberar estructura FiestaValue de alumno

void
patan_fiesta_value_free (PatanFiestas * alumnos)

*/


/* TODO Liberar tabla hash de fiestas, reusando patan_fiestas_value_free

void
patan_fiestas_free (PatanFiestas * alumnos)

*/


FiestaValue *
fiesta_value_new (const char * nombre, int precio, QDate * fecha)
{
  FiestaValue* fiesta_value;

  fiesta_value = malloc (sizeof (FiestaValue));
  fiesta_value->nombre = nombre;
  fiesta_value->precio = precio;
  fiesta_value->fecha.day = fecha->day;
  fiesta_value->fecha.month = fecha->month;
  fiesta_value->fecha.year = fecha->year;
  fiesta_value->alumnos = NULL;
  fiesta_value->registro_interes = q_queue_new ();
  return fiesta_value;
}

void
patan_fiesta_value_print (qpointer data, qpointer user_data)
{
  QHashKeyValue *fiesta;
  FiestaValue *val;

  fiesta = Q_HASH_KEY_VALUE (data);
  val = FIESTA_VALUE (fiesta->value);

  printf (PATAN_CONSOLE_STR_FORMAT PATAN_CONSOLE_STR_FORMAT \
      PATAN_CONSOLE_STR_FORMAT PATAN_CONSOLE_INT_FORMAT "\n",
      fiesta->key, val->nombre, q_date_to_string (&(val->fecha)), val->precio);
}

/* Compare Functions */
static int
patan_fiestas_cmp_by_id (QHashKeyValue * kv1,
    QHashKeyValue * kv2)
{
  return strcmp (kv1->key, kv2->key);
}

static int
patan_fiestas_cmp_by_nombre (QHashKeyValue * kv1,
    QHashKeyValue * kv2)
{
  FiestaValue *val1 = Q_HASH_KEY_VALUE (kv1->value);
  FiestaValue *val2 = Q_HASH_KEY_VALUE (kv2->value);

  return strcmp (val1->nombre, val2->nombre);
}

static int
patan_fiestas_cmp_by_fecha (QHashKeyValue * kv1,
    QHashKeyValue * kv2)
{
  QDate *date1, *date2;
  FiestaValue *val1 = FIESTA_VALUE (kv1->value);
  FiestaValue *val2 = FIESTA_VALUE (kv2->value);
  date1 = &(val1->fecha);
  date2 = &(val2->fecha);

  return qfunc_date_cmp (date1, date2);
}

void
patan_fiestas_print (QSList * fiestas_list, PatanSortBy sort_by)
{
  patan_print_header ("ID", PATAN_CONSOLE_STR, "NOMBRE", PATAN_CONSOLE_STR,
      "FECHA", PATAN_CONSOLE_STR, "PRECIO (S/.)", PATAN_CONSOLE_INT, NULL);

  switch (sort_by) {
    case PATAN_SORT_BY_ID:
      fiestas_list = q_slist_sort (fiestas_list, patan_fiestas_cmp_by_id);
      break;
    case PATAN_SORT_BY_NOMBRE:
      fiestas_list = q_slist_sort (fiestas_list, patan_fiestas_cmp_by_nombre);
      break;
    case PATAN_SORT_BY_FECHA:
      fiestas_list = q_slist_sort (fiestas_list, patan_fiestas_cmp_by_fecha);
      break;
    default:
      break;
  }
  q_slist_foreach (fiestas_list, patan_fiesta_value_print, NULL);
}

void
patan_fiesta_print_registro_interes (QHashKeyValue * fiesta_kv,
    PatanSortBy sort_by)
{
  QQueue *registro_interes;

  registro_interes = FIESTA_VALUE (fiesta_kv->value)->registro_interes;
  q_list_foreach (registro_interes->head, patan_alumno_value_print, NULL);
}

QHashTable *
patan_fiestas_new ()
{
  return q_hash_table_new (qfunc_hash_numeric_string,
      func_hash_key_strings_equal);
}

void
patan_fiestas_insert (QHashTable * hash_table, const char * id,
    const char * nombre, int precio, QDate * date)
{
  q_hash_table_insert (hash_table, strdup (id),
      fiesta_value_new (strdup (nombre), precio, date));
}

int
patan_fiesta_eq_nombre (QHashKeyValue * fiesta_kv, char * nombre_fiesta)
{
  FiestaValue *fiesta_val = FIESTA_VALUE (fiesta_kv->value);
  return strcmp (fiesta_val->nombre, nombre_fiesta) == 0;
}

void
patan_fiesta_registrar_interes (QHashKeyValue * fiesta_kv,
    QHashKeyValue * alumno_kv)
{
  AlumnoValue *alumno_val = ALUMNO_VALUE (alumno_kv->value);
  FiestaValue *fiesta_val = FIESTA_VALUE (fiesta_kv->value);

  q_queue_push_head (fiesta_val->registro_interes, alumno_kv);
}

/* Parsear archivo de fiestas */


QHashTable *
patan_parse_fiestas (const char * filename)
{
  FILE *f;
  char c;
  QHashTable *hash_table;


  f = fopen (filename, "r");

  if (!f)
    return NULL;

  hash_table = q_hash_table_new (qfunc_hash_numeric_string,
      func_hash_key_strings_equal);

  do {
    int i, j;
    char something[1000];
    i = 0;
    int precio;
    char *id_fiesta, *nombre;
    QDate date;

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
    id_fiesta = strdup (something);

    i = 0;
    while ((c = fgetc (f)) && (!isdigit (c))) {
      something[i] = c;
      i++;
    }
    something[i - 1] = '\0';
    nombre = strdup (something);

    i = 0;
    something[i] = c;
    i++;
    while ((c = fgetc (f)) && isdigit (c) && (c != ' ')) {
      something[i] = c;
      i++;
    }
    something[i] = '\0';
    precio = atoi (something);

    i = 0;
    while ((c = fgetc (f)) && isdigit (c)) {
      something[i] = c;
      i++;
    }
    something[i] = '\0';
    date.day = atoi (something);

    i = 0;
    while ((c = fgetc (f)) && isdigit (c)) {
      something[i] = c;
      i++;
    }
    something[i] = '\0';
    date.month = atoi (something);

    i = 0;
    while ((c = fgetc (f)) && isdigit (c)) {
      something[i] = c;
      i++;
    }
    something[i] = '\0';
    date.year = atoi (something);


    q_hash_table_insert (hash_table, strdup (id_fiesta),
        fiesta_value_new (strdup (nombre), precio, &date));

  } while (c != EOF);

  fclose (f);

  return hash_table;
}

