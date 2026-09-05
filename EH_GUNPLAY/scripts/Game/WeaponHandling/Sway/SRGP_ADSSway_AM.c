class SRGP_ADSSway_AM : ScriptedWeaponAimModifier
{
	IEntity m_weaponEnt
	IEntity m_weaponOwner;
	SRGP_HandsStaminaCharacterComponent m_HandsStamCharComp;
	bool m_bWasADSPrev;
	
	[Attribute("0.65", uiwidget: UIWidgets.Slider, desc: "Sway * this when crouching (def 0.65)", category: "Settings", params: "0 1")]
	float m_fCrouchMultiplier;
	
	[Attribute("3", uiwidget: UIWidgets.Slider, desc: "Total sway strength", category: "Settings", params: "0 100")]
	float SWAY_STRENGTH;
	
	[Attribute("0.6", uiwidget: UIWidgets.Slider, desc: "Vertical sping power (how springy gun behaves)", category: "Settings", params: "0 1")]
	float SWAY_SPRING_VERTICAL;
	
	[Attribute("0.4", uiwidget: UIWidgets.Slider, desc: "Vertical damping power (how hard it tries to become fine)", category: "Settings", params: "0 1")]
	float SWAY_DAMPING_VERTICAL;
	
	[Attribute("0.6", uiwidget: UIWidgets.Slider, desc: "Horizontal sping power (how springy gun behaves)", category: "Settings", params: "0 1")]
	float SWAY_SPRING_HORIZONTAL;
	
	[Attribute("0.4", uiwidget: UIWidgets.Slider, desc: "Horizontal damping power (how hard it tries to become fine)", category: "Settings", params: "0 1")]
	float SWAY_DAMPING_HORIZONTAL;
	
	[Attribute("-5", uiwidget: UIWidgets.Slider, desc: "How strong ADS sway impacts on camera movement", category: "Settings", params: "-100 100")]
	float SWAY_CAMERA_IMPACT;
	
	[Attribute("0.015", uiwidget: UIWidgets.Slider, desc: "How strong weapon moves vertically from sway (inverted)", category: "Settings", params: "0 1")]
	float SWAY_MOVE_VERTICAL;
	
	[Attribute("0.015", uiwidget: UIWidgets.Slider, desc: "How strong weapon moves horizontally from sway (inverted)", category: "Settings", params: "0 1")]
	float SWAY_MOVE_HORIZONTAL;
	
	[Attribute("3", uiwidget: UIWidgets.Slider, desc: "Sway roll power (multiplied from horizontal sway)", category: "Settings", params: "0 50")]
	float SWAY_ROLL_POWER;
	
	[Attribute("15", uiwidget: UIWidgets.Slider, desc: "Sway speed (better not to touch actually)", category: "Settings", params: "1 100")]
	float SWAY_SPEED;
	
	[Attribute("0.6", uiwidget: UIWidgets.Slider, desc: "Lower limit for vertical sway impulse", category: "Settings", params: "0 1")]
	float SWAY_MINIMAL_VERTICAL;
	
	[Attribute("0.2", uiwidget: UIWidgets.Slider, desc: "Lower limit for horizontal sway impulse", category: "Settings", params: "0 1")]
	float SWAY_MINIMAL_HORIZONTAL;
	
	[Attribute("true", uiwidget: UIWidgets.CheckBox, desc: "Enable sway when quit ADSing", category: "Settings")]
	bool SWAY_ENABLE_OUT;
	
	private float m_fSwayImpulsePower;
	private float m_fTargetSwayImpulse;
	private float m_fTargetSwayHORImpulse;
	private float m_fCurrentSwayImpulse;
	private float m_fCurrentSwayHORImpulse;
	private float m_fSwayVelocity = 0.8;
	private float m_fHORSwayVelocity = 0.8;
	private float m_fNoiseSeed;
	private float m_fNoiseSeedHOR;
	
	float m_fStanceFactor;
	float m_fdeploymentFactor;
	int m_DeploymentState;
	
	override protected void OnInit(IEntity weaponEnt)
	{
		m_weaponEnt = weaponEnt;
	}
	
	override protected void OnActivated(IEntity weaponOwner)
	{
		m_weaponOwner = weaponOwner;
		m_HandsStamCharComp = SRGP_HandsStaminaCharacterComponent.Cast(weaponOwner.FindComponent(SRGP_HandsStaminaCharacterComponent));
	}
	
	override void OnCalculate(IEntity owner, WeaponAimModifierContext context, float timeSlice, out vector translation, out vector rotation, out vector turnOffset)
	{	
		float t = GetGame().GetWorld().GetWorldTime() * 0.001;
		
		if (!m_HandsStamCharComp)
			return;
		if (!owner)
			return;
		if (!m_weaponEnt)
			return;
		if (!m_weaponOwner)
			return;
		
		if (SRGP_Utils.SRGP_IsInADS(m_weaponOwner) && !m_bWasADSPrev)
		{
			m_fSwayImpulsePower = 0.3;
			m_fNoiseSeed = Math.RandomFloat(0, 100);
    		m_fNoiseSeedHOR = Math.RandomFloat(0, 100);
			m_bWasADSPrev = true;
		}
		
		if (!SRGP_Utils.SRGP_IsInADS(m_weaponOwner) && m_bWasADSPrev)
		{
			if (SWAY_ENABLE_OUT)
			{
				m_fSwayImpulsePower = 0.3;
				m_fNoiseSeed = Math.RandomFloat(0, 100);
	    		m_fNoiseSeedHOR = Math.RandomFloat(0, 100);
			}
			m_bWasADSPrev = false;
		}
		
		int stance = SRGP_Utils.SRGP_GetStance(m_weaponOwner);
		if (stance == 2)
		{
			m_fStanceFactor = m_fCrouchMultiplier * 0.3;
		}
		else if (stance == 1)
			m_fStanceFactor = m_fCrouchMultiplier;
		else if (m_fStanceFactor < 1)
			m_fStanceFactor = 1;
		
		m_DeploymentState = SRGP_Utils.SRGP_IsWeaponDeployed(m_weaponOwner);
		
		switch (m_DeploymentState)
		{
			case 1:
				m_fdeploymentFactor = 0.5;
				break;
			case 2:
				m_fdeploymentFactor = 0.3;
				break;
			default:
				m_fdeploymentFactor = 1;
				break;
		}
		
		m_fNoiseSeed += timeSlice * 0.7;
		m_fNoiseSeedHOR += timeSlice * 0.2;
		
		float freq1 = 0.7;
		float freq2 = 1.3;
		float freq3 = 1.9;
		
		m_fTargetSwayImpulse = Math.Sin(m_fNoiseSeed * freq1) * 0.6 
		                     + Math.Sin(m_fNoiseSeed * freq2) * 0.3 
		                     + Math.Sin(m_fNoiseSeed * freq3) * 0.1;
		
		m_fTargetSwayHORImpulse = Math.Sin(m_fNoiseSeedHOR * 1.1 + 0.5) * 0.5 
		                        + Math.Sin(m_fNoiseSeedHOR * 1.7 + 1.2) * 0.35 
		                        + Math.Sin(m_fNoiseSeedHOR * 2.3 + 2.1) * 0.15;
		
		m_fTargetSwayImpulse = Math.Clamp(m_fTargetSwayImpulse, -1.0, 1.0);
		m_fTargetSwayHORImpulse = Math.Clamp(m_fTargetSwayHORImpulse, -0.7, 0.7);
		
		if (Math.AbsFloat(m_fTargetSwayImpulse) < SWAY_MINIMAL_VERTICAL)
		{
		    if (m_fTargetSwayImpulse >= 0)
		        m_fTargetSwayImpulse = SWAY_MINIMAL_VERTICAL;
		    else
		        m_fTargetSwayImpulse = SWAY_MINIMAL_VERTICAL * -1;
		}
		
		if (Math.AbsFloat(m_fTargetSwayHORImpulse) < SWAY_MINIMAL_HORIZONTAL)
		{
		    if (m_fTargetSwayHORImpulse >= 0)
		        m_fTargetSwayHORImpulse = SWAY_MINIMAL_HORIZONTAL;
		    else
		        m_fTargetSwayHORImpulse = SWAY_MINIMAL_HORIZONTAL * -1;
		}
		
		float swayStrength = m_fSwayImpulsePower * SWAY_STRENGTH * m_fStanceFactor * m_fdeploymentFactor;
		m_fTargetSwayImpulse *= swayStrength;
		m_fTargetSwayHORImpulse *= swayStrength;
		
		m_fCurrentSwayImpulse = Math.SmoothSpring(m_fCurrentSwayImpulse, m_fTargetSwayImpulse, m_fSwayVelocity, SWAY_SPRING_VERTICAL, SWAY_DAMPING_VERTICAL, timeSlice * SWAY_SPEED);
		m_fCurrentSwayHORImpulse = Math.SmoothSpring(m_fCurrentSwayHORImpulse, m_fTargetSwayHORImpulse, m_fHORSwayVelocity, SWAY_SPRING_HORIZONTAL, SWAY_DAMPING_HORIZONTAL, timeSlice * SWAY_SPEED);
		
		// 0hor 1ver 2roll
		rotation[0] = m_fCurrentSwayHORImpulse;
		rotation[1] = m_fCurrentSwayImpulse;
		rotation[2] = m_fCurrentSwayHORImpulse * SWAY_ROLL_POWER * -1;
		
		translation[0] = m_fCurrentSwayHORImpulse * -1 * SWAY_MOVE_HORIZONTAL;
		translation[1] = m_fCurrentSwayImpulse * -1 * SWAY_MOVE_VERTICAL;
		
		turnOffset[0] = m_fCurrentSwayHORImpulse * SWAY_CAMERA_IMPACT;
		turnOffset[1] = m_fCurrentSwayImpulse * SWAY_CAMERA_IMPACT;
		
		if (m_fSwayImpulsePower > 0)
			m_fSwayImpulsePower -= 0.01;
		if (m_fSwayImpulsePower < 0)
			m_fSwayImpulsePower = 0;
	}
}