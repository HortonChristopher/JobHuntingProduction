#pragma once

#include "EnemyHuman.h"
#include "Player.h"
#include "DeltaTime.h"

#include <DirectXMath.h> 

extern DeltaTime* deltaTime;

class BaseAreaConditionals
{
public: // Bool members
	static bool IsScreenShaking(bool screenShake)
	{
		return screenShake;
	}

	static bool IsInitializationFinished(bool initializeFinished)
	{
		return initializeFinished;
	}

	static bool IsGameStarting(bool gameStart)
	{
		return gameStart;
	}

	static bool ShouldFadeInBegin(float startTimer, const float startTimerLimit)
	{
		return startTimer >= startTimerLimit;
	}

	static bool IsFadeSpriteAlphaBelowZero(float fadeSpriteAlpha, const float minAlpha)
	{
		return fadeSpriteAlpha <= minAlpha;
	}

	static bool IsFadeSpriteAlphaAboveOne(float fadeSpriteAlpha, const float maxAlpha)
	{
		return fadeSpriteAlpha >= maxAlpha;
	}

	static bool IsBaseAreaOpeningCutscenePlaying(bool baseAreaOpeningCutscene, bool initializeFinished, float startTimer, const float startTimerLimit)
	{
		return baseAreaOpeningCutscene && initializeFinished && (startTimer > startTimerLimit);
	}

	static bool BaseAreaOpeningCutscenePlayingCheck(bool baseAreaOpeningCutscene)
	{
		return baseAreaOpeningCutscene;
	}

	static bool IsOpeningCutsceneFinished(float zPosition, const float mapBorder)
	{
		return zPosition >= -mapBorder;
	}

	static bool IsEnemyLandingAttack(EnemyHuman::status enumStatus)
	{
		return enumStatus == EnemyHuman::LANDINGATTACK;
	}

	static bool IsEnemyChargingNOSTAGE(EnemyHuman::status enumStatus)
	{
		return enumStatus == EnemyHuman::CHARGEATTACK;
	}

	static bool IsEnemyJetStreamAttackingNOSTAGE(EnemyHuman::status enumStatus)
	{
		return enumStatus == EnemyHuman::JETSTREAMATTACK;
	}

	static bool IsEnemySurroundAttackingNOSTAGE(EnemyHuman::status enumStatus)
	{
		return enumStatus == EnemyHuman::TWOENEMYSURROUND;
	}

	static bool ShouldGravityBeDisabled(bool landed)
	{
		return landed;
	}

	static bool IsEnemyAliveBOOLIAN(bool isAlive)
	{
		return isAlive;
	}

	static bool IsEnemyAliveHP(float hp, float minHp)
	{
		return hp > minHp;
	}

	static bool IsEnemyFleeing(EnemyHuman::status enumStatus)
	{
		return enumStatus == EnemyHuman::FLEE;
	}

	static bool HasEnemyNotYetSetFleeTarget(bool fleeSet)
	{
		return !fleeSet;
	}

	static bool IsEnemyBeingComparedWithItself(int j, int i, EnemyHuman::patrol patrolStatus)
	{
		return (j == i) || (patrolStatus == EnemyHuman::FRONT && j == (i + 1)) || (patrolStatus == EnemyHuman::BACK && j == (i - 1));
	}

	static bool IsEnemyMinDistanceNewMinDistance(float x, float y, float min, bool isBeingCalledToHelp)
	{
		return (abs(sqrt(x * x + y * y)) < min) && (isBeingCalledToHelp == false);
	}

	static bool ThereIsAnEnemyAbleToHelp(int closestEnemy, const int closestEnemyDefaultNumber)
	{
		return closestEnemy != closestEnemyDefaultNumber;
	}

	static bool IsEnemyCloseEnoughToCall(float distance, const float helpCallRange, bool helpCall)
	{
		return (distance <= helpCallRange) && !helpCall;
	}

	static bool CanJetStreamAttackBeDone(int jetStream, const int jetStreamRequiredNumber, int enemyDefeated, const int clearCondition)
	{
		return (jetStream > jetStreamRequiredNumber) && (enemyDefeated <= clearCondition - 2);
	}

