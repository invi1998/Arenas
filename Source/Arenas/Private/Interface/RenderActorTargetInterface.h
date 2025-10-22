// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "RenderActorTargetInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class URenderActorTargetInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ARENAS_API IRenderActorTargetInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	// 获取渲染目标的本地位置和旋转（相对于渲染Actor的位置和旋转）
	virtual FVector GetCaptureTargetLocalPosition() const = 0;
	virtual FRotator GetCaptureTargetLocalRotation() const = 0;
	
};
