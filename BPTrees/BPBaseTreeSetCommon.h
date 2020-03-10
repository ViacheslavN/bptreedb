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
		 TBPTreeNodePtr pNode = m_NodeCache.GetElem(nAddr);
		 if (pNode.get() != nullptr)
		 {
			 pNode = LoadNodeFromStorage(nAddr);
			 AddToCache(pNode);
		 }

		 return pNode;
	 }
	 catch (std::exception& exc)
	 {
		 CommonLib::CExcBase::RegenExcT("TBPlusTreeSetBase failed to get node addr %1", nAddr, exc);
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
		CommonLib::CExcBase::RegenExcT("TBPlusTreeSetBase failed to get node with check parent addr %1", nAddr, exc);
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

		TBPTreeNodePtr node = TBPTreeNode::Load(&stream, m_pAlloc, m_bMulti, m_nNodePageSize, nAddr);
		node->InitCopmressor(m_InnerCompressParams, m_LeafCompressParams);
		return node;
	}
	catch (std::exception& exc)
	{
		CommonLib::CExcBase::RegenExcT("TBPlusTreeSetBase failed to load node from storage addr %1", nAddr, exc);
	}
}

BPSETBASE_TEMPLATE_PARAMS
BPSETBASE_TYPENAME_DECLARATION::TBPTreeNodePtr BPSETBASE_DECLARATION::FindAndSetParent(TBPTreeNodePtr& pCheckNode)
{
	try
	{
		const TKey& key = pCheckNode->key(0);
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
				break;
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
	}
	catch (std::exception& exc)
	{
		CommonLib::CExcBase::RegenExcT("TBPlusTreeSetBase FindParent failed to find parent node", exc);
	}
}

BPSETBASE_TEMPLATE_PARAMS
BPSETBASE_TYPENAME_DECLARATION::TBPTreeNodePtr BPSETBASE_DECLARATION::GetParentNode(TBPTreeNodePtr& pNode)
{
	try
	{

		TBPTreeNodePtr pParent = std::static_pointer_cast<TBPTreeNode>(pNode->GetParentNodePtr());
		if (!pParent.get())
		{
			pParent = GetNode(pNode->GetParentAddr());
			pNode->SetParent(pParent, pNode->GetFoundIndex());
		}

		if (!pParent.get() && pNode->GetAddr() != m_nRootAddr)
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
		CommonLib::CExcBase::RegenExcT("TBPlusTreeSetBase GetParentNode failed to find parent node", exc);
	}
}


BPSETBASE_TEMPLATE_PARAMS
void BPSETBASE_DECLARATION::SetParentForNextNode(TBPTreeNodePtr& pNode, TBPTreeNodePtr& pNodeNext)
{
	try
	{
		int32_t nFoundIndex = pNode->GetFoundIndex();
		TBPTreeNodePtr pParent = GetParentNode(pNode);

		if ((nFoundIndex + 1) < (int32_t)pParent->count() || nFoundIndex == -1)
		{
			pNodeNext->SetParent(pParent, nFoundIndex + 1);
			return;
		}
		
		GetParentNode(pNodeNext);
	}
	catch (std::exception& exc)
	{
		CommonLib::CExcBase::RegenExcT("TBPlusTreeSetBase failed to load node from storage addr %1", pNodeNext->GetAddr(), exc);
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
		CommonLib::CExcBase::RegenExcT("TBPlusTreeSetBase failed to load node from storage addr %1", pNodeNext->GetAddr(), exc);
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
		CommonLib::CExcBase::RegenExcT("TBPlusTreeSetBase failed to drop node addr", exc);
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
		 //TO DO log
	}

	return false;
}


BPSETBASE_TEMPLATE_PARAMS
void  BPSETBASE_DECLARATION::InnitTree(TInnerCompressorParamsPtr innerParams, TLeafCompressorParamsPtr leafParams)
{
	try
	{
		if (IsTreeInit())
			throw CommonLib::CExcBase("BTree has been created already");


		if (m_nPageBTreeInfo == -1)
			m_nPageBTreeInfo = m_pStorage->GetNewFilePageAddr(m_nNodePageSize);

		utils::TWriteStreamPage<TStorage> stream(m_pStorage);
		stream.Open(m_nPageBTreeInfo, m_nNodePageSize, true, true);

		m_nRootAddr = m_pStorage->GetNewFilePageAddr(m_nNodePageSize);

		stream.Write(m_nRootAddr);

		bool bInnerParams = innerParams.get() != nullptr ? true : false;
		bool bLeafParams = leafParams.get() != nullptr ? true : false;

		stream.Write(bInnerParams);
		stream.Write(bLeafParams);

		if (bInnerParams)
			innerParams->Write(&stream);
	
		if (bLeafParams)
			leafParams->Write(&stream);

		m_pRoot = CreateNode(m_nRootAddr, true, false); 
		m_pRoot->SetFlags(ROOT_NODE, true);
	}
	catch (std::exception& exc)
	{
		CommonLib::CExcBase::RegenExcT("TBPlusTreeSetBase failed to load root", exc);
	}
}