	static bool ShouldEnemyDoJetStreamAttack(int agroodEnemies, const int jetStreamAttackRequiredEnemyNumber, bool debugJetAttacked, bool jetStreamCounted, EnemyHuman::status enumStatus)
	{
		return (agroodEnemies > jetStreamAttackRequiredEnemyNumber) && !debugJetAttacked && !jetStreamCounted && enumStatus == EnemyHuman::AGGRO;
	}

	static bool CanEnemyJetStreamAttack(EnemyHuman::status enumStatus, bool jetAttacked)
	{
		return (enumStatus != EnemyHuman::JETSTREAMATTACK) && !jetAttacked;
	}

	static bool CanEnemySeeOtherEnemy(int intersect, bool enemy1Alive, bool enemy2Alive, EnemyHuman::status enumStatus)
	{
		return intersect && enemy1Alive && enemy2Alive && !IsEnemyStanding(enumStatus) && !IsEnemyWandering(enumStatus) && IsEnemyAlive(enumStatus);
	}

	static bool IsEnemyWithinChargingDistanceAndHasntChargedYet(float distance, const float maxChargeDistance, const float minChargeDistance, bool chargeAttackCheck, EnemyHuman::status enumStatus, bool enemyKnockback, bool enemyAlive)
	{
		return (distance < maxChargeDistance) && (distance > minChargeDistance) && !chargeAttackCheck && enumStatus == EnemyHuman::AGGRO && !enemyKnockback && enemyAlive;
	}

	static bool SetEnemyAgrooNumberForJetStreamAttackUse(int agrooNumber)
	{
		return agrooNumber == 0;
	}

	static bool IsEnemyWithinMinimumChargeDistance(float distance, float minChargeDistance)
	{
		return distance <= minChargeDistance;
	}

	static bool IsEnemyDamaged(EnemyHuman::status enumStatus)
	{
		return enumStatus == EnemyHuman::DAMAGED;
	}

	static bool IsEnemyChargeOrJetStreamAttacking(EnemyHuman::status enumStatus, int chargeAttackStage, int jetStreamAttackStage)
	{
		return (enumStatus == EnemyHuman::CHARGEATTACK && chargeAttackStage == 1) || (enumStatus == EnemyHuman::JETSTREAMATTACK && jetStreamAttackStage == 2);
	}

	static bool CreateParticlesAtFrontPatrolPosition(float nextDegree, float initialDegree)
	{
		return nextDegree > (initialDegree + DirectX::XMConvertToRadians(90.0f));
	}

	static bool CreateParticlesAtBackPatrolPosition(float nextDegree, float initialDegree)
	{
		return nextDegree < (initialDegree - DirectX::XMConvertToRadians(90.0f));
	}

	static bool IsEnemyOrPlayerBeingKnockbacked(bool knockback)
	{
		return knockback;
	}

	static bool HasKnockbackFinished(float knockbackTime, const float knockbackMaxTime)
	{
		return knockbackTime >= knockbackMaxTime;
	}

	static bool CanEnemyDamagePlayer(bool ableToDamage)
	{
		return ableToDamage;
	}

	static bool DidEnemyHitPlayerWithNormalAttack(bool intersect, bool attackDamagePossible)
	{
		return intersect && attackDamagePossible;
	}

	static bool DidEnemyHitPlayerWithParticleAttack(bool intersect, bool particleAttackActive)
	{
		return intersect && particleAttackActive;
	}

	static bool DidEnemyHitPlayerDuringSurroundAttack(bool intersect, EnemyHuman::status enumStatus, int twoEnemySurroundStage)
	{
		return intersect && enumStatus == EnemyHuman::TWOENEMYSURROUND && twoEnemySurroundStage == 1;
	}

	static bool DidEnemyHitPlayerDuringLandingAttack(bool intersect, bool landingParticles)
	{
		return intersect && landingParticles;
	}

