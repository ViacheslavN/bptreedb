#pragma once
BPSETBASE_TEMPLATE_PARAMS
template<class TIterator, class TComparator>
TIterator BPSETBASE_DECLARATION::find(const TComparator& comp, const TKey& key, TIterator *pFromIterator, bool bFindNext)
{
	try
	{
		CommonLib::CPrefCounterHolder holder(m_pBPTreePerfCounter, eFindKey);

		if (pFromIterator)
		{
			TIterator it(this, pFromIterator->m_pCurNode, pFromIterator->m_pCurNode->binary_search(comp, key));
			if (!it.IsNull() || !bFindNext)
				return it;
		}
		
		if (!m_pRoot.get())
			LoadTree();

		if (!m_pRoot.get())
			return TIterator(this, TBPTreeNodePtr(), -1);


		if (m_pRoot->IsLeaf())
		{
			return TIterator(this, m_pRoot, m_pRoot->binary_search(comp, key));
		}

		int32_t nIndex = -1;
		int64_t nNextAddr = m_pRoot->inner_lower_bound(comp, key, nIndex);

		TBPTreeNodePtr pParent = m_pRoot;
		for (;;)
		{
			if (nNextAddr == -1)
			{
				break;
			}
			TBPTreeNodePtr pNode = GetNode(nNextAddr);
			if (!pNode.get())
			{
				return TIterator(this, TBPTreeNodePtr(), -1);
				break;
			}
			pNode->SetParent(pParent, nIndex);

			if (pNode->IsLeaf())
			{
				return TIterator(this, pNode, pNode->binary_search(comp, key));
			}
			nNextAddr = pNode->inner_lower_bound(comp, key, nIndex);
			pParent = pNode;
		}

		return TIterator(this, TBPTreeNodePtr(), -1);
	}
	catch (std::exception& exc)
	{
		CommonLib::CExcBase::RegenExcT("[TBPlusTreeSetBase] failed to find", exc);
		throw;
	}
}


BPSETBASE_TEMPLATE_PARAMS
template<class TIterator>
TIterator BPSETBASE_DECLARATION::begin()
{
	try
	{
		TBPTreeNodePtr pFindBTNode;
		if (!m_pRoot.get())
			LoadTree();

		if (!m_pRoot.get())
			return TIterator(this, pFindBTNode, -1);

		if (m_pRoot->IsLeaf())
			return TIterator(this, m_pRoot, m_pRoot->Count() ? 0 : -1);


		int64_t nNextAddr = m_pRoot->Less();

		TBPTreeNodePtr pParent = m_pRoot;
		for (;;)
		{
			if (nNextAddr == -1)
			{
				break;
			}
			TBPTreeNodePtr pNode = GetNode(nNextAddr);
			pNode->SetParent(pParent, LESS_INDEX);

			if (pNode->IsLeaf())
			{
				pFindBTNode = pNode;
				break;
			}

			nNextAddr = pNode->Less();
			pParent = pNode;
		}

		return TIterator(this, pFindBTNode, 0);
	}
	catch (std::exception& exc)
	{
		CommonLib::CExcBase::RegenExcT("TBPlusTreeSetBase failed to begin", exc);
		throw;
	}
}

BPSETBASE_TEMPLATE_PARAMS
template<class TIterator>
TIterator BPSETBASE_DECLARATION::last()
{
	try
	{
		TBPTreeNodePtr pFindBTNode;
		if (!m_pRoot.get())
			LoadTree();

		if (!m_pRoot.get())
			return TIterator(this, pFindBTNode, -1);

		if (m_pRoot->IsLeaf())
		{
			return TIterator(this, m_pRoot, m_pRoot->Count() - 1);
		}

		int64_t nNextAddr = m_pRoot->BackLink();
		int32_t nIndex = m_pRoot->Count() - 1;

		TBPTreeNodePtr pParent = m_pRoot;
		for (;;)
		{
			if (nNextAddr == -1)
			{
				break;
			}
			TBPTreeNodePtr pNode = GetNode(nNextAddr);
			pNode->SetParent(pParent, nIndex);

			if (pNode->IsLeaf())
			{
				pFindBTNode = pNode;
				break;
			}
			nNextAddr = pNode->BackLink();
			pParent = pNode;
			nIndex = pNode->Count() - 1;
		}

		return TIterator(this, pFindBTNode, pFindBTNode.get() ? pFindBTNode->Count() - 1 : -1);
	}
	catch (std::exception& exc)
	{
		CommonLib::CExcBase::RegenExcT("TBPlusTreeSetBase failed to begin", exc);
	}
}

