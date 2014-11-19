#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "patan.h"

typedef struct {
  QHashKeyValue *fiesta_kv;
  FILE *f_asistencia;
} _PatanAsistenciaSerializeData;

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

    patan_registrar_asistencia (alumno_kv, fiesta_kv);

  } while (c != EOF);

  fclose (f);
}

static void
_patan_asistencia_serialize (QHashKeyValue * alumno_kv, 
    _PatanAsistenciaSerializeData * data)
{
  char buf[1000];
  FILE *f;
  QHashKeyValue *fiesta_kv;

  f = data->f_asistencia;
  fiesta_kv = data->fiesta_kv;

  sprintf (buf, "%s %s", fiesta_kv->key, alumno_kv->key);
  fprintf (f, "%s\n",  buf);
}

static void
_patan_asistencias_serialize (QHashKeyValue * fiesta_kv, FILE * f_asistencia)
{
  FiestaValue *fiesta_val;
  _PatanAsistenciaSerializeData data;

  data.fiesta_kv = fiesta_kv;
  data.f_asistencia = f_asistencia;

  fiesta_val = FIESTA_VALUE (fiesta_kv->value);
  q_slist_foreach (fiesta_val->asistentes, _patan_asistencia_serialize, &data);
}

void
patan_asistencias_serialize (const char * out_filename,
    QHashTable * fiestas)
{
  FILE *f;
  f = fopen (out_filename, "w");

  if (!f)
    return NULL;

  q_hash_table_foreach (fiestas, _patan_asistencias_serialize, f);

  fclose (f);
}

void
patan_registrar_asistencia (QHashKeyValue * alumno_kv,
    QHashKeyValue * fiesta_kv)
{
  FiestaValue *fiesta_val;
  AlumnoValue *alumno_val;

  Q_DEBUG ("Registrando asistencia.", NULL);

  alumno_val = ALUMNO_VALUE (alumno_kv->value);
  fiesta_val = FIESTA_VALUE (fiesta_kv->value);

  Q_DEBUG ("Tomando values.", NULL);

  if (fiesta_val->cantidad_inscritos == fiesta_val->aforo)
    return;

  fiesta_val->asistentes = q_slist_prepend (fiesta_val->asistentes, alumno_kv);
  alumno_val->fiestas = q_slist_prepend (alumno_val->fiestas, fiesta_kv);

  Q_DEBUG ("La fiesta %s tiene %d inscritos", fiesta_val->nombre,
      fiesta_val->cantidad_inscritos);
  Q_DEBUG ("La fiesta %s tiene un total de S/.%d", fiesta_val->nombre,
      fiesta_val->monto_recaudado);

  fiesta_val->cantidad_inscritos++;
  fiesta_val->monto_recaudado += fiesta_val->precio;

  Q_DEBUG ("Ahora la fiesta %s tiene %d inscritos", fiesta_val->nombre,
      fiesta_val->cantidad_inscritos);
  Q_DEBUG ("Ahora la fiesta %s tiene un total de S/.%d", fiesta_val->nombre,
      fiesta_val->monto_recaudado);
}