	static bool DidEnemyHitPlayerWhileCharging(bool intersect, EnemyHuman::status enumStatus, int chargeStage, int jetStreamStage)
	{
		if (intersect)
		{
			return (enumStatus == EnemyHuman::CHARGEATTACK && chargeStage == 1) || (enumStatus == EnemyHuman::JETSTREAMATTACK && jetStreamStage == 2);
		}

		return false;
	}

	static bool ShouldEnemyAttackToTakeAdvantageOfPlayerTakingDamage(bool damageAdvantage)
	{
		return damageAdvantage;
	}

	static bool ShouldChargeAttackBeUsedDuringDamageAdvantage(float distance, const float damageAdvantageShortLongRangeBorder)
	{
		return distance < damageAdvantageShortLongRangeBorder;
	}

	static bool ShouldTheDamageOverlayDisplay(bool damageOverlayDisplay)
	{
		return damageOverlayDisplay;
	}

	static bool AreBothPlayerAndEnemyInParryRange(bool intersect1, bool intersect2)
	{
		return intersect1 && intersect2;
	}

	static bool IsPlayerParryTimingCorrect(float attackTimer, const float minParryTime, const float maxParryTime)
	{
		return (attackTimer > minParryTime) && (attackTimer < maxParryTime);
	}

	static bool IsPlayerAttacking(Player::status enumStatus)
	{
		return enumStatus == Player::ATTACK;
	}

	static bool IsPlayerAttackTimingCorrect(float attackTimer, const float firstAttackStart, const float firstAttackEnd, const float secondAttackStart, const float secondAttackEnd, const float thirdAttackStart, const float thirdAttackEnd)
	{
		return (attackTimer >= firstAttackStart && attackTimer <= firstAttackEnd) || (attackTimer >= secondAttackStart && attackTimer <= secondAttackEnd) || (attackTimer >= thirdAttackStart && attackTimer <= thirdAttackEnd);
	}

	static bool CanPlayerDamageEnemy(bool ableToDamage)
	{
		return ableToDamage;
	}

	static bool HasScreenShakingFinished(float shakeTimer, const float maxShakeTimer)
	{
		return shakeTimer >= maxShakeTimer;
	}

	static bool WillPlayerAttackHit(int intersect)
	{
		return intersect;
	}

	static bool DoesPlayerKnockbackAttackHit(float timer, const float startTimer, const float endTimer)
	{
		return (timer >= startTimer) && (timer <= endTimer);
	}

	static bool WillPlayerAttackHitBasedOnYPosition(float position, float offset)
	{
		return position <= offset;
	}

	static bool ShouldPlayerAbilityToDamageReset(float timer, const float firstTimerEnd, const float secondTimerStart, const float secondTimerEnd, const float thirdTimerStart)
	{
		return (timer > firstTimerEnd && timer < secondTimerStart) || (timer > secondTimerEnd && timer < thirdTimerStart);
	}

	static bool ShouldEveryEnemyAggro(int enemyDefeated, int threshold)
	{
		return enemyDefeated > (threshold - 2);
	}

	static bool HasPlayerReachedGameClearThreshold(int enemyDefeated, int threshold)
	{
		return enemyDefeated > (threshold - 1);
	}

	static bool HasPlayerReachedMapBoundaries(float position, const float mapBorder)
	{
		if (mapBorder < 0.0f)
		{
			return position < mapBorder;
		}
		else
		{
			return position > mapBorder;
		}
	}

	static bool ShouldMapBoundaryDotsSpawn(float distance, const float visibleDistance)
	{
		return (distance <= visibleDistance);
	}

	static bool ShouldMissionSpriteMovementStart(bool spriteMovement)
	{
		return spriteMovement;
	}

	static bool HasMissionSpriteReachedDesiredPosition(bool set)
	{
		return set;
	}

	static bool ShouldMissionSpriteMove(float position, const float targetPosition)
	{
		return position < targetPosition;
	}

	static bool ShouldMissionSpritePositionBeSet(float position, const float targetPosition)
	{
		return position >= targetPosition;
	}

	static bool HasMissionSpriteReachedDesiredSize(bool set)
	{
		return set;
	}

