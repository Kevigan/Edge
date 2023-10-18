// Fill out your copyright notice in the Description page of Project Settings.


#include "EdgeAnimInstance.h"
#include "EdgeCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

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
	if (EdgeCharacter == nullptr) return;

	FVector Velocity = EdgeCharacter->GetVelocity();
	Velocity.Z = 0.f;
	Speed = Velocity.Size();

	bIsInAir = EdgeCharacter->GetCharacterMovement()->IsFalling();
	bIsAccelerating = EdgeCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f ? true : false;
	bWeaponEquipped = EdgeCharacter->IsWeaponEquipped();
	bIsCrouched = EdgeCharacter->bIsCrouched;
	bAiming = EdgeCharacter->IsAiming();

	// Offset Yaw for Strafing
	FRotator AimRotation = EdgeCharacter->GetBaseAimRotation();
	FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(EdgeCharacter->GetVelocity());
	FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation);
	DeltaRotation = FMath::RInterpTo(DeltaRotation, DeltaRot, DeltaTime, 6.f);
	YawOffset = DeltaRotation.Yaw;

	CharacterRotationLastFrame = CharacterRotation;
	CharacterRotation = EdgeCharacter->GetActorRotation();
	const FRotator Delta = UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation, CharacterRotationLastFrame);
	const float Target = Delta.Yaw / DeltaTime;
	const float Interp = FMath::FInterpTo(Lean, Target, DeltaTime, 6.f);
	Lean = FMath::Clamp(Interp, -90.f, 90.f);

	AO_Yaw = EdgeCharacter->GetAO_Yaw();
	AO_Pitch = EdgeCharacter->GetAO_Pitch();
}
