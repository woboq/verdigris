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

//static_assert(std::is_same<decltype(viewValidLiterals()), StringViewArray<>>::value, "");
constexpr auto vl1 = viewValidLiterals("H", "el");
static_assert(std::is_same<std::decay_t<decltype(vl1)>, StringViewArray<2>>::value, "");
static_assert (vl1[0].size() == 1, "");
static_assert (vl1[1].size() == 2, "");
static_assert(vl1[0].b[0] == 'H', "");
static_assert(vl1[0].b[1] == '\0', "");
static_assert(std::is_same<decltype(viewValidLiterals("H", "", "el")), StringViewArray<1>>::value, "");
static_assert(viewValidLiterals("H", "", "el")[0].b[0] == 'H', "");
static_assert(viewValidLiterals("H", "", "el")[0].b[1] == '\0', "");

static_assert(std::is_same<decltype(viewValidTails()), StringViewArray<>>::value, "");
static_assert(viewValidTails<2,1,3>("H", "", "el")[0].b[1] == '\0', "");

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
    void removedScope() {
        QCOMPARE(w_internal::removedScopeSize("foo"), int(sizeof("foo")));
        QCOMPARE(w_internal::removedScopeSize("::foo"), int(sizeof("foo")));
        QCOMPARE(w_internal::removedScopeSize("hallo::fo"), int(sizeof("fo")));
        QCOMPARE(w_internal::removedScopeSize("x::hallo::fo"), int(sizeof("fo")));

        using namespace w_internal;
        QCOMPARE(QByteArray(w_internal::viewValidTails<w_internal::removedScopeSize("foo")>("foo")[0].b),  QByteArray("foo"));
        QCOMPARE(QByteArray(W_PARAM_TOSTRING_REMOVE_SCOPE(::foo)[0].b), QByteArray("foo"));
        QCOMPARE(QByteArray(W_PARAM_TOSTRING_REMOVE_SCOPE(::foo, hallo::fo)[1].b), QByteArray("fo"));
        QCOMPARE(QByteArray(W_PARAM_TOSTRING_REMOVE_SCOPE(::foo, hallo::fo, x::hallo::fo)[2].b), QByteArray("fo"));
    }
    W_SLOT(removedScope)

};

W_OBJECT_IMPL(tst_Internal)

QTEST_MAIN(tst_Internal)