	static bool ShouldMissionSpriteSizeChange(float size, const float targetSize)
	{
		return size > targetSize;
	}

	static bool ShouldMissionSpriteSizeBeSet(float size, const float targetSize)
	{
		return size <= targetSize;
	}

	static bool CameraLockOn(bool lockOnK, bool lockOnC)
	{
		return lockOnK || lockOnC;
	}

	static bool IsStaminaBarBlinking(bool blinking)
	{
		return blinking;
	}

	static bool SwitchBlinking(float alpha, const float minMaxAlpha)
	{
		if (minMaxAlpha == 0.0f)
		{
			return alpha <= minMaxAlpha;
		}
		else
		{
			return alpha >= minMaxAlpha;
		}
	}

	static bool ShouldPowerBarAlphaBeFull(float power, const float powerMaximum)
	{
		return power < powerMaximum;
	}

	static bool ShouldBarAlphaBeZero(float alpha, const float minAlpha)
	{
		return alpha <= minAlpha;
	}

	static bool ShouldSprintParticlesBeCreated(bool inputC, bool inputK, float stamina, const float staminaMin)
	{
		return (inputC && (stamina > staminaMin)) || (inputK && (stamina > staminaMin));
	}

	static bool CanSlowMotionBeActivated(bool input, bool input2, float power, const float powerMin)
	{
		return (input && (power > powerMin)) || (input2 && (power > powerMin));
	}

	static bool AreFBXModelsColliding(bool stacking)
	{
		return stacking;
	}

	static bool IsPlayerInSlowMotion(bool slowMotion)
	{
		return slowMotion;
	}

	static bool IsFadeSpriteVisible(float alpha)
	{
		return alpha > 0.0f;
	}

	static int Intersect(DirectX::XMFLOAT3 player, DirectX::XMFLOAT3 wall, float circleR, float rectW, float rectH)
	{
		DirectX::XMFLOAT2 circleDistance;

		circleDistance.x = abs(player.x - wall.x);
		circleDistance.y = abs(player.z - wall.z);

		if (circleDistance.x > (rectW / 2.0f + circleR)) { return false; }
		if (circleDistance.y > (rectH / 2.0f + circleR)) { return false; }

		if (circleDistance.x <= (rectW / 2.0f)) { return true; }
		if (circleDistance.y <= (rectH / 2.0f)) { return true; }

		float cornerDistance_sq = ((circleDistance.x - rectW / 2.0f) * (circleDistance.x - rectW / 2.0f)) + ((circleDistance.y - rectH / 2.0f) * (circleDistance.y - rectH / 2.0f));

		return (cornerDistance_sq <= (circleR * circleR));
	}

	static bool CanEnemyBeJetStreamAttacked(EnemyHuman* enemy)
	{
		return !IsEnemyStanding(enemy->enumStatus) &&
			IsEnemyAlive(enemy->enumStatus) &&
			!IsEnemyFleeing(enemy->enumStatus) &&
			!IsEnemyWandering(enemy->enumStatus) &&
			!IsEnemyCoolingDown(enemy->enumStatus) &&
			CanEnemyJetStreamAttack(enemy->enumStatus, enemy->debugJetAttacked);
	}

	static bool IsEnemyReadyToTakeAdvantage(EnemyHuman* enemy)
	{
		return BaseAreaConditionals::IsEnemyAggro(enemy->enumStatus) || BaseAreaConditionals::IsEnemyCoolingDown(enemy->enumStatus);
	}

public: // Void members
	static void UpdateStaminaSpriteAlpha(float& alpha, const float spriteInteger)
	{
		alpha -= spriteInteger * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
	}

	static void UpdateBlinkingStaminaAlpha(float& alpha, bool increasing, const float spriteInteger)
	{
		alpha += increasing ? spriteInteger * (deltaTime->deltaTimeCalculated.count() / 1000000.0f) : -spriteInteger * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
	}

