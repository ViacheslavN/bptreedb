#pragma once


BPSETBASE_TEMPLATE_PARAMS
void BPSETBASE_DECLARATION::AddToCache(TBPTreeNodePtr pNode)
{

/*	m_TempCache.insert(std::make_pair(pNode->GetAddr(), pNode));

	while (m_NodeCache.Size() > m_nChacheSize && !m_bLockRemoveItemFromCache)
	{
		TBPTreeNodePtr pRemNode = m_NodeCache.RemoveBack();
		DropNode(pRemNode);
	}*/

	m_NodeCache.AddElem(pNode->GetAddr(), pNode);
//	m_TempCache.erase(pNode->GetAddr());
}

BPSETBASE_TEMPLATE_PARAMS
BPSETBASE_TYPENAME_DECLARATION::TBPTreeNodePtr BPSETBASE_DECLARATION::GetNodeFromCache(int64_t nAddr, bool bNotMove)
{
	CommonLib::CPrefCounterHolder holder(m_pBPTreePerfCounter, eGetNodeFromCache);
	TBPTreeNodePtr pNode = m_NodeCache.GetElem(nAddr, bNotMove);
	if (pNode.get() != nullptr)
		return pNode;

	return TBPTreeNodePtr();
}


BPSETBASE_TEMPLATE_PARAMS
BPSETBASE_TYPENAME_DECLARATION::TBPTreeNodePtr BPSETBASE_DECLARATION::GetNode(int64_t nAddr)
{
	 try
	 {

		 CommonLib::CPrefCounterHolder holder(m_pBPTreePerfCounter, eGetNode);

		 if (nAddr == -1)
			 return TBPTreeNodePtr();

		 TBPTreeNodePtr pNode = GetNodeFromCache(nAddr);
		 if (pNode.get() == nullptr)
		 {
			 pNode = LoadNodeFromStorage(nAddr);
			 CommonLib::CPrefCounterHolder holder(m_pBPTreePerfCounter, eAddToCache);
			 AddToCache(pNode);
		 }

		 return pNode;
	 }
	 catch (std::exception& exc)
	 {
		 CommonLib::CExcBase::RegenExcT("[TBPlusTreeSetBase] failed to get node addr %1", nAddr, exc);
		 throw;
	 }
}


BPSETBASE_TEMPLATE_PARAMS
BPSETBASE_TYPENAME_DECLARATION::TBPTreeNodePtr BPSETBASE_DECLARATION::LoadNodeFromStorage(int64_t nAddr)
{
	try
	{

		m_pStorage->GetFilePage(m_cachePage, nAddr, m_nNodePageSize, true);
		CommonLib::CReadMemoryStream stream;
		stream.AttachBuffer(m_cachePage->GetData(), m_cachePage->GetPageSize());

		CommonLib::CPrefCounterHolder holder(m_pBPTreePerfCounter, eLoadNode);
		TBPTreeNodePtr node = LoadNode<TBPTreeNode>(&stream, m_pAllocsSet, m_bMulti, m_cachePage->GetPageSize(), nAddr, m_pCompressParams, &m_Context);

		return node;
	}
	catch (std::exception& exc)
	{
		CommonLib::CExcBase::RegenExcT("[TBPlusTreeSetBase] failed to load node from storage addr %1", nAddr, exc);
		throw;
	}
}

BPSETBASE_TEMPLATE_PARAMS
BPSETBASE_TYPENAME_DECLARATION::TBPTreeNodePtr BPSETBASE_DECLARATION::GetParentNode(TBPTreeNodePtr pNode, int32_t* nElementPos)
{
	try
	{

		CommonLib::CPrefCounterHolder holder(m_pBPTreePerfCounter, eGetParentNode);


		if (pNode->GetAddr() == m_nRootAddr)
			return TBPTreeNodePtr();

		const TKey& key = pNode->Key(0);
		int32_t nIndex = -1;
		int64_t nNextAddr = m_pRoot->inner_lower_bound(m_comp, key, nIndex);
		TBPTreeNodePtr pParent = m_pRoot;

		for (;;)
		{
			if (pNode->GetAddr() == nNextAddr)
			{
				if (nElementPos)
					*nElementPos = nIndex;

				return pParent;
			}

			TBPTreeNodePtr pNode = GetNode(nNextAddr);
			if (!pNode.get())
			{
				throw CommonLib::CExcBase("FindParent failed to find parent for node addr %1", pNode->GetAddr());
			}
			
			nNextAddr = pNode->inner_lower_bound(m_comp, key, nIndex);
			pParent = pNode;
		}

		throw CommonLib::CExcBase("FindParent failed to find parent for node addr %1", pNode->GetAddr());
	}
	catch (std::exception& exc)
	{
		CommonLib::CExcBase::RegenExcT("[TBPlusTreeSetBase] GetParentNode failed to find parent node", exc);
		throw;
	}
}


