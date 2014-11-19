#include <ctype.h>
#include <stdio.h>
#include "alumnos.h"

void
patan_alumno_value_free (QHashKeyValue * alumno_kv, qpointer user_data)
{
  AlumnoValue *alumno_val;
  alumno_val = ALUMNO_VALUE (alumno_kv->value);
  free (alumno_val);
  free (alumno_kv);
}

void
patan_alumnos_free (PatanAlumnos * alumnos)
{
  int i;
  Q_DEBUG ("Liberando PatanAlumnos.", NULL);
  for (i=0; i < alumnos->size; i++)
  {
    q_slist_foreach (alumnos->table[i], patan_alumno_value_free, NULL);
    q_slist_free (alumnos->table[i]);    
  }
  free (alumnos->table);
  free (alumnos);
  Q_DEBUG ("Liberando PatanAlumnos.", NULL);
}

AlumnoValue *
alumno_value_new (const char * nombre, QDate *fecha_nacimiento,
    QHashKeyValue *especialidad)
{
  AlumnoValue *alumno_value;

  alumno_value = malloc (sizeof (AlumnoValue));
  alumno_value->nombre = strdup (nombre);
  alumno_value->fecha_nacimiento.day = fecha_nacimiento->day;
  alumno_value->fecha_nacimiento.month = fecha_nacimiento->month;
  alumno_value->fecha_nacimiento.year = fecha_nacimiento->year;
  alumno_value->especialidad = especialidad;
  alumno_value->fiestas = NULL;
  return alumno_value;
}


void
patan_alumno_value_print (qpointer data, qpointer user_data)
{
  QHashKeyValue *alumno;
  AlumnoValue *val;

  alumno = Q_HASH_KEY_VALUE (data);
  val = ALUMNO_VALUE (alumno->value);

  printf (PATAN_CONSOLE_STR_FORMAT PATAN_CONSOLE_STR_FORMAT \
      PATAN_CONSOLE_STR_FORMAT PATAN_CONSOLE_STR_FORMAT "\n" ,
      alumno->key, val->nombre,
      q_date_to_string (&(val->fecha_nacimiento)),
      val->especialidad ? val->especialidad->value : NULL);
}

/* Equal Funcions */

int
patan_alumno_eq_nombre (QHashKeyValue * alumno_kv, char * nombre)
{
  AlumnoValue *alumno_val = ALUMNO_VALUE (alumno_kv->value);
  return strcmp (alumno_val->nombre, nombre) == 0;
}

/* Compare Functions */
static int
patan_alumnos_cmp_by_id (QHashKeyValue * kv1,
    QHashKeyValue * kv2)
{
  return strcmp (kv1->key, kv2->key);
}

static int
patan_alumnos_cmp_by_nombre (QHashKeyValue * kv1,
    QHashKeyValue * kv2)
{
  AlumnoValue *val1 = ALUMNO_VALUE (kv1->value);
  AlumnoValue *val2 = ALUMNO_VALUE (kv2->value);

  return strcmp (val1->nombre, val2->nombre);
}

static int
patan_alumnos_cmp_by_fecha (QHashKeyValue * kv1,
    QHashKeyValue * kv2)
{
  QDate *date1, *date2;
  AlumnoValue *val1 = ALUMNO_VALUE (kv1->value);
  AlumnoValue *val2 = ALUMNO_VALUE (kv2->value);
  date1 = &(val1->fecha_nacimiento);
  date2 = &(val2->fecha_nacimiento);  
  return qfunc_date_cmp (date1, date2);
}

static int
patan_alumnos_cmp_by_especial (QHashKeyValue * kv1,
    QHashKeyValue * kv2)
{
  AlumnoValue *val1 = Q_HASH_KEY_VALUE (kv1->value);
  AlumnoValue *val2 = Q_HASH_KEY_VALUE (kv2->value);

  return strcmp (val1->especialidad->value, val2->especialidad->value);
}

