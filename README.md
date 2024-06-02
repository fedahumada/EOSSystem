# EOSSystem Project Overview

The EOSSystem project implements the Epic Online Services (EOS) subsystem to facilitate effective connectivity through a login registration system, which requires an Epic Games account. The primary objective of this project is to establish robust multiplayer sessions within the game. EOSSystem is not a standalone game but rather provides the foundational elements for creating lobbies and servers, including both listen servers and dedicated servers (See branches). 

This system can be applied to various projects beyond its current scope. Additionally, the project is supported by the creation of an organization on the Epic Games developer portal. Due to privacy reasons, the artifacts used in this project are not shared.


# UEOSSystemGameInstance Class

## Description
This class represents the game instance for the EOSSystem project. It handles functionalities related to Epic Online Services (EOS), including player login, session creation, session discovery, joining sessions, and session destruction.

## Public Functions

### LoginWithEOS
Initiates a login attempt with EOS using provided credentials.
- Parameters:
  - `Id` (FString): The user's ID.
  - `Token` (FString): The authentication token.
  - `LoginType` (FString): The type of login (e.g., "accountportal").

### GetPlayerUsername
Retrieves the username of the logged-in player.
- Return Type: FString.

### IsPlayerLoggedIn
Checks if a player is currently logged in.
- Return Type: bool.

### EOSCreateSession
Creates a new EOS session with specified parameters.
- Parameters:
  - `bIsDedicatedServer` (bool): Indicates if the session is dedicated.
  - `bIsLanServer` (bool): Indicates if the session is LAN-based.
  - `NumberOfPublicConnections` (int32): The maximum number of public connections allowed.

### EOSFindSessionAndJoin
Searches for available EOS sessions and joins the first one found.

### EOSDestroySession
Destroys the currently active EOS session.

## Private Functions

### LoginWithEOS_Response
Callback function triggered upon completion of the login process.
- Parameters:
  - `LocalUserNum` (int32): The local user number.
  - `bWasSuccess` (bool): Indicates if the login was successful.
  - `UserId` (FUniqueNetId): The unique ID of the user.
  - `Error` (FString): Error message in case of failure.

### OnCreateSessionCompleted
Callback function triggered upon completion of the session creation process.
- Parameters:
  - `SessionName` (FName): The name of the created session.
  - `bWasSuccess` (bool): Indicates if the session creation was successful.

### OnFindSessionCompleted
Callback function triggered upon completion of the session discovery process.
- Parameters:
  - `bWasSuccess` (bool): Indicates if session discovery was successful.

### OnJoinSessionCompleted
Callback function triggered upon completion of the session join process.
- Parameters:
  - `SessionName` (FName): The name of the session joined.
  - `Result` (EOnJoinSessionCompleteResult::Type): Result of the join session attempt.

### OnDestroySessionCompleted
Callback function triggered upon completion of the session destruction process.
- Parameters:
  - `SessionName` (FName): The name of the destroyed session.
  - `bWasSuccess` (bool): Indicates if the session destruction was successful.

# AEOSSessionGameMode Class

## Description
This class represents the game mode for managing sessions in the EOSSystem project. It extends the functionality provided by `AEOSSystemGameMode` or `AGameModeBase` and handles post-login operations, including player registration with the session.

## Public Functions

### PostLogin
Overrides the base `PostLogin` function to handle post-login operations, including player registration with the session.
- Parameters:
  - `NewPlayer` (APlayerController*): Pointer to the player controller of the newly logged-in player.

---
This documentation provides a comprehensive overview of the EOSSystem project and its associated classes.

