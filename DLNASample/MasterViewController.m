        //
//  MasterViewController.m
//  DLNASample
//
//  Created by 健司 古山 on 12/05/06.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import "MasterViewController.h"
#import "ServerContentViewController.h"
#import "DetailViewController.h"
#import "AppDelegate.h"
#import "UPnPDeviceTableViewCell.h"

#import "RendererTableViewController.h"

//#include <cybergarage/upnp/cupnp.h>
//#include <cybergarage/xml/cxml.h>

@implementation MasterViewController

@synthesize detailViewController = _detailViewController;
@synthesize dataSource = dataSource_;
@synthesize avController = _avController;
@synthesize renderers = _renderers;

#if 0
void PrintContentDirectory(CGUpnpAction *browseAction, int indent, NSString *objectId)
{
	int n;
	
	[browseAction setArgumentValue:objectId forName:@"ObjectID"];
	[browseAction setArgumentValue:@"BrowseDirectChildren" forName:@"BrowseFlag"];
	[browseAction setArgumentValue:@"*" forName:@"Filter"];
	[browseAction setArgumentValue:@"0" forName:@"StartingIndex"];
	[browseAction setArgumentValue:@"0" forName:@"RequestedCount"];
	[browseAction setArgumentValue:@"" forName:@"SortCriteria"];
	
	if (![browseAction post])
		return;
	
	NSMutableString *indentStr = [NSMutableString string];
	for (n=0; n<indent; n++)
		[indentStr appendString:@"  "];
    
	NSString *resultStr = [browseAction argumentValueForName:@"Result"];
	NSLog(@"%@", resultStr);
    NSArray* array = [CGUpnpAvObject arrayWithXMLString:resultStr];
    
    for (CGUpnpAvObject* avObj in array) {
        NSLog(@"####%@", avObj.title);
    }
 
}
#endif

void PrintDmsInfo(CGUpnpDevice *dev, int dmsNum) 
{
	NSLog(@"[%d] %@", dmsNum, [dev friendlyName]);
    
	CGUpnpService *conDirService = [dev getServiceForType:@"urn:schemas-upnp-org:service:ContentDirectory:1"];
	if (!conDirService)
		return;
    
	CGUpnpStateVariable *searchCap = [conDirService getStateVariableForName:@"SearchCapabilities"];
	if (searchCap) {
		if ([searchCap query])
			NSLog(@"  SearchCapabilities = %@\n", [searchCap value]);
	}
    
	CGUpnpStateVariable *sorpCap = [conDirService getStateVariableForName:@"SortCapabilities"];
	if (sorpCap) {
		if ([sorpCap query])
			NSLog(@"  SortCapabilities = %@\n", [sorpCap value]);
	}
    
	CGUpnpAction *browseAction = [conDirService getActionForName:@"Browse"];
	if (!browseAction)
		return;
    
	//PrintContentDirectory(browseAction, 0, @"0");
}

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        self.title = NSLocalizedString(@"DLNA Server", @"Master");
    }
    return self;
}
							
- (void)dealloc
{
    [self.avController stop];
    [_avController release];
    [_detailViewController release];
    self.avController = nil;
    [super dealloc];
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Release any cached data, images, etc that aren't in use.
}

#pragma mark - View lifecycle

- (void)viewDidLoad
{
    [super viewDidLoad];
	// Do any additional setup after loading the view, typically from a nib.
    
    CGUpnpAvController* avCtrl = [[CGUpnpAvController alloc] init];
    avCtrl.delegate = self;
    [avCtrl search];
    self.avController = avCtrl;
    [avCtrl release];
    
    UIBarButtonItem* leftButton = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemDone target:self action:@selector(leftButtonDidPush)];
    self.navigationItem.leftBarButtonItem = leftButton;
    [leftButton release];
    
}

- (void)viewDidUnload
{
    [super viewDidUnload];
    // Release any retained subviews of the main view.
    // e.g. self.myOutlet = nil;
}

- (void)viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];
}

- (void)viewDidAppear:(BOOL)animated
{
    [super viewDidAppear:animated];
}

- (void)viewWillDisappear:(BOOL)animated
{
	[super viewWillDisappear:animated];
}

- (void)viewDidDisappear:(BOOL)animated
{
	[super viewDidDisappear:animated];
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    // Return YES for supported orientations
    return (interfaceOrientation != UIInterfaceOrientationPortraitUpsideDown);
}

#pragma mark -
- (void)leftButtonDidPush
{
    RendererTableViewController* viewController = [[RendererTableViewController alloc] initWithAvController:self.avController];
    
    [self.navigationController pushViewController:viewController animated:YES];
    [viewController release];
    
}
// Customize the number of sections in the table view.
- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView
{
    return 1;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    return [self.dataSource count];
}

// Customize the appearance of table view cells.
- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
#if 0
    static NSString *CellIdentifier = @"Cell";
    
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:CellIdentifier];
    if (cell == nil) {
        cell = [[[UITableViewCell alloc] initWithStyle:UITableViewCellStyleSubtitle reuseIdentifier:CellIdentifier] autorelease];
        cell.accessoryType = UITableViewCellAccessoryDisclosureIndicator;
    }

    // Configure the cell.
    //cell.textLabel.text = NSLocalizedString(@"Detail", @"Detail");
    CGUpnpDevice *dev = [self.dataSource objectAtIndex:indexPath.row];
    cell.textLabel.text = [dev friendlyName];
    cell.detailTextLabel.text = [dev ipaddress];
    return cell;
