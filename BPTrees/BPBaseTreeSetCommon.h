#pragma once


BPSETBASE_TEMPLATE_PARAMS
void BPSETBASE_DECLARATION::AddToCache(TBPTreeNodePtr& pNode)
{
	if (m_NodeCache.Size() > m_nChacheSize && !m_bLockRemoveItemFromCache)
	{
		TBPTreeNodePtr pRemNode = m_NodeCache.RemoveBack();
		DropNode(pRemNode);
	}

	m_NodeCache.AddElem(pNode->GetAddr(), pNode);
}

BPSETBASE_TEMPLATE_PARAMS
BPSETBASE_TYPENAME_DECLARATION::TBPTreeNodePtr BPSETBASE_DECLARATION::GetNode(int64_t nAddr)
{
	 try
	 {

		 CommonLib::CPrefCounterHolder holder(m_pBPTreePerfCounter, eGetNode);

		 if (nAddr == -1)
			 return TBPTreeNodePtr();

		 TBPTreeNodePtr pNode = m_NodeCache.GetElem(nAddr);
		 if (pNode.get() == nullptr)
		 {
			 pNode = LoadNodeFromStorage(nAddr);
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
BPSETBASE_TYPENAME_DECLARATION::TBPTreeNodePtr BPSETBASE_DECLARATION::GetNodeAndCheckParent(int64_t nAddr)
{
	try
	{
		TBPTreeNodePtr pNode = GetNode(nAddr);
		if (pNode->GetParentAddr() == -1)
			FindAndSetParent(pNode);
	}
	catch (std::exception& exc)
	{
		CommonLib::CExcBase::RegenExcT("[TBPlusTreeSetBase] failed to get node with check parent addr %1", nAddr, exc);
	}
}

BPSETBASE_TEMPLATE_PARAMS
BPSETBASE_TYPENAME_DECLARATION::TBPTreeNodePtr BPSETBASE_DECLARATION::LoadNodeFromStorage(int64_t nAddr)
{
	try
	{

		FilePagePtr pFilePage = m_pStorage->GetFilePage(nAddr, m_nNodePageSize, true);
		CommonLib::CReadMemoryStream stream;
		stream.AttachBuffer(pFilePage->GetData(), pFilePage->GetPageSize());

		CommonLib::CPrefCounterHolder holder(m_pBPTreePerfCounter, eLoadNode);
		TBPTreeNodePtr node = TBPTreeNode::Load(&stream, m_pAlloc, m_bMulti, m_nNodePageSize, nAddr, m_pCompressParams, &m_Context);

		return node;
	}
	catch (std::exception& exc)
	{
		CommonLib::CExcBase::RegenExcT("[TBPlusTreeSetBase] failed to load node from storage addr %1", nAddr, exc);
		throw;
	}
}

BPSETBASE_TEMPLATE_PARAMS
BPSETBASE_TYPENAME_DECLARATION::TBPTreeNodePtr BPSETBASE_DECLARATION::FindAndSetParent(TBPTreeNodePtr& pCheckNode)
{
	try
	{
		const TKey& key = pCheckNode->Key(0);
		int32_t nIndex = -1;
		int64_t nNextAddr = m_pRoot->inner_lower_bound(m_comp, key, nIndex);
		TBPTreeNodePtr pParent = m_pRoot;

		for (;;)
		{
			if (nNextAddr == -1)
			{
				break;
			}

			if (pCheckNode->GetAddr() == nNextAddr)
			{
				pCheckNode->SetParent(pParent, nIndex);
				return pParent;
			}

			TBPTreeNodePtr pNode = GetNode(nNextAddr);
			if (!pNode.get())
			{
				throw CommonLib::CExcBase("FindParent failed to find parent for node addr %1", pCheckNode->GetAddr());
			}

			pNode->SetParent(pParent, nIndex);
			nNextAddr = pNode->inner_lower_bound(m_comp, key, nIndex);
			pParent = pNode;
		}

		throw CommonLib::CExcBase("FindParent failed to find parent for node addr %1", pCheckNode->GetAddr());
	}
	catch (std::exception& exc)
	{
		CommonLib::CExcBase::RegenExcT("[TBPlusTreeSetBase] FindParent failed to find parent node", exc);
		throw;
	}
}

BPSETBASE_TEMPLATE_PARAMS
BPSETBASE_TYPENAME_DECLARATION::TBPTreeNodePtr BPSETBASE_DECLARATION::GetParentNode(TBPTreeNodePtr& pNode)
{
	try
	{
		if (pNode->GetAddr() == m_nRootAddr)
			return TBPTreeNodePtr();

		TBPTreeNodePtr pParent = std::static_pointer_cast<TBPTreeNode>(pNode->GetParentNodePtr());
		if (!pParent.get() && pNode->GetParentAddr() != -1)
		{
			pParent = GetNode(pNode->GetParentAddr());
			pNode->SetParent(pParent, pNode->GetFoundIndex());
		}

		if (!pParent.get())
		{
			pParent = FindAndSetParent(pNode);
			if (!pParent.get())
			{
				pParent = GetNode(pNode->GetParentAddr());
				pNode->SetParent(pParent, pNode->GetFoundIndex());
			}
		}

		return pParent;
	}
	catch (std::exception& exc)
	{
		CommonLib::CExcBase::RegenExcT("[TBPlusTreeSetBase] GetParentNode failed to find parent node", exc);
		throw;
	}
}


BPSETBASE_TEMPLATE_PARAMS
void BPSETBASE_DECLARATION::SetParentForNextNode(TBPTreeNodePtr& pNode, TBPTreeNodePtr& pNodeNext)
{
	try
	{
		int32_t nFoundIndex = pNode->GetFoundIndex();
		TBPTreeNodePtr pParent = GetParentNode(pNode);

		if ((nFoundIndex + 1) < (int32_t)pParent->Count() || nFoundIndex == -1)
		{
			pNodeNext->SetParent(pParent, nFoundIndex + 1);
			return;
		}
		
		GetParentNode(pNodeNext);
	}
	catch (std::exception& exc)
	{
		CommonLib::CExcBase::RegenExcT("[TBPlusTreeSetBase] failed to load node from storage addr %1", pNodeNext->GetAddr(), exc);
	}
}

BPSETBASE_TEMPLATE_PARAMS
void BPSETBASE_DECLARATION::SetParentForPrevNode(TBPTreeNodePtr& pNode, TBPTreeNodePtr& pNodeNext)
{
	try
	{
		int32_t nFoundIndex = pNode->GetFoundIndex();
		TBPTreeNodePtr pParent = GetParentNode(pNode);

		if (nFoundIndex > 0)
		{
			pNodeNext->SetParent(pParent, nFoundIndex - 1);
			return;
		}

		GetParentNode(pNodeNext);
	}
	catch (std::exception& exc)
	{
		CommonLib::CExcBase::RegenExcT("[TBPlusTreeSetBase] failed to load node from storage addr %1", pNodeNext->GetAddr(), exc);
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
	int64_t nParent = pNode.get() ? pNode->GetParentAddr() : -1;
	uint32_t nInCount = pNode.get() ? pNode->Count() : 0;
	bool isLeaf = pNode.get() ? pNode->IsLeaf() : false;

	try
	{
		FilePagePtr pPage = m_pStorage->GetEmptyFilePage(nAddr, m_nNodePageSize);
		CommonLib::CFxMemoryWriteStream stream;
		stream.AttachBuffer(pPage->GetData(), pPage->GetPageSize());

		uint32_t nCount = 0;
		
		{
			CommonLib::CPrefCounterHolder holder(m_pBPTreePerfCounter, eSaveNode);
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

			stream.Seek(0, CommonLib::soFromBegin);
			{
				CommonLib::CPrefCounterHolder holder(m_pBPTreePerfCounter, eSaveNode);
				nCount = pNode->Save(&stream, &m_Context);
			}
		}

		m_pStorage->SaveFilePage(pPage);

		pNode->SetFlags(CHANGE_NODE, false);
	}
	catch (std::exception& exc)
	{
		CommonLib::CExcBase::RegenExcT("[TBPlusTreeSetBase] failed to save node addr: %1, parent: %2, count: %3, leaf: %4", nAddr, nParent, nInCount, isLeaf, exc);
	}
}

BPSETBASE_TEMPLATE_PARAMS
BPSETBASE_TYPENAME_DECLARATION::TBPTreeNodePtr BPSETBASE_DECLARATION::CreateNode(int64_t nAddr,  bool isLeaf, bool addToChache)
{
	try
	{
		if (nAddr == -1)
			nAddr = m_pStorage->GetNewFilePageAddr();

		TBPTreeNodePtr pNode = TBPTreeNode::Create(m_pAlloc, m_bMulti, m_nNodePageSize, nAddr, isLeaf, m_pCompressParams);

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

		if (m_pRoot->GetFlags() & CHANGE_NODE)
			SaveNode(m_pRoot);

		while (m_NodeCache.Size())
		{
			
			TBPTreeNodePtr pBNode = m_NodeCache.RemoveBack();
			if (!(pBNode->GetFlags() & CHANGE_NODE))
				continue;

			SaveNode(pBNode);

			if (m_pRoot->GetFlags() & CHANGE_NODE)
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
		pMinNode->SetParent(pNode, -1);
		pNode = pMinNode;
	}

	return pMinNode;
 }