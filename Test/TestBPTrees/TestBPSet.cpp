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


typedef bptreedb::TZstdEncoder<int64_t> TEncoder;
typedef bptreedb::TZstdEncoder<int64_t>::CZLibCompParams  TCompParams;
typedef bptreedb::BPTreeLeafNode<int64_t, bptreedb::TBaseLeafNodeSetCompressor<int64_t, TEncoder, TCompParams> > TLeafNode;
typedef bptreedb::BPTreeInnerNode<int64_t, bptreedb::TBaseNodeCompressor<int64_t, int64_t, TEncoder, TEncoder, TCompParams > > TInnerNode;
typedef bptreedb::TBPSet<int64_t, comp<int64_t>, bptreedb::IStorage, TInnerNode, TLeafNode> TBPSet;

/*
typedef bptreedb::TZLibEncoder<int64_t> TEncoder;
typedef bptreedb::TZLibEncoder<int64_t>::CZLibCompParams  TCompParams;
typedef bptreedb::BPTreeLeafNode<int64_t, bptreedb::TBaseLeafNodeSetCompressor<int64_t, TEncoder, TCompParams> > TLeafNode;
typedef bptreedb::BPTreeInnerNode<int64_t, bptreedb::TBaseNodeCompressor<int64_t, int64_t, TEncoder, TEncoder, TCompParams > > TInnerNode;
typedef bptreedb::TBPSet<int64_t, comp<int64_t>, bptreedb::IStorage, TInnerNode, TLeafNode> TBPSet;*/

/*
typedef bptreedb::BPTreeLeafNode<int64_t, bptreedb::TBaseLeafNodeSetCompressor<int64_t> > TLeafNode;
typedef bptreedb::BPTreeInnerNode<int64_t, bptreedb::TBaseNodeCompressor<int64_t, int64_t> > TInnerNode;
typedef bptreedb::TBPSet<int64_t, comp<int64_t>, bptreedb::IStorage, TInnerNode, TLeafNode> TBPSet;*/

template <class TBPTree, class TStorage>
uint64_t CreateBPTreeSet(CommonLib::IAllocPtr pAlloc, TStorage pStorage)
{
	try
	{
		std::cout << "CreateBPTreeSet";

		TBPSet tree(-1, pStorage, pAlloc, 10, nPageSize);

		typename TInnerNode::TInnerCompressorParamsPtr innerCompParmas (new TInnerNode::TInnerCompressorParams());
		innerCompParmas->m_compressRate = 100;
	//	innerCompParmas->m_compressLevel = 100;

		typename TLeafNode::TLeafCompressorParamsPtr leafCompParmas (new TLeafNode::TLeafCompressorParams());
		leafCompParmas->m_compressRate = 100;
	//	innerCompParmas->m_compressLevel = 100;

		tree.InnitTree(innerCompParmas, leafCompParmas, false);
		tree.Flush();
		return tree.GetPageBTreeInfo();
	}
	catch (std::exception& exe)
	{		
		CommonLib::CExcBase::RegenExc("Failed create BPTreeSet", exe);
		throw;
	}
}



template <class TBPTree, class TStorage>
void InsertBPTreeSet(CommonLib::IAllocPtr pAlloc, TStorage pStorage, int64_t nBPTreePage, int64_t nBegin, int64_t nEnd)
{
	try
	{
		int64_t nCount = (nEnd - nBegin);
		int64_t nStep = nCount / 100;

		if (nStep == 0)
			nStep = 1;

		std::cout << "InsertBPTreeSet begin: " << nBegin  << " end: " << nEnd << "\n";

		TBPSet tree(nBPTreePage, pStorage, pAlloc, 10, nPageSize);
		
		for (int64_t i = nBegin; i < nEnd; ++i)
		{
			if (i == 261120)
			{
				int dd = 0;
				dd++;
			}

			tree.insert(i);

			if (i == 261120)
			{
				auto it = tree.find(i);
			}

			if (i%nStep == 0)
			{
				std::cout << i << "  " << (i * 100) / nCount << " %" << '\r';
			}
		}

		tree.Flush();
	 
		std::cout << "InsertBPTreeSet finish "  << "\n";
	}
	catch (std::exception& exe)
	{
		CommonLib::CExcBase::RegenExc("Failed to insert BPTreeSet", exe);
	}
}




template <class TBPTree, class TStorage>
void FindBPTreeSet(CommonLib::IAllocPtr pAlloc, TStorage pStorage, int64_t nBPTreePage, int64_t nBegin, int64_t nEnd)
{
	try
	{
		int64_t nCount = (nEnd - nBegin);
		int64_t nStep = nCount / 100;
		if (nStep == 0)
			nStep = 1;

		std::cout << "FindBPTreeSet begin: " << nBegin << " end: " << nEnd << "\n";

		TBPSet tree(nBPTreePage, pStorage, pAlloc, 10, nPageSize);

		for (int64_t i = nBegin; i < nEnd; ++i)
		{
			auto it = tree.find(i);

			if (it.IsNull())
				throw CommonLib::CExcBase("Error find %1", i);

			if (it.Key() != i)
				throw CommonLib::CExcBase("Error find key %1", it.Key(), i);

			if (i%nStep == 0)
			{
				std::cout << i << "  " << (i * 100) / nCount << " %" << '\r';
			}
		}

		tree.Flush();

		std::cout << "FindBPTreeSet finish " << "\n";
	}
	catch (std::exception& exe)
	{
		CommonLib::CExcBase::RegenExc("Failed FindBPTreeSet", exe);
	}
}


void TestBPTreeSet()
{

	try
	{
		int64_t nBegin = 0;
		int64_t nEnd = 20000000;
		int64_t nBPTreePage = -1;

		CommonLib::IAllocPtr pAlloc(new CommonLib::CSimpleAlloc(true));

		{
			bptreedb::TStoragePtr  pStorage(new bptreedb::CFileStorage(pAlloc));
			pStorage->Open(L"F:\\BPSetTest.btdb", true, nPageSize);
			nBPTreePage = CreateBPTreeSet<TBPSet, bptreedb::TStoragePtr>(pAlloc, pStorage);

		}

		{
			bptreedb::TStoragePtr  pStorage(new bptreedb::CFileStorage(pAlloc));
			pStorage->Open(L"F:\\BPSetTest.btdb", false, nPageSize);
			InsertBPTreeSet<TBPSet, bptreedb::TStoragePtr>(pAlloc, pStorage, nBPTreePage, nBegin, nEnd);

			pStorage->Close();

			CommonLib::file::TFilePtr file = CommonLib::file::CFileCreator::OpenFile(L"F:\\BPSetTest.btdb", CommonLib::file::ofmOpenExisting, CommonLib::file::arRead, CommonLib::file::smNoMode, CommonLib::file::oftBinary);
			std::cout << "Storage size: " << file->GetFileSize() << "\n";
		}

		{
			bptreedb::TStoragePtr  pStorage(new bptreedb::CFileStorage(pAlloc));
			pStorage->Open(L"F:\\BPSetTest.btdb", false, nPageSize);
			FindBPTreeSet<TBPSet, bptreedb::TStoragePtr>(pAlloc, pStorage, nBPTreePage, nBegin, nEnd);
		}
	}
	catch (std::exception& exe )
	{
		std::cout << "TestBPTreeSet failed: " << "\n";

		astrvec msgChain = CommonLib::CExcBase::GetChainFromExc(exe);


		for (size_t i = 0, sz = msgChain.size(); i < sz; ++i)
		{
			if (i != 0)
				std::cout << "\r\n";

			std::cout << msgChain[i];
		}

	}
	
}