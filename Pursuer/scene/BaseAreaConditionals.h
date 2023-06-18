#pragma once

#include "EnemyHuman.h"

#include <DirectXMath.h>

class BaseAreaConditionals
{
public:
	static bool IsScreenNotShaking(bool screenShake)
	{
		if (screenShake)
		{
			return false;
		}

		return true;
	}

	static bool IsCameraLockedOn(bool lockOn)
	{
		if (lockOn)
		{
			return true;
		}

		return false;
	}

	static bool IsPlayerDodging(bool dodge)
	{
		if (dodge)
		{
			return true;
		}

		return false;
	}

	static bool IsInitializationFinished(bool initializeFinished)
	{
		if (initializeFinished)
		{
			return true;
		}

		return false;
	}

	static bool IsGameStarting(bool gameStart)
	{
		if (gameStart)
		{
			return true;
		}

		return false;
	}

	static bool ShouldFadeInBegin(float startTimer, const float startTimerLimit)
	{
		if (startTimer >= startTimerLimit)
		{
			return true;
		}

		return false;
	}

	static bool IsFadeSpriteAlphaBelowZero(float fadeSpriteAlpha, const float minAlpha)
	{
		if (fadeSpriteAlpha <= minAlpha)
		{
			return true;
		}

		return false;
	}

	static bool IsBaseAreaOpeningCutscenePlaying(bool baseAreaOpeningCutscene, bool initializeFinished, float startTimer, const float startTimerLimit)
	{
		if (baseAreaOpeningCutscene && initializeFinished == true && startTimer > startTimerLimit)
		{
			return true;
		}

		return false;
	}

	static bool HasPlayerReachedTriggerToMoveMissionSprite(float zPosition, const float movementStartZPosition)
	{
		if (zPosition >= -movementStartZPosition)
		{
			return true;
		}

		return false;
	}

	static bool IsOpeningCutsceneFinished(float zPosition, const float mapBorder)
	{
		if (zPosition >= -mapBorder)
		{
			return true;
		}

		return false;
	}

	static bool IsEnemyWandering(EnemyHuman::status enumStatus)
	{
		if (enumStatus == EnemyHuman::WANDER)
		{
			return true;
		}

		return false;
	}

	static bool IsEnemyStanding(EnemyHuman::status enumStatus)
	{
		if (enumStatus == EnemyHuman::STAND)
		{
			return true;
		}

		return false;
	}

	static bool IsEnemyAggro(EnemyHuman::status enumStatus)
	{
		if (enumStatus == EnemyHuman::AGGRO)
		{
			return true;
		}

		return false;
	}

	static bool IsEnemyAttacking(EnemyHuman::status enumStatus)
	{
		if (enumStatus == EnemyHuman::CHARGEATTACK || enumStatus == EnemyHuman::JETSTREAMATTACK || enumStatus == EnemyHuman::LANDINGATTACK
			|| enumStatus == EnemyHuman::PARTICLEATTACK || enumStatus == EnemyHuman::ATTACK)
		{
			return true;
		}

		return false;
	}

	static bool IsEnemyCoolingDown(EnemyHuman::status enumStatus)
	{
		if (enumStatus == EnemyHuman::COOLDOWN)
		{
			return true;
		}

		return false;
	}

	static bool ShouldEnemyFlee(float hp, const float fleeHP, bool enemyKnockback, EnemyHuman::status enumStatus, bool helpCall, bool isPartnerDead)
	{
		if (hp <= fleeHP && !enemyKnockback && enumStatus != EnemyHuman::DAMAGED && !helpCall && enumStatus != EnemyHuman::DEAD
			|| isPartnerDead && !enemyKnockback && enumStatus != EnemyHuman::DAMAGED && !helpCall && enumStatus != EnemyHuman::DEAD)
		{
			return true;
		}

		return false;
	}

	static bool HasEnemyNotYetSetFleeTarget(bool fleeSet)
	{
		if (fleeSet)
		{
			return false;
		}

		return true;
	}

	static bool IsEnemyFrontPatrolPosition(EnemyHuman::patrol patrolStatus)
	{
		if (patrolStatus == EnemyHuman::FRONT)
		{
			return true;
		}

		return false;
	}

	static bool IsEnemyBackPatrolPosition(EnemyHuman::patrol patrolStatus)
	{
		if (patrolStatus == EnemyHuman::BACK)
		{
			return true;
		}

		return false;
	}

	static bool IsEnemyBeingComparedWithItself(int j, int i, EnemyHuman::patrol patrolStatus)
	{
		if (j == i || patrolStatus == EnemyHuman::FRONT && j == (i + 1) || patrolStatus == EnemyHuman::BACK && j == (i - 1))
		{
			return true;
		}
		
		return false;
	}

	static bool IsEnemyMinDistanceNewMinDistance(float x, float y, float min, bool isBeingCalledToHelp)
	{
		if (abs(sqrt(x * x + y * y)) < min && isBeingCalledToHelp == false)
		{
			return true;
		}

		return false;
	}

