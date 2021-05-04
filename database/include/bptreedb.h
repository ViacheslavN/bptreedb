#pragma once
namespace bptreedb
{


	enum eFieldDataType
	{
		fdtUnknown = 0,
		fdtNull,
		fdtInteger8,
		fdtInteger16,
		fdtInteger32,
		fdtInteger64,
		fdtUInteger8,
		fdtUInteger16,
		fdtUInteger32,
		fdtUInteger64,
		fdtOid,
		fdtFloat,
		fdtDouble,
		fdtString,
		fdtBlob,
		fdtGeometry,
		fdtRaster,
		fdtDate,
		fdtSerializedObject
	};

	enum eIndexType
	{
		itUnknown = 0,
		itUnique,
		itMultiRegular,
		itSpatial,
		itText,
		itRouting
	};

	enum eFieldDataTypeProps
	{
		fdtpSimple = 1,
		fdtpIsNotEmpty = 2,
		fdtpIsUNIQUE = 4,
		fdtpIsCounter = 8,
		fdtpIsLink = 16
	};

	enum eOpType
	{
		OpEqual,
		OpNoEqual,
		OpLess,
		OpLessOrEqual,
		OpGreater,
		OpGreaterOrEqual
	};


	enum eQryptoALG
	{
		QNONE_ALG,
		QAES128,
		QAES256
	};

	enum eStatisticType
	{
		stNotUseStatisic,
		stFullStatistic,
		stDiagramStatistic,
		stBloomFilter,
		stLogLog
	};

	enum eUpdateStatisticType
	{
		usManualUpdateStat,
		usOnlineUpdateStat
	};

	enum DBTranlogMode
	{
		eDirect,
		e2PL,
		eWAL
	};

	enum eTransactionDataType
	{
		eTT_UNDEFINED = 1, 
		eTT_MODIFY = 2,
		eTT_SELECT = 4,
		eTT_DDL = 8
	};

	enum eTransationType
	{  
		eTTUndo,
		eTTRedo,
		eTTUndoRedo
	};

	typedef std::shared_ptr<class IField> IFieldPtr;
	typedef std::shared_ptr<class IConnection> IConnectionPtr;
	typedef std::shared_ptr<class IIndex> IIndexPtr; 
	typedef std::shared_ptr<class IValue> IValuePtr;
	typedef std::shared_ptr<class IRow> IRowPtr;
	typedef std::shared_ptr<class IDirectRow> IDirectRowPtr;
	typedef std::shared_ptr<class ITransaction> ITransactionPtr; 
	typedef std::shared_ptr<class IIndex> IIndexPtr; 
	typedef std::shared_ptr<class ITable> ITablePtr; 
	typedef std::shared_ptr<class ICursor> ICursorPtr; 
	typedef std::shared_ptr<class IStatement> IStatementPtr; 
	typedef std::shared_ptr<class IInsertCursor> IInsertCursorPtr;
	typedef std::shared_ptr<class IUpdateCursor> IUpdateCursorPtr;
	typedef std::shared_ptr<class IDeleteCursor> IDeleteCursorPtr; 
	typedef std::shared_ptr<class ISchema> ISchemaPtr;
	typedef std::shared_ptr<class IDatabase> IDatabasePtr; 
	typedef std::shared_ptr<class IFileStorage> IFileStoragePtr;

	class IField  
	{
	public:
		IField() {}
		virtual ~IField() {}

		virtual eFieldDataType GetType() const = 0;
		virtual const astr& GetName() const = 0;
		virtual const astr& GetAlias() const = 0;
		virtual uint32_t GetLength()	const = 0;
		virtual bool GetIsNotNull() const = 0;
		virtual double GetScale() const = 0;
		virtual int  GetPrecision() const = 0;
		virtual bool GetIsUnique() const = 0;
		virtual IValuePtr GetDevaultValue() const = 0;
	};

	struct SFieldProp
	{
		astr m_sFieldName;
		astr m_sFieldAlias;
		eFieldDataType m_dataType;
		uint32_t m_nLenField;
		IValuePtr m_devaultValue;
		double m_dScale;
		int m_nPrecision;
		bool m_bNotNull;
		bool m_bUNIQUE;
		bool m_bCounter;

		astr m_sStorageName;

		SFieldProp() : m_nLenField(0), m_dataType(fdtUnknown), m_dScale(0), m_nPrecision(0),
			m_bNotNull(false), m_bUNIQUE(false), m_bCounter(false) 
		{}
	};

	struct SStatisticInfo
	{

		SStatisticInfo() : m_Statistic(stNotUseStatisic), m_UpdateStat(usManualUpdateStat)
		{

		}
		eStatisticType m_Statistic;
		eUpdateStatisticType m_UpdateStat;
	};

