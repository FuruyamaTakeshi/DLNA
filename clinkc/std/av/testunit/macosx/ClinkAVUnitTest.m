//
//  ClinkAVUnitTest.m
//  ClinkUnitTest
//
//  Created by Satoshi Konno on 08/07/16.
//  Copyright 2008 CyberGarage. All rights reserved.
//

#import "ClinkAVUnitTest.h"

@implementation ClinkAVUnitTest

- (void) setUp
{
	dmc = [[CGUpnpAvController alloc] init];
	[dmc search];
}
 
- (void) tearDown
{
	[dmc release];
}

#define CLINKAVUNITTEST_TESTXMLNODE_STRINGVALUE @"testValue"
#define CLINKAVUNITTEST_TESTXMLNODE_ATTRIBUTE_NAME @"testAttrName"
#define CLINKAVUNITTEST_TESTXMLNODE_ATTRIBUTE_VALUE @"testAttrValue"

- (void) testXmlNode
{
	CGXmlNode *xmlNode = [[CGXmlNode alloc] init];
	
	/* String Value */
	NSString *nodeValue = [xmlNode stringValue];
	STAssertTrue(((nodeValue == nil) || ([nodeValue length] <= 0)), @"(nodeValue == nil) || ([nodeValue length] <= 0)");
	[xmlNode setStringValue:CLINKAVUNITTEST_TESTXMLNODE_STRINGVALUE];
	nodeValue = [xmlNode stringValue];
	STAssertTrue(0 < [nodeValue length], @"0 < [nodeValue length]");
	STAssertTrue([nodeValue isEqualToString:CLINKAVUNITTEST_TESTXMLNODE_STRINGVALUE], @"[nodeValue isEqualToString:CLINKAVUNITTEST_TESTXMLNODE_STRINGVALUE]");

	/* Attribute */
	NSString *attrValue = [xmlNode attributeValueForName:CLINKAVUNITTEST_TESTXMLNODE_ATTRIBUTE_NAME];
	STAssertTrue(((attrValue == nil) || ([attrValue length] <= 0)), @"(attrValue == nil) || ([attrValue length] <= 0)");
	[xmlNode setAttributeWithName:CLINKAVUNITTEST_TESTXMLNODE_ATTRIBUTE_NAME stringValue:CLINKAVUNITTEST_TESTXMLNODE_ATTRIBUTE_VALUE];
	attrValue = [xmlNode attributeValueForName:CLINKAVUNITTEST_TESTXMLNODE_ATTRIBUTE_NAME];
	STAssertTrue(0 < [attrValue length], @"0 < [attrValue length]");
	STAssertTrue([attrValue isEqualToString:CLINKAVUNITTEST_TESTXMLNODE_ATTRIBUTE_VALUE], @"[attrValue isEqualToString:CLINKAVUNITTEST_TESTXMLNODE_ATTRIBUTE_VALUE]");

	[xmlNode release];
}

- (void) testObjectNode
{
	CGUpnpAvRoot *rootNode = [[CGUpnpAvRoot alloc] init];
	NSString *objId = [rootNode objectId];
	STAssertTrue([objId isEqualToString:@"0"], @"[objId isEqualToString:@0]");
	[rootNode release];
}

- (void) testItem
{
	NSString *CLINKAVUNITTEST_BROWSEDIRECTCHILDREN_DATA =
	@"<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
	@"<DIDL-Lite xmlns:dc=\"http://purl.org/dc/elements/1.1/\" xmlns:upnp=\"urn:schemas-upnp-org:metadata-1-0/upnp/\" xmlns=\"urn:schemas-upnp-org:metadata-1-0/DIDL-Lite/\">"
	@"	<item id=\"6\" parentID=\"3\" restricted=\"false\">"
	@"		<dc:title>Chloe Dancer</dc:title>"
	@"		<dc:creator>Mother Love Bone</dc:creator>"
	@"		<upnp:class>object.item.audioItem.musicTrack</upnp:class>"
	@"		<res protocolInfo=\"http-get:*:audio/x-ms-wma:DLNA.ORG_PN=MPEG_PS_NTSC;DLNA.ORG_OP=01;DLNA.ORG_FLAGS=AD500000000000000000000000000000\" size=\"200000\">http://10.0.0.1/getcontent.asp?id=6</res>"
	@"	</item>"
	@"	<item id=\"8\" parentID=\"3\" restricted=\"false\">"
	@"		<dc:title>Drown</dc:title>"
	@"		<dc:creator>Smashing Pumpkins</dc:creator>"
	@"		<upnp:class>object.item.audioItem.musicTrack</upnp:class>"
	@"		<res protocolInfo=\"http-get:*:audio/mpeg:*\" size=\"140000\">http://10.0.0.1/getcontent.asp?id=8</res>"
	@"	</item>"
	@"	<item id=\"7\" parentID=\"3\" restricted=\"false\">"
	@"		<dc:title>State Of Love And Trust</dc:title>"
	@"		<dc:creator>Pearl Jam</dc:creator>"
	@"		<upnp:class>object.item.audioItem.musicTrack</upnp:class>"
	@"		<res protocolInfo=\"http-get:*:audio/x-ms-wma:*\" size=\"70000\">http://10.0.0.1/getcontent.asp?id=7</res>"
	@"	</item>"
	@"</DIDL-Lite>";

	NSArray *avObjArray = [CGUpnpAvObject arrayWithXMLString:CLINKAVUNITTEST_BROWSEDIRECTCHILDREN_DATA];

	CGUpnpAvItem *avItem;
	NSString *title;
	NSString *objectId;
	NSArray *avResArray;
	CGUpnpAvResource *avRes;
	NSString *url;
	NSString *mimeType;
	NSString *dlnaOrgPn;
	NSString *dlnaOrgOp;
	NSString *dlnaOrgFlags;

	avItem = [avObjArray objectAtIndex:0];
	objectId = [avItem objectId];
	STAssertTrue([objectId isEqualToString:@"6"], @"");
	title = [avItem title];
	STAssertTrue([title isEqualToString:@"Chloe Dancer"], @"");
	avResArray = [avItem resources];
	STAssertTrue(0 < [avResArray count], @"");
	avRes = [avResArray objectAtIndex:0];
	url = [avRes url];
	STAssertTrue([url isEqualToString:@"http://10.0.0.1/getcontent.asp?id=6"], @"");
	mimeType = [avRes contentFormat];
	STAssertTrue([mimeType isEqualToString:@"audio/x-ms-wma"], @"");
	dlnaOrgPn = [avRes dlnaOrgPn];
	STAssertTrue([dlnaOrgPn isEqualToString:@"MPEG_PS_NTSC"], @"");
	dlnaOrgOp = [avRes dlnaOrgOp];
	STAssertTrue([dlnaOrgOp isEqualToString:@"01"], @"");
	dlnaOrgFlags = [avRes dlnaOrgFlags];
	STAssertTrue([dlnaOrgFlags isEqualToString:@"AD500000000000000000000000000000"], @"");

	[avObjArray release];
}