	static bool IsThereNoEnemyAbleToBeCalledToHelp(int closestEnemy, const int closestEnemyDefaultNumber)
	{
		if (closestEnemy == closestEnemyDefaultNumber)
		{
			return true;
		}

		return false;
	}

	static bool ThereIsAnEnemyAbleToHelp(int closestEnemy, const int closestEnemyDefaultNumber)
	{
		if (closestEnemy != closestEnemyDefaultNumber)
		{
			return true;
		}

		return false;
	}

	static bool IsEnemyBeingCalledFrontPatroller(EnemyHuman::patrol patrolStatus)
	{
		if (patrolStatus == EnemyHuman::FRONT)
		{
			return true;
		}

		return false;
	}

	static bool IsEnemyCloseEnoughToCall(float distance, const float helpCallRange, bool helpCall)
	{
		if (distance <= helpCallRange && !helpCall)
		{
			return true;
		}

		return false;
	}

	static bool ShouldEnemyDoJetStreamAttack(int agroodEnemies, const int jetStreamAttackRequiredEnemyNumber, bool debugJetAttacked, bool jetStreamCounted, EnemyHuman::status enumStatus)
	{
		if (agroodEnemies > jetStreamAttackRequiredEnemyNumber && !debugJetAttacked && !jetStreamCounted && enumStatus == EnemyHuman::AGGRO)
		{
			return true;
		}

		return false;
	}

	static bool CanEnemySeePlayer(int intersect, bool enemyKnockback, bool enemyAlive, EnemyHuman::status enumStatus)
	{
		if (intersect && !enemyKnockback && enemyAlive || enumStatus == EnemyHuman::AGGRO && !enemyKnockback && enemyAlive)
		{
			return true;
		}

		return false;
	}

	static bool IsEnemyWithinChargingDistanceAndHasntChargedYet(float distance, const float maxChargeDistance, const float minChargeDistance, bool chargeAttackCheck, EnemyHuman::status enumStatus, bool enemyKnockback, bool enemyAlive)
	{
		if (distance < maxChargeDistance && distance > minChargeDistance && !chargeAttackCheck && enumStatus == EnemyHuman::AGGRO && !enemyKnockback && enemyAlive)
		{
			return true;
		}

		return false;
	}

	static bool SetEnemyAgrooNumberForJetStreamAttackUse(int agrooNumber)
	{
		if (agrooNumber == 0)
		{
			return true;
		}

		return false;
	}

	static bool IsEnemyAgrood(EnemyHuman::status enumStatus)
	{
		if (enumStatus == EnemyHuman::AGGRO)
		{
			return true;
		}

		return false;
	}

	static bool IsEnemyNOTAgrood(EnemyHuman::status enumStatus)
	{
		if (enumStatus != EnemyHuman::AGGRO)
		{
			return true;
		}

		return false;
	}

	static bool IsEnemyWithinMinimumChargeDistance(float distance, float minChargeDistance)
	{
		if (distance <= minChargeDistance)
		{
			return true;
		}

		return false;
	}

	static bool IsEnemyNotDamagedCooldownOrAttacking(EnemyHuman::status enumStatus)
	{
		if (enumStatus != EnemyHuman::DAMAGED && enumStatus != EnemyHuman::COOLDOWN && enumStatus != EnemyHuman::PARTICLEATTACK && enumStatus != EnemyHuman::ATTACK
			&& enumStatus != EnemyHuman::LANDINGATTACK && enumStatus != EnemyHuman::CHARGEATTACK && enumStatus != EnemyHuman::JETSTREAMATTACK && enumStatus != EnemyHuman::TWOENEMYSURROUND)
		{
			return true;
		}
		
		return false;
	}

	static bool IsEnemyCurrentlyParticleAttacking(bool particleAttackActive)
	{
		if (particleAttackActive)
		{
			return true;
		}

		return false;
	}

	static bool HasEnemyHitTheGroundDuringLandingAttack(bool landingParticles)
	{
		if (landingParticles)
		{
			return true;
		}

		return false;
	}

	static bool IsEnemyChargeOrJetStreamAttacking(EnemyHuman::status enumStatus, int chargeAttackStage, int jetStreamAttackStage)
	{
		if (enumStatus == EnemyHuman::CHARGEATTACK && chargeAttackStage == 1
			|| enumStatus == EnemyHuman::JETSTREAMATTACK && jetStreamAttackStage == 2)
		{
			return true;
		}

		return false;
	}

	static bool IsEnemySurroundAttackingThePlayer(EnemyHuman::status enumStatus, int twoEnemySurroundStage)
	{
		if (enumStatus == EnemyHuman::TWOENEMYSURROUND && twoEnemySurroundStage == 1)
		{
			return true;
		}

		return false;
	}

	static bool CreateParticlesAtFrontPatrolPosition(float nextDegree, float initialDegree)
	{
		if (nextDegree > (initialDegree + XMConvertToRadians(90.0f)))
		{
			return true;
		}

		return false;
	}

	static bool CreateParticlesAtBackPatrolPosition(float nextDegree, float initialDegree)
	{
		if (nextDegree < (initialDegree - XMConvertToRadians(90.0f)))
		{
			return true;
		}

		return false;
	}
};