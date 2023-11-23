// Fill out your copyright notice in the Description page of Project Settings.


#include "ElimAnnouncement.h"
#include "Components/TextBlock.h"
#include "Edge_TheGame/PlayerState/EdgePlayerState.h"
#include "Edge_TheGame/EdgeTypes/Team.h"

void UElimAnnouncement::SetElimAnnouncementText(FString AttackerName, FString VictimName, APlayerState* AttackerPS, APlayerState* VictimPS)
{
	//FString ElimAnnouncementText = FString::Printf(TEXT("%s elimmed %s"), *AttackerName, *VictimName);
	if (AnnouncementText && AttackerPS && VictimPS)
	{
		AEdgePlayerState* EdgeAttackerPS = Cast<AEdgePlayerState>(AttackerPS);
		AEdgePlayerState* EdgeVictimPS = Cast<AEdgePlayerState>(VictimPS);
		if (EdgeAttackerPS && EdgeVictimPS)
		{
			if (EdgeAttackerPS->GetTeam() == ETeam::ET_RedTeam)
			{
				AttackerNameText->SetColorAndOpacity(FLinearColor(255, 0, 0, 255));
			}
			else if (EdgeAttackerPS->GetTeam() == ETeam::ET_BlueTeam)
			{
				AttackerNameText->SetColorAndOpacity(FLinearColor(0, 0, 255, 255));
			}

			if (EdgeVictimPS->GetTeam() == ETeam::ET_RedTeam)
			{
				VictimNameText->SetColorAndOpacity(FLinearColor(255, 0, 0, 255));
			}
			else if (EdgeVictimPS->GetTeam() == ETeam::ET_BlueTeam)
			{
				VictimNameText->SetColorAndOpacity(FLinearColor(0, 0, 255, 255));
			}
		}
		AttackerNameText->SetText(FText::FromString(AttackerName));
		//AnnouncementText->SetText(FText::FromString(ElimAnnouncementText));
		VictimNameText->SetText(FText::FromString(VictimName));
	}
}