BPSETBASE_TEMPLATE_PARAMS
void BPSETBASE_DECLARATION::LoadTree()
{
	try
	{

		if (m_nPageBTreeInfo == -1)
			throw CommonLib::CExcBase("nPageBTreeInfo == -1");
		 
		utils::TReadStreamPage<TStorage> stream(m_pStorage);
		stream.Open(m_nPageBTreeInfo, m_nNodePageSize, true);

		m_nRootAddr = stream.ReadInt64();
		bool bInnerParams = stream.ReadBool();
		bool bLeafParams = stream.ReadBool();
		
		if (bInnerParams)
		{
			m_InnerCompressParams.reset(new TInnerCompressorParams());
			m_InnerCompressParams->Load(&stream);
		}

		if (bLeafParams)
		{
			m_LeafCompressParams.reset(new TLeafCompressorParams());
			m_LeafCompressParams->Load(&stream);
		}
		
		m_pRoot = LoadNodeFromStorage(m_nRootAddr);
		m_pRoot->SetFlags(ROOT_NODE, true);
	}
	catch (std::exception& exc)
	{
		CommonLib::CExcBase::RegenExcT("TBPlusTreeSetBase failed to load root", exc);
	}
}


BPSETBASE_TEMPLATE_PARAMS
void BPSETBASE_DECLARATION::SaveNode(TBPTreeNodePtr& pNode)
{
	try
	{
		FilePagePtr pPage = m_pStorage->GetEmptyFilePage(pNode->GetAddr(), m_nNodePageSize);
		CommonLib::CFxMemoryWriteStream stream;
		stream.AttachBuffer(pPage->GetData(), pPage->GetPageSize());

		uint32_t nCount = pNode->Save(&stream);
		while (nCount != 0)
		{

			//TO DO split

			stream.Seek(0, CommonLib::soFromBegin);
			nCount = pNode->Save(&stream);
		}

		m_pStorage->SaveFilePage(pPage);

		pNode->SetFlags(CHANGE_NODE, false);
	}
	catch (std::exception& exc)
	{
		CommonLib::CExcBase::RegenExcT("TBPlusTreeSetBase failed to save node addr",  exc);
	}
}

BPSETBASE_TEMPLATE_PARAMS
BPSETBASE_TYPENAME_DECLARATION::TBPTreeNodePtr BPSETBASE_DECLARATION::CreateNode(int64_t nAddr,  bool isLeaf, bool addToChache)
{
	try
	{
		if (nAddr == -1)
			nAddr = m_pStorage->GetNewFilePageAddr();

		TBPTreeNodePtr pNode = TBPTreeNode::Create(m_pAlloc, m_bMulti, m_nNodePageSize, nAddr, isLeaf);

		pNode->SetFlags(CHANGE_NODE, true);
		pNode->InitCopmressor(m_InnerCompressParams, m_LeafCompressParams);
		if (addToChache)
		{
			AddToCache(pNode);
		}

		return pNode;
	}
	catch (std::exception& exc)
	{
		CommonLib::CExcBase::RegenExcT("TBPlusTreeSetBase to CreateNode addr %1, isLeaf %2, addToCache %3", nAddr, isLeaf, addToChache, exc);
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
		m_bLockRemoveItemFromCache = true; // TO DO use RAII

		while (m_NodeCache.Size())
		{
			
			TBPTreeNodePtr pBNode = m_NodeCache.RemoveBack();
			if (!pBNode->GetFlags() & CHANGE_NODE)
				continue;

			SaveNode(pBNode);
		}

		m_bLockRemoveItemFromCache = false;
	}
	catch (std::exception& exc)
	{
		CommonLib::CExcBase::RegenExcT("TBPlusTreeSetBase failed to flash", exc);
	}
}