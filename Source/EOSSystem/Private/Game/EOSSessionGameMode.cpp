// Fedahumada Copyright


#include "Game/EOSSessionGameMode.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "Interfaces/OnlineSessionInterface.h"

void AEOSSessionGameMode::PostLogin(APlayerController* NewPlayer)
{
	if(IsRunningDedicatedServer()) return;
	
	Super::PostLogin(NewPlayer);
	
	if(NewPlayer == nullptr) return;

	FUniqueNetIdRepl UniqueNetIdRepl;
	if(NewPlayer->IsLocalController())
	{
		if(const ULocalPlayer* LocalPlayer = NewPlayer->GetLocalPlayer())
		{
			UniqueNetIdRepl = LocalPlayer->GetPreferredUniqueNetId();
		}
		else
		{
			const UNetConnection* RemoteNetConnection = Cast<UNetConnection>(NewPlayer->Player);
			check(IsValid(RemoteNetConnection));
			UniqueNetIdRepl = RemoteNetConnection->PlayerId;
		}
	}
	else
	{
		const UNetConnection* RemoteNetConnection = Cast<UNetConnection>(NewPlayer->Player);
		check(IsValid(RemoteNetConnection));
		UniqueNetIdRepl = RemoteNetConnection->PlayerId;
	}

	const TSharedPtr<const FUniqueNetId> UniqueNetId = UniqueNetIdRepl.GetUniqueNetId();
	check(UniqueNetId != nullptr);

	const IOnlineSubsystem* Subsystem = Online::GetSubsystem(NewPlayer->GetWorld());
	const IOnlineSessionPtr Session = Subsystem->GetSessionInterface();
	const bool bRegistrationSuccess = Session->RegisterPlayer(FName("MainSession"), *UniqueNetId, false);
	if(bRegistrationSuccess)
	{
		UE_LOG(LogTemp, Warning, TEXT("Registration Successful"));
	}
}
