#ifndef CPP_UNIT_CDISTTESTCASE_H
#define CPP_UNIT_CDISTTESTCASE_H

#include <cppunit/extensions/HelperMacros.h>

class ClinkTestCase : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( ClinkTestCase );

  CPPUNIT_TEST( testDevice );
  /*
  CPPUNIT_TEST( testDictionary );
  CPPUNIT_TEST( testURI );
  CPPUNIT_TEST( testNetworkInterface );
  CPPUNIT_TEST( testHttpServer );
  CPPUNIT_TEST( testControlPoint );
  */
  CPPUNIT_TEST( testSubscription );
  CPPUNIT_TEST( testStateVariable );
  
  CPPUNIT_TEST_SUITE_END();

public:

	void setUp();
	void tearDown();

protected:
  
	void testDictionary();
	void testURI();
	void testNetworkInterface();
	void testHttpServer();
	void testDevice();
	void testControlPoint();
	void testSubscription();
	void testStateVariable();
};

#endif
