#pragma once

#include "..\..\Include\LibSelect.h"										// ���C�u�����ؑփw�b�_�[

class AtlRegManager
{
public:
	AtlRegManager(void);
	virtual ~AtlRegManager(void);

	static bool IsRegularA(char const* val, char const* match);				// ���K�\�� ���`�F�b�N [1byte ascii] (���{�ꖢ�Ή�) �� �ꉞ�Ή��͏o������
	static bool IsRegularA(char const* val, int valSize, char const* match);// ���K�\�� ���`�F�b�N [1byte ascii] 

	static bool IsRegularMB(unsigned char const* val, unsigned char const* match);
};