	static void HandleBlinkingStamina(float& alpha, bool& effect, float minAlpha, float maxAlpha, const float spriteInteger)
	{
		UpdateBlinkingStaminaAlpha(alpha, effect, spriteInteger);

		if (BaseAreaConditionals::SwitchBlinking(alpha, effect ? maxAlpha : minAlpha))
		{
			alpha = effect ? maxAlpha : minAlpha;
			effect = !effect;
		}
	}

	static void SetClosestEnemyToHelp(EnemyHuman* enemy, std::array<EnemyHuman*, 10>& baseAreaEnemyFBX)
	{
		int closestEnemyIndex = enemy->closestEnemy;
		EnemyHuman* closestEnemy = baseAreaEnemyFBX[enemy->closestEnemy];

		bool frontPatrol = BaseAreaConditionals::IsEnemyFrontPatrolPosition(closestEnemy->patrolStatus);
		int helpIndex = frontPatrol ? closestEnemyIndex + 1 : closestEnemyIndex - 1;

		closestEnemy->isBeingCalledToHelp = true;
		baseAreaEnemyFBX[helpIndex]->isBeingCalledToHelp = true;
	}

	static void HandleEnemyHelpCall(EnemyHuman* enemy, int closestEnemyDefaultNumber, std::array<EnemyHuman*, 10>& baseAreaEnemyFBX)
	{
		if (!BaseAreaConditionals::ThereIsAnEnemyAbleToHelp(enemy->closestEnemy, closestEnemyDefaultNumber)) 
		{
			enemy->aggroSet = false;
			enemy->Reset();
			enemy->SetEnumStatus(EnemyHuman::AGGRO);
			enemy->helpCall = true;
		}

		enemy->fleeSet = true;

		if (BaseAreaConditionals::ThereIsAnEnemyAbleToHelp(enemy->closestEnemy, closestEnemyDefaultNumber)) 
		{
			SetClosestEnemyToHelp(enemy, baseAreaEnemyFBX);
		}
	}

	static void DetermineClosestEnemy(EnemyHuman* enemy, int i, int numberOfEnemiesTotal, int closestEnemyDefaultNumber, std::array<EnemyHuman*, 10>& baseAreaEnemyFBX) {
		float minDistance = FLT_MAX;

		enemy->closestEnemy = closestEnemyDefaultNumber;

		for (int j = 0; j < numberOfEnemiesTotal; j++) 
		{
			EnemyHuman* comparedEnemy = baseAreaEnemyFBX[j];

			float x = comparedEnemy->GetPosition().x - enemy->GetPosition().x;
			float y = comparedEnemy->GetPosition().z - enemy->GetPosition().z;

			if (BaseAreaConditionals::IsEnemyFrontPatrolPosition(enemy->patrolStatus) && j == (i + 1) ||
				BaseAreaConditionals::IsEnemyBackPatrolPosition(enemy->patrolStatus) && j == (i - 1))
			{
				continue;
			}

			if (!comparedEnemy->dead && j != i && !comparedEnemy->helpCall &&
				BaseAreaConditionals::IsEnemyMinDistanceNewMinDistance(x, y, minDistance, comparedEnemy->isBeingCalledToHelp))
			{
				minDistance = abs(sqrt(x * x + y * y));
				enemy->closestEnemy = j;
			}
		}

		HandleEnemyHelpCall(enemy, closestEnemyDefaultNumber, baseAreaEnemyFBX);
	}

	static void HandleEnemyFleeTarget(EnemyHuman* enemy, int i, int numberOfEnemiesTotal, int closestEnemyDefaultNumber, std::array<EnemyHuman*, 10>& baseAreaEnemyFBX)
	{
		if (BaseAreaConditionals::HasEnemyNotYetSetFleeTarget(enemy->fleeSet))
		{
			enemy->SetEnumStatus(EnemyHuman::FLEE);

			enemy->SetAggroSwitch(true);

			DetermineClosestEnemy(enemy, i, numberOfEnemiesTotal, closestEnemyDefaultNumber, baseAreaEnemyFBX);
		}
	}

