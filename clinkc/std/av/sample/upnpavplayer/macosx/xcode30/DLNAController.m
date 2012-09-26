#import "DLNAController.h"
#import "DLNAImageView.h"

@implementation DLNAController

@synthesize imageView;
@synthesize browserView;

- (id)init
{
	if ((self = [super init]) == nil)
		return nil;
		
	dmc = [[CGUpnpAvController alloc] init];
	if (!dmc)
		return nil;
		
	return self;
}

- (void)dealloc
{
	[dmc release];
	[super dealloc];
}

- (void)finalize
{
	[dmc release];
	[super finalize];
}

- (void)awakeFromNib 
{
    // Make the browser user our custom browser cell.
    //[fsBrowser setCellClass: [FSBrowserCell class]];

    // Tell the browser to send us messages when it is clicked.
	[dmcBrowser setTarget:self];
	[dmcBrowser setAction:@selector(doClick:)];
	[dmcBrowser setDoubleAction:@selector(doDoubleClick:)];

	/*
    // Configure the number of visible columns (default max visible columns is 1).
    [fsBrowser setMaxVisibleColumns:MAX_VISIBLE_COLUMNS];
    [fsBrowser setMinColumnWidth:NSWidth([fsBrowser bounds])/(CGFloat)MAX_VISIBLE_COLUMNS];

    // Drag and drop support
    [fsBrowser registerForDraggedTypes:[NSArray arrayWithObject:NSFilenamesPboardType]];
    [fsBrowser setDraggingSourceOperationMask:NSDragOperationEvery forLocal:YES];
    [fsBrowser setDraggingSourceOperationMask:NSDragOperationEvery forLocal:NO];
    
    // Prime the browser with an initial load of data.
    [self reloadData:nil];
	*/
	[thumbnailView setImageScaling:NSScaleToFit];
	NSURL *imgUrl = [NSURL URLWithString:@"http://certification.dlna.org//prodimages/RDA1_RF.jpg"];
	NSImage *img = [[NSImage alloc] initWithContentsOfURL:imgUrl];
	[thumbnailView setImage:img];
	[img release];	
}

+(NSMutableString *)pathToColum:(NSBrowser *)browser numberOfRowsInColumn:(NSInteger)column
{
	[browser setPathSeparator:@"¥t"];
	NSString *pathToColum = [browser pathToColumn:column];
	NSArray *pathArray = [pathToColum componentsSeparatedByString:@"¥t"];
	NSMutableString *path = [NSMutableString string];
	for (NSString *pathStr in pathArray) {
		if ([pathStr isEqualToString:@"¥t"] || [pathStr length] <= 0)
			continue;
		[path appendString:@"/"];
		[path appendString:[CGXml escapestring:pathStr]];
	}
	return path;
}

+(NSMutableString *)selectedPath:(NSBrowser *)browser
{
	return [DLNAController pathToColum:browser numberOfRowsInColumn:[browser selectedColumn]];
}

+(NSBrowserCell *)selectedCell:(NSBrowser *)browser
{
	return [browser selectedCellInColumn:[browser selectedColumn]];
}

+(NSMutableString *)selectedItemPath:(NSBrowser *)browser
{
	NSMutableString *path = [DLNAController selectedPath:browser];
	NSCell *selectedCell = [DLNAController selectedCell:browser];	
	[path appendString:@"/"];
	[path appendString:[CGXml escapestring:[selectedCell title]]];
	return path;
}

- (IBAction)searchDMS:(id)sender 
{
	if (dmc)
		[dmc search];
	NSWindow *mainWin = [NSApp mainWindow];
	if (mainWin == nil)
		return;
	//[mainWin update];
	//[browser displayColumn:0];
	[dmcBrowser reloadColumn:0];
}

- (IBAction)itemSelected:(id)sender
{
}

- (IBAction)doClick:(id)sender
{
	NSBrowserCell *selectedCell = [DLNAController selectedCell:sender];
	if (![selectedCell isLeaf])
		return;
	
	NSString *selectedItemPath = [DLNAController selectedItemPath:sender];
}

- (IBAction)doClickImage:(id)sender
{
	NSBrowserCell *selectedCell = [DLNAController selectedCell:sender];
	if (![selectedCell isLeaf])
		return;
	
	NSString *selectedItemPath = [DLNAController selectedItemPath:sender];
}

