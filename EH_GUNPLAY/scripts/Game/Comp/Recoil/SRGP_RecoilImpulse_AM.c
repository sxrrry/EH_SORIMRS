class SRGP_RecoilImpulse_AM : ScriptedWeaponAimModifier
{
	const float MAXIMAL_VERTICAL_DEGREES = 100;
	const float MAXIMAL_VERTICAL_IMPULSE = 100;
	const float MAXIMAL_HORIZONTAL_DEGREES = 60;
	const float MAXIMAL_HORIZONTAL_IMPULSE = 100;
	const float RECOIL_SPEED_MULT = 14;
	
	float m_fTotalVerticalImpulse;
	float m_fCurrentVerticalImpulse;
	float m_fMaxVerticalImpulse;
	float m_fTotalHorizontalImpulse;
	float m_fCurrentHorizontalImpulse;
	float m_fMaxHorizontalImpulse;
	
	private float m_fVerticalVelocity = 0.8;
	private float m_fHorizontalVelocity = 0.9;
	
	float m_fWeaponMass;
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
			m_HandsStamCharComp = SRGP_HandsStaminaCharacterComponent.Cast(m_weaponOwner.FindComponent(SRGP_HandsStaminaCharacterComponent));
		
		if(!m_muzzleComp)
			return;
		
		SCR_MuzzleEffectComponent muzzEffComp = SCR_MuzzleEffectComponent.Cast(m_weaponEnt.FindComponent(SCR_MuzzleEffectComponent));
		if (!muzzEffComp)
			return;
		bulletMass = muzzEffComp.GetBulletMass();
		bulletSpeed = muzzEffComp.GetBulletSpeed();
		float energy = (bulletMass * bulletSpeed * bulletSpeed) / 2; // bullet energy in J

		float energyFactor = Math.InverseLerp(200, 3000, energy);
		m_fAmmoPower = energyFactor;
		
		m_fWeaponMass = m_HandsStamCharComp.SRGP_GetWeaponWeight(m_weaponOwner);
		float m_fWeaponMassFactor = Math.InverseLerp(20, 0, m_fWeaponMass);
		
		m_fTotalVerticalImpulse = Math.Lerp(MAXIMAL_VERTICAL_IMPULSE, 0, m_fWeaponMassFactor) * energyFactor;
		m_fTotalHorizontalImpulse = Math.Lerp(MAXIMAL_HORIZONTAL_IMPULSE, 0, m_fWeaponMassFactor) * energyFactor;
		
		PrintFormat("%1|%2|%3", m_fWeaponMassFactor, energyFactor, m_fTotalVerticalImpulse);
	}
	
	override void OnCalculate(IEntity owner, WeaponAimModifierContext context, float timeSlice, out vector translation, out vector rotation, out vector turnOffset)
	{
		
		m_fTotalVerticalImpulse = Math.Clamp(m_fTotalVerticalImpulse, 0, MAXIMAL_VERTICAL_DEGREES);
		m_fTotalHorizontalImpulse = Math.Clamp(m_fTotalHorizontalImpulse, 0, MAXIMAL_HORIZONTAL_DEGREES)* Math.RandomFloat(-1, 1);
		
		m_fCurrentVerticalImpulse = Math.SmoothSpring(m_fCurrentVerticalImpulse, m_fTotalVerticalImpulse, m_fVerticalVelocity, 0.9, 0.5, timeSlice * RECOIL_SPEED_MULT);
		m_fCurrentHorizontalImpulse = Math.SmoothSpring(m_fCurrentHorizontalImpulse, m_fTotalHorizontalImpulse, m_fHorizontalVelocity, 0.9, 0.5, timeSlice * RECOIL_SPEED_MULT);
		
		rotation[1] = m_fCurrentVerticalImpulse;
		rotation[0] = m_fCurrentHorizontalImpulse;
		m_fTotalHorizontalImpulse = 0;
		m_fTotalVerticalImpulse = 0;
	}
}