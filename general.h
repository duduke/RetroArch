/*  SSNES - A Super Nintendo Entertainment System (SNES) Emulator frontend for libsnes.
 *  Copyright (C) 2010-2011 - Hans-Kristian Arntzen
 *
 *  Some code herein may be based on code found in BSNES.
 * 
 *  SSNES is free software: you can redistribute it and/or modify it under the terms
 *  of the GNU General Public License as published by the Free Software Found-
 *  ation, either version 3 of the License, or (at your option) any later version.
 *
 *  SSNES is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 *  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 *  PURPOSE.  See the GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along with SSNES.
 *  If not, see <http://www.gnu.org/licenses/>.
 */


#ifndef __SSNES_GENERAL_H
#define __SSNES_GENERAL_H

#include "boolean.h"
#include <stdio.h>
#include <time.h>
#include <limits.h>
#include "driver.h"
#include "record/ffemu.h"
#include "message.h"
#include "rewind.h"
#include "movie.h"
#include "autosave.h"
#include "dynamic.h"
#include "cheats.h"
#include "audio/ext/ssnes_dsp.h"

#ifdef __CELLOS_LV2__
#include <sys/timer.h>
#endif

#ifdef XENON
#include <time/time.h>
#endif

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_NETPLAY
#include "netplay.h"
#endif

#include "audio/hermite.h"

#if defined(_WIN32) && !defined(_XBOX)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "msvc/msvc_compat.h"
#elif defined(_XBOX)
#include <xtl.h>
#include "360/msvc_compat.h"
#endif

#define MAX_PLAYERS 5
#define MAX_BINDS (SSNES_BIND_LIST_END + 1)
#define SSNES_NO_JOYPAD 0xFFFF

enum ssnes_shader_type
{
   SSNES_SHADER_CG,
   SSNES_SHADER_BSNES,
   SSNES_SHADER_AUTO,
   SSNES_SHADER_NONE
};

// All config related settings go here.
struct settings
{
   struct 
   {
      char driver[32];
      float xscale;
      float yscale;
      bool fullscreen;
      unsigned fullscreen_x;
      unsigned fullscreen_y;
      bool vsync;
      bool smooth;
      bool force_aspect;
      bool crop_overscan;
      float aspect_ratio;
      char cg_shader_path[PATH_MAX];
      char bsnes_shader_path[PATH_MAX];
      char filter_path[PATH_MAX];
      enum ssnes_shader_type shader_type;
      float refresh_rate;

      bool render_to_texture;
      double fbo_scale_x;
      double fbo_scale_y;
      char second_pass_shader[PATH_MAX];
      bool second_pass_smooth;
      char shader_dir[PATH_MAX];

      char font_path[PATH_MAX];
      unsigned font_size;
      bool font_enable;
      float msg_pos_x;
      float msg_pos_y;
      float msg_color_r;
      float msg_color_g;
      float msg_color_b;

      bool force_16bit;
      bool disable_composition;

      bool hires_record;
      bool post_filter_record;

      char external_driver[PATH_MAX];
   } video;

   struct
   {
      char driver[32];
      bool enable;
      unsigned out_rate;
      float in_rate;
      float rate_step;
      char device[PATH_MAX];
      unsigned latency;
      bool sync;

      char dsp_plugin[PATH_MAX];
      char external_driver[PATH_MAX];
   } audio;

   struct
   {
      char driver[32];
      struct snes_keybind binds[MAX_PLAYERS][MAX_BINDS];
      float axis_threshold;
      unsigned joypad_map[MAX_PLAYERS];
      bool netplay_client_swap_input;
   } input;

   char libsnes[PATH_MAX];
   char cheat_database[PATH_MAX];
   char cheat_settings_path[PATH_MAX];

   char screenshot_directory[PATH_MAX];

   bool rewind_enable;
   size_t rewind_buffer_size;
   unsigned rewind_granularity;

   bool pause_nonactive;
   unsigned autosave_interval;

   bool block_sram_overwrite;
   bool savestate_auto_index;
};

enum ssnes_game_type
{
   SSNES_CART_NORMAL = 0,
   SSNES_CART_SGB,
   SSNES_CART_BSX,
   SSNES_CART_BSX_SLOTTED,
   SSNES_CART_SUFAMI
};


// All run-time- / command line flag-related globals go here.
struct global
{
   bool verbose;
   bool audio_active;
   bool video_active;
   bool force_fullscreen;

   bool has_mouse[2];
   bool has_scope[2];
   bool has_justifier;
   bool has_justifiers;
   bool has_multitap;
   bool disconnect_device[2];

   FILE *rom_file;
   enum ssnes_game_type game_type;
   uint32_t cart_crc;

   char gb_rom_path[PATH_MAX];
   char bsx_rom_path[PATH_MAX];
   char sufami_rom_path[2][PATH_MAX];
   bool has_set_save_path;
   bool has_set_state_path;

#ifdef HAVE_CONFIGFILE
   char config_path[PATH_MAX];
#endif
   
