
#include "MultitudHUD.h"

#include "MultitudCharacter.h"
#include "MultitudGameState.h"
#include "MultitudGameMode.h"
#include "MultitudPlayerState.h"

#include "CanvasItem.h"
#include "Engine/Canvas.h"
#include "Engine/Texture2D.h"
#include "Kismet/GameplayStatics.h"
#include "TextureResource.h"
#include "UObject/ConstructorHelpers.h"

AMultitudHUD::AMultitudHUD()
{
	static ConstructorHelpers::FObjectFinder<UTexture2D> CrosshairTexObj(TEXT("/Game/FirstPerson/Textures/FirstPersonCrosshair"));
	CrosshairTex = CrosshairTexObj.Object;
}

void AMultitudHUD::DrawHUD()
{
	Super::DrawHUD();

	const FVector2D Center(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);

	//----------------------------------

	AMultitudGameState* thisGameState = Cast<AMultitudGameState>(GetWorld()->GetGameState());

	if (thisGameState != nullptr && thisGameState->bInMenu)
	{
		int BlueScreenPos = 50;        
		int RedScreenPos = Center.Y + 50;       
		int nameSpacing = 25;       
		int NumBlueteam = 1;       
		int NumReadTeam = 1;

		FString thisString = "BLUE TEAM:"; 
		DrawText(thisString, FColor::Cyan, 50, BlueScreenPos);

		thisString = "RED TEAM:"; 
		DrawText(thisString, FColor::Red, 50, RedScreenPos);

		for (auto player : thisGameState->PlayerArray)
		{
			AMultitudPlayerState* thisPS = Cast<AMultitudPlayerState>(player);

			if (thisPS)
			{
				if (thisPS->Team == ETeam::BLUE_TEAM)
				{
					thisString = FString::Printf(TEXT("%s"), &(thisPS->PlayerName[0]));
					DrawText(thisString, FColor::Cyan, 50, BlueScreenPos + nameSpacing * NumBlueteam);
					NumBlueteam++;
				}
				else
				{
					thisString = FString::Printf(TEXT("%s"), &(thisPS->PlayerName[0]));
					DrawText(thisString, FColor::Red, 50, RedScreenPos + nameSpacing * NumReadTeam);
					NumReadTeam++;
				}
			}
		}

		if (GetWorld()->GetAuthGameMode())
		{
			thisString = "Pulsa R para empezar la partida";
			DrawText(thisString, FColor::Yellow, Center.X, Center.Y);
		}
		else
		{
			thisString = "Esperando al Servidor";
			DrawText(thisString, FColor::Yellow, Center.X, Center.Y);
		}
	}
	else
	{
		AMultitudCharacter* ThisChar = Cast<AMultitudCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(),0));

		if (ThisChar != nullptr)
		{
			if (ThisChar->GetMultitudPlayerState())
			{
				FString HUDString = FString::Printf(TEXT("Vida: %.0f, Muertes: %d"), ThisChar->GetMultitudPlayerState()->Health, ThisChar->GetMultitudPlayerState()->Deaths);
				DrawText(HUDString, FColor::Yellow, 50, 50);

				const FVector2D CrosshairDrawPosition((Center.X), (Center.Y + 20.0f));

				FCanvasTileItem TileItem(CrosshairDrawPosition, CrosshairTex->Resource, FLinearColor::White);
				TileItem.BlendMode = SE_BLEND_Translucent;
				Canvas->DrawItem(TileItem);
			}
		}
	}
}
