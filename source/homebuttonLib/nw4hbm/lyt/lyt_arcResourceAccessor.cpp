#include "arcResourceAccessor.h"

/*******************************************************************************
 * headers
 */

#include <cstring>
#include <extras_string.h>

#include <macros.h>

#include <revolution/types.h>

#include "../ut/LinkList.h"

// #include <revolution/arc.h>

#include <context_rvl.h>

/*******************************************************************************
 * local function declarations
 */

namespace
{
	using namespace nw4hbm;
	using namespace nw4hbm::lyt;

	ARCEntryNum FindNameResource(ARCHandle *pArcHandle, char const *resName);
	void *GetResourceSub(ARCHandle *pArcHandle, char const *resRootDir,
	                     byte4_t resType, char const *name, u32 *pSize);
} // unnamed namespace

/*******************************************************************************
 * functions
 */

namespace {

ARCEntryNum FindNameResource(ARCHandle *pArcHandle, char const *resName)
{
	ARCEntryNum entryNum = ARC_ENTRY_NUM_INVALID;

	ARCDir dir;
	BOOL bSuccess ATTR_UNUSED = ARCOpenDir(pArcHandle, ".", &dir);

	ARCDirEntry dirEntry;
	while (ARCReadDir(&dir, &dirEntry))
	{
		if (dirEntry.isDir)
		{
			bSuccess = ARCChangeDir(pArcHandle, dirEntry.name);
			entryNum = FindNameResource(pArcHandle, resName);
			bSuccess = ARCChangeDir(pArcHandle, "..");

			if (entryNum != ARC_ENTRY_NUM_INVALID)
				break;
		}
		else if (stricmp(resName, dirEntry.name) == 0)
		{
			entryNum = dirEntry.entryNum;
			break;
		}
	}

	bSuccess = ARCCloseDir(&dir);

	// stripped assert? maybe on bSuccess?
	(void)0;

	return entryNum;
}

void *GetResourceSub(ARCHandle *pArcHandle, char const *resRootDir,
                     byte4_t resType, char const *name, u32 *pSize)
{
	ARCEntryNum entryNum = ARC_ENTRY_NUM_INVALID;

	if (ARCConvertPathToEntrynum(pArcHandle, resRootDir)
	        != ARC_ENTRY_NUM_INVALID
	    && ARCChangeDir(pArcHandle, resRootDir))
	{
		if (!resType)
		{
			entryNum = FindNameResource(pArcHandle, name);
		}
		else
		{
			char resTypeStr[sizeof(byte4_t) + 1];
			resTypeStr[0] = resType >> 24;
			resTypeStr[1] = resType >> 16;
			resTypeStr[2] = resType >> 8;
			resTypeStr[3] = resType;
			resTypeStr[4] = '\0';

			if (ARCConvertPathToEntrynum(pArcHandle, resTypeStr)
			        != ARC_ENTRY_NUM_INVALID
			    && ARCChangeDir(pArcHandle, resTypeStr))
			{
				entryNum = ARCConvertPathToEntrynum(pArcHandle, name);
				BOOL bSuccess ATTR_UNUSED = ARCChangeDir(pArcHandle, "..");
			}
		}

		BOOL bSuccess ATTR_UNUSED = ARCChangeDir(pArcHandle, "..");
	}

	if (entryNum != ARC_ENTRY_NUM_INVALID)
	{
		ARCFileInfo arcFileInfo;
		BOOL bSuccess ATTR_UNUSED =
			ARCFastOpen(pArcHandle, entryNum, &arcFileInfo);

		void *resPtr = ARCGetStartAddrInMem(&arcFileInfo);

		if (pSize)
			*pSize = ARCGetLength(&arcFileInfo);

		ARCClose(&arcFileInfo);

		return resPtr;
	}

	return nullptr;
}

} // unnamed namespace

namespace nw4hbm { namespace lyt {

FontRefLink::FontRefLink():
	mpFont	(nullptr)
	{}

