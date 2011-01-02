/* vim: set sw=4 sts=4 et foldmethod=syntax : */

/*
 * Copyright (c) 2011 Ciaran McCreesh
 *
 * This file is part of the Paludis package manager. Paludis is free software;
 * you can redistribute it and/or modify it under the terms of the GNU General
 * Public License version 2, as published by the Free Software Foundation.
 *
 * Paludis is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <paludis/repositories/e/e_keywords_key.hh>
#include <paludis/repositories/e/eapi.hh>

#include <paludis/util/singleton-impl.hh>
#include <paludis/util/hashes.hh>
#include <paludis/util/singleton-impl.hh>
#include <paludis/util/pimp-impl.hh>
#include <paludis/util/join.hh>
#include <paludis/util/wrapped_output_iterator.hh>
#include <paludis/util/tokeniser.hh>
#include <paludis/util/create_iterator-impl.hh>

#include <paludis/name.hh>
#include <paludis/pretty_printer.hh>
#include <paludis/call_pretty_printer.hh>

#include <tuple>
#include <unordered_map>
#include <algorithm>

using namespace paludis;
using namespace paludis::erepository;

namespace
{
    struct EKeywordsKey :
        MetadataCollectionKey<Set<KeywordName> >
    {
        const std::shared_ptr<const Set<KeywordName> > parsed_value;
        const std::shared_ptr<const EAPIMetadataVariable> variable;
        const MetadataKeyType key_type;

        EKeywordsKey(
                const std::shared_ptr<const Set<KeywordName> > & v,
                const std::shared_ptr<const EAPIMetadataVariable> & m,
                const MetadataKeyType t) :
            parsed_value(v),
            variable(m),
            key_type(t)
        {
        }

        ~EKeywordsKey()
        {
        }

        const std::shared_ptr<const Set<KeywordName> > value() const
        {
            return parsed_value;
        }

        const std::string raw_name() const
        {
            return variable->name();
        }

        const std::string human_name() const
        {
            return variable->description();
        }

        virtual MetadataKeyType type() const
        {
            return key_type;
        }

        virtual const std::string pretty_print_value(
                const PrettyPrinter & p,
                const PrettyPrintOptions &) const
        {
            return join(value()->begin(), value()->end(), " ", CallPrettyPrinter(p));
        }
    };

    typedef std::tuple<std::shared_ptr<const EAPIMetadataVariable>, std::shared_ptr<const Set<KeywordName> >, MetadataKeyType> EKeywordsKeyStoreIndex;

    long hash_set(const std::shared_ptr<const Set<KeywordName> > & v)
    {
        int result(0);
        for (auto s(v->begin()), s_end(v->end()) ;
                s != s_end ; ++s)
            result = (result << 4) ^ Hash<std::string>()(stringify(*s));
        return result;
    }

    struct EKeywordsKeyHash
    {
        std::size_t operator() (const EKeywordsKeyStoreIndex & p) const
        {
            return
                Hash<std::string>()(std::get<0>(p)->description()) ^
                std::get<0>(p)->flat_list_index() ^
                Hash<std::string>()(std::get<0>(p)->name()) ^
                hash_set(std::get<1>(p)) ^
                static_cast<int>(std::get<2>(p));
        }
    };

    struct EKeywordsKeyStoreCompare
    {
        bool operator() (const EKeywordsKeyStoreIndex & a, const EKeywordsKeyStoreIndex & b) const
        {
            return std::get<0>(a) == std::get<0>(b) && std::get<2>(a) == std::get<2>(b) &&
                std::get<1>(a)->size() == std::get<1>(b)->size() &&
                std::get<1>(a)->end() == std::mismatch(std::get<1>(a)->begin(), std::get<1>(a)->end(), std::get<1>(b)->begin()).first;
        }
    };
}

namespace paludis
{
    template <>
    struct Imp<EKeywordsKeyStore>
    {
        mutable Mutex mutex;
        mutable std::unordered_map<EKeywordsKeyStoreIndex, std::shared_ptr<const EKeywordsKey>, EKeywordsKeyHash, EKeywordsKeyStoreCompare> store;
    };
}

EKeywordsKeyStore::EKeywordsKeyStore() :
    Pimp<EKeywordsKeyStore>()
{
}

EKeywordsKeyStore::~EKeywordsKeyStore() = default;

const std::shared_ptr<const MetadataCollectionKey<Set<KeywordName> > >
EKeywordsKeyStore::fetch(
        const std::shared_ptr<const EAPIMetadataVariable> & v,
        const std::string & s,
        const MetadataKeyType t) const
{
    Lock lock(_imp->mutex);

    auto k(std::make_shared<Set<KeywordName> >());
    tokenise_whitespace(s, create_inserter<KeywordName>(k->inserter()));

    EKeywordsKeyStoreIndex x(v, k, t);
    auto i(_imp->store.find(x));
    if (i == _imp->store.end())
        i = _imp->store.insert(std::make_pair(x, std::make_shared<const EKeywordsKey>(k, v, t))).first;
    return i->second;
}

