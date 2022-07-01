TEMPLATE = subdirs

SUBDIRS += internal inherit basic qt templates manyproperties

!gcc:SUBDIRS += cppapi
