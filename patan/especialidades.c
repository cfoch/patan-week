#include <ctype.h>
#include <stdio.h>
#include "especialidades.h"


/* TODO Liberar tabla hash de especialidades

void
patan_especialidades_free (PatanEspecialidades * alumnos)

*/

QHashTable *
patan_especialidades_new ()
{
  QHashTable *hash_table = q_hash_table_new (qfunc_hash_numeric_string,
      func_hash_key_strings_equal);
  return PATAN_ESPECIALIDADES (hash_table);
}

void
patan_especialidades_insert (PatanEspecialidades * especialidades,
    const char * id, const char * especialidad)
{
  QHashTable *hash_table = Q_HASH_TABLE (especialidades);
  q_hash_table_insert (hash_table, strdup (id), strdup (especialidad));
}

int
patan_especialidades_cmp_by_especialidad (QHashKeyValue * kv1,
    QHashKeyValue * kv2)
{
  return strcmp (kv1->value, kv2->value);
}

static int
patan_especialidades_cmp_by_id (QHashKeyValue * kv1,
    QHashKeyValue * kv2)
{
  return strcmp (kv1->key, kv2->key);
}

void
patan_especialidad_value_print (qpointer data, qpointer user_data)
{
  QHashKeyValue *especialidad;

  especialidad = Q_HASH_KEY_VALUE (data);
  printf ("%s\t\t\t%s\n", especialidad->key, especialidad->value);
}

void
patan_especialidades_print (QSList * especialidades_list, PatanSortBy sort_by)
{
  patan_print_header ("ID", PATAN_CONSOLE_STR,
      "ESPECIALIDAD", PATAN_CONSOLE_STR, NULL);

  switch (sort_by) {
    case PATAN_SORT_BY_ESPECIALIDAD:
      especialidades_list = q_slist_sort (especialidades_list,
          patan_especialidades_cmp_by_especialidad);
      break;
    case PATAN_SORT_BY_ID:
      especialidades_list = q_slist_sort (especialidades_list,
          patan_especialidades_cmp_by_id);
      break;
    default:
      break;
  }
  q_slist_foreach (especialidades_list, patan_especialidad_value_print, NULL);
}

/* Compare Functions */
int
patan_especialidad_eq_nombre (QHashKeyValue * especialidad_kv, char * nombre)
{
  Q_DEBUG ("%s==%s: %d", especialidad_kv->value, nombre);
  return strcmp (especialidad_kv->value, nombre) == 0;
}

/* Leer especialidades desde un archivo */

QHashTable *
patan_parse_especialidades (const char * filename) {
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
    char *id_especialidad, *especialidad;

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
    id_especialidad = strdup (something);


    i = 0;
    while ((c = fgetc (f)) && (isalpha (c) || (c == ' '))) {
      something[i] = c;
      i++;
    }
    something[i] = '\0';
    especialidad = strdup (something);

    q_hash_table_insert (hash_table, strdup (id_especialidad),
        strdup (especialidad));

  } while (c != EOF);

  fclose (f);

  return hash_table;
}

static void
_patan_especialidad_serialize (QHashKeyValue * especialidad,
    FILE * f_especialidades)
{
  char buf[1000];
  sprintf (buf, "%s %s", especialidad->key, especialidad->value);
  fprintf (f_especialidades, "%s\n",  buf);
}

void
patan_especialidades_serialize (const char * out_filename,
    QHashTable * especialidades)
{
  FILE *f;
  f = fopen (out_filename, "w");

  if (!f)
    return NULL;

  q_hash_table_foreach (especialidades, _patan_especialidad_serialize, f);

  fclose (f);
}
