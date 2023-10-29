// Fill out your copyright notice in the Description page of Project Settings.


#include "MiniMapComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/SceneCaptureComponent2D.h"


UMiniMapComponent::UMiniMapComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	Springarm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
}


void UMiniMapComponent::BeginPlay()
{
	Super::BeginPlay();

	
}


void UMiniMapComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

