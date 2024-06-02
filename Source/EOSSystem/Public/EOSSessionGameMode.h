// Fedahumada Copyright

#pragma once

#include "CoreMinimal.h"
#include "EOSSystemGameMode.h"
#include "EOSSessionGameMode.generated.h"

/**
 * 
 */
UCLASS()
class EOSSYSTEM_API AEOSSessionGameMode : public AEOSSystemGameMode
{
	GENERATED_BODY()

public:	
	virtual void PostLogin(APlayerController* NewPlayer) override;
};
