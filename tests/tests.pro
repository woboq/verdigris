TEMPLATE = subdirs

SUBDIRS += internal basic qt templates manyproperties

!lessThan(QT_VERSION,5.12.0): SUBDIRS += cppapi
