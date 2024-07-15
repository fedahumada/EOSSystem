// Fedahumada Copyright


#include "Game/EOSSystemGameInstance.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystemUtils.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Kismet/GameplayStatics.h"

void UEOSSystemGameInstance::LoginWithEOS(FString Id, FString Token, FString LoginType)
{
	const IOnlineSubsystem* Subsystem = Online::GetSubsystem(this->GetWorld());
	const IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
	if(Subsystem == nullptr || Identity == nullptr) return;

	FOnlineAccountCredentials AccountDetails;
	AccountDetails.Id = Id;
	AccountDetails.Token = Token;
	AccountDetails.Type = LoginType;

	Identity->OnLoginCompleteDelegates->AddUObject(this, &UEOSSystemGameInstance::LoginWithEOS_Response);
	Identity->Login(0,AccountDetails);
}

FString UEOSSystemGameInstance::GetPlayerUsername()
{
	const IOnlineSubsystem* Subsystem = Online::GetSubsystem(this->GetWorld());
	const IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
	if(Subsystem == nullptr || Identity == nullptr) return FString();

	if (Identity->GetLoginStatus(0) == ELoginStatus::LoggedIn)
	{
		return Identity->GetPlayerNickname(0);
	}
	
	return FString();
}

bool UEOSSystemGameInstance::IsPlayerLoggedIn()
{
	const IOnlineSubsystem* Subsystem = Online::GetSubsystem(this->GetWorld());
	const IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
	if(Subsystem == nullptr || Identity == nullptr) return false;

	if (Identity->GetLoginStatus(0) == ELoginStatus::LoggedIn)
	{
		return true;
	}
	return false;
}

