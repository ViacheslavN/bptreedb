#pragma once

namespace CommonLib
{
	namespace WaitForGraph
	{
		enum SyncObjectType
		{
			Mutex,
			Event
		};

		enum  eWaitRes
		{
			wrObjectCaptured = 0,
			wrWaitingObject = 1,
			wrDeadLock = 2
		};

		typedef std::shared_ptr<class ISyncObject> TSyncObjectPtr;

		class ISyncObject
		{
		public:
			virtual int64_t GetOwner() const = 0 ;
			virtual int64_t GetHandle() const = 0;
			virtual SyncObjectType GetObjectType() const = 0;
			virtual void Lock() = 0;
			virtual void UnLock() = 0;
		};


		class IWaitForGraph
		{
			public:
				virtual eWaitRes TryToLockObject(SyncObjectType objectType, int64_t handle) = 0;
				virtual void LockObject(SyncObjectType objectType, int64_t handle) = 0;
				virtual void UnLockObject(SyncObjectType objectType, int64_t handle) = 0;
		};

		class IWaitForGraphWithSyncObject
		{
		public:
			virtual eWaitRes TryToLockObject(TSyncObjectPtr ptrSyncObj) = 0;
			virtual void LockObject(TSyncObjectPtr ptrSyncObj) = 0;
			virtual void UnLockObject(TSyncObjectPtr ptrSyncObj) = 0;
		};
	}
}