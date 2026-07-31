class SRGP_RecoilImpulse_AM : ScriptedWeaponAimModifier
{
	[Attribute("0.65", uiwidget: UIWidgets.Auto, desc: "Recoil * this when crouching (def 0.65)", category: "Settings", params: "0 1")]
	float m_fCrouchMultiplier;
	
	const float MAXIMAL_VERTICAL_DEGREES = 20;
	const float MAXIMAL_VERTICAL_IMPULSE = 100;
	const float MAXIMAL_HORIZONTAL_DEGREES = 10;
	const float MAXIMAL_HORIZONTAL_IMPULSE = 100;
	const float RECOIL_SPEED_MULT = 16;
	
	[Attribute("0.15", uiwidget: UIWidgets.Auto, desc: "Overall recoil * this", category: "Settings", params: "0 100")]
	float RECOIL_POWER;
	
	const float RECOIL_HOR_POWER = 0.5;
	const float RECOIL_VERT_POWER = 0.5;
	const float RECOIL_ROLL_POWER = 3;
	const float RECOIL_SPRING = 0.8;
	const float RECOIL_DAMPING = 0.7;
	
	float m_fTotalVerticalImpulse;
	float m_fCurrentVerticalImpulse;
	float m_fTotalHorizontalImpulse;
	float m_fCurrentHorizontalImpulse;
	
	private float m_fVerticalVelocity = 0.8;
	private float m_fHorizontalVelocity = 0.9;
	
	float m_fStanceFactor;
	float m_fdeploymentFactor;
	int m_DeploymentState;
	
	float m_fWeaponMass;
	float m_fBulletInitSpeedCoef;
	float m_fAmmoPower;
	IEntity m_weaponEnt
	IEntity m_weaponOwner;
	MuzzleComponent m_muzzleComp;
	SRGP_HandsStaminaCharacterComponent m_HandsStamCharComp;
	
	override protected void OnInit(IEntity weaponEnt)
	{
		m_weaponEnt = weaponEnt;
	}
	
	override protected void OnActivated(IEntity weaponOwner)
	{
		m_weaponOwner = weaponOwner;
		m_HandsStamCharComp = SRGP_HandsStaminaCharacterComponent.Cast(weaponOwner.FindComponent(SRGP_HandsStaminaCharacterComponent));
		m_muzzleComp = MuzzleComponent.Cast(m_weaponEnt.FindComponent(MuzzleComponent));
	}
	
	override void OnWeaponFired()
	{
		float bulletMass;
		float bulletSpeed;
		
		if (!m_HandsStamCharComp)
		{
			m_HandsStamCharComp = SRGP_HandsStaminaCharacterComponent.Cast(m_weaponOwner.FindComponent(SRGP_HandsStaminaCharacterComponent));
			
			if(!m_HandsStamCharComp)
				return;
			
		}
		
		if(!m_muzzleComp)
			return;
		
		SCR_MuzzleEffectComponent muzzEffComp = SCR_MuzzleEffectComponent.Cast(m_weaponEnt.FindComponent(SCR_MuzzleEffectComponent));
		if (!muzzEffComp)
			return;
		
		int stance = m_HandsStamCharComp.SRGP_GetStance(m_weaponOwner);
		if (stance == 2)
		{
			m_fStanceFactor = m_fCrouchMultiplier * 0.3;
		}
		else if (stance == 1)
			m_fStanceFactor = m_fCrouchMultiplier;
		else if (m_fStanceFactor < 1)
			m_fStanceFactor = 1;
		
		m_DeploymentState = m_HandsStamCharComp.SRGP_IsWeaponDeployed(m_weaponOwner);
		
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
		
		bulletMass = muzzEffComp.GetBulletMass();
		bulletSpeed = muzzEffComp.GetBulletSpeed();
		m_fBulletInitSpeedCoef = m_muzzleComp.GetBulletInitSpeedCoef();
		bulletSpeed *= m_fBulletInitSpeedCoef;
		
		float energy = (bulletMass * bulletSpeed * bulletSpeed) / 2; // bullet energy in J

		float energyFactor = Math.InverseLerp(200, 3000, energy);
		m_fAmmoPower = energyFactor;
		
		m_fWeaponMass = m_HandsStamCharComp.SRGP_GetWeaponWeight(m_weaponOwner);
		float m_fWeaponMassFactor = Math.InverseLerp(20, 0, m_fWeaponMass);
		m_fWeaponMassFactor = Math.Min(m_fWeaponMassFactor, 1);
		m_fWeaponMassFactor = Math.Max(m_fWeaponMassFactor, 0.1);
		
		m_fTotalVerticalImpulse = Math.Lerp(0, MAXIMAL_VERTICAL_IMPULSE, energyFactor) * m_fWeaponMassFactor * m_fStanceFactor * m_fdeploymentFactor * RECOIL_VERT_POWER * RECOIL_POWER;
		m_fTotalHorizontalImpulse = Math.Lerp(0, MAXIMAL_HORIZONTAL_IMPULSE, energyFactor) * m_fWeaponMassFactor * m_fStanceFactor * m_fdeploymentFactor* RECOIL_HOR_POWER * RECOIL_POWER;
		
		//PrintFormat("%1|%2|%3|%4", m_fWeaponMassFactor, energyFactor, m_fTotalVerticalImpulse, m_fBulletInitSpeedCoef);
	}
	
	override void OnCalculate(IEntity owner, WeaponAimModifierContext context, float timeSlice, out vector translation, out vector rotation, out vector turnOffset)
	{
		
		m_fTotalVerticalImpulse = Math.Clamp(m_fTotalVerticalImpulse, 0, MAXIMAL_VERTICAL_DEGREES);
		m_fTotalHorizontalImpulse = Math.Clamp(m_fTotalHorizontalImpulse, 0, MAXIMAL_HORIZONTAL_DEGREES)* Math.RandomFloat(-1, 1);
		
		m_fCurrentVerticalImpulse = Math.SmoothSpring(m_fCurrentVerticalImpulse, m_fTotalVerticalImpulse, m_fVerticalVelocity, RECOIL_SPRING, RECOIL_DAMPING, timeSlice * RECOIL_SPEED_MULT);
		m_fCurrentHorizontalImpulse = Math.SmoothSpring(m_fCurrentHorizontalImpulse, m_fTotalHorizontalImpulse, m_fHorizontalVelocity, RECOIL_SPRING, RECOIL_DAMPING, timeSlice * RECOIL_SPEED_MULT);
		
		rotation[1] = m_fCurrentVerticalImpulse;
		rotation[0] = m_fCurrentHorizontalImpulse;
		rotation[2] = m_fCurrentHorizontalImpulse * RECOIL_ROLL_POWER;
		
		translation[0] = m_fCurrentHorizontalImpulse * 0.0015; // decorative side sway
		translation[2] = Math.Max(m_fCurrentVerticalImpulse * 0.015 * -1, -0.009); // kick
		
		turnOffset[0] = m_fCurrentHorizontalImpulse * 3;
		turnOffset[1] = m_fCurrentVerticalImpulse * 3;
		
		m_fTotalHorizontalImpulse = 0;
		m_fTotalVerticalImpulse = 0;
	}
}