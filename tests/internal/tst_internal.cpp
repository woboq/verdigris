/****************************************************************************
 *  Copyright (C) 2016-2018 Woboq GmbH
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

// Just some unit test for internal functions

#include <wobjectimpl.h>
#include <QtTest/QtTest>

namespace w_internal {

constexpr auto operator==(StringView a, StringView b) {
    if (a.size() != b.size()) return false;
    for (auto i = 0u; i < a.size() && i < b.size(); i++) if (a.b[i] != b.b[i]) return false;
    return true;
}

static_assert (countParsedLiterals(" a ")==1, "");
static_assert (countParsedLiterals(" a, b ")==2, "");
static_assert (countParsedLiterals(",")==0, "");

static_assert (viewParsedLiterals<2>("H, el")[0] == viewLiteral("H"), "");
static_assert (viewParsedLiterals<2>("H, el")[1] == viewLiteral("el"), "");

static_assert (viewParsedLiterals<2>(" c H , , el")[0] == viewLiteral("c H"), "");
static_assert (viewParsedLiterals<2>(" H , , el")[1] == viewLiteral("el"), "");

static_assert (viewScopedLiterals<1>("one")[0] == viewLiteral("one"), "");
static_assert (viewScopedLiterals<1>("::two")[0] == viewLiteral("two"), "");
static_assert (viewScopedLiterals<1>(" hallo::one ")[0] == viewLiteral("one"), "");
static_assert (viewScopedLiterals<1>("x::h:: two ")[0] == viewLiteral("two"), "");

}

namespace testEnum {
    enum ME1 {};
    enum class ME2 {};
    static_assert(w_internal::EnumIsScoped<ME1>::Value == 0, "");
    static_assert(w_internal::EnumIsScoped<ME2>::Value == 2, "");
}

class tst_Internal : public QObject
{
    W_OBJECT(tst_Internal)

private slots:
};

W_OBJECT_IMPL(tst_Internal)

QTEST_MAIN(tst_Internal)
