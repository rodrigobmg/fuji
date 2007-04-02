#include "Fuji.h"
#include "MFHeap.h"
#include "MFFileSystem_Internal.h"
#include "FileSystem/MFFileSystemNative.h"

#include <pspkernel.h>
#include <stdio.h>

const char *gPSPSystemPath __attribute__((weak)) = "host0:";

void MFFileSystemNative_Register()
{

}

void MFFileSystemNative_Unregister()
{

}

int MFFileNative_Open(MFFile *pFile, MFOpenData *pOpenData)
{
	MFCALLSTACK;

	MFDebug_Assert(pOpenData->cbSize == sizeof(MFOpenDataNative), "Incorrect size for MFOpenDataNative structure. Invalid pOpenData.");
	MFOpenDataNative *pNative = (MFOpenDataNative*)pOpenData;

	int access = ((pOpenData->openFlags&MFOF_Read) ? PSP_O_RDONLY : NULL) | ((pOpenData->openFlags&MFOF_Write) ? PSP_O_WRONLY|PSP_O_CREAT|PSP_O_TRUNC : NULL);
	MFDebug_Assert(access, "Neither MFOF_Read nor MFOF_Write specified.");

	const char *pFilename = MFStr("%s/%s", gPSPSystemPath, pNative->pFilename);
	SceUID hFile = sceIoOpen(pFilename, access, 0777);

	if(hFile < 0)
	{
		MFDebug_Warn(4, MFStr("File does not exist: '%s'", pFilename));
		return -1;
	}

	pFile->pFilesysData = (void*)hFile;
	pFile->state = MFFS_Ready;
	pFile->operation = MFFO_None;
	pFile->createFlags = pOpenData->openFlags;
	pFile->offset = 0;

	// find file length
	SceOff fileSize = sceIoLseek(hFile, 0, SEEK_END);

	// TODO: something's very wrong with this line! :/
//	MFDebug_Assert(fileSize < 2147483648LL, MFStr("Error opening file '%s', Fuji does not support files larger than 2,147,483,647 bytes.", pFilename));
	pFile->length = (int)fileSize;

	// return to start of file
	sceIoLseek32(hFile, 0, SEEK_SET);

#if defined(_DEBUG)
	MFString_Copy(pFile->fileIdentifier, pFilename);
#endif

	return 0;
}

int MFFileNative_Close(MFFile* fileHandle)
{
	MFCALLSTACK;

	sceIoClose((SceUID)fileHandle->pFilesysData);

	return 0;
}

int MFFileNative_Read(MFFile* fileHandle, void *pBuffer, uint32 bytes, bool async)
{
	MFCALLSTACK;

	MFDebug_Assert(async == false, "Asynchronous Filesystem not yet supported...");

	int bytesRead;
	bytesRead = sceIoRead((SceUID)fileHandle->pFilesysData, pBuffer, bytes);
	fileHandle->offset += (uint32)bytesRead;

	return bytesRead;
}

int MFFileNative_Write(MFFile* fileHandle, const void *pBuffer, uint32 bytes, bool async)
{
	MFCALLSTACK;

	MFDebug_Assert(async == false, "Asynchronous Filesystem not yet supported...");

	int bytesWritten;
	bytesWritten = sceIoWrite((SceUID)fileHandle->pFilesysData, pBuffer, bytes);
	fileHandle->offset += (uint32)bytesWritten;
	fileHandle->length = MFMax(fileHandle->offset, (uint32)fileHandle->length);

	return bytesWritten;
}

int MFFileNative_Seek(MFFile* fileHandle, int bytes, MFFileSeek relativity)
{
	MFCALLSTACK;

	int method = 0;

	switch(relativity)
	{
		case MFSeek_Begin:
			method = SEEK_SET;
			break;
		case MFSeek_End:
			method = SEEK_END;
			break;
		case MFSeek_Current:
			method = SEEK_CUR;
			break;
		default:
			MFDebug_Assert(false, "Invalid 'relativity' for file seeking.");
	}

	SceOff newPos = sceIoLseek((SceUID)fileHandle->pFilesysData, bytes, method);
	fileHandle->offset = (uint32)newPos;
	return newPos;
}

int MFFileNative_Tell(MFFile* fileHandle)
{
	return fileHandle->offset;
}

MFFileState MFFileNative_Query(MFFile* fileHandle)
{
	return fileHandle->state;
}

int MFFileNative_GetSize(MFFile* fileHandle)
{
	return fileHandle->length;
}

uint32 MFFileNative_GetSize(const char* pFilename)
{
	MFCALLSTACK;

	uint32 fileSize = 0;

	pFilename = MFStr("%s/%s", gPSPSystemPath, pFilename);
	SceUID hFile = sceIoOpen(pFilename, PSP_O_RDONLY, 0777);

	if(hFile > 0)
	{
		SceOff fileSize = sceIoLseek(hFile, 0, SEEK_END);
		MFDebug_Assert(fileSize < (SceOff)4294967296ULL, "Fuji does not support files larger than 4,294,967,295 bytes.");
		fileSize = (uint32)fileSize;

		sceIoClose(hFile);
	}

	return fileSize;
}

bool MFFileNative_Exists(const char* pFilename)
{
	MFCALLSTACK;

	bool exists = false;

	pFilename = MFStr("%s/%s", gPSPSystemPath, pFilename);
	SceUID hFile = sceIoOpen(pFilename, PSP_O_RDONLY, 0777);

	if(hFile > 0)
	{
		sceIoClose(hFile);
		exists = true;
	}

	return exists;
}

bool MFFileNative_FindFirst(MFFind *pFind, const char *pSearchPattern, MFFindData *pFindData)
{
	SceIoDirent findData;
	int findStatus;

	SceUID hFind = sceIoDopen(pSearchPattern);

	if(hFind < 0)
		return false;

	findStatus = sceIoDread(hFind, &findData);

	MFDebug_Assert(findStatus >= 0, "Error reading directory.");

	if(findStatus == 0)
		return false;

	pFindData->attributes = (FIO_SO_ISDIR(findData.d_stat.st_attr) ? MFFA_Directory : 0) |
							(FIO_SO_ISLNK(findData.d_stat.st_attr) ? MFFA_SymLink : 0);
	pFindData->fileSize = (uint64)findData.d_stat.st_size;
	MFString_Copy((char*)pFindData->pFilename, findData.d_name);

	MFString_CopyCat(pFindData->pSystemPath, (char*)pFind->pMount->pFilesysData, pSearchPattern);
	char *pLast = MFString_RChr(pFindData->pSystemPath, '/');
	if(pLast)
		pLast[1] = 0;
	else
		pFindData->pSystemPath[0] = 0;

	pFind->pFilesystemData = (void*)hFind;

	return true;
}

bool MFFileNative_FindNext(MFFind *pFind, MFFindData *pFindData)
{
	SceIoDirent findData;
	int findStatus;

	findStatus = sceIoDread((SceUID)pFind->pFilesystemData, &findData);

	MFDebug_Assert(findStatus >= 0, "Error reading directory.");

	if(findStatus == 0)
		return false;

	pFindData->attributes = (FIO_SO_ISDIR(findData.d_stat.st_attr) ? MFFA_Directory : 0) |
							(FIO_SO_ISLNK(findData.d_stat.st_attr) ? MFFA_SymLink : 0);
	pFindData->fileSize = (uint64)findData.d_stat.st_size;
	MFString_Copy((char*)pFindData->pFilename, findData.d_name);

	return true;
}

void MFFileNative_FindClose(MFFind *pFind)
{
	sceIoDclose((SceUID)pFind->pFilesystemData);
}
