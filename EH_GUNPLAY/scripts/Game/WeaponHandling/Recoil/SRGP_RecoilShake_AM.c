class SRGP_RecoilShake_AM : ScriptedWeaponAimModifier
{
	float stanceFactor = 1; // def 1
	int deploymentState = 0;
	float deploymentFactor = 1;
	float weightFactor = 1;
	
	float m_fShake;
	
	float m_fSVTurnH;
	float m_fSVTurnV;
	float m_fSVRotV;
	float m_fSVRotH;
	
	float rotVSoft;
	float rotHSoft;
	
	float rotV;
	float rotH;
	
	float m_fTick;
	
	[Attribute("1", uiwidget: UIWidgets.Auto, desc: "Total shake power (def 0.5)", category: "Settings", params: "0 100")]
	float m_fOverallShakeMult;
	[Attribute("0.5", uiwidget: UIWidgets.Auto, desc: "Shake * this when crouching (def 0.5)", category: "Settings", params: "0 1")]
	float m_fCrouchMultiplier;
	[Attribute("0 0 20 3", uiwidget: UIWidgets.CurveDialog, desc: "Relation of shake to weapon weight", category: "Settings", params: "20 3 0 0")]
	protected ref Curve m_cShakeOnWeaponWeight;
	
	override protected void OnCalculate(IEntity owner, WeaponAimModifierContext context, float timeSlice, out vector translation, out vector rotation, out vector turnOffset)
	{
		PlayerController pc = GetGame().GetPlayerController();
		if (!pc)
			return;
		SCR_ChimeraCharacter player = SCR_ChimeraCharacter.Cast(pc.GetControlledEntity());
		if (!player)
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
		
	    if (m_fShake > 0)
	    {
			float weight = SRGP_Utils.SRGP_GetWeaponWeight(player);
			weightFactor = LegacyCurve.Curve(
			ECurveType.CurveProperty2D,
			weight,
			m_cShakeOnWeaponWeight)[1];
			float mult = weightFactor * stanceFactor * deploymentFactor * m_fOverallShakeMult;
			rotV = Math.RandomFloat(-1, 1) * mult;
			rotH = Math.RandomFloat(-1, 1) * mult;
			
			m_fShake = Math.Max(0, m_fShake - 10 * timeSlice);
		}
	
		rotV *= m_fShake;
		rotH *= m_fShake;
		
		rotVSoft = Math.SmoothSpring(rotVSoft, rotV, m_fSVRotV, 0.6, 0.5, timeSlice * 35);
		rotHSoft = Math.SmoothSpring(rotHSoft, rotH, m_fSVRotH, 0.6, 0.5, timeSlice * 35);
		
		rotation[0] = rotVSoft;
		rotation[1] = rotHSoft;
		
		translation[0] = rotHSoft * 0.01;
		translation[1] = rotVSoft * 0.01;
	}
	
	override void OnWeaponFired()
	{
		m_fShake = 1;
	}
	
}