BPSETBASE_TEMPLATE_PARAMS
void BPSETBASE_DECLARATION::DropNode(TBPTreeNodePtr& pNode)
{
	try
	{
		uint32_t nFlags = pNode->GetFlags();
		if (nFlags & REMOVE_NODE)
		{
			m_pStorage->DropFilePage(pNode->GetAddr());
			return;
		}

		if (nFlags & CHANGE_NODE)
		{
			m_bLockRemoveItemFromCache = true; // TO DO use RAII

			SaveNode(pNode);

			m_bLockRemoveItemFromCache = false; // TO DO use RAII
		}

	}
	catch (std::exception& exc)
	{
		CommonLib::CExcBase::RegenExcT("[TBPlusTreeSetBase] failed to drop node addr", exc);
	}
}

BPSETBASE_TEMPLATE_PARAMS
bool BPSETBASE_DECLARATION::IsTreeInit()
{
	try
	{
		if (m_nPageBTreeInfo == -1)
			return false;

		FilePagePtr pPage = m_pStorage->GetFilePage(m_nPageBTreeInfo, m_nNodePageSize, true);

		return true;

	}
	catch (std::exception& exc)
	{
		exc;
		 //TO DO log
	}

	return false;
}


BPSETBASE_TEMPLATE_PARAMS
void  BPSETBASE_DECLARATION::InnitTree(TCompressorParamsBasePtr pParams, bool bMinSplit)
{
	try
	{
		if (IsTreeInit())
			throw CommonLib::CExcBase("BTree has been created already");

		m_bMinSplit = bMinSplit;
		if (m_nPageBTreeInfo == -1)
			m_nPageBTreeInfo = m_pStorage->GetNewFilePageAddr(m_nNodePageSize);

		utils::TWriteStreamPage<TStorage> stream(m_pStorage);
		stream.Open(m_nPageBTreeInfo, m_nNodePageSize, true, true);

		m_nRootAddr = m_pStorage->GetNewFilePageAddr(m_nNodePageSize);

		stream.Write(m_nRootAddr);

		bool bParams = pParams.get() != nullptr ? true : false;
 		stream.Write(bParams);
		m_pCompressParams = pParams;

		if (bParams)
			pParams->Save(&stream);
	
		stream.Close();

		m_pRoot = CreateNode(m_nRootAddr, true, false); 
		m_pRoot->SetFlags(ROOT_NODE, true);
	}
	catch (std::exception& exc)
	{
		CommonLib::CExcBase::RegenExcT("[TBPlusTreeSetBase] failed to load root", exc);
	}
}

BPSETBASE_TEMPLATE_PARAMS
void BPSETBASE_DECLARATION::LoadTree()
{
	try
	{

		if (m_nPageBTreeInfo == -1)
			throw CommonLib::CExcBase("PageBTreeInfo == -1");
		 
		utils::TReadStreamPage<TStorage> stream(m_pStorage);
		stream.Open(m_nPageBTreeInfo, m_nNodePageSize, true);

		m_nRootAddr = stream.ReadInt64();
		bool bParams = stream.ReadBool();
 
		
		if (bParams)
		{
			m_pCompressParams.reset(new CompressorParamsBase());
			m_pCompressParams->Load(&stream);
		}
 		
		m_pRoot = LoadNodeFromStorage(m_nRootAddr);
		m_pRoot->SetFlags(ROOT_NODE, true);
	}
	catch (std::exception& exc)
	{
		CommonLib::CExcBase::RegenExcT("[TBPlusTreeSetBase] failed to load root", exc);
	}
}

 

BPSETBASE_TEMPLATE_PARAMS
void BPSETBASE_DECLARATION::SaveNode(TBPTreeNodePtr& pNode)
{
	int64_t nAddr = pNode.get() ? pNode->GetAddr() : -1;
	uint32_t nInCount = pNode.get() ? pNode->Count() : 0;
	bool isLeaf = pNode.get() ? pNode->IsLeaf() : false;


	try
	{


		/*FilePagePtr pPage = m_pStorage->GetEmptyFilePage(nAddr, m_nNodePageSize);
		CommonLib::CFxMemoryWriteStream stream;
		stream.AttachBuffer(pPage->GetData(), pPage->GetPageSize());*/

		m_cachePage->Erase();
		CommonLib::CFxMemoryWriteStream stream;
		stream.AttachBuffer(m_cachePage->GetData(), m_cachePage->GetPageSize());
		m_cachePage->SetAddr(nAddr);
		uint32_t nCount = 0;
		
		{
			CommonLib::CPrefCounterHolder holder(m_pBPTreePerfCounter, isLeaf ? eSaveLeafNode : eSaveInnerNode);
			nCount = pNode->Save(&stream, &m_Context);
		}

 

		while (nCount != 0)
		{

		 	if (m_pBPTreePerfCounter.get() != nullptr)
			{
				m_pBPTreePerfCounter->StartOperation(eMissedNode);
				m_pBPTreePerfCounter->StopOperation(eMissedNode);
			}

			TBPTreeNodePtr pParentNode = GetParentNode(pNode);
			if (pParentNode.get() == nullptr)
			{
				if (pNode->IsLeaf())
					TransformRootToInner();
				else
					SplitRootInnerNode();
			}
			else			
			{
				if (pNode->IsLeaf())
				{				
					TBPTreeNodePtr pNewNode = NewNode(true, true);
					SplitLeafNode(pNode, pNewNode, pParentNode, nCount);
				}
				else
				{
					TBPTreeNodePtr pNewNode = NewNode(false, true);
					SplitInnerNode(pNode, pNewNode, pParentNode, nCount);
				}
			}

			m_cachePage->Erase();
			stream.Seek(0, CommonLib::soFromBegin);
			{
				CommonLib::CPrefCounterHolder holder(m_pBPTreePerfCounter, isLeaf ? eSaveLeafNode : eSaveInnerNode);
				nCount = pNode->Save(&stream, &m_Context);
			}
		}

		m_pStorage->SaveFilePage(m_cachePage);

		pNode->SetFlags(CHANGE_NODE, false);
	}
	catch (std::exception& exc)
	{
		CommonLib::CExcBase::RegenExcT("[TBPlusTreeSetBase] failed to save node addr: %1,  count: %2, leaf: %3", nAddr, nInCount, isLeaf, exc);
	}

}