	void FontRefLink::Set(char const *name, ut::Font *pFont)
	{
		std::strcpy(mFontName, name);

		mpFont = pFont;
	}

namespace detail {

ut::Font *FindFont(FontRefLink::LinkList *pFontRefList, char const *name)
{
	NW4HBM_RANGE_FOR(it, *pFontRefList)
	{
		if (std::strcmp(name, it->GetFontName()) == 0)
			return it->GetFont();
	}

	return nullptr;
}

} // namespace detail

ArcResourceAccessor::ArcResourceAccessor():
	mArcBuf	(nullptr)
	{}

bool ArcResourceAccessor::Attach(void *archiveStart,
                                 char const *resourceRootDirectory)
{
	BOOL bSuccess = ARCInitHandle(archiveStart, &mArcHandle);

	if (!bSuccess)
		return false;

	mArcBuf = archiveStart;

	std::strncpy(mResRootDir, resourceRootDirectory, RES_ROOT_DIR_SIZE - 1);
	mResRootDir[RES_ROOT_DIR_SIZE - 1] = '\0';

	return true;
}

void *ArcResourceAccessor::Detach()
{
	void *ret = mArcBuf;

	mArcBuf = nullptr;

	return ret;
}

void *ArcResourceAccessor::GetResource(byte4_t resType, char const *name,
                                       u32 *pSize)
{
	return GetResourceSub(&mArcHandle, mResRootDir, resType, name, pSize);
}

bool ArcResourceLink::Set(void *archiveStart, char const *resourceRootDirectory)
{
	BOOL bSuccess = ARCInitHandle(archiveStart, &mArcHandle);

	if (!bSuccess)
		return false;

	std::strncpy(mResRootDir, resourceRootDirectory, RES_ROOT_DIR_SIZE - 1);
	mResRootDir[RES_ROOT_DIR_SIZE - 1] = '\0';

	return true;
}

void *ArcResourceLink::GetArchiveDataStart() const
{
	return mArcHandle.archiveStartAddr;
}

void ArcResourceAccessor::RegistFont(FontRefLink *pLink)
{
	mFontList.PushBack(pLink);
}

void ArcResourceAccessor::UnregistFont(FontRefLink *pLink)
{
	mFontList.Erase(pLink);
}

ut::Font *ArcResourceAccessor::GetFont(char const *name)
{
	return detail::FindFont(&mFontList, name);
}

MultiArcResourceAccessor::MultiArcResourceAccessor() {}

MultiArcResourceAccessor::~MultiArcResourceAccessor()
{
	DetachAll();
}

void MultiArcResourceAccessor::Attach(ArcResourceLink *pLink)
{
	mArcList.PushBack(pLink);
}

ArcResourceLink *MultiArcResourceAccessor::Detach(void const *archiveStart)
{
	NW4HBM_RANGE_FOR(it, mArcList)
	{
		if (archiveStart == it->GetArchiveDataStart())
		{
			ArcResourceLink *ret = &(*it);

			mArcList.Erase(it);

			return ret;
		}
	}

	return nullptr;
}

void MultiArcResourceAccessor::Detach(ArcResourceLink *pLink)
{
	mArcList.Erase(pLink);
}

void MultiArcResourceAccessor::DetachAll()
{
	// Who is doing this

	// TODO(FAKE): What
	// NOTE: C-style cast is for static_cast with extensions to private bases
	((ut::detail::LinkListImpl *)&mArcList)->Clear();
}

void *MultiArcResourceAccessor::GetResource(byte4_t resType, char const *name,
                                            u32 *pSize)
{
	NW4HBM_RANGE_FOR(it, mArcList)
	{
		ARCHandle *pArcHandle = it->GetArcHandle();

		if (void *resPtr = GetResourceSub(pArcHandle, it->GetResRootDir(),
		                                  resType, name, pSize))
		{
			return resPtr;
		}
	}

	return nullptr;
}

void MultiArcResourceAccessor::RegistFont(FontRefLink *pLink)
{
	mFontList.PushBack(pLink);
}

void MultiArcResourceAccessor::UnregistFont(FontRefLink *pLink)
{
	mFontList.Erase(pLink);
}

ut::Font *MultiArcResourceAccessor::GetFont(char const *name)
{
	return detail::FindFont(&mFontList, name);
}

}} // namespace nw4hbm::lyt