	class IIndex
	{
	public:
		IIndex() {}
		virtual ~IIndex() {}

		virtual eIndexType GetType() const = 0;
		virtual IFieldPtr GetField() const = 0;

	};

	struct SIndexProp
	{
		eIndexType m_indexType;
		astr m_sStorageName;
		SIndexProp() : m_indexType(itUnknown)
		{}
	};


	class IValue
	{
	public:
		IValue() {}
		virtual ~IValue() {};

		virtual bool IsNull() const = 0;
		virtual eFieldDataType GetDataType() const = 0;

		virtual void Get(uint8_t& val) = 0;
		virtual void Get(uint16_t& val) = 0;
		virtual void Get(uint32_t& val) = 0;
		virtual void Get(uint64_t& val) = 0;
		virtual void Get(int8_t& val) = 0;
		virtual void Get(int16_t& val) = 0;
		virtual void Get(int32_t& val) = 0;
		virtual void Get(int64_t& val) = 0;
		virtual void Get(double& val) = 0;
		virtual void Get(float& val) = 0;
		virtual void Get(astr& val) = 0;

		virtual void Set(const uint8_t& val) = 0;
		virtual void Set(const uint16_t& val) = 0;
		virtual void Set(const uint32_t& val) = 0;
		virtual void Set(const uint64_t& val) = 0;
		virtual void Set(const int8_t& val) = 0;
		virtual void Set(const int16_t& val) = 0;
		virtual void Set(const int32_t& val) = 0;
		virtual void Set(const int64_t& val) = 0;
		virtual void Set(const double& val) = 0;
		virtual void Set(const float& val) = 0;
		virtual void Set(const astr& val) = 0;

	};


	class IRow
	{
	public:
		IRow() {}
		virtual ~IRow() {};

		virtual uint32_t GetCount() const = 0;
		virtual bool IsFieldSelected(int index) const = 0;
		virtual bool IsFieldSelected(const astr& colName) const = 0;

		virtual int64_t	GetRowID() const = 0;
		virtual void	SetRow(int64_t nRowID) = 0;

		virtual IValuePtr GetValue(uint32_t col) = 0;
		virtual bool SetValue(IValuePtr ptrValue, uint32_t col) = 0;

		virtual IValuePtr GetValue(const astr& colName) = 0;
		virtual bool SetValue(IValuePtr ptrValue, const astr& colName) = 0;

	};

	class IDirectRow
	{
	public:
		IDirectRow() {}
		virtual ~IDirectRow() {};

		virtual uint32_t GetCount() const = 0;
		virtual bool IsFieldSelected(int index) const = 0;
		virtual bool IsFieldSelected(const astr& colName) const = 0;

		virtual int64_t	GetRowID() const = 0;
		virtual void	SetRow(int64_t nRowID) = 0;
		virtual bool IsNull(int32_t col) = 0;
		virtual bool IsNull(const astr& colName) = 0;

		virtual void GetUint8(uint8_t& val, int32_t col) = 0;
		virtual void GetUint16(uint16_t& val, int32_t col) = 0;
		virtual void GetUint32(uint32_t& val, int32_t col) = 0;
		virtual void GetUint64(uint64_t& val, int32_t col) = 0;
		virtual void GetInt8(int8_t& val, int32_t col) = 0;
		virtual void GetInt16(int16_t& val, int32_t col) = 0;
		virtual void GetInt32(int32_t& val, int32_t col) = 0;
		virtual void GetInt64(int64_t& val, int32_t col) = 0;
		virtual void GetFloat(float& val, int32_t col) = 0;
		virtual void GetDouble(double& val, int32_t col) = 0;
		virtual void GetString(astr& val, int32_t col) = 0;

		virtual void GetUint8(uint8_t& val, const astr&  col) = 0;
		virtual void GetUint16(uint16_t& val, const astr&  col) = 0;
		virtual void GetUint32(uint32_t& val, const astr&  col) = 0;
		virtual void GetUint64(uint64_t& val, const astr&  col) = 0;
		virtual void GetInt8(int8_t& val, const astr&  col) = 0;
		virtual void GetInt16(int16_t& val, const astr&  col) = 0;
		virtual void GetInt32(int32_t& val, const astr&  col) = 0;
		virtual void GetInt64(int64_t& val, const astr&  col) = 0;
		virtual void GetFloat(float& val, const astr& col) = 0;
		virtual void GetDouble(double& val, const astr&  col) = 0;
		virtual void GetString(astr& val, const astr&  col) = 0;