BPSETBASE_TEMPLATE_PARAMS
BPSETBASE_TYPENAME_DECLARATION::TBPTreeNodePtr BPSETBASE_DECLARATION::CreateNode(int64_t nAddr,  bool isLeaf, bool addToChache)
{
	try
	{
		if (nAddr == -1)
			nAddr = m_pStorage->GetNewFilePageAddr();

		TBPTreeNodePtr pNode = AllocateNewNode(nAddr, isLeaf);

		pNode->SetFlags(CHANGE_NODE, true);
		//pNode->InitCopmressor(m_InnerCompressParams, m_LeafCompressParams);
		if (addToChache)
		{
			AddToCache(pNode);
		}

		return pNode;
	}
	catch (std::exception& exc)
	{
		CommonLib::CExcBase::RegenExcT("[TBPlusTreeSetBase] to CreateNode addr %1, isLeaf %2, addToCache %3", nAddr, isLeaf, addToChache, exc);
		throw;
	}
}

BPSETBASE_TEMPLATE_PARAMS
BPSETBASE_TYPENAME_DECLARATION::TBPTreeNodePtr BPSETBASE_DECLARATION::NewNode(bool isLeaf, bool addToChache)
{
	return CreateNode(-1, isLeaf, addToChache);
}

BPSETBASE_TEMPLATE_PARAMS
void BPSETBASE_DECLARATION::Flush()
{
	try
	{
		CommonLib::CPrefCounterHolder holder(m_pBPTreePerfCounter, eFlush);
		m_bLockRemoveItemFromCache = true; // TO DO use RAII

		while (m_pRoot->GetFlags() & CHANGE_NODE || m_NodeCache.Size() > 0)
		{
		
			while (m_NodeCache.Size())
			{

				TBPTreeNodePtr pBNode = m_NodeCache.RemoveBack();
				if (!(pBNode->GetFlags() & CHANGE_NODE))
					continue;

				SaveNode(pBNode);
			}

			if(m_pRoot->GetFlags() & CHANGE_NODE)
				SaveNode(m_pRoot);
		}
		
		m_bLockRemoveItemFromCache = false;
	}
	catch (std::exception& exc)
	{
		CommonLib::CExcBase::RegenExcT("TBPlusTreeSetBase failed to flash", exc);
	}
}


BPSETBASE_TEMPLATE_PARAMS
void BPSETBASE_DECLARATION::CheckCache()
{
	m_bLockRemoveItemFromCache = true; // TO DO use RAII

	while (m_NodeCache.Size() > m_nCacheSize)
	{

		TBPTreeNodePtr pBNode = m_NodeCache.RemoveBack();
		if (!(pBNode->GetFlags() & CHANGE_NODE))
			continue;

		SaveNode(pBNode);
	}

	m_bLockRemoveItemFromCache = false;
}


BPSETBASE_TEMPLATE_PARAMS
void BPSETBASE_DECLARATION::SetBPTreePerfCounter(CommonLib::TPrefCounterPtr pBPTreePerfCounter)
{
	m_pBPTreePerfCounter = pBPTreePerfCounter;
}

BPSETBASE_TEMPLATE_PARAMS
BPSETBASE_TYPENAME_DECLARATION::TBPTreeNodePtr BPSETBASE_DECLARATION::GetMinimumNode(TBPTreeNodePtr pNode)
 {
	TBPTreeNodePtr pMinNode = pNode;
	while (!pMinNode->IsLeaf())
	{

		pMinNode = GetNode(pNode->Less());
		pNode = pMinNode;
	}

	return pMinNode;
 }