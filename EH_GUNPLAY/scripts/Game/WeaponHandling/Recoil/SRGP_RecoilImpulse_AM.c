class SRGP_RecoilImpulse_AM : ScriptedWeaponAimModifier
{
	[Attribute("0.65", uiwidget: UIWidgets.Slider, desc: "Recoil * this when crouching (def 0.65)", category: "Settings", params: "0 1")]
	float m_fCrouchMultiplier;
	
	const float MAXIMAL_VERTICAL_DEGREES = 20;
	const float MAXIMAL_VERTICAL_IMPULSE = 100;
	const float MAXIMAL_HORIZONTAL_DEGREES = 10;
	const float MAXIMAL_HORIZONTAL_IMPULSE = 100;

	[Attribute("0.15", uiwidget: UIWidgets.Slider, desc: "Overall recoil * this", category: "Settings", params: "0 5")]
	float RECOIL_POWER;
	
	[Attribute("0.7", uiwidget: UIWidgets.Slider, desc: "Horizontal recoil power multiplier", category: "Settings", params: "0 5")]
	float RECOIL_HOR_POWER;  // \/
	
	[Attribute("0.5", uiwidget: UIWidgets.Slider, desc: "Vertical recoil power multiplier", category: "Settings", params: "0 5")]
	float RECOIL_VERT_POWER; // > ~0.5 realistic
	
	[Attribute("11", uiwidget: UIWidgets.Slider, desc: "Recoil roll power", category: "Settings", params: "0 100")]
	float RECOIL_ROLL_POWER; // 0.5 hor = ~15 this - best
	
	[Attribute("0.8", uiwidget: UIWidgets.Slider, desc: "Recoil spring", category: "Settings", params: "0 1")]
	float RECOIL_SPRING;
	
	[Attribute("0.7", uiwidget: UIWidgets.Slider, desc: "Recoil damping", category: "Settings", params: "0 1")]
	float RECOIL_DAMPING;
	
	[Attribute("6", uiwidget: UIWidgets.Slider, desc: "How fast all recoil happens...?", category: "Settings", params: "0.1 50")]
	float RECOIL_SPEED_MULT;
	
	[Attribute("3", uiwidget: UIWidgets.Slider, desc: "How much recoil converts into aim kick (camera turn)", category: "Settings", params: "0 100")]
	float RECOIL_AIMKICK_VERTICAL;
	
	[Attribute("3", uiwidget: UIWidgets.Slider, desc: "How much recoil converts into aim kick (camera turn)", category: "Settings", params: "0 100")]
	float RECOIL_AIMKICK_HORIZONTAL;
	
	[Attribute("0.0015", uiwidget: UIWidgets.Slider, desc: "How much gun will travel horizontally in hands (hor recoil impulse * this)", category: "Settings", params: "0 0.25")]
	float RECOIL_GUN_SIDEMOVE;
	
	[Attribute("0.005", uiwidget: UIWidgets.Slider, desc: "How much gun will travel horizontally into shoulder (hor recoil impulse * this)", category: "Settings", params: "0 0.25")]
	float RECOIL_GUN_SHOULDERKICK;
	
	[Attribute("-0.003", uiwidget: UIWidgets.Slider, desc: "Limits gun kick distance by this (so gun wont pierce shoulder)", category: "Settings", params: "-0.5 0")]
	float RECOIL_GUN_SHOULDERKICK_LIMIT;
	
	private float m_fTotalVerticalImpulse;
	private float m_fCurrentVerticalImpulse;
	private float m_fTotalHorizontalImpulse;
	private float m_fCurrentHorizontalImpulse;
	
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
	
	override protected void OnInit(IEntity weaponEnt)
	{
		m_weaponEnt = weaponEnt;
	}
	
	override protected void OnActivated(IEntity weaponOwner)
	{
		m_weaponOwner = weaponOwner;
		m_muzzleComp = MuzzleComponent.Cast(m_weaponEnt.FindComponent(MuzzleComponent));
	}
	
	override void OnWeaponFired()
	{
		float bulletMass;
		float bulletSpeed;
		
		if (!m_muzzleComp)
		{
			m_muzzleComp = MuzzleComponent.Cast(m_weaponEnt.FindComponent(MuzzleComponent));
			return;
		}
		
		SCR_MuzzleEffectComponent muzzEffComp = SCR_MuzzleEffectComponent.Cast(m_weaponEnt.FindComponent(SCR_MuzzleEffectComponent));
		if (!muzzEffComp)
			return;
		
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
		
		bulletMass = muzzEffComp.GetBulletMass();
		bulletSpeed = muzzEffComp.GetBulletSpeed();
		m_fBulletInitSpeedCoef = m_muzzleComp.GetBulletInitSpeedCoef();
		bulletSpeed *= m_fBulletInitSpeedCoef;
		
		float energy = (bulletMass * bulletSpeed * bulletSpeed) / 2; // bullet energy in J

		float energyFactor = Math.InverseLerp(200, 3000, energy);
		m_fAmmoPower = energyFactor;
		
		m_fWeaponMass = SRGP_Utils.SRGP_GetWeaponWeight(m_weaponOwner);
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
		rotation[2] = Math.Min(Math.Max(m_fCurrentHorizontalImpulse * RECOIL_ROLL_POWER, -3), 3);
		
		translation[0] = m_fCurrentHorizontalImpulse * RECOIL_GUN_SIDEMOVE; // decorative side sway
		translation[2] = Math.Max(m_fCurrentVerticalImpulse * RECOIL_GUN_SHOULDERKICK * -1, RECOIL_GUN_SHOULDERKICK_LIMIT); // kick
		
		turnOffset[0] = m_fCurrentHorizontalImpulse * RECOIL_AIMKICK_HORIZONTAL;
		turnOffset[1] = m_fCurrentVerticalImpulse * RECOIL_AIMKICK_VERTICAL;
		
		m_fTotalHorizontalImpulse = 0;
		m_fTotalVerticalImpulse = 0;
	}
}