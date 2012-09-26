noinst_PROGRAMS = clinkctestunit

INCLUDES = -I$(top_srcdir)/include -I../

noinst_HEADERS = \
	../ClinkTestCase.h \
	../TestDevice.h

clinkctestunit_SOURCES = \
	../ClinkTestCase.cpp \
	../ClinkTestCaseControlPoint.cpp \
	../ClinkTestCaseDevice.cpp \
	../ClinkTestCaseDictionary.cpp \
	../ClinkTestCaseHttp.cpp \
	../ClinkTestCaseNetwork.cpp \
	../ClinkTestCaseURI.cpp \
	../ClinkTestCaseSubscription.cpp \
	../ClinkTestCaseStateVariable.cpp \
	../TestDevice.c \
	../cppunitmain.cpp

#if HAVE_LIBTOOL
#clinkctestunit_LDADD = ../../lib/unix/libclinkc.la -lcppunit @XML_LIBS@
#else
clinkctestunit_LDADD = ../../lib/unix/libclinkc.a  -lcppunit @XML_LIBS@
#endif
