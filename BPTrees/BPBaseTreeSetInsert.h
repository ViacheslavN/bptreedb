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

		return true;
	}
	catch (std::exception& exc)
	{
		CommonLib::CExcBase::RegenExcT("TBPlusTreeSetBase failed findLeafNodeForInsert", exc);
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
		int nSplitIndex = m_pRoot->SplitIn(pLeftNode.get(), pRightNode.get(), &splitKey);

		m_pRoot->Clear();
		m_pRoot->TransformToInner(m_pAlloc, m_bMulti, m_nNodePageSize, m_InnerCompressParams);

		m_pRoot->SetLess(pLeftNode->addr());
		int nInsertIndex = m_pRoot->InsertInInnerNode(m_comp, splitKey, pRightNode->GetAddr());

		pRightNode->SetParent(m_pRoot, nInsertIndex);
		pLeftNode->SetParent(m_pRoot, -1);
		pLeftNode->SetNext(pRightNode->addr());
		pRightNode->SetPrev(pLeftNode->addr());

		pLeftNode->SetFlags(CHANGE_NODE, true);
		pRightNode->SetFlags(CHANGE_NODE, true);
		m_pRoot->SetFlags(CHANGE_NODE, true);
	}
	catch (std::exception& exc)
	{
		CommonLib::CExcBase::RegenExcT("TBPlusTreeSetBase failed TransformRootToInner", exc);
	}
}