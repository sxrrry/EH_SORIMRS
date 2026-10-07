class SRGP_RecoilShake_AM : ScriptedWeaponAimModifier
{
	private float stanceFactor = 1; // def 1
	private int deploymentState = 0;
	private float deploymentFactor = 1;
	private float weightFactor = 1;
	
	private float m_fShake;
	
	private float m_fSVTurnH;
	private float m_fSVTurnV;
	private float m_fSVRotV;
	private float m_fSVRotH;
	
	private float rotVSoft;
	private float rotHSoft;
	
	private float rotV;
	private float rotH;
	
	private float m_fTick;
	
	private float m_fControlCheckTDelta;
	private bool m_bIsLocalPlayer;
	
	[Attribute("0.5", uiwidget: UIWidgets.Auto, desc: "Total shake power (def 0.5)", category: "Settings", params: "0 100")]
	private float m_fOverallShakeMult;
	[Attribute("0.7", uiwidget: UIWidgets.Auto, desc: "Shake * this when crouching (def 0.7)", category: "Settings", params: "0 1")]
	private float m_fCrouchMultiplier;
	
	private IEntity m_weaponOwner;
	
	override protected void OnActivated(IEntity weaponOwner)
	{
		m_fControlCheckTDelta = 1;
		m_bIsLocalPlayer = false;
		m_weaponOwner = weaponOwner;
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
		
	    if (m_fShake > 0)
	    {
			float weight = SRGP_Utils.SRGP_GetWeaponWeight(m_weaponOwner);
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
		
		rotation[0] = Math.Min(Math.Max(rotVSoft, -10), 10);
		rotation[1] = Math.Min(Math.Max(rotHSoft, -10), 10);
		
		translation[0] = Math.Min(Math.Max(rotHSoft * 0.005, -0.5), 0.5);
		translation[1] = Math.Min(Math.Max(rotVSoft * 0.005, -0.5), 0.5);
	}
	
	override void OnWeaponFired()
	{
		m_fShake = 1;
	}
	
}