		virtual void SetUint8(const uint8_t& val, int32_t col) = 0;
		virtual void SetUint16(const uint16_t& val, int32_t col) = 0;
		virtual void SetUint32(const uint32_t& val, int32_t col) = 0;
		virtual void SetUint64(const uint64_t& val, int32_t col) = 0;
		virtual void SetInt8(const int8_t& val, int32_t col) = 0;
		virtual void SetInt16(const int16_t& val, int32_t col) = 0;
		virtual void SetInt32(const int32_t& val, int32_t col) = 0;
		virtual void SetInt64(const int64_t& val, int32_t col) = 0;
		virtual void SetFloat(const float& val, int32_t col) = 0;
		virtual void SetDouble(const double& val, int32_t col) = 0;
		virtual void SetString(const astr& val, int32_t col) = 0;

	};


	class  ICursor
	{
	public:
		ICursor() {}
		virtual ~ICursor() {}

		virtual uint32_t  GetFiledCount() const = 0;
		virtual IFieldPtr GetField(uint32_t filed) const = 0;
		virtual IFieldPtr GetField(const astr& filedName) const = 0;

	};

	class  ISelectCursor : public ICursor
	{
	public:
		ISelectCursor() {}
		virtual ~ISelectCursor() {}

		virtual bool  NextRow() = 0;
		virtual IRowPtr  GetRow() = 0;
	};

	class  IDirectSelectCursor : public ICursor
	{
	public:
		ISelectCursor() {}
		virtual ~ISelectCursor() {}

		virtual bool  NextRow() = 0;
		virtual IDirectRowPtr  GetRow() = 0;
	};

	class IInsertCursor : public ICursor
	{
	public:
		IInsertCursor() {}
		virtual ~IInsertCursor() {}

		virtual IRowPtr CreateRow() = 0;
		virtual int64_t Insert(IRowPtr ptrRow) = 0;

	};

	class IUpdateCursor : public ICursor
	{
	public:
		IUpdateCursor() {}
		virtual ~IUpdateCursor() {}

		virtual IRowPtr CreateRow() = 0;
		virtual bool Update(IRowPtr ptrRow) = 0;
	};

	class IDeleteCursor
	{
	public:
		IDeleteCursor() {}
		virtual ~IDeleteCursor() {}

		virtual bool Remove(IRowPtr ptrRow) = 0;
		virtual bool Remove(int64_t nRowID) = 0;
	};

	class ITable  
	{
	public:
		ITable() {}
		virtual ~ITable() {}

		virtual bool GetOIDFieldName(astr& sOIDName) = 0;
		virtual bool SetOIDFieldName(const astr& sOIDName) = 0;

		virtual const astr& GetName() const = 0;

		virtual IFieldPtr GetField(const astr& sName) const = 0;
		virtual uint32_t GetFieldCnt() const = 0;
		virtual IFieldPtr GetField(uint32_t nIdx) const = 0;


		virtual IFieldPtr CreateField(const  SFieldProp& sFP, ITransactionPtr ptrTran) = 0;
		 
		virtual void DeleteField(IField* pField) = 0;
		virtual bool CreateIndex(const astr& filedName, SIndexProp& ip, ITransactionPtr ptrTran) = 0;
		virtual bool CreateCompositeIndex(const std::vector<astr>& vecFields, SIndexProp& ip, ITransactionPtr ptrTran) = 0;
		virtual bool CreateStatistic(const astr& filedName, const SStatisticInfo& ip, ITransactionPtr ptrTran) = 0;
		virtual bool UpdateStatistic(const astr& filedName, ITransactionPtr ptrTran) = 0;
	};

	struct STableParams
	{
		astr m_sName;
		astr m_sAlias;
		astr m_sStorageName;
	};

	class ISchema  
	{
	public:
		ISchema() {}
		virtual ~ISchema() {}
		virtual uint32_t  GetTableCnt() const = 0;
		virtual ITablePtr GetTable(uint32_t nIndex) const = 0;
		virtual ITablePtr GetTableByID(int64_t nID) const = 0;
		virtual ITablePtr GetTableByName(const astr& pszTableName) const = 0;

		virtual bool AddTable(const STableParams& params, ITransactionPtr ptrTran) = 0;
		virtual bool DropTable(const CommonLib::CString& sTableName, ITransactionPtr ptrTran) = 0;
		virtual bool DropTable(int64_t nID, ITransactionPtr ptrTran) = 0;
		virtual bool DropTable(ITablePtr ptrTable, ITransactionPtr ptrTran) = 0;		
	};

	class IStatement  
	{
	public:

		IStatement() {}
		virtual ~IStatement() {}

