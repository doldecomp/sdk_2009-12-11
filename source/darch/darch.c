#include <revolution/darch.h>

/*******************************************************************************
 * headers
 */

#include <ctype.h> // tolower
#include <string.h>

#include <macros.h>

#include <revolution/types.h>

#if 0
#include <revolution/os/OSReport.h>
#else
#include "context_rvl.h"
#endif

/*******************************************************************************
 * macros
 */

#undef NULL
#define NULL			((void *)(0))

#define DARCH_SIGNATURE	0x55aa382d // U, \xAA, 8, -

/*******************************************************************************
 * types
 */

// just use typedef here because DARCHFileInfo is in global scope
typedef struct DARCHItem DARCHItem;

struct DARCHItem
{
	DARCHItem	*nextItem;		// size 0x04, offset 0x00
	DARCHItem	*nextFile;		// size 0x04, offset 0x04
	DARCHItem	*parentDir;		// size 0x04, offset 0x08
	DARCHItem	*nextDir;		// size 0x04, offset 0x0c
	DARCHItem	*nextInOrder;	// size 0x04, offset 0x10
	char		*name;			// size 0x04, offset 0x14
	BOOL		isDir;			// size 0x04, offset 0x18
	long		id;				// size 0x04, offset 0x1c
	int			childNum;		// size 0x04, offset 0x20
}; // size 0x24

// same here
typedef struct DARCHHeader DARCHHeader;

struct DARCHHeader
{
	byte4_t	signature;		// size 0x04, offset 0x00
	u32		itemListOffset;	// size 0x04, offset 0x04
	u32		itemListSize;	// size 0x04, offset 0x08
	u32		fileDataSize;	// size 0x04, offset 0x0c
	byte4_t	at_0x10;		// size 0x04, offset 0x10
	byte4_t	at_0x14;		// size 0x04, offset 0x14
	byte4_t	at_0x18;		// size 0x04, offset 0x18
	byte4_t	at_0x1c;		// size 0x04, offset 0x1c
}; // size 0x20

/*******************************************************************************
 * local function declarations
 */

static s32 GetFirstDirCount(char **path);
static BOOL MyStrnCmp(char *s1, char *s2, u32 n);
static DARCHItem *FindMatchingItemName(DARCHItem *item, char *name,
                                       u32 nameLen);
static void AddItem(DARCHItem *parent, DARCHItem *child);
static BOOL MergeDirStructure(DARCHItem *into, DARCHItem *from);
static DARCHItem *DecideOrderInFst(DARCHItem *item, int *fileInfoCountOut,
                                   u32 *stringPoolSizeOut);
static void *__DARCHAlloc(u32 size, void *dstEndBuffer, u32 workBufferMaxSize);
static u32 __DARCHFree(void *ptr, u32 size);
static BOOL InitializeRootDir(DARCHItem *item);
static BOOL GetFileChar(u32 *stringPoolLenOut, int *itemCountOut,
                        DARCHFileInfo *fileInfos, u32 fileInfoCount);
static BOOL CreateDirStructure(DARCHItem *item, DARCHFileInfo const *fileInfo,
                               u32 fileInfoCount, void *dstStartBuffer,
                               u32 dstSize);
static BOOL ConstructFSTFromStructure(DARCHItem *items,
                                      DARCHFileInfo const *fileInfos,
                                      void *dstStartBuffer, u32 dstSize);

/*******************************************************************************
 * variables
 */

// .sbss
static u32 workBufferSize = 0;

/*******************************************************************************
 * functions
 */

static s32 GetFirstDirCount(char **path)
{
	if (**path == '/')
		++*path;

	if (**path == '\0')
		return -1;

	char *it = *path;
	while (*it != '/' && *it != '\0')
		++it;

	return it - *path;
}

// Returns true if the strings did not match within n chars; else false
static BOOL MyStrnCmp(char *s1, char *s2, u32 n)
{
	u32 m = n; // ?

	if (!n)
		return true;

	for (m = 0; m < n; ++m)
	{
		if (tolower(*s1) != tolower(*s2))
			return true;

		if (*s1 == '\0')
			return false;

		++s1;
		++s2;
	}

	return false;
}

static DARCHItem *FindMatchingItemName(DARCHItem *item, char *name, u32 nameLen)
{
	DARCHItem *it;

	for (it = item->nextFile; it != nullptr; it = it->nextItem)
	{
		if (!MyStrnCmp(it->name, name, nameLen) && strlen(it->name) == nameLen)
			return it;
	}

	for (it = item->nextDir; it != nullptr; it = it->nextItem)
	{
		if (!MyStrnCmp(it->name, name, nameLen) && strlen(it->name) == nameLen)
			return it;
	}

	return nullptr;
}