   char basename[PATH_MAX];
   char savefile_name_srm[PATH_MAX];
   char savefile_name_rtc[PATH_MAX]; // Make sure that fill_pathname has space.
   char savefile_name_psrm[PATH_MAX];
   char savefile_name_asrm[PATH_MAX];
   char savefile_name_bsrm[PATH_MAX];
   char savestate_name[PATH_MAX];
   char xml_name[PATH_MAX];

   bool ups_pref;
   bool bps_pref;
   char ups_name[PATH_MAX];
   char bps_name[PATH_MAX];

   unsigned state_slot;

   struct
   {
      struct snes_geometry geom;
      unsigned pitch; // If 0, has classic libsnes semantics.
      char fullpath[PATH_MAX];
      struct snes_system_timing timing;
      bool timing_set;
      bool need_fullpath;
   } system;

   struct
   {
      hermite_resampler_t *source;

      float *data;
      size_t data_ptr;
      size_t chunk_size;
      size_t nonblock_chunk_size;
      size_t block_chunk_size;

      double src_ratio;

      bool use_float;
      bool mute;

      float *outsamples;
      int16_t *conv_outsamples;

      int16_t *rewind_buf;
      size_t rewind_ptr;
      size_t rewind_size;

      dylib_t dsp_lib;
      const ssnes_dsp_plugin_t *dsp_plugin;
      void *dsp_handle;
   } audio_data;

   struct
   {
      bool active;
      uint32_t *buffer;
      uint32_t *colormap;
      unsigned pitch;
      dylib_t lib;
      unsigned scale;

      void (*psize)(unsigned *width, unsigned *height);
      void (*prender)(uint32_t *colormap, uint32_t *output, unsigned outpitch,
            const uint16_t *input, unsigned pitch, unsigned width, unsigned height);
   } filter;

   msg_queue_t *msg_queue;

   // Rewind support.
   state_manager_t *state_manager;
   void *state_buf;
   size_t state_size;
   bool frame_is_reverse;

   // Movie playback/recording support.
   struct
   {
      bsv_movie_t *movie;
      char movie_path[PATH_MAX];
      bool movie_playback;

      // Immediate playback/recording.
      char movie_start_path[PATH_MAX];
      bool movie_start_recording;
      bool movie_start_playback;
      bool movie_end;
   } bsv;

   bool sram_load_disable;
   bool sram_save_disable;

   // Pausing support
   bool is_paused;
   bool is_oneshot;

   // Autosave support.
   autosave_t *autosave[2];

   // Netplay.
#ifdef HAVE_NETPLAY
   netplay_t *netplay;
   char netplay_server[PATH_MAX];
   bool netplay_enable;
   bool netplay_is_client;
   unsigned netplay_sync_frames;
   uint16_t netplay_port;
#endif

   // FFmpeg record.
#ifdef HAVE_FFMPEG
   ffemu_t *rec;
   char record_path[PATH_MAX];
   bool recording;
   unsigned record_width;
   unsigned record_height;
#endif

   struct
   {
      const uint16_t *data;
      unsigned width;
      unsigned height;
   } frame_cache;

   char title_buf[64];

   struct
   {
      char **elems;
      size_t size;
      size_t ptr;
   } shader_dir;

   char sha256[64 + 1];

#ifdef HAVE_XML
   cheat_manager_t *cheat;
#endif
};

void parse_config(void);

extern struct settings g_settings;
extern struct global g_extern;

#define SSNES_LOG(...) do { \
   if (g_extern.verbose) \
      fprintf(stderr, "SSNES: " __VA_ARGS__); \
      fflush(stderr); \
   } while (0)

#define SSNES_ERR(...) do { \
      fprintf(stderr, "SSNES [ERROR] :: " __VA_ARGS__); \
      fflush(stderr); \
   } while (0)

#define SSNES_WARN(...) do { \
      fprintf(stderr, "SSNES [WARN] :: " __VA_ARGS__); \
      fflush(stderr); \
   } while (0)

#ifndef max
#define max(a, b) ((a) > (b) ? (a) : (b))
#endif

#ifndef min
#define min(a, b) ((a) < (b) ? (a) : (b))
#endif

#define SSNES_SCALE_BASE 256

static inline uint32_t next_pow2(uint32_t v)
{
   v--;
   v |= v >> 1;
   v |= v >> 2;
   v |= v >> 4;
   v |= v >> 8;
   v |= v >> 16;
   v++;
   return v;
}

static inline uint8_t is_little_endian(void)
{
   union
   {
      uint16_t x;
      uint8_t y[2];
   } u;

   u.x = 1;
   return u.y[0];
}

#ifdef GEKKO
#include <unistd.h>
#endif

static inline void ssnes_sleep(unsigned msec)
{
#ifdef __CELLOS_LV2__
   sys_timer_usleep(1000 * msec);
#elif defined(_WIN32)
   Sleep(msec);
#elif defined(XENON)
   udelay(1000 * msec);
#elif defined(GEKKO)
   usleep(1000 * msec);
#else
   struct timespec tv = {0};
   tv.tv_sec = msec / 1000;
   tv.tv_nsec = (msec % 1000) * 1000000;
   nanosleep(&tv, NULL);
#endif
}

#define ssnes_assert(cond) \
   if (!(cond)) { SSNES_ERR("Assertion failed at %s:%d!\n", __FILE__, __LINE__); exit(2); }

#endif