		virtual void SetValue(const astr& valueName, IValuePtr ptrVal) = 0;
		virtual IValuePtr GetValue(const astr& valueName) = 0;
		virtual void SetValue(uint32_t col, IValuePtr ptrVal) = 0;
		virtual IValuePtr GetValue(uint32_t col) = 0;

		virtual IFieldPtr GetField(uint32_t nCount) = 0;
		virtual uint32_t GetFieldCount() const = 0;
		virtual int32_t GetColNum(const astr& valueName) const = 0;
		

		virtual bool SetValueByte(uint32_t col, byte_t val) = 0;
		virtual bool SetValueInt8(uint32_t col, int8_t val) = 0;
		virtual bool SetValueInt16(uint32_t col, int16_t val) = 0;
		virtual bool SetValueUInt16(uint32_t col, uint16_t val) = 0;
		virtual bool SetValueInt32(uint32_t col, int32_t val) = 0;
		virtual bool SetValueUInt32(uint32_t col, uint32_t val) = 0;
		virtual bool SetValueInt64(uint32_t col, int64_t val) = 0;
		virtual bool SetValueUInt64(uint32_t Col, uint64_t val) = 0;
		virtual bool SetValueFloat(uint32_t col, float val) = 0;
		virtual bool SetValueDouble(uint32_t col, double val) = 0;
		virtual bool SetValueBlob(uint32_t col, const byte_t* pBuf, uint32_t nSize) = 0;
		virtual bool SetValueText(uint32_t col, const astr& str) = 0;
		virtual bool SetValueText(uint32_t Col, const char* pBuf, uint32_t nLen) = 0;
 	};


	struct ITransaction  
	{
	public:
		ITransaction() {}
		virtual ~ITransaction() {}


		virtual eTransactionDataType GetType() const = 0;
		virtual void Begin() = 0;
		virtual void Commit() = 0;
		virtual void Rollback() = 0;
		virtual bool IsError() const = 0; 
		virtual CommonLib::CGuid GetId() const = 0;

		virtual IStatementPtr CreateStatement(const astr& sSQLQuery) = 0;
		virtual ICursorPtr ExecuteQuery(IStatement* pStatement) = 0;
		 
		virtual ICursorPtr ExecuteSelectQuery(const astr& sTable, const std::vector<astr>& fileds) = 0; // For test
		virtual ICursorPtr ExecuteSelectQuery(const astr& sTable, const std::vector<astr>& fileds, const astr& sField, IValuePtr ptrVal, eOpType opType) = 0; // For test

		virtual IInsertCursorPtr CreateInsertCursor(const astr& sTable, const std::vector<astr>& fileds) = 0;
		virtual IUpdateCursorPtr CreateUpdateCursor() = 0;
		virtual IDeleteCursorPtr CreateDeleteCursor(const astr& sTable) = 0;
	};


	class IConnection
	{
		public:
			IConnection() {};
			virtual ~IConnection(){}

			virtual ISchemaPtr getSchema() const = 0;
			virtual ITransactionPtr startTransaction(eTransactionDataType trType, eTransationType trDbType = eTTUndoRedo) = 0;
			virtual bool closeTransaction(ITransactionPtr ptrTran) = 0;
	};

	class IFileStorage
	{
	public:
		IFileStorage() {};
		virtual ~IFileStorage() {};

		virtual const astr&  GetName() const;
		virtual const astr&  GetPath() const;
		virtual CommonLib::CGuid GetGuid() const = 0;
		virtual uint64_t GetSize() const = 0;
		virtual uint64_t GetCapacity() const = 0;
	};

	struct SFileStorageParams
	{
		astr m_name;
		astr m_path;
		uint64_t m_initialsize{ 0 };
		uint64_t m_growingsize{ 0 };
	};
	   	 
	class  IDatabase  
	{
	public:
		IDatabase() {}
		virtual ~IDatabase() {}
		
		virtual IConnectionPtr Connect() = 0;
		virtual void CloseConnection(IConnectionPtr ptrConnection) = 0;
		virtual void Close() = 0;
		virtual CommonLib::CGuid GetDatabaseGuid() const = 0;
		virtual uint32_t GetFilesStoragesCount() const = 0;
		virtual IFileStoragePtr GetFileStorage(uint32_t idx) = 0;
		virtual IFileStoragePtr GetFileStorageByName(const astr& name) = 0;
		virtual void AddFileStorage(const SFileStorageParams& fileStorageParam) = 0;

		static IDatabasePtr CreateDatabase(const astr& dbFilePath, const astr& pwd, DBTranlogMode TranLogMode = e2PL);
		static IDatabasePtr OpenDatabase(const astr& dbFilePath, const astr& pwd);
	};

}