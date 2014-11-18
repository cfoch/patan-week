#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "qlib.h"
#include "patan.h"

typedef enum {
  PATAN_OPT_REGISTRAR_ESPECIALIDAD = 1,
  PATAN_OPT_REGISTRAR_ALUMNO = 2,
  PATAN_OPT_REGISTRAR_FIESTA = 3,
  PATAN_OPT_REGISTRAR_INTERES = 4,
  PATAN_OPT_REGISTRAR_AFORO_FIESTA = 5,
  PATAN_OPT_REGISTRAR_CERRAR_REGISTRO_INTERES = 6,
  PATAN_OPT_MOSTRAR_ESPECIALIDADES = 7,
  PATAN_OPT_MOSTRAR_ALUMNOS = 8,
  PATAN_OPT_MOSTRAR_FIESTAS = 9,
  PATAN_OPT_MOSTRAR_FIESTA_REGISTRO_INTERES = 10,
  PATAN_OPT_MOSTRAR_FIESTA_ASISTENTES = 11,
  PATAN_OPT_MOSTRAR_ALUMNO_FIESTAS_ASISTIDAS = 12,
  
  /* Especialidades */
  PATAN_OPT_SORT_ESPECIALIDADES_BY_ID = 1,
  PATAN_OPT_SORT_ESPECIALIDADES_BY_ESPECIALIDAD = 2,
  /* Alumnos */
  PATAN_OPT_SORT_ALUMNOS_BY_ID = 1,
  PATAN_OPT_SORT_ALUMNOS_BY_NOMBRE = 2,
  PATAN_OPT_SORT_ALUMNOS_BY_FECHA_NACIMIENTO = 3,
  PATAN_OPT_SORT_ALUMNOS_BY_ESPECIALIDAD = 4,
  /* Fiestas */
  PATAN_OPT_SORT_FIESTAS_BY_ID = 1,
  PATAN_OPT_SORT_FIESTAS_BY_NOMBRE = 2,
  PATAN_OPT_SORT_FIESTAS_BY_FECHA = 3,
  /* Extras */
  PATAN_OPT_BACK = 0,
  PATAN_OPT_EXIT = -1,
  PATAN_OPT_YES = 'y',
  PATAN_OPT_NO = 'n'
} PatanOpt;

void
patan_init (PatanEspecialidades ** especialidades,
  PatanAlumnos ** alumnos, PatanFiestas ** fiestas)
{
  *especialidades = patan_especialidades_new ();
  *alumnos = patan_alumnos_new ();
  *fiestas = patan_fiestas_new ();
}

void
patan_console_ask_for_files (PatanEspecialidades ** especialidades,
  PatanAlumnos ** alumnos, PatanFiestas ** fiestas,
  char ** especialidades_filename, char ** alumnos_filename,
  char ** fiestas_filename, char ** asistencias_filename)
{
  char filename[100];

  printf ("Ingrese archivo de especialidades: ");
  scanf ("%s", filename);
  *especialidades = patan_parse_especialidades (filename);
  *especialidades_filename = strdup (filename);

  printf ("Ingrese archivo de alumnos: ");
  scanf ("%s", filename);
  *alumnos = patan_parse_alumnos (filename, *especialidades);
  *alumnos_filename = strdup (filename);

  printf ("Ingrese archivo de fiestas: ");
  scanf ("%s", filename);
  *fiestas = patan_parse_fiestas (filename);
  *fiestas_filename = strdup (filename);

  printf ("Ingrese archivo de asistencias: ");
  scanf ("%s", filename);
  patan_parse_asistencia (filename, *fiestas, *alumnos);
  asistencias_filename = strdup (filename);
}

static void
patan_console_show_main_menu ()
{
  printf (" 1) Registrar especialidad.\n");
  printf (" 2) Registrar alumno.\n");
  printf (" 3) Registrar fiesta.\n");
  printf (" 4) Registrar interes.\n");
  printf (" 5) Registrar aforo de fiesta.\n");
  printf (" 6) Registrar lista de interes automaticamente (avanzar cola).\n");
  printf (" 7) Mostrar especialidades.\n"); 
  printf (" 8) Mostrar alumnos.\n");
  printf (" 9) Mostrar fiestas.\n");
  printf ("10) Mostrar registro de interés.\n");
  printf ("11) Mostrar lista de asistentes.\n");
  printf ("12) Mostrar fiestas a la que asistió un alumno.\n");
  printf ("-1) Quit.\n");
}

