project( SDL2 )
cmake_minimum_required( VERSION 2.8 )

#------ sources ------
#file(GLOB_RECURSE SDL_HEADERS *.h)
set(SDL_HEADERS 
	include/begin_code.h
	include/close_code.h
	include/SDL.h
	include/SDL_assert.h
	include/SDL_atomic.h
	include/SDL_audio.h
	include/SDL_bits.h
	include/SDL_blendmode.h
	include/SDL_clipboard.h
	include/SDL_config.h
	include/SDL_cpuinfo.h
	include/SDL_endian.h
	include/SDL_error.h
	include/SDL_events.h
	include/SDL_filesystem.h
	include/SDL_gamecontroller.h
	include/SDL_gesture.h
	include/SDL_haptic.h
	include/SDL_hints.h
	include/SDL_joystick.h
	include/SDL_keyboard.h
	include/SDL_keycode.h
	include/SDL_loadso.h
	include/SDL_log.h
	include/SDL_main.h
	include/SDL_messagebox.h
	include/SDL_mouse.h
	include/SDL_mutex.h
	include/SDL_name.h
	include/SDL_opengl.h
	include/SDL_opengles.h
	include/SDL_opengles2.h
	include/SDL_pixels.h
	include/SDL_platform.h
	include/SDL_power.h
	include/SDL_quit.h
	include/SDL_rect.h
	include/SDL_render.h
	include/SDL_revision.h
	include/SDL_rwops.h
	include/SDL_scancode.h
	include/SDL_shape.h
	include/SDL_stdinc.h
	include/SDL_surface.h
	include/SDL_system.h
	include/SDL_syswm.h
	include/SDL_test.h
	include/SDL_test_assert.h
	include/SDL_test_common.h
	include/SDL_test_compare.h
	include/SDL_test_crc32.h
	include/SDL_test_font.h
	include/SDL_test_fuzzer.h
	include/SDL_test_harness.h
	include/SDL_test_images.h
	include/SDL_test_log.h
	include/SDL_test_md5.h
	include/SDL_test_random.h
	include/SDL_thread.h
	include/SDL_timer.h
	include/SDL_touch.h
	include/SDL_types.h
	include/SDL_version.h
	include/SDL_video.h )
set_source_files_properties(${SDL_HEADERS} PROPERTIES HEADER_FILE_ONLY TRUE)
source_group("SDL2" FILES ${SDL_HEADERS})

#------ additional includes ------
include_directories( ${HGE_SOURCE_DIR}/deps/SDL2-2.0.3/include )

#------ link directories listed ------
link_directories( ${HGE_SOURCE_DIR}/deps/SDL2-2.0.3/lib/x86 )
link_directories( ${HGE_SOURCE_DIR}/deps/SDL2-2.0.3/lib/x64 )

#------ target -----