static void AddItem(DARCHItem *parent, DARCHItem *child)
{
	child->parentDir = parent;

	DARCHItem *it;
	DARCHItem **next;

	if (child->isDir)
		next = &parent->nextFile;
	else
		next = &parent->nextDir;

	if (*next == nullptr)
	{
		*next = child;
		child->nextItem = nullptr;
	}
	else
	{
		it = *next;
		while (it->nextItem)
			it = it->nextItem;

		it->nextItem = child;
		child->nextItem = nullptr;
	}
}

static BOOL MergeDirStructure(DARCHItem *into, DARCHItem *from)
{
	DARCHItem *it;
	DARCHItem *match;
	DARCHItem *next;

	u32 itemCount ATTR_UNUSED = 0;

	for (it = from->nextFile; it; it = it->nextItem)
	{
		next = it->nextItem;

		match = FindMatchingItemName(into, it->name, strlen(it->name));
		++itemCount;

		if (match == nullptr)
		{
			AddItem(into, it);
			continue;
		}

		if (match->isDir)
		{
			if (!MergeDirStructure(match, it))
				return false;
		}
		else
		{
			OSReport("DARCHCreate(): %s: file exists\n", into->name);

			return false;
		}
	}

	for (it = from->nextDir; it; it = next)
	{
		next = it->nextItem;

		match = FindMatchingItemName(into, it->name, strlen(it->name));

		if (match == nullptr)
		{
			AddItem(into, it);
			continue;
		}

		if (match->isDir)
		{
			OSReport("DARCHCreate(): %s: directory exists\n", into->name);

			return false;
		}
		else
		{
			OSReport("DARCHCreate(): %s: file exists\n", into->name);

			return false;
		}
	}

	return true;
}

static DARCHItem *DecideOrderInFst(DARCHItem *item, int *fileInfoCountOut,
                                   u32 *stringPoolSizeOut)
{
	u32 itemNamesLen;

	DARCHItem *it;
	DARCHItem *last;

	u32 itemCount;

	int fileInfoCount;
	u32 stringPoolSize;

	itemCount = 1;
	itemNamesLen = strlen(item->name) + 1;
	last = item;

	if (item->nextDir)
		it = item->nextDir;
	else
		it = item->nextFile;

	while (it)
	{
		last->nextInOrder = it;

		if (it->isDir)
		{
			last = DecideOrderInFst(it, &fileInfoCount, &stringPoolSize);

			it->childNum = fileInfoCount;
			itemCount += fileInfoCount;
			itemNamesLen += stringPoolSize;

			it = it->nextItem;
		}
		else
		{
			last = it;

			++itemCount;
			itemNamesLen += strlen(it->name) + 1;

			if (it->nextItem)
				it = it->nextItem;
			else
				it = it->parentDir->nextFile;
		}
	}

	*fileInfoCountOut = itemCount;
	*stringPoolSizeOut = itemNamesLen;

	return last;
}

static void *__DARCHAlloc(u32 size, void *dstEndBuffer, u32 workBufferMaxSize)
{
	workBufferSize += size;

	void *ptr = (byte_t *)dstEndBuffer - workBufferSize;

	if (workBufferSize >= workBufferMaxSize)
		return nullptr;
	else
		return ptr;
}

static u32 __DARCHFree(void *ptr, u32 size)
{
	byte_t *p;

	while (size)
	{
		p = (byte_t *)ptr - size;

		*p = 0;
		--size;
	}

	// Wait... this is always 0
	return size;
}

static BOOL InitializeRootDir(DARCHItem *item)
{
	// ERRATUM: Address of temporary still held at function exit
	char empty[] = "";

	item->nextItem	= nullptr;
	item->nextFile	= nullptr;
	item->parentDir	= item;

	item->name		= empty;
	item->nextDir	= nullptr;

	item->isDir		= true;
	item->id		= 0;

	return true;
}

