// Fedahumada Copyright


#include "Game/EOSSessionGameMode.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "Interfaces/OnlineSessionInterface.h"

void AEOSSessionGameMode::PostLogin(APlayerController* NewPlayer)
{
	if(IsRunningDedicatedServer()) return;
	Super::PostLogin(NewPlayer);
}