static void
patan_console_show_sort_by_especialidades ()
{
  printf ("1) Ordenar por id.\n");
  printf ("2) Ordenar por especialidad.\n");
  printf ("-1) Back.\n");
}

static void
patan_console_show_sort_by_alumnos ()
{
  printf ("1) Ordenar por id.\n");
  printf ("2) Ordenar por nombre.\n");
  printf ("3) Ordenar por fecha de nacimiento.\n");
  printf ("4) Ordenar por especialidad.\n");
  printf ("-1) Back.\n");
}

static void
patan_console_show_sort_by_fiestas ()
{
  printf ("1) Ordenar por id.\n");
  printf ("2) Ordenar por nombre.\n");
  printf ("3) Ordenar por fecha de fiesta.\n");
  printf ("-1) Back.\n");
}

static int
validate_id_alumno (char * id)
{
  int i, val = 1;
  for (i=0; i<8; i++)
    if(!isdigit(id[i]))
      return 0;   
  return (8 == strlen(id));
}

char
patan_console_loop_exit_screen_message (int * error)
{
  char opt;
  printf ("¿Desea guardar los cambios? y/n: ");
  scanf ("%c", &opt);
  *error = (opt != 'y') || (opt != 'n');
  return opt;
}


qboolean
patan_console_menu (PatanEspecialidades * especialidades,
    PatanAlumnos * alumnos, PatanFiestas * fiestas,
    const char * especialidades_filename, const char * alumnos_filename,
    const char * fiestas_filename, const char * asistencias_filename)
{
  PatanOpt opt;
  qboolean ret;

  ret = TRUE;
  patan_console_show_main_menu ();

  printf ("Ingrese opcion: ");
  scanf ("%d", &opt);

  switch (opt) {
    case PATAN_OPT_REGISTRAR_ESPECIALIDAD:
    {
      char id[100];
      char especialidad[500];
      printf ("id: ");
      scanf ("%s", id);
      printf ("especialidad: ");
      scanf (" %[^\n]s", especialidad);
      patan_especialidades_insert (especialidades, id, especialidad);
      break;
    }
    case PATAN_OPT_REGISTRAR_ALUMNO:
    {
      char id[9];
      char nombre[500];
      char especialidad[500];
      char fecha_nacimiento[9];
      QDate date;
      QHashKeyValue *especialidad_kv;

      printf ("Codigo de alumno: ");
      scanf ("%s", id);
      
      if (!validate_id_alumno (id)) {
        printf ("Codigo de alumno no valido.\n");
        break;
      }

      printf ("Nombre de alumno: ");
      scanf (" %[^\n]s", nombre);
      Q_DEBUG ("Nombre: %s", nombre); 

      printf ("Nombre de especialidad: ");
      scanf (" %[^\n]s", especialidad);
      Q_DEBUG ("Especialidad: %s", especialidad); 

      Q_DEBUG ("Obteniendo especialidad_kv (QHashKeyValue)", NULL);
      especialidad_kv =\
          q_hash_table_get_key_value_by_data (Q_HASH_TABLE (especialidades),
              especialidad, patan_especialidad_eq_nombre);
      Q_DEBUG ("Validando especialidad_kv (QHashKeyValue)", NULL);

      /* Validar existencia de especialidad */
      if (!especialidad_kv) {
        printf ("Especialidad no registrada.");
        break;
      }

      printf ("Fecha de nacimiento (dd mm yyyy): ");
      scanf ("%d %d %d", &(date.day), &(date.month), &(date.year));
      patan_alumnos_insert (alumnos, id, nombre, &date, especialidad_kv);
      
      break;
    }
    case PATAN_OPT_REGISTRAR_FIESTA:
    {
      char id[9];
      char nombre[100];
      QDate date;
      int precio;

      printf ("ID de fiesta: ");
      scanf ("%s", id);
      Q_DEBUG ("ID de fiesta: %s", id);

      printf ("Nombre de fiesta: ");
      scanf (" %[^\n]s", nombre);
      Q_DEBUG ("Nombre de fiesta: %s", nombre);

      printf ("Fecha de nacimiento (dd mm yyyy): ");
      scanf ("%d %d %d", &(date.day), &(date.month), &(date.year));
      Q_DEBUG ("Fecha de nacimiento: %s", q_date_to_string (&date));

      printf ("Precio: ");
      scanf ("%d", precio);

      patan_fiestas_insert (fiestas, id, nombre, precio, &date);

      break;
    }
    case PATAN_OPT_REGISTRAR_INTERES:
    {
      QHashKeyValue *alumno_kv;
      QHashKeyValue *fiesta_kv;
      char codigo[9];
      char fiesta[100];
      printf ("Nombre de fiesta: ");
      scanf (" %[^\n]s", fiesta);

      fiesta_kv = q_hash_table_get_key_value_by_data (fiestas, fiesta,
          patan_fiesta_eq_nombre);
      if (fiesta_kv) {
        printf ("Código de alumno: ");
        scanf ("%s", codigo);
        alumno_kv = q_hash_table_get_key_value_by_key (alumnos, codigo);
        if (alumno_kv) {
          patan_fiesta_registrar_interes (fiesta_kv, alumno_kv);
        } else
          printf ("Codigo de alumno no registrado.\n");
      } else
        printf ("Nombre de fiesta no registrado.\n");      
      break;
    }
    case PATAN_OPT_REGISTRAR_AFORO_FIESTA:
    {
      char fiesta[100];
      QHashKeyValue *fiesta_kv;

      printf ("Nombre de fiesta: ");
      scanf (" %[^\n]s", fiesta);

      fiesta_kv = q_hash_table_get_key_value_by_data (fiestas, fiesta,
          patan_fiesta_eq_nombre);

      if (fiesta_kv) {
        printf ("Aforo de fiesta: ");
        scanf ("%d", &(FIESTA_VALUE (fiesta_kv->value)->aforo));
      } else
        printf ("Nombre de fiesta no registrado.\n"); 
      break;
    }
    case PATAN_OPT_REGISTRAR_CERRAR_REGISTRO_INTERES:
    {
      char fiesta[100];
      QHashKeyValue *fiesta_kv;

      printf ("Nombre de fiesta: ");
      scanf (" %[^\n]s", fiesta);

      fiesta_kv = q_hash_table_get_key_value_by_data (fiestas, fiesta,
          patan_fiesta_eq_nombre);

      Q_DEBUG ("Se encontro la fiesta %s. Codigo: %s", fiesta, fiesta_kv->key);
      if (fiesta_kv)
        patan_fiesta_avanzar_cola (fiesta_kv);
      else
        printf ("Nombre de fiesta no registrado.\n");

      break;
    }
    case PATAN_OPT_MOSTRAR_ESPECIALIDADES:
    {
      PatanSortBy sort_by;
      QSList *especialidades_list;

      patan_console_show_sort_by_especialidades ();

      printf ("Ingrese opcion: ");
      scanf ("%d", &opt);

      switch (opt) {
        case PATAN_OPT_SORT_ESPECIALIDADES_BY_ID:
          especialidades_list = q_hash_table_get_key_values (especialidades);
          patan_especialidades_print (especialidades_list, PATAN_SORT_BY_ID);
          break;
        case PATAN_OPT_SORT_ESPECIALIDADES_BY_ESPECIALIDAD:
          especialidades_list = q_hash_table_get_key_values (especialidades);
          patan_especialidades_print (especialidades_list,
              PATAN_SORT_BY_ESPECIALIDAD);
          break;
        default:
          opt = PATAN_OPT_BACK;
          break;
      }
      break;
    }
    case PATAN_OPT_MOSTRAR_ALUMNOS:
    {
      PatanSortBy sort_by;
      QSList *alumnos_list;

      patan_console_show_sort_by_alumnos ();

      printf ("Ingrese opcion: ");
      scanf ("%d", &opt);

      switch (opt) {
        case PATAN_OPT_SORT_ALUMNOS_BY_ID:
          alumnos_list = q_hash_table_get_key_values (alumnos);
          patan_alumnos_print (alumnos_list, PATAN_SORT_BY_ID);
          break;
        case PATAN_OPT_SORT_ALUMNOS_BY_NOMBRE:
          alumnos_list = q_hash_table_get_key_values (alumnos);
          patan_alumnos_print (alumnos_list,
              PATAN_SORT_BY_NOMBRE);
          break;
        case PATAN_OPT_SORT_ALUMNOS_BY_FECHA_NACIMIENTO:          
          alumnos_list = q_hash_table_get_key_values (alumnos);
          patan_alumnos_print (alumnos_list,
              PATAN_SORT_BY_FECHA);         
          break;
        case PATAN_OPT_SORT_ALUMNOS_BY_ESPECIALIDAD:          
          alumnos_list = q_hash_table_get_key_values (alumnos);
          patan_alumnos_print (alumnos_list,
              PATAN_SORT_BY_ESPECIALIDAD);
          break;
        default:
          opt = PATAN_OPT_BACK;
          break;
      }
      break;
    }
    case PATAN_OPT_MOSTRAR_FIESTAS:
    {
      PatanSortBy sort_by;
      QSList *fiestas_list;

      patan_console_show_sort_by_fiestas ();

      printf ("Ingrese opcion: ");
      scanf ("%d", &opt);

      switch (opt) {
        case PATAN_OPT_SORT_FIESTAS_BY_ID:
          fiestas_list = q_hash_table_get_key_values (fiestas);
          patan_fiestas_print (fiestas_list, PATAN_SORT_BY_ID);
          break;
        case PATAN_OPT_SORT_FIESTAS_BY_NOMBRE:
          fiestas_list = q_hash_table_get_key_values (fiestas);
          patan_fiestas_print (fiestas_list, PATAN_SORT_BY_NOMBRE);
          break;
        case PATAN_OPT_SORT_FIESTAS_BY_FECHA:
          fiestas_list = q_hash_table_get_key_values (fiestas);
          patan_fiestas_print (fiestas_list, PATAN_SORT_BY_FECHA);
          break;
        default:
          /* Este es un truco para retroceder en el menu */
          opt = PATAN_OPT_BACK;
          break;
      }
      break;
    }
    case PATAN_OPT_MOSTRAR_FIESTA_REGISTRO_INTERES:
    {
      QHashKeyValue *fiesta_kv;
      char fiesta[100];
      printf ("Ingrese nombre de fiesta: ");
      scanf ("%s", fiesta);

      fiesta_kv = q_hash_table_get_key_value_by_data (fiestas, fiesta,
          patan_fiesta_eq_nombre);
      if (fiesta_kv)
        patan_fiesta_print_registro_interes (fiesta_kv);
      else
        printf ("Nombre de fiesta no registrado.\n");      
      break;
    }
    case PATAN_OPT_MOSTRAR_FIESTA_ASISTENTES:
    {
      QHashKeyValue *fiesta_kv;
      char fiesta[100];
      printf ("Ingrese nombre de fiesta: ");
      scanf ("%s", fiesta);

      fiesta_kv = q_hash_table_get_key_value_by_data (fiestas, fiesta,
          patan_fiesta_eq_nombre);
      if (fiesta_kv)
        patan_alumnos_print (FIESTA_VALUE (fiesta_kv->value)->asistentes,
            PATAN_SORT_BY_ID);
      else
        printf ("Nombre de fiesta no registrado.\n");
      break;
    }
    case PATAN_OPT_MOSTRAR_ALUMNO_FIESTAS_ASISTIDAS:
    {
      QHashKeyValue *alumno_kv;
      char codigo[9];
      printf ("Codigo de alumno: ");
      scanf ("%s", codigo);

      alumno_kv = q_hash_table_get_key_value_by_key (alumnos, codigo);
      if (alumno_kv)
        patan_fiestas_print (ALUMNO_VALUE (alumno_kv->value)->fiestas,
            PATAN_SORT_BY_ID);
      else
        printf ("Alumno no registrado.\n");
      break;
    }
    case PATAN_OPT_EXIT:
    {

      patan_especialidades_serialize (especialidades_filename,
          especialidades);

      patan_alumnos_serialize (alumnos_filename, alumnos);

      patan_fiestas_serialize (fiestas_filename, fiestas);

      patan_asistencias_serialize (asistencias_filename, fiestas);

      ret = FALSE;
      break;
    }
    default:
      ret = TRUE;
  }
  return ret;
}

void
patan_console_loop (PatanEspecialidades * especialidades,
    PatanAlumnos * alumnos, PatanFiestas * fiestas,
    const char * especialidades_filename, const char * alumnos_filename,
    const char * fiestas_filename, const char * asistencias_filename)
{
  while (patan_console_menu (especialidades, alumnos, fiestas,
      especialidades_filename, alumnos_filename, fiestas_filename,
      asistencias_filename) == TRUE);
}


int
main (int argc, char ** argv)
{
  PatanEspecialidades *especialidades;
  PatanAlumnos *alumnos;
  PatanFiestas *fiestas;
  char *especialidades_filename, *alumnos_filename;
  char *asistencias_filename, *fiestas_filename;

  patan_console_ask_for_files (&especialidades, &alumnos, &fiestas,
      &especialidades_filename, &alumnos_filename, &fiestas_filename,
      &asistencias_filename);

  printf ("%s\n", alumnos_filename);

  patan_console_loop (especialidades, alumnos, fiestas,
      especialidades_filename, alumnos_filename, fiestas_filename,
      asistencias_filename);  

  return 0;
}
