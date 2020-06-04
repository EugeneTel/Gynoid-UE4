// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"
#include "Components/BoxComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Initializing components
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	SetRootComponent(BoxComponent);

	ParticleComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleComponent"));
	ParticleComponent->bAutoActivate = true;
	ParticleComponent->SetupAttachment(BoxComponent);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();

	// Registering overlap function
	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AProjectile::OnOverlap);

	//ProjectileMovementComponent
}

// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//----------------------------------------------------------------------------------------------------------------------
// Called when the projectile overlaps with another actor
//----------------------------------------------------------------------------------------------------------------------
void AProjectile::OnOverlap(
	UPrimitiveComponent* OverlappedComp, 
    AActor* OtherActor, 
    UPrimitiveComponent* OtherComp, 
    int32 OtherBodyIndex, 
    bool bFromSweep, 
    const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("AProjectile::OnBoxOverlap"));
}

//----------------------------------------------------------------------------------------------------------------------
// Called when the projectile stops
//----------------------------------------------------------------------------------------------------------------------
void AProjectile::OnProjectileStop(const FHitResult& ImpactResult)
{
	UE_LOG(LogTemp, Warning, TEXT("AProjectile::OnProjectileStop"));

	Destroy();
}



