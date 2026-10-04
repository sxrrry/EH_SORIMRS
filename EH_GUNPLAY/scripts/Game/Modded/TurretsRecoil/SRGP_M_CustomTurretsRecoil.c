modded class SCR_RecoilForceAimModifier
{
	const float MAXIMAL_VERTICAL_DEGREES = 20;
	const float MAXIMAL_VERTICAL_IMPULSE = 100;
	const float MAXIMAL_HORIZONTAL_DEGREES = 10;
	const float MAXIMAL_HORIZONTAL_IMPULSE = 100;

	[Attribute("0.05", uiwidget: UIWidgets.Slider, desc: "Overall recoil * this", category: "Settings", params: "0 5")]
	float RECOIL_POWER;
	
	[Attribute("0.3", uiwidget: UIWidgets.Slider, desc: "Horizontal recoil power multiplier", category: "Settings", params: "0 5")]
	float RECOIL_HOR_POWER;
	
	[Attribute("0.5", uiwidget: UIWidgets.Slider, desc: "Vertical recoil power multiplier", category: "Settings", params: "0 5")]
	float RECOIL_VERT_POWER;
	
	[Attribute("0.8", uiwidget: UIWidgets.Slider, desc: "Recoil spring", category: "Settings", params: "0 2")]
	float RECOIL_SPRING_VERTICAL;
	
	[Attribute("0.8", uiwidget: UIWidgets.Slider, desc: "Recoil spring", category: "Settings", params: "0 2")]
	float RECOIL_SPRING_HORIZONTAL;
	
	[Attribute("0.4", uiwidget: UIWidgets.Slider, desc: "Recoil damping", category: "Settings", params: "0 2")]
	float RECOIL_DAMPING_VERTICAL;
	
	[Attribute("0.4", uiwidget: UIWidgets.Slider, desc: "Recoil damping", category: "Settings", params: "0 2")]
	float RECOIL_DAMPING_HORIZONTAL;
	
	[Attribute("15", uiwidget: UIWidgets.Slider, desc: "How fast all recoil happens...?", category: "Settings", params: "0.1 50")]
	float RECOIL_SPEED_MULT;
	
	private float m_fTotalVerticalImpulse;
	private float m_fCurrentVerticalImpulse;
	private float m_fTotalHorizontalImpulse;
	private float m_fCurrentHorizontalImpulse;
	private float m_fTimeElapsedSinceFire;
	
	private float m_fPrevVerticalImpulse;
	private float m_fPrevHorizontalImpulse;
	
	private bool m_bFirstFrameAfterFire = false;
	
	private float m_fVerticalVelocity = 0.8;
	private float m_fHorizontalVelocity = 0.9;
	
	float m_fWeaponMass;
	float m_fVehicleMass;
	float m_fBulletInitSpeedCoef;
	float m_fAmmoPower;
	IEntity m_weaponEnt;
	IEntity m_weaponOwner;
	Vehicle m_vehicleEntity;
	MuzzleComponent m_muzzleComp;
	protected AimingComponent m_AimingComp;
	
	override protected void OnInit(IEntity weaponEnt)
	{
		m_weaponEnt = weaponEnt;
		m_AimingComp = AimingComponent.Cast(weaponEnt.FindComponent(AimingComponent));
		if (!m_AimingComp)
		{
			IEntity parent = weaponEnt.GetParent();
			if (!parent)
				return;

			m_AimingComp = AimingComponent.Cast(parent.FindComponent(AimingComponent));
			if (!m_AimingComp)
				return;
		}
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
		
		bulletMass = muzzEffComp.GetBulletMass();
		bulletSpeed = muzzEffComp.GetBulletSpeed();
		m_fBulletInitSpeedCoef = m_muzzleComp.GetBulletInitSpeedCoef();
		bulletSpeed *= m_fBulletInitSpeedCoef;
		
		float energy = (bulletMass * bulletSpeed * bulletSpeed) / 2;

		float energyFactor = Math.InverseLerp(200, 3000, energy);
		m_fAmmoPower = energyFactor;
		
		if (!m_weaponOwner)
			return;
		m_vehicleEntity = Vehicle.Cast(m_weaponOwner.GetParent());
		if (m_vehicleEntity)
		{
			Physics physics = m_vehicleEntity.GetPhysics();
			if (physics)
				m_fVehicleMass = physics.GetMass();
		}
		if (m_fVehicleMass == 0)
			m_fVehicleMass = 2000;
		
		float m_fVehicleMassFactor = Math.InverseLerp(15000, 0, m_fVehicleMass);
		m_fVehicleMassFactor = Math.Min(m_fVehicleMassFactor, 1);
		m_fVehicleMassFactor = Math.Max(m_fVehicleMassFactor, 0.01);
		
		m_fTotalVerticalImpulse = Math.Lerp(0, MAXIMAL_VERTICAL_IMPULSE, energyFactor) * m_fVehicleMassFactor * RECOIL_VERT_POWER * RECOIL_POWER;
		m_fTotalHorizontalImpulse = Math.Lerp(0, MAXIMAL_HORIZONTAL_IMPULSE, energyFactor) * m_fVehicleMassFactor * RECOIL_HOR_POWER * RECOIL_POWER;
		
		m_fTimeElapsedSinceFire = 0;
		
		m_fCurrentVerticalImpulse = 0;
		m_fCurrentHorizontalImpulse = 0;
		m_fPrevVerticalImpulse = 0;
		m_fPrevHorizontalImpulse = 0;
		
		m_bFirstFrameAfterFire = true;
	}
	
	override void OnCalculate(IEntity owner, WeaponAimModifierContext context, float timeSlice, out vector translation, out vector rotation, out vector turnOffset)
	{
		vector newRotation = m_AimingComp.GetAimingRotation();
		
		m_fTotalVerticalImpulse = Math.Clamp(m_fTotalVerticalImpulse, 0, MAXIMAL_VERTICAL_DEGREES);
		if (Math.RandomFloat(0, 1) < 0.5)
			m_fTotalVerticalImpulse*=-1;
		m_fTotalHorizontalImpulse = Math.Clamp(m_fTotalHorizontalImpulse, 0, MAXIMAL_HORIZONTAL_DEGREES) * Math.RandomFloat(-1, 1);
		
		m_fCurrentVerticalImpulse = Math.SmoothSpring(m_fCurrentVerticalImpulse, m_fTotalVerticalImpulse, m_fVerticalVelocity, RECOIL_SPRING_VERTICAL, RECOIL_DAMPING_VERTICAL, timeSlice * RECOIL_SPEED_MULT);
		m_fCurrentHorizontalImpulse = Math.SmoothSpring(m_fCurrentHorizontalImpulse, m_fTotalHorizontalImpulse, m_fHorizontalVelocity, RECOIL_SPRING_HORIZONTAL, RECOIL_DAMPING_HORIZONTAL, timeSlice * RECOIL_SPEED_MULT);
		
		float deltaV;
		float deltaH;
		
		if (m_bFirstFrameAfterFire)
		{
			deltaV = m_fCurrentVerticalImpulse;
			deltaH = m_fCurrentHorizontalImpulse;
			m_bFirstFrameAfterFire = false;
		}
		else
		{
			deltaV = m_fCurrentVerticalImpulse - m_fPrevVerticalImpulse;
			deltaH = m_fCurrentHorizontalImpulse - m_fPrevHorizontalImpulse;
		}
		
		newRotation[1] = newRotation[1] - deltaV * m_vRotationOffset[1];
		newRotation[0] = newRotation[0] + deltaH * m_vRotationOffset[0];
		
		m_fPrevVerticalImpulse = m_fCurrentVerticalImpulse;
		m_fPrevHorizontalImpulse = m_fCurrentHorizontalImpulse;
		
		newRotation *= Math.DEG2RAD;
		
		m_AimingComp.SetAimingRotation(newRotation);
		
		m_fTotalHorizontalImpulse = 0;
		m_fTotalVerticalImpulse = 0;
	}
}