void
patan_alumnos_print (QSList * alumnos_list, PatanSortBy sort_by)
{
  patan_print_header ("ID", PATAN_CONSOLE_STR, "NOMBRE", PATAN_CONSOLE_STR,
      "FECHA NAC", PATAN_CONSOLE_STR, "ESPECIALIDAD", PATAN_CONSOLE_STR, NULL);

  switch (sort_by) {
    case PATAN_SORT_BY_ID:
      alumnos_list = q_slist_sort (alumnos_list, patan_alumnos_cmp_by_id);
      break;
    case PATAN_SORT_BY_NOMBRE:
      alumnos_list = q_slist_sort (alumnos_list, patan_alumnos_cmp_by_nombre);
      break;
    case PATAN_SORT_BY_FECHA:
      alumnos_list = q_slist_sort (alumnos_list, patan_alumnos_cmp_by_fecha);
      break;
    case PATAN_SORT_BY_ESPECIALIDAD:
      alumnos_list = q_slist_sort (alumnos_list, patan_alumnos_cmp_by_especial);
      break;
    default:
      break;
  }
  q_slist_foreach (alumnos_list, patan_alumno_value_print, NULL);
}


QHashTable *
patan_alumnos_new ()
{
  return q_hash_table_new (qfunc_hash_numeric_string,
      func_hash_key_strings_equal);
}

void
patan_alumnos_insert (QHashTable * hash_table, const char * id,
    const char * nombre, QDate *date, QHashKeyValue *especialidad_kv)
{

  q_hash_table_insert (hash_table, strdup (id),
      alumno_value_new (strdup (nombre), date, especialidad_kv));
}


/* Parsear archivo de alumnos */

QHashTable *
patan_parse_alumnos (const char * filename, QHashTable *especialidades) {
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
    char *id_alumno, *nombre, *id_especialidad;
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
    id_alumno = strdup (something);
    Q_DEBUG ("ID Alumno: %s", something);

    i = 0;
    while ((c = fgetc (f)) && (isalpha (c) || (c == ' '))) {
      something[i] = c;
      i++;
    }
    something[i - 1] = '\0';
    nombre = strdup (something);
    Q_DEBUG ("Nombre Alumno: %s", something);

    i = 0;
    something[i] = c;
    i++;
    while ((c = fgetc (f)) && isdigit (c) && (c != ' ')) {
      something[i] = c;
      i++;
    }
    something[i] = '\0';
    id_especialidad = strdup (something);
    Q_DEBUG ("Especialidad de alumno: %s", something);

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

    /* Kill me: this is fucking ugly. Parse correctly in Windows files */
    if (c == 13)
      c = fgetc (f);

    Q_DEBUG ("Alumno Fecha Nacimiento: %s", q_date_to_string (&date));
    Q_DEBUG ("Salto de linea: %d", c);


    q_hash_table_insert (hash_table, strdup (id_alumno),
        alumno_value_new (strdup (nombre), &date,
            q_hash_table_get_key_value_by_key (especialidades,
                id_especialidad)));

  } while (c != EOF);

  fclose (f);

  return hash_table;
}

static void
_patan_alumno_serialize (QHashKeyValue * alumno_kv, FILE * f_alumnos)
{
  char buf[1000];
  AlumnoValue *alumno_val;

  alumno_val = ALUMNO_VALUE (alumno_kv->value);

  sprintf (buf, "%s %s %s %s", alumno_kv->key, alumno_val->nombre,
      alumno_val->especialidad->key,
      q_date_to_string (&(alumno_val->fecha_nacimiento)));
  fprintf (f_alumnos, "%s\n",  buf);
}

void
patan_alumnos_serialize (const char * out_filename,
    QHashTable * alumnos)
{
  FILE *f;
  f = fopen (out_filename, "w");

  if (!f)
    return NULL;

  q_hash_table_foreach (alumnos, _patan_alumno_serialize, f);

  fclose (f);
}
