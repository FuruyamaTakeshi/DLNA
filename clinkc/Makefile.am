SUBDIRS = include lib sample 
#SUBDIRS = include lib sample testunit

devdocsdir = $(datadir)/doc/clinkc-dev
docsdir = $(datadir)/doc/clinkc0

EXTRA_DIST = debian/changelog \
	debian/clinkc0.install \
	debian/clinkc-dev.install \
	debian/compat \
	debian/control \
	debian/copyright \
	debian/rules \
	debian/docs \
	include/cybergarage/upnp/cupnp.h.in

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = clinkc.pc

dist_docs_DATA = \
	COPYING

dist_devdocs_DATA = \
	ChangeLog \
	COPYING \
	Doxyfile

nobase_devdocs_DATA = \
	doxygen/html/* \
	doxygen-objc/html/*

doxygen/html/*:
	cd $(srcdir) && doxygen Doxyfile
# scp -r doxygen skonno,clinkc@frs.sourceforge.net:/home/groups/c/cl/clinkc/htdocs

doxygen-objc/html/*:
	cd $(srcdir) && doxygen Doxyfile.objc
# scp -r doxygen-objc skonno,clinkobjc@frs.sourceforge.net:/home/groups/c/cl/clinkobjc/htdocs

deb: dist
	-mkdir $(top_builddir)/debian-build
	-cp $(top_builddir)/../$(PACKAGE)*.orig.tar.gz $(top_builddir)/debian-build/
	cd $(top_builddir)/debian-build && tar zxf ../$(top_builddir)/$(PACKAGE)-$(VERSION).tar.gz
	cd $(top_builddir)/debian-build/$(PACKAGE)-$(VERSION) && dpkg-buildpackage -rfakeroot -sa -D
	-rm -rf $(top_builddir)/debian-build/$(PACKAGE)-$(VERSION)
