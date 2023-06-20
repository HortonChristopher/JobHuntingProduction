#pragma once

#include "EnemyHuman.h"
#include "Player.h"

#include <DirectXMath.h>

class BaseAreaConditionals
{
public:
	static bool IsScreenShaking(bool screenShake)
	{
		return screenShake;
	}

	static bool IsCameraLockedOn(bool lockOn)
	{
		return lockOn;
	}

	static bool IsPlayerDodging(bool dodge)
	{
		return dodge;
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

	static bool HasPlayerReachedTriggerToMoveMissionSprite(float zPosition, const float movementStartZPosition)
	{
		return zPosition >= -movementStartZPosition;
	}

	static bool IsOpeningCutsceneFinished(float zPosition, const float mapBorder)
	{
		return zPosition >= -mapBorder;
	}

	static bool IsEnemyWandering(EnemyHuman::status enumStatus)
	{
		return enumStatus == EnemyHuman::WANDER;
	}

	static bool IsEnemyStanding(EnemyHuman::status enumStatus)
	{
		return enumStatus == EnemyHuman::STAND;
	}

	static bool IsEnemyAggro(EnemyHuman::status enumStatus)
	{
		return enumStatus == EnemyHuman::AGGRO;
	}

	static bool IsEnemyAlive(EnemyHuman::status enumStatus)
	{
		return enumStatus != EnemyHuman::DEAD;
	}

	static bool IsEnemyAliveBOOLIAN(bool isAlive)
	{
		return isAlive;
	}

	static bool IsEnemyAttacking(EnemyHuman::status enumStatus)
	{
		return enumStatus == EnemyHuman::CHARGEATTACK || enumStatus == EnemyHuman::JETSTREAMATTACK || enumStatus == EnemyHuman::LANDINGATTACK || enumStatus == EnemyHuman::PARTICLEATTACK || enumStatus == EnemyHuman::ATTACK;
	}

	static bool IsEnemyCoolingDown(EnemyHuman::status enumStatus)
	{
		return enumStatus == EnemyHuman::COOLDOWN;
	}

	static bool IsEnemyFleeing(EnemyHuman::status enumStatus)
	{
		return enumStatus == EnemyHuman::FLEE;
	}

	static bool ShouldEnemyFlee(float hp, const float fleeHP, bool enemyKnockback, EnemyHuman::status enumStatus, bool helpCall, bool isPartnerDead)
	{
		if (!enemyKnockback && enumStatus != EnemyHuman::DAMAGED && !helpCall && enumStatus != EnemyHuman::DEAD)
		{
			return (hp <= fleeHP) || isPartnerDead;
		}

		return false;
	}

	static bool HasEnemyNotYetSetFleeTarget(bool fleeSet)
	{
		return !fleeSet;
	}

	static bool IsEnemyFrontPatrolPosition(EnemyHuman::patrol patrolStatus)
	{
		return patrolStatus == EnemyHuman::FRONT;
	}

	static bool IsEnemyBackPatrolPosition(EnemyHuman::patrol patrolStatus)
	{
		return patrolStatus == EnemyHuman::BACK;
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
		return enumStatus != EnemyHuman::JETSTREAMATTACK && !jetAttacked;
	}

	static bool CanEnemySeePlayer(int intersect, bool enemyKnockback, bool enemyAlive, EnemyHuman::status enumStatus)
	{
		return (intersect && !enemyKnockback && enemyAlive) || (enumStatus == EnemyHuman::AGGRO && !enemyKnockback && enemyAlive);
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

	static bool IsEnemyCurrentlyParticleAttacking(bool particleAttackActive)
	{
		return particleAttackActive;
	}

	static bool HasEnemyHitTheGroundDuringLandingAttack(bool landingParticles)
	{
		return landingParticles;
	}

	static bool IsEnemyChargeOrJetStreamAttacking(EnemyHuman::status enumStatus, int chargeAttackStage, int jetStreamAttackStage)
	{
		return (enumStatus == EnemyHuman::CHARGEATTACK && chargeAttackStage == 1) || (enumStatus == EnemyHuman::JETSTREAMATTACK && jetStreamAttackStage == 2);
	}

	static bool IsEnemyChargeAttacking(EnemyHuman::status enumStatus, int chargeAttackStage)
	{
		return enumStatus == EnemyHuman::CHARGEATTACK && chargeAttackStage == 1;
	}

	static bool IsEnemyJetStreamAttacking(EnemyHuman::status enumStatus, int jetStreamAttackStage)
	{
		return enumStatus == EnemyHuman::JETSTREAMATTACK && jetStreamAttackStage == 2;
	}
	
	static bool IsEnemySurroundAttackingThePlayer(EnemyHuman::status enumStatus, int twoEnemySurroundStage)
	{
		return enumStatus == EnemyHuman::TWOENEMYSURROUND && twoEnemySurroundStage == 1;
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

	static bool HasPlayerHPReachedZero(int hp, const int defeatHP)
	{
		return hp < defeatHP;
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

	static bool IsPlayerDead(bool isPlayerDead)
	{
		return isPlayerDead;
	}

	static bool IsPlayerParrying(Player::status enumStatus)
	{
		return enumStatus == Player::PARRY;
	}

	static bool IsPlayerAttacking(Player::status enumStatus)
	{
		return enumStatus == Player::ATTACK;
	}

	static bool IsPlayerAttackTimingCorrect(float attackTimer, const float firstAttackStart, const float firstAttackEnd, const float secondAttackStart, const float secondAttackEnd, const float thirdAttackStart, const float thirdAttackEnd)
	{
		return (attackTimer >= firstAttackStart && attackTimer <= firstAttackEnd) || attackTimer >= secondAttackStart && attackTimer <= secondAttackEnd || attackTimer >= thirdAttackStart && attackTimer <= thirdAttackEnd);
	}

	static bool CanPlayerDamageEnemy(bool ableToDamage)
	{
		return ableToDamage;
	}

	static bool IsScreenShaking(bool screenShake)
	{
		return screenShake;
	}
};