TEMPLATE = subdirs

SUBDIRS = \
	src \
	demo \
	example

demo.depends = src
example.depends = src
