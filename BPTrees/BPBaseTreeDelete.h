#pragma once


BPSETBASE_TEMPLATE_PARAMS
BPSETBASE_TYPENAME_DECLARATION::TBPTreeNodePtr BPSETBASE_DECLARATION::FindLeafNodeForDelete(const TKey& key)
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
		int64_t nNextAddr = m_pRoot->FindNodeRemove(m_comp, key, nIndex);
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
			nNextAddr = pNode->FindNodeRemove(m_comp, key, nIndex);
		}

		return TBPTreeNodePtr();
	}
	catch (std::exception& exc)
	{
		CommonLib::CExcBase::RegenExcT("[TBPlusTreeSetBase] failed FindLeafNodeForDelete", exc);
		throw;
	}
}


BPSETBASE_TEMPLATE_PARAMS
bool BPSETBASE_DECLARATION::remove(const TKey& key)
{
	try
	{
		CommonLib::CPrefCounterHolder holder(m_pBPTreePerfCounter, eRemoveValue);

		TBPTreeNodePtr pNode = FindLeafNodeForDelete(key);
		if (!pNode.get())
			return false;

		if(!pNode->IsLeaf())
			throw CommonLib::CExcBase("Node %1 isn't leaf", pNode->GetAddr());

		int32_t nIndex = pNode->binary_search(m_comp, key);

		if (nIndex == -1)
			return false;


		RemoveFromLeafNode(pNode, nIndex, key);

	}
	catch (std::exception& exc)
	{
		CommonLib::CExcBase::RegenExcT("[TBPlusTreeSetBase] failed remove", exc);
		throw;
	}

	return true;
}


BPSETBASE_TEMPLATE_PARAMS
void BPSETBASE_DECLARATION::RemoveFromLeafNode(TBPTreeNodePtr pNode, int32_t nIndex, const TKey& key)
{
	try
	{
		pNode->RemoveByIndex(nIndex);
		pNode->SetFlags(CHANGE_NODE, true);

		if (pNode->GetAddr() == m_nRootAddr)
			return;

		int32_t nFoundIndex = pNode->GetFoundIndex();
		TBPTreeNodePtr pParentNode = GetParentNode(pNode);
		if (!pParentNode.get())
			return;


		if (nFoundIndex != LESS_INDEX && nIndex == 0)
		{
			pParentNode->UpdateKey(nFoundIndex, pNode->Key(0));
			pParentNode->SetFlags(CHANGE_NODE, true);
		}

		if (!pNode->IsHalfEmpty())
		{
			return 	RemoveFromInnerNode(pParentNode, key);
		}



		TBPTreeNodePtr pDonorNode;
		bool bLeft = false; //The position of the donor node relative to the node
		bool bUnion = false;
		bool bAlignment = false;

		if (pParentNode->Less() == pNode->GetAddr())
		{
			pDonorNode = GetNode(pParentNode->Link(0));
			pDonorNode->SetParent(pParentNode, 0);

			bUnion = pNode->PossibleUnion(pDonorNode);
			if (!bUnion)
				bAlignment = pNode->PossibleAlignment(pDonorNode);
		}
		else
		{

			TBPTreeNodePtr pLeafNodeRight;
			TBPTreeNodePtr pLeafNodeLeft;

			if (nFoundIndex == 0)
			{
				pLeafNodeLeft = GetNode(pParentNode->Less());
				pLeafNodeLeft->SetParent(pParentNode, LESS_INDEX);

				if (pParentNode->Count() > 1)
				{
					pLeafNodeRight = GetNode(pParentNode->Link(1));
					pLeafNodeRight->SetParent(pParentNode, 1);
				}
			}
			else
			{
				pLeafNodeLeft = GetNode(pParentNode->Link(nFoundIndex - 1));
				pLeafNodeLeft->SetParent(pParentNode, nFoundIndex - 1);

				if ((int32_t)pParentNode->Count() > nFoundIndex + 1)
				{
					pLeafNodeRight = GetNode(pParentNode->Link(nFoundIndex + 1));
					pLeafNodeRight->SetParent(pParentNode, nFoundIndex + 1);
				}
			}

			if (pLeafNodeLeft) // left is preferable
			{
				bUnion = pNode->PossibleUnion(pLeafNodeLeft);
				if (bUnion)
				{
					pDonorNode = pLeafNodeLeft;
					bLeft = true;
				}
				else
				{
					bAlignment = pNode->PossibleAlignment(pLeafNodeLeft);
					pDonorNode = pLeafNodeLeft;
					bLeft = true;
				}
			}

			if (pDonorNode.get() == nullptr && pLeafNodeRight.get() != nullptr)
			{
				bUnion = pNode->PossibleUnion(pLeafNodeRight);
				if (bUnion)
				{
					pDonorNode = pLeafNodeLeft;
					bLeft = false;
				}
				else
				{
					bAlignment = pNode->PossibleAlignment(pLeafNodeRight);
					pDonorNode = pLeafNodeRight;
					bLeft = false;
				}
			}
		}

		if (bUnion)
		{

			if (pDonorNode->GetFoundIndex() == -1)
			{
				UnionLeafNode(pParentNode, pDonorNode, pNode, false); //union in less node
				nFoundIndex = LESS_INDEX;
			}
			else
			{
				UnionLeafNode(pParentNode, pNode, pDonorNode, bLeft);
				nFoundIndex = pNode->GetFoundIndex();
			}

		}
		else if (bAlignment)
		{
			AlignmentLeafNode(pParentNode, pNode, pDonorNode, bLeft);
			nFoundIndex = pNode->GetFoundIndex();
		}

		if (nFoundIndex != LESS_INDEX && pParentNode->IsKey(m_comp, key, nFoundIndex))
		{
			TBPTreeNodePtr pIndexNode = GetNode(pParentNode->Link(nFoundIndex));
			pParentNode->UpdateKey(nFoundIndex, pIndexNode->Key(0));
			pParentNode->SetFlags(CHANGE_NODE, true);
		}
		RemoveFromInnerNode(pParentNode, key);
	}
	catch (std::exception& exc)
	{
		CommonLib::CExcBase::RegenExcT("[TBPlusTreeSetBase] failed remove from leaf node", exc);
		throw;
	}
}