	static void RunToAlly(std::array<EnemyHuman*, 10>& baseAreaEnemyFBX, int i, const float fleeSpeed, const float yRotationOffset)
	{
		float x2 = baseAreaEnemyFBX[baseAreaEnemyFBX[i]->closestEnemy]->GetPosition().x - baseAreaEnemyFBX[i]->GetPosition().x;
		float y2 = baseAreaEnemyFBX[baseAreaEnemyFBX[i]->closestEnemy]->GetPosition().z - baseAreaEnemyFBX[i]->GetPosition().z;
		float hypotenuse = sqrt((x2 * x2) + (y2 * y2));
		float radians = atan2(y2, x2);
		float degrees = DirectX::XMConvertToDegrees(radians);
		baseAreaEnemyFBX[i]->SetRotation({ baseAreaEnemyFBX[i]->GetRotation().x, -degrees + yRotationOffset, baseAreaEnemyFBX[i]->GetRotation().z });
		baseAreaEnemyFBX[i]->SetPosition({ baseAreaEnemyFBX[i]->GetPosition().x + fleeSpeed * (deltaTime->deltaTimeCalculated.count() / 1000000.0f) * (x2 / hypotenuse),
										   baseAreaEnemyFBX[i]->GetPosition().y,
										   baseAreaEnemyFBX[i]->GetPosition().z + fleeSpeed * (deltaTime->deltaTimeCalculated.count() / 1000000.0f) * (y2 / hypotenuse) });
	}

	static void CallAlly(std::array<EnemyHuman*, 10>& baseAreaEnemyFBX, int i)
	{
		baseAreaEnemyFBX[i]->aggroSet = false;
		baseAreaEnemyFBX[i]->Reset();
		baseAreaEnemyFBX[i]->SetEnumStatus(EnemyHuman::AGGRO);
		baseAreaEnemyFBX[baseAreaEnemyFBX[i]->closestEnemy]->aggroSet = false;
		baseAreaEnemyFBX[baseAreaEnemyFBX[i]->closestEnemy]->SetEnumStatus(EnemyHuman::AGGRO);
		baseAreaEnemyFBX[baseAreaEnemyFBX[i]->closestEnemy]->isBeingCalledToHelp = false;
		baseAreaEnemyFBX[i]->helpCall = true;
		baseAreaEnemyFBX[i]->fleeSet = false;
	}

	static void ResetAggro(std::array<EnemyHuman*, 10>& baseAreaEnemyFBX, int i)
	{
		if (!BaseAreaConditionals::IsEnemyDamaged(baseAreaEnemyFBX[i]->enumStatus) && !BaseAreaConditionals::IsEnemyCoolingDown(baseAreaEnemyFBX[i]->enumStatus) && !BaseAreaConditionals::IsEnemyAttacking(baseAreaEnemyFBX[i]->enumStatus) && !BaseAreaConditionals::IsEnemyAggro(baseAreaEnemyFBX[i]->enumStatus))
		{
			baseAreaEnemyFBX[i]->SetEnumStatus(EnemyHuman::AGGRO);
		}
	}