static BOOL GetFileChar(u32 *stringPoolLenOut, int *itemCountOut,
                        DARCHFileInfo *fileInfos, u32 fileInfoCount)
{
	char mainBuf[DARCH_PATH_MAX];
	char strcmpBuf[DARCH_PATH_MAX];

	int i;
	int j;

	s32 firstDirNameLen;
	s32 curEnd;
	u32 stringPoolLen;
	int itemCount;
	char *tmp;
	char *start;
	char *pathName;
	BOOL isNewItem;

	stringPoolLen = 1;
	itemCount = 1;

	for (i = 0; i < fileInfoCount; ++i)
	{
		if (strlen(fileInfos[i].pathName) >= DARCH_PATH_MAX + 1)
		{
			OSReport(" %dth path name is too long! \n", i);

			return false;
		}

		if (strlen(fileInfos[i].pathName) == 0)
		{
			OSReport(" %dth path name is wrong! \n", i);

			return false;
		}

		tmp = fileInfos[i].pathName;
		start = fileInfos[i].pathName;

		while (tmp[0] != '\0' && (tmp[0] != '/' || tmp[1] != '\0'))
		{
			firstDirNameLen = GetFirstDirCount(&tmp);

			strcpy(mainBuf, start);

			curEnd = tmp - start;
			mainBuf[curEnd + firstDirNameLen] = '\0';
			isNewItem = true;

			for (j = 0; j < i; ++j)
			{
				pathName = fileInfos[j].pathName;

				strcpy(strcmpBuf, pathName);
				strcmpBuf[curEnd + firstDirNameLen] = '\0';

				if (strcmp(mainBuf, strcmpBuf) == 0)
				{
					isNewItem = false;
					break;
				}
			}

			if (isNewItem)
			{
				stringPoolLen += firstDirNameLen + 1;
				++itemCount;
			}

			tmp += firstDirNameLen;
		}
	}

	*stringPoolLenOut = stringPoolLen;
	*itemCountOut = itemCount;

	// I dislike doing this
	(void)&curEnd;
	(void)j;
	(void)j;

	return true;
}

static BOOL CreateDirStructure(DARCHItem *item, DARCHFileInfo const *fileInfo,
                               u32 fileInfoCount, void *dstStartBuffer,
                               u32 dstSize)
{
	int i;

	s32 firstDirCount;
	s32 stringPoolOffset;
	u32 workBufferMaxSize;

	char *path;
	void *dstEndBuffer;

	DARCHItem *tmpItem;
	DARCHItem *parent;
	DARCHItem *child;

	workBufferMaxSize = 0;
	for (i = 0; i < fileInfoCount - 1; ++i)
		workBufferMaxSize += ROUND_UP(fileInfo[i].fileSize, 32);

	workBufferMaxSize += fileInfo[fileInfoCount - 1].fileSize;
	dstEndBuffer = (byte_t *)dstStartBuffer + dstSize;

	for (i = 0; i < fileInfoCount; ++i)
	{
		OSAssert_Line(539, fileInfo[i].pathName);

		if (nullptr
		    == (tmpItem = __DARCHAlloc(sizeof *tmpItem, dstEndBuffer,
		                               workBufferMaxSize)))
		{
			return false;
		}

		InitializeRootDir(tmpItem);
		path = fileInfo[i].pathName;
		parent = tmpItem;

		while (path[0] != '\0' && (path[0] != '/' || path[1] != '\0'))
		{
			if (nullptr
			    == (child = __DARCHAlloc(sizeof *child, dstEndBuffer,
			                             workBufferMaxSize)))
				return false;

			firstDirCount = GetFirstDirCount(&path);
			if (nullptr
			    == (child->name =
			            __DARCHAlloc(strlen(fileInfo[i].pathName) + 1,
			                         dstEndBuffer, workBufferMaxSize)))
			{
				return false;
			}

			child->isDir = true;
			child->nextItem = nullptr;
			child->nextFile = nullptr;
			child->parentDir = parent;
			child->nextDir = nullptr;

			stringPoolOffset = path - fileInfo[i].pathName;

			if (nullptr == strcpy(child->name, fileInfo[i].pathName))
				return false;

			child->name[stringPoolOffset + firstDirCount] = '\0';
			child->name += stringPoolOffset;

			path += firstDirCount;

			parent->nextFile = child;

			parent = child;
		}

		parent->parentDir->nextFile = nullptr;
		parent->parentDir->nextDir = parent;
		parent->isDir = false;

		if (!MergeDirStructure(item, tmpItem))
			return false;
	}

	return true;
}

