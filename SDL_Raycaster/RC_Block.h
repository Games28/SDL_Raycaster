#pragma once
#ifndef RC_BLOCK_H
#define RC_BLOCK_H
#include "Constants.h"
#include <map>
#include <string>

typedef struct sBLockStruct
{
	char cID;
	float fHeight;
	int nFaces[FACE_NR_OF];
}BlockType;

class Blocks
{
public:
	Blocks() = default;
	~Blocks() = default;

	void AddBlock(char cID, float fH, int nE, int nN, int nW, int nS, int nT, int nB)
	{
		BlockType sTmpBlock = { cID, fH, nE, nN, nW, nS, nT, nB };
		mBlockLook.insert(std::make_pair(cID, sTmpBlock));
	}

	void InitBlockTypes() {
		AddBlock('.', 0.00f, 0, 0, 0, 0, 0, 0);

		AddBlock('#', 1.00f, 0, 0, 0, 0, 0, 0);   // these are indices into the vectors of wall, roof and ceiling sprite pointers
		AddBlock('%', 1.00f, 1, 1, 1, 1, 1, 1);
		AddBlock('!', 1.00f, 2, 2, 2, 2, 2, 2);
		AddBlock('@', 1.00f, 3, 3, 3, 3, 3, 3);
		AddBlock('$', 1.00f, 4, 4, 4, 4, 4, 4);
		AddBlock('&', 1.00f, 5, 5, 5, 5, 5, 5);
		AddBlock('*', 1.00f, 6, 6, 6, 6, 6, 6);
		AddBlock('+', 1.00f, 7, 7, 7, 7, 7, 7);

		AddBlock('Q', 0.25f, 0, 0, 0, 0, 0, 0);
		AddBlock('H', 0.50f, 0, 0, 0, 0, 0, 0);
		AddBlock('T', 0.75f, 0, 0, 0, 0, 0, 0);

		AddBlock('1', 0.10f, 0, 0, 0, 0, 0, 0);
		AddBlock('2', 0.20f, 0, 0, 0, 0, 0, 0);
		AddBlock('3', 0.30f, 0, 0, 0, 0, 0, 0);
		AddBlock('4', 0.40f, 0, 0, 0, 0, 0, 0);
		AddBlock('5', 0.50f, 0, 0, 0, 0, 0, 0);
		AddBlock('6', 0.60f, 0, 0, 0, 0, 0, 0);
		AddBlock('7', 0.70f, 0, 0, 0, 0, 0, 0);
		AddBlock('8', 0.80f, 0, 0, 0, 0, 0, 0);
		AddBlock('9', 0.90f, 0, 0, 0, 0, 0, 0);
	}
	
	BlockType& GetBlock(char cID) {
		std::map<char, BlockType>::iterator itBlock = mBlockLook.find(cID);
		if (itBlock == mBlockLook.end()) {
			std::cout << "ERROR: GetBlock() --> can't find element with ID: " << cID << std::endl;
		}
		return (*itBlock).second;
	}

	char GetBlockID(BlockType& b) { return b.cID; }
	char GetBlockHeight(BlockType& b) { return b.fHeight; }
	char GetBlockFaceIx(BlockType& b, int nFace) {
		int nIndex = -1;
		if (nFace < 0 || nFace >= FACE_NR_OF) {
			std::cout << "GetBlockFaceIx() --> index out of range: " << nFace << std::endl;
		}
		else {
			nIndex = b.nFaces[nFace];
		}
		return nIndex;
	}

private:
	std::map<char, BlockType> mBlockLook;

};

#endif // !RC_BLOCK_H

