// Fedahumada Copyright

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "EOSSystemGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class EOSSYSTEM_API UEOSSystemGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:

	//~ LOGIN
	UFUNCTION(BlueprintCallable, Category="EOS Functions")
	void LoginWithEOS(FString Id, FString Token, FString LoginType);

	UFUNCTION(BlueprintCallable,BlueprintPure, Category="EOS Functions")
	FString GetPlayerUsername();

	UFUNCTION(BlueprintCallable,BlueprintPure, Category="EOS Functions")
	bool IsPlayerLoggedIn();

	void LoginWithEOS_Response(int32 LocalUserNum, bool bWasSuccess, const FUniqueNetId& UserId, const FString& Error) const;
	


	//~ CREATE SESSION
	void OnCreateSessionCompleted(FName SessionName, bool bWasSuccess) const;
	UFUNCTION(BlueprintCallable,Category="EOS Functions")
	void EOSCreateSession(bool bIsDedicatedServer, bool bIsLanServer, int32 NumberOfPublicConnections);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="EOS Variables")
	FString OpenLevelText;
	

	//~ FIND & JOIN SESSION
	void OnFindSessionCompleted(bool bWasSuccess);
	void OnJoinSessionCompleted(FName SessionName, EOnJoinSessionCompleteResult::Type Result) const;
	UFUNCTION(BlueprintCallable,Category="EOS Functions")
	void EOSFindSessionAndJoin();

	TSharedPtr<FOnlineSessionSearch> SessionSearch;
	

	//~ DESTROY SESSION
	void OnDestroySessionCompleted(FName SessionName, bool bWasSuccess) const;
	UFUNCTION(BlueprintCallable,Category="EOS Functions")
	void EOSDestroySession();
	
};
