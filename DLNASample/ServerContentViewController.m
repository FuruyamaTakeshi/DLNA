//
//  ServerContentViewController.m
//  DLNASample
//
//  Created by 田中 浩一 on 12/06/27.
//  Copyright (c) 2012 エヌエスプランニング株式会社. All rights reserved.
//

#import "ServerContentViewController.h"

#import <CyberLink/UPnPAV.h>
#import "NDLUtility.h"
#import "DLNAPlaybackViewController.h"

@implementation ServerContentViewController

@synthesize server = _server;
@synthesize dataSource = _dataSource;

- (void)dealloc 
{
    self.server = nil;
    [super dealloc];
}

- (id)initWithStyle:(UITableViewStyle)style
{
    self = [super initWithStyle:style];
    if (self) {
        // Custom initialization
    }
    return self;
}


- (id)initWithAvServer:(CGUpnpAvServer*)aServer atIndexPath:(NSIndexPath*)aIndexPath objectId:(NSString *)anObjectId
{
    self = [super init];
    if (self) {
        // Custom initialization
        self.server = (CGUpnpAvServer*)aServer;
        self.dataSource = [self.server browseDirectChildren:anObjectId];
    }
    return self;
}
- (void)didReceiveMemoryWarning
{
    // Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
    
    // Release any cached data, images, etc that aren't in use.
}

#pragma mark - View lifecycle

- (void)viewDidLoad
{
    [super viewDidLoad];

    // Uncomment the following line to preserve selection between presentations.
    // self.clearsSelectionOnViewWillAppear = NO;
 
    // Uncomment the following line to display an Edit button in the navigation bar for this view controller.
    // self.navigationItem.rightBarButtonItem = self.editButtonItem;
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
    return (interfaceOrientation == UIInterfaceOrientationPortrait);
}

#pragma mark - Table view data source

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView
{
    // Return the number of sections.
    return 1;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    // Return the number of rows in the section.
    return [self.dataSource count];
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    static NSString *CellIdentifier = @"Cell";
    
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:CellIdentifier];
    if (cell == nil) {
        cell = [[[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:CellIdentifier] autorelease];
    }
    CGUpnpAvObject* avObj = [self.dataSource objectAtIndex:indexPath.row];
    cell.textLabel.text = avObj.title;
    //cell.imageView.image = avObj.thumbnailImage;
    
    // Configure the cell...
    if ([avObj isItem]) {
        CGUpnpAvItem* avItem;
        avItem = (CGUpnpAvItem*)avObj;
        NSURL* url = [NSURL URLWithString:avItem.thumbnailUrl];
        NSData* imgData = [NSData dataWithContentsOfURL:url];
        cell.imageView.image = [UIImage imageWithData:imgData];
    }
    
    return cell;
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
        // Delete the row from the data source
        [tableView deleteRowsAtIndexPaths:[NSArray arrayWithObject:indexPath] withRowAnimation:UITableViewRowAnimationFade];
    }   
    else if (editingStyle == UITableViewCellEditingStyleInsert) {
        // Create a new instance of the appropriate class, insert it into the array, and add a new row to the table view
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

#pragma mark - Table view delegate

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
    // Navigation logic may go here. Create and push another view controller.
    
    CGUpnpAvObject* avObject = [self.dataSource objectAtIndex:indexPath.row];
    UIViewController *detailViewController;
    if ([avObject isItem]) {
        CGUpnpAvItem* avItem = (CGUpnpAvItem*)avObject;
        CGUpnpAvResource* avResorce = [avItem resource];
        NSLog(@"resourceUrl=%@, resorce:%@", [avItem.resourceUrl description], [avResorce protocolInfo]);
        NSLog(@"title1 %@", [avItem title]);
        detailViewController = [[DLNAPlaybackViewController alloc] initWithAvItem:avItem];        
    }
    else {
      detailViewController = [[ServerContentViewController alloc] initWithAvServer:self.server atIndexPath:indexPath objectId:avObject.objectId];
    }                                                                                 
     // ...
     // Pass the selected object to the new view controller.
     [self.navigationController pushViewController:detailViewController animated:YES];
     [detailViewController release];
     
}

@end
