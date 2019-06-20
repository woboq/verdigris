TEMPLATE = subdirs

SUBDIRS += internal basic qt templates manyproperties

equals(QT_MAJOR_VERSION, 5):greaterThan(QT_MINOR_VERSION, 11): SUBDIRS += cppapi
greaterThan(QT_MAJOR_VERSION, 5): SUBDIRS += cppapi
