


#include "Camera.h"

// Sets default values
ACamera::ACamera()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("MaaScene"));

	m_armComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraSpringArm"));
	m_armComponent->SetupAttachment(RootComponent);
	m_armComponent->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, 30.0f), FRotator(-50.0f, 0.0f, 0.0f));
	m_armComponent->TargetArmLength = 333.0f;
	m_armComponent->bEnableCameraLag = true;
	m_armComponent->CameraLagSpeed = 6.0f;
	m_armComponent->bDrawDebugLagMarkers = false;


	m_cameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraMain"));
	m_cameraComponent->SetupAttachment(m_armComponent, USpringArmComponent::SocketName);

}

// Called when the game starts or when spawned
void ACamera::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ACamera::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