BPSETBASE_TEMPLATE_PARAMS
void BPSETBASE_DECLARATION::UnionLeafNode(TBPTreeNodePtr pParentNode, TBPTreeNodePtr pLeafNode, TBPTreeNodePtr pDonorNode, bool bLeft)
{
	try
	{
		pLeafNode->UnionWithLeafNode(pDonorNode, bLeft, nullptr);
		pLeafNode->SetFlags(CHANGE_NODE, true);
		if (bLeft)
		{
			TBPTreeNodePtr pPrevNode = GetNode(pDonorNode->GetPrev());
			if (pPrevNode.get())
			{
				pLeafNode->SetPrev(pPrevNode->GetAddr());
				pPrevNode->SetNext(pLeafNode->GetAddr());
				pPrevNode->SetFlags(CHANGE_NODE, true);
			}
			else
				pLeafNode->SetPrev(EMPTY_PAGE_ADDR);
		}
		else
		{
			TBPTreeNodePtr pNextNode = GetNode(pDonorNode->GetNext());
			if (pNextNode.get())
			{
				pLeafNode->SetNext(pNextNode->GetAddr());
				pNextNode->SetPrev(pLeafNode->GetAddr());
				pNextNode->SetFlags(CHANGE_NODE, true);
			}
			else
				pLeafNode->SetNext(EMPTY_PAGE_ADDR);
		}

		pParentNode->RemoveByIndex(pDonorNode->GetFoundIndex());

		if (bLeft && pLeafNode->GetFoundIndex() != -1)
		{
			pLeafNode->SetFoundIndex(pLeafNode->GetFoundIndex() - 1);
			pParentNode->UpdateKey(pLeafNode->GetFoundIndex(), pLeafNode->Key(0));
		}

		DeleteNode(pDonorNode);
		pParentNode->SetFlags(CHANGE_NODE | CHECK_REM_NODE, true);
	}
	catch (std::exception& exc)
	{
		CommonLib::CExcBase::RegenExcT("[TBPlusTreeSetBase] failed to union from leaf nodes", exc);
		throw;
	}
}


BPSETBASE_TEMPLATE_PARAMS
void BPSETBASE_DECLARATION::DeleteNode(TBPTreeNodePtr pNode)
{
	m_pStorage->DropFilePage(pNode->GetAddr());
	pNode->SetFlags(REMOVE_NODE, true);
	m_NodeCache.Remove(pNode->GetAddr());
}

