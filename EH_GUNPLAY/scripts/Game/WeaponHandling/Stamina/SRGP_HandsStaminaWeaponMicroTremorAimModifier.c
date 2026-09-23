class SRGP_HandsStaminaWeaponMicroTremorAimModifier : ScriptedWeaponAimModifier
{
	float stanceFactor = 1; // def 1
	int deploymentState = 0;
	float deploymentFactor = 1;
	float weightFactor = 1;
	
	float m_fSVTurnH;
	float m_fSVTurnV;
	float m_fSVRotV;
	float m_fSVRotH;
	
	float rotVSoft;
	float rotHSoft;
	
	float rotV;
	float rotH;
	
	float m_fTick;
	
	[Attribute("0.1", uiwidget: UIWidgets.Auto, desc: "Total tremor power (def 0.5)", category: "Settings", params: "0 100")]
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
			weightFactor = LegacyCurve.Curve(
			ECurveType.CurveProperty2D,
			weight,
			m_cTremorOnWeaponWeight)[1];
			
			CalculateMicroTremorTurn(HSCC.GetStamina(), turnOffset, timeSlice);
			CalculateMicroTremorRot(HSCC.GetStamina(), rotation, translation, timeSlice);
		}
		else if (!SRGP_Utils.SRGP_IsInADS(player))
			turnOffset = vector.Zero;
	}
	
	
	protected void CalculateMicroTremorTurn(float stamina, out vector turnOffset, float timeSlice)
	{
		float staminaFactor = LegacyCurve.Curve(
		ECurveType.CurveProperty2D,
		stamina,
		m_cTremorOnStamina)[1];
		
		float turnV = Math.RandomFloat(-1, 1) * staminaFactor * weightFactor * stanceFactor * deploymentFactor * m_fOverallTremorMult;
		float turnH = Math.RandomFloat(-1, 1) * staminaFactor * weightFactor * stanceFactor * deploymentFactor * m_fOverallTremorMult;
	
		float turnVSoft = Math.SmoothSpring(turnVSoft, turnV, m_fSVTurnV, 0.7, 0.5, timeSlice * 25);
		float turnHSoft = Math.SmoothSpring(turnHSoft, turnH, m_fSVTurnH, 0.7, 0.5, timeSlice * 25);
		
		turnOffset[0] = turnVSoft;
		turnOffset[1] = turnHSoft;

	}
	
	protected void CalculateMicroTremorRot(float stamina, out vector rotation, out vector translation, float timeSlice)
	{
		float staminaFactor = LegacyCurve.Curve(
		ECurveType.CurveProperty2D,
		stamina,
		m_cTremorOnStamina)[1];
		
		m_fTick -= timeSlice;
	    if (m_fTick <= 0)
	    {
			m_fTick = Math.RandomFloat(0.05, 0.1);
			float mult = staminaFactor * weightFactor * stanceFactor * deploymentFactor * m_fOverallTremorMult;
			rotV = Math.RandomFloat(-1, 1) * mult;
			rotH = Math.RandomFloat(-1, 1) * mult;
		}
	
		rotVSoft = Math.SmoothSpring(rotVSoft, rotV, m_fSVRotV, 0.6, 0.3, timeSlice * 35);
		rotHSoft = Math.SmoothSpring(rotHSoft, rotH, m_fSVRotH, 0.6, 0.3, timeSlice * 35);
		
		rotation[0] = rotVSoft;
		rotation[1] = rotHSoft;
		
		translation[0] = rotVSoft * 0.003;
		translation[1] = rotHSoft * 0.003;

	}
	
}