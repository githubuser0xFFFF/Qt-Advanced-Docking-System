TEMPLATE = subdirs

SUBDIRS = \
	src \
	demo \
	examples

demo.depends = src
examples.depends = src
