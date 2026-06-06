#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Sound/SoundBase.h"
#include "SOHPlayerCharacter.generated.h"

class ASOHFlashlight;
class UAnimMontage;
class USoundBase;
class UUserWidget;
class USpringArmComponent;
class UCameraComponent;
//lass USkeletalMeshComponent;

UENUM(BlueprintType)
enum class EViewMode : uint8
{
	BackView UMETA(DisplayName = "BackView"), // 0번 인덱스
	ShoulderView UMETA(DisplayName = "ShoulderView") // 1번 인덱스
};

USTRUCT(BlueprintType)
struct FSpringArmConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float TargetArmLength = 60.f;
	
};

UCLASS()
class SOH_API ASOHPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ASOHPlayerCharacter();

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* FollowCamera;

	//GameInstance에서 호출하기 위해 만들었습니다.
	UFUNCTION(BlueprintCallable)
	float GetHealth() const { return Health; }

	UFUNCTION(BlueprintCallable)
	float GetStamina() const { return Stamina; }

	void ApplyLoadedData();
	
	// Battery 호출용

	// Flashlight 상태 동기화 수행
	void SetFlashlight(ASOHFlashlight* InLight);

	ASOHFlashlight* GetFlashlight() const { return Flashlight; }

	// CrossHair 호출용 bool 변수

	UPROPERTY(BlueprintReadOnly)
	bool bUIHit;
	
	// UI 열고 닫을 때 재생할 사운드
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI Audio")
	USoundBase* UIOpenSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI Audio")
	USoundBase* UICloseSound;

	UFUNCTION(BlueprintCallable, Category = "Camera")
	void SetViewMode(EViewMode NewViewMode);

protected:
	virtual void BeginPlay() override;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float RotationRate = 500.f;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float WalkSpeed = 200.f;

	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float RunSpeed = 400.f;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float StandingRotationSpeed = 20.0f;

	FVector2D CurrentMoveInput;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	TArray<FSpringArmConfig> SpringArmConfigs;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	EViewMode CurrentViewMode = EViewMode::ShoulderView;

	// 여기서부터

	// 체력

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stat|Health")
	float MaxHealth = 200.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat|Health")
	float Health;

	// 스태미너

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stat|Stamina")
	float MaxStamina = 140.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat|Stamina")
	float Stamina;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stat|Stamina")
	float StaminaDrainPerSec = 20.f;   // 달릴 때 초당 소모

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stat|Stamina")
	float StaminaRegenPerSec = 10.f;   // 쉬면 초당 회복

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
	float StaminaRegenDelay = 1.f; // 회복 시작 전 대기시간

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stat|Stamina")
	float MinStaminaToRun = 10.f;       // 이 이하이면 달리기 불가

	UPROPERTY(BlueprintReadOnly, Category = "Stamina")
	bool bIsExhausted = false; // 탈진 상태

	UPROPERTY(BlueprintReadOnly, Category = "Stamina")
	bool bCanSprint = true; // 달리기 가능 여부

	// 스태미너 오디오
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina|Audio")
	USoundBase* HeavyBreathingSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina|Audio")
	USoundBase* ExhaustedSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina|Audio")
	float LowStaminaThreshold = 30.f;

	// 발소리
	UPROPERTY(EditAnywhere, Category = "Audio|Footstep")
	USoundBase* FootstepSound;

	// 발소리 재생 최소 속도
	UPROPERTY(EditAnywhere, Category = "Audio|Footstep")
	float MinFootstepSpeed = 10.0f;

	// 회복 사운드
	UPROPERTY(EditAnywhere, Category = "Stat|Health")
	USoundBase* HealSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* HealMontage;

	bool bIsPlayingHealMontage = false;

	// 피격 반응
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	UAnimMontage* HitReactionMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bIsPlayingHitReaction = false;

	UFUNCTION()
	void OnHitReactionEnded(UAnimMontage* Montage, bool bInterrupted);

	// 체력 감소/죽음 처리

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Death")
	UAnimMontage* DeathMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Death")
	USoundBase* DeathSound;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Death")
	bool bIsDead = false;

	void Die();

	UFUNCTION()
	void CallGameModeOnPlayerDied();

	virtual float TakeDamage(
		float DamageAmount,
		struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator,
		AActor* DamageCauser
	) override;

	// 여기까지 구현.

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	bool bIsRunning = false;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	//USkeletalMeshComponent* FaceMesh;



	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    float MinPitchAngle = -60.f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    float MaxPitchAngle = 60.f;

	// Flashlight 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flashlight")
	bool bFlashlightOn = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* FlashlightMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	ASOHFlashlight* Flashlight;

	void TraceForInteractable();
	
	UPROPERTY(BlueprintReadWrite, Category = "UI")
	bool bIsUIOpen = false;

	UPROPERTY(BlueprintReadWrite, Category = "UI")
	UUserWidget* CurrentOpenUI = nullptr;

	UPROPERTY(BlueprintReadWrite, Category = "UI")
	FName CurrentUIType = NAME_None;

	UFUNCTION(BlueprintCallable, Category = "UI")
	void OpenUI(UUserWidget* NewUI, FName UIType);

	UFUNCTION(BlueprintCallable, Category = "UI")
	void CloseUI();

	UFUNCTION(BlueprintCallable)
	void ToggleUI(FName UIType, TSubclassOf<UUserWidget> WidgetClass);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> PauseMenuClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> MapMenuClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> InventoryMenuClass;

	// 상호작용 거리
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trace")
	float TraceDistance = 300.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* InteractMontage;

	bool bIsPlayingInteractMontage = false;

	UFUNCTION()
	void OnInteractMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UPROPERTY()
	TArray<UUserWidget*> RemovedWidgets;

public:
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

	UFUNCTION(BlueprintCallable)
	void StartRun();
	UFUNCTION(BlueprintCallable)
	void StopRun();

	void ToggleCrouch();
	void Interact();
	void ToggleFlashlight();
	void PlayUpperBodyMontage(UAnimMontage* Montage);
	void UseBattery();
	void OnHealMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	void OnTogglePause(const FInputActionValue& Value);
	void OnToggleMap(const FInputActionValue& Value);
	void OnToggleInventory(const FInputActionValue& Value);
	virtual void Tick(float DeltaTime) override;
	// 체력 관련 UI
	
	// 체력 회복 아이템 사용
	UFUNCTION(BlueprintCallable)
	void UseHealthItem();

	// 체력 회복 함수 (직접 호출용)
	UFUNCTION(BlueprintCallable)
	void Heal(float HealAmount);

	// 발소리 재생 (AnimNotify에서 호출)
	UFUNCTION(BlueprintCallable)
	void PlayFootstepSound();

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateOverlay(float HealthValue, float MaxHealthValue);

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> PlayerHUDClass;

	UPROPERTY(BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	UUserWidget* PlayerHUD = nullptr;

	UPROPERTY(BlueprintReadWrite, Category = "Interaction")
	bool bIsInInteractionCamera = false;

private:
	UPROPERTY()
	AActor* LastHighlightedItem = nullptr;

	float TimeSinceLastStaminaUse = 0.f;
	void UpdateStamina();
	void OnExhausted();
	void OnRecovered();
	void PlayHitReaction();

	UPROPERTY()
	class UAudioComponent* BreathingAudioComponent;

	void StartHeavyBreathing();
	void StopHeavyBreathing();

	FTimerHandle TraceTimerHandle;
	FTimerHandle CrouchMovementCheckTimer;
	FTimerHandle StaminaUpdateTimer;

	void CheckCrouchMovement();
};

