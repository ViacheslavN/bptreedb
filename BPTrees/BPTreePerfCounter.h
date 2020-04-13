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
		eFlush,
		eFindKey
	};


}