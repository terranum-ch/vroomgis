#------------------------------------------------------------------------------
# Desc:
# Tabs: 3
#
# Copyright (c) 2007 Novell, Inc. All Rights Reserved.
#
# This program and the accompanying materials are made available 
# under the terms of the Eclipse Public License v1.0 which
# accompanies this distribution, and is available at 
# http://www.eclipse.org/legal/epl-v10.html
#
# To contact Novell about this file by physical or electronic mail, 
# you may find current contact information at www.novell.com.
#
# $Id$
#
# Author: Andrew Hodgkinson <ahodgkinson@novell.com>
#------------------------------------------------------------------------------

# Include the local modules directory

set( CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMakeModules")

# Include FindGLib

set( GLIB_FIND_REQUIRED ${GTK2_FIND_REQUIRED})

# Locate GLib files

if( NOT GLIB_FOUND)

	find_path( GLIB_INCLUDE_DIR glib.h 
		PATHS /opt/gtk/include
				/opt/gnome/include
				/usr/include
				/usr/local/include
		PATH_SUFFIXES glib-2.0
		NO_DEFAULT_PATH
	)
	MARK_AS_ADVANCED( GLIB_INCLUDE_DIR)
		
	find_path( GLIB_CONFIG_INCLUDE_DIR glibconfig.h 
		PATHS	/opt/gtk/include
				/opt/gtk/lib
				/opt/gnome/include 
				/opt/gnome/lib
				/usr/include
				/usr/lib
				/usr/local/include
		PATH_SUFFIXES /glib-2.0/include
		NO_DEFAULT_PATH
	)
	MARK_AS_ADVANCED( GLIB_CONFIG_INCLUDE_DIR)

	if( NOT GLIB_CONFIG_INCLUDE_DIR)
		message( STATUS "Unable to find GLIB_CONFIG_INCLUDE_DIR")
	endif( NOT GLIB_CONFIG_INCLUDE_DIR)
		
	find_library( GLIB_LIBRARY 
		NAMES glib-2.0 
		PATHS /opt/gtk/lib
				/opt/gnome/lib
				/usr/lib 
				/usr/local/lib
		NO_DEFAULT_PATH
	) 
	MARK_AS_ADVANCED( GLIB_LIBRARY)
		
	if( GLIB_INCLUDE_DIR AND GLIB_CONFIG_INCLUDE_DIR AND GLIB_LIBRARY)
		set( GLIB_FOUND TRUE)
		set( GLIB_INCLUDE_DIRS ${GLIB_INCLUDE_DIR} ${GLIB_CONFIG_INCLUDE_DIR})
		set( GLIB_LIBRARIES ${GLIB_LIBRARY})
	endif( GLIB_INCLUDE_DIR AND GLIB_CONFIG_INCLUDE_DIR AND GLIB_LIBRARY)
	
	if( GLIB_FOUND)
		if( NOT GLIB_FIND_QUIETLY)
			message( STATUS "Found GLib library: ${GLIB_LIBRARY}")
			message( STATUS "Found GLib inc dirs: ${GLIB_INCLUDE_DIRS}")
		endif( NOT GLIB_FIND_QUIETLY)
	else( GLIB_FOUND)
		if( GLIB_FIND_REQUIRED)
			message( FATAL_ERROR "Could not find GLib")
		else( GLIB_FIND_REQUIRED)
			if( NOT GLIB_FIND_QUIETLY)
				message( STATUS "Could not find GLib")
			endif( NOT GLIB_FIND_QUIETLY)
		endif( GLIB_FIND_REQUIRED)
	endif( GLIB_FOUND)

endif( NOT GLIB_FOUND)


message( STATUS "Looking for GTK2 ...")

# Find gtk include directory

find_path( GTK2_gtk_INCLUDE_DIR gtk/gtk.h
	PATHS /opt/gtk/include
			/opt/gnome/include
			/usr/include
			/usr/local/include
	PATH_SUFFIXES gtk-2.0
	NO_DEFAULT_PATH
)
mark_as_advanced( GTK2_gtk_INCLUDE_DIR)

if( NOT GTK2_gtk_INCLUDE_DIR)
	message( STATUS "Unable to find GTK2_gtk_INCLUDE_DIR")
endif( NOT GTK2_gtk_INCLUDE_DIR)

# Find gtk library

find_library( GTK2_gtk_LIBRARY
	NAMES gtk-quartz-2.0 gtk-x11-2.0
	PATHS /opt/gtk/lib
			/opt/gnome/lib
			/usr/lib
			/usr/local/lib
	NO_DEFAULT_PATH
)
mark_as_advanced( GTK2_gtk_LIBRARY)

if( NOT GTK2_gtk_LIBRARY)
	message( STATUS "Unable to find GTK2_gtk_LIBRARY")
endif( NOT GTK2_gtk_LIBRARY)

# Find gdk config include directory

find_path( GTK2_gdk_CONFIG_INCLUDE_DIR gdkconfig.h
	PATHS /opt/gtk/include
			/opt/gtk/lib
			/opt/gnome/include
			/opt/gnome/lib
			/usr/include
			/usr/local/include
			/usr/lib
	PATH_SUFFIXES gtk-2.0/include gtk-2.0
	NO_DEFAULT_PATH
)
mark_as_advanced( GTK2_gdk_CONFIG_INCLUDE_DIR)

if( NOT GTK2_gdk_CONFIG_INCLUDE_DIR)
	message( STATUS "Unable to find GTK2_gdk_CONFIG_INCLUDE_DIR")
endif( NOT GTK2_gdk_CONFIG_INCLUDE_DIR)

# Find gdk library

find_library( GTK2_gdk_LIBRARY
	NAMES gdk-quartz-2.0 gdk-x11-2.0
	PATHS /opt/gtk/lib
			/opt/gnome/lib
			/usr/lib
			/usr/local/lib
	NO_DEFAULT_PATH
)
mark_as_advanced( GTK2_gdk_LIBRARY)

if( NOT GTK2_gdk_LIBRARY)
	message( STATUS "Unable to find GTK2_gdk_LIBRARY")
endif( NOT GTK2_gdk_LIBRARY)

# Find gdk_pixbuf library

find_library( GTK2_gdk_pixbuf_LIBRARY
	NAMES gdk_pixbuf-2.0 gdk_pixbuf_xlib-2.0
	PATHS /opt/gtk/lib
			/opt/gnome/lib
			/usr/lib
			/usr/local/lib
	NO_DEFAULT_PATH
)
mark_as_advanced( GTK2_gdk_pixbuf_LIBRARY)

if( NOT GTK2_gdk_pixbuf_LIBRARY)
	message( STATUS "Unable to find GTK2_gdk_pixbuf_LIBRARY")
endif( NOT GTK2_gdk_pixbuf_LIBRARY)

# Find gmodule library

find_library( GTK2_gmodule_LIBRARY
	NAMES gmodule-2.0
	PATHS /opt/gtk/lib
			/opt/gnome/lib
			/usr/lib
			/usr/local/lib
	NO_DEFAULT_PATH
)
mark_as_advanced( GTK2_gmodule_LIBRARY)

if( NOT GTK2_gmodule_LIBRARY)
	message( STATUS "Unable to find GTK2_gmodule_LIBRARY")
endif( NOT GTK2_gmodule_LIBRARY)

# Find gthread library

find_library( GTK2_gthread_LIBRARY
	NAMES gthread-2.0
	PATHS /opt/gtk/lib
			/opt/gnome/lib
			/usr/lib
			/usr/local/lib
	NO_DEFAULT_PATH
)
mark_as_advanced( GTK2_gthread_LIBRARY)

if( NOT GTK2_gthread_LIBRARY)
	message( STATUS "Unable to find GTK2_gthread_LIBRARY")
endif( NOT GTK2_gthread_LIBRARY)

# Find gobject library

find_library( GTK2_gobject_LIBRARY
	NAMES gobject-2.0
	PATHS /opt/gtk/lib
			/opt/gnome/lib
			/usr/lib
			/usr/local/lib
	NO_DEFAULT_PATH
)
mark_as_advanced( GTK2_gobject_LIBRARY)

if( NOT GTK2_gobject_LIBRARY)
	message( STATUS "Unable to find GTK2_gobject_LIBRARY")
endif( NOT GTK2_gobject_LIBRARY)

# Find cairo include directory

find_path( GTK2_cairo_INCLUDE_DIR cairo.h
	PATHS /opt/gtk/include
			/opt/gnome/include
			/usr/include
			/usr/local/include
	PATH_SUFFIXES cairo
	NO_DEFAULT_PATH
)
mark_as_advanced( GTK2_cairo_INCLUDE_DIR)

if( NOT GTK2_cairo_INCLUDE_DIR)
	message( STATUS "Unable to find GTK2_cairo_INCLUDE_DIR")
endif( NOT GTK2_cairo_INCLUDE_DIR)

# Find pango include directory

find_path( GTK2_pango_INCLUDE_DIR pango/pango.h
	PATHS /opt/gtk/include
			/opt/gnome/include
			/usr/include
			/usr/local/include
	PATH_SUFFIXES pango-1.0
	NO_DEFAULT_PATH
)
mark_as_advanced( GTK2_pango_INCLUDE_DIR)

if( NOT GTK2_pango_INCLUDE_DIR)
	message( STATUS "Unable to find GTK2_pango_INCLUDE_DIR")
endif( NOT GTK2_pango_INCLUDE_DIR)

# Find atk include directory

find_path( GTK2_atk_INCLUDE_DIR atk/atk.h
	PATHS /opt/gtk/include
			/opt/gnome/include
			/usr/include
			/usr/local/include
	PATH_SUFFIXES atk-1.0
	NO_DEFAULT_PATH
)
mark_as_advanced( GTK2_atk_INCLUDE_DIR)

if( NOT GTK2_atk_INCLUDE_DIR)
	message( STATUS "Unable to find GTK2_atk_INCLUDE_DIR")
endif( NOT GTK2_atk_INCLUDE_DIR)

# Find atk library

find_library( GTK2_atk_LIBRARY
	NAMES atk-1.0
	PATHS /opt/gtk/lib
			/opt/gnome/lib
			/usr/lib
			/usr/local/lib
	NO_DEFAULT_PATH
)
mark_as_advanced( GTK2_atk_LIBRARY)

if( NOT GTK2_atk_LIBRARY)
	message( STATUS "Unable to find GTK2_atk_LIBRARY")
endif( NOT GTK2_atk_LIBRARY)

if( GTK2_gtk_INCLUDE_DIR AND GLIB_INCLUDE_DIR AND
	 GTK2_gdk_CONFIG_INCLUDE_DIR AND
	 GTK2_gtk_LIBRARY AND GLIB_LIBRARY AND GTK2_cairo_INCLUDE_DIR AND
	 GTK2_pango_INCLUDE_DIR AND GTK2_gdk_LIBRARY AND GTK2_atk_INCLUDE_DIR)
	 
	message( STATUS "Found GTK2")
	set( GTK2_FOUND "YES" )
	
	set( GTK2_INCLUDE_DIRS ${GTK2_gtk_INCLUDE_DIR}
			${GLIB_CONFIG_INCLUDE_DIR}
			${GLIB_INCLUDE_DIR}
			${GTK2_cairo_INCLUDE_DIR}
			${GTK2_pango_INCLUDE_DIR}
			${GTK2_gdk_CONFIG_INCLUDE_DIR}
			${GTK2_atk_INCLUDE_DIR}
		)

	set( GTK2_LIBRARIES ${GTK2_gtk_LIBRARY}
			${GTK2_gdk_LIBRARY}
			${GLIB_LIBRARY} 
		)

	if( GTK2_gmodule_LIBRARY)
		SET( GTK2_LIBRARIES ${GTK2_LIBRARIES} ${GTK2_gmodule_LIBRARY})
	endif( GTK2_gmodule_LIBRARY)
	
	if( GTK2_gthread_LIBRARY)
		SET( GTK2_LIBRARIES ${GTK2_LIBRARIES} ${GTK2_gthread_LIBRARY})
	endif( GTK2_gthread_LIBRARY)

	if( GTK2_gobject_LIBRARY)
		SET( GTK2_LIBRARIES ${GTK2_LIBRARIES} ${GTK2_gobject_LIBRARY})
	endif( GTK2_gobject_LIBRARY)
	
	if( GTK2_gdk_pixbuf_LIBRARY)
		SET( GTK2_LIBRARIES ${GTK2_LIBRARIES} ${GTK2_gdk_pixbuf_LIBRARY})
	endif( GTK2_gdk_pixbuf_LIBRARY)
	
	if( GTK2_atk_LIBRARY)
		SET( GTK2_LIBRARIES ${GTK2_LIBRARIES} ${GTK2_atk_LIBRARY})
	endif( GTK2_atk_LIBRARY)
	
endif( GTK2_gtk_INCLUDE_DIR AND GLIB_INCLUDE_DIR AND
	 GTK2_gdk_CONFIG_INCLUDE_DIR AND
	 GTK2_gtk_LIBRARY AND GLIB_LIBRARY AND GTK2_cairo_INCLUDE_DIR AND
	 GTK2_pango_INCLUDE_DIR AND GTK2_gdk_LIBRARY AND GTK2_atk_INCLUDE_DIR)

if( GTK2_FOUND)
	if( NOT GTK2_FIND_QUIETLY)
		message( STATUS "Found GTK2 libraries: ${GTK2_LIBRARIES}")
		message( STATUS "Found GTK2 inc dirs: ${GTK2_INCLUDE_DIRS}")
	endif( NOT GTK2_FIND_QUIETLY)
else( GTK2_FOUND)
	if( GTK2_FIND_REQUIRED)
		message( FATAL_ERROR "Could not find GTK2")
	else( GTK2_FIND_REQUIRED)
		if( NOT GTK2_FIND_QUIETLY)
			message( STATUS "Could not find GTK2")
		endif( NOT GTK2_FIND_QUIETLY)
	endif( GTK2_FIND_REQUIRED)
endif( GTK2_FOUND)

# VISIT: Should set the following defines
#  -DG_DISABLE_DEPRECATED \
#  -DGDK_DISABLE_DEPRECATED \
#  -DGDK_PIXBUF_DISABLE_DEPRECATED \
#  -DGNOME_DISABLE_DEPRECATED

