#include <ctype.h>
#include <stdio.h>
#include "fiestas.h"

void
patan_fiesta_value_free (QHashKeyValue * fiesta_kv, qpointer user_data)
{
  FiestaValue *fiesta_val;

  fiesta_val = FIESTA_VALUE (fiesta_kv->value);

  Q_DEBUG ("Liberando FiestaValue.", NULL);
  Q_DEBUG ("Liberando FiestaValue. Liberar lista.", NULL);
  q_slist_free (fiesta_val->asistentes);
  Q_DEBUG ("Liberando FiestaValue. Liberar cola.", NULL);
  q_queue_free (fiesta_val->registro_interes);
  Q_DEBUG ("Liberando FiestaValue. Liberar estructura.", NULL);
  free (fiesta_val);
  free (fiesta_kv);
  Q_DEBUG ("Liberando FiestaValue.", NULL);
}

void
patan_fiestas_free (PatanFiestas * fiestas)
{
  int i;
  Q_DEBUG ("Liberando PatanFiestas.", NULL);
  for (i=0; i < fiestas->size ; i++) {
    q_slist_foreach (fiestas->table[i], patan_fiesta_value_free, NULL);
    q_slist_free (fiestas->table[i]);
  }
  free (fiestas->table); 
  free (fiestas);  
  Q_DEBUG ("Liberando PatanFiestas.", NULL);
}

FiestaValue *
fiesta_value_new (const char * nombre, double precio, QDate * fecha, int aforo)
{
  FiestaValue* fiesta_value;

  Q_DEBUG ("Creando nueva fiesta", NULL);
  fiesta_value = malloc (sizeof (FiestaValue));
  fiesta_value->nombre = nombre;
  fiesta_value->precio = precio;
  fiesta_value->fecha.day = fecha->day;
  fiesta_value->fecha.month = fecha->month;
  fiesta_value->fecha.year = fecha->year;
  Q_DEBUG ("Fiesta de fecha: %s", q_date_to_string (&(fiesta_value->fecha)));

  fiesta_value->asistentes = NULL;
  fiesta_value->registro_interes = q_queue_new ();
  fiesta_value->monto_recaudado = 0;

  fiesta_value->aforo = aforo;
  fiesta_value->cantidad_inscritos = 0;
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
patan_fiesta_avanzar_cola (QHashKeyValue * fiesta_kv)
{
  FiestaValue *fiesta_val;
  QQueue *registro_interes;
  QSList *asistentes;
  int i, n_asistentes;

  fiesta_val = FIESTA_VALUE (fiesta_kv->value);
  registro_interes = fiesta_val->registro_interes;
  asistentes = fiesta_val->asistentes;

  Q_DEBUG ("Cantidad de elementos en la cola de interes: %d", 
      registro_interes->length);
  Q_DEBUG ("Aforo: %d", fiesta_val->aforo);

  n_asistentes = q_min (registro_interes->length, fiesta_val->aforo);

  for (i = 0; i < n_asistentes; i++) {
    Q_DEBUG ("Se inserto %d elementos.", i + 1);
    QHashKeyValue *alumno_kv;
    alumno_kv = q_queue_pop_head (registro_interes);
    patan_registrar_asistencia (alumno_kv, fiesta_kv);
    asistentes = q_slist_prepend (asistentes, alumno_kv);
  }
  fiesta_val->asistentes = asistentes;
}

static void
_sumar_total_recaudado (QHashKeyValue * fiesta_kv, double * total)
{
  Q_DEBUG ("Total actual S/.%7.2f", *total); 
  Q_DEBUG ("Precio a sumar: S/.%7.2f", FIESTA_VALUE (fiesta_kv->value)->precio);
  (*total) += FIESTA_VALUE (fiesta_kv->value)->monto_recaudado;
  Q_DEBUG ("Total es ahora S/.%7.2f", *total); 
}

double
patan_fiestas_obtener_total (QHashTable * fiestas)
{
  int i;
  double total = 0;

  for (i = 0; i < fiestas->size; i++)
    q_slist_foreach (fiestas->table[i], _sumar_total_recaudado, &total);

  return total;
}


void
patan_fiesta_print_registro_interes (QHashKeyValue * fiesta_kv)
{
  QQueue *registro_interes;

  patan_print_header ("ID", PATAN_CONSOLE_STR, "NOMBRE", PATAN_CONSOLE_STR,
      "FECHA NAC", PATAN_CONSOLE_STR, "ESPECIALIDAD", PATAN_CONSOLE_STR, NULL);
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
    const char * nombre, double precio, QDate * date, int aforo)
{
  Q_DEBUG ("Insertando fiesta", NULL);
  q_hash_table_insert (hash_table, strdup (id),
      fiesta_value_new (strdup (nombre), precio, date, aforo));
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

  q_queue_push_tail (fiesta_val->registro_interes, alumno_kv);
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
    double precio;
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
    while ((c = fgetc (f)) && (c == '.' || isdigit (c)) && (c != ' ')) {
      something[i] = c;
      i++;
    }
    something[i] = '\0';
    precio = atof (something);

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
        fiesta_value_new (strdup (nombre), precio, &date, -1));

    /* Registramos un aforo -1 ya que los archivos no guardan aforo */

  } while (c != EOF);

  fclose (f);

  return hash_table;
}


static void
_patan_fiesta_serialize (QHashKeyValue * fiesta_kv, FILE * f_alumnos)
{
  char buf[1000];
  FiestaValue *fiesta_val;

  fiesta_val = FIESTA_VALUE (fiesta_kv->value);

  sprintf (buf, "%s %s %d %s", fiesta_kv->key, fiesta_val->nombre,
      fiesta_val->precio, q_date_to_string (&(fiesta_val->fecha)));
  fprintf (f_alumnos, "%s\n",  buf);
}

void
patan_fiestas_serialize (const char * out_filename,
    QHashTable * fiestas)
{
  FILE *f;
  f = fopen (out_filename, "w");

  if (!f)
    return NULL;

  q_hash_table_foreach (fiestas, _patan_fiesta_serialize, f);

  fclose (f);
}
