// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileBullet.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework//Character.h"
#include "Edge_TheGame/Character/EdgeCharacter.h"

void AProjectileBullet::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (OwnerCharacter)
	{
		AController* OwnerController = OwnerCharacter->Controller;
		if (OwnerController)
		{
			UGameplayStatics::ApplyDamage(OtherActor, Damage, OwnerController, this, UDamageType::StaticClass());
			AEdgeCharacter* EdgeCharacter = Cast<AEdgeCharacter>(OwnerCharacter);
			if (EdgeCharacter)
			{
				//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Blue, FString(TEXT("Hit ya mother fucker!")));
				//EdgeCharacter->ChangeCrosshairColor(FColor::Purple);
			}
			/*if (OtherActor->Implements<UInteractWithCrosshairsInterface>())
			{
				GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Blue, FString(TEXT("Hit ya mother fucker123ad123!")));
			}*/
		}
	}

	Super::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
}
