ifdef(`__gnu__',`',`errprint(`This is not GNU m4...
')m4exit(1)') include(`misc/generated-file.txt')

dnl vim: set ft=m4 noet :

define(`filelist', `')dnl
define(`testlist', `')dnl
define(`headerlist', `')dnl
define(`selist', `')dnl
define(`secleanlist', `')dnl
define(`seheaderlist', `')dnl
define(`srlist', `')dnl
define(`srcleanlist', `')dnl
define(`srheaderlist', `')dnl
define(`testscriptlist', `')dnl
define(`addtest', `define(`testlist', testlist `$1_TEST')dnl
$1_TEST_SOURCES = $1_TEST.cc
$1_TEST_LDADD = \
	ihateautomake.o \
	$(top_builddir)/paludis/util/test_extras.o \
	$(top_builddir)/test/libtest.a \
	$(top_builddir)/paludis/environments/test/libpaludistestenvironment.la \
	$(top_builddir)/paludis/repositories/fake/libpaludisfakerepository.la \
	$(top_builddir)/paludis/repositories/virtuals/libpaludisvirtualsrepository.la \
	libpaludis.la \
	$(top_builddir)/paludis/util/libpaludisutil.la \
	$(DYNAMIC_LD_LIBS)
$1_TEST_CXXFLAGS = -I$(top_srcdir) $(AM_CXXFLAGS)
')dnl
define(`addtestscript', `define(`testscriptlist', testscriptlist `$1_TEST_setup.sh $1_TEST_cleanup.sh')')dnl
define(`addhh', `define(`filelist', filelist `$1.hh')define(`headerlist', headerlist `$1.hh')')dnl
define(`addfwd', `define(`filelist', filelist `$1-fwd.hh')define(`headerlist', headerlist `$1-fwd.hh')')dnl
define(`addhhx', `define(`filelist', filelist `$1.hh')')dnl
define(`addcc', `define(`filelist', filelist `$1.cc')')dnl
define(`addimpl', `define(`filelist', filelist `$1-impl.hh')define(`headerlist', headerlist `$1-impl.hh')')dnl
define(`addsr', `define(`srlist', srlist `$1.sr')dnl
define(`srcleanlist', srcleanlist `$1-sr.hh $1-sr.cc')dnl
define(`srheaderlist', srheaderlist `$1-sr.hh')dnl
$1-sr.hh : $1.sr $(top_srcdir)/misc/make_sr.bash
	if ! $(top_srcdir)/misc/make_sr.bash --header $`'(srcdir)/$1.sr > $`'@ ; then rm -f $`'@ ; exit 1 ; fi

$1-sr.cc : $1.sr $(top_srcdir)/misc/make_sr.bash
	if ! $(top_srcdir)/misc/make_sr.bash --source $`'(srcdir)/$1.sr > $`'@ ; then rm -f $`'@ ; exit 1 ; fi

')dnl
define(`addse', `define(`selist', selist `$1.se')dnl
define(`secleanlist', secleanlist `$1-se.hh $1-se.cc')dnl
define(`seheaderlist', seheaderlist `$1-se.hh')dnl
$1-se.hh : $1.se $(top_srcdir)/misc/make_se.bash
	if ! $(top_srcdir)/misc/make_se.bash --header $`'(srcdir)/$1.se > $`'@ ; then rm -f $`'@ ; exit 1 ; fi

$1-se.cc : $1.se $(top_srcdir)/misc/make_se.bash
	if ! $(top_srcdir)/misc/make_se.bash --source $`'(srcdir)/$1.se > $`'@ ; then rm -f $`'@ ; exit 1 ; fi

')dnl
define(`addthis', `dnl
ifelse(`$2', `hh', `addhh(`$1')', `')dnl
ifelse(`$2', `fwd', `addfwd(`$1')', `')dnl
ifelse(`$2', `hhx', `addhhx(`$1')', `')dnl
ifelse(`$2', `cc', `addcc(`$1')', `')dnl
ifelse(`$2', `sr', `addsr(`$1')', `')dnl
ifelse(`$2', `se', `addse(`$1')', `')dnl
ifelse(`$2', `impl', `addimpl(`$1')', `')dnl
ifelse(`$2', `test', `addtest(`$1')', `')dnl
ifelse(`$2', `testscript', `addtestscript(`$1')', `')')dnl
define(`add', `addthis(`$1',`$2')addthis(`$1',`$3')addthis(`$1',`$4')dnl
addthis(`$1',`$5')addthis(`$1',`$6')addthis(`$1',`$7')addthis(`$1',`$8')')dnl

AM_CXXFLAGS = -I$(top_srcdir) @PALUDIS_CXXFLAGS@ @PALUDIS_CXXFLAGS_VISIBILITY@

include(`paludis/files.m4')

CLEANFILES = *~ gmon.out *.gcov *.gcno *.gcda ihateautomake.cc ihateautomake.o
MAINTAINERCLEANFILES = Makefile.in Makefile.am about.hh paludis.hh
DISTCLEANFILES = srcleanlist secleanlist
DEFS= \
	-DSYSCONFDIR=\"$(sysconfdir)\" \
	-DLIBEXECDIR=\"$(libexecdir)\" \
	-DDATADIR=\"$(datadir)\" \
	-DLIBDIR=\"$(libdir)\" \
	-DPYTHONINSTALLDIR=\"$(PYTHON_INSTALL_DIR)\"
EXTRA_DIST = about.hh.in Makefile.am.m4 paludis.hh.m4 files.m4 \
	testscriptlist srlist srcleanlist selist secleanlist \
	repository_blacklist.txt hooker.bash
SUBDIRS = distributions fetchers syncers util selinux . repositories environments args
BUILT_SOURCES = srcleanlist secleanlist

libpaludis_la_SOURCES = filelist
libpaludis_la_LDFLAGS = -version-info @VERSION_LIB_CURRENT@:@VERSION_LIB_REVISION@:0 $(PTHREAD_LIBS)

libpaludispythonhooks_la_SOURCES = python_hooks.cc
libpaludispythonhooks_la_CXXFLAGS = $(AM_CXXFLAGS) \
	@PALUDIS_CXXFLAGS_NO_STRICT_ALIASING@ \
	@PALUDIS_CXXFLAGS_NO_WSHADOW@ \
	-I@PYTHON_INCLUDE_DIR@
libpaludispythonhooks_la_LDFLAGS = -version-info @VERSION_LIB_CURRENT@:@VERSION_LIB_REVISION@:0 @BOOST_PYTHON_LIB@ -lpython@PYTHON_VERSION@
libpaludispythonhooks_la_LIBADD = libpaludis.la

libpaludismanpagethings_la_SOURCES = name.cc

libpaludissohooks_TEST_la_SOURCES = sohooks_TEST.cc

# -rpath to force shared library
libpaludissohooks_TEST_la_LDFLAGS = -rpath /nowhere -version-info @VERSION_LIB_CURRENT@:@VERSION_LIB_REVISION@:0

libpaludissohooks_TEST_la_LIBADD = libpaludis.la

libpaludis_la_LIBADD = \
	$(top_builddir)/paludis/selinux/libpaludisselinux.la \
	$(top_builddir)/paludis/util/libpaludisutil.la \
	@DYNAMIC_LD_LIBS@ \
	$(PTHREAD_LIBS)

libpaludismanpagethings_la_LIBADD = \
	$(top_builddir)/paludis/util/libpaludisutil.la

dep_list_TEST_SOURCES += dep_list_TEST.hh
define(`testlist', testlist `dep_list_TEST_blockers')dnl
dep_list_TEST_blockers_SOURCES = dep_list_TEST_blockers.cc dep_list_TEST.hh
dep_list_TEST_blockers_LDADD = \
	ihateautomake.o \
	$(top_builddir)/paludis/util/test_extras.o \
	$(top_builddir)/test/libtest.a \
	$(top_builddir)/paludis/environments/test/libpaludistestenvironment.la \
	$(top_builddir)/paludis/repositories/fake/libpaludisfakerepository.la \
	$(top_builddir)/paludis/repositories/virtuals/libpaludisvirtualsrepository.la \
	libpaludis.la \
	$(top_builddir)/paludis/util/libpaludisutil.la \
	$(DYNAMIC_LD_LIBS)
dep_list_TEST_blockers_CXXFLAGS = -I$(top_srcdir) $(AM_CXXFLAGS)

TESTS = testlist

check_PROGRAMS = $(TESTS)
check_SCRIPTS = testscriptlist
check_LTLIBRARIES = libpaludissohooks_TEST.la

paludis_datadir = $(datadir)/paludis
paludis_data_DATA = repository_blacklist.txt

paludis_libexecdir = $(libexecdir)/paludis
paludis_libexec_SCRIPTS = hooker.bash

if MONOLITHIC

noinst_LTLIBRARIES = libpaludis.la libpaludismanpagethings.la

else

lib_LTLIBRARIES = libpaludis.la
noinst_LTLIBRARIES = libpaludismanpagethings.la

if ENABLE_PYTHON_HOOKS
lib_LTLIBRARIES += libpaludispythonhooks.la
endif

endif


paludis_includedir = $(includedir)/paludis-$(PALUDIS_PC_SLOT)/paludis/
paludis_include_HEADERS = headerlist srheaderlist seheaderlist

Makefile.am : Makefile.am.m4 files.m4
	$(top_srcdir)/misc/do_m4.bash Makefile.am

paludis.hh : paludis.hh.m4 files.m4
	$(top_srcdir)/misc/do_m4.bash paludis.hh

comparison_policy.hh : comparison_policy.hh.m4
	$(top_srcdir)/misc/do_m4.bash comparison_policy.hh.m4

ihateautomake.cc : all
	test -f $@ || touch $@

changequote(`<', `>')
built-sources : $(BUILT_SOURCES)
	for s in `echo $(SUBDIRS) | tr -d .` ; do $(MAKE) -C $$s built-sources || exit 1 ; done

DISTCHECK_DEPS = libpaludis.la

distcheck-deps : $(DISTCHECK_DEPS) distcheck-deps-subdirs

distcheck-deps-subdirs :
	for s in `echo $(SUBDIRS) | tr -d .` ; do $(MAKE) -C $$s distcheck-deps || exit 1 ; done


TESTS_ENVIRONMENT = env \
	PALUDIS_EBUILD_DIR="$(top_srcdir)/paludis/repositories/e/ebuild/" \
	PALUDIS_EAPIS_DIR="$(top_srcdir)/paludis/repositories/e/eapis/" \
	PALUDIS_DISTRIBUTIONS_DIR="$(top_srcdir)/paludis/distributions/" \
	PALUDIS_DISTRIBUTION="gentoo" \
	PALUDIS_HOOKER_DIR="$(top_srcdir)/paludis/" \
	PALUDIS_OUTPUTWRAPPER_DIR="`$(top_srcdir)/paludis/repositories/e/ebuild/utils/canonicalise $(top_builddir)/paludis/util/`" \
	PALUDIS_SKIP_CONFIG="yes" \
	PALUDIS_REPOSITORY_SO_DIR="$(top_builddir)/paludis/repositories" \
	TEST_SCRIPT_DIR="$(srcdir)/" \
	SO_SUFFIX=@VERSION_LIB_CURRENT@ \
	PYTHONPATH="$(top_builddir)/python/" \
	PALUDIS_PYTHON_DIR="$(top_srcdir)/python/" \
	LD_LIBRARY_PATH="`echo $$LD_LIBRARY_PATH: | sed -e 's,^:,,'`` \
		$(top_srcdir)/paludis/repositories/e/ebuild/utils/canonicalise $(top_builddir)/paludis/.libs/ \
		`:`$(top_srcdir)/paludis/repositories/e/ebuild/utils/canonicalise $(top_builddir)/paludis/util/.libs/ \
		`:`$(top_srcdir)/paludis/repositories/e/ebuild/utils/canonicalise $(top_builddir)/python/.libs/`" \
	bash $(top_srcdir)/test/run_test.sh

