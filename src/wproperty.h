/****************************************************************************
 *  Copyright (C) 2016 - 2019 Woboq GmbH
 *  Olivier Goffart <ogoffart at woboq.com>
 *  https://woboq.com/
 *
 *  This file is part of Verdigris: a way to use Qt without moc.
 *  https://github.com/woboq/verdigris
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as
 *  published by the Free Software Foundation, either version 3 of the
 *  License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this program.
 *  If not, see <http://www.gnu.org/licenses/>.
 */
#pragma once

#ifndef Q_MOC_RUN // don't define anything when moc is run

/// \macro W_PROPERTY(<type>, <name> [, <flags>]*)
///
/// Declare a property <name> with the type <type>.
/// The flags can be function pointers that are detected to be setter, getters, notify signal or
/// other flags. Use the macro READ, WRITE, MEMBER, ... for the flag so you can write W_PROPERTY
/// just like in a Q_PROPERTY. The only difference with Q_PROPERTY would be the semicolon before the
/// name.
/// W_PROPERTY need to be put after all the setters, getters, signals and members have been declared.
///
/// <type> can optionally be put in parentheses, if you have a type containing a comma
#define W_PROPERTY(...) W_MACRO_MSVC_EXPAND(W_PROPERTY2(__VA_ARGS__)) // expands the READ, WRITE, and other sub marcos
#define W_PROPERTY2(TYPE, NAME, ...) \
    W_STATE_APPEND(PropertyState, \
        w_internal::makeMetaPropertyInfo<W_MACRO_REMOVEPAREN(TYPE)>(\
                            w_internal::viewLiteral(#NAME), w_internal::viewLiteral(W_MACRO_STRIGNIFY(W_MACRO_REMOVEPAREN(TYPE))), __VA_ARGS__))

#define WRITE , &W_ThisType::
#define READ , &W_ThisType::
#define NOTIFY , W_Notify, &W_ThisType::
#define RESET , W_Reset, &W_ThisType::
#define MEMBER , &W_ThisType::
#define CONSTANT , W_Constant
#define FINAL , W_Final

#undef Q_PRIVATE_PROPERTY // the official macro is not a variadic macro, and the coma in READ would break it
#define Q_PRIVATE_PROPERTY(...)

#else // Q_MOC_RUN
#define W_PROPERTY(...)
#endif
