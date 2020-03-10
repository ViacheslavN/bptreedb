#include "pch.h"

uint32_t nPageSize = 8192;

template<class _Ty>
struct comp
{
	bool LE(const _Ty& _Left, const _Ty& _Right) const
	{
		return (_Left < _Right);
	}
	bool EQ(const _Ty& _Left, const _Ty& _Right) const
	{
		return (_Left == _Right);
	}
	bool operator() (const _Ty& _Left, const _Ty& _Right) const
	{
		return (_Left < _Right);
	}
	/*bool MR(const _Ty& _Left, const _Ty& _Right){
	return (_Left > _Right);
	}*/
};

typedef bptreedb::BPTreeLeafNodeSetBase<int64_t, bptreedb::TBaseLeafNodeSetCompressor<int64_t> > TLeafNode;
typedef bptreedb::BPTreeInnerNode<int64_t, bptreedb::TBaseNodeCompressor<int64_t, int64_t> > TInnerNode;
typedef bptreedb::TBPSet<int64_t, comp<int64_t>, bptreedb::IStorage, TInnerNode, TLeafNode> TBPSet;

template <class TBPTree>
uint64_t CreateBPTreeSet(CommonLib::IAllocPtr pAlloc, bptreedb::TStoragePtr pStorage)
{
	try
	{
		TBPSet tree(-1, pStorage, pAlloc, 10, nPageSize);

		typename TInnerNode::TInnerCompressorParamsPtr innerCompParmas = new TInnerNode::TInnerCompressorParams();
		typename TLeafNode::TLeafCompressorParamsPtr leafCompParmas = new TLeafNode::TLeafCompressorParams();
		tree.InnitTree(innerCompParmas, leafCompParmas);
	}
	catch (std::exception exe)
	{
		
	CommonLib::CExcBase::RegenExc("Failed create BPTreeSet", exe);
	}
}



void TestBPTreeSet()
{

	try
	{
		CommonLib::IAllocPtr pAlloc(new CommonLib::CSimpleAlloc(true));
		bptreedb::TStoragePtr  pStorage(new bptreedb::CFileStorage(pAlloc));
		pStorage->Open(L"F:\\BPSetTest.btdb", true, nPageSize);
	}
	catch (std::exception exe )
	{
		std::cout << "TestBPTreeSet failed: " << "\n";
		std::cout << CommonLib::CExcBase::GetErrorDesc(exe);
	}
	
}