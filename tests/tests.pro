TEMPLATE = subdirs

SUBDIRS += internal basic qt templates manyproperties

!gcc:SUBDIRS += cppapi