- (void) testServers
{
	NSArray *servers = [dmc servers];
	NSLog(@"[servers count] = %d", [servers count]);
	STAssertTrue(0 < [servers count], @"0 < [servers count]");
	NSLog(@"[servers objectAtIndex:0] = %@", [servers objectAtIndex:0]);
	int firstServerCnt = [servers count];
	CGUpnpAvServer *firstServer = [servers objectAtIndex:0];
	for (CGUpnpAvServer *server in servers) {
		NSLog(@"%@", [server friendlyName]);
		CGUpnpAvServer *foundServer; 
		foundServer = [dmc serverForFriendlyName:[server friendlyName]]; 
		STAssertNotNil(foundServer, nil);
		STAssertTrue(server == foundServer, @"server == foundServer");
		foundServer = [dmc serverForUDN:[server udn]]; 
		STAssertNotNil(foundServer, nil);
		STAssertTrue(server == foundServer, @"server == foundServer");
	}
	
	servers = [dmc servers];
	NSLog(@"[servers count] = %d", [servers count]);
	STAssertTrue([servers count] == firstServerCnt, @"0 < [devices count]");
	NSLog(@"[servers objectAtIndex:0] = %@", [servers objectAtIndex:0]);
	STAssertTrue([servers objectAtIndex:0] == firstServer, @"[servers objectAtIndex:0] == firstServer");
}

- (void) testBrowse
{
	NSArray *servers = [dmc servers];
	STAssertTrue(0 < [servers count], @"0 < [servers count]");
	CGUpnpAvServer *firstServer = [servers objectAtIndex:0];

	NSArray *firstBrowseSet = [firstServer browse:@"0"];
	STAssertTrue(0 < [firstBrowseSet count], @"0 < [firstBrowseSet count]");

	/* [CGUpnpAvServer objectForId] */
	CGUpnpAvObject *rootObj = [firstServer objectForId:@"0"];
	STAssertTrue([rootObj isContainer], @"[rootObj isContainer]");
	CGUpnpAvContainer *rootCon = (CGUpnpAvContainer *)rootObj;
	NSArray *rootObjChildren = [rootCon children];
	STAssertTrue([firstBrowseSet count] == [rootObjChildren count], @"[firstBrowseSet count] == [rootObjChildren count]");
	int n;
	for (n=0; n<[firstBrowseSet count]; n++) {
		CGUpnpAvObject *firstSetObj = [firstBrowseSet objectAtIndex:n];
		CGUpnpAvObject *rootChildObj = [rootObjChildren objectAtIndex:n];
		NSLog(@"[%d] firstSetObj:%@ == rootChildObj:%@", n, firstSetObj, rootChildObj);
		STAssertTrue([firstSetObj isEqual:rootChildObj], @"[firstSetObj isEqual:rootChildObj]");
	}

	/* [CGUpnpAvController objectForTitlePath] */
	NSMutableArray *pathArray = [NSMutableArray array];
	[pathArray addObject:@"/"];
	[pathArray addObject:[CGXml escapestring:[firstServer friendlyName]]];
	NSString *pathStr = [NSString pathWithComponents:pathArray];
	NSLog(@"pathStr = %@", pathStr);
	rootObj = [dmc objectForTitlePath:pathStr];
	NSLog(@"rootObj = %@", rootObj);
	STAssertNotNil(rootObj, @"rootObj != nil");
	STAssertTrue([rootObj isContainer], @"[rootObj isContainer]");
	rootCon = (CGUpnpAvContainer *)rootObj;
	rootObjChildren = [rootCon children];
	STAssertTrue([firstBrowseSet count] == [rootObjChildren count], @"[firstBrowseSet count] == [rootObjChildren count]");
	for (n=0; n<[firstBrowseSet count]; n++) {
		CGUpnpAvObject *firstSetObj = [firstBrowseSet objectAtIndex:n];
		CGUpnpAvObject *rootChildObj = [rootObjChildren objectAtIndex:n];
		NSLog(@"[%d] firstSetObj:%@ == rootChildObj:%@", n, firstSetObj, rootChildObj);
		STAssertTrue([firstSetObj isEqual:rootChildObj], @"[firstSetObj isEqual:rootChildObj]");
	}
}

@end
