#include "qlib.h"
#include "alumnos.h"
#include "utils.h"

#ifndef __PATAN_FIESTAS_H__
#define __PATAN_FIESTAS_H__

#define FIESTA_VALUE(t)       ((FiestaValue *) t)

typedef QHashTable PatanFiestas;

typedef struct _FiestaValue FiestaValue;

struct _FiestaValue {
  const char *nombre;
  int precio;
  QDate fecha;
  QQueue *registro_interes;
  /* List of QHashKeyValue containing a key-value pair of AlumnoValue* */
  QSList *asistentes;
  int aforo;
  int monto_recaudado;
  int cantidad_inscritos;
};

QHashTable*     patan_fiestas_new     ();
FiestaValue*    fiesta_value_new      (const char * nombre, int precio,
                                      QDate *fecha);
void            patan_fiestas_insert  (QHashTable * hash_table, const char * id,
                                      const char * nombre, int precio,
                                      QDate * date);
QHashTable*     patan_parse_fiestas   (const char * filename);
void            patan_fiestas_print   (QSList * alumnos_list,
                                      PatanSortBy sort_by);
void            patan_fiesta_value_print (qpointer data, qpointer user_data);
void            patan_fiesta_registrar_interes (QHashKeyValue *fiesta_kv,
                                               QHashKeyValue * alumno_kv);
int             patan_fiesta_eq_nombre (QHashKeyValue * fiesta_kv,
                                          char * nombre_fiesta);
void            patan_fiesta_print_registro_interes (QHashKeyValue * fiesta_kv);
void            patan_fiesta_avanzar_cola (QHashKeyValue * fiesta_kv);

#endif
