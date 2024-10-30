#include "stdafx.h"
#include <ipp.h>

#include "Library/KizuLib/Class/AutoLock.h"

#include "rpi.h"
#include "RpiReaderFile.h"
#include "MultiThreadRpiReader.h"
#include "ResultQueue.h"



void RpiResultQueue::OnRpiOpen(RpiLib::RpiReader * rpi) {
	framesize = rpi->Header.width  *rpi->Header.height;
	for (int ii = 0; ii<m_qsize; ii++) {
		m_empty_frames.push_back((BYTE*)VirtualAlloc(NULL, framesize, MEM_COMMIT | MEM_TOP_DOWN, PAGE_READWRITE));
	}

}

BYTE * RpiResultQueue::GetFreeQueue() {
	KizuLib::AutoLock  ar(&crs);
	if (m_empty_frames.empty()) {
		return NULL;
	}
	BYTE *dst = m_empty_frames.back();
	m_empty_frames.pop_back();
	return dst;

}

BYTE * RpiResultQueue::GetFillQueue() {
	KizuLib::AutoLock  ar(&crs);
	if (m_filled_frames.empty()) {
		return NULL;
	}
	BYTE *dst = m_filled_frames.front();
	m_filled_frames.pop_front();
	return dst;
}

void RpiResultQueue::ReturnFree(BYTE * p)
{
	KizuLib::AutoLock  ar(&crs);
	m_empty_frames.push_back(p);
}

// Œ‹‰Ê‚ª‚Å‚Ä‚«‚½B

void RpiResultQueue::OnRpiFrameDecode(int f, BYTE * pImage) {
	BYTE *pDst = 0;
	for (;
		pDst == NULL;
		pDst = GetFreeQueue()) {
		Sleep(100);
	}
	memcpy(pDst, pImage, framesize);

	KizuLib::AutoLock  ar(&crs);
	m_filled_frames.push_back(pDst);

}