BPSETBASE_TEMPLATE_PARAMS
void BPSETBASE_DECLARATION::AlignmentLeafNode(TBPTreeNodePtr pParentNode, TBPTreeNodePtr pLeafNode, TBPTreeNodePtr pDonorNode, bool bLeft)
{
	try
	{
		if (!pLeafNode->AlignmenLeftNodeOf(pDonorNode, bLeft))
			return;

		if (bLeft)
			pParentNode->UpdateKey(pLeafNode->GetFoundIndex(), pLeafNode->Key(0));
		else
			pParentNode->UpdateKey(pDonorNode->GetFoundIndex(), pDonorNode->Key(0));

		pLeafNode->SetFlags(CHANGE_NODE, true);
		pDonorNode->SetFlags(CHANGE_NODE, true);
		pParentNode->SetFlags(CHANGE_NODE, true);
	}
	catch (std::exception& exc)
	{
		CommonLib::CExcBase::RegenExcT("[TBPlusTreeSetBase] failed to alignment leaf nodes", exc);
		throw;
	}
}

BPSETBASE_TEMPLATE_PARAMS
void BPSETBASE_DECLARATION::RemoveFromInnerNode(TBPTreeNodePtr pCheckNode, const TKey& key)
{
	try
	{
		while (pCheckNode.get())
		{
			TBPTreeNodePtr  pParentNode = GetParentNode(pCheckNode);
			if (!pParentNode.get())
			{
				if (!pCheckNode->Count())
				{
					TBPTreeNodePtr pNode = GetNode(pCheckNode->Less());

					if (pNode->IsLeaf())
					{
						pCheckNode->Clear();
						pCheckNode->TransformToLeaf(m_pAllocsSet, m_bMulti, m_nNodePageSize, m_pCompressParams);
						pCheckNode->UnionWithLeafNode(pNode, false);
						pCheckNode->SetFlags(ROOT_NODE, true);
					}
					else
					{
						pCheckNode->SetLess(EMPTY_PAGE_ADDR);
						pCheckNode->UnionWithInnerNode(pNode, NULL, true);
					}
					m_NodeCache.Remove(pNode->GetAddr());
					DeleteNode(pNode);
				}
				return;
			}


			if (pCheckNode->GetFoundIndex() != -1 && pParentNode->IsKey(m_comp, key, pCheckNode->GetFoundIndex()))
			{
				TBPTreeNodePtr pIndexNode = GetNode(pParentNode->Link(pCheckNode->GetFoundIndex()));
				TBPTreeNodePtr pMinNode = GetMinimumNode(pIndexNode);
				pParentNode->UpdateKey(pCheckNode->GetFoundIndex(), pMinNode->Key(0));
				pParentNode->SetFlags(CHANGE_NODE, true);
 
			}
			if (!(pCheckNode->GetFlags() & CHECK_REM_NODE))
			{
				pCheckNode = GetParentNode(pCheckNode);
				continue;
			}
			pCheckNode->SetFlags(CHECK_REM_NODE, false);

			if (!pCheckNode->IsHalfEmpty())
			{
				pCheckNode = GetParentNode(pCheckNode);
				continue;
			}

			TBPTreeNodePtr pDonorNode;
			bool bLeft = false; //The position of the donor node relative to the node
			bool bUnion = false;
			bool bAlignment = false;


			if (pParentNode->Less() == pCheckNode->GetAddr())
			{
				pDonorNode = GetNode(pParentNode->Link(0));
				pDonorNode->SetParent(pParentNode, 0);


				bUnion = pCheckNode->PossibleUnion(pDonorNode);
				if (!bUnion)
					bAlignment = pCheckNode->PossibleAlignment(pDonorNode);
			}
			else
			{

				TBPTreeNodePtr pLeafNodeRight;
				TBPTreeNodePtr pLeafNodeLeft;

				if (pCheckNode->GetFoundIndex() == 0)
				{
					pLeafNodeLeft = GetNode(pParentNode->Less());
					pLeafNodeLeft->SetParent(pParentNode, LESS_INDEX);
					if (pParentNode->Count() > 1)
					{
						pLeafNodeRight = GetNode(pParentNode->Link(1));
						pLeafNodeRight->SetParent(pParentNode, 1);
					}
				}
				else
				{
					pLeafNodeLeft = GetNode(pParentNode->Link(pCheckNode->GetFoundIndex() - 1));
					pLeafNodeLeft->SetParent(pParentNode, pCheckNode->GetFoundIndex() - 1);

					if ((int32_t)pParentNode->Count() > pCheckNode->GetFoundIndex() + 1)
					{
						pLeafNodeRight = GetNode(pParentNode->Link(pCheckNode->GetFoundIndex() + 1));
						pLeafNodeRight->SetParent(pParentNode, pCheckNode->GetFoundIndex() + 1);
					}
				}
 

				uint32_t nLeftCount = pLeafNodeLeft.get() ? pLeafNodeLeft->Count() : 0;
				uint32_t nRightCount = pLeafNodeRight.get() ? pLeafNodeRight->Count() : 0;
				if (nLeftCount < nRightCount)
				{
					pDonorNode = pLeafNodeLeft;
					bLeft = true;
				}
				else
				{
					pDonorNode = pLeafNodeRight;
					bLeft = false;
				}
			}

			bUnion = pCheckNode->PossibleUnion(pDonorNode);
			if (!bUnion && !m_bMinSplit)
			{
				bAlignment = pCheckNode->PossibleAlignment(pDonorNode);
			}

			if (bUnion)
			{
				if (pDonorNode->GetAddr() == pParentNode->Less())
					UnionInnerNode(pParentNode, pDonorNode, pCheckNode, false);
				else
					UnionInnerNode(pParentNode, pCheckNode, pDonorNode, bLeft);
			}
			else if (bAlignment)
			{
				AlignmentInnerNode(pParentNode, pCheckNode, pDonorNode, bLeft);
			}

			pCheckNode = pParentNode;
		}

	}
	catch (std::exception& exc)
	{
		CommonLib::CExcBase::RegenExcT("[TBPlusTreeSetBase] failed to remove from inner node", exc);
		throw;
	}
}



