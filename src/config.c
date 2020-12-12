// -------------------------------------------------------------
// config.c
// -------------------------------------------------------------
// Se encarga de cargar / salvar la configuracion de Kraptor
// Por Kronoman
// Copyright (c) 2003, Kronoman
// En memoria de mi querido padre
// -------------------------------------------------------------

#include <allegro.h>
#include "jugador.h"
#include "config.h"
#include "global.h"

#include <unistd.h>
#include <string.h>

void cargar_configuracion()
{
 char *home;
 home = getenv("HOME");
 if (home != NULL)
 {
  char str[4096];
  snprintf(str, sizeof(str), "%s/.kraptor.cfg", home);
  set_config_file("/etc/kraptor/kraptor.cfg");
  override_config_file(str);
 }
 else
 {
  set_config_file("kraptor.cfg");
 }
 teclado_jug.arr = get_config_int("kraptor_keyboard", "arr", teclado_jug.arr);
 teclado_jug.abj = get_config_int("kraptor_keyboard", "abj", teclado_jug.abj);
 teclado_jug.izq = get_config_int("kraptor_keyboard", "izq", teclado_jug.izq);
 teclado_jug.der = get_config_int("kraptor_keyboard", "der", teclado_jug.der);
 teclado_jug.sht = get_config_int("kraptor_keyboard", "sht", teclado_jug.sht);
 teclado_jug.wpn = get_config_int("kraptor_keyboard", "wpn", teclado_jug.wpn);
 teclado_jug.bmb = get_config_int("kraptor_keyboard", "bmb", teclado_jug.bmb);

 nivel_detalle = get_config_int("kraptor_detalle", "nivel_detalle", nivel_detalle);
 detalle_automatico = get_config_int("kraptor_detalle", "detalle_automatico", detalle_automatico);
 quiere_videos = get_config_int("kraptor_detalle", "quiere_videos", quiere_videos);
 
 quiere_snd = get_config_int("kraptor_snd", "quiere_snd", quiere_snd);
 volumen_sonido = get_config_int("kraptor_snd", "volumen_sonido", volumen_sonido);
 quiere_musica = get_config_int("kraptor_snd", "quiere_musica", quiere_musica);
 volumen_musica = get_config_int("kraptor_snd", "volumen_musica", volumen_musica);

 KRONO_QUIERE_DEBUG = get_config_int("KRONO_QUIERE_DEBUG", "KRONO_QUIERE_DEBUG", KRONO_QUIERE_DEBUG);
 quiere_usar_joystick = get_config_int("kraptor_joystick", "quiere_usar_joystick", quiere_usar_joystick);
 numero_de_joystick = get_config_int("kraptor_joystick", "numero_de_joystick", numero_de_joystick);


 quiere_usar_mouse = get_config_int("kraptor_mouse", "quiere_usar_mouse", quiere_usar_mouse);
 mouse_velocidad = get_config_int("kraptor_mouse", "mouse_velocidad", mouse_velocidad);

// load_joystick_data(NULL);  // esto me dio problemas la primera vez que lo use... ojo

 
}

void salvar_configuracion()
{
 char *home;
 home = getenv("HOME");
 if (home != NULL)
 {
  char str[4096];
  snprintf(str, sizeof(str), "%s/.kraptor.cfg", home);
  set_config_file(str);
 }
 else
 {
  set_config_file("kraptor.cfg");
 }

 set_config_int("kraptor_keyboard", "arr", teclado_jug.arr);
 set_config_int("kraptor_keyboard", "abj", teclado_jug.abj);
 set_config_int("kraptor_keyboard", "izq", teclado_jug.izq);
 set_config_int("kraptor_keyboard", "der", teclado_jug.der);
 set_config_int("kraptor_keyboard", "sht", teclado_jug.sht);
 set_config_int("kraptor_keyboard", "wpn", teclado_jug.wpn);
 set_config_int("kraptor_keyboard", "bmb", teclado_jug.bmb);

 set_config_int("kraptor_detalle", "nivel_detalle", nivel_detalle);
 set_config_int("kraptor_detalle", "quiere_videos", quiere_videos);
 set_config_int("kraptor_detalle", "detalle_automatico", detalle_automatico);

 set_config_int("kraptor_snd", "quiere_snd", quiere_snd);
 set_config_int("kraptor_snd", "volumen_sonido", volumen_sonido);
 set_config_int("kraptor_snd", "quiere_musica", quiere_musica);
 set_config_int("kraptor_snd", "volumen_musica", volumen_musica);
 set_config_int("KRONO_QUIERE_DEBUG", "KRONO_QUIERE_DEBUG", KRONO_QUIERE_DEBUG);

 set_config_int("kraptor_joystick", "quiere_usar_joystick", quiere_usar_joystick);
 set_config_int("kraptor_joystick", "numero_de_joystick", numero_de_joystick);

 set_config_int("kraptor_mouse", "quiere_usar_mouse", quiere_usar_mouse);
 set_config_int("kraptor_mouse", "mouse_velocidad", mouse_velocidad);
 
// save_joystick_data(NULL); // esto me dio problemas la primera vez que lo use... ojo
}
