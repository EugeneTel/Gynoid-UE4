// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Particles/ParticleSystemComponent.h"


// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	CollisionComp->InitSphereRadius(5.0f);

	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;
	
	SetRootComponent(CollisionComp);

	ParticleComp = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleComponent"));
	ParticleComp->bAutoActivate = true;
	ParticleComp->SetupAttachment(CollisionComp);

	MovementComp = CreateDefaultSubobject<class UProjectileMovementComponent>(TEXT("MovementComponent"));
	MovementComp->UpdatedComponent = CollisionComp;
	MovementComp->InitialSpeed = 10000.0f;
	MovementComp->MaxSpeed = 0.f;
	MovementComp->bRotationFollowsVelocity = true;
	MovementComp->ProjectileGravityScale = 0.f;
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();

	// Registering overlap function
	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &AProjectile::OnOverlap);

	//ProjectileMovementComponent
}

// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AProjectile::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	MovementComp->OnProjectileStop.AddDynamic(this, &AProjectile::OnImpact);
	MovementComp->InitialSpeed = ProjectileData.InitialSpeed;

	SetLifeSpan(ProjectileData.ProjectileLife);
}

void AProjectile::InitVelocity(FVector& ShootDirection) const
{
	if (MovementComp)
	{
		MovementComp->Velocity = ShootDirection * MovementComp->InitialSpeed;
	}
}

void AProjectile::SetProjectileData(FProjectileWeaponData& InProjectileData)
{
	ProjectileData = InProjectileData;
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
void AProjectile::OnImpact(const FHitResult& ImpactResult)
{
	UE_LOG(LogTemp, Warning, TEXT("AProjectile::OnImpact"));

	Destroy();
}



