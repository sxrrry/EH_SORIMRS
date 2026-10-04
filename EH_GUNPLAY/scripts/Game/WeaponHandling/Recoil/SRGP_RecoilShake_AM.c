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
			float weightFactor = Math.InverseLerp(20, 0, weight);
			weightFactor = Math.Min(weightFactor, 1);
			weightFactor = Math.Max(weightFactor, 0.1);
			float mult = weightFactor * stanceFactor * deploymentFactor * m_fOverallShakeMult;
			float shakeV = Math.RandomFloat(0.6, 1);
			float shakeH = Math.RandomFloat(0.6, 1);
			if (Math.RandomFloat(0, 1) > 0.5)
				shakeV*=-1;
			if (Math.RandomFloat(0, 1) > 0.5)
				shakeH*=-1;
			
			rotV = shakeV * mult;
			rotH = shakeH * mult;
			
			m_fShake = Math.Max(0, m_fShake - 12 * timeSlice);
		}
	
		rotV *= m_fShake;
		rotH *= m_fShake;
		
		rotVSoft = Math.SmoothSpring(rotVSoft, rotV, m_fSVRotV, 0.9, 0.6, timeSlice * 35);
		rotHSoft = Math.SmoothSpring(rotHSoft, rotH, m_fSVRotH, 0.9, 0.6, timeSlice * 35);
		
		rotation[0] = rotVSoft;
		rotation[1] = rotHSoft;
		
		translation[0] = rotHSoft * 0.005;
		translation[1] = rotVSoft * 0.005;
	}
	
	override void OnWeaponFired()
	{
		m_fShake = 1;
	}
	
}