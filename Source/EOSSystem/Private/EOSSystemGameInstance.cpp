// Fedahumada Copyright


#include "EOSSystemGameInstance.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystemUtils.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Kismet/GameplayStatics.h"
#include "VoiceChat.h"

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

void UEOSSystemGameInstance::EOSCreateSession(bool bIsDedicatedServer, bool bIsLanServer, int32 NumberOfPublicConnections)
{
	const IOnlineSubsystem* Subsystem = Online::GetSubsystem(this->GetWorld());
	const IOnlineSessionPtr Session = Subsystem->GetSessionInterface();
	if (Subsystem == nullptr || Session == nullptr) return;
	
	FOnlineSessionSettings SessionCreationInfo;
	SessionCreationInfo.bIsDedicated = bIsDedicatedServer;
	SessionCreationInfo.bIsLANMatch = bIsLanServer;
	SessionCreationInfo.NumPublicConnections = NumberOfPublicConnections;
	SessionCreationInfo.bUsesPresence = false;
	SessionCreationInfo.bAllowInvites = true;
	SessionCreationInfo.bUseLobbiesIfAvailable = true;
	SessionCreationInfo.bUseLobbiesVoiceChatIfAvailable = true;
	SessionCreationInfo.bShouldAdvertise = true;
	SessionCreationInfo.Set(FName("SEARCH_KEYWORDS"), FString("RandomHi"), EOnlineDataAdvertisementType::ViaOnlineService);

	Session->OnCreateSessionCompleteDelegates.AddUObject(this,&UEOSSystemGameInstance::OnCreateSessionCompleted);
	Session->CreateSession(0,FName("MainSession"),SessionCreationInfo);
}



void UEOSSystemGameInstance::OnFindSessionCompleted(bool bWasSuccess)
{
	if (!bWasSuccess)
	{
		UE_LOG(LogTemp, Warning, TEXT("Couldn't find servers online"));
	}
	else
	{
		const IOnlineSubsystem* Subsystem = Online::GetSubsystem(this->GetWorld());
		const IOnlineSessionPtr Session = Subsystem->GetSessionInterface();
		if(Subsystem == nullptr || Session == nullptr || SessionSearch->SearchResults.Num() <= 0) return;
		
		Session->OnJoinSessionCompleteDelegates.AddUObject(this, &UEOSSystemGameInstance::OnJoinSessionCompleted);
		Session->JoinSession(0,FName("MainSession"),SessionSearch->SearchResults[0]);
		UE_LOG(LogTemp, Warning, TEXT("Joined Successful"));
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
	Session->GetResolvedConnectString(SessionName,JoinAddress);
	UE_LOG(LogTemp, Warning, TEXT("Join address is %s"), *JoinAddress);
	if(!JoinAddress.IsEmpty())
	{
		PlayerController->ClientTravel(JoinAddress,TRAVEL_Absolute);
	}
}

void UEOSSystemGameInstance::EOSFindSessionAndJoin()
{
	const IOnlineSubsystem* Subsystem = Online::GetSubsystem(this->GetWorld());
	const IOnlineSessionPtr Session = Subsystem->GetSessionInterface();
	if (Subsystem == nullptr || Session == nullptr) return;

	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	SessionSearch->bIsLanQuery = false;
	SessionSearch->MaxSearchResults = 100;
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
	Session->DestroySession(FName("MainSession"));
}



void UEOSSystemGameInstance::OnVoiceLoginCompleted(const FString& PlayerName, const FVoiceChatResult& Result)
{
	if (Result.IsSuccess())
	{
		UE_LOG(LogTemp, Warning, TEXT("Success creating voicechat"));
	}
}

void UEOSSystemGameInstance::EOSVoiceChatLogin()
{
	const IOnlineSubsystem* Subsystem = Online::GetSubsystem(this->GetWorld());
	const IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
	IVoiceChat* VoiceChat = IVoiceChat::Get();
	IVoiceChatUser* VoiceChatUser = VoiceChat->CreateUser();
	if(Subsystem == nullptr || Identity == nullptr || !IsPlayerLoggedIn() || VoiceChat == nullptr || VoiceChatUser == nullptr) return;

	const TSharedPtr<const FUniqueNetId> UserId = Identity->GetUniquePlayerId(0);
	const FPlatformUserId PlatformUserId = Identity->GetPlatformUserIdFromUniqueNetId(*UserId);
	VoiceChatUser->Login(
		PlatformUserId,
		UserId->ToString(),
		TEXT(""),
		FOnVoiceChatLoginCompleteDelegate::CreateUObject(this, &UEOSSystemGameInstance::OnVoiceLoginCompleted)
	);
}

void UEOSSystemGameInstance::EOSSetInputVolume(float NewVolume)
{
	const IOnlineSubsystem* Subsystem = Online::GetSubsystem(this->GetWorld());
	const IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
	IVoiceChat* VoiceChat = IVoiceChat::Get();
	IVoiceChatUser* VoiceChatUser = VoiceChat->CreateUser();
	if(Subsystem == nullptr || Identity == nullptr || !IsPlayerLoggedIn() || VoiceChat == nullptr || VoiceChatUser == nullptr) return;
	
	const TSharedPtr<const FUniqueNetId> UserId = Identity->GetUniquePlayerId(0);
	VoiceChatUser->SetPlayerVolume(UserId->ToString(), NewVolume);
	
}

void UEOSSystemGameInstance::EOSMuteMic(bool bIsMuted)
{
	const IOnlineSubsystem* Subsystem = Online::GetSubsystem(this->GetWorld());
	const IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
	IVoiceChat* VoiceChat = IVoiceChat::Get();
	IVoiceChatUser* VoiceChatUser = VoiceChat->CreateUser();
	if(Subsystem == nullptr || Identity == nullptr || !IsPlayerLoggedIn() || VoiceChat == nullptr || VoiceChatUser == nullptr) return;

	const TSharedPtr<const FUniqueNetId> UserId = Identity->GetUniquePlayerId(0);
	VoiceChatUser->SetPlayerMuted(UserId->ToString(), bIsMuted);
	
}

void UEOSSystemGameInstance::SetInputOutputMode(int32 Value)
{
	const IOnlineSubsystem* Subsystem = Online::GetSubsystem(this->GetWorld());
	const IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
	IVoiceChat* VoiceChat = IVoiceChat::Get();
	IVoiceChatUser* VoiceChatUser = VoiceChat->CreateUser();
	if(Subsystem == nullptr || Identity == nullptr || !IsPlayerLoggedIn() || VoiceChat == nullptr || VoiceChatUser == nullptr) return;

	UE_LOG(LogTemp, Warning, TEXT("Using microphone - %s"),*VoiceChatUser->GetAvailableInputDeviceInfos()[Value].DisplayName);
	UE_LOG(LogTemp, Warning, TEXT("Using microphone - %s"), *VoiceChatUser->GetAvailableOutputDeviceInfos()[Value].DisplayName);
	VoiceChatUser->SetInputDeviceId(VoiceChatUser->GetAvailableInputDeviceInfos()[Value].DisplayName);
	VoiceChatUser->SetOutputDeviceId(VoiceChatUser->GetAvailableOutputDeviceInfos()[Value].DisplayName);
}