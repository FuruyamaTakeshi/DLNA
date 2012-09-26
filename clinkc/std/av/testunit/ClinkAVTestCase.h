#ifndef CPP_UNIT_CDISTTESTCASE_H
#define CPP_UNIT_CDISTTESTCASE_H

#include <cppunit/extensions/HelperMacros.h>

class ClinkAVTestCase : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( ClinkAVTestCase );

  CPPUNIT_TEST( testRenderer );

  CPPUNIT_TEST_SUITE_END();

public:

	void setUp();
	void tearDown();

protected:
  
	void testRenderer();

};

#endif
