class SRGP_HandsStaminaWeaponAimModifier : ScriptedWeaponAimModifier
{
	float stanceFactor = 1; // def 1
	const int STAMINA_TO_START = 90;
	
	override protected void OnCalculate(IEntity owner, WeaponAimModifierContext context, float timeSlice, out vector translation, out vector rotation, out vector turnOffset)
	{
		PlayerController pc = GetGame().GetPlayerController();
		if (!pc)
			return;
		SCR_ChimeraCharacter player = SCR_ChimeraCharacter.Cast(pc.GetControlledEntity());
		if (!player)
			return;
		SRGP_HandsStaminaCharacterComponent HSCC = SRGP_HandsStaminaCharacterComponent.Cast(player.FindComponent(SRGP_HandsStaminaCharacterComponent));
		if(!HSCC)
			return;
		
		int stance = HSCC.SRGP_GetStance(player);
		
		if (stance == 2)
		{
			turnOffset = vector.Zero;
			return;
		}
		else if (stance == 1)
			stanceFactor == 0.5;
		else if (stanceFactor < 1)
			stanceFactor == 1;
		
		if (HSCC.GetStamina() < 90 && HSCC.SRGP_IsInADS(player))
		{
			CalculateTremor(HSCC.GetStamina(), turnOffset);
			CalculateRotation(HSCC.GetStamina(), rotation);
		}
		else if (!HSCC.SRGP_IsInADS(player))
			turnOffset = vector.Zero;
	}
	
	protected void CalculateTremor(float stamina, out vector turnOffset)
	{
		
		float t = GetGame().GetWorld().GetWorldTime() * 0.001;

		float staminaFactor = 1.0 - (stamina / STAMINA_TO_START);
		
		float freq = 1.5 + staminaFactor * 1;
		//PrintFormat("STM:%1 | STF:%2 | FRQ:%3", stamina, staminaFactor, freq);
		
		turnOffset[0] = Math.PerlinNoise(t * freq + 10.0, -5, 5) * staminaFactor * stanceFactor;
		turnOffset[1] = Math.PerlinNoise(t * freq + 100.0, -5, 5) * staminaFactor * stanceFactor;

	}
	
	protected void CalculateRotation(float stamina, out vector rotation)
	{
		
		float t = GetGame().GetWorld().GetWorldTime() * 0.001;

		float staminaFactor = 1.0 - (stamina / STAMINA_TO_START);
		
		float freq = 1.5 + staminaFactor * 1;
		
		rotation[0] = Math.PerlinNoise(t * freq + 10.0, -5, 5) * staminaFactor * stanceFactor;
		rotation[1] = Math.PerlinNoise(t * freq + 100.0, -5, 5) * staminaFactor * stanceFactor;

	}
}