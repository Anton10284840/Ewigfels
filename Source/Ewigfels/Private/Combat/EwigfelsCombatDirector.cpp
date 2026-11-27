// Copyright Ewigfels. Alle Rechte vorbehalten.

#include "Combat/EwigfelsCombatDirector.h"
#include "Combat/EwigfelsCombatComponent.h"

void UEwigfelsCombatDirector::Prune()
{
	for (auto It = Tokens.CreateIterator(); It; ++It)
	{
		const UEwigfelsCombatComponent* AttackerCombat = UEwigfelsCombatComponent::Find(It.Key().Get());
		const UEwigfelsCombatComponent* TargetCombat = UEwigfelsCombatComponent::Find(It.Value().Get());
		if (!AttackerCombat || !TargetCombat || AttackerCombat->IsDead() || TargetCombat->IsDead())
		{
			It.RemoveCurrent();
		}
	}
}

int32 UEwigfelsCombatDirector::CountAttackers(const AActor* Target) const
{
	int32 Count = 0;
	for (const auto& Pair : Tokens)
	{
		if (Pair.Value.Get() == Target)
		{
			++Count;
		}
	}
	return Count;
}

bool UEwigfelsCombatDirector::RequestMeleeToken(AActor* Attacker, AActor* Target)
{
	if (!Attacker || !Target)
	{
		return false;
	}
	Prune();
	if (const TWeakObjectPtr<AActor>* Existing = Tokens.Find(Attacker))
	{
		if (Existing->Get() == Target)
		{
			return true;
		}
		Tokens.Remove(Attacker);
	}
	if (CountAttackers(Target) >= MaxMeleeAttackers)
	{
		return false;
	}
	Tokens.Add(Attacker, Target);
	return true;
}

void UEwigfelsCombatDirector::ReleaseMeleeToken(AActor* Attacker)
{
	Tokens.Remove(Attacker);
}

bool UEwigfelsCombatDirector::HasMeleeToken(const AActor* Attacker) const
{
	for (const auto& Pair : Tokens)
	{
		if (Pair.Key.Get() == Attacker)
		{
			return true;
		}
	}
	return false;
}
