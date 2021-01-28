#pragma once

namespace bptreedb
{

	enum EPrefOperation
	{
		eLoadNode,
		eSaveLeafNode,
		eSaveInnerNode,
		eGetNode,
		eMissedNode,
		eInsertValue,
		eRemoveValue,
		eFlush,
		eFindKey,
		eGetParentNode,
		eAddToCache,
		eGetNodeFromCache
	};


}