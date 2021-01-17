#pragma once
#include "BPTreeNodeHolder.h"

namespace bptreedb
{
	
	template <class _TKey, class _TBTreeNode, class _TBTree>
	class TBPSetIterator
	{
	public:
		typedef _TKey      TKey;
		typedef _TBTreeNode TBTreeNode;
		typedef std::shared_ptr<TBTreeNode> TBPTreeNodePtr;
		typedef _TBTree TBTree;

		TBPSetIterator(TBTree *pTree, TBPTreeNodePtr pCurNode, int32_t nIndex) :
			m_pTree(pTree), m_pCurNode(pCurNode), m_nIndex(nIndex)
		{
		}

		TBPSetIterator() : m_pTree(nullptr), m_nIndex(-1)
		{

		}

		TBPSetIterator(const TBPSetIterator& iter)
		{
			m_pCurNode = iter.m_pCurNode;
			m_pTree = iter.m_pTree;
			m_nIndex = iter.m_nIndex;
		}

		~TBPSetIterator()
		{

		}

		TBPSetIterator& operator = (const TBPSetIterator& iter)
		{
			m_pCurNode = iter.m_pCurNode;
			m_pTree = iter.m_pTree;
			m_nIndex = iter.m_nIndex;;

			return *this;
		}

		const TKey& Key() const
		{
			return m_pCurNode->Key((uint32_t)m_nIndex);
		}

		TKey& Key()
		{
			return m_pCurNode->Key((uint32_t)m_nIndex);
		}

		bool IsNull() const
		{
			return m_pCurNode.get() == NULL || m_nIndex == -1;
		
		}

		bool Next()
		{
			try
			{
				if (IsNull())
					return false;

				m_nIndex++;
				if ((uint32_t)m_nIndex < m_pCurNode->Count())
					return true;

				if (m_pCurNode->GetNext() != -1)
				{
					TBPTreeNodePtr pNode = m_pTree->GetNode(m_pCurNode->GetNext());
					if (!pNode.get())
						throw CommonLib::CExcBase("Failed to get node %1", m_pCurNode->GetNext());
			
				//	m_pTree->SetParentForNextNode(m_pCurNode, pNode);
					m_nIndex = 0;
					m_pCurNode = pNode;
					m_pTree->CheckCache();
					return true;
				}
				m_pCurNode = NULL;
				m_nIndex = -1;
				return false;
			}
			catch (std::exception& exc)
			{
				CommonLib::CExcBase::RegenExcT("[TBPSetIterator] failed to next step", exc);
				throw;
			}
 
		}

		void Update()
		{
			if(IsNull())
				throw CommonLib::CExcBase("[TBPSetIterator] Update node is null");

			m_pCurNode->SetFlags(CHANGE_NODE, true);
		}

		bool Back()
		{
			try
			{
				if (IsNull())
					return false;

				m_nIndex--;
				if (m_nIndex >= 0)
					return true;

				if (m_pCurNode->prev() != -1)
				{
					TBPTreeNodePtr pNode = m_pTree->GetNode(m_pCurNode->Prev());
					if (!pNode.get())
						throw CExcBase("Failed to get node %1", m_pCurNode->Prev());
				
					m_pCurNode = pNode;
					m_nIndex = m_pCurNode->Count() - 1;
					m_pTree->CheckCache();
					return true;
				}
				return false;
			}
			catch (std::exception& exc)
			{
				CommonLib::CExcBase::RegenExcT("[TBPSetIterator] failed to next step", exc);
			}
		}

		int64_t Addr() const
		{
			if (IsNull())
				return -1;

			return m_pCurNode->GetAddr();
		}

		int32_t Pos() const
		{
			return m_nIndex;
		}

	/*	int64_t GetParentAddr()
		{
			if (IsNull())
				return -1;

			return m_pCurNode->GetParentAddr();
		}

		int32_t GetFoundIndex()
		{
			if (IsNull())
				return -1;

			return m_pCurNode->GetFoundIndex();
		}*/

		void SetAddr(int64_t nAddr, int32_t  nPos)
		{
			try
			{
				if (nAddr == -1)
				{
					m_pCurNode = NULL;
					return;
				}

				TBPTreeNodePtr pNode = m_pTree->GetNodeAndCheckParent(nAddr);
				if (!pNode.get())
					throw CExcBase("Failed to get node %1", m_pCurNode->Prev());
				
				m_pCurNode = pNode;
				m_nIndex = nPos;
			}
			catch (std::exception& exc)
			{
				CommonLib::CExcBase::RegenExcT("[TBPSetIterator] failed SetAddr addr %1, pos %2", nAddr, nPos, exc);
			}
		}

	public:
		TBTree *m_pTree;
		TBPTreeNodePtr m_pCurNode;
		int32_t m_nIndex;
	};
}
