// Fill out your copyright notice in the Description page of Project Settings.


#include "EdgeAnimInstance.h"
#include "EdgeCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void UEdgeAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	EdgeCharacter = Cast<AEdgeCharacter>(TryGetPawnOwner());
}

void UEdgeAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (EdgeCharacter == nullptr)
	{
		EdgeCharacter = Cast<AEdgeCharacter>(TryGetPawnOwner());
	}
	if(EdgeCharacter == nullptr) return;

	FVector Velocity = EdgeCharacter->GetVelocity();
	Velocity.Z = 0.f;
	Speed = Velocity.Size();

	bIsInAir = EdgeCharacter->GetCharacterMovement()->IsFalling();

	bIsAccelerating = EdgeCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f ? true : false; 

}
