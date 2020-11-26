project('gi-lunar', 'cpp', 'c')

libsources = ['glunar.c', 'glunar.h', 'jlunar.cpp', 'jlunar.h']

gnome = import('gnome')
gobj_dep = dependency('gobject-2.0')

girlib = shared_library(
	'lunar',
	sources : libsources,
	dependencies : [gobj_dep],
	install : true
)

gnome.generate_gir(
	girlib,
	sources : libsources,
	nsversion : '1.0',
	namespace : 'GILunar',
	symbol_prefix : 'gi_lunar',
	identifier_prefix : 'GILunar',
	includes : ['GObject-2.0'],
	install : true,
)
