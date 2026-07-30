/*class SRGP_Shake_AM : ScriptedWeaponAimModifier
{
	IEntity m_weaponEnt
	IEntity m_weaponOwner;
	MuzzleComponent m_muzzleComp;
	SRGP_HandsStaminaCharacterComponent m_HandsStamCharComp;
	bool m_bWasADSPrev;
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

	}
}