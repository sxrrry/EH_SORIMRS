class SRGP_HandsStaminaWeaponAimModifier : ScriptedWeaponAimModifier
{
	
	float stanceFactor = 1; // def 1
	int deploymentState = 0;
	float deploymentFactor = 1;
	float weightFactor = 1;
	
	//[Attribute("90", uiwidget: UIWidgets.Auto, desc: "Stamina from which tremor starts (def 90)", params: "1 100")]
	//int STAMINA_TO_START;
	[Attribute("0.5", uiwidget: UIWidgets.Auto, desc: "Total tremor power (def 0.5)", category: "Settings", params: "0 1")]
	float m_fOverallTremorMult;
	[Attribute("0.5", uiwidget: UIWidgets.Auto, desc: "Tremor * this when crouching (def 0.5)", category: "Settings", params: "0 1")]
	float m_fCrouchMultiplier;
	[Attribute("0 0 100 1", uiwidget: UIWidgets.CurveDialog, desc: "Relation of tremor to stamina", category: "Settings", params: "100 1 0 0")]
	protected ref Curve m_cTremorOnStamina;
	[Attribute("0 0 20 3", uiwidget: UIWidgets.CurveDialog, desc: "Relation of tremor to weapon weight", category: "Settings", params: "20 3 0 0")]
	protected ref Curve m_cTremorOnWeaponWeight;
	
	override protected void OnCalculate(IEntity owner, WeaponAimModifierContext context, float timeSlice, out vector translation, out vector rotation, out vector turnOffset)
	{
		PlayerController pc = GetGame().GetPlayerController();
		if (!pc)
			return;
		SCR_ChimeraCharacter player = SCR_ChimeraCharacter.Cast(pc.GetControlledEntity());
		if (!player)
			return;
		SRGP_HandsStaminaCharacterComponent HSCC = SRGP_HandsStaminaCharacterComponent.Cast(player.FindComponent(SRGP_HandsStaminaCharacterComponent));
		if (!HSCC)
			return;
		
		int stance = SRGP_Utils.SRGP_GetStance(player);
		
		if (stance == 2)
		{
			turnOffset = vector.Zero;
			return;
		}
		else if (stance == 1)
			stanceFactor = m_fCrouchMultiplier;
		else if (stanceFactor < 1)
			stanceFactor = 1;
		
		deploymentState = SRGP_Utils.SRGP_IsWeaponDeployed(player);
		
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
		
		if (SRGP_Utils.SRGP_IsInADS(player))
		{
			float weight = SRGP_Utils.SRGP_GetWeaponWeight(player);
			float weightFactor = LegacyCurve.Curve(
			ECurveType.CurveProperty2D,
			weight,
			m_cTremorOnWeaponWeight)[1];
			
			CalculateTurn(HSCC.GetStamina(), turnOffset);
			CalculateRotation(HSCC.GetStamina(), rotation);
		}
		else if (!SRGP_Utils.SRGP_IsInADS(player))
			turnOffset = vector.Zero;
	}
	
	protected void CalculateTurn(float stamina, out vector turnOffset)
	{
		float t = GetGame().GetWorld().GetWorldTime() * 0.001;
		
		float staminaFactor = LegacyCurve.Curve(
		ECurveType.CurveProperty2D,
		stamina,
		m_cTremorOnStamina)[1];
		
		float freqX = 2 + staminaFactor * 1.6;
		float freqY = 2 + staminaFactor * 3;
		
		float noiseX = Math.PerlinNoise(t + freqX) * 5;
		float noiseY = Math.PerlinNoise(t + freqY) * 5;
		
	    turnOffset[0] = noiseX * staminaFactor * weightFactor * stanceFactor * deploymentFactor * m_fOverallTremorMult;
	    turnOffset[1] = noiseY * staminaFactor * weightFactor * stanceFactor * deploymentFactor * m_fOverallTremorMult;
		
		//PrintFormat("TO=%1|NO=%2|SF=%3|ST=%4", turnOffset[0], noiseX, staminaFactor, stamina);
	}
	
	protected void CalculateRotation(float stamina, out vector rotation)
	{
		float t = GetGame().GetWorld().GetWorldTime() * 0.001;
    
		float staminaFactor = LegacyCurve.Curve(
		ECurveType.CurveProperty2D,
		stamina,
		m_cTremorOnStamina)[1];
	    
		float freqX = 2 + staminaFactor * 1.6;
		float freqY = 2 + staminaFactor * 3;
		
		float noiseX = Math.PerlinNoise(t + freqX) * 5;
		float noiseY = Math.PerlinNoise(t + freqY) * 5;
	    
	    rotation[0] = noiseX * staminaFactor * weightFactor * stanceFactor * deploymentFactor * m_fOverallTremorMult;
	    rotation[1] = noiseY * staminaFactor * weightFactor * stanceFactor * deploymentFactor * m_fOverallTremorMult;
	}
	
	
	/* reduce stamina on shot
	override protected void OnWeaponFired()
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
		
		float stamina = HSCC.GetStamina();
		stamina -= 0.2 * HSCC.SRGP_GetWeaponWeight(player);
		if (stamina < 0)
			stamina = 0;
		Print(stamina - (stamina - 0.2 * HSCC.SRGP_GetWeaponWeight(player)));
		HSCC.SetStamina(stamina);
		
	}
	*/
	
}