void UEOSSystemGameInstance::LoginWithEOS_Response(int32 LocalUserNum, bool bWasSuccess, const FUniqueNetId& UserId, const FString& Error) const
{
	if (bWasSuccess)
	{
		UE_LOG(LogTemp, Warning, TEXT("Login Successful"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Login FAILED - Reason: %s"), *Error);
	}
}



void UEOSSystemGameInstance::OnCreateSessionCompleted(FName SessionName, bool bWasSuccess) const
{
	if (bWasSuccess)
	{
		UE_LOG(LogTemp, Warning, TEXT("Session name is %s"), *SessionName.ToString());
		GetWorld()->ServerTravel(OpenLevelText);
	}
}

void UEOSSystemGameInstance::EOSCreateSession(int32 NumberOfPublicConnections, int64 ServerPort)
{
	const IOnlineSubsystem* Subsystem = Online::GetSubsystem(this->GetWorld());
	const IOnlineSessionPtr Session = Subsystem->GetSessionInterface();
	if (Subsystem == nullptr || Session == nullptr) return;
	
	FOnlineSessionSettings SessionCreationInfo;
	UE_LOG(LogTemp, Warning, TEXT("CREATING ON DEDICATED SERVER"));
	SessionCreationInfo.NumPublicConnections = NumberOfPublicConnections;
	SessionCreationInfo.bIsLANMatch = false;
	SessionCreationInfo.bAllowInvites = false;
	SessionCreationInfo.bUseLobbiesIfAvailable = false;
	SessionCreationInfo.bAllowJoinViaPresence = false;
	SessionCreationInfo.bUsesPresence = false;
	SessionCreationInfo.bAllowJoinViaPresenceFriendsOnly = false;
	SessionCreationInfo.bShouldAdvertise = true;
	SessionCreationInfo.bIsDedicated = true;

	SessionCreationInfo.Settings.Add(TEXT("PortInfo"), FOnlineSessionSetting(ServerPort, EOnlineDataAdvertisementType::ViaOnlineService));
	SessionCreationInfo.Set(FName("SEARCH_KEYWORDS"), FString("DS"), EOnlineDataAdvertisementType::ViaOnlineService);

	Session->OnCreateSessionCompleteDelegates.AddUObject(this,&UEOSSystemGameInstance::OnCreateSessionCompleted);
	Session->CreateSession(0,FName("DsSession"),SessionCreationInfo);
}



void UEOSSystemGameInstance::OnFindSessionCompleted(bool bWasSuccess)
{
	if (!bWasSuccess)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed finding dedicated servers."));
	}
	else
	{
		const IOnlineSubsystem* Subsystem = Online::GetSubsystem(this->GetWorld());
		const IOnlineSessionPtr Session = Subsystem->GetSessionInterface();
		if(Subsystem == nullptr || Session == nullptr) return;

		if (SessionSearch->SearchResults.Num() > 0)
		{
			FName InfoSessionName = bIsDedicatedSearch ? FName("DsSession") : FName("P2PSession");
			Session->OnJoinSessionCompleteDelegates.AddUObject(this, &UEOSSystemGameInstance::OnJoinSessionCompleted);
			Session->JoinSession(0, InfoSessionName, SessionSearch->SearchResults[0]);
			LocalPortInfo = SessionSearch->SearchResults[0].Session.SessionSettings.Settings.Find(FName(TEXT("PortInfo")))->Data.ToString();
			UE_LOG(LogTemp, Warning, TEXT("Joined Successful"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Couldn't find dedicated servers online."));
			PlayFabServerRequired.Broadcast();
		}
	}
}

void UEOSSystemGameInstance::OnJoinSessionCompleted(FName SessionName, EOnJoinSessionCompleteResult::Type Result) const
{
	if(Result != EOnJoinSessionCompleteResult::Success) return;
	
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(),0);
	const IOnlineSubsystem* Subsystem = Online::GetSubsystem(this->GetWorld());
	const IOnlineSessionPtr Session = Subsystem->GetSessionInterface();
	if (PlayerController == nullptr || Subsystem == nullptr || Session == nullptr) return;
	
	FString JoinAddress;
	Session->GetResolvedConnectString(SessionName, JoinAddress);

	int32 index;
	if (JoinAddress.FindChar(':', index))
	{
		JoinAddress = JoinAddress.Left(index + 1);
		JoinAddress += LocalPortInfo;
		UE_LOG(LogTemp, Warning, TEXT("Join address is %s"), *JoinAddress);
	}
	
	if(!JoinAddress.IsEmpty())
	{
		PlayerController->ClientTravel(JoinAddress,TRAVEL_Absolute);
	}
}

void UEOSSystemGameInstance::EOSFindSessionAndJoin(bool bIsDedicatedSearchIn)
{
	const IOnlineSubsystem* Subsystem = Online::GetSubsystem(this->GetWorld());
	const IOnlineSessionPtr Session = Subsystem->GetSessionInterface();
	if (Subsystem == nullptr || Session == nullptr) return;

	bIsDedicatedSearch = bIsDedicatedSearchIn;
	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	SessionSearch->bIsLanQuery = false;
	SessionSearch->MaxSearchResults = 50;
	SessionSearch->QuerySettings.SearchParams.Empty();
	Session->OnFindSessionsCompleteDelegates.AddUObject(this, &UEOSSystemGameInstance::OnFindSessionCompleted);
	Session->FindSessions(0, SessionSearch.ToSharedRef());
}



void UEOSSystemGameInstance::OnDestroySessionCompleted(FName SessionName, bool bWasSuccess) const
{
	if (bWasSuccess)
	{
		UE_LOG(LogTemp, Warning, TEXT("Destroy Successful"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Destroy Failed"));
	}
}

void UEOSSystemGameInstance::EOSDestroySession()
{
	const IOnlineSubsystem* Subsystem = Online::GetSubsystem(this->GetWorld());
	const IOnlineSessionPtr Session = Subsystem->GetSessionInterface();
	if (Subsystem == nullptr || Session == nullptr) return;

	Session->OnDestroySessionCompleteDelegates.AddUObject(this,&UEOSSystemGameInstance::OnDestroySessionCompleted);
	Session->DestroySession(bIsDedicatedSearch ? FName("DsSession") : FName("P2PSession"));
}