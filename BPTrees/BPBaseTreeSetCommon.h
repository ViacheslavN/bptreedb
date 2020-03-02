#pragma once


BPSETBASE_TEMPLATE_PARAMS
IBPTreeNodePtr BPSETBASE_DECLARATION::GetNode(int64_t nAddr)
{
	 try
	 {

		 IBPTreeNodePtr pNode = m_NodeCache.GetElem(nAddr);
		 if (pNode.get() != nullptr)
			 return LoadNodeFromStorage(nAddr);

		 return pNode;
	 }
	 catch (std::exception& exc)
	 {
		 CommonLib::CExcBase::RegenExcT("TBPlusTreeSetBase failed to get node addr %1", nAddr, exc);
	 }
}

BPSETBASE_TEMPLATE_PARAMS
IBPTreeNodePtr BPSETBASE_DECLARATION::LoadNodeFromStorage(int64_t nAddr)
{
	try
	{



	}
	catch (std::exception& exc)
	{
		CommonLib::CExcBase::RegenExcT("TBPlusTreeSetBase failed to load node from storage addr %1", nAddr, exc);
	}
}


 