// Fill out your copyright notice in the Description page of Project Settings.


#include "OverHeadWidget.h"
#include "Components/TextBlock.h"

void UOverHeadWidget::SetDisplayText(FString TextToDisplay)
{
	if (DisplayText)
	{
		DisplayText->SetText(FText::FromString(TextToDisplay));
	}
}

void UOverHeadWidget::ShowPlayerNetRole(APawn* InPawn)
{
	//ENetRole RemoteRole = InPawn->GetRemoteRole();
	ENetRole RemoteRole = InPawn->GetLocalRole();
	FString Role;
	switch (RemoteRole)
	{
	case ROLE_None:
		Role = FString("None");
		break;
	case ROLE_SimulatedProxy:
		Role = FString("ROLE_SimulatedProxy");
		break;
	case ROLE_AutonomousProxy:
		Role = FString("ROLE_AutonomousProxy");
		break;
	case ROLE_Authority:
		Role = FString("Authority");
		break;
	}
	FString RemoteRoleString = FString::Printf(TEXT("Remote Role: %s"), *Role);
	SetDisplayText(RemoteRoleString);
}

void UOverHeadWidget::NativeDestruct()
{
	Super::NativeDestruct();
	RemoveFromParent();
}
