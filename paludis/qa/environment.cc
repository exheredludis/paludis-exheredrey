/* vim: set sw=4 sts=4 et foldmethod=syntax : */

/*
 * Copyright (c) 2006 Ciaran McCreesh <ciaran.mccreesh@blueyonder.co.uk>
 * Copyright (c) 2006 David Morgan <david.morgan@wadham.oxford.ac.uk>
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

#include <paludis/qa/environment.hh>
#include <paludis/util/collection_concrete.hh>
#include <map>

using namespace paludis;
using namespace paludis::qa;

QAEnvironment::QAEnvironment(const FSEntry & base) :
    Environment(PackageDatabase::Pointer(new PackageDatabase(this)))
{
    AssociativeCollection<std::string, std::string>::Pointer keys(
            new AssociativeCollection<std::string, std::string>::Concrete);

    keys->insert("format", "portage");
    keys->insert("importace", "1");
    keys->insert("location", stringify(base));
    keys->insert("cache", "/var/empty");
    keys->insert("profile", stringify(base / "profiles" / "base"));

    package_database()->add_repository(
            RepositoryMaker::get_instance()->find_maker("portage")(this,
            package_database().raw_pointer(), keys));
}

QAEnvironment::~QAEnvironment()
{
}

bool
QAEnvironment::query_use(const UseFlagName &, const PackageDatabaseEntry *) const
{
    return false;
}

bool
QAEnvironment::accept_keyword(const KeywordName &, const PackageDatabaseEntry * const) const
{
    return false;
}

bool
QAEnvironment::accept_license(const std::string &, const PackageDatabaseEntry * const) const
{
    return false;
}

bool
QAEnvironment::query_user_masks(const PackageDatabaseEntry &) const
{
    return false;
}

bool
QAEnvironment::query_user_unmasks(const PackageDatabaseEntry &) const
{
    return false;
}

namespace
{
    static const std::multimap<std::string, std::string> qa_environment_mirrors;
}

QAEnvironment::MirrorIterator
QAEnvironment::begin_mirrors(const std::string &) const
{
    return MirrorIterator(qa_environment_mirrors.begin());
}

QAEnvironment::MirrorIterator
QAEnvironment::end_mirrors(const std::string &) const
{
    return MirrorIterator(qa_environment_mirrors.end());
}

UseFlagNameCollection::Pointer
QAEnvironment::query_enabled_use_matching(const std::string &, const PackageDatabaseEntry *) const
{
    return UseFlagNameCollection::Pointer(new UseFlagNameCollection::Concrete);
}

