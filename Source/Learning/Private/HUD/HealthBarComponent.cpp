// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/HealthBarComponent.h"
#include "HUD/HealthBar.h"
#include "Components/ProgressBar.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/KismetMathLibrary.h"


void UHealthBarComponent::SetHealthPercent(float Percent)
{
	if (HealthBarWid == nullptr)
	{
		HealthBarWid = Cast<UHealthBar>(GetUserWidgetObject());
	}
	if (HealthBarWid && HealthBarWid->HealthBar)
	{
		HealthBarWid->HealthBar->SetPercent(Percent);
	}
}

void UHealthBarComponent::FacePlayerCamera()

{
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (!PlayerController)
	{
		return;
	}

	FVector CameraLocation;
	FRotator CameraRotation;
	PlayerController->GetPlayerViewPoint(CameraLocation, CameraRotation);

	// Calculate the rotation needed to face the camera
	FRotator NewRotation = UKismetMathLibrary::FindLookAtRotation(
		GetComponentLocation(), CameraLocation);

	// Set the widget's rotation
	SetWorldRotation(NewRotation);
}