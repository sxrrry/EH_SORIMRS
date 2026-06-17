class SRGP_HandsStaminaCharacterComponentClass : ScriptComponentClass
{
}

class SRGP_HandsStaminaCharacterComponent : ScriptComponent
{
	protected IEntity m_Owner;
	
	protected float m_fHandsStamina;
	
	[Attribute("0.1", uiwidget: UIWidgets.Auto, desc: "Drain rate on fixed frame", params: "0 1")]
	protected float m_fHSDrain;
	
	[Attribute("0.1", uiwidget: UIWidgets.Auto, desc: "Regen rate on fixed frame", params: "0 1")]
	protected float m_fHSRegen;

	override void OnPostInit(IEntity owner)
	{
		m_Owner = owner;
		m_fHandsStamina = 100;
		SetEventMask(owner, EntityEvent.FIXEDFRAME);
		
	}
	
	override void EOnFixedFrame(IEntity owner, float timeSlice)
    {
		if (SRGP_IsInADS(owner) && m_fHandsStamina > 0 && SRGP_GetStance(owner) != 2 && SRGP_IsWeaponDeployed(owner) == 0)
		{
			m_fHandsStamina = DrainTick(owner, m_fHandsStamina);
		}
		else if ((!SRGP_IsInADS(owner) && m_fHandsStamina < 100) || SRGP_GetStance(owner) == 2 || SRGP_IsWeaponDeployed(owner) >= 1)
			m_fHandsStamina = RegenTick(owner, m_fHandsStamina);
	}
	
	float GetStamina()
	{
		return m_fHandsStamina;
	}
	
	protected float RegenTick(IEntity owner, float stamina)
	{
		if (SRGP_GetStance(owner) == 0)
			stamina += m_fHSRegen;
		else if (SRGP_GetStance(owner) == 1)
			stamina += m_fHSRegen * 1.5;
		else if (SRGP_GetStance(owner) == 2)
			stamina += m_fHSRegen * 2;
		stamina = Math.Round(stamina * Math.Pow(10, 3)) / Math.Pow(10, 3);
		if (stamina > 100)
			stamina = 100;
		return stamina;
	}
	protected float DrainTick(IEntity owner, float stamina)
	{
		if (SRGP_GetStance(owner) == 0)
			stamina -= m_fHSDrain;
		else if (SRGP_GetStance(owner) == 1)
			stamina -= m_fHSDrain * 0.5;
		stamina = Math.Round(stamina * Math.Pow(10, 3)) / Math.Pow(10, 3);
		if (stamina < 0)
		stamina = 0;
		return stamina;
	}
	
	bool SRGP_IsInADS(IEntity owner)
	{
	    CharacterControllerComponent cc = CharacterControllerComponent.Cast(owner.FindComponent(CharacterControllerComponent));
	    if (!cc)
	        return false;
   		return cc.IsWeaponADS();
	}
	
	// returns int state (0 - not deployed, 1 - on obstacle, 2 - bipod)
	protected int SRGP_IsWeaponDeployed(IEntity owner)
	{
	    CharacterControllerComponent cc = CharacterControllerComponent.Cast(owner.FindComponent(CharacterControllerComponent));
	    if (!cc)
	        return 0;
		if (cc.GetIsWeaponDeployedBipod())
			return 2;
   		else if (cc.GetIsWeaponDeployed())
			return 1;
		return 0;
	}
	
	int SRGP_GetStance(IEntity owner)
	{
		CharacterControllerComponent cc = CharacterControllerComponent.Cast(owner.FindComponent(CharacterControllerComponent));
	    if (!cc)
	        return 0;
		return cc.GetStance();
	}
	
}