static BOOL ConstructFSTFromStructure(DARCHItem *items,
                                      DARCHFileInfo const *fileInfos,
                                      void *dstStartBuffer, u32 dstSize)
{
	/* possible ERRATUM? is address of temporary still held at function exit?
	 * this one is a lot more complex than InitializeRootDir so i'm not exactly
	 * sure
	 */
	char empty[] = "";

	DARCHItem *it;
	int i;

	u32 stringPoolSize;
	int fileInfoCount;

	int fileCount;
	u32 fullSize;
	u32 itemOffset;
	u32 dataSize;
	u32 tmpSize;
	char *start;
	char *end;
	byte_t *freePtr;
	char *data;

	items->name = empty;
	DecideOrderInFst(items, &fileInfoCount, &stringPoolSize);

	items->childNum = fileInfoCount;
	it = items;
	fileCount = 0;
	dataSize = stringPoolSize + sizeof(DARCHFileInfo) * fileInfoCount;
	itemOffset = sizeof(DARCHHeader);
	fullSize = ROUND_UP(itemOffset + dataSize, 32);
	data = (char *)dstStartBuffer + itemOffset;
	end = start = data + sizeof(DARCHFileInfo) * fileInfoCount;
	tmpSize = fullSize;

	for (i = 0; i < fileInfoCount; ++i)
	{
		it->id = i;

		// we really be reinventing the bswap wheel
		// Kids, don't do any of this in your code

		/* As a fun aside, I did find that offset_[buf_ + j] matched. Maybe I
		 * should have kept that. That might have amused someone.
		 */
#define STUFF_INTO_BUFF(buf_, offset_, type_, data_)					\
	do																	\
	{																	\
		for (int j = 0; j < (int)sizeof(type_); ++j)					\
		{																\
			*((buf_) + (offset_) + j) =									\
				(data_) >> ((((int)sizeof(type_) - j) - 1) * 8) & 0xff;	\
		}																\
	} while (false)

		{ // block to scope typedefs
			typedef u8 u8[1];
			typedef u8 u24[3];

			STUFF_INTO_BUFF(data, 0, u8, it->isDir);
			STUFF_INTO_BUFF(data, 1, u24, end - start);
		}

		strcpy(end, it->name);
		end += strlen(it->name) + 1;

		if (it->isDir)
		{
			STUFF_INTO_BUFF(data, 4, u32, it->parentDir->id);
			STUFF_INTO_BUFF(data, 8, u32, i + it->childNum);
		}
		else
		{
			STUFF_INTO_BUFF(data, 4, u32, tmpSize);
			STUFF_INTO_BUFF(data, 8, u32, fileInfos[fileCount].fileSize);

			tmpSize += ROUND_UP(fileInfos[fileCount].fileSize, 32);
			++fileCount;
		}

#undef STUFF_INTO_BUFF

		data += sizeof(DARCHFileInfo);
		it = it->nextInOrder;
	}

	freePtr = (byte_t *)dstStartBuffer + dstSize;

	__DARCHFree(freePtr, dstSize - fullSize);
	tmpSize = fullSize;

	for (i = 0; i < fileCount; ++i)
	{
		memcpy((byte_t *)dstStartBuffer + tmpSize, fileInfos[i].fileData,
		       fileInfos[i].fileSize);
		tmpSize += ROUND_UP(fileInfos[i].fileSize, 32);
	}

	DARCHHeader header;
	header.signature = DARCH_SIGNATURE;
	header.itemListOffset = itemOffset;
	header.itemListSize = dataSize;
	header.fileDataSize = fullSize;

	// Noted as always zero by wiibrew
	header.at_0x10 = 0;
	header.at_0x14 = 0;
	header.at_0x18 = 0;
	header.at_0x1c = 0;

	memcpy(dstStartBuffer, &header, sizeof header);
	workBufferSize = 0;

	// I still dislike doing this
	(void)&data;
	(void)i;

	return true;
}

u32 DARCHGetArcSize(DARCHFileInfo *fileInfos, u32 fileInfoCount)
{
	int i;
	u32 infoSize = 0;
	u32 arcSize = 0;

	u32 stringPoolLen;
	int itemCount;
	if (!GetFileChar(&stringPoolLen, &itemCount, fileInfos, fileInfoCount))
	{
		OSReport(" DARCHGetArcSize() : Error! \n");
		return 0;
	}

	u32 dataSize =
		ROUND_DOWN(63 + sizeof(DARCHFileInfo) * itemCount + stringPoolLen, 32);
	infoSize = dataSize;

	for (i = 0; i < fileInfoCount - 1; ++i)
		infoSize += ROUND_UP(fileInfos[i].fileSize, 32);

	arcSize = infoSize + fileInfos[fileInfoCount - 1].fileSize;

	return arcSize;
}

BOOL DARCHCreate(void *dstStartBuffer, u32 dstSize, DARCHFileInfo *fileInfos,
                 u32 fileInfoCount)
{
	OSAssert_Line(808, dstStartBuffer);

	DARCHItem rootDir;
	InitializeRootDir(&rootDir);

	if (!CreateDirStructure(&rootDir, fileInfos, fileInfoCount, dstStartBuffer,
	                        dstSize))
	{
		return false;
	}

	if (!ConstructFSTFromStructure(&rootDir, fileInfos, dstStartBuffer,
	                               dstSize))
	{
		return false;
	}

	return true;
}
