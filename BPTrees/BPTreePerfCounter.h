#pragma once

namespace bptreedb
{

	enum EPrefOperation
	{
		eLoadNode,
		eSaveNode,
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