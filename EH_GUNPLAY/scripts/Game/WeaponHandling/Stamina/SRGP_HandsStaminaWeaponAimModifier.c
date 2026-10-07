class SRGP_HandsStaminaWeaponAimModifier : ScriptedWeaponAimModifier
{
	private float stanceFactor = 1;
	private int deploymentState = 0;
	private float deploymentFactor = 1;
	private float weightFactor = 1;
	
	private float m_fControlCheckTDelta;
	private bool m_bIsLocalPlayer;
	
	[Attribute("0.5", uiwidget: UIWidgets.Auto, desc: "Total tremor power (def 0.5)", category: "Settings", params: "0 1")]
	private float m_fOverallTremorMult;
	[Attribute("0.5", uiwidget: UIWidgets.Auto, desc: "Tremor * this when crouching (def 0.5)", category: "Settings", params: "0 1")]
	private float m_fCrouchMultiplier;
	[Attribute("0 0 100 1", uiwidget: UIWidgets.CurveDialog, desc: "Relation of tremor to stamina", category: "Settings", params: "100 1 0 0")]
	private ref Curve m_cTremorOnStamina;
	[Attribute("0 0 20 3", uiwidget: UIWidgets.CurveDialog, desc: "Relation of tremor to weapon weight", category: "Settings", params: "20 3 0 0")]
	private ref Curve m_cTremorOnWeaponWeight;
	
	private IEntity m_weaponOwner
	private SRGP_HandsStaminaCharacterComponent HSCC;
	
	override protected void OnActivated(IEntity weaponOwner)
	{
		m_fControlCheckTDelta = 1;
		m_bIsLocalPlayer = false;
		m_weaponOwner = weaponOwner;
		if (!m_weaponOwner)
			return;
		HSCC = SRGP_HandsStaminaCharacterComponent.Cast(m_weaponOwner.FindComponent(SRGP_HandsStaminaCharacterComponent));
	}
	
	override protected void OnCalculate(IEntity owner, WeaponAimModifierContext context, float timeSlice, out vector translation, out vector rotation, out vector turnOffset)
	{
		timeSlice = Math.Min(timeSlice, 0.033);
		translation = vector.Zero;
		rotation = vector.Zero;
		turnOffset = vector.Zero;
		
		if (!m_weaponOwner)
			return;
		if (m_fControlCheckTDelta >= 0.5)
		{
			m_fControlCheckTDelta = 0;
			m_bIsLocalPlayer = SRGP_Utils.SRGP_IsLocalPlayerEntity(m_weaponOwner);
		}
		m_fControlCheckTDelta += timeSlice;
		if (!m_bIsLocalPlayer)
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
		float weightFactor = LegacyCurve.Curve(
		ECurveType.CurveProperty2D,
		weight,
		m_cTremorOnWeaponWeight)[1];
			
		CalculateTurn(HSCC.GetStamina(), turnOffset);
		CalculateRotation(HSCC.GetStamina(), rotation);
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
		
	    turnOffset[0] = Math.Min(Math.Max(noiseX * staminaFactor * weightFactor * stanceFactor * deploymentFactor * m_fOverallTremorMult, -10), 10);
	    turnOffset[1] = Math.Min(Math.Max(noiseY * staminaFactor * weightFactor * stanceFactor * deploymentFactor * m_fOverallTremorMult, -10), 10);
		
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
	    
	    rotation[0] = Math.Min(Math.Max(noiseX * staminaFactor * weightFactor * stanceFactor * deploymentFactor * m_fOverallTremorMult, -10), 10);
	    rotation[1] = Math.Min(Math.Max(noiseY * staminaFactor * weightFactor * stanceFactor * deploymentFactor * m_fOverallTremorMult, -10), 10);
	}
}