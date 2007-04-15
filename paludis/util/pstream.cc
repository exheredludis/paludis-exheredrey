/* vim: set sw=4 sts=4 et foldmethod=syntax : */

/*
 * Copyright (c) 2006, 2007 Ciaran McCreesh <ciaranm@ciaranm.org>
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

#include <paludis/util/log.hh>
#include <paludis/util/pstream.hh>

#include <cstring>
#include <errno.h>
#include <sys/utsname.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <grp.h>

/** \file
 * Implementation for PStream.
 *
 * \ingroup grpsystem
 */

using namespace paludis;

PStreamError::PStreamError(const std::string & our_message) throw () :
    Exception(our_message)
{
}

PStreamInBuf::int_type
PStreamInBuf::underflow()
{
    if (gptr() < egptr())
        return *gptr();

    int num_putback = gptr() - eback();
    if (num_putback > putback_size)
        num_putback = putback_size;
    std::memmove(buffer + putback_size - num_putback,
            gptr() - num_putback, num_putback);

    ssize_t n = read(fd, buffer + putback_size, buffer_size - putback_size);
    if (n <= 0)
        return EOF;

    setg(buffer + putback_size - num_putback, buffer + putback_size,
            buffer + putback_size + n);

    return *gptr();
}

PStreamInBuf::PStreamInBuf(const Command & cmd) :
    _command(cmd),
    child(fork())
{
    Context context("When running command '" + stringify(cmd.command()) + "' asynchronously:");

    if (0 == child)
    {
        std::string extras;

        if (! cmd.chdir().empty())
        {
            if (-1 == chdir(cmd.chdir().c_str()))
                throw RunCommandError("chdir failed: " + stringify(strerror(errno)));
            extras.append(" [chdir " + cmd.chdir() + "]");
        }

        for (Command::Iterator s(cmd.begin_setenvs()), s_end(cmd.end_setenvs()) ; s != s_end ; ++s)
        {
            setenv(s->first.c_str(), s->second.c_str(), 1);
            extras.append(" [setenv " + s->first + "=" + s->second + "]");
        }

        if (-1 == dup2(stdout_pipe.write_fd(), 1))
            throw PStreamError("dup2 failed");
        close(stdout_pipe.read_fd());

        if (-1 != PStream::stderr_fd)
        {
            Log::get_instance()->message(ll_debug, lc_no_context, "dup2 " +
                    stringify(PStream::stderr_fd) + " 2");

            if (-1 == dup2(PStream::stderr_fd, 2))
                throw PStreamError("dup2 failed");

            if (-1 != PStream::stderr_close_fd)
                    close(PStream::stderr_close_fd);
        }

        if (cmd.gid() && *cmd.gid() != getgid())
        {
            gid_t g(*cmd.gid());

            if (0 != ::setgid(*cmd.gid()))
                Log::get_instance()->message(ll_warning, lc_context, "setgid("
                        + stringify(*cmd.gid()) + ") failed: " + stringify(strerror(errno)));
            else if (0 != ::setgroups(1, &g))
                Log::get_instance()->message(ll_warning, lc_context, "setgroups failed: "
                        + stringify(strerror(errno)));

            extras.append(" [setgid " + stringify(*cmd.gid()) + "]");
        }

        if (cmd.uid() && *cmd.uid() != getuid())
        {
            if (0 != ::setuid(*cmd.uid()))
                Log::get_instance()->message(ll_warning, lc_context, "setuid("
                        + stringify(*cmd.uid()) + ") failed: " + stringify(strerror(errno)));
            extras.append(" [setuid " + stringify(*cmd.uid()) + "]");
        }

        std::string c(cmd.command());

        if ((! cmd.stdout_prefix().empty()) || (! cmd.stderr_prefix().empty()))
            c = LIBEXECDIR "/paludis/utils/outputwrapper --stdout-prefix '"
                + cmd.stdout_prefix() + "' --stderr-prefix '" + cmd.stderr_prefix() + "' -- " + c;

        cmd.echo_to_stderr();
        Log::get_instance()->message(ll_debug, lc_no_context, "execl /bin/sh -c " + c
                + " " + extras);
        execl("/bin/sh", "sh", "-c", c.c_str(), static_cast<char *>(0));
        throw PStreamError("execl /bin/sh -c '" + c + "' failed:"
                + stringify(strerror(errno)));
    }
    else if (-1 == child)
        throw PStreamError("fork failed: " + stringify(strerror(errno)));
    else
    {
        close(stdout_pipe.write_fd());
        fd = stdout_pipe.read_fd();
    }

    setg(buffer + putback_size, buffer + putback_size, buffer + putback_size);
}

PStreamInBuf::~PStreamInBuf()
{
    if (0 != fd)
    {
        int fdn(fd), x;
        waitpid(child, &x, 0);
        Log::get_instance()->message(ll_debug, lc_no_context,
                "pclose " + stringify(fdn) + " -> " + stringify(x));
    }
}

int
PStreamInBuf::exit_status()
{
    if (0 != fd)
    {
        int fdn(fd);
        waitpid(child, &_exit_status, 0);
        fd = 0;
        Log::get_instance()->message(ll_debug, lc_no_context,
                "manual pclose " + stringify(fdn) + " -> " + stringify(_exit_status));
    }
    return _exit_status;
}

void
PStream::set_stderr_fd(const int fd, const int fd2)
{
    _stderr_fd = fd;
    _stderr_close_fd = fd2;
}

int PStream::_stderr_fd(-1);
const int & PStream::stderr_fd(_stderr_fd);

int PStream::_stderr_close_fd(-1);
const int & PStream::stderr_close_fd(_stderr_close_fd);