BPSETBASE_TEMPLATE_PARAMS
template<class TIterator, class TComparator>
TIterator BPSETBASE_DECLARATION::upper_bound(const TComparator& comp, const TKey& key, TIterator *pFromIterator, bool bFindNext)
{
	try
	{
		int32_t nIndex = 0;
		if (!m_pRoot.get())
			LoadTree();

		if (!m_pRoot.get())
			return TIterator(this, TBPTreeNodePtr(), -1);


		if (m_pRoot->IsLeaf())
		{
			return TIterator(this, m_pRoot, m_pRoot->leaf_upper_bound(comp, key));
		}

		nIndex = -1;
		int64_t nNextAddr = m_pRoot->inner_upper_bound(comp, key, nIndex);
		TBPTreeNodePtr pParent = m_pRoot;

		for (;;)
		{
			if (nNextAddr == -1)
			{
				break;
			}

			TBPTreeNodePtr pNode = GetNode(nNextAddr);
			pNode->SetParent(pParent, nIndex);

			if (pNode->IsLeaf())
			{
				return TIterator(this, pNode, pNode->leaf_upper_bound(comp, key));
			}

			nNextAddr = pNode->inner_upper_bound(comp, key, nIndex);
			pParent = pNode;
		}

		return TIterator(this, TBPTreeNodePtr(), -1);
	}
	catch (std::exception& exc)
	{
		CommonLib::CExcBase::RegenExcT("TBPlusTreeSetBase failed upper_bound", exc);
	}
}

BPSETBASE_TEMPLATE_PARAMS
template<class TIterator, class TComparator>
TIterator BPSETBASE_DECLARATION::lower_bound(const TComparator& comp, const TKey& key, TIterator *pFromIterator, bool bFindNext)
{
	try
	{
		if (pFromIterator)
		{
			if (pFromIterator->IsNull() && !bFindNext)
				return TIterator(this, TBPTreeNodePtr(), -1);
		}

		int32_t nIndex = 0;
		if (!m_pRoot.get())
			LoadTree();

		if (!m_pRoot.get())
			return TIterator(this, TBPTreeNodePtr(), -1);

		nIndex = -1;
		if (m_pRoot->IsLeaf())
		{
			return TIterator(this, m_pRoot, m_pRoot->leaf_lower_bound(comp, key));
		}

		int64_t nNextAddr = m_pRoot->inner_lower_bound(comp, key, nIndex);
		TBPTreeNodePtr pParent = m_pRoot;
		for (;;)
		{
			if (nNextAddr == -1)
			{
				break;
			}

			TBPTreeNodePtr pNode = GetNode(nNextAddr);
			pNode->SetParent(pParent, nIndex);
			if (pNode->IsLeaf())
			{
				int32_t nLeafIndex = pNode->leaf_lower_bound(comp, key);
				if (nLeafIndex != -1 && nLeafIndex < (int32_t)pNode->Count())
					return TIterator(this, pNode, nLeafIndex);

				else if (nIndex < ((int32_t)pParent->Count() - 1))
				{
					TBPTreeNodePtr pNode = GetNode(pParent->link(nIndex + 1));
					pNode->SetParent(pParent, nIndex + 1);
					return TIterator(this, pNode, 0);
				}

				if (pNode->next() == -1)
					return TIterator(this, TBPTreeNodePtr(), -1);

				TBPTreeNodePtr pNodeNext = GetNode(pNode->GetNext());
				SetParentForNextNode(pNode.get(), pNodeNext);
				return TIterator(this, pNodeNext, 0);
			}
			nNextAddr = pNode->inner_lower_bound(comp, key, nIndex);
			pParent = pNode;
		}
		return TIterator(this, TBPTreeNodePtr(), -1);
	}
	catch (std::exception& exc)
	{
		CommonLib::CExcBase::RegenExcT("TBPlusTreeSetBase failed lower_bound", exc);
	}
}