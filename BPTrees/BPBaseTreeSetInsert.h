#pragma once


BPSETBASE_TEMPLATE_PARAMS
void BPSETBASE_DECLARATION::insert(TKey& key)
{
	try
	{
		TBPTreeNodePtr pNode = findLeafNodeForInsert(key);
		pNode->InsertInLeaf(m_comp, key);
		pNode->SetFlags(CHANGE_NODE, true);

		CheckLeafNode(pNode);
	}
	catch (std::exception& exc)
	{
		CommonLib::CExcBase::RegenExcT("TBPlusTreeSetBase failed insert", exc);
	}
}

BPSETBASE_TEMPLATE_PARAMS
BPSETBASE_TYPENAME_DECLARATION::TBPTreeNodePtr BPSETBASE_DECLARATION::findLeafNodeForInsert(const TKey& key)
{
	try
	{

		if (m_pRoot.get() == nullptr)
			LoadTree();

		if (m_pRoot->IsLeaf())
		{
			return m_pRoot;
		}

		int32_t nIndex = -1;
		int64_t nNextAddr = m_pRoot->FindNodeInsert(m_comp, key, nIndex);
		TBPTreeNodePtr pParent = m_pRoot;

		while (nNextAddr != -1)
		{
			TBPTreeNodePtr pNode = GetNode(nNextAddr);
			pNode->SetParent(pParent, nIndex);
			if (pNode->IsLeaf())
			{
				return pNode;
			}

			pParent = pNode;
			nNextAddr = pNode->FindNodeInsert(m_comp, key, nIndex);
		}

		throw CommonLib::CExcBase("can't find leaf node");
	}
	catch (std::exception& exc)
	{
		CommonLib::CExcBase::RegenExcT("TBPlusTreeSetBase failed findLeafNodeForInsert", exc);
		throw;
	}
}
 

BPSETBASE_TEMPLATE_PARAMS
void BPSETBASE_DECLARATION::CheckLeafNode(TBPTreeNodePtr &pNode)
{
	try
	{
		if (!pNode->IsNeedSplit())
			return;

		if (pNode->GetAddr() == m_nRootAddr)
		{
			TransformRootToInner();
			return;
		}

		TBPTreeNodePtr pCheckNode = GetParentNode(pNode);
		TBPTreeNodePtr pNewRightLeafNode = NewNode(true, true);
		SplitLeafNode(pNode, pNewRightLeafNode, pCheckNode, m_bMinSplit ? 1 : pNode->Count() / 2);

		while (pCheckNode->IsNeedSplit())
		{

			TBPTreeNodePtr pParentNode = GetParentNode(pCheckNode);
			if (!pParentNode.get())
			{
				SplitRootInnerNode();
				break;
			}

			TBPTreeNodePtr pNodeNewRight = NewNode(false, true);
			SplitInnerNode(pCheckNode, pNodeNewRight, pParentNode, m_bMinSplit ? 3 : pNode->Count() / 2);
			pCheckNode = pParentNode;

		}

	}
	catch (std::exception& exc)
	{
		CommonLib::CExcBase::RegenExcT("TBPlusTreeSetBase failed CheckLeafNode", exc);
	}
}


BPSETBASE_TEMPLATE_PARAMS
void BPSETBASE_DECLARATION::TransformRootToInner()
{
	try
	{
		TBPTreeNodePtr pLeftNode = NewNode(true, true);
		TBPTreeNodePtr pRightNode = NewNode(true, true);

		TKey splitKey;
		int nSplitIndex = m_pRoot->SplitIn(pLeftNode, pRightNode, &splitKey);

		m_pRoot->Clear();
		m_pRoot->TransformToInner(m_pAlloc, m_bMulti, m_nNodePageSize, m_pCompressParams);

		m_pRoot->SetLess(pLeftNode->GetAddr());
		int nInsertIndex = m_pRoot->InsertInInnerNode(m_comp, splitKey, pRightNode->GetAddr());

		pRightNode->SetParent(m_pRoot, nInsertIndex);
		pLeftNode->SetParent(m_pRoot, -1);
		pLeftNode->SetNext(pRightNode->GetAddr());
		pRightNode->SetPrev(pLeftNode->GetAddr());

		pLeftNode->SetFlags(CHANGE_NODE, true);
		pRightNode->SetFlags(CHANGE_NODE, true);
		m_pRoot->SetFlags(CHANGE_NODE, true);
	}
	catch (std::exception& exc)
	{
		CommonLib::CExcBase::RegenExcT("TBPlusTreeSetBase failed TransformRootToInner", exc);
	}
}


