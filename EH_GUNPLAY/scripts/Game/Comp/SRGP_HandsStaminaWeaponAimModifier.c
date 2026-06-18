class SRGP_HandsStaminaWeaponAimModifier : ScriptedWeaponAimModifier
{
	
	float stanceFactor = 1; // def 1
	int deploymentState = 0;
	float deploymentFactor = 1;
	const int STAMINA_TO_START = 90;
	const float OVERALL_TREMOR_MULT = 0.5;
	
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
			stanceFactor = 0.5;
		else if (stanceFactor < 1)
			stanceFactor = 1;
		
		deploymentState = HSCC.SRGP_IsWeaponDeployed(player);
		
		switch (deploymentState)
		{
			case 1:
				deploymentFactor = 0.5;
				break;
			case 2:
				deploymentFactor = 0.2;
				break;
			default:
				deploymentFactor = 1;
				break;
		}
		
		
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
	    float staminaFactor = 1.0 - Math.InverseLerp(0, STAMINA_TO_START, stamina);
		
		float freqX = 2 + staminaFactor * 1.6;
		float freqY = 2 + staminaFactor * 3;
		
		float noiseX = Math.PerlinNoise(t + freqX) * 5;
		float noiseY = Math.PerlinNoise(t + freqY) * 5;
		
	    turnOffset[0] = noiseX * staminaFactor * stanceFactor * deploymentFactor * OVERALL_TREMOR_MULT;
	    turnOffset[1] = noiseY * staminaFactor * stanceFactor * deploymentFactor * OVERALL_TREMOR_MULT;
		
		//PrintFormat("TO=%1|NO=%2|SF=%3|ST=%4", turnOffset[0], noiseX, staminaFactor, stamina);

	}
	
	protected void CalculateRotation(float stamina, out vector rotation)
	{
		float t = GetGame().GetWorld().GetWorldTime() * 0.001;
    
	    float staminaFactor = 1.0 - Math.InverseLerp(0, STAMINA_TO_START, stamina);
	    
		float freqX = 2 + staminaFactor * 1.6;
		float freqY = 2 + staminaFactor * 3;
		
		float noiseX = Math.PerlinNoise(t + freqX) * 5;
		float noiseY = Math.PerlinNoise(t + freqY) * 5;
	    
	    rotation[0] = noiseX * staminaFactor * stanceFactor * deploymentFactor * OVERALL_TREMOR_MULT;
	    rotation[1] = noiseY * staminaFactor * stanceFactor * deploymentFactor * OVERALL_TREMOR_MULT;
	}
}