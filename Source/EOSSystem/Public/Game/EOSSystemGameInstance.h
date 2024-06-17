// Fedahumada Copyright

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "EOSSystemGameInstance.generated.h"

//~ DELEGATES
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FServerRequiredSignature);

/**
 * 
 */
UCLASS()
class EOSSYSTEM_API UEOSSystemGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:

	//~ LOGIN
	UFUNCTION(BlueprintCallable, Category="EOS|Functions")
	void LoginWithEOS(FString Id, FString Token, FString LoginType);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="EOS|Functions")
	FString GetPlayerUsername();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="EOS|Functions")
	bool IsPlayerLoggedIn();

	void LoginWithEOS_Response(int32 LocalUserNum, bool bWasSuccess, const FUniqueNetId& UserId, const FString& Error) const;
	


	//~ CREATE SESSION
	void OnCreateSessionCompleted(FName SessionName, bool bWasSuccess) const;
	UFUNCTION(BlueprintCallable, Category="EOS|Functions")
	void EOSCreateSession(int32 NumberOfPublicConnections, int64 ServerPort);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="EOS|Variables")
	FString OpenLevelText;
	

	//~ FIND & JOIN SESSION
	void OnFindSessionCompleted(bool bWasSuccess);
	void OnJoinSessionCompleted(FName SessionName, EOnJoinSessionCompleteResult::Type Result) const;
	UFUNCTION(BlueprintCallable,Category="EOS|Functions")
	void EOSFindSessionAndJoin(bool bIsDedicatedSearchIn);
	
	bool bIsDedicatedSearch = false;
	TSharedPtr<FOnlineSessionSearch> SessionSearch;
	

	//~ DESTROY SESSION
	void OnDestroySessionCompleted(FName SessionName, bool bWasSuccess) const;
	UFUNCTION(BlueprintCallable,Category="EOS|Functions")
	void EOSDestroySession();

	
	//~ PLAYFAB LOGIC
	UPROPERTY(BlueprintAssignable, Category="EOS|Delegates")
	FServerRequiredSignature PlayFabServerRequired;

	FString LocalPortInfo;
	int64 PortInfo;
};
