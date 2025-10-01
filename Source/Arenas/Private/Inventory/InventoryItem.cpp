// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "InventoryItem.h"

FInventoryItemHandle::FInventoryItemHandle()
	: HandleID(GetInvalidID())
{
	// 对于默认构造的库存句柄，我们将其初始化为无效ID
	// 这样可以确保未初始化的句柄不会意外地与任何有效的句柄匹配
}

FInventoryItemHandle FInventoryItemHandle::CreateHandle()
{
	return FInventoryItemHandle(GenerateNextHandleID());
}

FInventoryItemHandle FInventoryItemHandle::InvalidHandle()
{
	return FInventoryItemHandle();
}

bool FInventoryItemHandle::IsValid() const
{
	return HandleID != GetInvalidID();
}

FInventoryItemHandle::FInventoryItemHandle(uint32 InHandleID)
	: HandleID(InHandleID)
{
}

uint32 FInventoryItemHandle::GenerateNextHandleID()
{
	static uint32 nextHandleID = 1;
	return nextHandleID++;	// 这会给我它之前的值，然后递增它（即首次调用返回1，下次调用返回2，以此类推，而非首次调用返回2）
}

uint32 FInventoryItemHandle::GetInvalidID()
{
	// 返回一个无效ID，这里选择0作为无效ID
	return 0;
}

bool operator==(const FInventoryItemHandle& A, const FInventoryItemHandle& B)
{
	return A.GetHandleId() == B.GetHandleId();	
}

bool operator!=(const FInventoryItemHandle& A, const FInventoryItemHandle& B)
{
	return !(A == B);
}

uint32 GetTypeHash(const FInventoryItemHandle& Handle)
{
	return Handle.GetHandleId();
}
