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
	float turnVSoft;
	float turnHSoft;
	
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
	
	SRGP_HandsStaminaCharacterComponent HSCC;
	IEntity m_weaponOwner;
	
	override protected void OnActivated(IEntity weaponOwner)
	{
		m_weaponOwner = weaponOwner;
		if (!m_weaponOwner)
			return;
		HSCC = SRGP_HandsStaminaCharacterComponent.Cast(m_weaponOwner.FindComponent(SRGP_HandsStaminaCharacterComponent));
	}
	
	override protected void OnCalculate(IEntity owner, WeaponAimModifierContext context, float timeSlice, out vector translation, out vector rotation, out vector turnOffset)
	{
		translation = vector.Zero;
		rotation = vector.Zero;
		turnOffset = vector.Zero;
		
		if (!m_weaponOwner)
			return;
		if (!SRGP_Utils.SRGP_IsLocalPlayerEntity(m_weaponOwner))
    		return;
		if (!HSCC)
		{
			HSCC = SRGP_HandsStaminaCharacterComponent.Cast(m_weaponOwner.FindComponent(SRGP_HandsStaminaCharacterComponent));
			return;
		}
		
		int stance = SRGP_Utils.SRGP_GetStance(m_weaponOwner);
		
		if (stance == 2)
		{
			turnOffset = vector.Zero;
			return;
		}
		else if (stance == 1)
			stanceFactor = m_fCrouchMultiplier;
		else if (stanceFactor < 1)
			stanceFactor = 1;
		
		deploymentState = SRGP_Utils.SRGP_IsWeaponDeployed(m_weaponOwner);
		
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
		
		float weight = SRGP_Utils.SRGP_GetWeaponWeight(m_weaponOwner);
		weightFactor = LegacyCurve.Curve(
		ECurveType.CurveProperty2D,
		weight,
		m_cTremorOnWeaponWeight)[1];
			
		CalculateMicroTremorTurn(HSCC.GetStamina(), turnOffset, timeSlice);
		CalculateMicroTremorRot(HSCC.GetStamina(), rotation, translation, timeSlice);
	}
	
	protected void CalculateMicroTremorTurn(float stamina, out vector turnOffset, float timeSlice)
	{
		timeSlice = Math.Min(timeSlice, 0.033);
		float staminaFactor = LegacyCurve.Curve(
		ECurveType.CurveProperty2D,
		stamina,
		m_cTremorOnStamina)[1];
		
		float turnV = Math.RandomFloat(-1, 1) * staminaFactor * weightFactor * stanceFactor * deploymentFactor * m_fOverallTremorMult;
		float turnH = Math.RandomFloat(-1, 1) * staminaFactor * weightFactor * stanceFactor * deploymentFactor * m_fOverallTremorMult;
	
		turnVSoft = Math.SmoothSpring(turnVSoft, turnV, m_fSVTurnV, 0.7, 0.5, timeSlice * 25);
		turnHSoft = Math.SmoothSpring(turnHSoft, turnH, m_fSVTurnH, 0.7, 0.5, timeSlice * 25);
		
		turnOffset[0] = Math.Min(Math.Max(turnVSoft, -10), 10);
		turnOffset[1] = Math.Min(Math.Max(turnHSoft, -10), 10);
	}
	
	protected void CalculateMicroTremorRot(float stamina, out vector rotation, out vector translation, float timeSlice)
	{
		timeSlice = Math.Min(timeSlice, 0.033);
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
		
		rotation[0] = Math.Min(Math.Max(rotVSoft, -10), 10);
		rotation[1] = Math.Min(Math.Max(rotHSoft, -10), 10);
		
		translation[0] = Math.Min(Math.Max(rotVSoft * 0.003, -0.5), 0.5);
		translation[1] = Math.Min(Math.Max(rotHSoft * 0.003, -0.5), 0.5);
	}
	
}