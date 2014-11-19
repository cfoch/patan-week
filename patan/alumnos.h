#include "qlib.h"
#include "utils.h"

#ifndef __PATAN_ALUMNOS_H__
#define __PATAN_ALUMNOS_H__

#define ALUMNO_VALUE(t)       ((AlumnoValue *) t)

typedef QHashTable PatanAlumnos;

typedef struct _AlumnoValue AlumnoValue;

struct _AlumnoValue {
  const char *nombre;
  QDate fecha_nacimiento;
  QHashKeyValue *especialidad;
  /* This is an easter egg dedicated to our friend Oscar Fiestas */
  /* List of QHashKeyValue containing a key-value pair of FiestaValue* */
  QSList *fiestas; 
};


QHashTable*     patan_alumnos_new         ();
AlumnoValue*    alumno_value_new          (const char * nombre,
                                          QDate *fecha_nacimiento,
                                          QHashKeyValue * especialidad);
void            patan_alumnos_insert      (QHashTable * hash_table,
                                          const char * id, const char * nombre,
                                          QDate * date,
                                          QHashKeyValue * especialidad_kv);
QHashTable*     patan_parse_alumnos       (const char * filename,
                                          QHashTable * especialidades);
void            patan_alumno_value_print  (qpointer data, qpointer user_data);
int             patan_alumno_eq_nombre    (QHashKeyValue * alumno_kv,
                                            char * nombre);

#endif
