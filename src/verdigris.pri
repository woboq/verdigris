INCLUDEPATH += $$PWD
CONFIG += c++14

# MSVC do not pre-define the feature test macro required for Qt to enable some C++14 features
msvc:DEFINES+=__cpp_constexpr=201304 __cpp_variable_templates=201304
