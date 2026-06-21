class SRGP_HandsStaminaWeaponMicroTremorAimModifier : ScriptedWeaponAimModifier
{
	float stanceFactor = 1; // def 1
	int deploymentState = 0;
	float deploymentFactor = 1;
	float weightFactor = 1;
	
	//[Attribute("90", uiwidget: UIWidgets.Auto, desc: "Stamina from which tremor starts (def 90)", category: "Settings", params: "1 100")]
	//int STAMINA_TO_START;
	[Attribute("0.5", uiwidget: UIWidgets.Auto, desc: "Total tremor power (def 0.5)", category: "Settings", params: "0 1")]
	float m_fOverallTremorMult;
	[Attribute("0.5", uiwidget: UIWidgets.Auto, desc: "Tremor * this when crouching (def 0.5)", category: "Settings", params: "0 1")]
	float m_fCrouchMultiplier;
	[Attribute("0 0 100 1", uiwidget: UIWidgets.CurveDialog, desc: "Relation of microtremor to stamina", category: "Settings", params: "100 1 0 0")]
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
		if(!HSCC)
			return;
		
		int stance = HSCC.SRGP_GetStance(player);
		
		if (stance == 2)
		{
			turnOffset = vector.Zero;
			return;
		}
		else if (stance == 1)
			stanceFactor = m_fCrouchMultiplier;
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
		
		if (HSCC.SRGP_IsInADS(player))
		{
			float weight = HSCC.SRGP_GetWeaponWeight(player);
			weightFactor = LegacyCurve.Curve(
			ECurveType.CurveProperty2D,
			weight,
			m_cTremorOnWeaponWeight)[1];
			
			CalculateMicroTremorTurn(HSCC.GetStamina(), turnOffset);
			CalculateMicroTremorRot(HSCC.GetStamina(), rotation);
		}
		else if (!HSCC.SRGP_IsInADS(player))
			turnOffset = vector.Zero;
	}
	
	
	protected void CalculateMicroTremorTurn(float stamina, out vector turnOffset)
	{
		float staminaFactor = LegacyCurve.Curve(
		ECurveType.CurveProperty2D,
		stamina,
		m_cTremorOnStamina)[1];
		
		turnOffset[0] = Math.RandomFloat(-1, 1) * staminaFactor * weightFactor * stanceFactor * deploymentFactor * m_fOverallTremorMult;
		turnOffset[1] = Math.RandomFloat(-1, 1) * staminaFactor * weightFactor * stanceFactor * deploymentFactor * m_fOverallTremorMult;

	}
	
	protected void CalculateMicroTremorRot(float stamina, out vector rotation)
	{
		float staminaFactor = LegacyCurve.Curve(
		ECurveType.CurveProperty2D,
		stamina,
		m_cTremorOnStamina)[1];
		
		rotation[0] = Math.RandomFloat(-1, 1) * staminaFactor * weightFactor * stanceFactor * deploymentFactor * m_fOverallTremorMult;
		rotation[1] = Math.RandomFloat(-1, 1) * staminaFactor * weightFactor * stanceFactor * deploymentFactor * m_fOverallTremorMult;

	}
	
}