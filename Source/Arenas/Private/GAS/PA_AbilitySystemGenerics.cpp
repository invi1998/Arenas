// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "PA_AbilitySystemGenerics.h"

const FRealCurve* UPA_AbilitySystemGenerics::GetEXPCurve() const
{
	return EXP_CurveTable->FindCurve(EXP_RowName, ""); 
}