	static void PatrolAttack(std::array<EnemyHuman*, 10>& baseAreaEnemyFBX, int i)
	{
		if (BaseAreaConditionals::IsEnemyFrontPatrolPosition(baseAreaEnemyFBX[i]->patrolStatus))
		{
			if (BaseAreaConditionals::IsEnemyAggro(baseAreaEnemyFBX[i + 1]->enumStatus))
			{
				baseAreaEnemyFBX[i]->twoEnemySurroundStage = baseAreaEnemyFBX[i]->twoEnemySurroundStageReset;
				baseAreaEnemyFBX[i]->timer = baseAreaEnemyFBX[i]->timerReset;
				baseAreaEnemyFBX[i]->modelChange = true;
				baseAreaEnemyFBX[i]->SetEnumStatus(EnemyHuman::TWOENEMYSURROUND);
				baseAreaEnemyFBX[i + 1]->twoEnemySurroundStage = baseAreaEnemyFBX[i]->twoEnemySurroundStageReset;
				baseAreaEnemyFBX[i + 1]->timer = baseAreaEnemyFBX[i + 1]->timerReset;
				baseAreaEnemyFBX[i + 1]->modelChange = true;
				baseAreaEnemyFBX[i + 1]->SetEnumStatus(EnemyHuman::TWOENEMYSURROUND);
			}
			else
			{
				baseAreaEnemyFBX[i]->SetEnumStatus(EnemyHuman::ATTACK);
			}
		}
		else if (BaseAreaConditionals::IsEnemyBackPatrolPosition(baseAreaEnemyFBX[i]->patrolStatus))
		{
			if (BaseAreaConditionals::IsEnemyAggro(baseAreaEnemyFBX[i - 1]->enumStatus))
			{
				baseAreaEnemyFBX[i]->twoEnemySurroundStage = baseAreaEnemyFBX[i]->twoEnemySurroundStageReset;
				baseAreaEnemyFBX[i]->timer = baseAreaEnemyFBX[i]->timerReset;
				baseAreaEnemyFBX[i]->modelChange = true;
				baseAreaEnemyFBX[i]->SetEnumStatus(EnemyHuman::TWOENEMYSURROUND);
				baseAreaEnemyFBX[i - 1]->twoEnemySurroundStage = baseAreaEnemyFBX[i - 1]->twoEnemySurroundStageReset;
				baseAreaEnemyFBX[i - 1]->timer = baseAreaEnemyFBX[i - 1]->timerReset;
				baseAreaEnemyFBX[i - 1]->modelChange = true;
				baseAreaEnemyFBX[i - 1]->SetEnumStatus(EnemyHuman::TWOENEMYSURROUND);
			}
			else
			{
				baseAreaEnemyFBX[i]->SetEnumStatus(EnemyHuman::ATTACK);
			}
		}
		else
		{
			baseAreaEnemyFBX[i]->SetEnumStatus(EnemyHuman::ATTACK);
		}
	}

	static void ChooseEnemyAttack(std::array<EnemyHuman*, 10>& baseAreaEnemyFBX, int i)
	{
		int random = rand() % 10;

		if (random < 2) // Regular Attack
		{
			baseAreaEnemyFBX[i]->SetEnumStatus(EnemyHuman::ATTACK);
		}
		else if (random < 4) // Particle Attack
		{
			baseAreaEnemyFBX[i]->particleAttackStage = 0;
			baseAreaEnemyFBX[i]->modelChange = true;
			baseAreaEnemyFBX[i]->SetEnumStatus(EnemyHuman::PARTICLEATTACK);
		}
		else if (random < 6) // Landing Attack
		{
			baseAreaEnemyFBX[i]->landingAttackStage = 0;
			baseAreaEnemyFBX[i]->modelChange = true;
			baseAreaEnemyFBX[i]->SetEnumStatus(EnemyHuman::LANDINGATTACK);
		}
		else
		{
			PatrolAttack(baseAreaEnemyFBX, i);
		}
	}

	static void WithinMinimumDistanceAttackProcess(std::array<EnemyHuman*, 10>& baseAreaEnemyFBX, int i)
	{
		if (BaseAreaConditionals::IsEnemyAggro(baseAreaEnemyFBX[i]->enumStatus))
		{
			ChooseEnemyAttack(baseAreaEnemyFBX, i);
		}
	}

	static void InitialCharge(std::array<EnemyHuman*, 10>& baseAreaEnemyFBX, int i, int agroodEnemies)
	{
		if (SetEnemyAgrooNumberForJetStreamAttackUse(baseAreaEnemyFBX[i]->agrooNumber))
		{
			agroodEnemies++;
			baseAreaEnemyFBX[i]->agrooNumber = agroodEnemies;
		}

		if (IsEnemyAggro(baseAreaEnemyFBX[i]->enumStatus))
		{
			/*int random = rand() % 10;

			if (random < 5)
			{
				baseAreaEnemyFBX[i]->chargeAttackStage = 0;
				baseAreaEnemyFBX[i]->modelChange = true;
				baseAreaEnemyFBX[i]->SetEnumStatus(EnemyHuman::CHARGEATTACK);
				baseAreaEnemyFBX[i]->chargeAttackCheck = true;
			}
			else
			{
				baseAreaEnemyFBX[i]->chargeAttackCheck = true;
			}*/

			baseAreaEnemyFBX[i]->chargeAttackStage = 0;
			baseAreaEnemyFBX[i]->timer = 0.0f;
			baseAreaEnemyFBX[i]->modelChange = true;
			baseAreaEnemyFBX[i]->SetEnumStatus(EnemyHuman::CHARGEATTACK);
		}
	}

