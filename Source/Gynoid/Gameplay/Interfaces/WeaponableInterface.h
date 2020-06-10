// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "UObject/Interface.h"
#include "WeaponableInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UWeaponableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Must be implemented by Character who use the Weapon system
 */
class GYNOID_API IWeaponableInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	/**
	 * Returns the Skeletal mesh of the Character
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	USkeletalMeshComponent* GetMesh();

	/**
	* Returns the Camera of the Character
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    UCameraComponent* GetCamera();
};
