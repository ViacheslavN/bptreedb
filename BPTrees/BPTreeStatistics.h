#pragma once

#include "../../CommonLib/CommonLib.h"
#include "../../CommonLib/exception/exc_base.h"
#include "../../CommonLib/alloc/alloc.h"
#include "../../CommonLib/alloc/simpleAlloc.h"
#include "../../CommonLib/alloc/stl_alloc.h"


namespace bptreedb
{

	class IReadTreeCancel
	{
		public:
			virtual bool IsCancel() = 0;
	};

	template <class _TKey, class _TBTreeNode, class _TBTree>
	class TBPTreeStatistics
	{
	public:
		typedef _TKey      TKey;
		typedef int64_t     TLink;
		typedef _TBTreeNode TBTreeNode;
		typedef std::shared_ptr<TBTreeNode> TBTreeNodePtr;

		typedef CommonLib::STLAllocator<TKey> TAlloc;
		typedef std::vector<TKey, TAlloc> TKeyMemSet;
		typedef _TBTree TBTree;

		struct SNodeInfo
		{

			SNodeInfo(TBTreeNodePtr _pBTNode, int32_t _nPos) : pBTNode(_pBTNode), nPos(_nPos)
			{}

			TBTreeNodePtr pBTNode;
			int32_t nPos;
		};

		TBPTreeStatistics(TBTree *pTree) :
			m_pTree(pTree), m_bFinishRead(false)
		{
 
		}

		~TBPTreeStatistics()
		{

		}

		void CalcNodesInTree(IReadTreeCancel *pCancel = nullptr)
		{
			try
			{

				m_bFinishRead = false;
				Clear();

				TBTreeNodePtr pRootNode = m_pTree->Root();

				if (!pRootNode.get())
					throw CommonLib::CExcBase("Root not found");


				if (pRootNode->IsLeaf())
				{
					m_nLeafNodeCount += 1;
					m_bFinishRead = true;
				}
				else
					m_nInnerNodeCount += 1;

				std::stack<SNodeInfo> nNodes;

				nNodes.push(SNodeInfo(pRootNode, -1));

				TBTreeNodePtr pNode;
				while (!nNodes.empty())
				{
					auto& node = nNodes.top();
					if (node.nPos == node.pBTNode->Count())
					{
						nNodes.pop();
						continue;
					}

					int32_t nPos = node.nPos++;



					pNode = m_pTree->GetNode(nPos == -1 ? node.pBTNode->Less() : node.pBTNode->Link(nPos));
					if (!pNode->IsLeaf())
					{
						if (nPos == -1)
						{
							m_nInnerNodeCount += (node.pBTNode->Count() + 1);
						}

						nNodes.push(SNodeInfo(pNode, -1));
						continue;
					}

					node.nPos = node.pBTNode->Count();
					m_setHeights.insert(nNodes.size());
					CountLeafNode(node.pBTNode, pNode);
				}
				
				m_bFinishRead = true;
			}
			catch (std::exception& exc)
			{
				CommonLib::CExcBase::RegenExcT("[TBPTreeStatistics] failed CalcNodesInTree", exc);
				throw;
			}
		}


		void Clear()
		{
			m_nLeafNodeCount = 0;
			m_nInnerNodeCount = 0;
			m_nKeyCount = 0;
			m_nHeight = 0;
			m_setHeights.clear();
		}

		void CountLeafNode(TBTreeNodePtr pParentNode, TBTreeNodePtr pNode)
		{
			m_nLeafNodeCount += (pParentNode->Count() + 1);
			m_nKeyCount += (pParentNode->Count() + 1) *pNode->Count();
		}
		


	public:

		TBTree *m_pTree{ nullptr };
		bool m_bFinishRead{ false };

		uint64_t m_nLeafNodeCount{ 0 };
		uint64_t m_nInnerNodeCount{ 0 };
		uint64_t m_nKeyCount{ 0 };
		uint64_t m_nHeight{ 0 };
		std::set<uint64_t> m_setHeights;
	};

}