BPSETBASE_TEMPLATE_PARAMS
void BPSETBASE_DECLARATION::SplitLeafNode(TBPTreeNodePtr &pNode, TBPTreeNodePtr &pNewNode, TBPTreeNodePtr &pParentNode, int32_t nCount)
{
	try
	{
		if (!pNode->IsLeaf() || !pNewNode->IsLeaf())
			throw CommonLib::CExcBase("Node isn't a leaf");

		TKey splitKey;
		int nSplitIndex = pNode->SplitIn(pNewNode, nCount, &splitKey);
		int nInsertIndex = pParentNode->InsertInInnerNode(m_comp, splitKey, pNewNode->GetAddr());
		pNewNode->SetParent(pParentNode, nInsertIndex);

		if (pNode->GetNext() != -1)
		{
			pNewNode->SetNext(pNode->GetNext());				
			TBPTreeNodePtr pNextNode = GetNode(pNode->GetNext());
			pNextNode->SetPrev(pNewNode->GetAddr());

			SetParentForNextNode(pNode, pNextNode);	
			pNextNode->SetFlags(CHANGE_NODE, true);
		}

		pNode->SetNext(pNewNode->GetAddr());
		pNewNode->SetPrev(pNode->GetAddr());
				
		pNode->SetFlags(CHANGE_NODE, true);
		pNewNode->SetFlags(CHANGE_NODE, true);
		pParentNode->SetFlags(CHANGE_NODE, true);
		
	}
	catch (std::exception& exc)
	{
		CommonLib::CExcBase::RegenExcT("TBPlusTreeSetBase failed SplitLeafNode", exc);
	}
}



BPSETBASE_TEMPLATE_PARAMS
void BPSETBASE_DECLARATION::SplitRootInnerNode()
{
	try
	{
		TBPTreeNodePtr pLeftNode = NewNode(false, true);
		TBPTreeNodePtr pRightNode = NewNode(false, true);
		TKey nMedianKey;

		m_pRoot->SplitIn(pLeftNode, pRightNode, &nMedianKey);
		int nIndex = m_pRoot->InsertInInnerNode(m_comp, nMedianKey, pRightNode->GetAddr());
		m_pRoot->SetLess(pLeftNode->GetAddr());

		pLeftNode->SetParent(m_pRoot, -1);
		pRightNode->SetParent(m_pRoot, nIndex);

		SetParentInChildCacheOnly(pLeftNode);
		SetParentInChildCacheOnly(pRightNode);


		pLeftNode->SetFlags(CHANGE_NODE, true);
		pRightNode->SetFlags(CHANGE_NODE, true);
		m_pRoot->SetFlags(CHANGE_NODE, true);
	}
	catch (std::exception& exc)
	{
		CommonLib::CExcBase::RegenExcT("TBPlusTreeSetBase failed TransformRootToInner", exc);
	}

}

BPSETBASE_TEMPLATE_PARAMS
void BPSETBASE_DECLARATION::SetParentInChildCacheOnly(TBPTreeNodePtr& pNode)
{
	try
	{
		TBPTreeNodePtr pLessNode = m_NodeCache.GetElem(pNode->Less(), true);
		if (pLessNode)
			pLessNode->SetParent(pNode, -1);

		for (uint32_t i = 0, sz = pNode->Count(); i < sz; ++i)
		{
			TBPTreeNodePtr pChildNode = m_NodeCache.GetElem(pNode->Link(i), true);
			if (pChildNode.get())
				pChildNode->SetParent(pNode, i);
		}
	}
	catch (std::exception& exc)
	{
		CommonLib::CExcBase::RegenExcT("TBPlusTreeSetBase failed SetParentInChildCacheOnly", exc);
	}
}


BPSETBASE_TEMPLATE_PARAMS
void BPSETBASE_DECLARATION::SplitInnerNode(TBPTreeNodePtr&pNode, TBPTreeNodePtr& pNodeNewRight, TBPTreeNodePtr& pParentNode, int32_t nCount)
{
	try
	{
		if (!pParentNode.get())
		{
			throw CommonLib::CExcBase("Parent node is null");
		}
					   
		TKey nMedianKey;
		pNode->SplitIn(pNodeNewRight, nCount, &nMedianKey);

		SetParentInChildCacheOnly(pNodeNewRight);

		int nIndex = pParentNode->InsertInInnerNode(m_comp, nMedianKey, pNodeNewRight->GetAddr());
		pNodeNewRight->SetParent(pParentNode, nIndex);

		pParentNode->SetFlags(CHANGE_NODE, true);
		pNodeNewRight->SetFlags(CHANGE_NODE, true);
		pNode->SetFlags(CHANGE_NODE, true);

	}
	catch (std::exception& exc)
	{
		CommonLib::CExcBase::RegenExcT("TBPlusTreeSetBase failed SplitInnerNode", exc);
	}
}

