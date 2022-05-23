


#include "Camera.h"

ACamera::ACamera()
{
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("MaaScene"));

	m_armComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraSpringArm"));
	m_armComponent->SetupAttachment(RootComponent);
	m_armComponent->SetRelativeLocationAndRotation(FVector(0.0f, 123.0f, 41.0f), FRotator(-50.0f, 0.0f, 0.0f));
	m_armComponent->TargetArmLength = 333.0f;
	m_armComponent->bEnableCameraLag = true;
	m_armComponent->CameraLagSpeed = 6.0f;
	m_armComponent->bDrawDebugLagMarkers = false;


	m_cameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraMain"));
	m_cameraComponent->SetupAttachment(m_armComponent, USpringArmComponent::SocketName);

}

void ACamera::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACamera::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

