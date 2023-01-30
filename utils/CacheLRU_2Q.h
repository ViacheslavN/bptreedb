#pragma once

#include "../../CommonLib/CommonLib.h"
#include "../../CommonLib/exception/exc_base.h"
#include "../../CommonLib/alloc/alloc.h"
#include "../../CommonLib/alloc/simpleAlloc.h"
#include "List.h"


namespace bptreedb
{
	namespace utils
	{
		template<class TObj>
		struct TEmptyFreeChecker
		{
			bool IsFree(TObj& pObj)
			{
				return true;
			}
		};

		template<class TKey, class TObj, class TFreeChecker = TEmptyFreeChecker<TObj> >
		class TCacheLRU_2Q
		{

			enum eQueueType
			{
				UNDEFINED = 0,
				TOP = 1,
				BACK = 2
			};

			struct TCacheVal
			{
				TCacheVal() : m_type(UNDEFINED)
				{}
				TCacheVal(const TKey key, TObj obj, eQueueType type) : m_key(key), m_obj(obj), m_type(type)
				{}

				~TCacheVal()
				{}

				TKey m_key;
				TObj m_obj;
				eQueueType m_type;
			};

			typedef  TList<TCacheVal> QList;
			typedef typename QList::TNode TListNode;
			typedef std::map<TKey, TListNode*> TCacheMap;

		public:
			typedef typename QList::iterator TListIterator;

			TCacheLRU_2Q(CommonLib::IAllocPtr ptrAlloc = CommonLib::IAllocPtr(), TObj nullObj = TObj()) :
				m_pAlloc(ptrAlloc), m_TopList(ptrAlloc), m_BackList(ptrAlloc),
				m_NullObj(nullObj)
			{
				if (m_pAlloc.get() == nullptr)
					m_pAlloc = CommonLib::IAlloc::CreateSimpleAlloc();
			}

			~TCacheLRU_2Q()
			{
				try
				{
					Clear();
				}
				catch (CommonLib::CExcBase& exc)
				{
					exc;
					//TO DO log
				}
			}

			class iterator
			{
			public:
				iterator(typename TCacheMap::iterator start, typename TCacheMap::iterator end) : m_itBegin(start), m_itEnd(end)
				{			

				}

				bool IsNull()
				{
					return m_itBegin == m_itEnd;
				}

				TObj& Object() { return  m_itBegin->second->m_val.m_obj; }
				const TKey& Key() { return m_itBegin->first;}

				void Next()
				{
					try
					{
						++m_itBegin;
					}
					catch (CommonLib::CExcBase& exc)
					{
						exc.AddMsg(L"CacheRU_2Q Next");
						throw;
					}

				}

			private:
				typename TCacheMap::iterator m_itBegin;
				typename TCacheMap::iterator m_itEnd;
			};

			void AddElem(const TKey& key, TObj& pObj, bool bAddBack = true)
			{

				if (m_CacheMap.find(key) != m_CacheMap.end())
					throw CommonLib::CExcBase("CacheRU_2Q:  element with key %1 exists", key);

				TListIterator it;
				if (bAddBack)
					it = m_BackList.PushTop(TCacheVal(key, pObj, BACK));
				else
					it = m_TopList.PushBack(TCacheVal(key, pObj, TOP));

				m_CacheMap.insert(std::make_pair(key, it.Node()));
			}


			void AddElemIfNoExists(const TKey& key, TObj& pObj, bool bAddBack = true)
			{

				if (m_CacheMap.find(key) != m_CacheMap.end())
					return;

				AddElem(key, pObj, bAddBack);
			}

			uint32_t Size() const { return (uint32_t)m_CacheMap.size(); }

			void Clear()
			{
				m_CacheMap.clear();
				m_TopList.Clear();
				m_BackList.Clear();
			}

			TObj Remove(const TKey& key)
			{
				typename TCacheMap::iterator it = m_CacheMap.find(key);
				if (it == m_CacheMap.end())
					return m_NullObj;

				typename QList::TNode* pNode = it->second;
				TCacheVal& cacheVal = pNode->m_val;
				TObj pObj = cacheVal.m_obj;

				if (cacheVal.m_type == BACK)
					m_BackList.Remove(pNode);
				else
					m_TopList.Remove(pNode);

				m_CacheMap.erase(it);
				return pObj;
			}

			TObj RemoveBack()
			{
				TObj pObj = RemoveBack(m_BackList);
				if (pObj != m_NullObj)
					MoveFromTop();
				else
					pObj = RemoveBack(m_TopList);

				return pObj;
			}

			TObj& GetElem(const TKey& key, bool bNotMove = false)
			{
				typename TCacheMap::iterator it = m_CacheMap.find(key);
				if (it == m_CacheMap.end())
					return m_NullObj;

				typename QList::TNode* pNode = it->second;
				TCacheVal& cacheVal = pNode->m_val;

				if (bNotMove)
					return cacheVal.m_obj;

				if (cacheVal.m_type == BACK)
				{

					if (!m_BackList.IsTop(pNode))
					{
						m_BackList.Remove(pNode, false);
						m_BackList.PushTop(pNode);
					}
					else
					{
						m_BackList.Remove(pNode, false);
						cacheVal.m_type = TOP;
						m_TopList.PushTop(pNode);
					}

				}
				else
				{
					if (!m_TopList.IsTop(pNode))
					{
						m_TopList.Remove(pNode, false);
						m_TopList.PushTop(pNode);
					}
				}

				return cacheVal.m_obj;
			}

			iterator Begin() { return iterator(m_CacheMap.begin(), m_CacheMap.end()); }

		private:

			void MoveFromTop()
			{
				auto listIt = m_TopList.Back();
				if (!listIt.IsNull())
				{
					m_TopList.Remove(listIt, false);
					TCacheVal& cacheVal = listIt.Node()->m_val;
					cacheVal.m_type = BACK;
					m_BackList.PushTop(listIt.Node());
				}
			}

			TObj RemoveBack(QList& list)
			{
				auto listIt = list.Back();
				if (listIt.IsNull())
					return m_NullObj;

				while (!listIt.IsNull())
				{

					if (m_FreeChecker.IsFree(listIt.Value().m_obj))
					{
						TObj pObj = listIt.Value().m_obj;
						m_CacheMap.erase(listIt.Value().m_key);
						list.Remove(listIt);
						return pObj;
					}
					else
					{
						listIt.Next();
					}
				}

				return m_NullObj;
			}

		private:

	

			std::shared_ptr<CommonLib::IAlloc> m_pAlloc;
			TCacheMap m_CacheMap;
			QList m_TopList;
			QList m_BackList;
			TFreeChecker m_FreeChecker;

			TObj m_NullObj;
		};

	}
}