BPSETBASE_TEMPLATE_PARAMS
void BPSETBASE_DECLARATION::UnionInnerNode(TBPTreeNodePtr pParentNode, TBPTreeNodePtr pNode, TBPTreeNodePtr pDonorNode, bool bLeft)
{
	try
	{
		TBPTreeNodePtr pMinNode = bLeft ? GetMinimumNode(GetNode(pNode->Less())) : GetMinimumNode(GetNode(pDonorNode->Less()));
		pNode->UnionWithInnerNode(pDonorNode, &pMinNode->Key(0), bLeft);
		pNode->SetFlags(CHANGE_NODE, true);

		pParentNode->RemoveByIndex(pDonorNode->GetFoundIndex());
		if (bLeft && pNode->GetFoundIndex() != -1)
		{
			pNode->SetFoundIndex(pNode->GetFoundIndex() - 1);
 
			pMinNode = GetMinimumNode(GetNode(pNode->Less()));
			pParentNode->UpdateKey(pNode->GetFoundIndex(), pMinNode->Key(0));
		}

		DeleteNode(pDonorNode);
		SetParentInChildCacheOnly(pNode);
 
		pParentNode->SetFlags(CHANGE_NODE | CHECK_REM_NODE, true);
	}
	catch (std::exception& exc)
	{
		CommonLib::CExcBase::RegenExcT("[TBPlusTreeSetBase] failed to union inners node", exc);
		throw;
	}
}



BPSETBASE_TEMPLATE_PARAMS
void BPSETBASE_DECLARATION::AlignmentInnerNode(TBPTreeNodePtr pParentNode, TBPTreeNodePtr pNode, TBPTreeNodePtr pDonorNode, bool bLeft)
{

	TBPTreeNodePtr pMinNode = bLeft ? GetMinimumNode(GetNode(pNode->Less())) : GetMinimumNode(GetNode(pDonorNode->Less()));
	if (!pNode->AlignmentInnerNodeOf(pDonorNode, pMinNode->Key(0), bLeft))
		return;

	if (!bLeft) // Node donor is on the right
	{
		pMinNode = GetMinimumNode(GetNode(pDonorNode->Less()));
		pParentNode->UpdateKey(pDonorNode->GetFoundIndex(), pMinNode->Key(0));
		pParentNode->SetFlags(CHANGE_NODE, true);
	}
	else
	{
		pMinNode = GetMinimumNode(GetNode(pNode->Less()));
		pParentNode->UpdateKey(pNode->GetFoundIndex(), pMinNode->Key(0));
		pParentNode->SetFlags(CHANGE_NODE, true);
	}

	SetParentInChildCacheOnly(pNode);
	SetParentInChildCacheOnly(pDonorNode);

	pParentNode->SetFlags(CHANGE_NODE, true);
	pNode->SetFlags(CHANGE_NODE, true);
	pDonorNode->SetFlags(CHANGE_NODE, true);
}