- (IBAction)doDoubleClick:(id)sender
{
	NSBrowserCell *selectedCell = [DLNAController selectedCell:sender];
	if (![selectedCell isLeaf])
		return;
	
	NSString *selectedItemPath = [DLNAController selectedItemPath:sender];

  // Set up a videoView by hand. You can also do that in the nib file
  // videoView = [[VLCVideoView alloc] initWithFrame:[[window contentView] bounds]];
	NSWindow *mainWin = [NSApp mainWindow];
	[self setBrowserView:[mainWin contentView]];
	
	NSURL *imgUrl = [NSURL URLWithString:@"http://certification.dlna.org//prodimages/RDA1_RF.jpg"];
	NSImage *img = [[NSImage alloc] initWithContentsOfURL:imgUrl];
	DLNAImageView *imgView = [[DLNAImageView alloc] initWithFrame:[[mainWin contentView] bounds]];
	[imgView setController:self];
	[imgView setImage:img];
	[imgView setTarget:self];
	[imgView setAction:@selector(doClickImage:)];
	[mainWin setContentView:imgView];

/*
   [videoView setAutoresizingMask: NSViewHeightSizable|NSViewWidthSizable];
  
   // Init the player object
   player = [[VLCMediaPlayer alloc] initWithVideoView:videoView];
 
   [player setMedia:[VLCMedia mediaWithPath:@"/to/my/movie"]];
   [player play];	
   */
}

- (NSInteger)browser:(NSBrowser *)sender numberOfRowsInColumn:(NSInteger)column
{		
	if (column == 0) {
		NSArray *serverArray = [dmc servers];
		if ([serverArray count] <= 0) {
			[dmc search];
			serverArray = [dmc servers];
		}
		return [serverArray count];
	}

	NSString *path = [DLNAController pathToColum:sender numberOfRowsInColumn:column];

	//[NSCursor closedHandCursor];
	NSArray *avObjs = [dmc browseWithTitlePath:path];
	//[NSCursor arrowCursor];
	
	if (avObjs == nil)
		return 0;
		
	return [avObjs count];
}

- (void)browser:(NSBrowser *)sender willDisplayCell:(id)cell atRow:(NSInteger)row column:(NSInteger)column
{
	if (column == 0) {
		NSArray *serverArray = [dmc servers];
		if (row < [serverArray count]) {
			CGUpnpAvServer *server = [serverArray objectAtIndex:row];
			[cell setTitle:[server friendlyName]];
			[cell setLeaf:NO];
		}
	}

	NSString *path = [DLNAController pathToColum:sender numberOfRowsInColumn:column];
	
	CGUpnpAvObject *avObj = [dmc objectForTitlePath:path];
	if (avObj == nil)
		return;
	if (![avObj isContainer])
		return;
	CGUpnpAvContainer *avCon = (CGUpnpAvContainer *)avObj;
	CGUpnpAvObject *childObj = [avCon childAtIndex:row];
	if (childObj == nil) {
		[cell setLeaf:YES];
		return;
	}
	[cell setTitle:[childObj title]];
	if (childObj.isContainer)
		[cell setLeaf:NO];
	else if (childObj.isItem)
		[cell setLeaf:YES];
}

- (int)numberOfRowsInTableView:(NSTableView *)aTableView
{
	return 1;
}

- (id)tableView:(NSTableView *)aTableView objectValueForTableColumn:(NSTableColumn *)aTableColumn row:(int)rowIndex
{
	return @"Sample";
}

- (NSArray *)toolbarAllowedItemIdentifiers:(NSToolbar*)toolbar 
{
	return [ NSArray arrayWithObjects:
                 @"Search", 
                 nil ]; 
} 

- (NSArray *)toolbarDefaultItemIdentifiers:(NSToolbar *)toolbar 
{ 
	return [ NSArray arrayWithObjects:
                 @"Search", 
                 nil ]; 
} 

- (NSToolbarItem *)toolbar:(NSToolbar *)toolbar itemForItemIdentifier:(NSString *)itemIdentifier willBeInsertedIntoToolbar:(BOOL)flag
{
	NSToolbarItem *searchItem = [[NSToolbarItem alloc] initWithItemIdentifier:@"Search"];
	[searchItem setLabel:@"Search"];
	[searchItem setEnabled:YES];
	[searchItem setTarget:self];
	[searchItem setAction:@selector(searchDMS:)];
	return searchItem;
}

@end
