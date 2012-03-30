/* vim: set sw=4 sts=4 et foldmethod=syntax : */

/*
 * Copyright (c) 2012 David Leverton
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

#ifndef PALUDIS_GUARD_SRC_CLIENTS_CAVE_CMD_PRINT_CHECKSUM_ALGORITHMS_HH
#define PALUDIS_GUARD_SRC_CLIENTS_CAVE_CMD_PRINT_CHECKSUM_ALGORITHMS_HH 1

#include "command.hh"

namespace paludis
{
    namespace cave
    {
        class PALUDIS_VISIBLE PrintChecksumAlgorithmsCommand :
            public Command
        {
            public:
                virtual CommandImportance importance() const PALUDIS_ATTRIBUTE((warn_unused_result));

                int run(const std::shared_ptr<Environment> &,
                    const std::shared_ptr<const Sequence<std::string> > & args);

                std::shared_ptr<args::ArgsHandler> make_doc_cmdline();
        };
    }
}

#endif