#else
    //NSLog(@"numberOfRowsInSection = %@", indexPath);
    
	static NSString *CELLID = @"upnprootobj";
	
	UPnPDeviceTableViewCell *cell = (UPnPDeviceTableViewCell *)[tableView dequeueReusableCellWithIdentifier:CELLID];
	if (cell == nil) {
		cell = [[[UPnPDeviceTableViewCell alloc] initWithFrame:CGRectZero reuseIdentifier:CELLID] autorelease];
	}
	
	int row = [indexPath indexAtPosition:1];
    
	
	if (row < [self.dataSource count]) {
		CGUpnpDevice *device = [self.dataSource objectAtIndex:row];
		/*
         UIImage *icon = [UIImage imageWithContentsOfFile:imagePath];
         cell.image = icon;
         */
		[cell setDevice:device];
		//[cell setText:[server friendlyName]];
		//[cell setAccessoryType:UITableViewCellAccessoryDisclosureIndicator];
	}
    
	return cell;

#endif
}

/*
// Override to support conditional editing of the table view.
- (BOOL)tableView:(UITableView *)tableView canEditRowAtIndexPath:(NSIndexPath *)indexPath
{
    // Return NO if you do not want the specified item to be editable.
    return YES;
}
*/

/*
// Override to support editing the table view.
- (void)tableView:(UITableView *)tableView commitEditingStyle:(UITableViewCellEditingStyle)editingStyle forRowAtIndexPath:(NSIndexPath *)indexPath
{
    if (editingStyle == UITableViewCellEditingStyleDelete) {
        // Delete the row from the data source.
        [tableView deleteRowsAtIndexPaths:[NSArray arrayWithObject:indexPath] withRowAnimation:UITableViewRowAnimationFade];
    } else if (editingStyle == UITableViewCellEditingStyleInsert) {
        // Create a new instance of the appropriate class, insert it into the array, and add a new row to the table view.
    }   
}
*/

/*
// Override to support rearranging the table view.
- (void)tableView:(UITableView *)tableView moveRowAtIndexPath:(NSIndexPath *)fromIndexPath toIndexPath:(NSIndexPath *)toIndexPath
{
}
*/

/*
// Override to support conditional rearranging of the table view.
- (BOOL)tableView:(UITableView *)tableView canMoveRowAtIndexPath:(NSIndexPath *)indexPath
{
    // Return NO if you do not want the item to be re-orderable.
    return YES;
}
*/

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
    //if (!self.detailViewController) {
        self.detailViewController = [[[ServerContentViewController alloc] initWithAvServer:[self.dataSource objectAtIndex:indexPath.row] atIndexPath:indexPath objectId:@"0"] autorelease];
    //}
   
    //[self.detailViewController setDetailItem:[self.dataSource objectAtIndex:indexPath.row]];
    
    [self.navigationController pushViewController:self.detailViewController animated:YES];
}

#pragma mark -
#pragma mark CGUpnpControlPointDelegate
- (void)controlPoint:(CGUpnpControlPoint *)controlPoint deviceUpdated:(NSString *)deviceUdn {
    NSLog(@"%@", deviceUdn);
    self.avController = (CGUpnpAvController*)controlPoint;
    
    //self.dataSource = [controlPoint devices];
    
    self.renderers =  [((CGUpnpAvController*)controlPoint) renderers];
//    NSArray* renderers = [((CGUpnpAvController*)controlPoint) renderers];
//    if ([renderers count] > 0) {
//        for (CGUpnpAvRenderer* renderer in renderers) {
//            NSLog(@"avRendererUDN:%@", [renderer udn]);
//        }
//    }
    
    self.dataSource = [((CGUpnpAvController*)controlPoint) servers];
    int dmsNum = 0;
//    for (CGUpnpDevice *dev in [controlPoint devices]) {
//        //NSLog(@"%@:%@", [dev friendlyName], [dev ipaddress]);
//        if ([dev isDeviceType:@"urn:schemas-upnp-org:device:MediaServer:1"]) {
////			PrintDmsInfo(dev, ++dmsNum);
//            //NSLog(@"#Server:%@", [dev deviceType]);
//
//        }
//        else if([dev isDeviceType:@"urn:schemas-upnp-org:device:MediaRenderer:1"]) {
//            NSLog(@"#Renderer%@", [dev deviceType]);
//        }
//    }
    [self.tableView reloadData];
}

- (void)controlPoint:(CGUpnpControlPoint *)controlPoint deviceAdded:(NSString *)deviceUdn
{
    NSLog(@"device added udn %@", deviceUdn); 
}

- (void)controlPoint:(CGUpnpControlPoint *)controlPoint deviceRemoved:(NSString *)deviceUdn
{
    NSLog(@"device removed udn %@", deviceUdn);
    self.dataSource = [controlPoint devices];
    [self.tableView reloadData];
}
@end
