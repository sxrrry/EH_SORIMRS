class SRGP_WeaponDisplacement_AM : ScriptedWeaponAimModifier
{
    IEntity m_weaponOwner;
    SRGP_WeaponDisplacementComponent m_SettingsComp;

    float SPRING_VELOCITY = 0;
    float m_fCurrentMult;
    float m_fTargetMult;

    override protected void OnActivated(IEntity weaponOwner)
    {
        m_weaponOwner = weaponOwner;
        m_SettingsComp = SRGP_WeaponDisplacementComponent.Cast(
            weaponOwner.FindComponent(SRGP_WeaponDisplacementComponent)
        );
        PushLocalSettingsIfOwner();
    }

    // Отправляет локальные настройки игрока на сервер, если оружие принадлежит
    // персонажу, которым управляет эта машина
    protected void PushLocalSettingsIfOwner()
    {
        if (!m_SettingsComp)
            return;

        SCR_PlayerController pc = SCR_PlayerController.Cast(GetGame().GetPlayerController());
        if (!pc || pc.GetControlledEntity() != m_weaponOwner)
            return;

        BaseContainer settings = GetGame().GetGameUserSettings().GetModule("SRGP_WeaponDisplacementSettings");
        if (!settings)
            return;

        float rotX, rotY, rotZ, offX, offY, offZ;
        settings.Get("ROTATION_X", rotX);
        settings.Get("ROTATION_Y", rotY);
        settings.Get("ROTATION_Z", rotZ);
        settings.Get("OFFSET_X", offX);
        settings.Get("OFFSET_Y", offY);
        settings.Get("OFFSET_Z", offZ);

        m_SettingsComp.RpcAsk_ApplyValues(rotX, rotY, rotZ, offX, offY, offZ);
    }

    override void OnCalculate(
	    IEntity owner,
	    WeaponAimModifierContext context,
	    float timeSlice,
	    out vector translation,
	    out vector rotation,
	    out vector turnOffset
	)
	{
	    if (!m_weaponOwner)
	        return;
	
	    float rotX, rotY, rotZ, offX, offY, offZ;
	
	    SCR_PlayerController pc = SCR_PlayerController.Cast(GetGame().GetPlayerController());
	    bool isOwner = (pc && pc.GetControlledEntity() == m_weaponOwner);
	
	    if (isOwner)
	    {
	        BaseContainer s = GetGame().GetGameUserSettings().GetModule("SRGP_WeaponDisplacementSettings");
	        if (!s)
	            return;
	
	        s.Get("ROTATION_X", rotX);
	        s.Get("ROTATION_Y", rotY);
	        s.Get("ROTATION_Z", rotZ);
	        s.Get("OFFSET_X", offX);
	        s.Get("OFFSET_Y", offY);
	        s.Get("OFFSET_Z", offZ);
	    }
	    else
	    {
	        if (!m_SettingsComp)
	        {
	            m_SettingsComp = SRGP_WeaponDisplacementComponent.Cast(
	                m_weaponOwner.FindComponent(SRGP_WeaponDisplacementComponent)
	            );
	            if (!m_SettingsComp)
	                return;
	        }
	
	        rotX = m_SettingsComp.ROTATION_X;
	        rotY = m_SettingsComp.ROTATION_Y;
	        rotZ = m_SettingsComp.ROTATION_Z;
	        offX = m_SettingsComp.OFFSET_X;
	        offY = m_SettingsComp.OFFSET_Y;
	        offZ = m_SettingsComp.OFFSET_Z;
	    }
	
	    if (SRGP_Utils.SRGP_IsInADS(m_weaponOwner))
	        m_fTargetMult = 0;
	    else
	        m_fTargetMult = 1;
	
	    m_fCurrentMult = Math.SmoothSpring(
	        m_fCurrentMult, m_fTargetMult,
	        SPRING_VELOCITY, 0.1, 0.5, timeSlice * 35
	    );
	
	    translation[0] = offX * m_fCurrentMult;
	    translation[1] = offY * m_fCurrentMult;
	    translation[2] = offZ * m_fCurrentMult;
	
	    rotation[0] = rotX * m_fCurrentMult;
	    rotation[1] = rotY * m_fCurrentMult;
	    rotation[2] = rotZ * m_fCurrentMult;
	}
}