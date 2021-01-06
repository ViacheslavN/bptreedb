#include "pch.h"
#include "PerfLog.h"




typedef bptreedb::TZLibEncoder<double> TValueEncoder;
typedef bptreedb::TBPTreeStringMapLeafNode<double, bptreedb::TBaseNodeCompressor<bptreedb::StringValue, double, bptreedb::CZlibStringComp, TValueEncoder > > TLeafNode;


typedef bptreedb::TBPTreeStringMapInnerNode<bptreedb::TBaseNodeCompressor<bptreedb::StringValue, int64_t, bptreedb::CZlibStringComp, TValueEncoder > > TInnerNode;
typedef bptreedb::TBPSMap< double, bptreedb::CompStringValue, bptreedb::IStorage, TInnerNode, TLeafNode> TBPMap;


void TestBPTString()
{

}