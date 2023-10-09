// Test Multiplayer Shooter Game. All Rights Reserved.

#include "MSGameModeBase.h"
#include "MSBaseCharacter.h"
#include "MSPlayerController.h"
#include "MSGameHUD.h"

AMSGameModeBase::AMSGameModeBase()
{
    DefaultPawnClass = AMSBaseCharacter::StaticClass();
    PlayerControllerClass = AMSPlayerController::StaticClass();
    HUDClass = AMSGameHUD::StaticClass();
}

void AMSGameModeBase::StartPlay()
{
    Super::StartPlay();

    CurrentRound = 1;
    StartRound();
}

void AMSGameModeBase::StartRound()
{
    RoundTime = GameSettings.RoundTime;
    GetWorldTimerManager().SetTimer(RoundTimer, this, &AMSGameModeBase::RoundTimerUpdate, 1.0f, true);
}

void AMSGameModeBase::RoundTimerUpdate()
{
    if (--RoundTime == 0)
    {
        GetWorldTimerManager().ClearTimer(RoundTimer);

        if (CurrentRound + 1 <= GameSettings.RoundsNum)
        {
            ++CurrentRound;
            RespawnPlayers();
            StartRound();
        }
    }
}

void AMSGameModeBase::RespawnPlayers()
{
    if (!GetWorld())
        return;

    for (auto Controller = GetWorld()->GetControllerIterator(); Controller; ++Controller)
    {
        RespawnOnePlayer(Controller->Get());
    }
}

void AMSGameModeBase::RespawnOnePlayer(AController* Controller)
{
    if (Controller && Controller->GetPawn())
    {
        Controller->GetPawn()->Reset();
    }

    RestartPlayer(Controller);
}

void AMSGameModeBase::Killed(AController* KillerController, AController* VictimController)
{
    if (!GetWorld()) { return; }

    FTimerHandle RespawnTimer;
    GetWorldTimerManager().SetTimer(RespawnTimer, this, &AMSGameModeBase::RespawnPlayers, GameSettings.RespawnTime, false);
}
