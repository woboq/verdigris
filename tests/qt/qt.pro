TEMPLATE = subdirs

# Tests that have object with too many methods reaches the template recursion limit with MSVC
!msvc:SUBDIRS += qmetaobject qobject qmetamethod
SUBDIRS += qmetaproperty
