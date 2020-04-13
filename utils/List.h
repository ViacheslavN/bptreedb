#pragma once

#include "../../CommonLib/CommonLib.h"
#include "../../CommonLib/exception/exc_base.h"
#include "../../CommonLib/alloc/alloc.h"
#include "../../CommonLib/alloc/simpleAlloc.h"

namespace bptreedb
{
namespace utils
{
	template <class TypeVal>
	class TList
	{
	public:

		template<class Type>
		class TListNode
		{
		public:

			TListNode(const Type& val) : m_val(val), m_pNext(nullptr), m_pPrev(nullptr)
			{
			}

			~TListNode()
			{

			}

			TListNode *m_pNext;
			TListNode *m_pPrev;
			Type m_val;
		};

		typedef TListNode<TypeVal> TNode;

		class iterator
		{
		public:
			iterator(TNode*  pNode = nullptr) : m_pNode(pNode)
			{}

			void Next()
			{
				if (!m_pNode)
					throw CommonLib::CExcBase(L"List iterator next Node is null");

				m_pNode = m_pNode->m_pNext;
			}

			void Back()
			{
				if (!m_pNode)
					throw CommonLib::CExcBase(L"List iterator back Node is null");

				m_pNode = m_pNode->m_pPrev;
			}

			bool IsNull()
			{
				return m_pNode == nullptr;
			}

			TypeVal & Value() 
			{ 

				if(!m_pNode)
					throw CommonLib::CExcBase(L"List iterator Value Node is null");

				return m_pNode->m_val; 
			}
			const TypeVal & Value() const
			{
				if (!m_pNode)
					throw CommonLib::CExcBase(L"List iterator Value Node is null");

				return m_pNode->m_val; 
			}
			TNode*  Node() { return m_pNode; }
			const TNode* Node() const { return m_pNode; }

			friend class TList;
		protected:
			TNode*   m_pNode;
		};

		TList(std::shared_ptr<CommonLib::IAlloc> pAlloc = std::shared_ptr<CommonLib::IAlloc>()) : m_pAlloc(pAlloc), m_nSize(0), m_pBack(NULL), m_pBegin(NULL)
		{
			if (m_pAlloc.get() == nullptr)
			{
				m_pAlloc = CommonLib::IAlloc::CreateSimpleAlloc();
			}

		}
		~TList()
		{
			Clear();
		}

		iterator PushBack(const TypeVal& val)
		{
			TNode* pElem = new (m_pAlloc->Alloc(sizeof(TNode))) TNode(val);
			return PushBack(pElem);
		}
		iterator PushTop(const TypeVal& val)
		{
			TNode* pElem = new (m_pAlloc->Alloc(sizeof(TNode))) TNode(val);
			return PushTop(pElem);
		}


		iterator PushBack(TNode* pElem)
		{
			if (m_pBack == pElem)
				return iterator(pElem);

			if (m_pBack == nullptr)
				m_pBack = m_pBegin = pElem;
			else
			{
				pElem->m_pPrev = m_pBack;
				pElem->m_pNext = nullptr;
				m_pBack->m_pNext = pElem;
				m_pBack = pElem;
			}
			m_nSize++;
			return iterator(pElem);
		}
		iterator PushTop(TNode* pElem)
		{

			if (m_pBegin == pElem)
				return iterator(pElem);

			if (m_pBegin == nullptr)
				m_pBack = m_pBegin = pElem;
			else
			{
				m_pBegin->m_pPrev = pElem;
				pElem->m_pNext = m_pBegin;
				pElem->m_pPrev = nullptr;
				m_pBegin = pElem;
			}

			m_nSize++;
			return iterator(pElem);
		}

		uint32_t Size() const { return m_nSize; }

		void Clear()
		{

			if (m_pBegin == NULL || m_pBack == NULL)
				return;

			TNode *pNode = m_pBack;
			TNode *pNextNode = pNode->m_pNext;
			if (!pNextNode)
			{
				pNode->~TListNode();
				m_pAlloc->Free(pNode);
				m_pBegin = nullptr;
				m_pBack = nullptr;
				return;
			}

			while (pNextNode)
			{
				pNode->~TListNode();
				m_pAlloc->Free(pNode);
				pNode = pNextNode;
				pNextNode = pNextNode->m_pNext;
			}

			pNode->~TListNode();
			m_pAlloc->Free(pNode);

			m_pBegin = nullptr;
			m_pBack = nullptr;
			m_nSize = 0;
		}

		iterator Begin() { return iterator(m_pBegin); }
		iterator Back() { return iterator(m_pBack); }

		iterator Insert(const iterator& it, const TypeVal& val)
		{
			TNode* pNode = it.m_pNode;
			if (!pNode)
				return iterator(nullptr);

			TNode* pElem = new (m_pAlloc->Alloc(sizeof(TNode))) TNode(val);

			pElem->m_pNext = pNode->m_pNext;
			pElem->m_pPrev = pNode;
			if (pNode->m_pNext)
			{
				pNode->m_pNext->m_pPrev = pElem;
			}
			else
			{
				m_pBegin = pElem;
				pNode->m_pNext = pElem;
			}

			m_nSize++;
			return iterator(pElem);
		}

		iterator Remove(TNode* pNode, bool bDel = true)
		{
			if (!pNode)
				return iterator(nullptr);

			TNode *pPrev = pNode->m_pPrev;
			TNode *pNext = pNode->m_pNext;

			if (pNext)
				pNext->m_pPrev = pPrev;
			if (pPrev)
				pPrev->m_pNext = pNext;

			if (!pNext)
			{
				m_pBack = pPrev;
			}
			
			if (!pPrev)
			{
				m_pBegin = pNext;
			}
			
			if (bDel)
			{
				pNode->~TListNode();
				m_pAlloc->Free(pNode);
			}
			else
			{
				pNode->m_pNext = nullptr;
				pNode->m_pPrev = nullptr;
			}

			m_nSize--;
			return iterator(pNext);
		}

		iterator Remove(const iterator& it, bool bDel = true)
		{
			TNode* pNode = it.m_pNode;
			return Remove(pNode, bDel);
		}

		bool IsTop(const TNode *pNode) const
		{
			return m_pBegin == pNode;
		}

	protected:
		TNode* m_pBegin;
		TNode* m_pBack;
		std::shared_ptr<CommonLib::IAlloc> m_pAlloc;
		uint32_t m_nSize;


	};

}
}