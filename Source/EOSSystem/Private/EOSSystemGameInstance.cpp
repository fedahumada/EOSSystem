// Fedahumada Copyright


#include "EOSSystemGameInstance.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystemUtils.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Kismet/GameplayStatics.h"


void UEOSSystemGameInstance::LoginWithEOS(FString Id, FString Token, FString LoginType)
{
	const IOnlineSubsystem* Subsystem = Online::GetSubsystem(this->GetWorld());
	if(Subsystem == nullptr) return;
	
	const IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
	if(Identity == nullptr) return;

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
	if(Subsystem == nullptr) return FString();
	
	const IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
	if(Identity == nullptr) return FString();

	if (Identity->GetLoginStatus(0) == ELoginStatus::LoggedIn)
	{
		return Identity->GetPlayerNickname(0);
	}
	
	return FString();
}

bool UEOSSystemGameInstance::IsPlayerLoggedIn()
{
	const IOnlineSubsystem* Subsystem = Online::GetSubsystem(this->GetWorld());
	if(Subsystem == nullptr) return false;
	
	const IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
	if(Identity == nullptr) return false;

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

void UEOSSystemGameInstance::EOSCreateSession(bool bIsDedicatedServer, bool bIsLanServer, int32 NumberOfPublicConnections)
{
	if(const IOnlineSubsystem* Subsystem = Online::GetSubsystem(this->GetWorld()))
	{
		if (const IOnlineSessionPtr Session = Subsystem->GetSessionInterface())
		{
			FOnlineSessionSettings SessionCreationInfo;
			SessionCreationInfo.bIsDedicated = bIsDedicatedServer;
			SessionCreationInfo.bIsLANMatch = bIsLanServer;
			SessionCreationInfo.NumPublicConnections = NumberOfPublicConnections;
			SessionCreationInfo.bAllowInvites = false;
			SessionCreationInfo.bUseLobbiesIfAvailable = false;
			SessionCreationInfo.bUsesPresence = false;
			SessionCreationInfo.bShouldAdvertise = true;
			SessionCreationInfo.bAllowJoinViaPresence = false;
			SessionCreationInfo.bAllowJoinViaPresenceFriendsOnly = false;
			SessionCreationInfo.Set(FName("SEARCH_KEYWORDS"), FString("RandomHi"), EOnlineDataAdvertisementType::ViaOnlineService);

			Session->OnCreateSessionCompleteDelegates.AddUObject(this,&UEOSSystemGameInstance::OnCreateSessionCompleted);
			Session->CreateSession(0,FName("MainSession"),SessionCreationInfo);
		}
	}
}



void UEOSSystemGameInstance::OnFindSessionCompleted(bool bWasSuccess)
{
	if (bWasSuccess)
	{
		if(const IOnlineSubsystem* Subsystem = Online::GetSubsystem(this->GetWorld()))
		{
			if(const IOnlineSessionPtr Session = Subsystem->GetSessionInterface())
			{
				if(SessionSearch->SearchResults.Num() > 0)
				{
					Session->OnJoinSessionCompleteDelegates.AddUObject(this, &UEOSSystemGameInstance::OnJoinSessionCompleted);
					Session->JoinSession(0,FName("MainSession"),SessionSearch->SearchResults[0]);
					UE_LOG(LogTemp, Warning, TEXT("Joined Successful"));
				}
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Couldn't find servers online"));
	}
}

void UEOSSystemGameInstance::OnJoinSessionCompleted(FName SessionName, EOnJoinSessionCompleteResult::Type Result) const

{
	if(Result == EOnJoinSessionCompleteResult::Success)
	{
		if(APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(),0))
		{
			if(const IOnlineSubsystem* Subsystem = Online::GetSubsystem(this->GetWorld()))
			{
				if (const IOnlineSessionPtr Session = Subsystem->GetSessionInterface())
				{
					FString JoinAddress;
					Session->GetResolvedConnectString(SessionName,JoinAddress);
					UE_LOG(LogTemp, Warning, TEXT("Join address is %s"), *JoinAddress);
					if(!JoinAddress.IsEmpty())
					{
						PlayerController->ClientTravel(JoinAddress,TRAVEL_Absolute);
					}
				}
			}
		}
	}
}

void UEOSSystemGameInstance::EOSFindSessionAndJoin()
{
	if(const IOnlineSubsystem* Subsystem = Online::GetSubsystem(this->GetWorld()))
	{
		if (const IOnlineSessionPtr Session = Subsystem->GetSessionInterface())
		{
			SessionSearch = MakeShareable(new FOnlineSessionSearch());
			SessionSearch->bIsLanQuery = false;
			SessionSearch->MaxSearchResults = 100;
			SessionSearch->QuerySettings.SearchParams.Empty();
			Session->OnFindSessionsCompleteDelegates.AddUObject(this, &UEOSSystemGameInstance::OnFindSessionCompleted);
			Session->FindSessions(0, SessionSearch.ToSharedRef());
		}
	}
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
	if(const IOnlineSubsystem* Subsystem = Online::GetSubsystem(this->GetWorld()))
	{
		if (const IOnlineSessionPtr Session = Subsystem->GetSessionInterface())
		{
			Session->OnDestroySessionCompleteDelegates.AddUObject(this,&UEOSSystemGameInstance::OnDestroySessionCompleted);
			Session->DestroySession(FName("MainSession"));
		}
	}
}