	static void EnemyAggro(std::array<EnemyHuman*, 10>& baseAreaEnemyFBX, Player* playerFBX, int i, const float maxChargeDistance, const float minChargeDistance, bool enemyKnockback, std::array<bool, 10>& baseAreaEnemyAliveBOOL, int agroodEnemies)
	{
		float distance = sqrt((baseAreaEnemyFBX[i]->GetPosition().x - playerFBX->GetPosition().x) * (baseAreaEnemyFBX[i]->GetPosition().x - playerFBX->GetPosition().x) + (baseAreaEnemyFBX[i]->GetPosition().z - playerFBX->GetPosition().z) * (baseAreaEnemyFBX[i]->GetPosition().z - playerFBX->GetPosition().z));

		if (IsEnemyWithinChargingDistanceAndHasntChargedYet(distance, maxChargeDistance, minChargeDistance, baseAreaEnemyFBX[i]->chargeAttackCheck, baseAreaEnemyFBX[i]->enumStatus, enemyKnockback, baseAreaEnemyAliveBOOL[i]))
		{
			InitialCharge(baseAreaEnemyFBX, i, agroodEnemies);
		}
		else if (BaseAreaConditionals::IsEnemyWithinMinimumChargeDistance(distance, minChargeDistance))
		{
			WithinMinimumDistanceAttackProcess(baseAreaEnemyFBX, i);
		}
		else
		{
			ResetAggro(baseAreaEnemyFBX, i);
		}
	}

	static void EnemySeeEnemyAggroCondition(std::array<EnemyHuman*, 10>& baseAreaEnemyFBX, int i, std::array<Object3d*, 10>& enemyVisionRangeOBJ, float playerInteresectSize, const float enemyAggroVisionRange, std::array<bool, 10>& baseAreaEnemyAliveBOOL, const int numberOfEnemiesTotal)
	{
		for (int j = 0; j < numberOfEnemiesTotal; j++)
		{
			if (i == j)
			{
				continue;
			}

			bool canSeeOtherEnemy = BaseAreaConditionals::CanEnemySeeOtherEnemy(
				Intersect(baseAreaEnemyFBX[j]->GetPosition(),
					enemyVisionRangeOBJ[i]->GetPosition(),
					playerInteresectSize,
					enemyAggroVisionRange,
					enemyAggroVisionRange),
				baseAreaEnemyAliveBOOL[j],
				baseAreaEnemyAliveBOOL[i],
				baseAreaEnemyFBX[j]->enumStatus);

			if (canSeeOtherEnemy)
			{
				baseAreaEnemyFBX[i]->SetEnumStatus(EnemyHuman::AGGRO);
				break; // As the enemy status has been set, no need to check for other enemies
			}
		}
	}

	static void DecideAndExecuteAttack(EnemyHuman* enemy, Player* player, float rangeBorder)
	{
		float distance = sqrt((enemy->GetPosition().x - player->GetPosition().x) * (enemy->GetPosition().x - player->GetPosition().x) + (enemy->GetPosition().z - player->GetPosition().z) * (enemy->GetPosition().z - player->GetPosition().z));

		if (BaseAreaConditionals::ShouldChargeAttackBeUsedDuringDamageAdvantage(distance, rangeBorder))
		{
			enemy->chargeAttackStage = enemy->chargeAttackStageReset;
			enemy->modelChange = true;
			enemy->SetEnumStatus(EnemyHuman::CHARGEATTACK);
		}
		else
		{
			enemy->landingAttackStage = enemy->chargeAttackStageReset;
			enemy->modelChange = true;
			enemy->SetEnumStatus(EnemyHuman::LANDINGATTACK);
		}
	}
};