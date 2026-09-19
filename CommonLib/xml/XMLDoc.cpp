#include "stdafx.h"
#include "XMLDoc.h"
#include "XMLUtils.h"
#include "XMLNode.h"
#include "../stream/stream.h"
#include "../stream/FileStream.h"

namespace CommonLib
	{
		namespace xml
		{
			CXMLDoc::CXMLDoc()
			{
				m_char = 0;
				m_token.reserve(1000);
				m_nCurrCol = 1;
				m_nCurrRow = 1;
				m_pRoot = std::make_shared<CXMLNode>(IXMLNodePtr(), "");
			}
			CXMLDoc::~CXMLDoc()
			{

			}

		 
			void  CXMLDoc::Open(const std::string& fileName)
			{
				try
				{
					CommonLib::file::TFilePtr ptrFile = CommonLib::file::CFileCreator::OpenFileA(fileName.c_str(), CommonLib::file::ofmOpenExisting, CommonLib::file::arRead, CommonLib::file::smNoMode, CommonLib::file::oftBinary);
					CommonLib::CFileReadStream fStream(ptrFile);
					Open((CommonLib::IReadStream*)&fStream);
				}
				catch (std::exception& exc)
				{
					CommonLib::CExcBase::RegenExc("Failed to open xml file: %1", fileName, exc);
				}		 			
			}

			void  CXMLDoc::Open(CommonLib::IReadStream* pStream)
			{
				try
				{
					Clear();
					m_nCurrCol = 1;
					m_nCurrRow = 1;

					IXMLNodePtr pNode;
					enXmlLoadingState state = xlsParseTag;
 
					while (get_token(pStream))
					{
						switch (state)
						{
						case xlsParseTag:
							if (m_token == "<")
							{
								state = xlsParseName;
							}
							else
							{
								Clear();
								throw CommonLib::CExcBase("Open tag waiting, but found: %s, row: %d, col %d", m_token.c_str(), m_nCurrRow, m_nCurrCol);
					
							}
							break;
						case xlsParseName:
							if (m_token == "/")
							{
								state = xlsParseCloseName;
							}
							else if (m_token == "?xml")
							{
								while (get_token(pStream) && m_token != ">") {}
								state = xlsParseTag;
							}
							else if (m_token == "!--")
							{
								while (get_token(pStream) && m_token != "--") {}
								while (get_token(pStream) && m_token != ">") {}

							}
							else if (m_token == "![CDATA[")
							{
								std::string t = "";
								do
								{
									if (m_char == ']')
									{
										if (get_char(pStream) && m_char == ']')
										{
											pNode->SetCDATA(t);
											break;
										}
									}
									if (m_char != ' ')
										t += m_char;
								} while (get_char(pStream));
								while (get_token(pStream) && m_token != ">") {}
								state = xlsParseTag;
							}
							else
							{
								if (pNode.get())
								{
									IXMLNodePtr  pN = pNode->CreateChildNode(m_token);
									pNode = pN;
								}
								else
								{
									pNode = m_pRoot->CreateChildNode(m_token.c_str());
								}
								state = xlsParseAttributes;
							}
							break;
						case xlsParseAttributes:
							if (m_token == "/")
							{
								get_token(pStream);
								if (m_token == ">")
								{
									pNode = pNode->GetParent();
									state = xlsParseTag;
								}
								else
								{
									Clear();
									throw CommonLib::CExcBase("Missed close branch in node: %s, row: %d, col %d", pNode->GetName(), m_nCurrRow, m_nCurrCol);
								}
							}
							else if (m_token == ">")
							{
								find_open_tag(pStream);
								state = xlsParseTag;

								if (!m_vecText.empty())
								{
									if (m_vecText.back() != 0)
										m_vecText.push_back(0);
				
									pNode->SetText((const char *)&m_vecText[0]);
								}


							}
							else
							{
                                std::string sAttrName = m_token;
								get_token(pStream);
								if (m_token != "=")
								{
									Clear();
									throw CommonLib::CExcBase("Expected '=' but found: %s, row: %d, col %d", m_token.c_str(), m_nCurrRow, m_nCurrCol);
								}
								get_string(pStream);
								if (!m_vecText.empty())
								{
									if (m_vecText.back() != 0)
										m_vecText.push_back(0);

									pNode->AddPropertyString(sAttrName, (const char *)&m_vecText[0]);
								}
							}
							break;
						case  xlsParseCloseName:
							if (m_token.c_str() != pNode->GetName())
							{
								Clear();
								throw CommonLib::CExcBase("Error not close tag: %s, row: %d, col %d", m_token.c_str(), m_nCurrRow, m_nCurrCol);
							}
							else
							{
								pNode = pNode->GetParent();
								while (get_token(pStream) && m_token != ">") {}

								find_open_tag(pStream);
								if (!m_vecText.empty())
								{
									if (m_vecText.back() != 0)
										m_vecText.push_back(0);

									pNode->SetText((const char *)&m_vecText[0]);
								}

								state = xlsParseTag;
							}
							break;
						}
					}
 
				}
				catch (std::exception& exc)
				{
					CommonLib::CExcBase::RegenExc("Failed to parse xml", exc);
				}
			}

			bool CXMLDoc::get_char(CommonLib::IReadStream* pStream)
			{

				if (!pStream->ReadSafe(m_char))
					return false;

				if (m_char == '\n')
				{
					m_nCurrRow++;
					m_nCurrCol = 1;
				}
				else
					m_nCurrCol++;

				return pStream->Pos() != pStream->Size();
			}

			bool CXMLDoc::get_token(CommonLib::IReadStream* pStream)
			{


				m_token = "";
				skip_space(pStream);
				m_token = m_char;
				if (!is_escape_symbol())
				{
					while (get_char(pStream) && !is_escape_symbol())
						m_token += m_char;
				}
				else
				{
					return get_char(pStream);
				}
				return pStream->Pos() != pStream->Size();
			}


			bool CXMLDoc::skip_space(CommonLib::IReadStream* pStream)
			{
				while (is_empty_char() && get_char(pStream));
				return pStream->Pos() == pStream->Size();
			}

			bool CXMLDoc::is_empty_char()
			{
				return m_char == 0 || m_char == ' ' || m_char == '\n' || m_char == '\r' || m_char == '\t';
			}

			bool CXMLDoc::is_escape_symbol()
			{
				return m_char == '<' || m_char == '"' || m_char == '>' || m_char == ' ' || m_char == '=' || m_char == '/';
			}

			bool CXMLDoc::find_open_tag(CommonLib::IReadStream* pStream)
			{
				m_vecText.clear();
				while (is_empty_char() && get_char(pStream));

				m_token = "";
				while (m_char != '<')
				{
					m_vecText.push_back(m_char);
					if (!get_char(pStream))
						break;
				}
				m_token = m_char;

				return pStream->Pos() == pStream->Size();
			}

			bool CXMLDoc::get_string(CommonLib::IReadStream* pStream)
			{
				m_vecText.clear();
				skip_space(pStream);
				while (m_char != '\"' && get_char(pStream));
				m_token = "";
				while (get_char(pStream) && m_char != '\"')
					m_vecText.push_back(m_char);
				m_char = 0;
				return pStream->Pos() == pStream->Size();
			}

			void  CXMLDoc::Save(const std::string& fileName)
			{
				try
				{
					CommonLib::file::TFilePtr ptrFile = CommonLib::file::CFileCreator::OpenFileA(fileName.c_str(), CommonLib::file::ofmCreateAlways, CommonLib::file::arWrite, CommonLib::file::smNoMode, CommonLib::file::oftBinary);
					CommonLib::CFileWriteStream fStream(ptrFile);
					return Save((CommonLib::IWriteStream*)&fStream);
				}
				catch (std::exception& exc)
				{
					CommonLib::CExcBase::RegenExc("Failed to save xml file: %1", fileName, exc);
				}

			}

			void  CXMLDoc::Save(CommonLib::IWriteStream* pStream)
			{
				try
				{
					pStream->Write("<?xml version=\"1.0\"  encoding=\"UTF-8\"?>\n");

					for (uint32_t i = 0; i < m_pRoot->GetChildCnt(); ++i)
					{
						CXMLNode *pNode = (CXMLNode*)m_pRoot->GetChild(i).get();
						if (pNode)
							pNode->Save(pStream);
					}
				}
				catch (std::exception& exc)
				{
					CommonLib::CExcBase::RegenExc("Failed to save xml nodes", exc);
				}
	
			}

			IXMLNodePtr	CXMLDoc::GetNodes() const
			{
				return m_pRoot;
			}

			void CXMLDoc::Clear()
			{ 
				m_pRoot = std::make_shared<CXMLNode>(IXMLNodePtr(), "");
			}
		}
	}
