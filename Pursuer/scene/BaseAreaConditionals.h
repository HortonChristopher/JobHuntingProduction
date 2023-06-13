#pragma once

#include "EnemyHuman.h"

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

	static bool IsCameraNotLockedOnAndPlayerNotDodging(bool lockOn, bool dodge)
	{
		if (!lockOn && !dodge)
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

	static bool IsInitializationFinishedAndGameStarting(bool gameStart, bool initializeFinished)
	{
		if (!gameStart && initializeFinished)
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

	static bool IsTheEnemyWanderingOrStanding(EnemyHuman::status enumStatus)
	{
		if (enumStatus == EnemyHuman::WANDER || enumStatus == EnemyHuman::STAND)
		{
			return true;
		}

		return false;
	}

	static bool IsEnemyAggroAttackOrCooldown(EnemyHuman::status enumStatus)
	{
		if (enumStatus == EnemyHuman::AGGRO || enumStatus == EnemyHuman::CHARGEATTACK || enumStatus == EnemyHuman::JETSTREAMATTACK || enumStatus == EnemyHuman::LANDINGATTACK
			|| enumStatus == EnemyHuman::PARTICLEATTACK || enumStatus == EnemyHuman::COOLDOWN || enumStatus == EnemyHuman::ATTACK)
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
};